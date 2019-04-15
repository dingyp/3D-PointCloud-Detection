#pragma once

//该类可以实现CEdit的水平居中，用于播放工具栏里面帧数的显示

class CEditVC : public CEdit
{
	DECLARE_DYNAMIC(CEditVC)
public:
	CEditVC();																		//CEditVC类的构造函数
	virtual ~CEditVC();																//CEditVC类的析构函数

protected:
	CRect m_rcClient;																//记录红色的框

protected:
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);	//求出字体的边框宽度
	afx_msg void OnNcPaint();														//实现绘制窗架
	afx_msg UINT OnGetDlgCode();													//调用控件，以便控件可以处理鼠标和tab键的输入
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);							//CEditVC类在create函数被调用的时候调用

	DECLARE_MESSAGE_MAP()

	//使用该函数获得所选字体中指定字符串的高度和宽度
	BOOL GetTextExtent(HDC hDC, LPCSTR lpString, int nCount, HFONT hFont, LPSIZE lpSize, int iDir = 0);
};

