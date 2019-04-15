// SceneNode.h: interface for the SceneNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENENODE_H__7FDF7FF2_8043_48FA_AEE5_482FABE00E36__INCLUDED_)
#define AFX_SCENENODE_H__7FDF7FF2_8043_48FA_AEE5_482FABE00E36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Container.h"
#include "Enum.h"
#include "Vector.h"
#include "Rotation.h"
#include "Trackball.h"
#include "Matrix.h"

class AFX_EXT_CLASS SceneNode : public Container  
{
public:
 	SceneNode();
	virtual ~SceneNode();

public:
	virtual	int getType(){return SCENE_NODE_BASE;}

	// ������Ϊ name �Ľڵ�
	virtual SceneNode *Search(char *name);
	virtual void Draw(void)=0;
	// Ϊ Mouse Select �������Ƶ�ǰ�ڵ�
	virtual void DrawForMouseSelect(void){};

	void RotateAround(float angle, float nx, float ny, float nz,float refx, float refy, float refz);
	void RotateAround(float angle, VECTOR axis, VECTOR ref);

	void RotateInCameraAround(float angle, float nx, float ny, float nz,float refx, float refy, float refz);
	void RotateInCameraAround(float angle, VECTOR axis, VECTOR ref);

	void Rotate(float angle, float nx, float ny, float nz);
	void Rotate(float angle, VECTOR axis);

	// ���������ϵ�е���ת�任
	void RotateInCamera(float angle, VECTOR axis);
	void RotateInCamera(float angle, float nx, float ny, float nz);	

	void Translate(VECTOR trans);
	void Translate(float dx, float dy, float dz);

	// ���������ϵ�е�ƽ�Ʊ任
	void TranslateInCamera(VECTOR trans);
	void TranslateInCamera(float dx, float dy, float dz); 

	float getScaleStep(void);
	float getTranslateStep(void);
	float getRotateStep(void);
	void getScale(float *sx, float *sy, float *sz);
	void getRotation(float *angle, float *nx, float *ny, float *nz);
	void GetInverseRotation(float *angle,float *nx,float *ny,float *nz); //�õ��Ӹ���㵽�ýڵ����ת����֮��
	void getPosition(float *x,float *y,float *z);
	SceneNode *getParent(void){ return m_pParent; }
	void getName(char *name);
	BOOL getTransparent(); //����Ƿ�͸�����ԣ�ֻ��GeometryNode������

	void setScaleStep(float scalestep);
	void setTranslateStep(float transStep);
	void setRotateStep(float rotateStep);
	void setScale(float  sx, float  sy, float  sz);
	void setRotation(float  angle, float  nx, float  ny, float  nz);
	void setPosition(float x,float y,float z);
	void setParent(SceneNode *node){m_pParent=node;}
	void setName(const char *name);

	void Showed(void) {m_bShowed = TRUE;}  //��ǰ�ڵ���ʾ
	void UnShowed(void) {m_bShowed = FALSE;} //��ǰ�ڵ�����
	BOOL IsShowed(void) { return m_bShowed; } //��ʾ��ǰ�ڵ��Ƿ���ʾ

	void Scale(float sx, float sy, float sz);
	void ResetTransform(void);
	void ResetScale(void) { m_scale = m_oldScale;}
	void ResetRotation(void) { m_rotation = m_oldRotation;}
	void ResetPosition(void) { m_position = m_oldPosition;}
	void Selected(void){m_bSelected = TRUE;	}
	void UnSelected(void){m_bSelected = FALSE; }
	BOOL IsSelected(void) {return m_bSelected; } //��ʾ��ǰ�ڵ��Ƿ�ʰȡ

	// ����������ת���ڹ۲�����ϵ�У�2004��3��2
	//����������Ϊ����ԭ��
    void MouseTrackStart(int iViewportWidth, int iViewportHeight, int mx, int my);
	void MouseTracking(int dx, int dy); 

	// ���ڵ㶨���� World ����ϵ��
	// FWTO : From World To Object
	// FOTW : From Object To World 
	// FCTO : From Camera To Object
	// FOTC : From Object To Camera 

	void LoadMatrix(void);	// ִ�е�ǰ�ڵ�� Translate, Rotate, Scale
	void LoadInverseMatrix(void);

	void LoadMatrix_FWTO(void);
	void LoadMatrix_FOTW(void);

	void GetMatrix(float M[16]);		// ���٣���ȷ
	void GetInverseMatrix(float M[16]);	// ���٣���ȷ
	void GetMatrix_FWTO(float M[16]);	// ���٣���ȷ
	void GetMatrix_FOTW(float M[16]);	// ���٣���ȷ
	void GetMatrix_FCTO(float M[16]);	// ���٣���ȷ
	void GetMatrix_FOTC(float M[16]);	// �������㣬����������ȷ

	// �� Object ����ϵ�ĵ� Po(xo, yo, zo, wo)
	// �� World  ����ϵ�ĵ� Pw(xw, yw, zw, ww)
	// �� World  ����ϵ�� Object ����ϵ�ľ���Ϊ M_FWTO[16]
	// �� Object ����ϵ�� World  ����ϵ�ľ���Ϊ M_FOTW[16]
	// ������ϵ���� M_FWTO�� �� ������ϵ���� M_FOTW�� ����
	// ������ϵ���� �� ����ı任���� ����
	// ���� VertexTransform_FWTO ��ʾ  Po = Pw * M_FOTW
	// ���� VertexTransform_FOTW ��ʾ  Pw = Po * M_FWTO

	void VertexTransform_FWTO(float *x, float *y, float *z);
	void VertexTransform_FOTW(float *x, float *y, float *z);
	void VertexTransform_FCTO(float *x, float *y, float *z); // �������㣬����������ȷ
	void VertexTransform_FOTC(float *x, float *y, float *z); 
	//Ҫע�ⴰ��y������windows��opengl��ת���������Ӧ�ó�������ɣ��ƻۣ�04��9��29
	void VertexTransform_WindowToObject(float *x, float *y, float *z); // ����
	void VertexTransform_ObjectToWindow(float *x, float *y, float *z); // ����

	// �� V = P(x,y,z) - O(0,0,0)
	// �� V'= P' - O'
	// ���ڼ�����ת���
	void VectorTransform_FWTO(VECTOR *V);
	void VectorTransform_FOTW(VECTOR *V);
	void VectorTransform_FCTO(VECTOR *V);	// �������㣬����������ȷ
	void VectorTransform_FOTC(VECTOR *V); 

	void setBoundBox(float centerx,float centery,float centerz,float xrange,float yrange,float zrange); //���ð�Χ�еĴ�С
	void setShowBoundBox(BOOL bShow);
	void setBoundColor(float red, float green, float blue);

	virtual  void   DrawBoundBox(); //���ư�Χ�У��ڼ̳����п��Ը�����Ҫ�޸�
	virtual  void   ComputeBoundBox(void) {};//���ݶ������ݼ����Χ�У��ڸ�����BoundBoxֵȫΪ0

protected:
	char m_strNodeName[NODE_NAME_LENGTH];// ��ǰ�ڵ������

	SceneNode *m_pParent;

	VECTOR      m_position;			// ��ǰ�ڵ��ڸ��ڵ��е�λ�ò���
	ROTATION    m_rotation;			// ��ǰ�ڵ����ת�任����
	VECTOR      m_scale;			// ��ǰ�ڵ�ı����任����

	BOOL        m_bTransparent;   //�ýڵ��Ƿ�͸����־��ȱʡΪFALSE

	// ���� ResetTransform()
	VECTOR      m_oldPosition;		
	ROTATION    m_oldRotation;		
	VECTOR      m_oldScale;			

	float		m_fRotateStep;		// ��ǰ�ڵ���ת�任�Ĳ�������
	float		m_fTranslateStep;	// ��ǰ�ڵ�ƽ�Ʊ任�Ĳ�������
	float		m_fScaleStep;		// ��ǰ�ڵ�����任�Ĳ�������

	Trackball   m_trackball;   //��ǰ�ڵ��Trackball��Ϊʰȡ��Ըýڵ����ʹ��
	BOOL        m_bSelected;   // ��ǰ�ڵ㱻ʰȡ�ı�־
	BOOL        m_bShowed;     //��ǰ�ڵ��Ƿ�����Ļ����ʾ��־

	BOOL		m_bShowBoundBox;  //�Ƿ���ʾ��Χ�б�־��ȱʡΪFALSE��ѡ�и������ʱ���ΪTRUE
	BOUND_BOX   m_struBoundBox; //��Χ��
	VECTOR      m_struBoundColor; //��Χ����ɫ��ȱʡΪ��ɫ

};

#endif // !defined(AFX_SCENENODE_H__7FDF7FF2_8043_48FA_AEE5_482FABE00E36__INCLUDED_)
