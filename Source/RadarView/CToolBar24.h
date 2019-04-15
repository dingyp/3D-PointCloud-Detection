#pragma once
// EnBitmap.h: CEnBitmap类的接口
#if _MSC_VER > 1000
#pragma once
#endif
#include "CEditVC.h"

class CToolBar24 : public CToolBar
{
	DECLARE_DYNAMIC(CToolBar24)
public:
	CToolBar24();
	virtual ~CToolBar24();

	BOOL Init(UINT BITMap_ID, UINT Disabled_ID, UINT Hot_ID);		//对Toolbar的状态进行初始化
	void UpdateColorImage(UINT BITMap_ID, UINT Disabled_ID, UINT Hot_ID, int idx);
	void UpdateColorImage(UINT BITMap_ID, UINT Disabled_ID, int idx);//更换工具栏中第idx个按钮的图像为bitmap中第idx个图像

protected:
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);		//工具栏位置发生变化的响应函数
	DECLARE_MESSAGE_MAP()

public:
	RECT rect;														//工具栏的位置

protected:
	CImageList m_imgToolbar;										//一般状态下工具栏显示图列表
	CImageList m_imgToolbarDis;										//disable状态下工具栏显示图列表
	CImageList m_imgToolbarNA;										//热键状态下工具栏显示图列表
	CBitmap m_bmToolbar;											//一般状态下工具栏位图
	CBitmap m_bmToolbarDis;											//disable状态下工具栏位图
	CBitmap m_bmToolbarNA;											//热键状态下工具栏位图

	BOOL m_bMenuRemoved;											//工具栏是否移动过标志位
};

//播放工具栏
class CPlayToolBar :public CToolBar24
{
public:
	CPlayToolBar();
	~CPlayToolBar();

	BOOL InitSlider(UINT ID_SLIDER, UINT ID_FrameEdit);				//对PlayToolbar的slider进行初始化
	void SetFrameNumber(UINT frameNumber);							//设置显示框和滑动条中帧数表达
	void InitFrameNumber(UINT totalFrameNumber);					//初始化总帧数，让滑动条可以进行滑动
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);//播放的进度条被调整
	DECLARE_MESSAGE_MAP()
private:
	CSliderCtrl playSlider;											//滑动进度条
	CEditVC frameEdit;												//帧数显示框
};

//颜色工具栏
class CColorToolBar :public CToolBar24
{
public:
	CColorToolBar();
	~CColorToolBar();

	BOOL InitColorAttribute(UINT ID_Attribute);						//初始化颜色属性
	void SetAttriComBoxCurSel(int setSel, bool bUpdate=false);		//设置AttributeComBox的值,update表示是否redraw
	afx_msg void OnSelchangeCombo();								//combobox的响应事件
	DECLARE_MESSAGE_MAP()
private:
	CComboBox AttributeComBox;										//属性下拉框
};