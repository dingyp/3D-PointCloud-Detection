#include "stdafx.h"
#include "RadarPointsNode.h"

/**************************************************************************
*  @Function Name :: RadarPointsNode
*  @Description   :: RadarPointsNode���Ĭ�Ϲ��캯��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
RadarPointsNode::RadarPointsNode()
{
	m_iColorMode = 2; //��ɫ����
	m_DataMin = 0.0;
	m_DataMax = 10.0;
	//isFirst = TRUE;

	m_PointSize = 2.0;
	isSelectStatus = -1;//û����ѡ����ͬͨ�������

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
*  @Description   :: RadarPointsNode�����������
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
*  @Description   :: ���������ʾ
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
*  @Description   :: ���Ƶ�
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
*  @Description   :: �Ե�֡���л���
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
		//�ж���û�м����ƽ��֡���еĻ���ʾƽ��֡��û������ʾ����֡
		if (m_pPointClouds->is_Calcul)
		{//��ʾƽ��֡
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
		{//��ʾ����֡
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
*  @Description   :: ��ȡ������ֵ�������Сֵ
*  @Author        :: zzw
*  @Parameters    :: DataMin:��ȡ������Сֵ������
*					 DataMax:��ȡ�������ֵ������
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::GetDataRange(double &DataMin, double &DataMax)
{
	DataMin = m_DataMin;
	DataMax = m_DataMax;
}

/**************************************************************************
*  @Function Name :: SetDataRange
*  @Description   :: �������ݵ���С���ֵ
*  @Author        :: zzw
*  @Parameters    :: DataMin:�������ݵ���Сֵ
*					 DataMax:�������ݵ����ֵ
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::SetDataRange(double DataMin, double DataMax)
{
	m_DataMin = DataMin;
	m_DataMax = DataMax;
}

/**************************************************************************
*  @Function Name :: GetDefaultBoundBox
*  @Description   :: ��ȡ��ǰ��֡�ĵ�BoundBox������������
*  @Author        :: zzw
*  @Parameters    :: GridLength:����ı߳�
*					 GridRadius:ͬ��Բ�İ뾶
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::GetDefaultBoundBox(double &xMin, double &xMax, double &yMin, double &yMax, double &zMin, double &zMax)
{
	RadarPoint CurPoint;
	if (NULL != m_pPointClouds)
	{
		//��ʱ��ֻŪǰ����
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

//���нṹ�Ż���������һ����ĸ�ֵ
void RadarPointsNode::StructuralOptimization()
{
	//m_pPointClouds->StructuralOptimization();
}

//���㣬��������ʾһ֡��������
void RadarPointsNode::BeginCalculation()
{
	StructuralOptimization();//�Ƚ��нṹ�Ż�������֮��ļ���

}