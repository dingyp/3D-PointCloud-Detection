// GridSet.cpp: 实现文件
//

#include "stdafx.h"
#include "RadarView.h"
#include "GridSetDlg.h"
#include "afxdialogex.h"
#include "RadarViewView.h"
#include "MainFrm.h"

// GridSet 对话框

IMPLEMENT_DYNAMIC(GridSetDlg, CDialogEx)

/**************************************************************************
*  @Function Name :: GridSet
*  @Description   :: GridSet类的构造函数
*  @Author        :: zzw
*  @Parameters    :: pParent:父窗口
*
*  @Return        :: None
**************************************************************************/
GridSetDlg::GridSetDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GRIDSET, pParent)
{
}

/**************************************************************************
*  @Function Name :: ~GridSet
*  @Description   :: GridSet类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
GridSetDlg::~GridSetDlg()
{
}

void GridSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SPIN_GRIDX, m_SpinGridX);
	DDX_Control(pDX, IDC_SPIN_GRIDY, m_SpinGridY);
	DDX_Control(pDX, IDC_SPIN_GRIDZ, m_SpinGridZ);

	DDX_Control(pDX, IDC_SPIN_GRIDLENGTH, m_SpinGridLength);
	DDX_Control(pDX, IDC_SPIN_RESOLUTION, m_SpinGridResolution);
	DDX_Control(pDX, IDC_SPIN_GRIDRADIUS, m_SpinGridRadius);
	DDX_Control(pDX, IDC_SPIN_GRIDCONCIRCLE, m_SpinGridConCircle);
	DDX_Control(pDX, IDC_SPIN_THICKNESS, m_SpinGridWidth);
	DDX_Control(pDX, IDC_GRIDCOLORBUTTON, m_GridColorPicker);

	DDX_Control(pDX, IDC_CHECK_BASELINESHOW, m_BaseLineCheckBox); 
	DDX_Control(pDX, IDC_CHECK_AXISNODESHOW, m_AxisNodeCheckBox);
	DDX_Control(pDX, IDC_CHECK_FILTER, m_FilterCheckBox);
}


BEGIN_MESSAGE_MAP(GridSetDlg, CDialogEx)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GRIDX, &GridSetDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GRIDY, &GridSetDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GRIDZ, &GridSetDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GRIDLENGTH, &GridSetDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_RESOLUTION, &GridSetDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GRIDRADIUS, &GridSetDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GRIDCONCIRCLE, &GridSetDlg::OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_THICKNESS, &GridSetDlg::OnDeltaposSpin)
	ON_BN_CLICKED(IDOK, &GridSetDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDOK_RESET, &GridSetDlg::OnBnClickedReset)
END_MESSAGE_MAP()


// GridSet 消息处理程序

/**************************************************************************
*  @Function Name :: OnInitDialog
*  @Description   :: 网格属性对话框的初始化函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 是否初始化成功
**************************************************************************/
BOOL GridSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CRadarViewView *pView = (CRadarViewView *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView();
	Points3DWnd * m_pPoints3DWnd = pView->Get3DWnd();

	CString m_show;
	//进行网格数据的同步更新并显示
	m_show.Format(_T("%0.2f"), m_pPoints3DWnd->m_GridX);
	GetDlgItem(IDC_EDIT_GRIDX)->SetWindowTextW(m_show);
	m_show.Format(_T("%0.2f"), m_pPoints3DWnd->m_GridY);
	GetDlgItem(IDC_EDIT_GRIDY)->SetWindowTextW(m_show);
	m_show.Format(_T("%0.2f"), m_pPoints3DWnd->m_GridZ);
	GetDlgItem(IDC_EDIT_GRIDZ)->SetWindowTextW(m_show);

	m_show.Format(_T("%0.2f"), m_pPoints3DWnd->m_GridLength);
	GetDlgItem(IDC_EDIT_GRIDLENGTH)->SetWindowTextW(m_show);
	m_show.Format(_T("%2d"), m_pPoints3DWnd->m_GridResolution);
	GetDlgItem(IDC_EDIT_RESOLUTION)->SetWindowTextW(m_show);
	m_show.Format(_T("%0.2f"), m_pPoints3DWnd->m_GridRadius);
	GetDlgItem(IDC_EDIT_GRIDRADIUS)->SetWindowTextW(m_show);
	m_show.Format(_T("%2d"), m_pPoints3DWnd->m_GridConCircle);
	GetDlgItem(IDC_EDIT_GRIDCONCIRCLE)->SetWindowTextW(m_show);

	m_show.Format(_T("%0.2f"), m_pPoints3DWnd->m_GridWidth);
	GetDlgItem(IDC_EDIT_THICKNESS)->SetWindowTextW(m_show);

	VECTOR GridColor = m_pPoints3DWnd->m_GridColor;
	int red = GridColor.x * 255;
	int green = GridColor.y * 255;
	int blue = GridColor.z * 255;
	m_GridColorPicker.SetColor(RGB(red, green, blue));
	m_GridColorPicker.EnableOtherButton(_T("自定义"));

	bool isAxisNodeShow = true;
	LINEPLANE_TYPE BaseLineType = NONEVIEW;
	pMain->Get_GridView_Bitmap(BaseLineType);
	m_pPoints3DWnd->GetAxisNodeIsShow(isAxisNodeShow);
	if (NONEVIEW == BaseLineType)
	{
		m_BaseLineCheckBox.SetCheck(false);
	}
	else
	{
		m_BaseLineCheckBox.SetCheck(true);
	}
	m_AxisNodeCheckBox.SetCheck(isAxisNodeShow);

	bool isFilter = true;
	pView->Get_IsFilter(isFilter);
	m_FilterCheckBox.SetCheck(isFilter);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

/**************************************************************************
*  @Function Name :: OnDeltaposSpin
*  @Description   :: 点击spin的响应函数
*  @Author        :: zzw
*  @Parameters    :: pNMHDR:记录了触发的spin的ID
*					 pResult:返回的运行结果
*  @Return        :: None
**************************************************************************/
void GridSetDlg::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	UINT ID_FROM= pNMUpDown->hdr.idFrom;
	UINT ID_EDIT=0;//所要改变的EDIT的ID
	int mode = -1;//索要进行的反应，因为spin点击之后，不是完全相同的反应
	switch (ID_FROM)
	{
	case IDC_SPIN_GRIDX:
		ID_EDIT = IDC_EDIT_GRIDX; 
		mode = 0; 
		break;
	case IDC_SPIN_GRIDY:
		ID_EDIT = IDC_EDIT_GRIDY; 
		mode = 0; 
		break;
	case IDC_SPIN_GRIDZ:
		ID_EDIT = IDC_EDIT_GRIDZ; 
		mode = 0; 
		break;
	case IDC_SPIN_GRIDLENGTH:
		ID_EDIT = IDC_EDIT_GRIDLENGTH; 
		mode = 1; 
		break;
	case IDC_SPIN_RESOLUTION:
		ID_EDIT = IDC_EDIT_RESOLUTION; 
		mode = 2; 
		break;
	case IDC_SPIN_GRIDRADIUS:
		ID_EDIT = IDC_EDIT_GRIDRADIUS; 
		mode = 1; 
		break;
	case IDC_SPIN_GRIDCONCIRCLE:
		ID_EDIT = IDC_EDIT_GRIDCONCIRCLE; 
		mode = 2; 
		break;
	case IDC_SPIN_THICKNESS:
		ID_EDIT = IDC_EDIT_THICKNESS;
		mode = 1;
		break;
	default:
		break;
	}
	// TODO: 在此添加控件通知处理程序代码
	CString m_edit;
	GetDlgItem(ID_EDIT)->GetWindowTextW(m_edit);

	if (0 == mode)
	{//可正可负
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
	{//非负数
		double m_editDouble_number = _ttof(m_edit);
		if (pNMUpDown->iDelta < 0)
		{
			m_editDouble_number = m_editDouble_number + 1;
		}
		if (pNMUpDown->iDelta > 0)
		{
			m_editDouble_number = m_editDouble_number - 1;
		}
		if (m_editDouble_number > 200)
		{
			m_editDouble_number = 200;
		}
		if (m_editDouble_number < 0)
		{
			m_editDouble_number = 0;
		}
		m_edit.Format(_T("%0.2f"), m_editDouble_number);
		GetDlgItem(ID_EDIT)->SetWindowTextW(m_edit);
	}
	else if (2 == mode)
	{//正整数
		int m_editInt_number = _ttoi(m_edit);
		if (pNMUpDown->iDelta < 0)
		{
			m_editInt_number = m_editInt_number + 1;
		}
		if (pNMUpDown->iDelta > 0)
		{
			m_editInt_number = m_editInt_number - 1;
		}
		if (m_editInt_number > 40)
		{
			m_editInt_number = 40;
		}
		if (m_editInt_number < 1)
		{
			m_editInt_number = 1;
		}
		m_edit.Format(_T("%2d"), m_editInt_number);
		GetDlgItem(ID_EDIT)->SetWindowTextW(m_edit);
	}

	*pResult = 0;
}

/**************************************************************************
*  @Function Name :: OnBnClickedOk
*  @Description   :: 点击确定按钮的响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void GridSetDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CRadarViewView *pView = (CRadarViewView *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView();
	Points3DWnd * m_pPoints3DWnd = pView->Get3DWnd();
	CString m_show;
	//网格数据的读取并更新显示
	GetDlgItem(IDC_EDIT_GRIDX)->GetWindowTextW(m_show);
	double m_editGridX_number = _ttof(m_show);
	GetDlgItem(IDC_EDIT_GRIDY)->GetWindowTextW(m_show);
	double m_editGridY_number = _ttof(m_show);
	GetDlgItem(IDC_EDIT_GRIDZ)->GetWindowTextW(m_show);
	double m_editGridZ_number = _ttof(m_show);

	GetDlgItem(IDC_EDIT_GRIDLENGTH)->GetWindowTextW(m_show);
	double m_editGridLength_number = _ttof(m_show);
	GetDlgItem(IDC_EDIT_RESOLUTION)->GetWindowTextW(m_show);
	int m_editGridResolution_number = _ttoi(m_show);
	GetDlgItem(IDC_EDIT_GRIDRADIUS)->GetWindowTextW(m_show);
	double m_editGridRadius_number = _ttof(m_show);
	GetDlgItem(IDC_EDIT_GRIDCONCIRCLE)->GetWindowTextW(m_show);
	int m_editGridConCircle_number = _ttoi(m_show);
	GetDlgItem(IDC_EDIT_THICKNESS)->GetWindowTextW(m_show);
	double m_editGridWidth= _ttof(m_show);

	COLORREF color = m_GridColorPicker.GetColor();//获取颜色
	if (color == -1)
	{
		color = m_GridColorPicker.GetAutomaticColor();
	}
	VECTOR GridColor;
	GridColor.x = float(GetRValue(color) / 255.0);
	GridColor.y = float(GetGValue(color) / 255.0);
	GridColor.z = float(GetBValue(color) / 255.0);

	//不显示网格
	if (m_BaseLineCheckBox.GetCheck() == 0)
	{
		pView->SetGridView(NONEVIEW);
	}
	else//显示
	{
		//查看之前是否有显示，有的话不进行设置
		LINEPLANE_TYPE BaseLineType = NONEVIEW;
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		pMain->Get_GridView_Bitmap(BaseLineType);
		if (NONEVIEW == BaseLineType)
		{
			pView->SetGridView(XOY);
		}
	}

	//不显示坐标轴
	if (m_AxisNodeCheckBox.GetCheck() == 0)
	{
		m_pPoints3DWnd->SetAxisNodeIsShow(false);
	}
	else//显示
	{
		m_pPoints3DWnd->SetAxisNodeIsShow(true);
	}

	if (m_FilterCheckBox.GetCheck() == 0)
	{//不进行滤波
		pView->Set_IsFilter(false);
	}
	else//滤波
	{
		pView->Set_IsFilter(true);
	}

	m_pPoints3DWnd->SetGridColor(GridColor);
	m_pPoints3DWnd->SetGridThickness(m_editGridWidth);
	m_pPoints3DWnd->SetGridCenter(m_editGridX_number, m_editGridY_number, m_editGridZ_number);
	m_pPoints3DWnd->SetGridParameters((float)m_editGridLength_number, m_editGridResolution_number);
	m_pPoints3DWnd->SetCircleParameters((float)m_editGridRadius_number, m_editGridConCircle_number);
	

	//修改状态栏的网格信息,首先得得到网格的类型
	CString gridInfo;
	if (GRID == m_pPoints3DWnd->GetGirdType())
	{//网格
		gridInfo.Format(_T("栅格边长:%0.2f m"), (float)m_editGridLength_number / m_editGridResolution_number);
		m_pPoints3DWnd->SetAxisNodeSize((float)m_editGridLength_number / m_editGridResolution_number * 2);
	}
	else if(CON_CIRCLE == m_pPoints3DWnd->GetGirdType())
	{//同心圆
		gridInfo.Format(_T("同心圆半径:%0.2f m"), (float)m_editGridRadius_number/ m_editGridConCircle_number);
		m_pPoints3DWnd->SetAxisNodeSize((float)m_editGridRadius_number / m_editGridConCircle_number * 2);
	}
	pView->ChangStatusTEXT(3, (LPARAM)LPCTSTR(gridInfo));

	CDialogEx::OnOK();
}

/**************************************************************************
*  @Function Name :: OnBnClickedReset
*  @Description   :: 点击重置按钮的响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void GridSetDlg::OnBnClickedReset()
{
	// TODO: 在此添加控件通知处理程序代码
	//进行网格数据的同步更新并显示
	GetDlgItem(IDC_EDIT_GRIDX)->SetWindowTextW(_T("0.00"));
	GetDlgItem(IDC_EDIT_GRIDY)->SetWindowTextW(_T("0.00"));
	GetDlgItem(IDC_EDIT_GRIDZ)->SetWindowTextW(_T("0.00"));

	//重置大小感觉应该针对数据来
	CRadarViewView *pView = (CRadarViewView *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView();
	Points3DWnd * m_pPoints3DWnd = pView->Get3DWnd();
	double GridLength=0.0f;
	double GridRadius=0.0f;
	m_pPoints3DWnd->GetDefaultBoundBox(GridLength, GridRadius);

	//当其为0,也就是可能还没有读入点的时候，设置默认的200和100
	if (GridLength == 0.0)
	{
		GridLength = 200.0;
	}
	if (GridRadius == 0.0)
	{
		GridRadius = 100.0;
	}
	CString m_show;
	m_show.Format(_T("%0.2f"), GridLength);
	GetDlgItem(IDC_EDIT_GRIDLENGTH)->SetWindowTextW(m_show);
	GetDlgItem(IDC_EDIT_RESOLUTION)->SetWindowTextW(_T("20"));
	m_show.Format(_T("%0.2f"), GridRadius);
	GetDlgItem(IDC_EDIT_GRIDRADIUS)->SetWindowTextW(m_show);
	GetDlgItem(IDC_EDIT_GRIDCONCIRCLE)->SetWindowTextW(_T("10"));
	GetDlgItem(IDC_EDIT_THICKNESS)->SetWindowTextW(_T("1.00"));
	m_GridColorPicker.SetColor(RGB(127, 127, 127));

	m_BaseLineCheckBox.SetCheck(true);
	m_AxisNodeCheckBox.SetCheck(true);
	m_FilterCheckBox.SetCheck(true);
}
