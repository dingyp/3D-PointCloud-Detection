#include "stdafx.h"
#include "BaseLinesNode.h"

/**************************************************************************
*  @Function Name :: BaseLinesNode
*  @Description   :: BaseLinesNode��Ĺ��캯��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
BaseLinesNode::BaseLinesNode()
{
	m_vColor.x = 0.5;
	m_vColor.y = 0.5;
	m_vColor.z = 0.5; //ȱʡΪ��ɫ

	m_fLineWidth = 1.0;
	m_fLineDistance = 30; //20��
	m_fLineStep = 2; //���2�׻�һ����

	addx = 0.00;//���������ʼΪ(0,0,0)
	addy = 0.00;
	addz = 0.00;

	m_eLinePlaneType = XOY; //ȱʡ��XOYƽ��

	m_LineList.clear();
}

/**************************************************************************
*  @Function Name :: ~BaseLinesNode
*  @Description   :: BaseLinesNode�����������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
BaseLinesNode::~BaseLinesNode()
{
	if (!m_LineList.empty())
		m_LineList.clear();
}

/**************************************************************************
*  @Function Name :: Draw
*  @Description   :: ����������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void BaseLinesNode::Draw()
{
	if (FALSE == IsShowed())
	{
		return;
	}

	if (NONEVIEW == m_eLinePlaneType)
	{
		return;
	}

	glPushMatrix();
	LoadMatrix();	// Translate, Rotate, Scale

	DrawLines();

	glPopMatrix();

}

/**************************************************************************
*  @Function Name :: DrawLines
*  @Description   :: ��������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void BaseLinesNode::DrawLines()
{
	glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glColor3f(m_vColor.x, m_vColor.y, m_vColor.z);

	if (m_bShowed == TRUE)
	{
		glLineWidth(m_fLineWidth);

		int LineNum = m_LineList.size();
		int i;

		glBegin(GL_LINES);
		for (i = 0; i < LineNum; i++)
		{
			glVertex3f(m_LineList[i].beginpt.x + addx, m_LineList[i].beginpt.y + addy, m_LineList[i].beginpt.z + addz);//ÿ���㶼����ƫ����
			glVertex3f(m_LineList[i].endpt.x + addx, m_LineList[i].endpt.y + addy, m_LineList[i].endpt.z + addz);
		}
		glEnd();
	}
	glPopAttrib();

}

/**************************************************************************
*  @Function Name :: SetGridCenter
*  @Description   :: �������������
*  @Author        :: zzw
*  @Parameters    :: SetX:�������ĵ�x���� SetY:�������ĵ�y����
*					 SetZ:�������ĵ�z����
*  @Return        :: None
**************************************************************************/
void BaseLinesNode::SetGridCenter(float SetX, float SetY, float SetZ)
{
	addx = SetX;
	addy = SetY;
	addz = SetZ;
}

/**************************************************************************
*  @Function Name :: SetGridColor
*  @Description   :: ����������ɫ
*  @Author        :: zzw
*  @Parameters    :: SetGridColor:�����������ɫ
*
*  @Return        :: None
**************************************************************************/
void BaseLinesNode::SetGridColor(VECTOR SetGridColor)
{
	m_vColor.x = SetGridColor.x;
	m_vColor.y = SetGridColor.y;
	m_vColor.z = SetGridColor.z;
}

/**************************************************************************
*  @Function Name :: SetLinePlane
*  @Description   :: ����������
*  @Author        :: zzw
*  @Parameters    :: eLinePlaneType:��������ĳ���
*
*  @Return        :: None
**************************************************************************/
void BaseLinesNode::SetLinePlane(LINEPLANE_TYPE eLinePlaneType)
{
	m_eLinePlaneType = eLinePlaneType;
	//����������
	m_LineList.clear();
	if (GRID == m_eLineDrawType)
	{
		GenerateGridLines();
	}
	else if (CON_CIRCLE == m_eLineDrawType)
	{
		GenerateConCricles();
	}
}

/**************************************************************************
*  @Function Name :: SetLineDraw
*  @Description   :: ������������
*  @Author        :: zzw
*  @Parameters    :: eLineDrawType:�������������
*
*  @Return        :: None
**************************************************************************/
void  BaseLinesNode::SetLineDraw(LINEDRAW_TYPE eLineDrawType)
{
	m_eLineDrawType = eLineDrawType;
	//����������
	m_LineList.clear();
	if (GRID == m_eLineDrawType)
	{
		GenerateGridLines();
	}
	else if (CON_CIRCLE == m_eLineDrawType)
	{
		GenerateConCricles();
	}
}

/**************************************************************************
*  @Function Name :: SetLineParameters
*  @Description   :: ���������С
*  @Author        :: zzw
*  @Parameters    :: LineDistance:��������ĳ���
*					 LineStep:��������Ĳ���
*  @Return        :: None
**************************************************************************/
void BaseLinesNode::SetLineParameters(float LineDistance, float LineStep)
{
	m_fLineDistance = LineDistance;
	m_fLineStep = LineStep;

	//����������
	m_LineList.clear();
	if (GRID == m_eLineDrawType)
	{
		GenerateGridLines();
	}
	else if (CON_CIRCLE == m_eLineDrawType)
	{
		GenerateConCricles();
	}
}

/**************************************************************************
*  @Function Name :: GenerateConCricles
*  @Description   :: ����ͬ��Բ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void BaseLinesNode::GenerateConCricles()
{
	int CircleNum = int(m_fLineDistance /(m_fLineStep*2));//ͬ��Բ�ĸ���=ֱ��/(�뾶����*2)
	int LineOneCircle = 30;//һ��ԲԤ��Ļ����ߵ�����Ĭ����30
	double radius = m_fLineStep;//�뾶����

	Line tempLine;
	switch (m_eLinePlaneType)
	{
	case XOY:
		for (int i = 0; i < LineOneCircle; i++)
		{
			for (int j = 0; j < CircleNum; j++)
			{
				tempLine.beginpt.x = (j + 1)*radius*sin(2.0*PI*i / LineOneCircle);
				tempLine.beginpt.y = (j + 1)*radius*cos(2.0*PI*i / LineOneCircle);
				tempLine.beginpt.z = 0;

				tempLine.endpt.x = (j + 1)*radius*sin(2.0*PI*(i + 1) / LineOneCircle);
				tempLine.endpt.y = (j + 1)*radius*cos(2.0*PI*(i + 1) / LineOneCircle);
				tempLine.endpt.z = 0;
				m_LineList.push_back(tempLine);
			}
		}
		break;
	case XOZ:
		for (int i = 0; i < LineOneCircle; i++)
		{
			for (int j = 0; j < CircleNum; j++)
			{
				tempLine.beginpt.x = (j + 1)*radius*sin(2.0*PI*i / LineOneCircle);
				tempLine.beginpt.y = 0;
				tempLine.beginpt.z = (j + 1)*radius*cos(2.0*PI*i / LineOneCircle);

				tempLine.endpt.x = (j + 1)*radius*sin(2.0*PI*(i + 1) / LineOneCircle);
				tempLine.endpt.y = 0;
				tempLine.endpt.z = (j + 1)*radius*cos(2.0*PI*(i + 1) / LineOneCircle);
				m_LineList.push_back(tempLine);
			}
		}
		break;
	case YOZ:
		for (int i = 0; i < LineOneCircle; i++)
		{
			for (int j = 0; j < CircleNum; j++)
			{
				tempLine.beginpt.x = 0;
				tempLine.beginpt.y = (j + 1)*radius*sin(2.0*PI*i / LineOneCircle);
				tempLine.beginpt.z = (j + 1)*radius*cos(2.0*PI*i / LineOneCircle);

				tempLine.endpt.x = 0;
				tempLine.endpt.y = (j + 1)*radius*sin(2.0*PI*(i + 1) / LineOneCircle);
				tempLine.endpt.z = (j + 1)*radius*cos(2.0*PI*(i + 1) / LineOneCircle);
				m_LineList.push_back(tempLine);
			}
		}
		break;
	case NONEVIEW:
		break;
	default://Ĭ��XOY
		for (int i = 0; i < LineOneCircle; i++)
		{
			for (int j = 0; j < CircleNum; j++)
			{
				tempLine.beginpt.x = (j + 1)*radius*sin(2.0*PI*i / LineOneCircle);
				tempLine.beginpt.y = (j + 1)*radius*cos(2.0*PI*i / LineOneCircle);
				tempLine.beginpt.z = 0;

				tempLine.endpt.x = (j + 1)*radius*sin(2.0*PI*(i + 1) / LineOneCircle);
				tempLine.endpt.y = (j + 1)*radius*cos(2.0*PI*(i + 1) / LineOneCircle);
				tempLine.endpt.z = 0;
				m_LineList.push_back(tempLine);
			}
		}
		break;
	}
}

/**************************************************************************
*  @Function Name :: GenerateGridLines
*  @Description   :: ����������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void BaseLinesNode::GenerateGridLines()
{
	int LineNum = int(m_fLineDistance / m_fLineStep) + 1;
	int i;
	float xMin, zMin, xMax, zMax;
	xMin = zMin =  -m_fLineDistance / 2;
	xMax = zMax = m_fLineDistance / 2;

	Line tempLine1, tempLine2;

	switch (m_eLinePlaneType)
	{
	case XOY:
		for (i = 0; i < LineNum; i++)
		{
			tempLine1.beginpt.x = xMin;
			tempLine1.beginpt.y = zMin + i * m_fLineStep;
			tempLine1.beginpt.z = 0;

			tempLine1.endpt.x = xMax;
			tempLine1.endpt.y = zMin + i * m_fLineStep;
			tempLine1.endpt.z = 0;

			tempLine2.beginpt.x = xMin + i * m_fLineStep;
			tempLine2.beginpt.y = zMin;
			tempLine2.beginpt.z = 0;

			tempLine2.endpt.x = xMin + i * m_fLineStep;
			tempLine2.endpt.y = zMax;
			tempLine2.endpt.z = 0;

			m_LineList.push_back(tempLine1);
			m_LineList.push_back(tempLine2);
		}
		break;
	case XOZ:
		for (i = 0; i < LineNum; i++)
		{
			tempLine1.beginpt.x = xMin;
			tempLine1.beginpt.y = 0;
			tempLine1.beginpt.z = zMin + i * m_fLineStep;

			tempLine1.endpt.x = xMax;
			tempLine1.endpt.y = 0;
			tempLine1.endpt.z = zMin + i * m_fLineStep;

			tempLine2.beginpt.x = xMin + i * m_fLineStep;
			tempLine2.beginpt.y = 0;
			tempLine2.beginpt.z = zMin;

			tempLine2.endpt.x = xMin + i * m_fLineStep;
			tempLine2.endpt.y = 0;
			tempLine2.endpt.z = zMax;

			m_LineList.push_back(tempLine1);
			m_LineList.push_back(tempLine2);
		}
		break;
	case YOZ:
		for (i = 0; i < LineNum; i++)
		{
			tempLine1.beginpt.x = 0;
			tempLine1.beginpt.y = xMin;
			tempLine1.beginpt.z = zMin + i * m_fLineStep;

			tempLine1.endpt.x = 0;
			tempLine1.endpt.y = xMax;
			tempLine1.endpt.z = zMin + i * m_fLineStep;

			tempLine2.beginpt.x = 0;
			tempLine2.beginpt.y = xMin + i * m_fLineStep;
			tempLine2.beginpt.z = zMin;

			tempLine2.endpt.x = 0;
			tempLine2.endpt.y = xMin + i * m_fLineStep;
			tempLine2.endpt.z = zMax;

			m_LineList.push_back(tempLine1);
			m_LineList.push_back(tempLine2);
		}
		break;
	case NONEVIEW:
		break;
	default://Ĭ��XOY
		for (i = 0; i < LineNum; i++)
		{
			tempLine1.beginpt.x = xMin;
			tempLine1.beginpt.y = zMin + i * m_fLineStep;
			tempLine1.beginpt.z = 0;

			tempLine1.endpt.x = xMax;
			tempLine1.endpt.y = zMin + i * m_fLineStep;
			tempLine1.endpt.z = 0;

			tempLine2.beginpt.x = xMin + i * m_fLineStep;
			tempLine2.beginpt.y = zMin;
			tempLine2.beginpt.z = 0;

			tempLine2.endpt.x = xMin + i * m_fLineStep;
			tempLine2.endpt.y = zMax;
			tempLine2.endpt.z = 0;

			m_LineList.push_back(tempLine1);
			m_LineList.push_back(tempLine2);
		}
		break;
	}

}

/**************************************************************************
*  @Function Name :: SetIsShowed
*  @Description   :: �����������Ƿ���ʾ
*  @Author        :: zzw
*  @Parameters    :: setIsDraw:���������Ƿ���ʾ�ı�־λ
*
*  @Return        :: None
**************************************************************************/
void BaseLinesNode::SetIsShowed(bool setIsDraw)
{
	if (false == setIsDraw)
	{
		m_eLinePlaneType = NONEVIEW;
	}
	else
	{
		m_eLinePlaneType = XOY;
	}
}

/**************************************************************************
*  @Function Name :: GetIsShowed
*  @Description   :: ��ȡ�������Ƿ���ʾ��־λ
*  @Author        :: zzw
*  @Parameters    :: setIsDraw:�������Ƿ���ʾ�ı�־λ
*
*  @Return        :: None
**************************************************************************/
void BaseLinesNode::GetIsShowed(bool &setIsDraw)
{
	if (NONEVIEW == m_eLinePlaneType)
	{
		setIsDraw = false;
	}
	else
	{
		setIsDraw = true;
	}
}