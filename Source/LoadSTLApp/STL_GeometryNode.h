#pragma once
#include "..\SceneGraph\SceneObjectNode.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include "CDataList.h"
//#include "Vector.h"

using namespace DataDefinition;
class STL_GeometryNode :public SceneObjectNode
{
public:
	STL_GeometryNode();
	virtual ~STL_GeometryNode();
	virtual int getType(void) { return STL_GEOMETRY_NODE; }			//获得类型
	virtual void Draw();

	void SetPolygonMode(GLenum Mode);							//设置表面的绘制方式
	void AttachDataList(CDataList * pDataList);					//添加数据list,并得到框格大小

protected:
	GLenum m_ePolygonMode;										//表面的绘制方式
	CDataList *m_pDataList;										//数据list
};