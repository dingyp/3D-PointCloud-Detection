#pragma once
#include "stdafx.h"
#include "SensorAttribute.h"

using namespace DataDefinition;

// SensorSet 对话框
class SensorSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SensorSetDlg)

public:
	SensorSetDlg(SensorAttribute* sensor_attri,int Type, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~SensorSetDlg();

protected:
	SensorSetDlg(UINT nIDTemplate, SensorAttribute* sensor_attri, CWnd* pParent = nullptr);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL OnInitDialog();
	SensorAttribute* m_pSensor;
	int m_iShowType;									//显示类型

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
};