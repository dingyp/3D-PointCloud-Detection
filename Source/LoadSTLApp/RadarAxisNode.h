#pragma once
#include "StructDefine.h"
#include "..\SceneGraph\SceneObjectNode.h"
#include <Gl/GL.h>


class RadarAxisNode : public SceneObjectNode
{
public:
	RadarAxisNode(UINT TextListID);
	virtual ~RadarAxisNode();
	virtual int getType(void) { return RADARAXIS_NODE; }//获得类型
	virtual void Draw();								//绘制坐标轴和文字
	void SetLineLength(double setLineLength);			//设置坐标轴的长度

protected:
	void DrawAxis();									//绘制坐标轴的牵引轴
	void DrawArrows();									//绘制坐标轴的箭头

private:
	void GenerateArrows();								//生成圆锥体
	void DestroyPointers();								//对坐标轴对象进行销毁

protected:
	UINT		m_TextListID;							//文本列表的ID
	float		m_fLineLenth;							//坐标轴的尺寸大小
	VECTOR		* m_pNormalsX;							//X法向量指针
	VECTOR		* m_pNormalsY;							//Y法向量指针
	VECTOR		* m_pNormalsZ;							//Z法向量指针
	VECTOR		* m_pVerticesX;							//X顶点数组指针
	VECTOR		* m_pVerticesY;							//Y顶点数组指针
	VECTOR		* m_pVerticesZ;							//Z顶点数组指针
	unsigned int  m_iTriNum;							//每个圆锥三角面数目
};
