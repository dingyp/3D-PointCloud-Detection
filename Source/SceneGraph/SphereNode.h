#pragma once
#include "SceneObjectNode.h"

class AFX_EXT_CLASS SphereNode :public SceneObjectNode
{
public:
	SphereNode(void);
	virtual ~SphereNode(void);
	virtual	int getType(){return SPHERE_NODE;}

	// ��������������뾶���ȷ���
	void SetSphere(float radius, int slices=32); 
	void SetColor(float red,float green,float blue);//����������ɫ

	void GetSphereRadius(float *radius);

protected:

	virtual  void   DrawObject(void); //����ľ�����ƺ������ڸ�����Ϊ�գ��ڼ̳����о���ʵ��
	void	drawSphere(float radius, int numMajor=16, int numMinor=16);

protected:
	float	m_fRadius;		// �뾶
	int		m_iSlices;		// ��γ�ߵȷ���

	float   m_color[3];     //�������ɫ
};
