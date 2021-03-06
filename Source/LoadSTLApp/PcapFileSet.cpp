// PcapFileSet.cpp: 实现文件
//

#include "stdafx.h"
#include "LoadSTLApp.h"
#include "PcapFileSet.h"
#include "afxdialogex.h"
#include "LoadSTLAppDoc.h"
//#include "SystemConfigVarManager.h"

// PcapFileSet 对话框

IMPLEMENT_DYNAMIC(PcapFileSet, CDialogEx)

BEGIN_MESSAGE_MAP(PcapFileSet, CDialogEx)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDOK, &PcapFileSet::OnBnClickedOk)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_X, &PcapFileSet::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_Y, &PcapFileSet::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_Z, &PcapFileSet::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PITCH, &PcapFileSet::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ROLL, &PcapFileSet::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_YAW, &PcapFileSet::OnDeltaposSpin)
END_MESSAGE_MAP()

/**************************************************************************
*  @Function Name :: SensorSet
*  @Description   :: SensorSet类带文件的构造函数
*  @Author        :: zzw
*  @Parameters    :: File_path:文件路径
*					 pParent:父窗口
*  @Return        :: None
**************************************************************************/
PcapFileSet::PcapFileSet(CString File_path, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PCAPFILE, pParent)
{
	memset(&mySensor, 0, sizeof(DataDefinition::SensorAttribute));
	open_File_path = File_path;
}

/**************************************************************************
*  @Function Name :: SensorSet
*  @Description   :: SensorSet类的带ID的构造函数
*  @Author        :: zzw
*  @Parameters    :: nIDTemplate:窗口ID
*					 pParent:父窗口
*  @Return        :: None
**************************************************************************/
PcapFileSet::PcapFileSet(UINT nIDTemplate, CWnd* pParent /*=nullptr*/)
	: CDialogEx(nIDTemplate, pParent)
{
	memset(&mySensor, 0, sizeof(DataDefinition::SensorAttribute));
}

/**************************************************************************
*  @Function Name :: ~SensorSet
*  @Description   :: SensorSet类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
PcapFileSet::~PcapFileSet()
{
}

void PcapFileSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

/**************************************************************************
*  @Function Name :: OnInitDialog
*  @Description   :: SensorSet窗口的初始化函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 是否初始化成功
**************************************************************************/
BOOL PcapFileSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CScrollBar* pScrollBar = (CScrollBar*)GetDlgItem(IDC_PCAPSCROLL);
	pScrollBar->SetScrollRange(0, 3);//滑块移动的位置为0--3;

	CListBox *pListBox=(CListBox *)GetDlgItem(IDC_SENSORLIST);
	//先插入的数据显示在下面
	pListBox->AddString(_T("LIDAR_INVALID"));
	pListBox->AddString(_T("VLP_16"));
	pListBox->AddString(_T("VLP_2D"));

	//设置默认选项
	CString suffixName;
	if (open_File_path == "")		//如果路径是空则返回
	{
		suffixName = "";
	}
	int pos = open_File_path.ReverseFind('.');
	suffixName = open_File_path.Right(open_File_path.GetLength() - pos - 1);

	if (suffixName == "pcap")
	{
		mySensor.sensorKind = LIDAR_HLP16;
		pListBox->SetCurSel(1);
	}
	else if (suffixName == "pcapng")
	{
		mySensor.sensorKind = LIDAR_2D;
		pListBox->SetCurSel(2);
	}
	else//LIDAR_INVALID
	{
		mySensor.sensorKind = LIDAR_INVALID;
		pListBox->SetCurSel(0);
	}


	////将界面中的参数都设置默认参数
	//m_pPoints3DWnd->GetSensorCenter(m_sensorX, m_sensorY, m_sensorZ);
	//m_pPoints3DWnd->GetSensorEuler(m_sensorPitch, m_sensorRoll, m_sensorYaw);
	m_sensorX = m_sensorY = m_sensorZ = 0;
	m_sensorPitch = m_sensorRoll = m_sensorYaw = 0;

	CString m_show;
	m_show.Format(_T("%0.2f"), m_sensorX);
	GetDlgItem(IDC_EDIT_X)->SetWindowTextW(m_show);
	m_show.Format(_T("%0.2f"), m_sensorY);
	GetDlgItem(IDC_EDIT_Y)->SetWindowTextW(m_show);
	m_show.Format(_T("%0.2f"), m_sensorZ);
	GetDlgItem(IDC_EDIT_Z)->SetWindowTextW(m_show);

	m_show.Format(_T("%0.2f"), m_sensorPitch);
	GetDlgItem(IDC_EDIT_PITCH)->SetWindowTextW(m_show);
	m_show.Format(_T("%0.2f"), m_sensorRoll);
	GetDlgItem(IDC_EDIT_ROLL)->SetWindowTextW(m_show);
	m_show.Format(_T("%0.2f"), m_sensorYaw);
	GetDlgItem(IDC_EDIT_YAW)->SetWindowTextW(m_show);

	return true;
}

/**************************************************************************
*  @Function Name :: OnVScroll
*  @Description   :: 文件属性滑动条的响应函数
*  @Author        :: zzw
*  @Parameters    :: nSBCode:触发事件的类型 nPos:事件触发的位置
*					 pScrollBar:触发之后的位置
*  @Return        :: None
**************************************************************************/
void PcapFileSet::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int TempPos = pScrollBar->GetScrollPos();
	switch (nSBCode)
	{
	case SB_THUMBPOSITION://拖动滑块
		pScrollBar->SetScrollPos(nPos);
		break;
	case SB_LINEUP://点击上边的箭头
		if (TempPos > 0)
		{
			TempPos--;
		}
		pScrollBar->SetScrollPos(TempPos);
		break;
	case SB_LINEDOWN://点击下边的箭头
		if (TempPos<3)
		{
			TempPos++;
		}
		pScrollBar->SetScrollPos(TempPos);
		break;
	}
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}

/**************************************************************************
*  @Function Name :: OnDeltaposSpin
*  @Description   :: 点击spin的响应函数
*  @Author        :: zzw
*  @Parameters    :: pNMHDR:记录了触发的spin的ID
*					 pResult:返回的运行结果
*  @Return        :: None
**************************************************************************/
void PcapFileSet::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	UINT ID_FROM = pNMUpDown->hdr.idFrom;
	UINT ID_EDIT = 0;//所要改变的EDIT的ID
	int mode = -1;//索要进行的反应，因为spin点击之后，不是完全相同的反应
	switch (ID_FROM)
	{
	case IDC_SPIN_X:
		ID_EDIT = IDC_EDIT_X;
		mode = 0;
		break;
	case IDC_SPIN_Y:
		ID_EDIT = IDC_EDIT_Y;
		mode = 0;
		break;
	case IDC_SPIN_Z:
		ID_EDIT = IDC_EDIT_Z;
		mode = 0;
		break;
	case IDC_SPIN_PITCH:
		ID_EDIT = IDC_EDIT_PITCH;
		mode = 1;
		break;
	case IDC_SPIN_ROLL:
		ID_EDIT = IDC_EDIT_ROLL;
		mode = 1;
		break;
	case IDC_SPIN_YAW:
		ID_EDIT = IDC_EDIT_YAW;
		mode = 1;
		break;
	default:
		break;
	}
	// TODO: 在此添加控件通知处理程序代码
	CString m_edit;
	GetDlgItem(ID_EDIT)->GetWindowTextW(m_edit);

	if (0 == mode)
	{//可正可负,没有限制，但是还是不要超过太远了
		double m_editDouble_number = _ttof(m_edit);
		if (pNMUpDown->iDelta < 0)
		{
			m_editDouble_number = m_editDouble_number + 1;
		}
		if (pNMUpDown->iDelta > 0)
		{
			m_editDouble_number = m_editDouble_number - 1;
		}
		if (m_editDouble_number > 100)
		{
			m_editDouble_number = 100;
		}
		if (m_editDouble_number < -100)
		{
			m_editDouble_number = -100;
		}
		m_edit.Format(_T("%0.2f"), m_editDouble_number);
		GetDlgItem(ID_EDIT)->SetWindowTextW(m_edit);
	}
	else if (1 == mode)
	{//[-90-90]
		double m_editDouble_number = _ttof(m_edit);
		if (pNMUpDown->iDelta < 0)
		{
			m_editDouble_number = m_editDouble_number + 1;
		}
		if (pNMUpDown->iDelta > 0)
		{
			m_editDouble_number = m_editDouble_number - 1;
		}
		if (m_editDouble_number > 90)
		{
			m_editDouble_number = 90;
		}
		if (m_editDouble_number < -90)
		{
			m_editDouble_number = -90;
		}
		m_edit.Format(_T("%0.2f"), m_editDouble_number);
		GetDlgItem(ID_EDIT)->SetWindowTextW(m_edit);
	}

	*pResult = 0;
}

/**************************************************************************
*  @Function Name :: OnBnClickedOk
*  @Description   :: 点击确定按钮之后的响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void PcapFileSet::OnBnClickedOk()
{
	CLoadSTLAppDoc *pCSTLDoc = (CLoadSTLAppDoc *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView()->GetDocument();

	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_SENSORLIST);

	int idx=pListBox->GetCurSel();
	switch (idx)
	{
	case 0://LIDAR_INVALID
		mySensor.sensorKind = LIDAR_INVALID;
		break;
	case 1://VLP_16
		mySensor.sensorKind = LIDAR_HLP16;
		break;
	case 2://2D
		mySensor.sensorKind = LIDAR_2D;
		break;
	}

	CString m_show;
	GetDlgItem(IDC_EDIT_X)->GetWindowTextW(m_show);
	mySensor.sensorX = _ttof(m_show);
	GetDlgItem(IDC_EDIT_Y)->GetWindowTextW(m_show);
	mySensor.sensorY = _ttof(m_show);
	GetDlgItem(IDC_EDIT_Z)->GetWindowTextW(m_show);
	mySensor.sensorZ = _ttof(m_show);
	GetDlgItem(IDC_EDIT_PITCH)->GetWindowTextW(m_show);
	mySensor.sensorPitch = _ttof(m_show);
	GetDlgItem(IDC_EDIT_ROLL)->GetWindowTextW(m_show);
	mySensor.sensorRoll = _ttof(m_show);
	GetDlgItem(IDC_EDIT_YAW)->GetWindowTextW(m_show);
	mySensor.sensorYaw = _ttof(m_show);

	//如果没有改变，则不需要设置了
	double differ_sensorX = mySensor.sensorX - m_sensorX;
	double differ_sensorY = mySensor.sensorY - m_sensorY;
	double differ_sensorZ = mySensor.sensorZ - m_sensorZ;
	double differ_sensorPitch = mySensor.sensorPitch - m_sensorPitch;
	double differ_sensorRoll = mySensor.sensorRoll - m_sensorRoll;
	double differ_sensorYaw = mySensor.sensorYaw - m_sensorYaw;
	if (fabs(differ_sensorX) > 1e-7 || fabs(differ_sensorY) > 1e-7 || fabs(differ_sensorZ) > 1e-7 || \
		fabs(differ_sensorPitch) > 1e-7 || fabs(differ_sensorRoll) > 1e-7 || fabs(differ_sensorYaw) > 1e-7)
	{
		pCSTLDoc->SetSensorCenter(mySensor.sensorX, mySensor.sensorY, mySensor.sensorZ);			//设置传感器中心的坐标
		pCSTLDoc->SetSensorEuler(mySensor.sensorPitch, mySensor.sensorRoll, mySensor.sensorYaw);	//设置传感器的姿态角
	}
	if (LIDAR_INVALID == mySensor.sensorKind)
	{
		MessageBox(_T("请设置协议！"), _T("提示"), 0);
	}
	else
	{
		CDialogEx::OnOK();
	}
}

/**************************************************************************
*  @Function Name :: GetSensorAtt
*  @Description   :: 获取页面的设置的属性值
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 页面设置的属性值
**************************************************************************/
DataDefinition::SensorAttribute PcapFileSet::GetSensorAtt()
{
	return mySensor;
}
