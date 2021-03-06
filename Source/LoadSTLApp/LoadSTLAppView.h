
// LoadSTLAppView.h: CLoadSTLAppView 类的接口
//

#pragma once
#include "LoadSTLAppDoc.h"
#include "PointsAndSTL3DWnd.h"

class CSimplifyEXEDoc;

#define MY_MSG_FullOneFrame WM_USER+1100
#define MY_MSG_EndPcapFresh WM_USER+1101

class CLoadSTLAppView : public CView
{
protected: // 仅从序列化创建
	CLoadSTLAppView();
	DECLARE_DYNCREATE(CLoadSTLAppView)

// 特性
public:
	CLoadSTLAppDoc * GetDocument() const;

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	PointsAndSTL3DWnd *GetShow3DWnd() { return m_pPointAndSTL3DWnd; }
	void ClearFrameShow();

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CLoadSTLAppView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	PointsAndSTL3DWnd *m_pPointAndSTL3DWnd;
	int m_iWndCX, m_iWndCY;

protected:
	void ArrangeWnd();

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg LRESULT GetOneFrame(WPARAM wParam, LPARAM lParam);			//满一帧的事件响应函数
	afx_msg LRESULT EndPcapfresh(WPARAM wParam, LPARAM lParam);			//文件读取到最后的响应函数
};

#ifndef _DEBUG  // LoadSTLAppView.cpp 中的调试版本
inline CLoadSTLAppDoc* CLoadSTLAppView::GetDocument() const
   { return reinterpret_cast<CLoadSTLAppDoc*>(m_pDocument); }
#endif

