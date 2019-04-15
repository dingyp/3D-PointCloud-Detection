#pragma once
#include "stdafx.h"

//显示编码属性范围的对话框
class DataRangeDlg : public CDialogEx
{
public:
	DataRangeDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DataRangeDlg();

protected:
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};