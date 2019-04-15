#pragma once
#include "SceneGroup.h"
#include "TextNode.h"
#include "SphereNode.h"

class AFX_EXT_CLASS MarkNode :public SceneGroup
{
public:
	MarkNode(void);
	MarkNode(UINT TextListID);
	virtual ~MarkNode(void);

	virtual	int getType(){return MARK_NODE;}

	virtual void Draw();
	void SetText(const char *str);
	//��Postion�������Sphere��λ��
	void    SetTextPostion(float px,float py,float pz);
	void	SetTextColor(float red, float green, float blue);
	void	SetTextSize(float sx, float sy, float sz); 

	// ��������������뾶���ȷ���
	void SetSphere(float radius, int slices=32); 
	void SetSphereColor(float red,float green,float blue);//����������ɫ

protected:
	SphereNode * m_sphere;
	TextNode * m_text;
};
