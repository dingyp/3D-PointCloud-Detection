#include "stdafx.h"
#include "resource.h"
#include "SensorSetDlg.h"
// RadarSet �Ի���

IMPLEMENT_DYNAMIC(SensorSetDlg, CDialogEx)

BEGIN_MESSAGE_MAP(SensorSetDlg, CDialogEx)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDOK, &SensorSetDlg::OnBnClickedOk)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_X, &SensorSetDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_Y, &SensorSetDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_Z, &SensorSetDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PITCH, &SensorSetDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ROLL, &SensorSetDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_YAW, &SensorSetDlg::OnDeltaposSpin)
END_MESSAGE_MAP()

/**************************************************************************
*  @Function Name :: SensorSet
*  @Description   :: SensorSet����ļ��Ĺ��캯��
*  @Author        :: zzw
*  @Parameters    :: sensor_attri:���������������� Type:��ʾ����
*					 pParent:������
*  @Return        :: None
**************************************************************************/
SensorSetDlg::SensorSetDlg(SensorAttribute* sensor_attri, int Type, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SENSORSET, pParent)
{
	m_pSensor = NULL;
	m_iShowType = Type;
	//memset(&mySensor, 0, sizeof(DataDefinition::SensorAttribute));
	m_pSensor = sensor_attri;
}

/**************************************************************************
*  @Function Name :: SensorSet
*  @Description   :: SensorSet��Ĵ�ID�Ĺ��캯��
*  @Author        :: zzw
*  @Parameters    :: nIDTemplate:����ID
*					 pParent:������
*  @Return        :: None
**************************************************************************/
SensorSetDlg::SensorSetDlg(UINT nIDTemplate, SensorAttribute* sensor_attri, CWnd* pParent /*=nullptr*/)
	: CDialogEx(nIDTemplate, pParent)
{
	m_pSensor = NULL;
	m_iShowType = 1;
	m_pSensor = sensor_attri;
	//memset(&mySensor, 0, sizeof(DataDefinition::SensorAttribute));
}

/**************************************************************************
*  @Function Name :: ~SensorSet
*  @Description   :: SensorSet�����������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
SensorSetDlg::~SensorSetDlg()
{
	m_pSensor = NULL;
}

void SensorSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

/**************************************************************************
*  @Function Name :: OnInitDialog
*  @Description   :: SensorSet���ڵĳ�ʼ������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �Ƿ��ʼ���ɹ�
**************************************************************************/
BOOL SensorSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CScrollBar* pScrollBar = (CScrollBar*)GetDlgItem(IDC_SENSORSCROLL);
	pScrollBar->SetScrollRange(0, 3);//�����ƶ���λ��Ϊ0--3;

	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_LIST2);
	//�Ȳ����������ʾ������
	pListBox->AddString(_T("VLP_16"));
	pListBox->AddString(_T("VLP_2D"));

	//����Ĭ��ѡ��
	if (NULL == m_pSensor)
	{
		return false;
	}

	if (LIDAR_HLP16 == m_pSensor->sensorKind)
	{
		pListBox->SetCurSel(0);
	}
	else if (LIDAR_2D== m_pSensor->sensorKind)
	{
		pListBox->SetCurSel(1);
	}
	if (0 == m_iShowType)
	{//��������ѡ��Э��
		pListBox->EnableWindow(false);
	}
	else
	{//����ѡ��Э��
		pListBox->EnableWindow(true);
	}

	CString m_show;
	m_show.Format(_T("%0.2f"), m_pSensor->sensorX);
	GetDlgItem(IDC_EDIT_X)->SetWindowTextW(m_show);
	m_show.Format(_T("%0.2f"), m_pSensor->sensorY);
	GetDlgItem(IDC_EDIT_Y)->SetWindowTextW(m_show);
	m_show.Format(_T("%0.2f"), m_pSensor->sensorZ);
	GetDlgItem(IDC_EDIT_Z)->SetWindowTextW(m_show);

	m_show.Format(_T("%0.2f"), m_pSensor->sensorPitch);
	GetDlgItem(IDC_EDIT_PITCH)->SetWindowTextW(m_show);
	m_show.Format(_T("%0.2f"), m_pSensor->sensorRoll);
	GetDlgItem(IDC_EDIT_ROLL)->SetWindowTextW(m_show);
	m_show.Format(_T("%0.2f"), m_pSensor->sensorYaw);
	GetDlgItem(IDC_EDIT_YAW)->SetWindowTextW(m_show);

	return true;
}

/**************************************************************************
*  @Function Name :: OnVScroll
*  @Description   :: �ļ����Ի���������Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nSBCode:�����¼������� nPos:�¼�������λ��
*					 pScrollBar:����֮���λ��
*  @Return        :: None
**************************************************************************/
void SensorSetDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	int TempPos = pScrollBar->GetScrollPos();
	switch (nSBCode)
	{
	case SB_THUMBPOSITION://�϶�����
		pScrollBar->SetScrollPos(nPos);
		break;
	case SB_LINEUP://����ϱߵļ�ͷ
		if (TempPos > 0)
		{
			TempPos--;
		}
		pScrollBar->SetScrollPos(TempPos);
		break;
	case SB_LINEDOWN://����±ߵļ�ͷ
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
*  @Description   :: ���spin����Ӧ����
*  @Author        :: zzw
*  @Parameters    :: pNMHDR:��¼�˴�����spin��ID
*					 pResult:���ص����н��
*  @Return        :: None
**************************************************************************/
void SensorSetDlg::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	UINT ID_FROM = pNMUpDown->hdr.idFrom;
	UINT ID_EDIT = 0;//��Ҫ�ı��EDIT��ID
	int mode = -1;//��Ҫ���еķ�Ӧ����Ϊspin���֮�󣬲�����ȫ��ͬ�ķ�Ӧ
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString m_edit;
	GetDlgItem(ID_EDIT)->GetWindowTextW(m_edit);

	if (0 == mode)
	{//�����ɸ�,û�����ƣ����ǻ��ǲ�Ҫ����̫Զ��
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
*  @Description   :: ���ȷ����ť֮�����Ӧ����
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void SensorSetDlg::OnBnClickedOk()
{
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_LIST2);

	int idx = pListBox->GetCurSel();
	switch (idx)
	{
	case 0://VLP_16
		m_pSensor->sensorKind = LIDAR_HLP16;
		break;
	case 1://2D
		m_pSensor->sensorKind = LIDAR_2D;
		break;
	}

	CString m_show;
	GetDlgItem(IDC_EDIT_X)->GetWindowTextW(m_show);
	m_pSensor->sensorX = _ttof(m_show);
	GetDlgItem(IDC_EDIT_Y)->GetWindowTextW(m_show);
	m_pSensor->sensorY = _ttof(m_show);
	GetDlgItem(IDC_EDIT_Z)->GetWindowTextW(m_show);
	m_pSensor->sensorZ = _ttof(m_show);
	GetDlgItem(IDC_EDIT_PITCH)->GetWindowTextW(m_show);
	m_pSensor->sensorPitch = _ttof(m_show);
	GetDlgItem(IDC_EDIT_ROLL)->GetWindowTextW(m_show);
	m_pSensor->sensorRoll = _ttof(m_show);
	GetDlgItem(IDC_EDIT_YAW)->GetWindowTextW(m_show);
	m_pSensor->sensorYaw = _ttof(m_show);

	if (LIDAR_INVALID == m_pSensor->sensorKind)
	{
		MessageBox(_T("������Э�飡"), _T("��ʾ"), 0);
	}
	else
	{
		CDialogEx::OnOK();
	}
}
