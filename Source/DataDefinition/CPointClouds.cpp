#include "stdafx.h"
#include "CPointClouds.h"

using namespace DataDefinition;

CPointClouds::CPointClouds()
{
	is_Calcul = false;
	m_PointCloud = NULL;
	m_uiCloudFrameNums = 0;
	m_uiNowCloudNums = 0;
	UnSetProtocol();

	m_pErrorPointClouds = NULL;
	m_pSysErrorPointCloud = nullptr;
	memset(m_ISDistance, 0, sizeof(double) * 16 * 2000);
	memset(m_MaxDistance, -1, sizeof(double) * 16 * 2000);
	memset(m_MinDistance, DBL_MAX, sizeof(double) * 16 * 2000);
	memset(m_AvgDistance, 0, sizeof(double) * 16 * 2000);
	memset(m_PointNumber, 0, sizeof(unsigned int) * 16 * 2000);
	memset(m_VarDistance, 0, sizeof(double) * 16 * 2000);
	memset(m_parameter, 0, sizeof(double) * 4);
	//��׼����ͨ������
	memset(m_choosechannel, 0, sizeof(int) * 16);
	m_choosechannel_number = 0;
}

CPointClouds::~CPointClouds()
{
	is_Calcul = false;
	if (0 != m_uiNowCloudNums)//�������֡����Ŀ��Ϊ0�����ͷ�
	{
		//�������֡����ĵ�������
		for (int i = 0; i < m_uiNowCloudNums; i++)
		{
			m_PointCloud[i] = NULL;
		}
		delete[] m_PointCloud;
	}
	m_PointCloud = NULL;

	m_uiCloudFrameNums = 0;
	m_uiNowCloudNums = 0;
	UnSetProtocol();
	m_pErrorPointClouds = NULL;
	m_pSysErrorPointCloud = nullptr;
	memset(m_ISDistance, 0, sizeof(double) * 16 * 2000);
	memset(m_MaxDistance, -1, sizeof(double) * 16 * 2000);
	memset(m_MinDistance, DBL_MAX, sizeof(double) * 16 * 2000);
	memset(m_AvgDistance, 0, sizeof(double) * 16 * 2000);
	memset(m_PointNumber, 0, sizeof(unsigned int) * 16 * 2000);
	memset(m_VarDistance, 0, sizeof(double) * 16 * 2000);
	memset(m_parameter, 0, sizeof(double) * 4);

	memset(m_choosechannel, 0, sizeof(int) * 16);
	m_choosechannel_number = 0;
}

//����ִ���ƣ������ܵĵ�����Ŀ,��������
void CPointClouds::Clear()
{
	is_Calcul = false;
	if (0 != m_uiNowCloudNums)//�������֡����Ŀ��Ϊ0�����ͷ�
	{
		//�������֡����ĵ�������
		for (int i = 0; i < m_uiNowCloudNums; i++)
		{
			m_PointCloud[i] = NULL;
		}
		delete[] m_PointCloud;
	}
	m_PointCloud = NULL;

	m_uiCloudFrameNums = 0;
	m_uiNowCloudNums = 0;
	UnSetProtocol();
	m_pErrorPointClouds = NULL;
	m_pSysErrorPointCloud = nullptr;
	memset(m_ISDistance, 0, sizeof(double) * 16 * 2000);
	memset(m_MaxDistance, -1, sizeof(double) * 16 * 2000);
	memset(m_MinDistance, DBL_MAX, sizeof(double) * 16 * 2000);
	memset(m_AvgDistance, 0, sizeof(double) * 16 * 2000);
	memset(m_PointNumber, 0, sizeof(unsigned int) * 16 * 2000);
	memset(m_VarDistance, 0, sizeof(double) * 16 * 2000);
	memset(m_parameter, 0, sizeof(double) * 4);

	memset(m_choosechannel, 0, sizeof(int) * 16);
	m_choosechannel_number = 0;
}


//���õ��Ƶ����ԣ�һ�����ڵ��ƹر����ݣ��������¶�ȡ����֮ǰ
void CPointClouds::ResetAttr()
{
	memset(m_ISDistance, 0, sizeof(double) * 16 * 2000);
	memset(m_MaxDistance, -1, sizeof(double) * 16 * 2000);
	memset(m_MinDistance, DBL_MAX, sizeof(double) * 16 * 2000);
	memset(m_AvgDistance, 0, sizeof(double) * 16 * 2000);
	memset(m_PointNumber, 0, sizeof(unsigned int) * 16 * 2000);
	memset(m_VarDistance, 0, sizeof(double) * 16 * 2000);
	memset(m_parameter, 0, sizeof(double) * 4);
	memset(m_Arg_VarDistance, 0, sizeof(double) * 16);
	m_VarDistanceMaxChannel = m_VarDistanceMinChannel = -1;
}

//����Э��
void CPointClouds::SetProtocol(LiDAR_Protocol Protocol)
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
	}
	LoadAnglesV(m_Protocol);
}

//ȡ������Э��
void CPointClouds::UnSetProtocol()
{
	m_Protocol = LIDAR_INVALID;
	ClearAnglesV();
	LiDAR_Channels = 0;
}

/**************************************************************************
*  @Function Name :: LoadAnglesV
*  @Description   :: ���ݲ�Ʒ�ͺż��ش�ֱ�Ƕ����ݣ������Ƕȣ����ȣ�Cos��Sinֵ
*  @Author        :: zzw
*  @Parameters    :: unsigned int Protocol:Э������
*
*  @Return        :: true:���سɹ���false������ʧ��
**************************************************************************/
bool CPointClouds::LoadAnglesV(LiDAR_Protocol uProtocol)
{
	if (LIDAR_INVALID == uProtocol)
	{
		return false;
	}
	ClearAnglesV();
	if (LIDAR_HLP16 == uProtocol)
	{
		for (UINT i = 0; i < LiDAR_Channels; i++)
		{
			if (i & 0x01)//����
				AnglesV_Deg.push_back(i);
			else
				AnglesV_Deg.push_back((int)i - 15);
		}
	}
	else if (LIDAR_2D == uProtocol)
	{
		AnglesV_Deg.push_back(0);
	}

	for (int i = 0; i < AnglesV_Deg.size(); i++)
	{
		AnglesV_Rad.push_back(Deg2Rad(double(AnglesV_Deg[i])));
		AnglesV_Cos.push_back(cos(AnglesV_Rad[i]));
		AnglesV_Sin.push_back(sin(AnglesV_Rad[i]));
	}
	return true;
}

/**************************************************************************
*  @Function Name :: ClearAnglesV
*  @Description   :: �����ֱ�Ƕ�����
*  @Author        :: hgb
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CPointClouds::ClearAnglesV()
{
	AnglesV_Deg.clear();
	AnglesV_Rad.clear();
	AnglesV_Cos.clear();
	AnglesV_Sin.clear();
}

//���һ֡�������ݣ������Ƿ���ӳɹ�
bool CPointClouds::AttachPointCloud(pRadarPointList pPointCloud)
{
	if (m_uiNowCloudNums > m_uiCloudFrameNums)
	{
		return false;//û�п�����ӵĵط������ʧ��
	}
	is_Calcul = false;
	memset(m_parameter, 0, sizeof(double) * 4);
	m_PointCloud[m_uiNowCloudNums] = pPointCloud;
	m_uiNowCloudNums++;
	return true;
}

//���õ�����������ֵһ��ʼ����,�����Ƿ���óɹ�
bool CPointClouds::SetPointCloudNums(UINT pointCloudNums)
{
	if (0 == m_uiCloudFrameNums)
	{
		m_uiCloudFrameNums = pointCloudNums;
		m_PointCloud = new pRadarPointList[m_uiCloudFrameNums];
		return true;
	}
	return false;
}

//�õ�ƽ��ֵ��boundingBox
bool CPointClouds::GetArgPointCloud_BoundBox(float& xMin, float& xMax, float& yMin, float& yMax, float& zMin, float& zMax)
{
	if (false == is_Calcul)
	{
		return false;
	}
	double angle_H;
	xMax = yMax = zMax = FLT_MIN;
	xMin = yMin = zMin = FLT_MAX;
	float tmpx, tmpy, tmpz;
	for (int channelIdx = 0; channelIdx<16; channelIdx++)
	{
		for (int angleIdx = 0; angleIdx < 2000; angleIdx++)
		{
			angle_H = angleIdx * 0.18 + 0.09 + 0.05 * m_parameter[0];
			tmpx = m_AvgDistance[channelIdx][angleIdx] * AnglesV_Cos[channelIdx] * sin(Deg2Rad(angle_H)) + 0.05 * m_parameter[1];
			tmpy = m_AvgDistance[channelIdx][angleIdx] * AnglesV_Cos[channelIdx] * cos(Deg2Rad(angle_H)) + 0.05 * m_parameter[2];
			tmpz = m_AvgDistance[channelIdx][angleIdx] * AnglesV_Sin[channelIdx] + 0.05 * m_parameter[3];

			//x
			if (xMin > tmpx)
			{
				xMin = tmpx;//xmin
			}
			if (xMax < tmpx)
			{
				xMax = tmpx;//xmax
			}
			//y
			if (yMin > tmpy)
			{
				yMin = tmpy;//ymin
			}
			if (yMax < tmpy)
			{
				yMax = tmpy;//ymax
			}
			//z
			if (zMin > tmpz)
			{
				zMin = tmpz;//zmin
			}
			if (zMax < tmpz)
			{
				zMax = tmpz;//zmax
			}
		}
	}

	return true;
}

//�õ�һ֡���Ƶ�boundingBox
void CPointClouds::GetOnePointCloud_BoundBox(pRadarPointList pPointCloud, float& xMin, float& xMax, float& yMin, float& yMax, float& zMin, float& zMax)
{
	if (NULL == pPointCloud)
	{
		return;
	}
	int Total_Num = pPointCloud->size();
	RadarPoint CurPoint;
	xMax = yMax = zMax = FLT_MIN;
	xMin = yMin = zMin = FLT_MAX;
	for (int i = 0; i < Total_Num; i++)
	{
		CurPoint = (*pPointCloud)[i];
		//x
		if (xMin > CurPoint.x)
		{
			xMin = CurPoint.x;//xmin
		}
		if (xMax < CurPoint.x)
		{
			xMax = CurPoint.x;//xmax
		}
		//y
		if (yMin > CurPoint.y)
		{
			yMin = CurPoint.y;//ymin
		}
		if (yMax < CurPoint.y)
		{
			yMax = CurPoint.y;//ymax
		}
		//z
		if (zMin > CurPoint.z)
		{
			zMin = CurPoint.z;//zmin
		}
		if (zMax < CurPoint.z)
		{
			zMax = CurPoint.z;//zmax
		}
	}
}

//����parameter����
void CPointClouds::SetParameter(double *parameter)
{
	for (int i = 0; i < 4; i++)
	{
		m_parameter[i] = parameter[i];
	}
}

//������׼����ͨ��
void CPointClouds::Setchoosechannel(int *set_arry, int number)
{
	if (number < 0)
	{//����ʧ��
		return;
	}
	m_choosechannel_number = number > 16 ? 16 : number;//ͨ�������ڻ�ֻ��16������
	for (int i = 0; i < m_choosechannel_number; i++)
	{
		m_choosechannel[i] = set_arry[i];
	}
}

//�õ�ȫ�ֵ��Ƶ�boundingBox
void CPointClouds::GetPointCloudsBoundBox(float& xMin, float& xMax, float& yMin, float& yMax, float& zMin, float& zMax)
{
	if (NULL == m_PointCloud || 0 == m_uiNowCloudNums)
	{
		return;
	}
	xMax = yMax = zMax = FLT_MIN;
	xMin = yMin = zMin = FLT_MAX;
	for (int i = 0; i < m_uiNowCloudNums; i++)
	{
		float tmp_xMin, tmp_xMax, tmp_yMin, tmp_yMax, tmp_zMin, tmp_zMax;
		GetOnePointCloud_BoundBox(m_PointCloud[i], tmp_xMin, tmp_xMax, tmp_yMin, tmp_yMax, tmp_zMin, tmp_zMax);
		if (tmp_xMin < xMin)
		{
			xMin = tmp_xMin;
		}
		if (tmp_xMax > xMax)
		{
			xMax = tmp_xMax;
		}
		if (tmp_yMin < yMin)
		{
			yMin = tmp_yMin;
		}
		if (tmp_yMax > yMax)
		{
			yMax = tmp_yMax;
		}
		if (tmp_zMin < zMin)
		{
			zMin = tmp_zMin;
		}
		if (tmp_zMax > zMax)
		{
			zMax = tmp_zMax;
		}
	}
}
