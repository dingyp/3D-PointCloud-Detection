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
	virtual int getType(void) { return STL_GEOMETRY_NODE; }			//�������
	virtual void Draw();

	void SetPolygonMode(GLenum Mode);							//���ñ���Ļ��Ʒ�ʽ
	void AttachDataList(CDataList * pDataList);					//�������list,���õ�����С

protected:
	GLenum m_ePolygonMode;										//����Ļ��Ʒ�ʽ
	CDataList *m_pDataList;										//����list
};