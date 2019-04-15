#include "stdafx.h"
#include "CEditVC.h"

IMPLEMENT_DYNAMIC(CEditVC, CEdit)

/**************************************************************************
*  @Function Name :: CEditVC
*  @Description   :: CEditVC类的构造函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CEditVC::CEditVC()
{
	m_rcClient.SetRectEmpty();
}

/**************************************************************************
*  @Function Name :: ~CEditVC
*  @Description   :: CEditVC类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CEditVC::~CEditVC()
{
}

BEGIN_MESSAGE_MAP(CEditVC, CEdit)
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_GETDLGCODE()
	ON_WM_CREATE()
END_MESSAGE_MAP()

/**************************************************************************
*  @Function Name :: OnCreate
*  @Description   :: CEditVC类在create函数被调用的时候调用
*  @Author        :: zzw
*  @Parameters    :: lpCreateStruct:包含了应用程序中窗口过程的初始化参数
*
*  @Return        :: 运行是否成功
**************************************************************************/
int CEditVC::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

/**************************************************************************
*  @Function Name :: OnNcCalcSize
*  @Description   :: 求出字体的边框宽度
*  @Author        :: zzw
*  @Parameters    :: bCalcValidRects:OnNcCalcSize中参数
*					 lpncsp:OnNcCalcSize中参数
*  @Return        :: None
**************************************************************************/
void CEditVC::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp)
{
	CEdit::OnNcCalcSize(bCalcValidRects, lpncsp);

	CRect rectWnd;
	GetClientRect(rectWnd);
	CFont* font = GetFont();
	if (font)
	{
		CDC* pDC = GetDC();

		SIZE szName = { 0, 0 };
		//使用该函数获得所选字体中指定字符串的高度和宽度
		GetTextExtent(pDC->m_hDC, "中国", _tcslen(L"中国"), (HFONT)GetFont()->m_hObject, &szName);

		//根据字体高度求出上下要空出多少距离
		UINT nHeight = (rectWnd.Height() - szName.cy) / 2;

		if (rectWnd.Height() == 0)
		{//说明一开始字体就是居中，所以没有必要继续进行处理
			return;
		}

		//求出字体窗格的居中矩阵
		m_rcClient = rectWnd;
		lpncsp->rgrc[0].top += nHeight;
		lpncsp->rgrc[0].bottom -= nHeight;
		lpncsp->rgrc[0].left += 2;
		lpncsp->rgrc[0].right -= 2;
	}
}

/**************************************************************************
*  @Function Name :: GetTextExtent
*  @Description   :: 使用该函数获得所选字体中指定字符串的高度和宽度
*  @Author        :: zzw
*  @Parameters    :: hDC:设备描述表句柄 lpString:指向一个字符串 nCount:所要计算字符数
*					 hFont:要载入的对象句柄 lpSize:装载字串范围的高度和宽度信息 iDir:默认是0
*  @Return        :: 运行是否成功
**************************************************************************/
BOOL CEditVC::GetTextExtent(HDC hDC, LPCSTR lpString, int nCount, HFONT hFont, LPSIZE lpSize, int iDir)
{
	BOOL ret = FALSE;

	if (NULL != hDC)
	{
		//把一个对象（位图、画笔、画刷等）选入指定的设备描述表
		//hdc设备描述表句柄，hFont要载入的对象句柄
		HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);

		if (iDir == 0)
		{//该函数取得一个指定字符串里的字符数，该字符数将符合一个指定的空间
			::GetTextExtentPointA(hDC, lpString, nCount, lpSize);
		}
		else
		{
			lpSize->cx = 0;
			lpSize->cy = 0;

			int textlen = strlen(lpString);
			int dispChCount = min(textlen, nCount);
			char ch[3];
			int i;

			SIZE sz;

			for (i = 0; i<dispChCount; i++)
			{
				ch[0] = lpString[i];
				ch[1] = 0;
				if (lpString[i] != 0 && lpString[i] > 127)
				{
					i++;
					ch[1] = lpString[i];
					ch[2] = 0;
				}
				::GetTextExtentPointA(hDC, ch, strlen(ch), &sz);
				lpSize->cy += sz.cy;
				if (sz.cx > lpSize->cx)
				{
					lpSize->cx = sz.cx;
				}
			}
		}

		::SelectObject(hDC, hOldFont);
		ret = TRUE;
	}
	return ret;
}

/**************************************************************************
*  @Function Name :: OnNcPaint
*  @Description   :: 实现绘制窗架
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CEditVC::OnNcPaint()
{
	CWindowDC dc(this);

	CBrush Brush(RGB(255, 20, 0));
	dc.FillRect(m_rcClient, &Brush);

	CBrush Brush2(RGB(240, 240, 240));
	CRect dd = m_rcClient;
	dd.InflateRect(-1, -1, -1, -1);//增大或者减少指定矩形的宽和高
	dc.FillRect(dd, &Brush2);
}

/**************************************************************************
*  @Function Name :: OnGetDlgCode
*  @Description   :: 调用控件，以便控件可以处理鼠标和tab键的输入
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
UINT CEditVC::OnGetDlgCode()
{
	if (m_rcClient.IsRectEmpty())
	{//改变一个子窗口，弹出式窗口，及顶层窗口根据它们在屏幕上出现的顺序排序
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
	}

	return CEdit::OnGetDlgCode();
}