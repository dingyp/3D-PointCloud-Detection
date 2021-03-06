// RadarSet.cpp: 实现文件
//
#include "stdafx.h"
#include "resource.h"
#include "RadarSetDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(RadarSetDlg, SensorSetDlg)

BEGIN_MESSAGE_MAP(RadarSetDlg, SensorSetDlg)
	ON_BN_CLICKED(IDOK, &RadarSetDlg::OnBnClickedOk)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DESTPORT, &RadarSetDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOCALPORT, &RadarSetDlg::OnDeltaposSpin)
END_MESSAGE_MAP()

/**************************************************************************
*  @Function Name :: RadarSet
*  @Description   :: RadarSet类的构造函数
*  @Author        :: zzw
*  @Parameters    :: pParent:父窗口
*
*  @Return        :: None
**************************************************************************/
RadarSetDlg::RadarSetDlg(SensorAttribute* sensor_attri, CWnd* pParent/* = nullptr*/)
	: SensorSetDlg(IDD_RADERSET,sensor_attri, pParent)
{
}

/**************************************************************************
*  @Function Name :: ~RadarSet
*  @Description   :: RadarSet类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
RadarSetDlg::~RadarSetDlg()
{
}

void RadarSetDlg::DoDataExchange(CDataExchange* pDX)
{
	SensorSetDlg::DoDataExchange(pDX);
}

/**************************************************************************
*  @Function Name :: OnBnClickedOk
*  @Description   :: 点击确定按钮的触发事件
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void RadarSetDlg::OnBnClickedOk()
{
	SetNetWorkParam();
	SensorSetDlg::OnBnClickedOk();
}

/**************************************************************************
*  @Function Name :: OnDeltaposSpin
*  @Description   :: 点击spin的响应函数
*  @Author        :: zzw
*  @Parameters    :: pNMHDR:记录了触发的spin的ID
*					 pResult:返回的运行结果
*  @Return        :: None
**************************************************************************/
void RadarSetDlg::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	UINT ID_FROM = pNMUpDown->hdr.idFrom;
	if (IDC_SPIN_DESTPORT == ID_FROM )
	{
		CString m_edit;
		GetDlgItem(IDC_EDIT_DESTPORT)->GetWindowTextW(m_edit);
		unsigned short port= _ttoi(m_edit);
		if (pNMUpDown->iDelta < 0)
		{
			port = port + 1;
		}
		if (pNMUpDown->iDelta > 0)
		{
			port = port - 1;
		}
		m_edit.Format(_T("%d"), port);
		GetDlgItem(IDC_EDIT_DESTPORT)->SetWindowTextW(m_edit);
		return;
	}
	else if (IDC_SPIN_LOCALPORT == ID_FROM)
	{
		CString m_edit;
		GetDlgItem(IDC_EDIT_LOCALPORT)->GetWindowTextW(m_edit);
		unsigned short port = _ttoi(m_edit);
		if (pNMUpDown->iDelta < 0)
		{
			port = port + 1;
		}
		if (pNMUpDown->iDelta > 0)
		{
			port = port - 1;
		}
		m_edit.Format(_T("%d"), port);
		GetDlgItem(IDC_EDIT_LOCALPORT)->SetWindowTextW(m_edit);
		return;
	}
	SensorSetDlg::OnDeltaposSpin(pNMHDR, pResult);
}

/**************************************************************************
*  @Function Name :: OnInitDialog
*  @Description   :: RadarSet的初始化函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 是否初始化成功
**************************************************************************/
BOOL RadarSetDlg::OnInitDialog()
{
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS))->SetAddress(192, 168, 1, 100);
	((CIPAddressCtrl*)GetDlgItem(IDC_ADDRESSMASK))->SetAddress(255, 255, 255, 0);
	GetDlgItem(IDC_EDIT_DESTPORT)->SetWindowTextW(_T("2368"));
	GetDlgItem(IDC_EDIT_LOCALPORT)->SetWindowTextW(_T("2368"));
	return SensorSetDlg::OnInitDialog();
}

/**************************************************************************
*  @Function Name :: SetNetWorkParam
*  @Description   :: 设置网络连接方面的属性
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void RadarSetDlg::SetNetWorkParam()
{
	//获得ip
	BYTE field0, field1, field2, field3;
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS))->GetAddress(field0, field1, field2, field3);
	sprintf_s(m_pSensor->ip, "%d.%d.%d.%d", field0, field1, field2, field3);
	//获得mask
	((CIPAddressCtrl*)GetDlgItem(IDC_ADDRESSMASK))->GetAddress(field0, field1, field2, field3);
	sprintf_s(m_pSensor->mask, "%d.%d.%d.%d", field0, field1, field2, field3);
	//获得netport
	CString str_netport;
	GetDlgItem(IDC_EDIT_DESTPORT)->GetWindowTextW(str_netport);
	m_pSensor->sensorPort = _ttoi(str_netport);
	GetDlgItem(IDC_EDIT_LOCALPORT)->GetWindowTextW(str_netport);
	m_pSensor->localPort = _ttoi(str_netport);
}
