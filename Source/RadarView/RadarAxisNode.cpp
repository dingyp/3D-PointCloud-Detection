#include "stdafx.h"
#include "RadarAxisNode.h"

/**************************************************************************
*  @Function Name :: RadarAxisNode
*  @Description   :: RadarAxisNode类的带有文本列表ID的构造函数
*  @Author        :: zzw
*  @Parameters    :: TextListID:文本列表的ID
*
*  @Return        :: None
**************************************************************************/
RadarAxisNode::RadarAxisNode(UINT TextListID)
{
	m_TextListID = TextListID;

	m_fLineLenth = 20.0f;

	m_pNormalsX = NULL;
	m_pVerticesX = NULL;

	m_pNormalsY = NULL;
	m_pVerticesY = NULL;

	m_pNormalsZ = NULL;
	m_pVerticesZ = NULL;

	m_iTriNum = 100;

	m_bIsDraw = true;

	GenerateArrows(); //生成锥体
}

/**************************************************************************
*  @Function Name :: ~RadarAxisNode
*  @Description   :: RadarAxisNode类的默认析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
RadarAxisNode::~RadarAxisNode()
{
	DestroyPointers();
}

/**************************************************************************
*  @Function Name :: DestroyPointers
*  @Description   :: 对坐标轴对象进行销毁
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void RadarAxisNode::DestroyPointers()
{
	if (m_pNormalsX != NULL)
	{
		delete[]m_pNormalsX;
		m_pNormalsX = NULL;
	}
	if (m_pNormalsY != NULL)
	{
		delete[]m_pNormalsY;
		m_pNormalsY = NULL;
	}
	if (m_pNormalsZ != NULL)
	{
		delete[]m_pNormalsZ;
		m_pNormalsZ = NULL;
	}

	if (m_pVerticesX != NULL)
	{
		delete[]m_pVerticesX;
		m_pVerticesX = NULL;
	}
	if (m_pVerticesY != NULL)
	{
		delete[]m_pVerticesY;
		m_pVerticesY = NULL;
	}
	if (m_pVerticesZ != NULL)
	{
		delete[]m_pVerticesZ;
		m_pVerticesZ = NULL;
	}
}

/**************************************************************************
*  @Function Name :: Draw
*  @Description   :: 绘制坐标轴和文字
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void RadarAxisNode::Draw()
{
	if (FALSE == IsShowed())
	{
		return;
	}

	if (false == m_bIsDraw)
	{
		return;
	}

	glPushMatrix();
	glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	LoadMatrix();

	DrawAxis();
	DrawArrows();

	//文字
	if (m_TextListID != 0)
	{
		glEnable(GL_COLOR_MATERIAL);

		//X轴
		glPushMatrix();
		glTranslatef(1.1f*m_fLineLenth , 0.0, 0.0);
		glRotatef(90, 1.0, 0.0, 0.0);
		glScalef(0.25f*m_fLineLenth, 0.25f*m_fLineLenth, 0.25f*m_fLineLenth);
		glColor3f(1.0, 0.0, 0.0);
		glListBase(m_TextListID);
		glCallLists(1, GL_UNSIGNED_BYTE, _T("X"));
		glPopMatrix();

		//Y轴
		glPushMatrix();
		glTranslatef(0.0, 1.1f*m_fLineLenth, 0.0);
		glRotatef(90, 1.0, 0.0, 0.0);
		glScalef(0.25f*m_fLineLenth, 0.25f*m_fLineLenth, 0.25f*m_fLineLenth);
		glColor3f(0.0, 1.0, 0.0);
		glListBase(m_TextListID);
		glCallLists(1, GL_UNSIGNED_BYTE, _T("Y"));
		glPopMatrix();

		//Z轴
		glPushMatrix();
		glTranslatef(0.0, 0.0, 1.1f*m_fLineLenth);
		glRotatef(90, 1.0, 0.0, 0.0);
		glScalef(0.25f*m_fLineLenth, 0.25f*m_fLineLenth, 0.25f*m_fLineLenth);
		glColor3f(0.0, 0.0, 1.0);
		glListBase(m_TextListID);
		glCallLists(1, GL_UNSIGNED_BYTE, _T("Z"));
		glPopMatrix();
		glDisable(GL_COLOR_MATERIAL); //该属性只对文字有效

	}
	glPopAttrib();
	glPopClientAttrib();
	glPopMatrix();
}

/**************************************************************************
*  @Function Name :: DrawAxis
*  @Description   :: 绘制坐标轴的牵引轴
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void RadarAxisNode::DrawAxis()
{
	glPushAttrib(GL_LIGHTING_BIT | GL_POINT_BIT);
	glDisable(GL_LIGHTING);
	glLineWidth(3.0); //坐标轴

	glBegin(GL_LINES);

	glColor3f(1.0f, 0.0, 0.0); //红色X坐标轴
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(1.0f*m_fLineLenth,0.0f, 0.0f);

	glColor3f(0.0, 1.0f, 0.0); //绿色Y坐标轴
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0f, 1.0f*m_fLineLenth, 0.0f);

	glColor3f(0.0, 0.0, 1.0f); //蓝色Z坐标轴
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0f, 0.0f, 1.0f*m_fLineLenth);
	glEnd();

	glPopAttrib();

}

/**************************************************************************
*  @Function Name :: DrawArrows
*  @Description   :: 绘制坐标轴的箭头
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void RadarAxisNode::DrawArrows()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glEnable(GL_COLOR_MATERIAL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	unsigned int i;
	int index = 0;

	//X轴箭头
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_TRIANGLES);
	for (i = 0; i < m_iTriNum * 3; i++)
	{
		glNormal3f(m_pNormalsX[i].x, m_pNormalsX[i].y, m_pNormalsX[i].z);
		glVertex3f(m_pVerticesX[i].x, m_pVerticesX[i].y, m_pVerticesX[i].z);
	}
	glEnd();

	//Y轴箭头
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_TRIANGLES);
	for (i = 0; i < m_iTriNum * 3; i++)
	{
		glNormal3f(m_pNormalsY[i].x, m_pNormalsY[i].y, m_pNormalsY[i].z);
		glVertex3f(m_pVerticesY[i].x, m_pVerticesY[i].y, m_pVerticesY[i].z);
	}
	glEnd();

	//Z轴箭头
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_TRIANGLES);
	for (i = 0; i < m_iTriNum * 3; i++)
	{
		glNormal3f(m_pNormalsZ[i].x, m_pNormalsZ[i].y, m_pNormalsZ[i].z);
		glVertex3f(m_pVerticesZ[i].x, m_pVerticesZ[i].y, m_pVerticesZ[i].z);
	}
	glEnd();

	glPopAttrib();
}

/**************************************************************************
*  @Function Name :: GenerateArrows
*  @Description   :: 生成圆锥体
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void RadarAxisNode::GenerateArrows()
{
	DestroyPointers(); //销毁指针
	int PointsNum = m_iTriNum / 2; //分成的等分数
	int VertexNum = m_iTriNum * 3; //圆锥的顶点数
	int i = 0;
	//float Height = 3.0f;
	float bottom = 0.85f*m_fLineLenth;
	double DeltaAngle = 2.0*PI / double(PointsNum);
	double Angle, Angle1;
	double Radius = 0.1*m_fLineLenth;

	//X方向的箭头
	m_pVerticesX = new VECTOR[VertexNum];
	m_pNormalsX = new VECTOR[VertexNum];

	for (i = 0; i < PointsNum; i++)
	{
		Angle = i * DeltaAngle;
		Angle1 = (i + 1) * DeltaAngle;

		//尖点
		m_pVerticesX[i * 6].x = m_fLineLenth;
		m_pVerticesX[i * 6].y = 0.0;
		m_pVerticesX[i * 6].z = 0.0;
		m_pNormalsX[i * 6].x = 1.0;
		m_pNormalsX[i * 6].y = 0.0;
		m_pNormalsX[i * 6].z = 0.0;

		m_pVerticesX[i * 6 + 1].x = bottom;
		m_pVerticesX[i * 6 + 1].y = Radius * cos(Angle);
		m_pVerticesX[i * 6 + 1].z = Radius * sin(Angle);
		m_pNormalsX[i * 6 + 1].x = 0.0;
		m_pNormalsX[i * 6 + 1].y = cos(Angle);
		m_pNormalsX[i * 6 + 1].z = sin(Angle);

		m_pVerticesX[i * 6 + 2].x = bottom;
		m_pVerticesX[i * 6 + 2].y = Radius * cos(Angle1);
		m_pVerticesX[i * 6 + 2].z = Radius * sin(Angle1);
		m_pNormalsX[i * 6 + 2].x = 0.0;
		m_pNormalsX[i * 6 + 2].y = cos(Angle1);
		m_pNormalsX[i * 6 + 2].z = sin(Angle1);

		//底面圆心
		m_pVerticesX[i * 6 + 3].x = bottom;
		m_pVerticesX[i * 6 + 3].y = 0.0;
		m_pVerticesX[i * 6 + 3].z = 0.0;
		m_pNormalsX[i * 6 + 3].x = -1.0;
		m_pNormalsX[i * 6 + 3].y = 0.0;
		m_pNormalsX[i * 6 + 3].z = 0.0;

		m_pVerticesX[i * 6 + 4].x = bottom;
		m_pVerticesX[i * 6 + 4].y = Radius * cos(Angle1);
		m_pVerticesX[i * 6 + 4].z = Radius * sin(Angle1);
		m_pNormalsX[i * 6 + 4].x = -1.0;
		m_pNormalsX[i * 6 + 4].y = 0.0;
		m_pNormalsX[i * 6 + 4].z = 0.0;

		m_pVerticesX[i * 6 + 5].x = bottom;
		m_pVerticesX[i * 6 + 5].y = Radius * cos(Angle);
		m_pVerticesX[i * 6 + 5].z = Radius * sin(Angle);
		m_pNormalsX[i * 6 + 5].x = -1.0;
		m_pNormalsX[i * 6 + 5].y = 0.0;
		m_pNormalsX[i * 6 + 5].z = 0.0;
	}

	//Y方向的箭头
	m_pVerticesY = new VECTOR[VertexNum];
	m_pNormalsY = new VECTOR[VertexNum];

	for (i = 0; i < PointsNum; i++)
	{
		Angle = i * DeltaAngle;
		Angle1 = (i + 1) * DeltaAngle;

		//尖点
		m_pVerticesY[i * 6].x = 0.0;
		m_pVerticesY[i * 6].y = m_fLineLenth;
		m_pVerticesY[i * 6].z = 0.0;
		m_pNormalsY[i * 6].x = 0.0;
		m_pNormalsY[i * 6].y = 1.0;
		m_pNormalsY[i * 6].z = 0.0;

		m_pVerticesY[i * 6 + 1].x = Radius * cos(Angle1);
		m_pVerticesY[i * 6 + 1].y = bottom;
		m_pVerticesY[i * 6 + 1].z = Radius * sin(Angle1);
		m_pNormalsY[i * 6 + 1].x = cos(Angle1);
		m_pNormalsY[i * 6 + 1].y = 0.0;
		m_pNormalsY[i * 6 + 1].z = sin(Angle1);

		m_pVerticesY[i * 6 + 2].x = Radius * cos(Angle);
		m_pVerticesY[i * 6 + 2].y = bottom;
		m_pVerticesY[i * 6 + 2].z = Radius * sin(Angle);
		m_pNormalsY[i * 6 + 2].x = cos(Angle);
		m_pNormalsY[i * 6 + 2].y = 0.0;
		m_pNormalsY[i * 6 + 2].z = sin(Angle);

		//底面圆心
		m_pVerticesY[i * 6 + 3].x = 0.0;
		m_pVerticesY[i * 6 + 3].y = bottom;
		m_pVerticesY[i * 6 + 3].z = 0.0;
		m_pNormalsY[i * 6 + 3].x = 0.0;
		m_pNormalsY[i * 6 + 3].y = -1.0;
		m_pNormalsY[i * 6 + 3].z = 0.0;

		m_pVerticesY[i * 6 + 4].x = Radius * cos(Angle);
		m_pVerticesY[i * 6 + 4].y = bottom;
		m_pVerticesY[i * 6 + 4].z = Radius * sin(Angle);
		m_pNormalsY[i * 6 + 4].x = 0.0;
		m_pNormalsY[i * 6 + 4].y = -1.0;
		m_pNormalsY[i * 6 + 4].z = 0.0;

		m_pVerticesY[i * 6 + 5].x = Radius * cos(Angle1);
		m_pVerticesY[i * 6 + 5].y = bottom;
		m_pVerticesY[i * 6 + 5].z = Radius * sin(Angle1);
		m_pNormalsY[i * 6 + 5].x = 0.0;
		m_pNormalsY[i * 6 + 5].y = -1.0;
		m_pNormalsY[i * 6 + 5].z = 0.0;
	}

	//Z方向的箭头
	m_pVerticesZ = new VECTOR[VertexNum];
	m_pNormalsZ = new VECTOR[VertexNum];

	for (i = 0; i < PointsNum; i++)
	{
		Angle = i * DeltaAngle;
		Angle1 = (i + 1) * DeltaAngle;

		//尖点
		m_pVerticesZ[i * 6].x = 0.0;
		m_pVerticesZ[i * 6].y = 0.0;
		m_pVerticesZ[i * 6].z = m_fLineLenth;
		m_pNormalsZ[i * 6].x = 0.0;
		m_pNormalsZ[i * 6].y = 0.0;
		m_pNormalsZ[i * 6].z = 1.0;

		m_pVerticesZ[i * 6 + 1].x = Radius * cos(Angle);
		m_pVerticesZ[i * 6 + 1].y = Radius * sin(Angle);
		m_pVerticesZ[i * 6 + 1].z = bottom;
		m_pNormalsZ[i * 6 + 1].x = cos(Angle);
		m_pNormalsZ[i * 6 + 1].y = sin(Angle);
		m_pNormalsZ[i * 6 + 1].z = 0.0;

		m_pVerticesZ[i * 6 + 2].x = Radius * cos(Angle1);
		m_pVerticesZ[i * 6 + 2].y = Radius * sin(Angle1);
		m_pVerticesZ[i * 6 + 2].z = bottom;
		m_pNormalsZ[i * 6 + 2].x = cos(Angle1);
		m_pNormalsZ[i * 6 + 2].y = sin(Angle1);
		m_pNormalsZ[i * 6 + 2].z = 0.0;

		//底面圆心
		m_pVerticesZ[i * 6 + 3].x = 0.0;
		m_pVerticesZ[i * 6 + 3].y = 0.0;
		m_pVerticesZ[i * 6 + 3].z = bottom;
		m_pNormalsZ[i * 6 + 3].x = 0.0;
		m_pNormalsZ[i * 6 + 3].y = 0.0;
		m_pNormalsZ[i * 6 + 3].z = -1.0;

		m_pVerticesZ[i * 6 + 4].x = Radius * cos(Angle1);
		m_pVerticesZ[i * 6 + 4].y = Radius * sin(Angle1);
		m_pVerticesZ[i * 6 + 4].z = bottom;
		m_pNormalsZ[i * 6 + 4].x = 0.0;
		m_pNormalsZ[i * 6 + 4].y = 0.0;
		m_pNormalsZ[i * 6 + 4].z = -1.0;

		m_pVerticesZ[i * 6 + 5].x = Radius * cos(Angle);
		m_pVerticesZ[i * 6 + 5].y = Radius * sin(Angle);
		m_pVerticesZ[i * 6 + 5].z = bottom;
		m_pNormalsZ[i * 6 + 5].x = 0.0;
		m_pNormalsZ[i * 6 + 5].y = 0.0;
		m_pNormalsZ[i * 6 + 5].z = -1.0;
	}

}

/**************************************************************************
*  @Function Name :: SetLineLength
*  @Description   :: 设置坐标轴的长度
*  @Author        :: zzw
*  @Parameters    :: setLineLength:设置坐标轴的长度
*
*  @Return        :: None
**************************************************************************/
void RadarAxisNode::SetLineLength(double setLineLength)
{
	if (setLineLength > 0)
	{
		m_fLineLenth = setLineLength;
		GenerateArrows(); //重新生成圆锥体
	}
}

/**************************************************************************
*  @Function Name :: SetIsShowed
*  @Description   :: 设置坐标轴是否显示
*  @Author        :: zzw
*  @Parameters    :: setIsDraw:设置坐标轴是否显示的标志位
*
*  @Return        :: None
**************************************************************************/
void RadarAxisNode::SetIsShowed(bool setIsDraw)
{
	m_bIsDraw = setIsDraw;
}

/**************************************************************************
*  @Function Name :: GetIsShowed
*  @Description   :: 获取坐标轴是否显示标志位
*  @Author        :: zzw
*  @Parameters    :: setIsDraw:坐标轴是否显示的标志位
*
*  @Return        :: None
**************************************************************************/
void RadarAxisNode::GetIsShowed(bool &setIsDraw)
{
	setIsDraw = m_bIsDraw;
}