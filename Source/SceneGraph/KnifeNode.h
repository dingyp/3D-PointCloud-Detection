#pragma once
#include "GeometryNode.h"
#include "VectorFile.h"

class AFX_EXT_CLASS KnifeNode :public GeometryNode
{
public:
	KnifeNode(void);
	virtual ~KnifeNode(void);

	virtual void Draw();
	virtual	int getType(){return KNIFE_NODE;}

	void SetDirection(VECTOR direction);
	void SetDirection(float directionx, float directiony, float directionz);
	void SetKnifePoint(VECTOR knifepoint);
	void SetKnifePoint(float knifepointx, float knifepointy, float knifepointz);

	void GetDirection(VECTOR *direction);
	void GetDirection(float *directionx,float *directiony,float *directionz);
	void GetKnifePoint(VECTOR *knifepoint);
	void GetKnifePoint(float *knifepointx,float *knifepointy,float *knifepointz);

	void SetKnifeLength(float length);
	void GetKnifeLength(float *length);

	void SetKnifeColor(float red, float green, float blue);
	void GetKnifeColor(float *red, float *green,float *blue);

	void SetKnifeMoveStep(float step);
	void GetKnifeMoveStep(float *step);

	void KnifeForward();
	void KnifeBackward();

	int  LoadKnifeFile(const char *name);

protected:
	void DrawKnife();
	void DrawSolidCylinder(float radius, float height);
	void DrawSolidCone(float radius, float height);
    void DrawSphere(float radius, int numMajor, int numMinor);

protected:
	VECTOR m_vDirection; //��������ָ�򣬹�һ����
	VECTOR m_vKnifePoint; //����������λ��

	float m_fMoveStep;  //�������ƶ�����
	float m_fKnifeLength; //����������
	VECTOR m_vColor; //��������ɫ

	VECTOR m_vOrginalDir; //������ԭʼ����
	VECTOR m_vOrginalPoint; //������ԭʼ����λ��
	float  m_fOrginalLength; //������ԭʼ����

};
