
#pragma once

#include "Scene3DMessageWnd.h"


// Scene3DWnd, ������ڹ���������Ѿ����
class AFX_EXT_CLASS Scene3DWnd : public Scene3DMessageWnd
{
	DECLARE_DYNAMIC(Scene3DWnd)

public:
	Scene3DWnd();
	virtual ~Scene3DWnd();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	void   SetLBTInteractionMode(InteractionMode mode); //09-7-17�����ý���ģʽ
	void   SetRBTInteractionMode(InteractionMode mode); //09-7-17�����ý���ģʽ
	void   SetMBTInteractionMode(InteractionMode mode); //09-7-17�����ý���ģʽ


protected:	
	InteractionMode m_eLBTInterationMode; //�������ģʽ
	InteractionMode m_eRBTInterationMode; //�Ҽ�����ģʽ
	InteractionMode m_eMBTInterationMode; //�м������ģʽ

	BOOL m_bLBtDown;
	BOOL m_bRBtDown;
	BOOL m_bMBtDown;

};


