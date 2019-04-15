// Camera.h: interface for the Camera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERA_H__A380E057_4627_463F_910B_8CD45B0F42A4__INCLUDED_)
#define AFX_CAMERA_H__A380E057_4627_463F_910B_8CD45B0F42A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Matrix.h"
#include "Enum.h"
#include "Vector.h"
#include "Rotation.h"

//�����ʹ��һ��ȫ�����
class AFX_EXT_CLASS Camera  
{
public:
	Camera();
	virtual ~Camera();

	//ʹ������������
	/*�����������Ӧ�ļ򵥺���*/
	Matrix m_matProj;   //ͶӰ����
	Matrix m_matView;   //�Ӿ�����
	Matrix m_matOldView; //ԭʼ�Ӿ�����Ϊ��Աʹ��
	Matrix getProjMat(){return m_matProj;}
	Matrix getViewMat(){return m_matView;}

	/*ͶӰ��������ͶӰ��صĺ���*/
	float m_fFovY;  //y������ӽǷ�Χ���Զ�Ϊ��λ
	float m_fAspect; //x��������ӳ��Ŀ��ȣ�����Ϊx������y������
	float m_fNearZ;  //�۲��ߵ����ü���ľ��루�����㣩
	float m_fFarZ;  //�۲��ߵ�Զ�ü���ľ��루�����㣩

	PROJ_MODE   m_eProjMode;
	void setProjMode(PROJ_MODE  mode) { m_eProjMode = mode;}

	void setAspect(float Aspect);
	void setProj(float FovY,float Aspect,float NearZ,float FarZ);
	void setProj(float left,float right,float bottom,float top,float nearZ,float farZ);

	/*��������������������������������
	View��������Ҫ��������ı任
	���ƹ̶�����ת��ƽ�Ƶ�
	��������������������������������*/
	VECTOR m_vEyePt; //������������ۣ�λ��
	VECTOR m_vLookatPt; //�����׼�ķ��򣨼�����������λ�ã�
	VECTOR m_vUpVec;  //����������򣨼�������ĸ�������������
	
	VECTOR m_vOldEyePt;//������������ۣ�ԭ��λ��
	VECTOR m_vOldLookatPt;//�����׼�ķ����ʼֵ��������������λ�ã�
	VECTOR m_vOldUpVec;//������������ʼֵ����������ĸ�������������

	void setView(VECTOR vEyePt, VECTOR vLookatPt, VECTOR vUpVec);
	void setView(float eyex,float eyey,float eyez,float centerx,float centery,float centerz,float upx,float upy,float upz);

	void Translate(float x,float y,float z); //����ƶ�������ӵ㲻�ƶ����������ǳ�����ת�ˣ�

	void Rotation (VECTOR vDirection, float Angle);//����ƹ̶�����ת
	void RotateX(float Angle); //�������������ϵ��X����ת
	void RotateY(float Angle); //�������������ϵ��Y����ת
	void RotateZ(float Angle); //�������������ϵ��Z����ת
	void RotateIncameraX(float Angle);//�������������ϵ��X����ת
	void RotateIncameraY(float Angle);//�������������ϵ��Y����ת
	void RotateIncameraZ(float Angle);//�������������ϵ��Z����ת
	void ReStoreView();          //�ָ�View��ԭʼ״̬
	void SaveViewState();        //���浱ǰView״̬

	void MultViewMatrix(const Matrix *M); //�����View�������һ���任����

};

#endif // !defined(AFX_CAMERA_H__A380E057_4627_463F_910B_8CD45B0F42A4__INCLUDED_)
