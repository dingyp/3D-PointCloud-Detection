#include "stdafx.h"
#include "MeasureLineNode.h"

/**************************************************************************
*  @Function Name :: MeasureLineNode
*  @Description   :: MeasureLineNode类的默认构造函数
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
*  @Description   :: MeasureLineNode类的带有TextListID的构造函数
*  @Author        :: zzw
*  @Parameters    :: TextListID:textlist的ID号
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
*  @Description   :: MeasureLineNode类的析构函数
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
*  @Description   :: 设置text的旋转属性
*  @Author        :: zzw
*  @Parameters    :: angle:旋转角度 x:x值
*					 y:y值 z:z值
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
*  @Description   :: 绘制测量的直线
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
*  @Description   :: 设置测量直线的起始点
*  @Author        :: zzw
*  @Parameters    :: beginp:起始点坐标
*
*  @Return        :: None
**************************************************************************/
void MeasureLineNode::SetBeginPoint(VECTOR beginp)
{
	m_Line->SetBeginPoint(beginp);
}

/**************************************************************************
*  @Function Name :: SetEndPoint
*  @Description   :: 设置测量直线的终了点
*  @Author        :: zzw
*  @Parameters    :: endp:终了点的坐标
*
*  @Return        :: None
**************************************************************************/
void MeasureLineNode::SetEndPoint(VECTOR endp)
{
	m_Line->SetEndPoint(endp);
}

/**************************************************************************
*  @Function Name :: SetLineWidth
*  @Description   :: 设置直线的宽度
*  @Author        :: zzw
*  @Parameters    :: width:直线的宽度
*
*  @Return        :: None
**************************************************************************/
void MeasureLineNode::SetLineWidth(float width)
{
	m_Line->SetLineWidth(width);
}

/**************************************************************************
*  @Function Name :: SetLineColor
*  @Description   :: 设置直线的颜色
*  @Author        :: zzw
*  @Parameters    :: red:直线颜色的红色分量 green:直线颜色的绿色分量
*					 blue:直线颜色的蓝色分量
*  @Return        :: None
**************************************************************************/
void MeasureLineNode::SetLineColor(float red, float green, float blue)
{
	m_Line->SetLineColor(red, green, blue);
}

/**************************************************************************
*  @Function Name :: SetText
*  @Description   :: 设置字体文本
*  @Author        :: zzw
*  @Parameters    :: str:字体被设置的文本
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
*  @Description   :: 设置文本的位置
*  @Author        :: zzw
*  @Parameters    :: px:文本位置的x坐标 py:文本位置的y坐标
*					 pz:文本位置的z坐标
*  @Return        :: None
**************************************************************************/
void MeasureLineNode::SetTextPostion(float px, float py, float pz)
{
	if (m_text == NULL) return;
	m_text->setPosition(px, py, pz);
}

/**************************************************************************
*  @Function Name :: SetTextColor
*  @Description   :: 设置文本的颜色
*  @Author        :: zzw
*  @Parameters    :: red:文本颜色的红色分量 green:文本颜色的绿色分量
*					 blue:文本颜色的蓝色分量
*  @Return        :: None
**************************************************************************/
void MeasureLineNode::SetTextColor(float red, float green, float blue)
{
	if (m_text == NULL) return;
	m_text->SetTextColor(red, green, blue);
}

/**************************************************************************
*  @Function Name :: SetTextSize
*  @Description   :: 设置文本的尺寸
*  @Author        :: zzw
*  @Parameters    :: sx:文本在x方向的大小 sy:文本在y方向的大小
*					 sz:文本在z方向的大小
*  @Return        :: None
**************************************************************************/
void MeasureLineNode::SetTextSize(float sx, float sy, float sz)
{
	if (m_text == NULL) return;
	m_text->Scale(sx, sy, sz);
}