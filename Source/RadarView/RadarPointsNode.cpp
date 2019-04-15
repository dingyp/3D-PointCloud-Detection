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
	ColorCoding = CODING_INTEN;
	isSelectStatus = -1;//没有在选择相同通道的情况
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
}

/**************************************************************************
*  @Function Name :: AttachPointsData
*  @Description   :: 将点加进来
*  @Author        :: zzw
*  @Parameters    :: DataList:存储点帧的队列
*
*  @Return        :: 返回是否添加成功
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
*  @Description   :: 获取显示的点帧
*  @Author        :: zzw
*  @Parameters    :: PointList:获取显示点帧的引用
*
*  @Return        :: 返回是否获取成功
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
*  @Description   :: 变换显示帧的数据
*  @Author        :: zzw
*  @Parameters    :: addX:变换的X方向的位移 addY:变换的Y方向的位移 addZ:变换的Z方向的位移
*					 pitch:X轴的旋转角 roll:Y轴的旋转角 yaw:Z轴的旋转角 transformer:对点帧进行变换的类
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::RefreshPointData(double addX, double addY, double addZ, double pitch, double roll, double yaw, DataProcess::Transform &transformer)
{
	transformer.ChangeOne(addX, addY, addZ, pitch, roll, yaw, m_PointsList);//需要有支持一次性的变换
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
	m_PointsList.clear();
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
*  @Description   :: 获取某一点对应的颜色，除了赋予颜色之外，里面还添加了一个针对特定通道进行高亮显示的功能
*  @Author        :: zzw
*  @Parameters    :: onePoint:某需要获取颜色的某一点 fData:根据这一数值进行获取
*					 R:获取红色数值的引用 G:获取绿色数值的引用 B:获取蓝色数值的引用
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::GetColorFromData(RadarPoint onePoint, double fData, GLubyte &R, GLubyte &G, GLubyte &B)
{
	if (-1 != isSelectStatus)
	{//是在选择对应通道的情况之下,颜色先设置成红色
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
*  @Function Name :: GetCrossPoint
*  @Description   :: 搜索nearpt和farpt连成的线与点云的交点中离视线最近的点
*  @Author        :: zzw
*  @Parameters    :: nearpt:近处的点 farpt:远处的点
*					 pt:获取的视线中最近的点
*  @Return        :: 返回是否成功
**************************************************************************/
bool RadarPointsNode::GetCrossPoint(VECTOR nearpt, VECTOR farpt, RadarPoint &pt)
{
	bool bFind = false;
	pt.x = pt.y = pt.z = 0.0f;
	RadarPointList CandidatePoints; //候选点云

	VECTOR AB, AC, CrossABC;
	AB = farpt - nearpt;
	float dAB = VectorMagnitude(AB); //AB的模
	float disTance; //点到直线的距离

	float Miu = 0.05f*dAB; //允许范围

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

		if (disTance < Miu) //点到直线的距离够小
		{
			CandidatePoints.push_back(m_PointsList[i]);
		}
	}

	//处理候选点云，找到最近的点返回
	TotalPts = CandidatePoints.size();
	if (TotalPts >= 1)
	{
		pt = CandidatePoints[0];
		bFind = true;
	}
	disTance = dAB; //点到nearpt的距离，dAB最大
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
*  @Description   :: 改变颜色的编码模式
*  @Author        :: zzw
*  @Parameters    :: set_ColorMode:设置的颜色的编码模式
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
*  @Description   :: 获取颜色的编码模式
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 返回颜色的编码模式
**************************************************************************/
int RadarPointsNode::GetColorMode()
{
	return m_iColorMode;
}

/**************************************************************************
*  @Function Name :: ReScaleColorRange
*  @Description   :: 重置颜色编码的数值范围
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
*  @Description   :: 设置颜色编码属性为chooseColorCoding
*  @Author        :: zzw
*  @Parameters    :: chooseColorCoding:设置的颜色编码属性
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
*  @Description   :: 得到颜色编码属性
*  @Author        :: zzw
*  @Parameters    :: getColorCoding:颜色编码属性的引用
*
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::GetColorCoding(COLORCODING &getColorCoding)
{
	getColorCoding = ColorCoding;
}

/**************************************************************************
*  @Function Name :: GetSameAngleH
*  @Description   :: 得到相同竖直角的所有点
*  @Author        :: zzw
*  @Parameters    :: sameAngleH:相同竖直角的点队列
*					 set_AngleH:需要找寻的竖直角
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::GetSameAngleH(RadarPointList& sameAngleH, unsigned short set_AngleH)
{
	int TotalPts = m_PointsList.size();
	sameAngleH.clear();//清空里面所有的点
	RadarPoint CurPoint;
	for (int i = 0; i < TotalPts; i++)
	{
		if (set_AngleH == m_PointsList[i].angleH)
		{//相同的竖直角度,添加到队列里面
			CurPoint = m_PointsList[i];
			sameAngleH.push_back(CurPoint);
		}
	}
}

/**************************************************************************
*  @Function Name :: GetMaxAndMin
*  @Description   :: 得到所有点的最大最小值
*  @Author        :: zzw
*  @Parameters    :: min:需要找寻的最小值的引用
*					 max:需要找寻的最大值的引用
*  @Return        :: 返回是否找寻成功
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
*  @Description   :: 设置高亮显示的通道
*  @Author        :: zzw
*  @Parameters    :: SetChannelnumber:需要高亮显示的通道号
*
*  @Return        :: None
**************************************************************************/
void RadarPointsNode::SetSelectChannel(int SetChannelnumber)
{//注意通道数以后需要进行修改，增加对16的判断
	if (SetChannelnumber < 0)
	{
		return;
	}
	isSelectStatus = SetChannelnumber;
}

/**************************************************************************
*  @Function Name :: CancelChannel
*  @Description   :: 取消通道的高亮显示
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
*  @Description   :: 获取当前点帧的的BoundBox方面的相关属性
*  @Author        :: zzw
*  @Parameters    :: GridLength:网格的边长
*					 GridRadius:同心圆的半径
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

	//找出6个数里面最大的数
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
