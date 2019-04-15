#include "stdafx.h"
#include "CToolBar24.h"

IMPLEMENT_DYNAMIC(CToolBar24, CToolBar)

BEGIN_MESSAGE_MAP(CToolBar24, CToolBar)
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

/**************************************************************************
*  @Function Name :: CToolBar24
*  @Description   :: CToolBar24类的构造函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CToolBar24::CToolBar24()
{
	m_bMenuRemoved = false;
}

/**************************************************************************
*  @Function Name :: ~CToolBar24
*  @Description   :: CToolBar24类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CToolBar24::~CToolBar24()
{
}

/**************************************************************************
*  @Function Name :: Init
*  @Description   :: 将工具栏中所有控件的位置都进行具体化的初始化
*  @Author        :: zzw
*  @Parameters    :: BITMap_ID:一般状态下工具栏位图 Disabled_ID:disable状态下工具栏位图
*					 Hot_ID:热键状态下工具栏位图
*  @Return        :: None
**************************************************************************/
BOOL CToolBar24::Init(UINT BITMap_ID, UINT Disabled_ID, UINT Hot_ID)
{
	m_bmToolbar.LoadBitmap(BITMap_ID);
	m_imgToolbar.Create(24, 24, ILC_COLOR24 | ILC_MASK, 1, 1);
	m_imgToolbar.Add(&m_bmToolbar, RGB(255, 0, 255));
	GetToolBarCtrl().SetImageList(&m_imgToolbar);

	m_bmToolbarDis.LoadBitmap(Disabled_ID);
	m_imgToolbarDis.Create(24, 24, ILC_COLOR24 | ILC_MASK, 1, 1);
	m_imgToolbarDis.Add(&m_bmToolbarDis, RGB(255, 0, 255));
	GetToolBarCtrl().SetDisabledImageList(&m_imgToolbarDis);

	m_bmToolbarNA.LoadBitmap(Hot_ID);
	m_imgToolbarNA.Create(24, 24, ILC_COLOR24 | ILC_MASK, 1, 1);
	m_imgToolbarNA.Add(&m_bmToolbarNA, RGB(255, 0, 255));
	GetToolBarCtrl().SetHotImageList(&m_imgToolbarNA);

	return true;
}

/**************************************************************************
*  @Function Name :: UpdateColorImage
*  @Description   :: 更新idx(从0开始)图标的图片为bitmap所指向的内容
*  @Author        :: zzw
*  @Parameters    :: BITMap_ID:一般状态下工具栏位图 Disabled_ID:disable状态下工具栏位图
*					 Hot_ID:热键状态下工具栏位图 idx:更新图标的按钮下标
*  @Return        :: None
**************************************************************************/
void CToolBar24::UpdateColorImage(UINT BITMap_ID, UINT Disabled_ID, UINT Hot_ID,int idx)
{
	HICON hicon1 = NULL, hicon2 = NULL, hicon3 = NULL;
	hicon1 = AfxGetApp()->LoadIconW(BITMap_ID);
	m_imgToolbar.Replace(idx, hicon1);

	hicon2 = AfxGetApp()->LoadIconW(Disabled_ID);
	m_imgToolbarDis.Replace(idx, hicon2);

	hicon3 = AfxGetApp()->LoadIconW(Hot_ID);
	m_imgToolbarNA.Replace(idx, hicon3);
}

/**************************************************************************
*  @Function Name :: UpdateColorImage
*  @Description   :: 更换工具栏中第idx个按钮的图像为bitmap中第idx个图像
*  @Author        :: zzw
*  @Parameters    :: BITMap_ID:一般状态下工具栏位图 Disabled_ID:disable状态下工具栏位图
*					 idx:更新图标的按钮下标
*  @Return        :: None
**************************************************************************/
void CToolBar24::UpdateColorImage(UINT BITMap_ID, UINT Disabled_ID, int idx)
{
	CBitmap cbitmap1, cbitmap2;
	BITMAP bm1, bm2;
	cbitmap1.LoadBitmap(BITMap_ID);
	cbitmap2.LoadBitmap(Disabled_ID);
	cbitmap1.GetBitmap(&bm1);
	cbitmap2.GetBitmap(&bm2);
	if ((idx * 24 + 23 > bm1.bmWidth) || (idx * 24 + 23 > bm2.bmWidth))
	{
		return;
	}
	int size = bm1.bmHeight*bm1.bmWidthBytes;
	BYTE *lpBits1 = new BYTE[size];
	BYTE *lpBits2 = new BYTE[size];
	memset(lpBits1, 0, sizeof(BYTE)*size);
	memset(lpBits2, 0, sizeof(BYTE)*size);
	cbitmap1.GetBitmapBits(size, lpBits1);//得到RGBA数据
	cbitmap2.GetBitmapBits(size, lpBits2);//得到RGBA数据
	BYTE *RGBPIC1 = new BYTE[bm1.bmHeight * 24 * 4];
	BYTE *RGBPIC2 = new BYTE[bm2.bmHeight * 24 * 4];
	memset(RGBPIC1, 0, sizeof(BYTE)*bm1.bmHeight * 24 * 4);
	memset(RGBPIC2, 0, sizeof(BYTE)*bm2.bmHeight * 24 * 4);

	for (int x = 0; x < bm1.bmHeight; x++)
	{
		for (int y = 24 * idx; y < 24 * (idx + 1); y++)
		{
			RGBPIC1[x * 24 * 4 + (y - 24 * idx) * 4] = lpBits1[x*bm1.bmWidthBytes + y * 4];//R
			RGBPIC1[x * 24 * 4 + (y - 24 * idx) * 4 + 1] = lpBits1[x*bm1.bmWidthBytes + y * 4 + 1];//G
			RGBPIC1[x * 24 * 4 + (y - 24 * idx) * 4 + 2] = lpBits1[x*bm1.bmWidthBytes + y * 4 + 2];//B
			RGBPIC1[x * 24 * 4 + (y - 24 * idx) * 4 + 3] = lpBits1[x*bm1.bmWidthBytes + y * 4 + 3];//A

			RGBPIC2[x * 24 * 4 + (y - 24 * idx) * 4] = lpBits2[x*bm2.bmWidthBytes + y * 4];//R
			RGBPIC2[x * 24 * 4 + (y - 24 * idx) * 4 + 1] = lpBits2[x*bm2.bmWidthBytes + y * 4 + 1];//G
			RGBPIC2[x * 24 * 4 + (y - 24 * idx) * 4 + 2] = lpBits2[x*bm2.bmWidthBytes + y * 4 + 2];//B
			RGBPIC2[x * 24 * 4 + (y - 24 * idx) * 4 + 3] = lpBits2[x*bm2.bmWidthBytes + y * 4 + 3];//A
		}
	}

	CBitmap res_cbitmap1, res_cbitmap2;
	res_cbitmap1.CreateBitmap(24, bm1.bmHeight, 1, 32, RGBPIC1);
	res_cbitmap2.CreateBitmap(24, bm2.bmHeight, 1, 32, RGBPIC2);
	m_imgToolbar.Replace(idx, &res_cbitmap1, NULL);
	m_imgToolbarDis.Replace(idx, &res_cbitmap2, NULL);
	m_imgToolbarNA.Replace(idx, &res_cbitmap1, NULL);

	delete[]lpBits1;
	delete[]lpBits2;
	delete[]RGBPIC1;
	delete[]RGBPIC2;
}

/**************************************************************************
*  @Function Name :: OnWindowPosChanged
*  @Description   :: 工具栏位置发生变化的响应函数,主要是为了使得工具栏不可以被主动关闭
*  @Author        :: zzw
*  @Parameters    :: lpwndpos:含有位置信息，OnWindowPosChanged函数参数
*
*  @Return        :: None
**************************************************************************/
void CToolBar24::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos)
{
	CToolBar::OnWindowPosChanged(lpwndpos);
	// should only be called once, when floated.
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