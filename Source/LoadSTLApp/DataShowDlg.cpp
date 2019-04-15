#include "stdafx.h"
#include "Resource.h"
#include "DataShowDlg.h"

IMPLEMENT_DYNAMIC(DataShowDlg, CDialogBar)

void DataShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);

	//DDX_Control(pDX, IDC_BUTTON1, m_mybutton);
	//DDX_Control(pDX, IDC_GRID_DATASHOW, m_wndPropList);
	
}

BEGIN_MESSAGE_MAP(DataShowDlg, CDialogBar)
	ON_WM_CREATE()
	//ON_WM_MOVING()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	ON_MESSAGE(WM_INITDIALOG, &OnInitDialog)
	//ON_WM_NCLBUTTONDOWN()
	//ON_COMMAND(IDC_BUTTON1,&OnButton)
END_MESSAGE_MAP()

DataShowDlg::DataShowDlg(CWnd* pParent /*=NULL*/)
{
	m_bMenuRemoved = false;

}

DataShowDlg::~DataShowDlg()
{
}

int DataShowDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
	{	
		return -1;
	}

	return 0; 
}

LRESULT DataShowDlg::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = HandleInitDialog(wParam, lParam);
	if (!UpdateData(FALSE))
	{
		TRACE0("Warning, Unalbe to init update.\n");
	}

	return bRet;
}

void DataShowDlg::ShowInitGrid()
{
	CMFCPropertyGridCtrl *m_pwndPropList = (CMFCPropertyGridCtrl *)GetDlgItem(IDC_GRID_DATASHOW);
	HDITEM item;
	item.cxy = 120;
	item.mask = HDI_WIDTH;
	HDITEMW *phditemw = new HDITEM(item);
	m_pwndPropList->GetHeaderCtrl().SetItem(0, phditemw);
	delete(phditemw);//�����ɾ��������ڴ�й©
	phditemw = NULL;

	CMFCPropertyGridProperty *pGroup1 = new CMFCPropertyGridProperty(_T("��������"));
	CMFCPropertyGridProperty *pGroup2 = new CMFCPropertyGridProperty(_T("ģ������"));
	CMFCPropertyGridProperty *pGroup3 = new CMFCPropertyGridProperty(_T("��׼����"));
	CMFCPropertyGridProperty *pGroup4 = new CMFCPropertyGridProperty(_T("������"));

	CMFCPropertyGridProperty *pProp1 = new CMFCPropertyGridProperty(_T("��֡��"), _T(""));//0
	pProp1->AllowEdit(FALSE);//�������ѡ������޸�
	CMFCPropertyGridProperty *pProp2 = new CMFCPropertyGridProperty(_T("ƽ��֡�Ƿ���ʾ"), _T(""));//1
	pProp2->AllowEdit(FALSE);//�������ѡ������޸�
	CMFCPropertyGridProperty *pProp3 = new CMFCPropertyGridProperty(_T("ƽ��֡������"), _T(""));//2
	pProp3->AllowEdit(FALSE);//�������ѡ������޸�
	CMFCPropertyGridProperty *pProp4 = new CMFCPropertyGridProperty(_T("ͨ����"), _T(""));//3
	pProp4->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp5 = new CMFCPropertyGridProperty(_T("������ͨ����"), _T(""));//4
	pProp5->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp6 = new CMFCPropertyGridProperty(_T("�����Сͨ����"), _T(""));//5
	pProp6->AllowEdit(FALSE);

	CMFCPropertyGridProperty *pProp7 = new CMFCPropertyGridProperty(_T("ÿһͨ��ƽ������(������)"));//6
	pProp7->AllowEdit(FALSE);
	

	//CMFCPropertyGridProperty *pProp6 = new CMFCPropertyGridProperty(_T("��С����"), _T(""));//5
	//pProp6->AllowEdit(FALSE);

	CMFCPropertyGridProperty *pProp11 = new CMFCPropertyGridProperty(_T("ģ������"), _T(""));
	pProp11->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp12 = new CMFCPropertyGridProperty(_T("��������"), _T(""));
	pProp12->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp13 = new CMFCPropertyGridProperty(_T("������Ƭ����"), _T(""));
	pProp13->AllowEdit(FALSE);

	CMFCPropertyGridProperty *pProp21 = new CMFCPropertyGridProperty(_T("��׼��ʽ"), _T(""));
	pProp21->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp22 = new CMFCPropertyGridProperty(_T("��׼����ͨ��"), _T(""));//6
	pProp22->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp23 = new CMFCPropertyGridProperty(_T("ˮƽ��ת��"), _T(""));
	pProp23->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp24 = new CMFCPropertyGridProperty(_T("X����λ��"), _T(""));
	pProp24->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp25 = new CMFCPropertyGridProperty(_T("Y����λ��"), _T(""));
	pProp25->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp26 = new CMFCPropertyGridProperty(_T("Z����λ��"), _T(""));
	pProp26->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp27 = new CMFCPropertyGridProperty(_T("��׼��ʱ"), _T(""));
	pProp27->AllowEdit(FALSE);

	CMFCPropertyGridProperty *pProp31 = new CMFCPropertyGridProperty(_T("ϵͳ���"), _T(""));
	pProp31->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp32 = new CMFCPropertyGridProperty(_T("ϵͳ������ͨ��"), _T(""));
	pProp32->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp33 = new CMFCPropertyGridProperty(_T("ϵͳ�����Сͨ��"), _T(""));
	pProp33->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp34 = new CMFCPropertyGridProperty(_T("ͨ��ϵͳ���"));
	pProp34->AllowEdit(FALSE);
	//CMFCPropertyGridProperty *pProp35 = new CMFCPropertyGridProperty(_T("������"), _T(""));
	//pProp35->AllowEdit(FALSE);

	pGroup1->AddSubItem(pProp1);
	pGroup1->AddSubItem(pProp2);
	pGroup1->AddSubItem(pProp3);
	pGroup1->AddSubItem(pProp4);
	pGroup1->AddSubItem(pProp5);
	pGroup1->AddSubItem(pProp6);
	pGroup1->AddSubItem(pProp7);

	pGroup2->AddSubItem(pProp11);
	pGroup2->AddSubItem(pProp12);
	pGroup2->AddSubItem(pProp13);

	pGroup3->AddSubItem(pProp21);
	pGroup3->AddSubItem(pProp22);
	pGroup3->AddSubItem(pProp23);
	pGroup3->AddSubItem(pProp24);
	pGroup3->AddSubItem(pProp25);
	pGroup3->AddSubItem(pProp26);
	pGroup3->AddSubItem(pProp27);

	pGroup4->AddSubItem(pProp31);
	pGroup4->AddSubItem(pProp32);
	pGroup4->AddSubItem(pProp33);
	pGroup4->AddSubItem(pProp34);
	//pGroup4->AddSubItem(pProp35);
	
	m_pwndPropList->AddProperty(pGroup1);
	m_pwndPropList->AddProperty(pGroup2);
	m_pwndPropList->AddProperty(pGroup3);
	m_pwndPropList->AddProperty(pGroup4);
}

//��pProp�ӵ���PropertyNumber��SubItemNumber����
void DataShowDlg::AddGridProperty(CMFCPropertyGridProperty *set_pProp,int PropertyNumber,int SubItemNumber)
{
	if (NULL == set_pProp)//ΪNULL�򷵻�
	{
		return;
	}
	CMFCPropertyGridProperty *pProp = ((CMFCPropertyGridCtrl *)GetDlgItem(IDC_GRID_DATASHOW))->GetProperty(PropertyNumber)->GetSubItem(SubItemNumber);
	pProp->AddSubItem(set_pProp);
	pProp->Expand(1);
}

//ɾ����������
void DataShowDlg::DelGridProperty(int PropertyNumber, int SubItemNumber)
{
	CMFCPropertyGridProperty *pProp = ((CMFCPropertyGridCtrl *)GetDlgItem(IDC_GRID_DATASHOW))->GetProperty(PropertyNumber)->GetSubItem(SubItemNumber);
	CMFCPropertyGridProperty *tmp_prop = pProp->GetSubItem(0);//ÿ��ɾ����һ��
	pProp->RemoveSubItem(tmp_prop);
	pProp->Expand(0);
}

//������ʾ����
void DataShowDlg::SetShowValue(int PropertyNumber,int SubItemNumber,CString setValue)
{
	CMFCPropertyGridProperty *pProp = ((CMFCPropertyGridCtrl *)GetDlgItem(IDC_GRID_DATASHOW))->GetProperty(PropertyNumber)->GetSubItem(SubItemNumber);

	pProp->SetValue((_variant_t)setValue);
}

//���ظ�����ʾ����
void DataShowDlg::SetShowValue(int PropertyNumber, int SubItemNumber, int setValue)
{
	CMFCPropertyGridProperty *pProp = ((CMFCPropertyGridCtrl *)GetDlgItem(IDC_GRID_DATASHOW))->GetProperty(PropertyNumber)->GetSubItem(SubItemNumber);

	CString ValueNumber_show;
	ValueNumber_show.Format(_T("%d"), setValue);
	pProp->SetValue((_variant_t)ValueNumber_show);
}

//���ظ�����ʾ����
void DataShowDlg::SetShowValue(int PropertyNumber, int SubItemNumber, double setValue)
{
	CMFCPropertyGridProperty *pProp = ((CMFCPropertyGridCtrl *)GetDlgItem(IDC_GRID_DATASHOW))->GetProperty(PropertyNumber)->GetSubItem(SubItemNumber);

	CString ValueNumber_show;
	ValueNumber_show.Format(_T("%.7f"), setValue);
	pProp->SetValue((_variant_t)ValueNumber_show);
}

/**************************************************************************
*  @Function Name :: OnWindowPosChanged
*  @Description   :: ����λ�÷����仯����Ӧ��������Ҫ��Ϊ�˴��ڲ����Ա������ر�
*  @Author        :: zzw
*  @Parameters    :: lpwndpos:����λ����Ϣ��OnWindowPosChanged��������
*
*  @Return        :: None
**************************************************************************/
void DataShowDlg::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos)
{
	CDialogBar::OnWindowPosChanged(lpwndpos);
	RECT rect;//���ڵ�λ��
	if (IsFloating())
	{
		GetDockingFrame()->GetWindowRect(&rect);
		if (m_pDockBar && !m_bMenuRemoved)
		{
			CWnd* pParent = m_pDockBar->GetParent();
			if (pParent->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)))
			{
				pParent->ModifyStyle(WS_SYSMENU, 0, 0);
				m_bMenuRemoved = TRUE;
			}
		}
	}
	else if (m_bMenuRemoved)
	{
		m_bMenuRemoved = FALSE;
	}
}

//�������ڲ��ؼ��Ĵ�С
void DataShowDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogBar::OnSize(nType, cx, cy);
	CMFCPropertyGridCtrl *m_pwndPropList = (CMFCPropertyGridCtrl *)GetDlgItem(IDC_GRID_DATASHOW);
	if (nType != SIZE_MINIMIZED)
	{
		static int tmp1idx = 0;
		tmp1idx++;
		if (tmp1idx>1)//��һ�γ�ʼ����ʱ�򲢲�ִ��
		{
			CRect rc0, rc1;
			GetClientRect(&rc0);//��ȡ�������ڶԻ���Ĵ�С��λ��
			rc1.top = rc0.top + 1;
			rc1.left = rc0.left + 1;
			rc1.right = rc0.right - 1;
			rc1.bottom = rc0.bottom - 1;
			m_pwndPropList->MoveWindow(rc1);
		}

		
	}
}
