#include "stdafx.h"
#include "MeasureLineNode.h"

/**************************************************************************
*  @Function Name :: MeasureLineNode
*  @Description   :: MeasureLineNode���Ĭ�Ϲ��캯��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
MeasureLineNode::MeasureLineNode()
{
	m_text = NULL;
	m_Line = new PathNode;

	this->AddChild(m_Line);
}

/**************************************************************************
*  @Function Name :: MeasureLineNode
*  @Description   :: MeasureLineNode��Ĵ���TextListID�Ĺ��캯��
*  @Author        :: zzw
*  @Parameters    :: TextListID:textlist��ID��
*
*  @Return        :: None
**************************************************************************/
MeasureLineNode::MeasureLineNode(UINT TextListID)
{
	m_Line = new PathNode;
	m_text = new TextNode(TextListID);
	this->AddChild(m_Line);
	this->AddChild(m_text);
}

/**************************************************************************
*  @Function Name :: ~MeasureLineNode
*  @Description   :: MeasureLineNode�����������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
MeasureLineNode::~MeasureLineNode()
{
}

/**************************************************************************
*  @Function Name :: RotateText
*  @Description   :: ����text����ת����
*  @Author        :: zzw
*  @Parameters    :: angle:��ת�Ƕ� x:xֵ
*					 y:yֵ z:zֵ
*  @Return        :: None
**************************************************************************/
void MeasureLineNode::RotateText(float angle, float x, float y, float z)
{
	if (m_text != NULL)
	{
		m_text->Rotate(angle, x, y, z);
	}

}

/**************************************************************************
*  @Function Name :: Draw
*  @Description   :: ���Ʋ�����ֱ��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void MeasureLineNode::Draw()
{
	if (FALSE == IsShowed())
		return;

	glPushMatrix();
	LoadMatrix();	// Translate, Rotate, Scale

	m_Line->Draw();
	if (m_text != NULL)
	{
		m_text->Draw();

	}

	glPopMatrix();
}

/**************************************************************************
*  @Function Name :: SetBeginPoint
*  @Description   :: ���ò���ֱ�ߵ���ʼ��
*  @Author        :: zzw
*  @Parameters    :: beginp:��ʼ������
*
*  @Return        :: None
**************************************************************************/
void MeasureLineNode::SetBeginPoint(VECTOR beginp)
{
	m_Line->SetBeginPoint(beginp);
}

/**************************************************************************
*  @Function Name :: SetEndPoint
*  @Description   :: ���ò���ֱ�ߵ����˵�
*  @Author        :: zzw
*  @Parameters    :: endp:���˵������
*
*  @Return        :: None
**************************************************************************/
void MeasureLineNode::SetEndPoint(VECTOR endp)
{
	m_Line->SetEndPoint(endp);
}

/**************************************************************************
*  @Function Name :: SetLineWidth
*  @Description   :: ����ֱ�ߵĿ��
*  @Author        :: zzw
*  @Parameters    :: width:ֱ�ߵĿ��
*
*  @Return        :: None
**************************************************************************/
void MeasureLineNode::SetLineWidth(float width)
{
	m_Line->SetLineWidth(width);
}

/**************************************************************************
*  @Function Name :: SetLineColor
*  @Description   :: ����ֱ�ߵ���ɫ
*  @Author        :: zzw
*  @Parameters    :: red:ֱ����ɫ�ĺ�ɫ���� green:ֱ����ɫ����ɫ����
*					 blue:ֱ����ɫ����ɫ����
*  @Return        :: None
**************************************************************************/
void MeasureLineNode::SetLineColor(float red, float green, float blue)
{
	m_Line->SetLineColor(red, green, blue);
}

/**************************************************************************
*  @Function Name :: SetText
*  @Description   :: ���������ı�
*  @Author        :: zzw
*  @Parameters    :: str:���屻���õ��ı�
*
*  @Return        :: None
**************************************************************************/
void MeasureLineNode::SetText(CString str)
{
	if (m_text == NULL) return;
	m_text->SetText(str);
}

/**************************************************************************
*  @Function Name :: SetTextPostion
*  @Description   :: �����ı���λ��
*  @Author        :: zzw
*  @Parameters    :: px:�ı�λ�õ�x���� py:�ı�λ�õ�y����
*					 pz:�ı�λ�õ�z����
*  @Return        :: None
**************************************************************************/
void MeasureLineNode::SetTextPostion(float px, float py, float pz)
{
	if (m_text == NULL) return;
	m_text->setPosition(px, py, pz);
}

/**************************************************************************
*  @Function Name :: SetTextColor
*  @Description   :: �����ı�����ɫ
*  @Author        :: zzw
*  @Parameters    :: red:�ı���ɫ�ĺ�ɫ���� green:�ı���ɫ����ɫ����
*					 blue:�ı���ɫ����ɫ����
*  @Return        :: None
**************************************************************************/
void MeasureLineNode::SetTextColor(float red, float green, float blue)
{
	if (m_text == NULL) return;
	m_text->SetTextColor(red, green, blue);
}

/**************************************************************************
*  @Function Name :: SetTextSize
*  @Description   :: �����ı��ĳߴ�
*  @Author        :: zzw
*  @Parameters    :: sx:�ı���x����Ĵ�С sy:�ı���y����Ĵ�С
*					 sz:�ı���z����Ĵ�С
*  @Return        :: None
**************************************************************************/
void MeasureLineNode::SetTextSize(float sx, float sy, float sz)
{
	if (m_text == NULL) return;
	m_text->Scale(sx, sy, sz);
}