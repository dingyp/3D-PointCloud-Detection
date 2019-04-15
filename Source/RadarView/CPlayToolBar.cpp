#include "stdafx.h"
#include "RadarViewDoc.h"
#include "RadarViewView.h"
#include "MainFrm.h"
#include "CToolBar24.h"

BEGIN_MESSAGE_MAP(CPlayToolBar, CToolBar)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

/**************************************************************************
*  @Function Name :: CPlayToolBar
*  @Description   :: CPlayToolBar类的构造函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CPlayToolBar::CPlayToolBar():CToolBar24()
{
	//TotalMark = 0;
}

/**************************************************************************
*  @Function Name :: ~CPlayToolBar
*  @Description   :: CPlayToolBar类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CPlayToolBar::~CPlayToolBar()
{
	//TotalMark = 0;
}

/**************************************************************************
*  @Function Name :: InitSlider
*  @Description   :: 对PlayToolbar的slider进行初始化
*  @Author        :: zzw
*  @Parameters    :: ID_SLIDER:滑动条的ID
*					 ID_FrameEdit:帧数显示框的ID
*  @Return        :: 运行是否成功
**************************************************************************/
BOOL CPlayToolBar::InitSlider(UINT ID_SLIDER,UINT ID_FrameEdit)
{
	int sliderIndex = CommandToIndex(ID_SLIDER);
	int editIndex = CommandToIndex(ID_FrameEdit);

	SetButtonInfo(sliderIndex, ID_SLIDER, TBBS_SEPARATOR, 120);
	SetButtonInfo(editIndex, ID_FrameEdit, TBBS_SEPARATOR, 40);

	CRect rect;
	GetItemRect(sliderIndex, &rect);
	if (!playSlider.Create(WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS | TBS_BOTTOM,
		rect, this, ID_SLIDER))
	{
		TRACE0("Failed to create slider ctrl\n");
		return FALSE;
	}
	playSlider.SetRange(0, 0);//初始化SliderRange设置成(0,0)
	playSlider.SetPos(0);
	playSlider.ShowWindow(SW_SHOW);


	GetItemRect(editIndex, &rect);

	if (!frameEdit.Create(WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS | TBS_BOTTOM,
		rect, this, ID_FrameEdit))
	{
		TRACE0("Failed to create frame number edit\n");
		return FALSE;
	}

	HFONT m_hFont = ::CreateFont(-15, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));
	frameEdit.SetFont(CFont::FromHandle(m_hFont));

	frameEdit.SetReadOnly();
	frameEdit.ShowWindow(SW_SHOW);

	return TRUE;
}

/**************************************************************************
*  @Function Name :: SetFrameNumber
*  @Description   :: 设置显示框和滑动条中帧数表达
*  @Author        :: zzw
*  @Parameters    :: frameNumber:帧数
*
*  @Return        :: None
**************************************************************************/
void CPlayToolBar::SetFrameNumber(UINT frameNumber)
{
	//对CSliderCtrl进行
	playSlider.SetPos(frameNumber);
	CString str_frameNumber;
	str_frameNumber.Format(_T("%d"), frameNumber);
	frameEdit.SetWindowTextW(str_frameNumber);
}

/**************************************************************************
*  @Function Name :: InitFrameNumber
*  @Description   :: 初始化总帧数，让滑动条可以进行滑动
*  @Author        :: zzw
*  @Parameters    :: totalFrameNumber:总的帧数
*
*  @Return        :: None
**************************************************************************/
void CPlayToolBar::InitFrameNumber(UINT totalFrameNumber)
{
	if (totalFrameNumber >= 0)
	{
		playSlider.SetRange(0, totalFrameNumber);
	}
	playSlider.SetPos(0);
}

/**************************************************************************
*  @Function Name :: OnHScroll
*  @Description   :: 播放的进度条被调整
*  @Author        :: zzw
*  @Parameters    :: nSBCode:OnHScroll参数 nPos:nHScroll参数
*					 pScrollBar:nHScroll参数
*  @Return        :: None
**************************************************************************/
void CPlayToolBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	unsigned int m_int = playSlider.GetPos();//取得当前位置值
											 //只需要将标志位记一下
	CRadarViewDoc *pDoc = (CRadarViewDoc *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveDocument();

	pDoc->myReader.Stop();//先停止产生此处的停止之后重新开始需要设置速度
	pDoc->myRawProcessor.End_ClearFrame();
	pDoc->myFrameProcessor.ReTrasform();//再清空
	pDoc->myReader.GoToFrame(m_int);
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CRadarViewView *pView = (CRadarViewView *)pMain->GetActiveView();
	if (!pMain->Get_STARTSTOP_Bitmap())
	{//按钮是播放状态
		pDoc->myReader.Start();
		//根据速度进行调节
		int nowSpeed;
		pMain->Get_Speed_Bitmap(nowSpeed);
		//根据速度进行加速调节
		switch (nowSpeed)
		{
		case ONE_SPEED || DEFAULT_SPEED:
			break;
		case DOUBLE_SPEED:
			pDoc->myReader.ACCSpeed();
			break;
		case THREE_SPEED:
			pDoc->myReader.ACCSpeed();
			pDoc->myReader.ACCSpeed();
			break;
		case HALF_SPEED:
			pDoc->myReader.ACCSpeed();
			pDoc->myReader.ACCSpeed();
			pDoc->myReader.ACCSpeed();
			break;
		case ONE_THRID_SPEED:
			pDoc->myReader.ACCSpeed();
			pDoc->myReader.ACCSpeed();
			pDoc->myReader.ACCSpeed();
			pDoc->myReader.ACCSpeed();
			break;
		}

	}

	CToolBar::OnHScroll(nSBCode, nPos, pScrollBar);
	pView->SetPoint3DWndFocus();//设置焦点
}