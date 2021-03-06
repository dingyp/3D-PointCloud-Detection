#pragma once
#include "SensorSetDlg.h"

// RadarSet 对话框
class RadarSetDlg : public SensorSetDlg
{
	DECLARE_DYNAMIC(RadarSetDlg)
public:
	RadarSetDlg(SensorAttribute* sensor_attri, CWnd* pParent = nullptr);
	virtual ~RadarSetDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RADARSET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
private:
	void SetNetWorkParam();//得到网络中的参数
};
