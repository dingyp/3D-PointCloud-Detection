
// LoadSTLAppView.cpp: CLoadSTLAppView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "LoadSTLApp.h"
#endif

#include "LoadSTLAppDoc.h"
#include "LoadSTLAppView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLoadSTLAppView

IMPLEMENT_DYNCREATE(CLoadSTLAppView, CView)

BEGIN_MESSAGE_MAP(CLoadSTLAppView, CView)
	ON_MESSAGE(MY_MSG_FullOneFrame, CLoadSTLAppView::GetOneFrame)			//进行文件的路径读取
	ON_MESSAGE(MY_MSG_EndPcapFresh, CLoadSTLAppView::EndPcapfresh)			//文件读取到最后响应函数
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CLoadSTLAppView 构造/析构

CLoadSTLAppView::CLoadSTLAppView()
{
	m_pPointAndSTL3DWnd = NULL;
}

CLoadSTLAppView::~CLoadSTLAppView()
{
	if (NULL != m_pPointAndSTL3DWnd)
	{
		m_pPointAndSTL3DWnd->DestroyWindow();
		delete(m_pPointAndSTL3DWnd);
		m_pPointAndSTL3DWnd = NULL;
	}
}

BOOL CLoadSTLAppView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CLoadSTLAppView 绘图

void CLoadSTLAppView::OnDraw(CDC* /*pDC*/)
{
	CLoadSTLAppDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	m_pPointAndSTL3DWnd->ReDraw();
}


// CLoadSTLAppView 打印

BOOL CLoadSTLAppView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CLoadSTLAppView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CLoadSTLAppView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CLoadSTLAppView 诊断

#ifdef _DEBUG
void CLoadSTLAppView::AssertValid() const
{
	CView::AssertValid();
}

void CLoadSTLAppView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLoadSTLAppDoc* CLoadSTLAppView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLoadSTLAppDoc)));
	return (CLoadSTLAppDoc*)m_pDocument;
}
#endif //_DEBUG


// CLoadSTLAppView 消息处理程序


int CLoadSTLAppView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pPointAndSTL3DWnd = new PointsAndSTL3DWnd();
	m_pPointAndSTL3DWnd->Create(CRect(0, 0, 100, 100), this, 100);

	return 0;
}


BOOL CLoadSTLAppView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return FALSE;
	//return CView::OnEraseBkgnd(pDC);
}


void CLoadSTLAppView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (cy > 2 && m_pPointAndSTL3DWnd != NULL) {
		m_pPointAndSTL3DWnd->MoveWindow(0, 0, cx, cy);
	}

}

void CLoadSTLAppView::ArrangeWnd()
{

}

//传递一帧过来的接口函数
LRESULT CLoadSTLAppView::GetOneFrame(WPARAM wParam, LPARAM lParam)
{
	std::shared_ptr<RadarPointList> pRadarPointList = *((std::shared_ptr<RadarPointList> *)lParam);
	//将一些超范围的点进行去除
	for (RadarPointList::iterator iter = pRadarPointList->begin(); iter != pRadarPointList->end(); )
	{
		if ((*iter).distance > 40000)
		{//大于40米需要舍弃
			iter = pRadarPointList->erase(iter);
		}
		else
		{
			++iter;
		}
	}
	CLoadSTLAppDoc *pSTLDoc = (CLoadSTLAppDoc *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveDocument();
	pSTLDoc->AttachPointCloud(pRadarPointList);

	return 0;
}

//文件读取到最后的响应函数
LRESULT CLoadSTLAppView::EndPcapfresh(WPARAM wParam, LPARAM lParam)
{
	CLoadSTLAppDoc *pSTLDoc = (CLoadSTLAppDoc *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveDocument();

	pSTLDoc->SetShowValue(0, 0, int(pSTLDoc->m_PointClouds.m_uiNowCloudNums));//帧数
	pSTLDoc->SetShowValue(0, 1, _T("否"));//是否显示平均帧
	
	//开始进行计算，计算好之后，传送来显示
	m_pPointAndSTL3DWnd->AttachPointClouds(&(pSTLDoc->m_PointClouds));

	//计算好平均帧之后再显示
	pSTLDoc->CalculatePointClouds();
	pSTLDoc->SetShowValue(0, 1, _T("是"));//是否显示平均帧
	pSTLDoc->SetShowValue(0, 2, int(2000 * pSTLDoc->m_PointClouds.LiDAR_Channels));//平均帧点云数
	pSTLDoc->SetShowValue(0, 3, int(pSTLDoc->m_PointClouds.LiDAR_Channels));//通道数
	pSTLDoc->SetShowValue(0, 4, pSTLDoc->m_PointClouds.m_VarDistanceMaxChannel);//最大方差通道
	pSTLDoc->SetShowValue(0, 5, pSTLDoc->m_PointClouds.m_VarDistanceMinChannel);//最小方差通道

	for (int i = 0; i<int(pSTLDoc->m_PointClouds.LiDAR_Channels); i++)
	{
		//求出每一通道的系统方差
		CString tmp_show1;
		CString tmp_show2;
		tmp_show1.Format(_T("%d"), i);
		tmp_show2.Format(_T("%.7f"), pSTLDoc->m_PointClouds.m_Arg_VarDistance[i]);
		CMFCPropertyGridProperty *ptmp_Prop6= new CMFCPropertyGridProperty(tmp_show1, tmp_show2);//5
		ptmp_Prop6->AllowEdit(FALSE);
		pSTLDoc->AddGridProperty(ptmp_Prop6, 0, 6);
	}
	

	//因为有一个标志位，标志了有没有计算平均帧，Wnd那边显示会根据这个标志位选择显示平均帧还是所有帧
	m_pPointAndSTL3DWnd->AttachPointClouds(&(pSTLDoc->m_PointClouds));

	return 0;
}

/**************************************************************************
*  @Function Name :: ClearFrameShow
*  @Description   :: 清除画面上所有的点，包括点的线
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CLoadSTLAppView::ClearFrameShow()
{
	m_pPointAndSTL3DWnd->ClearData();
}