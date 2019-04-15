#pragma once
#include "sceneobjectnode.h"

class AFX_EXT_CLASS HalfSphereNode :	public SceneObjectNode
{
public:
	HalfSphereNode(void);
	virtual ~HalfSphereNode(void);

	// ��������������뾶���ȷ���
	void SetSphere(float radius, int slices=32); 

protected:
	virtual  void   DrawObject(void); //��Բ�ľ�����ƺ���
	void drawHalfSphere(float radius, int numMajor, int numMinor);

protected:
	float	m_fRadius;		// �뾶
	int		m_iSlices;		// ��γ�ߵȷ���

};
