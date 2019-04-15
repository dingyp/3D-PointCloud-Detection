#pragma once

class DataShowDlg : public CDialogBar
{
	DECLARE_DYNAMIC(DataShowDlg)
public:
	DataShowDlg(CWnd* pParent =NULL);
	~DataShowDlg();

	void ShowInitGrid();																			//展示初始的表格
	void SetShowValue(int PropertyNumber, int SubItemNumber, CString setValue);						//更改显示内容
	void SetShowValue(int PropertyNumber, int SubItemNumber, int setValue);							//重载更改显示内容
	void SetShowValue(int PropertyNumber, int SubItemNumber, double setValue);						//重载更改显示内容
	void AddGridProperty(CMFCPropertyGridProperty *set_pProp, int PropertyNumber, int SubItemNumber);		//将pProp加到第PropertyNumber的SubItemNumber里面
	void DelGridProperty(int PropertyNumber, int SubItemNumber);									//删除里面内容

protected:
	BOOL m_bMenuRemoved;																			//窗口是否移动过标志位

	virtual void DoDataExchange(CDataExchange* pDX);												// DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);										//窗口位置发生变化的响应函数
	afx_msg void OnSize(UINT nType, int cx, int cy);												//控制其内部控件的大小

	//afx_msg void OnButton();
private:
};

