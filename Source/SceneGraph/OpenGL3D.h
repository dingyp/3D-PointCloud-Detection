#pragma once
#include "opengl.h"
#include "camera.h"
#include "Trackball.h"
#include "scenegroup.h"
#include "resource.h"
#include "Enum.h"
#include "MouseSelect.h"

class AFX_EXT_CLASS COpenGL3D :	public COpenGL
{
public:
	COpenGL3D(void);
	~COpenGL3D(void);

public:
	//�麯��
	virtual void Draw();
	virtual void DrawBoundBox(); //���ư�Χ��
	virtual void OpenGLSize(int cx, int cy);
	virtual BOOL initOpenGL(HWND hWnd);
	virtual void DestroyOpenGL(void);

	void setSceneGraph(SceneGroup * root);
	BOOL IsSceneSeted(){return m_SceneSeted;}
	// ���á���ȡ������ɫ
	void setClearColor(float red,float green,float blue,float alpha=1.0f);
	void getClearColor(float *red, float *green, float *blue, float *alpha);

	//��Ϣ��Ӧ��������trackball�й�
	afx_msg void OnButtonDown(CPoint point);
	afx_msg void OnButtonUp(CPoint point);
	afx_msg void OnMouseMove(CPoint point);
	afx_msg void OnMouseWheel(short zDelta, CPoint pt);

	// ��������
	afx_msg void OnMouseSelect(void);			// ʰȡͼ�Σ�Ҷ�ڵ㣩
	afx_msg void OnMouseRotate(void);			// ��ԭ�����ⷽ����ת
	afx_msg void OnMouseTranslate(void);        //ƽ�ƣ�ѡ�нڵ㣩
	afx_msg void OnUpdateMouseSelect(CCmdUI* pCmdUI);	
	afx_msg void OnUpdateMouseRotate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMouseTranslate(CCmdUI *pCmdUI);

	// ���á���ȡ��꽻���ķ�ʽ
	void setMouseInteraction(int  type);
	int  getMouseInteraction(void);

	void setMouseTranslateSteplength(float length) {m_fMouseTranslateSteplength = length;}

	// ʰȡ��ǰ������ĳ���ڵ�
	void selectNode(SceneNode * node);
	void selectNode(char *name);
	// ȡ����ʰȡ�Ľڵ�
	void UnselectNode(void);

	// ����˫����ձ�־
	void setTwoSideLight(BOOL twoside);

	SceneGroup * getRoot() {return m_pSceneRoot;}

protected:
	void initializeGL(void);

public:
	CPoint  m_Point;   //���ղ����ڵ�λ��
	Matrix  m_matRotation; //��ת����
	float	m_fClearColor[4];	// ������ɫ

	int		m_iMouseInteraction;	// ��꽻���ķ�ʽ
	int		m_iMouseX, m_iMouseY;	// ��������
	BOOL	m_bButtonDown;		// ��갴��
	float     m_fMouseTranslateSteplength; //���ƽ�Ʋ�����ȱʡΪ2

	float   m_fScale; //���ű���
	float   m_fScaleDelta; //���ŵĲ���

	BOOL	m_bNodeSelected;	// �Ƿ�ĳһ�ڵ��ѱ�ʰȡ
	SceneNode *m_pSelectedNode;	// �ѱ�ʰȡ�Ľڵ�
	char	m_strSelectedNodeName[NODE_NAME_LENGTH]; // �ѱ�ʰȡ�Ľڵ�����

	Camera m_Camera;
	Camera *getCamera(){return &m_Camera;}

	int m_iViewportHeight;
	int m_iViewportWidth;
	BOOL m_SceneSeted;

	BOOL m_bTwoSideLightUsed; //�Ƿ�ʹ��˫����ձ�־

protected:
	SceneGroup  * m_pSceneRoot;
};
