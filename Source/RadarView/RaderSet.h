#pragma once
#include "DataDefinition/SensorAttribute.h"

// RaderSet 对话框

class RaderSet : public CDialogEx
{
	DECLARE_DYNAMIC(RaderSet)

public:
	RaderSet(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~RaderSet();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RADERSET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	DataDefinition::SensorAttribute mySensor;
	
};
