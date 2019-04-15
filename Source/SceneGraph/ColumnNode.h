#pragma once
#include "sceneobjectnode.h"

class AFX_EXT_CLASS ColumnNode :	public SceneObjectNode
{
public:
	ColumnNode(void);
	virtual ~ColumnNode(void);

	//����Բ�������
	void SetColumn(float radius,float height);

protected:
	virtual  void   DrawObject(void); //�������Բ����ĺ���
	void DrawSolidCylinder(float radius, float height); //����Բ���壬�������µ���

protected:
	float	m_fRadius;		// ����뾶
	float	m_fHeight;		// �߶ȣ�Y����
};
