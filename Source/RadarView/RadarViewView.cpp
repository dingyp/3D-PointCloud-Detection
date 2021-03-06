// RadarViewView.cpp: CRadarViewView 类的实现
//

#include "stdafx.h"
#include <memory>
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "RadarView.h"
#endif

#include "RadarViewDoc.h"
#include "RadarViewView.h"
#include "CToolBar24.h"
#include "MainFrm.h"
#include "RadarSetDlg.h"
#include "GridSetDlg.h"
#include "DataRangeDlg.h"
#include "ColorCodingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRadarViewView

IMPLEMENT_DYNCREATE(CRadarViewView, CView)

BEGIN_MESSAGE_MAP(CRadarViewView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//颜色工具栏按钮
	ON_COMMAND(COLOR_LEGEND, &CRadarViewView::OnColorLegned)					//显示颜色标尺
	ON_COMMAND(COLORMAP_BUTTON, &CRadarViewView::OnChangeColorMode)				//改变编码的模式
	ON_COMMAND(CUS_RANGE_BUTTON, &CRadarViewView::OnCustomColorRange)			//改变颜色的范围
	ON_COMMAND(RERANGE, &CRadarViewView::OnReScaleColorRange)					//重置颜色范围
	//播放工具栏按钮
	ON_COMMAND(GSTART_BUTTON, &CRadarViewView::OnGoToStart)						//回到开始帧
	ON_COMMAND(BACKWARD_BUTTON, &CRadarViewView::OnSeekBackward)				//帧回退事件
	ON_COMMAND(PLAY_BUTTON, &CRadarViewView::OnStart_Stop)						//开始暂停事件
	ON_COMMAND(FORWARD_BUTTON, &CRadarViewView::OnSeekForward)					//帧前进事件
	ON_COMMAND(GEND_BUTTON, &CRadarViewView::OnGoToEnd)							//跳到最后帧
	ON_COMMAND(LOOP_BUTTON, &CRadarViewView::OnLoop)							//循环播放
	//视图工具栏按钮
	ON_COMMAND(RESET_BUTTON, &CRadarViewView::OnResetView)						//重置视野
	ON_COMMAND(ORTHOGONAL_BUTTON, &CRadarViewView::OnOrthoproj_Perspectiveproj)	//正交透视视图切换
	ON_COMMAND(RULAR_BUTTON, &CRadarViewView::OnRularButton)					//两点间距离测量
	ON_COMMAND(RULARZERO_BUTTON, &CRadarViewView::OnRularZeroButton)			//到零点距离测量
	ON_COMMAND(XYVIEW_BUTTON, &CRadarViewView::OnXYView)						//XY方向网格
	ON_COMMAND(XZVIEW_BUTTON, &CRadarViewView::OnXZView)						//XZ方向网格
	ON_COMMAND(YZVIEW_BUTTON, &CRadarViewView::OnYZView)						//YZ方向网格
	ON_COMMAND_RANGE(DIR_XP_BUTTON, DIR_ZS_BUTTON, OnChooseDir)					//视野的调整响应事件

	ON_COMMAND(ENLARGE_BUTTON, &CRadarViewView::OnEnLargeView)					//视野进行放大响应事件
	ON_COMMAND(REDUCE_BUTTON, &CRadarViewView::OnReduceView)					//视野进行缩小响应事件
	ON_COMMAND(GRID_CONCIRCLE_BUTTON, &CRadarViewView::OnChooseGridDraw)		//网格类型切换

	ON_COMMAND(CHOOSE_CALI_BUTTON, &CRadarViewView::OnSensorSetting)
	ON_COMMAND(GRID_SET_BUTTON, &CRadarViewView::OnGridSetting)

	ON_MESSAGE(MY_MSG_SensorReader, CRadarViewView::ChangeFrameMessage)			//重新设置关于帧的信息的事件响应函数
	ON_MESSAGE(MY_MSG_ChangStatusBar, CRadarViewView::ChangStatusTEXT)			//对状态栏中第wParam个框格显示字符串lParam
	ON_MESSAGE(MY_MSG_RestDir, CRadarViewView::ResetDirInfo)					//重置工具栏dirView
	ON_MESSAGE(MY_MSG_FileReader, CRadarViewView::GetFilePath)					//进行文件的路径读取
END_MESSAGE_MAP()

/**************************************************************************
*  @Function Name :: CRadarViewView
*  @Description   :: CRadarViewView类的默认构造函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CRadarViewView::CRadarViewView()
{
	// TODO: 在此处添加构造代码
	m_pPoints3DWnd = NULL;
	m_bLegendShow = FALSE;
	m_colorLegendDlg = NULL;
}

/**************************************************************************
*  @Function Name :: ~CRadarViewView
*  @Description   :: CRadarViewView类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CRadarViewView::~CRadarViewView()
{
	if (m_pPoints3DWnd != NULL){
		m_pPoints3DWnd->DestroyWindow();
		delete(m_pPoints3DWnd);
		m_pPoints3DWnd = NULL;
	}

	delete(m_colorLegendDlg);
	m_colorLegendDlg = NULL;
}

/**************************************************************************
*  @Function Name :: PreCreateWindow
*  @Description   :: 创建窗口前的设置工作
*  @Author        :: zzw
*  @Parameters    :: cs:PreCreateWindow参数
*
*  @Return        :: 返回是否运行成功
**************************************************************************/
BOOL CRadarViewView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	cs.style |= WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

/**************************************************************************
*  @Function Name :: OnDraw
*  @Description   :: CRadarViewView 绘图
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnDraw(CDC* /*pDC*/)
{
	CRadarViewDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	if (m_pPoints3DWnd != NULL)
	{
		m_pPoints3DWnd->ReDraw();
	}
}

// CRadarViewView 诊断

#ifdef _DEBUG
void CRadarViewView::AssertValid() const
{
	CView::AssertValid();
}

void CRadarViewView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRadarViewDoc* CRadarViewView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRadarViewDoc)));
	return (CRadarViewDoc*)m_pDocument;
}
#endif //_DEBUG


/**************************************************************************
*  @Function Name :: OnCreate
*  @Description   :: CRadarViewView类在被创建的时候调用
*  @Author        :: zzw
*  @Parameters    :: lpCreateStruct:OnCreate参数
*
*  @Return        :: None
**************************************************************************/
int CRadarViewView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	// TODO:  在此添加您专用的创建代码
	m_pPoints3DWnd = new Points3DWnd;
	m_pPoints3DWnd->Create(CRect(0, 0, 100, 100), this, NULL);

	m_colorLegendDlg = new ColorLegendDlg();
	m_colorLegendDlg->Create(IDD_COLORLEGEND, this);
	m_pPoints3DWnd->SetFocus();
	return 0;
}

/**************************************************************************
*  @Function Name :: OnSize
*  @Description   :: 设置显示窗口的大小
*  @Author        :: zzw
*  @Parameters    :: nType:窗口显示的形式
*					 cx:窗口显示的宽 cy:窗口显示的长
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (cy > 2 && m_pPoints3DWnd != NULL){
		m_pPoints3DWnd->MoveWindow(0, 0, cx, cy);
	}

	//重新调整尺度的位置和大小
	if (TRUE == GetIsLegendShow())
	{
		MoveLegend();
	}

}

/**************************************************************************
*  @Function Name :: CRadarViewView
*  @Description   :: CRadarViewView类在销毁前调用的响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnDestroy()
{
	if (m_pPoints3DWnd != NULL){
		m_pPoints3DWnd->DestroyWindow();
		delete(m_pPoints3DWnd);
		m_pPoints3DWnd = NULL;
	}

	CView::OnDestroy();
}

/**************************************************************************
*  @Function Name :: OnResetView
*  @Description   :: 重置视野响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnResetView()
{
	if (m_pPoints3DWnd != NULL)
	{
		m_pPoints3DWnd->ResetView();
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		pMain->Set_Dir_Bitmap(RESET);
	}
}

/**************************************************************************
*  @Function Name :: MoveLegend
*  @Description   :: 在主窗口移动时调用，用于对颜色标尺进行移动
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::MoveLegend()
{
	CRect rect;
	GetWindowRect(rect);
	m_colorLegendDlg->SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

/**************************************************************************
*  @Function Name :: ReDrawLegend
*  @Description   :: 对标尺进行重新绘制
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::ReDrawLegend()
{
	if (FALSE == m_bLegendShow)
	{//没有打开则不管
	}
	else if (TRUE == m_bLegendShow)
	{//已经打开则重绘
		if (NULL != m_colorLegendDlg)
		{
			m_colorLegendDlg->ShowWindow(SW_HIDE);//先隐藏
			//在显示之前先将尺度属性传进去
			COLORCODING ColorCoding;
			double dataMin = 0.0;
			double dataMax = 0.0;
			m_pPoints3DWnd->GetValueRange(dataMin, dataMax);
			m_pPoints3DWnd->GetColorCoding(ColorCoding);
			m_colorLegendDlg->SetAttribute(m_pPoints3DWnd->GetColorMode(), ColorCoding, dataMin, dataMax);

			MoveLegend();
			m_colorLegendDlg->ShowWindow(SW_SHOW);
		}
	}
}

/**************************************************************************
*  @Function Name :: OnColorLegned
*  @Description   :: 点击标尺显示按钮的响应函数，可以显示或隐藏颜色标尺
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnColorLegned()
{
	if (m_pPoints3DWnd != NULL)
	{
		if (FALSE == m_bLegendShow)
		{//没有打开则打开
			m_bLegendShow = TRUE;
			if (NULL != m_colorLegendDlg)
			{
				//在显示之前先将尺度属性传进去
				COLORCODING ColorCoding;
				double dataMin = 0.0;
				double dataMax = 0.0;
				m_pPoints3DWnd->GetValueRange(dataMin, dataMax);
				m_pPoints3DWnd->GetColorCoding(ColorCoding);
				m_colorLegendDlg->SetAttribute(m_pPoints3DWnd->GetColorMode(), ColorCoding, dataMin, dataMax);

				MoveLegend();
				m_colorLegendDlg->ShowWindow(SW_SHOW);
			}
		}
		else if (TRUE == m_bLegendShow)
		{//已经打开则关闭
			m_bLegendShow = FALSE;
			if (NULL != m_colorLegendDlg)
			{
				m_colorLegendDlg->ShowWindow(SW_HIDE);
			}
		}
		m_pPoints3DWnd->SetFocus();//不管显示还是没有显示，都加上聚焦函数
	}
}

/**************************************************************************
*  @Function Name :: GetPointsData
*  @Description   :: 获取正在显示的点帧
*  @Author        :: zzw
*  @Parameters    :: PointList:显示的点帧引用
*
*  @Return        :: 返回是否获取成功
**************************************************************************/
BOOL CRadarViewView::GetPointsData(RadarPointList &PointList)
{
	if (m_pPoints3DWnd != NULL)
	{
		return m_pPoints3DWnd->GetPointListData(PointList);
	}
	return FALSE;
}

//设置3D焦点
void CRadarViewView::SetPoint3DWndFocus()
{
	if (m_pPoints3DWnd != NULL)
	{
		m_pPoints3DWnd->SetFocus();
	}
}

/**************************************************************************
*  @Function Name :: OnChangeColorMode
*  @Description   :: 改变颜色编码的模式的响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnChangeColorMode()
{
	if (m_pPoints3DWnd != NULL)
	{
		ColorCodingDlg colorCodingDlg;
		colorCodingDlg.DoModal();
	}
}

/**************************************************************************
*  @Function Name :: OnCustomColorRange
*  @Description   :: 改变颜色编码的数值范围的框格显示的响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnCustomColorRange()
{
	if (m_pPoints3DWnd != NULL)
	{
		DataRangeDlg dataRangeDlg;
		if (dataRangeDlg.DoModal())
		{//IDOK
		}
		
	}
}

/**************************************************************************
*  @Function Name :: OnReScaleColorRange
*  @Description   :: 重置颜色范围的响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnReScaleColorRange()
{
	if (m_pPoints3DWnd != NULL)
	{
		m_pPoints3DWnd->ReScaleColorRange();
		//对标尺进行重新绘制
		ReDrawLegend();
	}
}

/**************************************************************************
*  @Function Name :: OnOrthoproj_Perspectiveproj
*  @Description   :: 进行正交投影和透视投影的切换的响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnOrthoproj_Perspectiveproj()
{
	if (m_pPoints3DWnd != NULL)
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		BOOL bProjMode = pMain->Get_ORTHOGONAL_Bitmap();
		bProjMode = !bProjMode;
		m_pPoints3DWnd->SetProjMode(bProjMode);
		pMain->Set_ORTHOGONAL_Bitmap(bProjMode);
	}
}

/**************************************************************************
*  @Function Name :: OnStart_Stop
*  @Description   :: 播放暂停按钮事件的响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnStart_Stop()
{
	if (m_pPoints3DWnd != NULL)
	{
		CRadarViewDoc* pDoc = GetDocument();
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		BOOL STARTSTOPStatue = pMain->Get_STARTSTOP_Bitmap();
		STARTSTOPStatue = !STARTSTOPStatue;
		pMain->Set_STARTSTOP_Bitmap(STARTSTOPStatue);
		if (STARTSTOPStatue)
		{//停止
			//将测量模式设置成测量模式
			int MeasureStatus = pMain->Get_Measure_Bitmap();
			if (MeasureStatus == 0)
			{//如果没有在测量，就设置成两点测量，其他默认
				CRadarViewView *pView = (CRadarViewView *)pMain->GetActiveView();
				pView->SetRularStatus(1);
			}

			pDoc->myReader.Stop();
			pDoc->myFrameProcessor.Stop();
		}
		else
		{//开始//添加开始的时候对帧数的判断，如果此时是最后一帧，则首先跳到对面，然后才开始，避免一开始就暂停
			//将测量模式设置成不测量模式
			int MeasureStatus = pMain->Get_Measure_Bitmap();
			if (MeasureStatus != 0)
			{//如果没有在不测量，就使之不测量
				CRadarViewView *pView = (CRadarViewView *)pMain->GetActiveView();
				pView->SetRularStatus(0);
			}

			pDoc->myFrameProcessor.Restart();
			pDoc->myReader.Start();
		}
	}
}

/**************************************************************************
*  @Function Name :: OnGoToStart
*  @Description   :: 回归开始按钮事件
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnGoToStart()
{
	if (m_pPoints3DWnd != NULL)
	{
		CRadarViewDoc* pDoc = GetDocument();
		pDoc->myReader.Stop();
		pDoc->myRawProcessor.End_ClearFrame();
		pDoc->myFrameProcessor.ReTrasform();//再清空
		pDoc->myReader.GoToStart();//再重新开始

		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;

		if (!pMain->Get_STARTSTOP_Bitmap())
		{//按钮是播放状态
			pDoc->myReader.Start(1);//状态位，表示如果到达端点，不跳到对面
		}
	}
}

/**************************************************************************
*  @Function Name :: OnSeekBackward
*  @Description   :: 后退按钮事件
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnSeekBackward()
{
	if (m_pPoints3DWnd != NULL)
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		CRadarViewDoc* pDoc = GetDocument();
		if (pMain->Get_STARTSTOP_Bitmap())
		{//暂停状态
			pDoc->myRawProcessor.End_ClearFrame();
			unsigned long long recent_fileframe = pDoc->myFrameProcessor.GetFileFrameIndex();//得到当前文件夹里面的帧数
			recent_fileframe--;
			pDoc->myFrameProcessor.ReTrasform();								//先清空
			if (recent_fileframe <= 0)
			{
				recent_fileframe = 0;
			}
			if (recent_fileframe>pDoc->myReader.GetFrameNumber())
			{
				recent_fileframe = pDoc->myReader.GetFrameNumber();
			}
			pDoc->myReader.GoToFrame(recent_fileframe);
		}
		else if (!pMain->Get_STARTSTOP_Bitmap())
		{//按钮是播放状态
			int my = pDoc->myReader.GetPlayType();
			if (BACKWORD == pDoc->myReader.GetPlayType())
			{//一开始就是反向播放，那么进行速度变化
				SPEED speed = pDoc->myReader.ACCSpeed();
				pMain->Set_Backward_Bitmap(speed);
			}
			else
			{//一开始是正向播放，进行速度反向，并且速度置一
				pDoc->myReader.Stop();//先停止产生
				pDoc->myRawProcessor.End_ClearFrame();
				unsigned long long recent_fileframe = pDoc->myFrameProcessor.GetFileFrameIndex();//得到当前文件夹里面的帧数
				recent_fileframe--;
				pDoc->myFrameProcessor.ReTrasform();								//先清空
				if (recent_fileframe <= 0)
				{
					recent_fileframe = 0;
				}
				if (recent_fileframe>pDoc->myReader.GetFrameNumber())
				{
					recent_fileframe = pDoc->myReader.GetFrameNumber();
				}
				pDoc->myReader.GoToFrame(recent_fileframe);
				pDoc->myReader.PlayBackward();
				pDoc->myReader.Start();
				pMain->Set_Backward_Bitmap(DEFAULT_SPEED);
			}
		}
	}
}

/**************************************************************************
*  @Function Name :: OnSeekForward
*  @Description   :: 前进按钮事件
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnSeekForward()
{
	if (m_pPoints3DWnd != NULL)
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		CRadarViewDoc* pDoc = GetDocument();
		if (pMain->Get_STARTSTOP_Bitmap())
		{//暂停状态
			pDoc->myRawProcessor.End_ClearFrame();
			unsigned long long recent_fileframe = pDoc->myFrameProcessor.GetFileFrameIndex();//得到当前文件夹里面的帧数
			recent_fileframe++;
			pDoc->myFrameProcessor.ReTrasform();								//先清空
			if (recent_fileframe <= 0)
			{
				recent_fileframe = 0;
			}
			if (recent_fileframe>pDoc->myReader.GetFrameNumber())
			{
				recent_fileframe = pDoc->myReader.GetFrameNumber();
			}
			pDoc->myReader.GoToFrame(recent_fileframe);
		}
		if (!pMain->Get_STARTSTOP_Bitmap())
		{//播放状态
			if (FORWORD == pDoc->myReader.GetPlayType())
			{//一开始就是正向播放，那么进行速度变化
				SPEED speed=pDoc->myReader.ACCSpeed();
				pMain->Set_ForWard_Bitmap(speed);
			}
			else
			{//一开始是反向播放，进行速度正向，并且速度置一
				pDoc->myReader.Stop();
				pDoc->myRawProcessor.End_ClearFrame();
				unsigned long long recent_fileframe = pDoc->myFrameProcessor.GetFileFrameIndex();//得到当前文件夹里面的帧数
				recent_fileframe++;
				pDoc->myFrameProcessor.ReTrasform();								//先清空
				if (recent_fileframe <= 0)
				{
					recent_fileframe = 0;
				}
				if (recent_fileframe>pDoc->myReader.GetFrameNumber())
				{
					recent_fileframe = pDoc->myReader.GetFrameNumber();
				}
				pDoc->myReader.GoToFrame(recent_fileframe);
				pDoc->myReader.PlayForward();
				pDoc->myReader.Start();
				pMain->Set_ForWard_Bitmap(DEFAULT_SPEED);
			}
		}
	}
}

/**************************************************************************
*  @Function Name :: OnGoToEnd
*  @Description   :: 跳到结束按钮事件
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnGoToEnd()
{
	if (m_pPoints3DWnd != NULL)
	{
		CRadarViewDoc* pDoc = GetDocument();
		pDoc->myReader.Stop();//先停止产生
		pDoc->myRawProcessor.End_ClearFrame();
		pDoc->myFrameProcessor.ReTrasform();//再清空
		pDoc->myReader.GoToEnd();//再重新开始
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		if (!pMain->Get_STARTSTOP_Bitmap())
		{//按钮是播放状态
			pDoc->myReader.Start(1);
		}
	}
}

/**************************************************************************
*  @Function Name :: OnLoop
*  @Description   :: 循环播放按钮事件
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnLoop()
{
	if (m_pPoints3DWnd != NULL)
	{
		CRadarViewDoc* pDoc = GetDocument();
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		BOOL LOOPStatue = pMain->Get_LOOP_Bitmap();
		LOOPStatue = !LOOPStatue;
		pMain->Set_LOOP_Bitmap(LOOPStatue);
		if (LOOPStatue)
		{//不循环事件
			pDoc->myReader.EndLoop();
		}
		else
		{//循环事件
			pDoc->myReader.Loop();
		}
	}
}

/**************************************************************************
*  @Function Name :: ClearFrameShow
*  @Description   :: 清除画面上所有的点，包括点的线
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::ClearFrameShow()
{
	m_pPoints3DWnd->ClearData();
}

/**************************************************************************
*  @Function Name :: ClearLinesAndPoints
*  @Description   :: 取消显示取点和画的线
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::ClearLinesAndPoints()
{
	m_pPoints3DWnd->ClearLinesAndPoints();
}

/**************************************************************************
*  @Function Name :: CRadarViewView
*  @Description   :: 对状态栏中第wParam个框格显示字符串lParam
*  @Author        :: zzw
*  @Parameters    :: wParam:表示状态栏的第几个框格
*					 lParam:显示的字符串
*  @Return        :: 返回0
**************************************************************************/
LRESULT CRadarViewView::ChangStatusTEXT(WPARAM wParam, LPARAM lParam)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;

	pMain->m_wndStatusBar.SetPaneText(int(wParam), (LPCTSTR)lParam);//对状态栏中第wParam个框格显示字符串lParam
	return 0;
}

/**************************************************************************
*  @Function Name :: ChangeFrameMessage
*  @Description   :: 重新设置关于帧的信息的事件响应函数
*  @Author        :: zzw
*  @Parameters    :: wParam:是否需要重新帧中点的尺寸
*					 lParam:指向点帧的指针
*  @Return        :: 返回0
**************************************************************************/
LRESULT CRadarViewView::ChangeFrameMessage(WPARAM wParam, LPARAM lParam)
{
	int resetFrameSize = int(wParam);
	std::shared_ptr<RadarPointList> pRadarPointList = *((std::shared_ptr<RadarPointList> *)lParam);

	if (1 == resetFrameSize)
	{//需要重新更新尺寸
		int Num = pRadarPointList->size();
		if (Num > 0)
		{
			RadarPoint CurPoint;
			double MinValue = (*pRadarPointList)[0].intensity, MaxValue = (*pRadarPointList)[0].intensity; //数值范围

			double BoundBox[6];
			BoundBox[0] = BoundBox[2] = BoundBox[4] = 0;//x,y,z
			BoundBox[1] = BoundBox[3] = BoundBox[5] = 0;

			for (int i = 0; i < pRadarPointList->size(); i++)
			{
				CurPoint = (*pRadarPointList)[i];

				if (MinValue > CurPoint.intensity)
				{
					MinValue = CurPoint.intensity;
				}
				if (MaxValue < CurPoint.intensity)
				{
					MaxValue = CurPoint.intensity;
				}

				//x
				if (BoundBox[0] > CurPoint.x)
				{
					BoundBox[0] = CurPoint.x;//xmin
				}
				if (BoundBox[1] < CurPoint.x)
				{
					BoundBox[1] = CurPoint.x;//xmax
				}
				//y
				if (BoundBox[2] > CurPoint.y)
				{
					BoundBox[2] = CurPoint.y;//ymin
				}
				if (BoundBox[3] < CurPoint.y)
				{
					BoundBox[3] = CurPoint.y;//ymax
				}
				//z
				if (BoundBox[4] > CurPoint.z)
				{
					BoundBox[4] = CurPoint.z;//zmin
				}
				if (BoundBox[5] < CurPoint.z)
				{
					BoundBox[5] = CurPoint.z;//zmax
				}
			}
			//设置网格以及视野尺寸
			m_pPoints3DWnd->SetValueRange(MinValue, MaxValue);
			m_pPoints3DWnd->SetBoundBox(BoundBox[0], BoundBox[1], BoundBox[2], BoundBox[3], BoundBox[4], BoundBox[5]);
		}
		m_pPoints3DWnd->AttachData(*pRadarPointList);
		//m_pPoints3DWnd->ReScaleColorRange();
	}
	else
	{//并不需要重新更新
		m_pPoints3DWnd->AttachData(*pRadarPointList);
	}

	CRadarViewDoc *pDoc = (CRadarViewDoc *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveDocument();
	CString frameInfo;
	frameInfo.Format(_T("帧数：%d"), pDoc->myFrameProcessor.GetFileFrameIndex());
	ChangStatusTEXT(4, (LPARAM)LPCTSTR(frameInfo));

	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->SetPos(pDoc->myFrameProcessor.GetFileFrameIndex());
	return 0;
}

//获取是否进行滤波
void CRadarViewView::Get_IsFilter(bool &isFilter)
{
	CRadarViewDoc *pDoc = (CRadarViewDoc *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveDocument();
	isFilter = pDoc->m_bIsFilter;
	//pDoc->myFrameProcessor.Get_IsFilter(isFilter);
}

//设置是否进行滤波
void CRadarViewView::Set_IsFilter(bool isFilter)
{
	CRadarViewDoc *pDoc = (CRadarViewDoc *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveDocument();
	//以下两个标志位同时设置
	pDoc->m_bIsFilter = isFilter;
	pDoc->myFrameProcessor.Set_IsFilter(isFilter);
}

/**************************************************************************
*  @Function Name :: OnChooseDir
*  @Description   :: 调整视野方向的响应事件
*  @Author        :: zzw
*  @Parameters    :: nID:触发按钮的ID，便于确定时哪个方向
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnChooseDir(UINT nID)
{
	if (m_pPoints3DWnd != NULL)
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		switch (nID)
		{
		case DIR_XP_BUTTON:
			m_pPoints3DWnd->SetDir(XP);//设置view方向
			pMain->Set_Dir_Bitmap(XP);
			break;
		case DIR_XS_BUTTON:
			m_pPoints3DWnd->SetDir(XS);//设置view方向
			pMain->Set_Dir_Bitmap(XS);
			break;
		case DIR_YP_BUTTON:
			m_pPoints3DWnd->SetDir(YP);//设置view方向
			pMain->Set_Dir_Bitmap(YP);
			break;
		case DIR_YS_BUTTON:
			m_pPoints3DWnd->SetDir(YS);//设置view方向
			pMain->Set_Dir_Bitmap(YS);
			break;
		case DIR_ZP_BUTTON:
			m_pPoints3DWnd->SetDir(ZP);//设置view方向
			pMain->Set_Dir_Bitmap(ZP);
			break;
		case DIR_ZS_BUTTON:
			m_pPoints3DWnd->SetDir(ZS);//设置view方向
			pMain->Set_Dir_Bitmap(ZS);
			break;
		}
	}
}

/**************************************************************************
*  @Function Name :: ResetDirInfo
*  @Description   :: 重置工具栏dirView
*  @Author        :: zzw
*  @Parameters    :: wParam:暂无
*					 lParam:暂无
*  @Return        :: 返回运行是否成功
**************************************************************************/
LRESULT CRadarViewView::ResetDirInfo(WPARAM wParam, LPARAM lParam)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->Set_Dir_Bitmap(RESET);
	return true;
}

/**************************************************************************
*  @Function Name :: OnChooseGridDraw
*  @Description   :: 网格类型切换的响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*					
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnChooseGridDraw()
{
	if (m_pPoints3DWnd != NULL)
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		BOOL GridDrawStatue = pMain->Get_GridDraw_Bitmap();
		GridDrawStatue = !GridDrawStatue;
		pMain->Set_GridDraw_Bitmap(GridDrawStatue);
		if (GridDrawStatue)
		{//同心圆
			m_pPoints3DWnd->SetGridType(CON_CIRCLE);

			//修改状态栏的网格信息
			float LineDistance;
			int Circlenumber;
			m_pPoints3DWnd->GetCircleParameters(LineDistance, Circlenumber);
			CString gridInfo;
			gridInfo.Format(_T("同心圆半径:%0.2f m"), LineDistance / Circlenumber);
			ChangStatusTEXT(3, (LPARAM)LPCTSTR(gridInfo));

		}
		else
		{//网格
			m_pPoints3DWnd->SetGridType(GRID);

			//修改状态栏的网格信息
			CString gridInfo;
			float LineDistance;
			int LineResolution;
			m_pPoints3DWnd->GetGridParameters(LineDistance, LineResolution);
			gridInfo.Format(_T("栅格边长:%0.2f m"), LineDistance / LineResolution);
			ChangStatusTEXT(3, (LPARAM)LPCTSTR(gridInfo));
		}
		
	}
}

/**************************************************************************
*  @Function Name :: OnSensorSetting
*  @Description   :: 对传感器的一些属性进行设置，不是连接传感器的时候，而是任意时候都可以设置，所以此时没有IP等网络参数,但是此时需要一开始就显示协议类型
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnSensorSetting()
{
	SetSensorAttribute(0);
}

//连接之后传感器属性设置
BOOL  CRadarViewView::SetSensorAttribute(int Type)
{
	CRadarViewDoc* pDoc = GetDocument();
	//传进去传感器属性
	m_pPoints3DWnd->GetSensorCenter(pDoc->mySensor.sensorX, pDoc->mySensor.sensorY, pDoc->mySensor.sensorZ);
	m_pPoints3DWnd->GetSensorEuler(pDoc->mySensor.sensorPitch, pDoc->mySensor.sensorRoll, pDoc->mySensor.sensorYaw);
	//输入之前的备份
	double tmp_sensorX = pDoc->mySensor.sensorX;
	double tmp_sensorY = pDoc->mySensor.sensorY;
	double tmp_sensorZ = pDoc->mySensor.sensorZ;
	double tmp_sensorPitch = pDoc->mySensor.sensorPitch;
	double tmp_sensorRoll = pDoc->mySensor.sensorRoll;
	double tmp_sensorYaw = pDoc->mySensor.sensorYaw;

	SensorSetDlg sensorSetDlg(&pDoc->mySensor, Type);
	sensorSetDlg.DoModal();

	//如果没有改变，则不需要设置了
	double differ_sensorX = pDoc->mySensor.sensorX - tmp_sensorX;
	double differ_sensorY = pDoc->mySensor.sensorY - tmp_sensorY;
	double differ_sensorZ = pDoc->mySensor.sensorZ - tmp_sensorZ;
	double differ_sensorPitch = pDoc->mySensor.sensorPitch - tmp_sensorPitch;
	double differ_sensorRoll = pDoc->mySensor.sensorRoll - tmp_sensorRoll;
	double differ_sensorYaw = pDoc->mySensor.sensorYaw - tmp_sensorYaw;
	if (fabs(differ_sensorX) > 1e-7 || fabs(differ_sensorY) > 1e-7 || fabs(differ_sensorZ) > 1e-7 || \
		fabs(differ_sensorPitch) > 1e-7 || fabs(differ_sensorRoll) > 1e-7 || fabs(differ_sensorYaw) > 1e-7)
	{
		m_pPoints3DWnd->SetSensorCenter(pDoc->mySensor.sensorX, pDoc->mySensor.sensorY, pDoc->mySensor.sensorZ);//设置传感器中心的坐标
		m_pPoints3DWnd->SetSensorEuler(pDoc->mySensor.sensorPitch, pDoc->mySensor.sensorRoll, pDoc->mySensor.sensorYaw);//设置传感器的姿态角
		m_pPoints3DWnd->RefreshData(differ_sensorX, differ_sensorY, differ_sensorZ, \
			differ_sensorPitch, differ_sensorRoll, differ_sensorYaw);//对显示部分的所有数据进行更新
																	 //消除显示的点和线
		ClearLinesAndPoints();
		//重置状态栏的显示
		ChangStatusTEXT(2, (LPARAM)LPCTSTR(_T("取点信息")));
	}

	m_pPoints3DWnd->SetFocus();
	return true;
}

/**************************************************************************
*  @Function Name :: SetSensorConAttribute
*  @Description   :: 进行传感器连接属性设置
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 返回是否设置成功
**************************************************************************/
BOOL CRadarViewView::SetSensorConAttribute()
{
	CRadarViewDoc* pDoc = GetDocument();
	//传进去传感器属性
	m_pPoints3DWnd->GetSensorCenter(pDoc->mySensor.sensorX, pDoc->mySensor.sensorY, pDoc->mySensor.sensorZ);
	m_pPoints3DWnd->GetSensorEuler(pDoc->mySensor.sensorPitch, pDoc->mySensor.sensorRoll, pDoc->mySensor.sensorYaw);
	//输入之前的备份
	double tmp_sensorX = pDoc->mySensor.sensorX;
	double tmp_sensorY = pDoc->mySensor.sensorY;
	double tmp_sensorZ = pDoc->mySensor.sensorZ;
	double tmp_sensorPitch = pDoc->mySensor.sensorPitch;
	double tmp_sensorRoll = pDoc->mySensor.sensorRoll;
	double tmp_sensorYaw = pDoc->mySensor.sensorYaw;

	RadarSetDlg radarSetDlg(&pDoc->mySensor);
	if (1 == radarSetDlg.DoModal())
	{//IDOK
	 //如果没有改变，则不需要设置了
		double differ_sensorX = pDoc->mySensor.sensorX - tmp_sensorX;
		double differ_sensorY = pDoc->mySensor.sensorY - tmp_sensorY;
		double differ_sensorZ = pDoc->mySensor.sensorZ - tmp_sensorZ;
		double differ_sensorPitch = pDoc->mySensor.sensorPitch - tmp_sensorPitch;
		double differ_sensorRoll = pDoc->mySensor.sensorRoll - tmp_sensorRoll;
		double differ_sensorYaw = pDoc->mySensor.sensorYaw - tmp_sensorYaw;
		if (fabs(differ_sensorX) > 1e-7 || fabs(differ_sensorY) > 1e-7 || fabs(differ_sensorZ) > 1e-7 || \
			fabs(differ_sensorPitch) > 1e-7 || fabs(differ_sensorRoll) > 1e-7 || fabs(differ_sensorYaw) > 1e-7)
		{
			m_pPoints3DWnd->SetSensorCenter(pDoc->mySensor.sensorX, pDoc->mySensor.sensorY, pDoc->mySensor.sensorZ);//设置传感器中心的坐标
			m_pPoints3DWnd->SetSensorEuler(pDoc->mySensor.sensorPitch, pDoc->mySensor.sensorRoll, pDoc->mySensor.sensorYaw);//设置传感器的姿态角
			m_pPoints3DWnd->RefreshData(differ_sensorX, differ_sensorY, differ_sensorZ, \
				differ_sensorPitch, differ_sensorRoll, differ_sensorYaw);//对显示部分的所有数据进行更新
																		 //消除显示的点和线
			ClearLinesAndPoints();
			//重置状态栏的显示
			ChangStatusTEXT(2, (LPARAM)LPCTSTR(_T("取点信息")));
		}
		m_pPoints3DWnd->SetFocus();
		return TRUE;
	}
	m_pPoints3DWnd->SetFocus();
	return FALSE;
}

/**************************************************************************
*  @Function Name :: OnGridSetting
*  @Description   :: 网格设置的响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnGridSetting()
{
	GridSetDlg gridSetDlg;
	if (1 == gridSetDlg.DoModal())
	{//IDOK
	}
	m_pPoints3DWnd->SetFocus();
}

/**************************************************************************
*  @Function Name :: OnXYView
*  @Description   :: 进行xy平面的显示响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnXYView()
{
	if (m_pPoints3DWnd != NULL)
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		LINEPLANE_TYPE tmp_GridView;
		pMain->Get_GridView_Bitmap(tmp_GridView);
		if (XOY == tmp_GridView)
		{
			m_pPoints3DWnd->SetGridDir(NONEVIEW);
			pMain->Set_GridView_Bitmap(NONEVIEW);
		}
		else
		{
			m_pPoints3DWnd->SetGridDir(XOY);
			pMain->Set_GridView_Bitmap(XOY);
		}
	}
}

/**************************************************************************
*  @Function Name :: OnXYView
*  @Description   :: 设置网格朝向
*  @Author        :: zzw
*  @Parameters    :: eLinePlaneType:设置网格朝向的参数
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::SetGridView(LINEPLANE_TYPE eLinePlaneType)
{
	if (m_pPoints3DWnd != NULL)
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		m_pPoints3DWnd->SetGridDir(eLinePlaneType);
		pMain->Set_GridView_Bitmap(eLinePlaneType);
	}
}

/**************************************************************************
*  @Function Name :: OnYZView
*  @Description   :: 进行yz平面的显示响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnYZView()
{
	if (m_pPoints3DWnd != NULL)
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		LINEPLANE_TYPE tmp_GridView;
		pMain->Get_GridView_Bitmap(tmp_GridView);
		if (YOZ == tmp_GridView)
		{
			m_pPoints3DWnd->SetGridDir(NONEVIEW);
			pMain->Set_GridView_Bitmap(NONEVIEW);
		}
		else
		{
			m_pPoints3DWnd->SetGridDir(YOZ);
			pMain->Set_GridView_Bitmap(YOZ);
		}
	}
}

/**************************************************************************
*  @Function Name :: OnXZView
*  @Description   :: 进行xz平面的显示响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnXZView()
{
	if (m_pPoints3DWnd != NULL)
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		LINEPLANE_TYPE tmp_GridView;
		pMain->Get_GridView_Bitmap(tmp_GridView);
		if (XOZ == tmp_GridView)
		{
			m_pPoints3DWnd->SetGridDir(NONEVIEW);
			pMain->Set_GridView_Bitmap(NONEVIEW);
		}
		else
		{
			m_pPoints3DWnd->SetGridDir(XOZ);
			pMain->Set_GridView_Bitmap(XOZ);
		}

	}
}

/**************************************************************************
*  @Function Name :: OnEnLargeView
*  @Description   :: 进行xz视野进行放大响应事件
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnEnLargeView()
{
	if (m_pPoints3DWnd != NULL)
	{
		m_pPoints3DWnd->EnLargeView();
	}
}

/**************************************************************************
*  @Function Name :: OnReduceView
*  @Description   :: 进行xz视野进行缩小响应事件
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnReduceView()
{
	if (m_pPoints3DWnd != NULL)
	{
		m_pPoints3DWnd->ReduceView();
	}
}

/**************************************************************************
*  @Function Name :: SetRularStatus
*  @Description   :: 设置测量状态
*  @Author        :: zzw
*  @Parameters    :: MeasureStatus:测量的状态，0表示不测量，1表示两点间测量，2表示到零点的测量
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::SetRularStatus(int MeasureStatus)
{
	if (m_pPoints3DWnd != NULL)
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		switch (MeasureStatus)
		{
		case 0:
			m_pPoints3DWnd->SetMeasureStatus(MM_ROTAL);//不测量
			pMain->Set_Measure_Bitmap(MeasureStatus);
			break;
		case 1:
			m_pPoints3DWnd->SetMeasureStatus(MM_MEASURE);//两点间测量
			pMain->Set_Measure_Bitmap(MeasureStatus);
			break;
		case 2:
			m_pPoints3DWnd->SetMeasureStatus(MM_MEASUREZERO);//到零点的测量
			pMain->Set_Measure_Bitmap(MeasureStatus);
			break;
		}
	}
}

/**************************************************************************
*  @Function Name :: OnRularButton
*  @Description   :: 两点间测量的响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnRularButton()
{
	if (m_pPoints3DWnd != NULL)
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		int MeasureStatus = pMain->Get_Measure_Bitmap();
		if (MeasureStatus != 1)
		{
			MeasureStatus = 1;
			m_pPoints3DWnd->SetMeasureStatus(MM_MEASURE);
			m_pPoints3DWnd->ReDraw();
		}
		else
		{
			MeasureStatus = 0;
			m_pPoints3DWnd->SetMeasureStatus(MM_ROTAL);
			m_pPoints3DWnd->ReDraw();
		}
		pMain->Set_Measure_Bitmap(MeasureStatus);
	}
}

/**************************************************************************
*  @Function Name :: OnRularZeroButton
*  @Description   :: 测量点到零点距离的响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewView::OnRularZeroButton()
{
	if (m_pPoints3DWnd != NULL)
	{

		CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		int MeasureStatus = pMain->Get_Measure_Bitmap();
		if (MeasureStatus != 2)
		{
			MeasureStatus = 2;
			m_pPoints3DWnd->SetMeasureStatus(MM_MEASUREZERO);
			m_pPoints3DWnd->ReDraw();
		}
		else
		{
			MeasureStatus = 0;
			m_pPoints3DWnd->SetMeasureStatus(MM_ROTAL);//只是恢复默认值
			m_pPoints3DWnd->ReDraw();
		}
		pMain->Set_Measure_Bitmap(MeasureStatus);
	}
}

/**************************************************************************
*  @Function Name :: GetFilePath
*  @Description   :: 进行文件的路径读取
*  @Author        :: zzw
*  @Parameters    :: wParam:暂无
*					 lParam:CRadarViewDoc类指针
*  @Return        :: None
**************************************************************************/
LRESULT CRadarViewView::GetFilePath(WPARAM wParam, LPARAM lParam)
{
	CRadarViewDoc *pDoc = (CRadarViewDoc *)lParam;

	//打开图像文件，获取文件路径名
	TCHAR szFilter[] = L"PCAP(*.pcap)|*.pcap|PCAPNG(*.pcapng)|*.pcapng||";
	//Sleep(10);
	CFileDialog dlgFileOpenFile(TRUE, _T("pcap"), _T("*.pcap"), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter, 0);
	//Sleep(10);
	HWND hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView()->GetSafeHwnd();
	//打开图像
	if (dlgFileOpenFile.DoModal() == IDOK)
	{
		//读取图像文件名
		CString tmppath = dlgFileOpenFile.GetPathName();

		pDoc->File_path = tmppath;
		pDoc->isMruFile = 2;
		pDoc->OnAlReadyReadFilePath(0, lParam);
		return true;
	}

	pDoc->isMruFile = 3;//3表示获取失败
	pDoc->OnAlReadyReadFilePath(1, 0);
	return false;
}