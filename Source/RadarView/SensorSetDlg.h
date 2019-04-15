#pragma once
#include "stdafx.h"
#include "SensorAttribute.h"

using namespace DataDefinition;

// SensorSet �Ի���
class SensorSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SensorSetDlg)

public:
	SensorSetDlg(SensorAttribute* sensor_attri,int Type, CWnd* pParent = nullptr);   // ��׼���캯��
	virtual ~SensorSetDlg();

protected:
	SensorSetDlg(UINT nIDTemplate, SensorAttribute* sensor_attri, CWnd* pParent = nullptr);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	BOOL OnInitDialog();
	SensorAttribute* m_pSensor;
	int m_iShowType;									//��ʾ����

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
};