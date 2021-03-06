#pragma once
#include "StructDefine.h"

// CCOLORLEGENDDlg 对话框

class ColorLegendDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ColorLegendDlg)

public:
	ColorLegendDlg(CWnd* pParent = nullptr);										// 标准构造函数
	virtual ~ColorLegendDlg();

	//在显示之前设置尺度属性颜色下标、属性下标、数据范围
	void SetAttribute(int colorIdx, COLORCODING ColorCodingIdx, double setMin, double setMax);//设置颜色标尺的属性
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COLORLEGEND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);								// DDX/DDV 支持

	virtual BOOL OnInitDialog();
	void SetScaleTXT(CDC* pDC,UINT m_ID, double data);								//设置标尺显示的字

	afx_msg LRESULT OnNcHitTest(CPoint pt);											//鼠标点击非标题栏区拖动MFC对话框
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);				//改变控件的颜色
	afx_msg void OnMoving(UINT nSide, LPRECT lpRect);								//移动过程中对颜色标尺移动范围进行限定
	DECLARE_MESSAGE_MAP()
private:
	CStatic m_colorLegend;															//颜色标尺控件
	CStatic m_txtName;																//颜色标尺标题
	CStatic m_txtUp;																//颜色标尺开始尺度
	CStatic m_txtQua;																//颜色标尺四分之一尺度
	CStatic m_txtMid;																//颜色标尺中间尺度
	CStatic m_txtBQua;																//颜色标尺四分之三尺度
	CStatic m_txtbottom;															//颜色标尺结束尺度

	int colorIdx;																	//颜色编码号
	COLORCODING AttrIdx;															//参加编码的属性
	double legendMax;																//标尺的最大值
	double legendMin;																//标尺的最小值
};
