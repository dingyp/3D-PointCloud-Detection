
// MainFrm.cpp: CMainFrame 类的实现
//

#include "stdafx.h"
#include "RadarView.h"

#include "MainFrm.h"
#include "RadarViewView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVING()
	ON_WM_ACTIVATE()
	ON_WM_LBUTTONDOWN()
	//ON_WM_LBUTTONDOWN()
	ON_COMMAND_EX(ID_BASICTOOLBAR, OnBarCheck)
	ON_COMMAND_EX(ID_COLORTOOLBAR, OnBarCheck)
	ON_COMMAND_EX(ID_PLAYBACKTOOLBAR, OnBarCheck)
	ON_COMMAND_EX(ID_VIEWTOOLBAR, OnBarCheck)
	ON_COMMAND_RANGE(ID_FILE_MRU_FILE_1, ID_FILE_MRU_FILE_10, OnFileMru)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,				//状态行指示器,用来显示命令提示信息
	IDS_STATUS_PANE1,			//显示文件路径信息
	IDS_STATUS_PANE2,			//这个用来显示选中的点的信息
	IDS_STATUS_PANE3,			//用来显示网格信息
	ID_INDICATOR_NUM,
	ID_INDICATOR_CLOCK,			//显示时间属性
};

// CMainFrame 构造/析构
/**************************************************************************
*  @Function Name :: CMainFrame
*  @Description   :: CMainFrame类的构造函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	m_bSTARTSTOPStatue = TRUE;
	m_bNOLOOPStatue = TRUE;
	m_bFileSaveStatue = FALSE;
	//m_bSensorStatue = FALSE;
	m_iReaderStatus = 0;//还没有Reader
	m_iMeasureStatue = 0;//不测量状态
	nowPlaySpeed = ONE_SPEED;//一开始前进和后退都是一倍速度
	m_bCurProjMode = TRUE;//透视投影
	m_bGridDraw = FALSE;//网格
	m_GridView = XOY;//XOY网格
	m_dirViewStatus = RESET;//初始状态时reset
	memset(m_isVisble, TRUE, sizeof(BOOL) * 4);
	memset(m_isFloating, FALSE, sizeof(BOOL) * 4);
}

/**************************************************************************
*  @Function Name :: ~CMainFrame
*  @Description   :: CMainFrame类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CMainFrame::~CMainFrame()
{
}

/**************************************************************************
*  @Function Name :: OnCreate
*  @Description   :: CMainFrame类被创建的时候调用
*  @Author        :: zzw
*  @Parameters    :: lpCreateStruct:OnCreate参数
*
*  @Return        :: 创建的状态
**************************************************************************/
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}


	//if (!m_MyDockablePane.Create(_T("图层"), this, CRect(0, 0, 200, 200), TRUE, 1001, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	//{
	//	TRACE0("Failed to create LeftPane\n");
	//	return -1;
	//}
	//m_MyDockablePane.EnableDocking(CBRS_ALIGN_ANY);
	//DockPane(&m_MyDockablePane);
	//为每个指示器设置显示文本，具体来说，就是用indicators数组中的对应元素为每个指示器设置一个ID,然后
	//加载每个ID代表的字符串，设置为这些指示器的显示文本。参数indicators为指向一个ID数组的指针，后面参数是前面数组中的个数
	if (!m_wndStatusBar.Create(this)||!m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	EnableDocking(CBRS_ALIGN_ANY);
	InitBar(&m_wndBasicBar, IDR_BASICBAR, IDB_BASIC24, IDB_BASIC24, IDB_BASIC24);
	InitBar(&m_wndColorBar, IDR_COLORBAR, IDB_COLOR24, IDB_COLOR24, IDB_COLOR24);
	InitBar(&m_wndViewBar, IDR_VIEWBAR, IDB_VIEW24, IDB_VIEW124, IDB_VIEW24);
	InitBar(&m_wndPlaybackBar, IDR_PLAYBACKBAR, IDB_PLAYBACK24, IDB_PLAYBACK124, IDB_PLAYBACK24);
	m_wndPlaybackBar.InitSlider(PLAYSLIDER_BUTTON, FRAMENUMBER_EDIT);
	m_wndColorBar.InitColorAttribute(COLOR_ATTRIBUTE);

	m_wndViewBar.UpdateColorImage(IDB_VIEW124, IDB_VIEW24, 4);//X_Y

	DockControlBarLeftOf(&m_wndColorBar, &m_wndBasicBar);
	DockControlBarLeftOf(&m_wndPlaybackBar, &m_wndColorBar);
	DockControlBarLeftOf(&m_wndViewBar, &m_wndPlaybackBar);

	CString mruFilesArray[10];
	CRadarViewApp *pViewApp = (CRadarViewApp *)AfxGetApp();
	int mruFilesNumber = pViewApp->m_systemConfigVarManager.Instance().GetMruFile(mruFilesArray);
	//根据配置文件对最近文件菜单栏进行显示
	CMenu *pMRUF_Menu = GetMenu()->GetSubMenu(0);
	CMenu submenu;
	submenu.CreatePopupMenu();//新建菜单
	if (mruFilesNumber > 10)
	{//对菜单栏的长度进行限制
		mruFilesNumber = 10;
	}
	for (int i = 0; i < mruFilesNumber; i++)
	{
		CString shortFilePath;//保存短路径
		pViewApp->m_systemConfigVarManager.Instance().GetShortFilePath(mruFilesArray[i], shortFilePath);
		submenu.AppendMenuW(MF_BYPOSITION | MF_POPUP | MF_ENABLED, UINT(ID_FILE_MRU_FILE_1 + i), shortFilePath);
	}
	pMRUF_Menu->InsertMenuW(1,MF_BYPOSITION|MF_POPUP | MF_ENABLED, (UINT)submenu.m_hMenu,_T("最近文件"));//增加子菜单
	submenu.Detach();//将submenu对象与菜单句柄脱开，析构submenu对象不影响句柄的使用

	//先将网格信息显示到状态栏中,
	BOOL GridDrawStatue = Get_GridDraw_Bitmap();
	CString gridInfo;
	if (GridDrawStatue)
	{//同心圆
		gridInfo.Format(_T("同心圆半径:%0.2f m"), 10.0);
	}
	else
	{//网格
		gridInfo.Format(_T("栅格边长:%0.2f m"), 10.0);
	}
	m_wndStatusBar.SetPaneText(3, gridInfo);
	if (!cameraDialog.Create(this, IDD_DIALOG2, WS_CHILD | WS_VISIBLE | CBRS_LEFT | CBRS_SIZE_DYNAMIC, IDD_DIALOG2))
	{
		TRACE0("控制条形式的非模态对话框创建失败！");
		return -1;
	}
	cameraDialog.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);//可以在任意地方进行停靠
	//cameraDialog.ShowInitGrid();//初始化显示值
	DockControlBar(&cameraDialog);
	return 0;
}

//更新窗口标题
void CMainFrame::OnUpdateFrameTitle(BOOL NaDa)
{
	CString csAppName;
	csAppName.Format(AFX_IDS_APP_TITLE);
#ifdef _WIN64
	csAppName = "OLamViewer - 1.1 64-bit";
#else
	csAppName = "OLamViewer - 1.1 32-bit";
#endif
	SetWindowText(csAppName);
}

/**************************************************************************
*  @Function Name :: OnMoving
*  @Description   :: 主窗口移动时候调用，主要用来移动颜色标尺
*  @Author        :: zzw
*  @Parameters    :: fwSide:OnMoving参数
*					 pRect:OnMoving参数
*  @Return        :: None
**************************************************************************/
void CMainFrame::OnMoving(UINT fwSide, LPRECT pRect)
{
	//重新对非模态子窗口进行绘制
	CRadarViewView *pView = (CRadarViewView *)GetActiveView();
	//只有当有窗口的时候才需要跟着移动
	if (TRUE == pView->GetIsLegendShow())
	{
		pView->MoveLegend();
	}
}

//失去焦点和得到焦点响应函数
void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	CRadarViewView *pView = (CRadarViewView *)GetActiveView();
	if (NULL != pView)
	{
		if (WA_ACTIVE == nState)//窗口是通过鼠标点击以外的方法激活的（例如，通过使用键盘接口来选择窗口）
		{
			pView->SetPoint3DWndFocus();
		}
		else if (WA_CLICKACTIVE == nState)//通过鼠标点击激活窗口
		{
			pView->SetPoint3DWndFocus();
		}
		else if (WA_INACTIVE == nState)//窗口被停用
		{
		}
	}
}

//鼠标左键单击击下的响应函数
void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRadarViewView *pView = (CRadarViewView *)GetActiveView();
	pView->SetPoint3DWndFocus();
}

/**************************************************************************
*  @Function Name :: OnSize
*  @Description   :: 改变主窗口大小时候调用
*  @Author        :: zzw
*  @Parameters    :: nType:窗口显示的形式
*					 cx:窗口显示的宽 cy:窗口显示的长
*  @Return        :: None
**************************************************************************/
void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
	if (nType != SIZE_MINIMIZED)
	{
		static int tmpidx11 = 0;
		tmpidx11++;
		if (tmpidx11>1)//第一次初始化的时候并不执行
		{
			//获取状态栏的索引号
			int ordinaryID = m_wndStatusBar.CommandToIndex(ID_SEPARATOR);
			int filePathID = m_wndStatusBar.CommandToIndex(IDS_STATUS_PANE1);
			int pointInfoID = m_wndStatusBar.CommandToIndex(IDS_STATUS_PANE2);
			int gridInfoID = m_wndStatusBar.CommandToIndex(IDS_STATUS_PANE3);
			int numID = m_wndStatusBar.CommandToIndex(ID_INDICATOR_NUM);
			int clockID = m_wndStatusBar.CommandToIndex(ID_INDICATOR_CLOCK);

			double total_pixel = 150.0 + 470.0+ 280.0 + 100.0 + 50.0 + 100.0;
			m_wndStatusBar.SetPaneInfo(ordinaryID, ID_SEPARATOR, SBPS_NOBORDERS, int(150.0 / total_pixel * (cx-8) ));
			m_wndStatusBar.SetPaneInfo(filePathID, IDS_STATUS_PANE1, SBPS_NOBORDERS, int(470.0 / total_pixel * (cx - 8)));
			m_wndStatusBar.SetPaneInfo(pointInfoID, IDS_STATUS_PANE2, SBPS_NOBORDERS, int(280.0 / total_pixel * (cx - 8)));
			m_wndStatusBar.SetPaneInfo(gridInfoID, IDS_STATUS_PANE3, SBPS_NOBORDERS, int(100.0 / total_pixel * (cx - 8)));
			m_wndStatusBar.SetPaneInfo(numID, ID_INDICATOR_NUM, SBPS_NOBORDERS, int(50.0 / total_pixel * (cx - 8)));
			m_wndStatusBar.SetPaneInfo(clockID, ID_INDICATOR_CLOCK, SBPS_NOBORDERS, int(100.0 / total_pixel * (cx - 8)));

			CRadarViewView *pView = (CRadarViewView *)GetActiveView();
			pView->SetPoint3DWndFocus();
		}
	}
}

/**************************************************************************
*  @Function Name :: OnFileMru
*  @Description   :: 确定是哪个最近文件被选中,并判定文件是否还在
*  @Author        :: zzw
*  @Parameters    :: nID:最近文件的下标
*
*  @Return        :: None
**************************************************************************/
void CMainFrame::OnFileMru(UINT nID)
{//确定是哪个最近文件被选中
	UINT idx = nID - ID_FILE_MRU_FILE_1;
	CString mruFile;
	CRadarViewApp *pViewApp = (CRadarViewApp *)AfxGetApp();
	if (!pViewApp->m_systemConfigVarManager.Instance().GetMruFile(idx, mruFile))
	{//没有，出现错误
		return;
	}
	//在打开之前先确定是否有这个文件，没有则跳出提示信息之后删除这个文件
	if (!pViewApp->m_systemConfigVarManager.Instance().IsFileExist(mruFile))
	{//文件不存在
		CString tmpshow = _T("文件") + mruFile + _T("已经不存在！");
		MessageBox(tmpshow, _T("提示"));
		//将这个文件从注册表里面删除
		pViewApp->m_systemConfigVarManager.Instance().DelMruFile(mruFile);
		return;
	}

	CRadarViewDoc *pDoc = (CRadarViewDoc *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveDocument();

	pDoc->File_path = mruFile;
	pDoc->isMruFile = 1;//在startPcapReader里面就不需要再打开对话框自己选择文件了，因为是通过最近文件打开的
	pDoc->StartPcapReader();
}

/**************************************************************************
*  @Function Name :: UpdateMenuMruFile
*  @Description   :: 更新菜单栏最近文件
*  @Author        :: zzw
*  @Parameters    :: mruFilesArray:最近文件列表
*					 mruFilesNumber:最近文件总数
*  @Return        :: None
**************************************************************************/
void CMainFrame::UpdateMenuMruFile(CString *mruFilesArray, int mruFilesNumber)
{
	//根据配置文件对最近文件菜单栏进行显示
	CMenu *pMRUF_Menu = GetMenu()->GetSubMenu(0);
	CMenu submenu;
	submenu.CreatePopupMenu();//新建菜单
	if (mruFilesNumber > 10)
	{//对菜单栏的长度进行限制
		mruFilesNumber = 10;
	}
	CRadarViewApp *pViewApp = (CRadarViewApp *)AfxGetApp();
	for (int i = 0; i < mruFilesNumber; i++)
	{
		CString shortFilePath;//保存短路径
		pViewApp->m_systemConfigVarManager.Instance().GetShortFilePath(mruFilesArray[i], shortFilePath);
		submenu.AppendMenuW(MF_BYPOSITION | MF_POPUP | MF_ENABLED, UINT(ID_FILE_MRU_FILE_1 + i), shortFilePath);
	}
	pMRUF_Menu->DeleteMenu(1, MF_BYPOSITION | MF_POPUP | MF_ENABLED);
	pMRUF_Menu->InsertMenuW(1, MF_BYPOSITION | MF_POPUP | MF_ENABLED, (UINT)submenu.m_hMenu, _T("最近文件"));//增加子菜单
	submenu.Detach();//将submenu对象与菜单句柄脱开，析构submenu对象不影响句柄的使用
}

/**************************************************************************
*  @Function Name :: InitBar
*  @Description   :: 初始化工具栏
*  @Author        :: zzw
*  @Parameters    :: bar:工具栏指针 Bar_ID:工具栏ID BITMap_ID:位图ID
*					 Disabled_ID:disable状态位图ID Hot_ID:热键状态位图ID
*  @Return        :: 返回是否初始化成功
**************************************************************************/
BOOL CMainFrame::InitBar(CToolBar24* bar, UINT Bar_ID, UINT BITMap_ID, UINT Disabled_ID, UINT Hot_ID)
{
	if (!bar->CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!bar->LoadToolBar(Bar_ID))
	{
		TRACE0("未能创建工具栏\n");
		return false;       //未能创建
	}
	bar->Init(BITMap_ID, Disabled_ID, Hot_ID);
	bar->EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(bar);
}

/**************************************************************************
*  @Function Name :: OnBarCheck
*  @Description   :: 对菜单栏中工具栏选择部分进行打勾显示
*  @Author        :: zzw
*  @Parameters    :: nID:工具栏ID,用来区别工具栏
*
*  @Return        :: 返回是否成功
**************************************************************************/
BOOL CMainFrame::OnBarCheck(UINT nID)
{
	int idx = -1;
	CToolBar24 *toolBarArray[4] = { &m_wndBasicBar ,&m_wndColorBar ,&m_wndPlaybackBar ,&m_wndViewBar };
	switch (nID)
	{
	case ID_BASICTOOLBAR:
		idx = 0;
		break;
	case ID_COLORTOOLBAR:
		idx = 1;
		break;
	case ID_PLAYBACKTOOLBAR:
		idx = 2;
		break;
	case ID_VIEWTOOLBAR:
		idx = 3;
		break;
	}
	if ((-1 == idx)||(NULL == toolBarArray[idx]))
	{
		return FALSE;
	}

	if (toolBarArray[idx]->IsWindowVisible() && m_isVisble[idx])
	{
		if (toolBarArray[idx]->IsFloating())
		{
			m_isFloating[idx] = TRUE;
			toolBarArray[idx]->GetDockingFrame()->GetWindowRect(m_rect + idx);
		}
		toolBarArray[idx]->ShowWindow(SW_HIDE);
		DockControlBar(toolBarArray[idx]);
		m_isVisble[idx] = FALSE;
	}
	else if (!toolBarArray[idx]->IsWindowVisible() && !m_isVisble[idx])
	{
		toolBarArray[idx]->ShowWindow(SW_SHOW);
		if (m_isFloating[idx])
		{
			FloatControlBar(toolBarArray[idx],CPoint(m_rect[idx].left, m_rect[idx].top),CBRS_ALIGN_TOP);
		}
		m_isVisble[idx] = TRUE;
		m_isFloating[idx] = FALSE;
	}
	else
	{
		toolBarArray[idx]->ShowWindow(SW_SHOW);
		FloatControlBar(toolBarArray[idx], CPoint(toolBarArray[idx]->rect.left, toolBarArray[idx]->rect.top), CBRS_ALIGN_BOTTOM);
	}
	RecalcLayout();
	GetMenu()->GetSubMenu(1)->CheckMenuItem(idx, MF_BYPOSITION | (m_isVisble[idx] ? MF_CHECKED : MF_UNCHECKED));
	return TRUE;
}

/**************************************************************************
*  @Function Name :: SetAttriComBoxCurSel
*  @Description   :: 设置AttributeComBox的值
*  @Author        :: zzw
*  @Parameters    :: setSel:设置的下标
*					 bUpdate:表示更改颜色编码属性之后是否redraw,默认否
*  @Return        :: None
**************************************************************************/
void CMainFrame::SetAttriComBoxCurSel(int setSel, bool bUpdate)
{
	if (setSel>=0)
	{
		m_wndColorBar.SetAttriComBoxCurSel(setSel, bUpdate);
	}
}

/**************************************************************************
*  @Function Name :: DockControlBarLeftOf
*  @Description   :: 将某个toolbar显示到另外一个的右边
*  @Author        :: zzw
*  @Parameters    :: bar:显示到右边的工具栏指针
*					 leftof:显示到左边的工具栏指针
*  @Return        :: None
**************************************************************************/
void CMainFrame::DockControlBarLeftOf(CToolBar24* bar, CToolBar24* leftof)
{
	//设置工具条并列停靠到同一条边上
	CRect rect;
	DWORD dw;
	UINT n = 0;
	RecalcLayout();
	leftof->GetWindowRect(rect);
	rect.OffsetRect(1, 0);
	dw = leftof->GetBarStyle();
	n = (dw & CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw & CBRS_ALIGN_BOTTOM && n == 0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw & CBRS_ALIGN_LEFT && n == 0) ? AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw & CBRS_ALIGN_RIGHT && n == 0) ? AFX_IDW_DOCKBAR_RIGHT : n;
	DockControlBar(bar, n, &rect);
}

/**************************************************************************
*  @Function Name :: Set_ORTHOGONAL_Bitmap
*  @Description   :: 设置正交/投影视图位图
*  @Author        :: zzw
*  @Parameters    :: Set_OrthGonalStatus:设置哪种位图
*
*  @Return        :: None
**************************************************************************/
void CMainFrame::Set_ORTHOGONAL_Bitmap(bool Set_OrthGonalStatus)
{
	if (m_bCurProjMode == Set_OrthGonalStatus)
	{
		return;
	}
	m_bCurProjMode = Set_OrthGonalStatus;
	if (Set_OrthGonalStatus)
	{
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 1);
	}
	else 
	{
		m_wndViewBar.UpdateColorImage(IDB_VIEW124, IDB_VIEW24, 1);
	}
	m_wndViewBar.RedrawWindow();
}

/**************************************************************************
*  @Function Name :: Set_Measure_Bitmap
*  @Description   :: 设置测量位图
*  @Author        :: zzw
*  @Parameters    :: Set_MeasureStatus:设置哪种测量位图
*
*  @Return        :: None
**************************************************************************/
void CMainFrame::Set_Measure_Bitmap(int Set_MeasureStatus)
{//现在有三种不同的模式
	m_iMeasureStatue = Set_MeasureStatus;
	if (0 == m_iMeasureStatue)
	{
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 2);
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 3);
	}
	else if (1== m_iMeasureStatue)
	{
		m_wndViewBar.UpdateColorImage(IDB_VIEW124, IDB_VIEW24, 2);
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 3);
	}
	else//测量距离零点的模式
	{
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 2);
		m_wndViewBar.UpdateColorImage(IDB_VIEW124, IDB_VIEW24, 3);
	}
	m_wndViewBar.RedrawWindow();
}

/**************************************************************************
*  @Function Name :: Set_PlayStop_Bitmap
*  @Description   :: 播放结束设置菜单栏
*  @Author        :: zzw
*  @Parameters    :: pParam:主窗口指针，因为这是全局函数
*
*  @Return        :: None
**************************************************************************/
void CMainFrame::Set_PlayStop_Bitmap(LPVOID pParam)
{
	CMainFrame *pMain = (CMainFrame *)pParam;
	pMain->Set_STARTSTOP_Bitmap(true);//播放结束暂停，此时也可以进行测量
	
	int MeasureStatus = pMain->Get_Measure_Bitmap();
	if (MeasureStatus == 0)
	{//如果没有在测量，就使之测量
		CRadarViewView *pView = (CRadarViewView *)pMain->GetActiveView();
		pView->SetRularStatus(1);
	}
}

///**************************************************************************
//*  @Function Name :: GetPos
//*  @Description   :: 得到播放工具栏滑动条的位置
//*  @Author        :: zzw
//*  @Parameters    :: None
//*
//*  @Return        :: None
//**************************************************************************/
//int CMainFrame::GetPos() const
//{
//	return 0;
//}

/**************************************************************************
*  @Function Name :: SetPos
*  @Description   :: 设置播放工具栏滑动条的位置
*  @Author        :: zzw
*  @Parameters    :: nPos:设置的位置
*
*  @Return        :: None
**************************************************************************/
int CMainFrame::SetPos(unsigned int nPos)
{
	m_wndPlaybackBar.SetFrameNumber(nPos);
	return 0;
}

/**************************************************************************
*  @Function Name :: SetTotalFrameNumber
*  @Description   :: 设置总的帧数
*  @Author        :: zzw
*  @Parameters    :: totalNumber:总的帧数
*
*  @Return        :: None
**************************************************************************/
void CMainFrame::SetTotalFrameNumber(unsigned int totalNumber)
{
	m_wndPlaybackBar.InitFrameNumber(totalNumber);
}

/**************************************************************************
*  @Function Name :: Set_GridDraw_Bitmap
*  @Description   :: 设置网格种类的位图
*  @Author        :: zzw
*  @Parameters    :: Set_GridDraw:网格种类
*
*  @Return        :: None
**************************************************************************/
void CMainFrame::Set_GridDraw_Bitmap(bool Set_GridDraw)
{
	m_bGridDraw = Set_GridDraw;
	if (Set_GridDraw)
	{//同心圆
		m_wndViewBar.UpdateColorImage(IDB_VIEW124, IDB_VIEW24, 15);//concircle
	}
	else
	{//网格
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 15);//grid
	}
	m_wndViewBar.RedrawWindow();
}

/**************************************************************************
*  @Function Name :: Set_GridView_Bitmap
*  @Description   :: 设置网格方向的位图
*  @Author        :: zzw
*  @Parameters    :: eLinePlaneType:网格方向
*
*  @Return        :: None
**************************************************************************/
void CMainFrame::Set_GridView_Bitmap(LINEPLANE_TYPE eLinePlaneType)
{
	m_GridView = eLinePlaneType;
	switch(m_GridView)
	{
	case XOY:
		m_wndViewBar.UpdateColorImage(IDB_VIEW124, IDB_VIEW24, 4);//X_Y
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 5);//X_Z
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 6);//Y_Z
		break;
	case XOZ:
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 4);//X_Y
		m_wndViewBar.UpdateColorImage(IDB_VIEW124, IDB_VIEW24, 5);//X_Z
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 6);//Y_Z
		break;
	case YOZ:
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 4);//X_Y
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 5);//X_Z
		m_wndViewBar.UpdateColorImage(IDB_VIEW124, IDB_VIEW24, 6);//Y_Z
		break;
	case NONEVIEW:
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 4);//X_Y
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 5);//X_Z
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 6);//Y_Z
		break;
	}
	m_wndViewBar.RedrawWindow();
}

/**************************************************************************
*  @Function Name :: Get_GridView_Bitmap
*  @Description   :: 得到网格方向
*  @Author        :: zzw
*  @Parameters    :: eLinePlaneType:网格方向
*
*  @Return        :: None
**************************************************************************/
void CMainFrame::Get_GridView_Bitmap(LINEPLANE_TYPE &eLinePlaneType) const
{
	eLinePlaneType = m_GridView;
}

/**************************************************************************
*  @Function Name :: Get_Dir_Bitmap
*  @Description   :: 得到视野的方向
*  @Author        :: zzw
*  @Parameters    :: viewDir:视野方向的引用
*
*  @Return        :: None
**************************************************************************/
void CMainFrame::Get_Dir_Bitmap(VIEW_DIR &viewDir)
{
	viewDir = m_dirViewStatus;
}

/**************************************************************************
*  @Function Name :: Set_Dir_Bitmap
*  @Description   :: 设置视野的方向
*  @Author        :: zzw
*  @Parameters    :: viewDir:视野的方向
*
*  @Return        :: None
**************************************************************************/
void CMainFrame::Set_Dir_Bitmap(VIEW_DIR viewDir)
{
	m_dirViewStatus = viewDir;
	switch (m_dirViewStatus)
	{
	case RESET:
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 7);//XP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 8);//XS
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 9);//YP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 10);//YS
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 11);//ZP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 12);//ZS
		break;
	case XP:
		m_wndViewBar.UpdateColorImage(IDB_VIEW124, IDB_VIEW24, 7);//XP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 8);//XS
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 9);//YP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 10);//YS
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 11);//ZP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 12);//ZS
		break;
	case XS:
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 7);//XP
		m_wndViewBar.UpdateColorImage(IDB_VIEW124, IDB_VIEW24, 8);//XS
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 9);//YP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 10);//YS
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 11);//ZP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 12);//ZS
		break;
	case YP:
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 7);//XP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 8);//XS
		m_wndViewBar.UpdateColorImage(IDB_VIEW124, IDB_VIEW24, 9);//YP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 10);//YS
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 11);//ZP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 12);//ZS
		break;
	case YS:
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 7);//XP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 8);//XS
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 9);//YP
		m_wndViewBar.UpdateColorImage(IDB_VIEW124, IDB_VIEW24, 10);//YS
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 11);//ZP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 12);//ZS
		break;
	case ZP:
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 7);//XP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 8);//XS
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 9);//YP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 10);//YS
		m_wndViewBar.UpdateColorImage(IDB_VIEW124, IDB_VIEW24, 11);//ZP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 12);//ZS
		break;
	case ZS:
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 7);//XP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 8);//XS
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 9);//YP
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 10);//YS
		m_wndViewBar.UpdateColorImage(IDB_VIEW24, IDB_VIEW124, 11);//ZP
		m_wndViewBar.UpdateColorImage(IDB_VIEW124, IDB_VIEW24, 12);//ZS
		break;
	}
	m_wndViewBar.RedrawWindow();
}

/**************************************************************************
*  @Function Name :: Get_Speed_Bitmap
*  @Description   :: 得到速度
*  @Author        :: zzw
*  @Parameters    :: getSpeed:速度的引用
*
*  @Return        :: None
**************************************************************************/
void CMainFrame::Get_Speed_Bitmap(int &getSpeed)
{
	getSpeed = nowPlaySpeed;
}

/**************************************************************************
*  @Function Name :: Set_ForWard_Bitmap
*  @Description   :: 设置前进速度位图
*  @Author        :: zzw
*  @Parameters    :: setSpeed:速度大小
*
*  @Return        :: None
**************************************************************************/
void CMainFrame::Set_ForWard_Bitmap(int setSpeed)
{
	m_wndPlaybackBar.UpdateColorImage(IDB_PLAYBACK24, IDB_PLAYBACK24, 1);
	nowPlaySpeed = setSpeed;
	switch (nowPlaySpeed)
	{
	case ONE_SPEED|| DEFAULT_SPEED:
		m_wndPlaybackBar.UpdateColorImage(IDB_PLAYBACK24, IDB_PLAYBACK24, 3);
		break;
	case DOUBLE_SPEED :
		m_wndPlaybackBar.UpdateColorImage(IDI_FORWARD224, IDI_FORWARD224, IDI_FORWARD224, 3);
		break;
	case THREE_SPEED:
		m_wndPlaybackBar.UpdateColorImage(IDI_FORWARD324, IDI_FORWARD324, IDI_FORWARD324, 3);
		break;
	case HALF_SPEED:
		m_wndPlaybackBar.UpdateColorImage(IDI_FORWARD1_224, IDI_FORWARD1_224, IDI_FORWARD1_224, 3);
		break;
	case ONE_THRID_SPEED:
		m_wndPlaybackBar.UpdateColorImage(IDI_FORWARD1_324, IDI_FORWARD1_324, IDI_FORWARD1_324, 3);
		break;
	}
	m_wndPlaybackBar.RedrawWindow();
}

/**************************************************************************
*  @Function Name :: Set_Backward_Bitmap
*  @Description   :: 设置回退速度位图
*  @Author        :: zzw
*  @Parameters    :: setSpeed:速度大小
*
*  @Return        :: None
**************************************************************************/
void CMainFrame::Set_Backward_Bitmap(int setSpeed)
{
	m_wndPlaybackBar.UpdateColorImage(IDB_PLAYBACK24, IDB_PLAYBACK24, 3);
	nowPlaySpeed = setSpeed;
	switch (nowPlaySpeed)
	{
	case ONE_SPEED || DEFAULT_SPEED:
		m_wndPlaybackBar.UpdateColorImage(IDB_PLAYBACK24, IDB_PLAYBACK24, 1);
		break;
	case DOUBLE_SPEED:
		m_wndPlaybackBar.UpdateColorImage(IDI_BACKWARD224, IDI_BACKWARD224, IDI_BACKWARD224, 1);
		break;
	case THREE_SPEED:
		m_wndPlaybackBar.UpdateColorImage(IDI_BACKWARD324, IDI_BACKWARD324, IDI_BACKWARD324, 1);
		break;
	case HALF_SPEED:
		m_wndPlaybackBar.UpdateColorImage(IDI_BACKWARD1_224, IDI_BACKWARD1_224, IDI_BACKWARD1_224, 1);
		break;
	case ONE_THRID_SPEED:
		m_wndPlaybackBar.UpdateColorImage(IDI_BACKWARD1_324, IDI_BACKWARD1_324, IDI_BACKWARD1_324, 1);
		break;
	}
	m_wndPlaybackBar.RedrawWindow();
}

/**************************************************************************
*  @Function Name :: Set_STARTSTOP_Bitmap
*  @Description   :: 设置开始暂停位图
*  @Author        :: zzw
*  @Parameters    :: Set_StartStopStatue:开始暂停的状态
*
*  @Return        :: None
**************************************************************************/
void CMainFrame::Set_STARTSTOP_Bitmap(bool Set_StartStopStatue)
{
	m_bSTARTSTOPStatue = Set_StartStopStatue;
	if (m_bSTARTSTOPStatue) 
	{//开始
		m_wndPlaybackBar.UpdateColorImage(IDB_PLAYBACK24, IDB_PLAYBACK124, 2);
	}
	else 
	{//暂停
		m_wndPlaybackBar.UpdateColorImage(IDB_PLAYBACK24, IDB_PLAYBACK124, 1);
		m_wndPlaybackBar.UpdateColorImage(IDB_PLAYBACK124, IDB_PLAYBACK24, 2);
		m_wndPlaybackBar.UpdateColorImage(IDB_PLAYBACK24, IDB_PLAYBACK124, 3);
	}
	m_wndPlaybackBar.RedrawWindow();
}

/**************************************************************************
*  @Function Name :: Set_LOOP_Bitmap
*  @Description   :: 设置循环的位图
*  @Author        :: zzw
*  @Parameters    :: Set_LoopStatue:循环的状态
*
*  @Return        :: None
**************************************************************************/
void CMainFrame::Set_LOOP_Bitmap(bool Set_LoopStatue)
{
	m_bNOLOOPStatue = Set_LoopStatue;
	if (m_bNOLOOPStatue)
	{
		m_wndPlaybackBar.UpdateColorImage(IDB_PLAYBACK24, IDB_PLAYBACK124, 5);
	}
	else 
	{
		m_wndPlaybackBar.UpdateColorImage(IDB_PLAYBACK124, IDB_PLAYBACK24, 5);
	}
	m_wndPlaybackBar.RedrawWindow();
}

/**************************************************************************
*  @Function Name :: Set_FileSave_Bitmap
*  @Description   :: 设置文件保存的位图
*  @Author        :: zzw
*  @Parameters    :: Set_FileSaveStatue:文件保存的状态
*
*  @Return        :: None
**************************************************************************/
void CMainFrame::Set_FileSave_Bitmap(bool Set_FileSaveStatue)
{
	m_bFileSaveStatue = Set_FileSaveStatue;
	if (m_bFileSaveStatue)
	{
		m_wndBasicBar.UpdateColorImage(IDB_BASIC124, IDB_BASIC24, 2);
	}
	else
	{
		m_wndBasicBar.UpdateColorImage(IDB_BASIC24, IDB_BASIC124, 2);
	}
	m_wndBasicBar.RedrawWindow();
}

/**************************************************************************
*  @Function Name :: Set_ReaderStatus
*  @Description   :: 设置读取的状态
*  @Author        :: zzw
*  @Parameters    :: Set_ReaderStatus:读取的种类
*
*  @Return        :: None
**************************************************************************/
void CMainFrame::Set_ReaderStatus(int Set_ReaderStatus)
{
	m_iReaderStatus = Set_ReaderStatus;
	switch (m_iReaderStatus)
	{//是哪种Reader，0代表还没有，1代表FileReader,2代表SensorReader,3代表CSVReader
	case 0://还没有
		m_wndBasicBar.UpdateColorImage(IDB_BASIC24, IDB_BASIC124, 0);
		m_wndBasicBar.UpdateColorImage(IDB_BASIC24, IDB_BASIC124, 1);
		break;
	case 1://FileReader
		m_wndBasicBar.UpdateColorImage(IDB_BASIC124, IDB_BASIC24, 0);
		m_wndBasicBar.UpdateColorImage(IDB_BASIC24, IDB_BASIC124, 1);
		break;
	case 2://SensorReader
		m_wndBasicBar.UpdateColorImage(IDB_BASIC24, IDB_BASIC124, 0);
		m_wndBasicBar.UpdateColorImage(IDB_BASIC124, IDB_BASIC24, 1);
		break;
	case 3://CSVReader
		m_wndBasicBar.UpdateColorImage(IDB_BASIC24, IDB_BASIC124, 0);
		m_wndBasicBar.UpdateColorImage(IDB_BASIC24, IDB_BASIC124, 1);
		break;
	}
	m_wndBasicBar.RedrawWindow();
}

/**************************************************************************
*  @Function Name :: PreCreateWindow
*  @Description   :: 创建窗口前的设置工作
*  @Author        :: zzw
*  @Parameters    :: cs:PreCreateWindow参数
*
*  @Return        :: 返回运行是否成功
**************************************************************************/
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

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


BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (cameraDialog.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) {
		return TRUE;
	}
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
