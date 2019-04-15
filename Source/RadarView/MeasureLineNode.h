#pragma once
#include "../SceneGraph/SceneGroup.h"
#include "../SceneGraph/PathNode.h"

class MeasureLineNode :public SceneGroup
{
public:

	MeasureLineNode(void);											//MeasureLineNode类的默认构造函数
	MeasureLineNode(UINT TextListID);								//MeasureLineNode类的带有TextListID的构造函数
	virtual ~MeasureLineNode();										//MeasureLineNode类的析构函数
	virtual void Draw();											//绘制测量的直线
	void SetBeginPoint(VECTOR beginp);								//设置测量直线的起始点
	void SetEndPoint(VECTOR endp);									//设置测量直线的终了点
	void SetLineWidth(float width);									//设置直线的宽度
	void SetLineColor(float red, float green, float blue);			//设置直线的颜色
	void SetText(CString str);										//设置字体文本
	//该Postion是相对于Sphere的位置
	void SetTextPostion(float px, float py, float pz);				//设置文本的位置
	void SetTextColor(float red, float green, float blue);			//设置文本的颜色
	void SetTextSize(float sx, float sy, float sz);					//设置文本的尺寸
	void RotateText(float angle, float x, float y, float z);		//设置text的旋转属性

protected:
	PathNode * m_Line;												//测量的直线
	TextNode * m_text;												//测量距离的文本
};

