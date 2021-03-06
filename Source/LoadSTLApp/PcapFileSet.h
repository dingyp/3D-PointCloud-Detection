#pragma once
#include "SensorAttribute.h"

// PcapFileSet 对话框
class PcapFileSet : public CDialogEx
{
	DECLARE_DYNAMIC(PcapFileSet)

public:
	PcapFileSet(CString File_path,CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~PcapFileSet();

	DataDefinition::SensorAttribute GetSensorAtt();
	
protected:
	PcapFileSet(UINT nIDTemplate, CWnd* pParent = nullptr);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL OnInitDialog();
	CString open_File_path;
	DataDefinition::SensorAttribute mySensor;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
private:
	double m_sensorX, m_sensorY, m_sensorZ;					//传感器的x、y、z坐标
	double m_sensorPitch, m_sensorRoll, m_sensorYaw;		//传感器的x轴、y轴、z轴旋转角
};
