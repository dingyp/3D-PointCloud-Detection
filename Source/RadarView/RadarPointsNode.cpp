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
	ColorCoding = CODING_INTEN;
	isSelectStatus = -1;//û����ѡ����ͬͨ�������
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
}

/**************************************************************************
*  @Function Name :: AttachPointsData
*  @Description   :: ����ӽ���
*  @Author        :: zzw
*  @Parameters    :: DataList:�洢��֡�Ķ���
*
*  @Return        :: �����Ƿ���ӳɹ�
**************************************************************************/
bool RadarPointsNode::AttachPointsData(RadarPointList &DataList)
{
	if (DataList.empty() == false)
	{
		m_PointsList = DataList;
		return true;
	}
	else
	{
		return false;
	}
}

/**************************************************************************
*  @Function Name :: GetPointListData
*  @Description   :: ��ȡ��ʾ�ĵ�֡
*  @Author        :: zzw
*  @Parameters    :: PointList:��ȡ��ʾ��֡������
*
*  @Return        :: �����Ƿ��ȡ�ɹ�
**************************************************************************/
bool RadarPointsNode::GetPointListData(RadarPointList &PointList)
{
	if (m_PointsList.empty()==false)
	{
		PointList = m_PointsList;
		return true;
	}
	else
	{
		return false;
	}
}

/**************************************************************************
*  @Function Name :: RefreshPointData
*  @Description   :: �任��ʾ֡������
*  @Author        :: zzw
*  @Parameters    :: addX:�任��X�����λ�� addY:�任��Y�����λ�� addZ:�任��Z�����λ��
*					 pitch:X�����ת�� roll:Y�����ת�� yaw:Z�����ת�� transformer:�Ե�֡���б任����
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::RefreshPointData(double addX, double addY, double addZ, double pitch, double roll, double yaw, DataProcess::Transform &transformer)
{
	transformer.ChangeOne(addX, addY, addZ, pitch, roll, yaw, m_PointsList);//��Ҫ��֧��һ���Եı任
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
	m_PointsList.clear();
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
	int i = 0;
	//int TotalPts = m_PointsList.size();

	GLubyte R, G, B;

	switch (ColorCoding)
	{
	case CODING_INTEN:
		for (i = 0; i < m_PointsList.size(); i++)
		{
			GetColorFromData(m_PointsList[i],m_PointsList[i].intensity, R, G, B);
			glColor3ub(R, G, B);
			glVertex3d(m_PointsList[i].x, m_PointsList[i].y, m_PointsList[i].z);
		}
		break;
	case CODING_X:
		for (i = 0; i < m_PointsList.size(); i++)
		{
			GetColorFromData(m_PointsList[i], m_PointsList[i].x, R, G, B);
			glColor3ub(R, G, B);
			glVertex3d(m_PointsList[i].x, m_PointsList[i].y, m_PointsList[i].z);
		}
		break;
	case CODING_Y:
		for (i = 0; i < m_PointsList.size(); i++)
		{
			GetColorFromData(m_PointsList[i], m_PointsList[i].y, R, G, B);
			glColor3ub(R, G, B);
			glVertex3d(m_PointsList[i].x, m_PointsList[i].y, m_PointsList[i].z);
		}
		break;
	case CODING_Z:
		for (i = 0; i < m_PointsList.size(); i++)
		{
			GetColorFromData(m_PointsList[i], m_PointsList[i].z, R, G, B);
			glColor3ub(R, G, B);
			glVertex3d(m_PointsList[i].x, m_PointsList[i].y, m_PointsList[i].z);
		}
		break;
	case CODING_ID:
		for (i = 0; i < m_PointsList.size(); i++)
		{
			GetColorFromData(m_PointsList[i], m_PointsList[i].Point_ID, R, G, B);
			glColor3ub(R, G, B);
			glVertex3d(m_PointsList[i].x, m_PointsList[i].y, m_PointsList[i].z);
		}
		break;
	case CODING_DIS:
		for (i = 0; i < m_PointsList.size(); i++)
		{
			GetColorFromData(m_PointsList[i], m_PointsList[i].distance, R, G, B);
			glColor3ub(R, G, B);
			glVertex3d(m_PointsList[i].x, m_PointsList[i].y, m_PointsList[i].z);
		}
		break;
	case CODING_HANGLE:
		for (i = 0; i < m_PointsList.size(); i++)
		{
			GetColorFromData(m_PointsList[i], m_PointsList[i].angleH, R, G, B);
			glColor3ub(R, G, B);
			glVertex3d(m_PointsList[i].x, m_PointsList[i].y, m_PointsList[i].z);
		}
		break;
	case CODING_VANGLE:
		for (i = 0; i < m_PointsList.size(); i++)
		{
			GetColorFromData(m_PointsList[i], m_PointsList[i].angleV, R, G, B);
			glColor3ub(R, G, B);
			glVertex3d(m_PointsList[i].x, m_PointsList[i].y, m_PointsList[i].z);
		}
		break;
	case CODING_CHAN:
		for (i = 0; i < m_PointsList.size(); i++)
		{
			GetColorFromData(m_PointsList[i], m_PointsList[i].channel, R, G, B);
			glColor3ub(R, G, B);
			glVertex3d(m_PointsList[i].x, m_PointsList[i].y, m_PointsList[i].z);
		}
		break;
	case CODING_TIME:
		for (i = 0; i < m_PointsList.size(); i++)
		{
			GetColorFromData(m_PointsList[i], m_PointsList[i].timestamp, R, G, B);
			glColor3ub(R, G, B);
			glVertex3d(m_PointsList[i].x, m_PointsList[i].y, m_PointsList[i].z);
		}
		break;
	case CODING_MERGE:
		for (i = 0; i < m_PointsList.size(); i++)
		{
			//GetColorFromData(m_PointsList[i], m_PointsList[i].timestamp, R, G, B);
			glColor3ub(m_PointsList[i].R, m_PointsList[i].G, m_PointsList[i].B);
			glVertex3d(m_PointsList[i].x, m_PointsList[i].y, m_PointsList[i].z);
		}
		break;
	}
	
	glEnd();
	glPopAttrib();
}

/**************************************************************************
*  @Function Name :: GetColorFromData
*  @Description   :: ��ȡĳһ���Ӧ����ɫ�����˸�����ɫ֮�⣬���滹�����һ������ض�ͨ�����и�����ʾ�Ĺ���
*  @Author        :: zzw
*  @Parameters    :: onePoint:ĳ��Ҫ��ȡ��ɫ��ĳһ�� fData:������һ��ֵ���л�ȡ
*					 R:��ȡ��ɫ��ֵ������ G:��ȡ��ɫ��ֵ������ B:��ȡ��ɫ��ֵ������
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::GetColorFromData(RadarPoint onePoint, double fData, GLubyte &R, GLubyte &G, GLubyte &B)
{
	if (-1 != isSelectStatus)
	{//����ѡ���Ӧͨ�������֮��,��ɫ�����óɺ�ɫ
		if (isSelectStatus==onePoint.channel)
		{
			R = 255.0;
			G = 0.0;
			B = 0.0;
			return;
		}
	}

	double DataWidth = m_DataMax - m_DataMin;
	double coef = 255.0 / DataWidth;
	unsigned char index;

	if (fData < m_DataMin)
	{
		R = ColorsTable[m_iColorMode][0][0];
		G = ColorsTable[m_iColorMode][0][1];
		B = ColorsTable[m_iColorMode][0][2];
	}
	else if (fData > m_DataMax)
	{
		R = ColorsTable[m_iColorMode][255][0];
		G = ColorsTable[m_iColorMode][255][1];
		B = ColorsTable[m_iColorMode][255][2];
	}
	else
	{
		index = (unsigned char)((fData - m_DataMin)*coef);
		R = ColorsTable[m_iColorMode][index][0];
		G = ColorsTable[m_iColorMode][index][1];
		B = ColorsTable[m_iColorMode][index][2];
	}
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
*  @Function Name :: GetCrossPoint
*  @Description   :: ����nearpt��farpt���ɵ�������ƵĽ���������������ĵ�
*  @Author        :: zzw
*  @Parameters    :: nearpt:�����ĵ� farpt:Զ���ĵ�
*					 pt:��ȡ������������ĵ�
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
bool RadarPointsNode::GetCrossPoint(VECTOR nearpt, VECTOR farpt, RadarPoint &pt)
{
	bool bFind = false;
	pt.x = pt.y = pt.z = 0.0f;
	RadarPointList CandidatePoints; //��ѡ����

	VECTOR AB, AC, CrossABC;
	AB = farpt - nearpt;
	float dAB = VectorMagnitude(AB); //AB��ģ
	float disTance; //�㵽ֱ�ߵľ���

	float Miu = 0.05f*dAB; //����Χ

	int i;
	int TotalPts = m_PointsList.size();
	CandidatePoints.clear();
	for (i = 0; i < TotalPts; i++)
	{
		AC.x = m_PointsList[i].x - nearpt.x;
		AC.y = m_PointsList[i].y - nearpt.y;
		AC.z = m_PointsList[i].z - nearpt.z;
		CrossABC = VectorCross(AB, AC);
		disTance = VectorMagnitude(CrossABC);

		if (disTance < Miu) //�㵽ֱ�ߵľ��빻С
		{
			CandidatePoints.push_back(m_PointsList[i]);
		}
	}

	//�����ѡ���ƣ��ҵ�����ĵ㷵��
	TotalPts = CandidatePoints.size();
	if (TotalPts >= 1)
	{
		pt = CandidatePoints[0];
		bFind = true;
	}
	disTance = dAB; //�㵽nearpt�ľ��룬dAB���
	float CurDistance;
	VECTOR AD;
	for (i = 0; i < TotalPts; i++)
	{
		AD.x = CandidatePoints[i].x - nearpt.x;
		AD.y = CandidatePoints[i].y - nearpt.y;
		AD.z = CandidatePoints[i].z - nearpt.z;
		CurDistance = VectorMagnitude(AD);
		if (CurDistance < disTance)
		{
			disTance = CurDistance;
			pt = CandidatePoints[i];
			bFind = true;
		}
	}

	return bFind;
}

/**************************************************************************
*  @Function Name :: ChangeColorMode
*  @Description   :: �ı���ɫ�ı���ģʽ
*  @Author        :: zzw
*  @Parameters    :: set_ColorMode:���õ���ɫ�ı���ģʽ
*
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::ChangeColorMode(int set_ColorMode)
{
	if (set_ColorMode >= 0 && set_ColorMode <COLOR_SCALE_COUNT)
	{
		m_iColorMode = set_ColorMode;
	}
	DrawPoints();
}

/**************************************************************************
*  @Function Name :: GetColorMode
*  @Description   :: ��ȡ��ɫ�ı���ģʽ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: ������ɫ�ı���ģʽ
**************************************************************************/
int RadarPointsNode::GetColorMode()
{
	return m_iColorMode;
}

/**************************************************************************
*  @Function Name :: ReScaleColorRange
*  @Description   :: ������ɫ�������ֵ��Χ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::ReScaleColorRange()
{
	double MinValue, MaxValue;
	if (GetMaxAndMin(MinValue, MaxValue))
	{
		m_DataMin = MinValue;
		m_DataMax = MaxValue;
	}

	DrawPoints();
}

/**************************************************************************
*  @Function Name :: SetColorCoding
*  @Description   :: ������ɫ��������ΪchooseColorCoding
*  @Author        :: zzw
*  @Parameters    :: chooseColorCoding:���õ���ɫ��������
*
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::SetColorCoding(COLORCODING chooseColorCoding)
{
	ColorCoding = chooseColorCoding;
	double MinValue=0, MaxValue=0;
	if (GetMaxAndMin(MinValue, MaxValue))
	{
		m_DataMin = MinValue;
		m_DataMax = MaxValue;
	}
}

/**************************************************************************
*  @Function Name :: GetColorCoding
*  @Description   :: �õ���ɫ��������
*  @Author        :: zzw
*  @Parameters    :: getColorCoding:��ɫ�������Ե�����
*
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::GetColorCoding(COLORCODING &getColorCoding)
{
	getColorCoding = ColorCoding;
}

/**************************************************************************
*  @Function Name :: GetSameAngleH
*  @Description   :: �õ���ͬ��ֱ�ǵ����е�
*  @Author        :: zzw
*  @Parameters    :: sameAngleH:��ͬ��ֱ�ǵĵ����
*					 set_AngleH:��Ҫ��Ѱ����ֱ��
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::GetSameAngleH(RadarPointList& sameAngleH, unsigned short set_AngleH)
{
	int TotalPts = m_PointsList.size();
	sameAngleH.clear();//����������еĵ�
	RadarPoint CurPoint;
	for (int i = 0; i < TotalPts; i++)
	{
		if (set_AngleH == m_PointsList[i].angleH)
		{//��ͬ����ֱ�Ƕ�,��ӵ���������
			CurPoint = m_PointsList[i];
			sameAngleH.push_back(CurPoint);
		}
	}
}

/**************************************************************************
*  @Function Name :: GetMaxAndMin
*  @Description   :: �õ����е�������Сֵ
*  @Author        :: zzw
*  @Parameters    :: min:��Ҫ��Ѱ����Сֵ������
*					 max:��Ҫ��Ѱ�����ֵ������
*  @Return        :: �����Ƿ���Ѱ�ɹ�
**************************************************************************/
BOOL RadarPointsNode::GetMaxAndMin(double &min, double &max)
{
	int TotalPts = m_PointsList.size();
	if (TotalPts <= 0)
	{
		return FALSE;
	}
	switch (ColorCoding)
	{
	case CODING_INTEN:
		min = max = m_PointsList[0].intensity;
		for (int i = 0; i < TotalPts; i++)
		{
			if (min > m_PointsList[i].intensity)
			{
				min = m_PointsList[i].intensity;
			}
			if (max < m_PointsList[i].intensity)
			{
				max = m_PointsList[i].intensity;
			}
		}
		break;
	case CODING_X:
		min = max = m_PointsList[0].x;
		for (int i = 0; i < TotalPts; i++)
		{
			if (min > m_PointsList[i].x)
			{
				min = m_PointsList[i].x;
			}
			if (max < m_PointsList[i].x)
			{
				max = m_PointsList[i].x;
			}
		}
		break;
	case CODING_Y:
		min = max = m_PointsList[0].y;
		for (int i = 0; i < TotalPts; i++)
		{
			if (min > m_PointsList[i].y)
			{
				min = m_PointsList[i].y;
			}
			if (max < m_PointsList[i].y)
			{
				max = m_PointsList[i].y;
			}
		}
		break;
	case CODING_Z:
		min = max = m_PointsList[0].z;
		for (int i = 0; i < TotalPts; i++)
		{
			if (min > m_PointsList[i].z)
			{
				min = m_PointsList[i].z;
			}
			if (max < m_PointsList[i].z)
			{
				max = m_PointsList[i].z;
			}
		}
		break;
	case CODING_ID:
		min = max = m_PointsList[0].Point_ID;
		for (int i = 0; i < TotalPts; i++)
		{
			if (min > m_PointsList[i].Point_ID)
			{
				min = m_PointsList[i].Point_ID;
			}
			if (max < m_PointsList[i].Point_ID)
			{
				max = m_PointsList[i].Point_ID;
			}
		}
		break;
	case CODING_DIS:
		min = max = m_PointsList[0].distance;
		for (int i = 0; i < TotalPts; i++)
		{
			if (min > m_PointsList[i].distance)
			{
				min = m_PointsList[i].distance;
			}
			if (max < m_PointsList[i].distance)
			{
				max = m_PointsList[i].distance;
			}
		}
		break;
	case CODING_HANGLE:
		min = max = m_PointsList[0].angleH;
		for (int i = 0; i < TotalPts; i++)
		{
			if (min > m_PointsList[i].angleH)
			{
				min = m_PointsList[i].angleH;
			}
			if (max < m_PointsList[i].angleH)
			{
				max = m_PointsList[i].angleH;
			}
		}
		break;
	case CODING_VANGLE:
		min = max = m_PointsList[0].angleV;
		for (int i = 0; i < TotalPts; i++)
		{
			if (min > m_PointsList[i].angleV)
			{
				min = m_PointsList[i].angleV;
			}
			if (max < m_PointsList[i].angleV)
			{
				max = m_PointsList[i].angleV;
			}
		}
		break;
	case CODING_CHAN:
		min = max = m_PointsList[0].channel;
		for (int i = 0; i < TotalPts; i++)
		{
			if (min > m_PointsList[i].channel)
			{
				min = m_PointsList[i].channel;
			}
			if (max < m_PointsList[i].channel)
			{
				max = m_PointsList[i].channel;
			}
		}
		break;
	case CODING_TIME:
		min = max = m_PointsList[0].timestamp;
		for (int i = 0; i < TotalPts; i++)
		{
			if (min > m_PointsList[i].timestamp)
			{
				min = m_PointsList[i].timestamp;
			}
			if (max < m_PointsList[i].timestamp)
			{
				max = m_PointsList[i].timestamp;
			}
		}
		break;
	}
	return TRUE;
}

/**************************************************************************
*  @Function Name :: SetSelectChannel
*  @Description   :: ���ø�����ʾ��ͨ��
*  @Author        :: zzw
*  @Parameters    :: SetChannelnumber:��Ҫ������ʾ��ͨ����
*
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::SetSelectChannel(int SetChannelnumber)
{//ע��ͨ�����Ժ���Ҫ�����޸ģ����Ӷ�16���ж�
	if (SetChannelnumber < 0)
	{
		return;
	}
	isSelectStatus = SetChannelnumber;
}

/**************************************************************************
*  @Function Name :: CancelChannel
*  @Description   :: ȡ��ͨ���ĸ�����ʾ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::CancelChannel()
{
	isSelectStatus = -1;
}

/**************************************************************************
*  @Function Name :: GetDefaultBoundBox
*  @Description   :: ��ȡ��ǰ��֡�ĵ�BoundBox������������
*  @Author        :: zzw
*  @Parameters    :: GridLength:����ı߳�
*					 GridRadius:ͬ��Բ�İ뾶
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::GetDefaultBoundBox(double &GridLength, double &GridRadius)
{
	RadarPoint CurPoint;
	int Num = m_PointsList.size();
	double BoundBox[6];
	BoundBox[0] = BoundBox[2] = BoundBox[4] = 0;//x,y,z
	BoundBox[1] = BoundBox[3] = BoundBox[5] = 0;

	for (int i = 0; i < m_PointsList.size(); i++)
	{
		CurPoint = m_PointsList[i];

		//x
		if (BoundBox[0] > CurPoint.x)
		{
			BoundBox[0] = CurPoint.x;//xmin
		}
		if (BoundBox[1] < CurPoint.x)
		{
			BoundBox[1] = CurPoint.x;//xmax
		}
		//y
		if (BoundBox[2] > CurPoint.y)
		{
			BoundBox[2] = CurPoint.y;//ymin
		}
		if (BoundBox[3] < CurPoint.y)
		{
			BoundBox[3] = CurPoint.y;//ymax
		}
		//z
		if (BoundBox[4] > CurPoint.z)
		{
			BoundBox[4] = CurPoint.z;//zmin
		}
		if (BoundBox[5] < CurPoint.z)
		{
			BoundBox[5] = CurPoint.z;//zmax
		}
	}

	//�ҳ�6��������������
	double xmax = max(fabs(BoundBox[0]), fabs(BoundBox[1]));
	double ymax = max(fabs(BoundBox[2]), fabs(BoundBox[3]));
	double zmax = max(fabs(BoundBox[4]), fabs(BoundBox[5]));
	double smax = max(fabs(xmax), fabs(ymax));
	smax = max(fabs(smax), fabs(zmax));
	GridLength = ceil(smax / 10) * 20;
	GridRadius = ceil(smax / 10) * 10;
}

RadarPointList & RadarPointsNode::gerCurrentPointList()
{
	return m_PointsList;
}
