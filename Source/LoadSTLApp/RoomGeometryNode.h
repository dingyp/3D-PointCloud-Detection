#pragma once
#include "..\SceneGraph\SceneObjectNode.h"
#include <Gl/GL.h>
#include "StructDefine.h"
#include "FilesLoadLib\obj.h"

class RoomGeometryNode : public SceneObjectNode
{
public:
	RoomGeometryNode();
	virtual ~RoomGeometryNode();
	virtual int getType(void) { return OBJ_GEOMETRY_NODE; }

	virtual void Draw();

	void AttachRoomModel(CModelList *pModellist);

private:
	void DrawRoom();
	void MyMaterial(GLenum mode, GLfloat * f, GLfloat alpha);
	void SelectMaterial(int i);

public:
	CModelList *m_pRoomModelList;
};

