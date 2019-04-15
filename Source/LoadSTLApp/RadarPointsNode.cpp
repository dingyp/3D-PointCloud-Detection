#include "stdafx.h"
#include "RadarPointsNode.h"

/**************************************************************************
*  @Function Name :: RadarPointsNode
*  @Description   :: RadarPointsNode类的默认构造函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
RadarPointsNode::RadarPointsNode()
{
	m_iColorMode = 2; //颜色编码
	m_DataMin = 0.0;
	m_DataMax = 10.0;
	//isFirst = TRUE;

	m_PointSize = 2.0;
	isSelectStatus = -1;//没有在选择相同通道的情况

	m_pPointClouds = NULL;

	//m_pMaxPointCloud = nullptr;
	//m_pMinPointCloud = nullptr;
	//m_pAvgPointCloud = nullptr;
	//m_pVarPointCloud = nullptr;

	//m_pErrorPointClouds = NULL;
	//m_pSysErrorPointCloud = nullptr;
}

/**************************************************************************
*  @Function Name :: ~RadarPointsNode
*  @Description   :: RadarPointsNode类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
RadarPointsNode::~RadarPointsNode()
{
	m_pPointClouds = NULL;

	//m_pMaxPointCloud = nullptr;
	//m_pMinPointCloud = nullptr;
	//m_pAvgPointCloud = nullptr;
	//m_pVarPointCloud = nullptr;

	//m_pErrorPointClouds = NULL;
	//m_pSysErrorPointCloud = nullptr;
}

bool RadarPointsNode::AttachPointClouds(CPointClouds *pPointClouds)
{
	if (NULL != pPointClouds)
	{
		m_pPointClouds = pPointClouds;
	}
	else
	{
		return false;
	}

}

/**************************************************************************
*  @Function Name :: ClearData
*  @Description   :: 清除所有显示
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::ClearData()
{
	m_pPointClouds = NULL;
}

/**************************************************************************
*  @Function Name :: Draw
*  @Description   :: 绘制点
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::Draw()
{
	if (FALSE == IsShowed())
	{
		return;
	}

	glPushMatrix();
	LoadMatrix();	// Translate, Rotate, Scale

	glGetDoublev(GL_MODELVIEW_MATRIX, m_DrawMartirx);

	DrawPoints();
	glPopMatrix();
}

/**************************************************************************
*  @Function Name :: DrawPoints
*  @Description   :: 对点帧进行绘制
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::DrawPoints()
{
	glPushAttrib(GL_LIGHTING_BIT | GL_POINT_BIT );
	glDisable(GL_LIGHTING);

	glPointSize(m_PointSize);

	glBegin(GL_POINTS);

	GLubyte R = 255, G = 0, B = 0;

	
	if (NULL != m_pPointClouds)
	{
		//判断有没有计算出平均帧，有的话显示平均帧，没有则显示所有帧
		if (m_pPointClouds->is_Calcul)
		{//显示平均帧
			int Channel = 16;
			double x, y, z, xy;
			double angle_H;
			for (int ch = 0; ch < Channel; ch++)
			{
				for (int angleIdx = 0; angleIdx < 2000; angleIdx++)
				{
					angle_H = angleIdx * 0.18 + 0.09 + 0.05 * m_pPointClouds->m_parameter[0];
					if (1999 == angleIdx)
					{
						int a = 1;
					}
					z = m_pPointClouds->m_AvgDistance[ch][angleIdx] * m_pPointClouds->AnglesV_Sin[ch] + 0.05 * m_pPointClouds->m_parameter[3];
					xy= m_pPointClouds->m_AvgDistance[ch][angleIdx] * m_pPointClouds->AnglesV_Cos[ch];
					y = xy * cos(Deg2Rad(angle_H)) + 0.05 * m_pPointClouds->m_parameter[2];
					x = xy * sin(Deg2Rad(angle_H)) + 0.05 * m_pPointClouds->m_parameter[1];
					glColor3ub(R, G, B);
					glVertex3d(x, y, z);
				}
			}
		}
		else
		{//显示所有帧
			for (int i = 0; i < m_pPointClouds->m_uiNowCloudNums; i++)
			{
				for (int j = 0; j < m_pPointClouds->m_PointCloud[i]->size(); j++)
				{
					glColor3ub(R, G, B);
					glVertex3d((*m_pPointClouds->m_PointCloud[i])[j].x, (*m_pPointClouds->m_PointCloud[i])[j].y, (*m_pPointClouds->m_PointCloud[i])[j].z);
				}
			}
		}
		
	}
	
	glEnd();

	glPopAttrib();
}

/**************************************************************************
*  @Function Name :: GetDataRange
*  @Description   :: 获取点中数值的最大最小值
*  @Author        :: zzw
*  @Parameters    :: DataMin:获取数据最小值的引用
*					 DataMax:获取数据最大值的引用
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::GetDataRange(double &DataMin, double &DataMax)
{
	DataMin = m_DataMin;
	DataMax = m_DataMax;
}

/**************************************************************************
*  @Function Name :: SetDataRange
*  @Description   :: 设置数据的最小最大值
*  @Author        :: zzw
*  @Parameters    :: DataMin:设置数据的最小值
*					 DataMax:设置数据的最大值
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::SetDataRange(double DataMin, double DataMax)
{
	m_DataMin = DataMin;
	m_DataMax = DataMax;
}

/**************************************************************************
*  @Function Name :: GetDefaultBoundBox
*  @Description   :: 获取当前点帧的的BoundBox方面的相关属性
*  @Author        :: zzw
*  @Parameters    :: GridLength:网格的边长
*					 GridRadius:同心圆的半径
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::GetDefaultBoundBox(double &xMin, double &xMax, double &yMin, double &yMax, double &zMin, double &zMax)
{
	RadarPoint CurPoint;
	if (NULL != m_pPointClouds)
	{
		//暂时先只弄前几个
		for (int i = 0; i < (m_pPointClouds->m_uiNowCloudNums>3 ? 3 : m_pPointClouds->m_uiNowCloudNums); i++)
		{
			int Num = m_pPointClouds->m_PointCloud[i]->size();
			if (0 == i)
			{
				if (Num > 0)
				{
					xMin = xMax = (*m_pPointClouds->m_PointCloud[i])[0].x;
					yMin = yMax = (*m_pPointClouds->m_PointCloud[i])[0].y;
					zMin = zMax = (*m_pPointClouds->m_PointCloud[i])[0].z;
				}
			}
			for (int j = 0; j < Num; j++)
			{
				glVertex3d((*m_pPointClouds->m_PointCloud[i])[j].x, (*m_pPointClouds->m_PointCloud[i])[j].y, (*m_pPointClouds->m_PointCloud[i])[j].z);
				if (xMin > (*m_pPointClouds->m_PointCloud[i])[j].x)
				{
					xMin = (*m_pPointClouds->m_PointCloud[i])[j].x;
				}
				if (xMax < (*m_pPointClouds->m_PointCloud[i])[j].x)
				{
					xMax = (*m_pPointClouds->m_PointCloud[i])[j].x;
				}

				if (yMin >(*m_pPointClouds->m_PointCloud[i])[j].y)
				{
					yMin = (*m_pPointClouds->m_PointCloud[i])[j].y;
				}
				if (yMax < (*m_pPointClouds->m_PointCloud[i])[j].y)
				{
					yMax = (*m_pPointClouds->m_PointCloud[i])[j].y;
				}
				
				if (zMin >(*m_pPointClouds->m_PointCloud[i])[j].z)
				{
					zMin = (*m_pPointClouds->m_PointCloud[i])[j].z;
				}
				if (zMax < (*m_pPointClouds->m_PointCloud[i])[j].z)
				{
					zMax = (*m_pPointClouds->m_PointCloud[i])[j].z;
				}
			}
		}
	}

}

//进行结构优化，比如下一个点的赋值
void RadarPointsNode::StructuralOptimization()
{
	//m_pPointClouds->StructuralOptimization();
}

//计算，并最终显示一帧点云数据
void RadarPointsNode::BeginCalculation()
{
	StructuralOptimization();//先进行结构优化，方便之后的计算

}