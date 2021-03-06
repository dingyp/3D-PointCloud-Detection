#pragma once
// GridSetDlg 对话框

class GridSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(GridSetDlg)

public:
	GridSetDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~GridSetDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GRIDSET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();							//网格属性对话框的初始化函数
private:
	CSpinButtonCtrl m_SpinGridX;							//X的spin控件
	CSpinButtonCtrl m_SpinGridY;							//Y的spin控件
	CSpinButtonCtrl m_SpinGridZ;							//Z的spin控件

	CSpinButtonCtrl m_SpinGridLength;						//GridLength的spin控件
	CSpinButtonCtrl m_SpinGridResolution;					//GridResolution的spin控件
	CSpinButtonCtrl m_SpinGridRadius;						//GridRadius的spin控件
	CSpinButtonCtrl m_SpinGridConCircle;					//GridConCircle的spin控件
	CSpinButtonCtrl m_SpinGridWidth;						//GridWidth的spin控件

	CMFCColorButton m_GridColorPicker;						//网格颜色选择按钮

	CButton m_BaseLineCheckBox;								//是否显示网格勾选框
	CButton m_AxisNodeCheckBox;								//是否显示坐标轴勾选框
	CButton m_FilterCheckBox;								//是否滤波勾选框
public:
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);//点击spin的响应函数
	afx_msg void OnBnClickedOk();							//点击确定按钮的响应函数
	afx_msg void OnBnClickedReset();						//点击重置的响应函数
};
