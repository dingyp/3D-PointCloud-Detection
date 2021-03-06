
// MainFrm.h: CMainFrame 类的接口
//

#pragma once

#include "CToolBar24.h"
#include "DataShowDlg.h"

class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 特性
public:
	bool Get_PCAPFILEStatus()const { return m_pcapFILEStatus; };						//得到pcap文件状态
	void Set_PCAPFILEStatus(bool Set_PCAPStatus);										//设置pcap文件状态
	bool Get_STLFILEStatus()const { return m_stlFILEStatus; };							//得到stl文件状态
	void Set_STLFILEStatus(bool Set_STLStatus);											//设置stl文件状态
	bool Get_OBJFILEStatus()const { return m_objFILEStatus; };							//得到obj文件状态
	void Set_OBJFILEStatus(bool Set_OBJStatus);											//设置obj文件状态
	bool Get_CalculationStatus()const { return m_calculationStatus; };					//得到计算状态
	void Set_CalculationStatus(bool Set_calStatus);										//设置计算状态
	bool Get_ShowDataGrid()const { return m_showDataGrid; };							//得到显示属性网格
	void Set_ShowDataGrid(bool Set_showDataStatus);										//设置显示属性网格

	void ReDrawShow();																	//刷新界面显示
	void SetShowValue(int PropertyNumber, int SubItemNumber, CString setValue);			//更改显示内容
	void SetShowValue(int PropertyNumber, int SubItemNumber, int setValue);
	void SetShowValue(int PropertyNumber, int SubItemNumber, double setValue);

	void AddGridProperty(CMFCPropertyGridProperty *pProp, int PropertyNumber, int SubItemNumber);		//将pProp加到第PropertyNumber的SubItemNumber里面
	void DelGridProperty(int PropertyNumber, int SubItemNumber);									//删除里面内容
// 操作
public:
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CToolBar24 m_wndToolBar;															//工具栏
	CStatusBar m_wndStatusBar;															//状态栏
	DataShowDlg m_datashowDlg;															//控制条形式的非模态对话框

	bool m_stlFILEStatus;																//stl文件打开状态
	bool m_pcapFILEStatus;																//pcap文件打开状态
	bool m_objFILEStatus;																//obj文件打开状态
	bool m_calculationStatus;															//计算状态
	bool m_showDataGrid;																//展示网格

// 生成的消息映射函数
protected:
	virtual void OnUpdateFrameTitle(BOOL NaDa);											//更新窗口标题
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//afx_msg void OnSize(UINT nType, int cx, int cy);					//设置初始时候的大小
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()

};


