#pragma once
// EnBitmap.h: CEnBitmap类的接口

class CToolBar24 : public CToolBar
{
	typedef struct
	{
		UINT Width, Height, Colors;
		LPBYTE lpBits;
		DWORD dwNumBytes;
		LPBITMAPINFO lpbi;
		LPBYTE lpXOR;
		LPBYTE lpAND;
	}ICONIMAGE, *LPICONIMAGE;

	DECLARE_DYNAMIC(CToolBar24)
public:
	CToolBar24();
	virtual ~CToolBar24();

	BOOL Init(UINT BITMap_ID, UINT Disabled_ID, UINT Hot_ID);		//对Toolbar的状态进行初始化
	void UpdateColorImage(UINT Icon_ID, UINT Disabled_ID, UINT Hot_ID, int idx);
	void UpdateColorImage(UINT BITMap_ID, UINT Disabled_ID, int idx);//更换工具栏中第idx个按钮的图像为bitmap中第idx个图像

protected:
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
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