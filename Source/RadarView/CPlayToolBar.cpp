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
*  @Description   :: CPlayToolBar��Ĺ��캯��
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
*  @Description   :: CPlayToolBar�����������
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
*  @Description   :: ��PlayToolbar��slider���г�ʼ��
*  @Author        :: zzw
*  @Parameters    :: ID_SLIDER:��������ID
*					 ID_FrameEdit:֡����ʾ���ID
*  @Return        :: �����Ƿ�ɹ�
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
	playSlider.SetRange(0, 0);//��ʼ��SliderRange���ó�(0,0)
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
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("����"));
	frameEdit.SetFont(CFont::FromHandle(m_hFont));

	frameEdit.SetReadOnly();
	frameEdit.ShowWindow(SW_SHOW);

	return TRUE;
}

/**************************************************************************
*  @Function Name :: SetFrameNumber
*  @Description   :: ������ʾ��ͻ�������֡�����
*  @Author        :: zzw
*  @Parameters    :: frameNumber:֡��
*
*  @Return        :: None
**************************************************************************/
void CPlayToolBar::SetFrameNumber(UINT frameNumber)
{
	//��CSliderCtrl����
	playSlider.SetPos(frameNumber);
	CString str_frameNumber;
	str_frameNumber.Format(_T("%d"), frameNumber);
	frameEdit.SetWindowTextW(str_frameNumber);
}

/**************************************************************************
*  @Function Name :: InitFrameNumber
*  @Description   :: ��ʼ����֡�����û��������Խ��л���
*  @Author        :: zzw
*  @Parameters    :: totalFrameNumber:�ܵ�֡��
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
*  @Description   :: ���ŵĽ�����������
*  @Author        :: zzw
*  @Parameters    :: nSBCode:OnHScroll���� nPos:nHScroll����
*					 pScrollBar:nHScroll����
*  @Return        :: None
**************************************************************************/
void CPlayToolBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	unsigned int m_int = playSlider.GetPos();//ȡ�õ�ǰλ��ֵ
											 //ֻ��Ҫ����־λ��һ��
	CRadarViewDoc *pDoc = (CRadarViewDoc *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveDocument();

	pDoc->myReader.Stop();//��ֹͣ�����˴���ֹ֮ͣ�����¿�ʼ��Ҫ�����ٶ�
	pDoc->myRawProcessor.End_ClearFrame();
	pDoc->myFrameProcessor.ReTrasform();//�����
	pDoc->myReader.GoToFrame(m_int);
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CRadarViewView *pView = (CRadarViewView *)pMain->GetActiveView();
	if (!pMain->Get_STARTSTOP_Bitmap())
	{//��ť�ǲ���״̬
		pDoc->myReader.Start();
		//�����ٶȽ��е���
		int nowSpeed;
		pMain->Get_Speed_Bitmap(nowSpeed);
		//�����ٶȽ��м��ٵ���
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
	pView->SetPoint3DWndFocus();//���ý���
}