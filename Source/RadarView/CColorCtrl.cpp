#include "stdafx.h"
#include "ColorCodingDlg.h"
#include "ColorTable.h"

BEGIN_MESSAGE_MAP(CColorCtrl, CListCtrl)
	ON_WM_MEASUREITEM()
	ON_WM_MEASUREITEM_REFLECT()
END_MESSAGE_MAP()

/**************************************************************************
*  @Function Name :: CColorCtrl
*  @Description   :: CColorCtrl类的构造函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CColorCtrl::CColorCtrl()
{
	colorlist.clear();
	m_nRowHeight = 30;//默认30
}

/**************************************************************************
*  @Function Name :: ~CColorCtrl
*  @Description   :: CColorCtrl类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CColorCtrl::~CColorCtrl()
{
}

/**************************************************************************
*  @Function Name :: SetRowHeight
*  @Description   :: 设置颜色列表的行高
*  @Author        :: zzw
*  @Parameters    :: nHeight:设置行高的高度
*
*  @Return        :: None
**************************************************************************/
void CColorCtrl::SetRowHeight(int nHeight) 
{
	m_nRowHeight = nHeight;

	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcWin.Width();
	wp.cy = rcWin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
}

/**************************************************************************
*  @Function Name :: OnMeasureItem
*  @Description   :: 设置行高的响应处理函数
*  @Author        :: zzw
*  @Parameters    :: nIDCtl:OnMeasureItem参数
*					 lpMeasureItemStruct:OnMeasureItem参数
*  @Return        :: None
**************************************************************************/
void CColorCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

/**************************************************************************
*  @Function Name :: MeasureItem
*  @Description   :: 设置item的高
*  @Author        :: zzw
*  @Parameters    :: lpMeasureItemStruct:包含CListCtrl里面的结构信息
*
*  @Return        :: None
**************************************************************************/
void CColorCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (m_nRowHeight>0)
	{
		lpMeasureItemStruct->itemHeight = m_nRowHeight;
	}
}

/**************************************************************************
*  @Function Name :: DrawItem
*  @Description   :: 画出item的结构,重载虚函数
*  @Author        :: zzw
*  @Parameters    :: lpDrawItemStruct:包含位置信息
*
*  @Return        :: None
**************************************************************************/
void CColorCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	TCHAR lpBuffer[256];
	LV_ITEM lvi;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.iItem = lpDrawItemStruct->itemID;
	lvi.iSubItem = 0;
	lvi.pszText = lpBuffer;
	lvi.cchTextMax = sizeof(lpBuffer);
	VERIFY(GetItem(&lvi));

	LV_COLUMN lvc, lvcprev;
	::ZeroMemory(&lvc, sizeof(lvc));
	::ZeroMemory(&lvcprev, sizeof(lvcprev));
	lvc.mask = LVCF_WIDTH | LVCF_FMT;
	lvcprev.mask = LVCF_WIDTH | LVCF_FMT;


	for (int nCol = 0; GetColumn(nCol, &lvc); nCol++)
	{
		if (nCol > 0)
		{
			GetColumn(nCol - 1, &lvcprev);
			lpDrawItemStruct->rcItem.left += lvcprev.cx;
			lpDrawItemStruct->rcItem.right += lpDrawItemStruct->rcItem.left;
		}

		::ZeroMemory(&lvi, sizeof(lvi));
		lvi.iItem = lpDrawItemStruct->itemID;
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iSubItem = nCol;
		lvi.pszText = lpBuffer;
		lvi.cchTextMax = sizeof(lpBuffer);
		VERIFY(GetItem(&lvi));

		CDC* pDC;
		pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

		CRect rcItem;
		if (!GetSubItemRect(lpDrawItemStruct->itemID, nCol, LVIR_LABEL, rcItem))
		{
			continue;
		}
		if (lpDrawItemStruct->itemState & ODS_SELECTED)
		{//选中某一颜色编码之后高亮显示
			if (nCol == 0)
			{
				CRect rcTemp;
				rcTemp = rcItem;
				rcTemp.top += 6;
				rcTemp.bottom -= 6;

				OneColor onecolor;
				GetColColor(lpDrawItemStruct->itemID, onecolor);

				double iColStep = 256.0 / rcTemp.Width();
				for (int iLength = 0; iLength < rcTemp.Width(); iLength++)
				{//计算RGB，先根据显示长度决定某一条线应该显示什么颜色
					int idx = int(iLength* iColStep);
					if (idx > 255) idx = 255;
					if (idx < 0)idx = 0;
					
					//绘制颜色
					CPen Pen(PS_SOLID, 1, RGB(onecolor.ColorTable[idx][0], onecolor.ColorTable[idx][1], onecolor.ColorTable[idx][2]));
					pDC->SelectObject(&Pen);
					pDC->MoveTo(iLength, rcTemp.top);
					pDC->LineTo(iLength, rcTemp.bottom);

					CPen Background(PS_SOLID, 1, GetSysColor(COLOR_HIGHLIGHT));
					pDC->SelectObject(&Background);
					pDC->MoveTo(iLength, rcItem.top);
					pDC->LineTo(iLength, rcTemp.top);
					pDC->MoveTo(iLength, rcTemp.bottom);
					pDC->LineTo(iLength, rcItem.bottom);
				}
				for (int i = 0; i < 4; i++)
				{
					pDC->MoveTo(rcTemp.Width() + i, rcItem.top);
					pDC->LineTo(rcTemp.Width() + i, rcItem.bottom);
				}
			}
			else
			{
				pDC->FillSolidRect(&lpDrawItemStruct->rcItem, GetSysColor(COLOR_HIGHLIGHT));
				pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
			}
		}
		else
		{//没有选中之后的恢复
			if (nCol == 0)
			{
				CRect rcTemp;
				rcTemp = rcItem;
				rcTemp.top += 6;
				rcTemp.bottom -= 6;

				OneColor onecolor;
				GetColColor(lpDrawItemStruct->itemID, onecolor);

				double iColStep = 256.0 / rcTemp.Width();
				for (int iLength = 0; iLength < rcTemp.Width(); iLength++)
				{//计算RGB，先根据显示长度决定某一条线应该显示什么颜色
					int idx = int(iLength* iColStep);
					if (idx > 255)
					{
						idx = 255;
					}
					if (idx < 0)
					{
						idx = 0;
					}

					//绘制颜色
					CPen Pen(PS_SOLID, 1, RGB(onecolor.ColorTable[idx][0], onecolor.ColorTable[idx][1], onecolor.ColorTable[idx][2]));
					pDC->SelectObject(&Pen);
					pDC->MoveTo(iLength, rcTemp.top);
					pDC->LineTo(iLength, rcTemp.bottom);
				}
			}
			pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));//将字的颜色进行恢复
		}

		pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));
		UINT uFormat = DT_LEFT;

		::DrawText(lpDrawItemStruct->hDC, lpBuffer, (strlen((const char*)lpBuffer) / 2), &lpDrawItemStruct->rcItem, uFormat);//需验证
		pDC->SelectStockObject(SYSTEM_FONT);
	}
}

/**************************************************************************
*  @Function Name :: InsertOneColor
*  @Description   :: 对某一下标的颜色编码进行赋值和赋予名称
*  @Author        :: zzw
*  @Parameters    :: nColorIdx:颜色列表的下标 ColorTable:颜色编码
*					 colorName:颜色编码的字符名称
*  @Return        :: 是否成功
**************************************************************************/
BOOL CColorCtrl::InsertOneColor(int nColorIdx, const unsigned char ColorTable[][4], LPCTSTR colorName)
{
	//将颜色拷贝进来
	OneColor onecolor;
	for (int i = 0; i < 256; i++)
	{
		onecolor.ColorTable[i][0] = ColorTable[i][0];//R
		onecolor.ColorTable[i][1] = ColorTable[i][1];//G
		onecolor.ColorTable[i][2] = ColorTable[i][2];//B
	}
	int number = colorlist.size();
	if (number > nColorIdx)
	{
		colorlist[nColorIdx] = onecolor;//将其赋给下标为nColorIdx的点
	}
	else if (nColorIdx== number)
	{
		colorlist.push_back(onecolor);
	}
	else
	{
		return FALSE;
	}

	InsertItem(nColorIdx, _T(""));
	SetItemText(nColorIdx, 1, colorName);
	return TRUE;
}

/**************************************************************************
*  @Function Name :: GetColColor
*  @Description   :: 返回下标是col的颜色编码
*  @Author        :: zzw
*  @Parameters    :: col:需要返回的颜色编码的下标
*					 onecolor:颜色编码的引用
*  @Return        :: 是否成功
**************************************************************************/
BOOL CColorCtrl::GetColColor(int col, OneColor& onecolor)
{
	int number = colorlist.size();
	if (col >= number)
	{
		return FALSE;
	}
	onecolor = colorlist[col];
	return TRUE;
}