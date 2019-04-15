#pragma once
#include "..\SceneGraph\SceneObjectNode.h"
#include "StructDefine.h"

class BaseLinesNode :	public SceneObjectNode
{
public:
	BaseLinesNode();
	virtual ~BaseLinesNode();
	virtual void Draw();													//绘制网格函数
	void SetLineWidth(float LineWidth) { m_fLineWidth = LineWidth; }		//设置网线的粗细
	void SetGridCenter(float SetX, float SetY, float SetZ);					//设置网格的中心
	void SetGridColor(VECTOR SetGridColor);									//设置网格颜色
	void SetLineParameters(float LineDistance, float LineStep);				//设置网格大小
	void SetLinePlane(LINEPLANE_TYPE eLinePlaneType);						//设置网格朝向
	void SetLineDraw(LINEDRAW_TYPE eLineDrawType);							//设置网格种类
	void SetIsShowed(bool setIsDraw);										//设置网格是否显示
	void GetIsShowed(bool &setIsDraw);										//获取网格是否显示标志位

protected:
	void DrawLines();														//绘制网线
	void GenerateGridLines();												//生成格子线
	void GenerateConCricles();												//生成同心圆

protected:
	VECTOR m_vColor;														//网线颜色
	float  m_fLineWidth;													//网线的宽度
	float  m_fLineDistance;													//网线框的边长
	float  m_fLineStep;														//网线的间隔
	LineArray m_LineList;													//网线的列表
	LINEPLANE_TYPE m_eLinePlaneType;										//网格朝向
	LINEDRAW_TYPE m_eLineDrawType;											//网格类型
	float addx, addy, addz;													//网格的中心坐标
};

