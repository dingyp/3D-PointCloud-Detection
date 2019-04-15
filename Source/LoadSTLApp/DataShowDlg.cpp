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
	delete(phditemw);//这个不删除会造成内存泄漏
	phditemw = NULL;

	CMFCPropertyGridProperty *pGroup1 = new CMFCPropertyGridProperty(_T("点云属性"));
	CMFCPropertyGridProperty *pGroup2 = new CMFCPropertyGridProperty(_T("模型属性"));
	CMFCPropertyGridProperty *pGroup3 = new CMFCPropertyGridProperty(_T("配准属性"));
	CMFCPropertyGridProperty *pGroup4 = new CMFCPropertyGridProperty(_T("误差分析"));

	CMFCPropertyGridProperty *pProp1 = new CMFCPropertyGridProperty(_T("总帧数"), _T(""));//0
	pProp1->AllowEdit(FALSE);//不允许对选项进行修改
	CMFCPropertyGridProperty *pProp2 = new CMFCPropertyGridProperty(_T("平均帧是否显示"), _T(""));//1
	pProp2->AllowEdit(FALSE);//不允许对选项进行修改
	CMFCPropertyGridProperty *pProp3 = new CMFCPropertyGridProperty(_T("平均帧点云数"), _T(""));//2
	pProp3->AllowEdit(FALSE);//不允许对选项进行修改
	CMFCPropertyGridProperty *pProp4 = new CMFCPropertyGridProperty(_T("通道数"), _T(""));//3
	pProp4->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp5 = new CMFCPropertyGridProperty(_T("误差最大通道数"), _T(""));//4
	pProp5->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp6 = new CMFCPropertyGridProperty(_T("误差最小通道数"), _T(""));//5
	pProp6->AllowEdit(FALSE);

	CMFCPropertyGridProperty *pProp7 = new CMFCPropertyGridProperty(_T("每一通道平均方差(随机误差)"));//6
	pProp7->AllowEdit(FALSE);
	

	//CMFCPropertyGridProperty *pProp6 = new CMFCPropertyGridProperty(_T("最小方差"), _T(""));//5
	//pProp6->AllowEdit(FALSE);

	CMFCPropertyGridProperty *pProp11 = new CMFCPropertyGridProperty(_T("模型类型"), _T(""));
	pProp11->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp12 = new CMFCPropertyGridProperty(_T("顶点总数"), _T(""));
	pProp12->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp13 = new CMFCPropertyGridProperty(_T("三角面片总数"), _T(""));
	pProp13->AllowEdit(FALSE);

	CMFCPropertyGridProperty *pProp21 = new CMFCPropertyGridProperty(_T("配准方式"), _T(""));
	pProp21->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp22 = new CMFCPropertyGridProperty(_T("配准所用通道"), _T(""));//6
	pProp22->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp23 = new CMFCPropertyGridProperty(_T("水平旋转角"), _T(""));
	pProp23->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp24 = new CMFCPropertyGridProperty(_T("X方向位移"), _T(""));
	pProp24->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp25 = new CMFCPropertyGridProperty(_T("Y方向位移"), _T(""));
	pProp25->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp26 = new CMFCPropertyGridProperty(_T("Z方向位移"), _T(""));
	pProp26->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp27 = new CMFCPropertyGridProperty(_T("配准用时"), _T(""));
	pProp27->AllowEdit(FALSE);

	CMFCPropertyGridProperty *pProp31 = new CMFCPropertyGridProperty(_T("系统误差"), _T(""));
	pProp31->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp32 = new CMFCPropertyGridProperty(_T("系统误差最大通道"), _T(""));
	pProp32->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp33 = new CMFCPropertyGridProperty(_T("系统误差最小通道"), _T(""));
	pProp33->AllowEdit(FALSE);
	CMFCPropertyGridProperty *pProp34 = new CMFCPropertyGridProperty(_T("通道系统误差"));
	pProp34->AllowEdit(FALSE);
	//CMFCPropertyGridProperty *pProp35 = new CMFCPropertyGridProperty(_T("随机误差"), _T(""));
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

//将pProp加到第PropertyNumber的SubItemNumber里面
void DataShowDlg::AddGridProperty(CMFCPropertyGridProperty *set_pProp,int PropertyNumber,int SubItemNumber)
{
	if (NULL == set_pProp)//为NULL则返回
	{
		return;
	}
	CMFCPropertyGridProperty *pProp = ((CMFCPropertyGridCtrl *)GetDlgItem(IDC_GRID_DATASHOW))->GetProperty(PropertyNumber)->GetSubItem(SubItemNumber);
	pProp->AddSubItem(set_pProp);
	pProp->Expand(1);
}

//删除里面内容
void DataShowDlg::DelGridProperty(int PropertyNumber, int SubItemNumber)
{
	CMFCPropertyGridProperty *pProp = ((CMFCPropertyGridCtrl *)GetDlgItem(IDC_GRID_DATASHOW))->GetProperty(PropertyNumber)->GetSubItem(SubItemNumber);
	CMFCPropertyGridProperty *tmp_prop = pProp->GetSubItem(0);//每次删除第一个
	pProp->RemoveSubItem(tmp_prop);
	pProp->Expand(0);
}

//更改显示内容
void DataShowDlg::SetShowValue(int PropertyNumber,int SubItemNumber,CString setValue)
{
	CMFCPropertyGridProperty *pProp = ((CMFCPropertyGridCtrl *)GetDlgItem(IDC_GRID_DATASHOW))->GetProperty(PropertyNumber)->GetSubItem(SubItemNumber);

	pProp->SetValue((_variant_t)setValue);
}

//重载更改显示内容
void DataShowDlg::SetShowValue(int PropertyNumber, int SubItemNumber, int setValue)
{
	CMFCPropertyGridProperty *pProp = ((CMFCPropertyGridCtrl *)GetDlgItem(IDC_GRID_DATASHOW))->GetProperty(PropertyNumber)->GetSubItem(SubItemNumber);

	CString ValueNumber_show;
	ValueNumber_show.Format(_T("%d"), setValue);
	pProp->SetValue((_variant_t)ValueNumber_show);
}

//重载更改显示内容
void DataShowDlg::SetShowValue(int PropertyNumber, int SubItemNumber, double setValue)
{
	CMFCPropertyGridProperty *pProp = ((CMFCPropertyGridCtrl *)GetDlgItem(IDC_GRID_DATASHOW))->GetProperty(PropertyNumber)->GetSubItem(SubItemNumber);

	CString ValueNumber_show;
	ValueNumber_show.Format(_T("%.7f"), setValue);
	pProp->SetValue((_variant_t)ValueNumber_show);
}

/**************************************************************************
*  @Function Name :: OnWindowPosChanged
*  @Description   :: 窗口位置发生变化的响应函数，主要是为了窗口不可以被主动关闭
*  @Author        :: zzw
*  @Parameters    :: lpwndpos:含有位置信息，OnWindowPosChanged函数参数
*
*  @Return        :: None
**************************************************************************/
void DataShowDlg::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos)
{
	CDialogBar::OnWindowPosChanged(lpwndpos);
	RECT rect;//窗口的位置
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

//控制其内部控件的大小
void DataShowDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogBar::OnSize(nType, cx, cy);
	CMFCPropertyGridCtrl *m_pwndPropList = (CMFCPropertyGridCtrl *)GetDlgItem(IDC_GRID_DATASHOW);
	if (nType != SIZE_MINIMIZED)
	{
		static int tmp1idx = 0;
		tmp1idx++;
		if (tmp1idx>1)//第一次初始化的时候并不执行
		{
			CRect rc0, rc1;
			GetClientRect(&rc0);//先取得主窗口对话框的大小及位置
			rc1.top = rc0.top + 1;
			rc1.left = rc0.left + 1;
			rc1.right = rc0.right - 1;
			rc1.bottom = rc0.bottom - 1;
			m_pwndPropList->MoveWindow(rc1);
		}

		
	}
}
