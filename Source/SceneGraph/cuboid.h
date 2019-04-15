#pragma once
#include "sceneobjectnode.h"

class AFX_EXT_CLASS Cuboid : public SceneObjectNode
{
public:
	Cuboid(void);
	~Cuboid(void);
	void SetSize(float length, float width, float height);
	void SetColor(float red , float green, float blue);
	virtual	int getType(){return CUBOID_NODE;}

private:
	static	VECTOR * m_pUnitVertices; 
	static  unsigned int * m_pIndices;
	VECTOR  m_pVertices[24];
	VECTOR  m_pNormals[8];

protected:
	virtual  void   DrawObject(void); //����ľ�����ƺ������ڸ�����Ϊ�գ��ڼ̳����о���ʵ��

protected:
	float m_fLength, m_fWidth, m_fHeight;
	VECTOR m_Color;

};
