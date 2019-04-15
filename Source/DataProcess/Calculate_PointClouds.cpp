#include "stdafx.h"
#include "Calculate_PointClouds.h"
#include <math.h>

using namespace DataProcess;

Calcul_PointClouds::Calcul_PointClouds()
{
	m_pPointClouds = NULL;
	m_Protocol = LIDAR_INVALID;
	LiDAR_Channels = 0;
}

Calcul_PointClouds::~Calcul_PointClouds()
{
	m_pPointClouds = NULL;
	m_Protocol = LIDAR_INVALID;
	LiDAR_Channels = 0;
}

//点云协议初始化
void Calcul_PointClouds::Init(LiDAR_Protocol Protocol)
{
	m_Protocol = Protocol;
	switch (m_Protocol)
	{
	case LIDAR_HLP16:
		LiDAR_Channels = 16;
		break;
	case LIDAR_2D:
		LiDAR_Channels = 1;
		break;
	case LIDAR_INVALID:
		LiDAR_Channels = 0;
		break;
	}
	if (NULL != m_pPointClouds)
	{//给点云设置协议，方便之后进行读取和显示
		m_pPointClouds->SetProtocol(m_Protocol);
	}
}

//得到多帧点云
void Calcul_PointClouds::GetPointClouds(CPointClouds * pPointClouds)
{
	if (NULL == pPointClouds)
	{
		return;
	}
	m_pPointClouds = pPointClouds;
	m_pPointClouds->SetProtocol(m_Protocol);
}

//计算点云帧的属性
void Calcul_PointClouds::Compute_attribution()
{
	if (NULL == m_pPointClouds)
	{
		return;
	}
	//当协议为空返回
	switch (m_Protocol)
	{
	case LIDAR_HLP16:
		Compute_HLP16_attribution();
		break;
	case LIDAR_2D:
		Compute_2D_attribution();
		break;
	case LIDAR_INVALID:
		return;
	}
}

//计算误差评价指标
void Calcul_PointClouds::Compute_Evaluation(double *result, int arrayNumber)
{
	if (NULL == m_pPointClouds)
	{
		return;
	}
	//当协议为空返回
	switch (m_Protocol)
	{
	case LIDAR_HLP16:
		Compute_HLP16_Evaluation(result, arrayNumber);
		break;
	case LIDAR_2D:
		Compute_2D_Evaluation(result, arrayNumber);
		break;
	case LIDAR_INVALID:
		return;
	}
}

//根据所选通道计算这些通道的误差评价指标，需要实现设置好有哪些通道
void Calcul_PointClouds::Compute_HLP16_Evaluation(double *result,int arrayNumber)
{
	//计算误差得把一些误差去掉
	if (arrayNumber < m_pPointClouds->m_choosechannel_number + 1)
	{
		return;
	}
	for (int i = 0; i < m_pPointClouds->m_choosechannel_number + 1; i++)
	{
		result[i] = 0.0;
	}
	int Total_number;
	for (int channelnumber = 0; channelnumber < m_pPointClouds->m_choosechannel_number; channelnumber++)
	{
		Total_number = 0;
		int channelIdx = m_pPointClouds->m_choosechannel[channelnumber];
		for (int angleIdx = 0; angleIdx < 2000; angleIdx++)
		{
			//选取可信的点进行计算
			double tmp;
			if (fabs(m_pPointClouds->m_AvgDistance[channelIdx][angleIdx]) > 1e-7&&fabs(m_pPointClouds->m_ISDistance[channelIdx][angleIdx]) > 1e-7)
			{
				tmp= fabs(m_pPointClouds->m_AvgDistance[channelIdx][angleIdx]- m_pPointClouds->m_ISDistance[channelIdx][angleIdx])
					/ m_pPointClouds->m_AvgDistance[channelIdx][angleIdx];
				Total_number++;
				result[channelnumber] += tmp;
			}
		}
		result[channelnumber] /= Total_number;
		result[m_pPointClouds->m_choosechannel_number] += result[channelnumber];
	}
	result[m_pPointClouds->m_choosechannel_number] /= m_pPointClouds->m_choosechannel_number;
}

//计算2D点云帧误差评价指标
void Calcul_PointClouds::Compute_2D_Evaluation(double *result, int arrayNumber)
{

}

//计算16通道点云帧的属性，包括最大最小，平均，方差，最大最小方差
void Calcul_PointClouds::Compute_HLP16_attribution()
{
	//求点云的一些属性之前需要将点云的属性重置
	m_pPointClouds->ResetAttr();

	//求最大最小值
	for (int i = 0; i < m_pPointClouds->m_uiNowCloudNums; i++)
	{
		pRadarPointList tmp_pPointCloud = m_pPointClouds->m_PointCloud[i];
		int PointNum = tmp_pPointCloud->size();
		UINT channel = 0;
		UINT angleIdx = 0;
		double now_distance;
		for (int j = 0; j < PointNum; j++)
		{
			//这个通道并不是按照从小到大的顺序依次来的,多以所有点云里面并不是按照从下往上的顺序进行存储的
			channel = UINT((*tmp_pPointCloud)[j].channel);

			angleIdx = UINT((*tmp_pPointCloud)[j].angleH / 18.0);
			if (angleIdx >= 2000)
			{
				angleIdx = 1999;
			}
			now_distance = (*tmp_pPointCloud)[j].distance / 1000.0;
			m_pPointClouds->m_AvgDistance[channel][angleIdx] += now_distance;
			m_pPointClouds->m_PointNumber[channel][angleIdx]++;
			if (m_pPointClouds->m_MaxDistance[channel][angleIdx] < now_distance)
			{
				m_pPointClouds->m_MaxDistance[channel][angleIdx] = now_distance;
			}
			if (m_pPointClouds->m_MinDistance[channel][angleIdx] > now_distance)
			{
				m_pPointClouds->m_MinDistance[channel][angleIdx] = now_distance;
			}
		}
	}
	////求平均值
	for (int i = 0; i < LiDAR_Channels; i++)
	{
		for (int j = 0; j < 2000; j++)
		{
			m_pPointClouds->m_AvgDistance[i][j] /= m_pPointClouds->m_PointNumber[i][j];
		}
	}
	//求方差
	for (int i = 0; i < m_pPointClouds->m_uiNowCloudNums; i++)
	{
		pRadarPointList tmp_pPointCloud = m_pPointClouds->m_PointCloud[i];
		int PointNum = tmp_pPointCloud->size();
		UINT channel = 0;
		UINT angleIdx = 0;
		for (int j = 0; j < PointNum; j++)
		{
			channel = UINT((*tmp_pPointCloud)[j].channel);
			angleIdx = UINT((*tmp_pPointCloud)[j].angleH / 20);
			if (angleIdx >= 2000)
			{
				angleIdx = 1999;
			}
			m_pPointClouds->m_VarDistance[channel][angleIdx] += pow(double((*tmp_pPointCloud)[j].distance/1000.0 - m_pPointClouds->m_AvgDistance[channel][angleIdx]), 2)/ m_pPointClouds->m_PointNumber[channel][angleIdx];
		}
	}

	int TotalNumber;
	for (int i = 0; i < LiDAR_Channels; i++)
	{
		m_pPointClouds->m_Arg_VarDistance[i] = 0;
		TotalNumber = 0;
		for (int j = 0; j < 2000; j++)
		{
			if (0 != m_pPointClouds->m_VarDistance[i][j])
			{
				if ((!isnan(m_pPointClouds->m_VarDistance[i][j]))&&(!isinf(m_pPointClouds->m_VarDistance[i][j])))
				{
					m_pPointClouds->m_Arg_VarDistance[i] += m_pPointClouds->m_VarDistance[i][j];
					TotalNumber++;
				}
				
			}
			//if (m_pPointClouds->m_VarDistanceMax < m_pPointClouds->m_VarDistance[i][j])
			//{
			//	m_pPointClouds->m_VarDistanceMax = m_pPointClouds->m_VarDistance[i][j];
			//}
			//if (m_pPointClouds->m_VarDistanceMin > m_pPointClouds->m_VarDistance[i][j])
			//{
			//	if (0 != m_pPointClouds->m_VarDistance[i][j])//防止是之前的0
			//	{
			//		m_pPointClouds->m_VarDistanceMin = m_pPointClouds->m_VarDistance[i][j];
			//	}
			//}
		}
		
		m_pPointClouds->m_Arg_VarDistance[i] /= TotalNumber;
	}

	for (int i = 0; i < LiDAR_Channels; i++)
	{
		if (0 == i)
		{
			m_pPointClouds->m_VarDistanceMaxChannel = m_pPointClouds->m_VarDistanceMinChannel = 0;
		}
		if (m_pPointClouds->m_Arg_VarDistance[m_pPointClouds->m_VarDistanceMinChannel] > m_pPointClouds->m_Arg_VarDistance[i])
		{
			m_pPointClouds->m_VarDistanceMinChannel = i;
		}
		if (m_pPointClouds->m_Arg_VarDistance[m_pPointClouds->m_VarDistanceMaxChannel] < m_pPointClouds->m_Arg_VarDistance[i])
		{
			m_pPointClouds->m_VarDistanceMaxChannel = i;
		}
	}

	//将计算位置true
	m_pPointClouds->is_Calcul = true;
}

//计算16通道点云帧的属性
void Calcul_PointClouds::Compute_2D_attribution()
{

}