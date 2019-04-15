#include "stdafx.h"
#include "RadarAxisNode.h"

/**************************************************************************
*  @Function Name :: RadarAxisNode
*  @Description   :: RadarAxisNode��Ĵ����ı��б�ID�Ĺ��캯��
*  @Author        :: zzw
*  @Parameters    :: TextListID:�ı��б��ID
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

	GenerateArrows(); //����׶��
}

/**************************************************************************
*  @Function Name :: ~RadarAxisNode
*  @Description   :: RadarAxisNode���Ĭ����������
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
*  @Description   :: ������������������
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
*  @Description   :: ���������������
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

	//����
	if (m_TextListID != 0)
	{
		glEnable(GL_COLOR_MATERIAL);

		//X��
		glPushMatrix();
		glTranslatef(1.1f*m_fLineLenth , 0.0, 0.0);
		glRotatef(90, 1.0, 0.0, 0.0);
		glScalef(0.25f*m_fLineLenth, 0.25f*m_fLineLenth, 0.25f*m_fLineLenth);
		glColor3f(1.0, 0.0, 0.0);
		glListBase(m_TextListID);
		glCallLists(1, GL_UNSIGNED_BYTE, _T("X"));
		glPopMatrix();

		//Y��
		glPushMatrix();
		glTranslatef(0.0, 1.1f*m_fLineLenth, 0.0);
		glRotatef(90, 1.0, 0.0, 0.0);
		glScalef(0.25f*m_fLineLenth, 0.25f*m_fLineLenth, 0.25f*m_fLineLenth);
		glColor3f(0.0, 1.0, 0.0);
		glListBase(m_TextListID);
		glCallLists(1, GL_UNSIGNED_BYTE, _T("Y"));
		glPopMatrix();

		//Z��
		glPushMatrix();
		glTranslatef(0.0, 0.0, 1.1f*m_fLineLenth);
		glRotatef(90, 1.0, 0.0, 0.0);
		glScalef(0.25f*m_fLineLenth, 0.25f*m_fLineLenth, 0.25f*m_fLineLenth);
		glColor3f(0.0, 0.0, 1.0);
		glListBase(m_TextListID);
		glCallLists(1, GL_UNSIGNED_BYTE, _T("Z"));
		glPopMatrix();
		glDisable(GL_COLOR_MATERIAL); //������ֻ��������Ч

	}
	glPopAttrib();
	glPopClientAttrib();
	glPopMatrix();
}

/**************************************************************************
*  @Function Name :: DrawAxis
*  @Description   :: �����������ǣ����
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void RadarAxisNode::DrawAxis()
{
	glPushAttrib(GL_LIGHTING_BIT | GL_POINT_BIT);
	glDisable(GL_LIGHTING);
	glLineWidth(3.0); //������

	glBegin(GL_LINES);

	glColor3f(1.0f, 0.0, 0.0); //��ɫX������
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(1.0f*m_fLineLenth,0.0f, 0.0f);

	glColor3f(0.0, 1.0f, 0.0); //��ɫY������
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0f, 1.0f*m_fLineLenth, 0.0f);

	glColor3f(0.0, 0.0, 1.0f); //��ɫZ������
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0f, 0.0f, 1.0f*m_fLineLenth);
	glEnd();

	glPopAttrib();

}

/**************************************************************************
*  @Function Name :: DrawArrows
*  @Description   :: ����������ļ�ͷ
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

	//X���ͷ
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_TRIANGLES);
	for (i = 0; i < m_iTriNum * 3; i++)
	{
		glNormal3f(m_pNormalsX[i].x, m_pNormalsX[i].y, m_pNormalsX[i].z);
		glVertex3f(m_pVerticesX[i].x, m_pVerticesX[i].y, m_pVerticesX[i].z);
	}
	glEnd();

	//Y���ͷ
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_TRIANGLES);
	for (i = 0; i < m_iTriNum * 3; i++)
	{
		glNormal3f(m_pNormalsY[i].x, m_pNormalsY[i].y, m_pNormalsY[i].z);
		glVertex3f(m_pVerticesY[i].x, m_pVerticesY[i].y, m_pVerticesY[i].z);
	}
	glEnd();

	//Z���ͷ
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
*  @Description   :: ����Բ׶��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void RadarAxisNode::GenerateArrows()
{
	DestroyPointers(); //����ָ��
	int PointsNum = m_iTriNum / 2; //�ֳɵĵȷ���
	int VertexNum = m_iTriNum * 3; //Բ׶�Ķ�����
	int i = 0;
	//float Height = 3.0f;
	float bottom = 0.85f*m_fLineLenth;
	double DeltaAngle = 2.0*PI / double(PointsNum);
	double Angle, Angle1;
	double Radius = 0.1*m_fLineLenth;

	//X����ļ�ͷ
	m_pVerticesX = new VECTOR[VertexNum];
	m_pNormalsX = new VECTOR[VertexNum];

	for (i = 0; i < PointsNum; i++)
	{
		Angle = i * DeltaAngle;
		Angle1 = (i + 1) * DeltaAngle;

		//���
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

		//����Բ��
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

	//Y����ļ�ͷ
	m_pVerticesY = new VECTOR[VertexNum];
	m_pNormalsY = new VECTOR[VertexNum];

	for (i = 0; i < PointsNum; i++)
	{
		Angle = i * DeltaAngle;
		Angle1 = (i + 1) * DeltaAngle;

		//���
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

		//����Բ��
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

	//Z����ļ�ͷ
	m_pVerticesZ = new VECTOR[VertexNum];
	m_pNormalsZ = new VECTOR[VertexNum];

	for (i = 0; i < PointsNum; i++)
	{
		Angle = i * DeltaAngle;
		Angle1 = (i + 1) * DeltaAngle;

		//���
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

		//����Բ��
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
*  @Description   :: ����������ĳ���
*  @Author        :: zzw
*  @Parameters    :: setLineLength:����������ĳ���
*
*  @Return        :: None
**************************************************************************/
void RadarAxisNode::SetLineLength(double setLineLength)
{
	if (setLineLength > 0)
	{
		m_fLineLenth = setLineLength;
		GenerateArrows(); //��������Բ׶��
	}
}

/**************************************************************************
*  @Function Name :: SetIsShowed
*  @Description   :: �����������Ƿ���ʾ
*  @Author        :: zzw
*  @Parameters    :: setIsDraw:�����������Ƿ���ʾ�ı�־λ
*
*  @Return        :: None
**************************************************************************/
void RadarAxisNode::SetIsShowed(bool setIsDraw)
{
	m_bIsDraw = setIsDraw;
}

/**************************************************************************
*  @Function Name :: GetIsShowed
*  @Description   :: ��ȡ�������Ƿ���ʾ��־λ
*  @Author        :: zzw
*  @Parameters    :: setIsDraw:�������Ƿ���ʾ�ı�־λ
*
*  @Return        :: None
**************************************************************************/
void RadarAxisNode::GetIsShowed(bool &setIsDraw)
{
	setIsDraw = m_bIsDraw;
}