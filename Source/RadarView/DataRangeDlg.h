#pragma once
#include "stdafx.h"

//��ʾ�������Է�Χ�ĶԻ���
class DataRangeDlg : public CDialogEx
{
public:
	DataRangeDlg(CWnd* pParent = nullptr);   // ��׼���캯��
	virtual ~DataRangeDlg();

protected:
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};