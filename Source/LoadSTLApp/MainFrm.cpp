
// MainFrm.cpp: CMainFrame 类的实现
//

#include "stdafx.h"
#include "LoadSTLApp.h"

#include "MainFrm.h"
#include "LoadSTLAppDoc.h"
#include "LoadSTLAppView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	//ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	m_stlFILEStatus = false;
	m_pcapFILEStatus = false;
	m_objFILEStatus = false;
	m_calculationStatus = false;
	m_showDataGrid = true;//一开始为显示状态
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}
	m_wndToolBar.Init(IDB_BASIC24, IDB_BASIC24, IDB_BASIC24);
	m_wndToolBar.UpdateColorImage(IDB_BASIC124, IDB_BASIC24, 4);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果不需要可停靠工具栏，则删除这三行
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	if (!m_datashowDlg.Create(this, IDD_SHOWDATA, WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_SIZE_DYNAMIC , IDD_SHOWDATA))
	{
		TRACE0("控制条形式的非模态对话框创建失败！");
		return -1;
	}
	m_datashowDlg.EnableDocking(CBRS_ALIGN_LEFT| CBRS_ALIGN_RIGHT);//可以在任意地方进行停靠
	m_datashowDlg.ShowInitGrid();//初始化显示值
	DockControlBar(&m_datashowDlg);

	return 0;
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_datashowDlg.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	{
		return TRUE;
	}
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

///**************************************************************************
//*  @Function Name :: OnSize
//*  @Description   :: 改变主窗口大小时候调用
//*  @Author        :: zzw
//*  @Parameters    :: nType:窗口显示的形式
//*					 cx:窗口显示的宽 cy:窗口显示的长
//*  @Return        :: None
//**************************************************************************/
//void CMainFrame::OnSize(UINT nType, int cx, int cy)
//{
//	CFrameWnd::OnSize(nType, cx, cy);
//	if (nType != SIZE_MINIMIZED)
//	{
//		static int tmpidx11 = 0;
//		tmpidx11++;
//		if (tmpidx11>1)//第一次初始化的时候并不执行
//		{
//			//目标是使得m_datashowDlg的大小和主窗口一样
//			//::SendMessage(m_datashowDlg.GetSafeHwnd(), WM_SIZE, cx/2, cy);//1代表需要尺度判断
//			CRect rc0, rc1, rc2;
//			GetClientRect(&rc0);//先取得主窗口对话框的大小及位置
//			rc1.top = rc0.top;
//			rc1.left = rc0.left + 1;
//			rc1.right = rc0.right / 5 - 1;
//			//如果工具栏没有显示
//			if (m_wndToolBar.IsWindowVisible())
//			{
//				rc1.bottom = rc0.bottom - 55;
//			}
//			else
//			{
//				rc1.bottom = rc0.bottom - 23;
//			}
//			if (!m_wndStatusBar.IsWindowVisible())
//			{
//				rc1.bottom += 23;
//			}
//			m_datashowDlg.MoveWindow(rc1, TRUE);
//
//
//			CLoadSTLAppView *pCSTLView = (CLoadSTLAppView *)GetActiveView();
//
//			PointsAndSTL3DWnd *pPointAndSTL3DWnd = pCSTLView->GetShow3DWnd();
//			pCSTLView->GetClientRect(&rc2);
//			//如果工具栏没有显示
//			if (m_wndToolBar.IsWindowVisible())
//			{
//				rc2.top = rc0.top + 32;
//				rc2.bottom = rc2.bottom + 36;
//			}
//			else
//			{
//				rc2.top = rc0.top;
//				rc2.bottom = rc2.bottom+4;
//			}
//			
//			rc2.left = rc0.right/5 +1;
//			rc2.right = rc0.right;
//			
//			pCSTLView->MoveWindow(rc2, TRUE);
//		}
//	}
//}

void CMainFrame::OnPaint()
{
	CFrameWnd::OnPaint();
	//目标是使得m_datashowDlg的大小和主窗口一样
	//::SendMessage(m_datashowDlg.GetSafeHwnd(), WM_SIZE, cx/2, cy);//1代表需要尺度判断
	CRect rc0, rc1, rc2;
	GetClientRect(&rc0);//先取得主窗口对话框的大小及位置
	rc1.top = rc0.top;
	rc1.left = rc0.left + 1;
	rc1.right = rc0.right / 5 - 1;
	//如果工具栏没有显示
	if (m_wndToolBar.IsWindowVisible())
	{
		rc1.bottom = rc0.bottom - 55;
	}
	else
	{
		rc1.bottom = rc0.bottom - 23;
	}
	if (!m_wndStatusBar.IsWindowVisible())
	{
		rc1.bottom += 23;
	}
	m_datashowDlg.MoveWindow(rc1, TRUE);


	CLoadSTLAppView *pCSTLView = (CLoadSTLAppView *)GetActiveView();

	PointsAndSTL3DWnd *pPointAndSTL3DWnd = pCSTLView->GetShow3DWnd();
	pCSTLView->GetClientRect(&rc2);
	//如果工具栏没有显示
	if (m_wndToolBar.IsWindowVisible())
	{
		rc2.top = rc0.top + 32;
		rc2.bottom = rc2.bottom + 36;
	}
	else
	{
		rc2.top = rc0.top;
		rc2.bottom = rc2.bottom + 4;
	}

	if (m_showDataGrid)
	{
		rc2.left = rc0.right / 5 + 1;
	}
	
	rc2.right = rc0.right;

	pCSTLView->MoveWindow(rc2, TRUE);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	//cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
	//	 | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

//设置pcap文件状态
void CMainFrame::Set_PCAPFILEStatus(bool Set_PCAPStatus)
{
	m_pcapFILEStatus = Set_PCAPStatus;
	if (m_pcapFILEStatus)
	{
		m_wndToolBar.UpdateColorImage(IDB_BASIC124, IDB_BASIC24, 0);
	}
	else
	{
		m_wndToolBar.UpdateColorImage(IDB_BASIC24, IDB_BASIC124, 0);
	}
	m_wndToolBar.RedrawWindow();
}

//设置stl文件状态
void CMainFrame::Set_STLFILEStatus(bool Set_STLStatus)
{
	m_stlFILEStatus = Set_STLStatus;
	if (m_stlFILEStatus)
	{
		m_wndToolBar.UpdateColorImage(IDB_BASIC124, IDB_BASIC24, 1);
	}
	else
	{
		m_wndToolBar.UpdateColorImage(IDB_BASIC24, IDB_BASIC124, 1);
	}
	m_wndToolBar.RedrawWindow();

}

//设置obj文件状态
void CMainFrame::Set_OBJFILEStatus(bool Set_OBJStatus)
{
	m_objFILEStatus = Set_OBJStatus;
	if (m_objFILEStatus)
	{
		m_wndToolBar.UpdateColorImage(IDB_BASIC124, IDB_BASIC24, 2);
	}
	else
	{
		m_wndToolBar.UpdateColorImage(IDB_BASIC24, IDB_BASIC124, 2);
	}
	m_wndToolBar.RedrawWindow();
}

//设置计算状态
void CMainFrame::Set_CalculationStatus(bool Set_calStatus)
{
	m_calculationStatus = Set_calStatus;
	if (m_calculationStatus)
	{
		m_wndToolBar.UpdateColorImage(IDB_BASIC124, IDB_BASIC24, 3);
	}
	else
	{
		m_wndToolBar.UpdateColorImage(IDB_BASIC24, IDB_BASIC124, 3);
	}
	m_wndToolBar.RedrawWindow();
}

//更新窗口标题
void CMainFrame::OnUpdateFrameTitle(BOOL NaDa)
{
	CString csAppName;
	csAppName.Format(AFX_IDS_APP_TITLE);
#ifdef _WIN64
	csAppName = "Calculation  - 1.0 64-bit";
#else
	csAppName = "Calculation  - 1.0 32-bit";
#endif
	SetWindowText(csAppName);
}

//设置显示属性网格
void CMainFrame::Set_ShowDataGrid(bool Set_showDataStatus)
{
	m_showDataGrid = Set_showDataStatus;
	if (m_showDataGrid)//显示
	{
		m_datashowDlg.ShowWindow(SW_SHOW);
		m_wndToolBar.UpdateColorImage(IDB_BASIC124, IDB_BASIC24, 4);
	}
	else//不显示
	{
		m_datashowDlg.ShowWindow(SW_HIDE);
		m_wndToolBar.UpdateColorImage(IDB_BASIC24, IDB_BASIC124, 4);
	}
	m_datashowDlg.RedrawWindow();
	m_wndToolBar.RedrawWindow();
}

//刷新界面显示
void CMainFrame::ReDrawShow()
{
	CRect rect;
	this->GetWindowRect(rect);
	ScreenToClient(rect);
	this->OnSize(SIZE_RESTORED, rect.Width(), rect.Height());
}

//更改显示内容
void CMainFrame::SetShowValue(int PropertyNumber, int SubItemNumber, CString setValue)
{
	m_datashowDlg.SetShowValue(PropertyNumber, SubItemNumber, setValue);
}

//更改显示内容
void CMainFrame::SetShowValue(int PropertyNumber, int SubItemNumber, int setValue)
{
	m_datashowDlg.SetShowValue(PropertyNumber, SubItemNumber, setValue);
}

void CMainFrame::SetShowValue(int PropertyNumber, int SubItemNumber, double setValue)
{
	m_datashowDlg.SetShowValue(PropertyNumber, SubItemNumber, setValue);
}

//将pProp加到第PropertyNumber的SubItemNumber里面
void CMainFrame::AddGridProperty(CMFCPropertyGridProperty *pProp, int PropertyNumber, int SubItemNumber)
{
	m_datashowDlg.AddGridProperty(pProp, PropertyNumber, SubItemNumber);
}

//删除里面内容
void CMainFrame::DelGridProperty(int PropertyNumber, int SubItemNumber)
{
	m_datashowDlg.DelGridProperty(PropertyNumber, SubItemNumber);
}