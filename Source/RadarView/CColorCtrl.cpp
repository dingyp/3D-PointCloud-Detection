#include "stdafx.h"
#include "ColorCodingDlg.h"
#include "ColorTable.h"

BEGIN_MESSAGE_MAP(CColorCtrl, CListCtrl)
	ON_WM_MEASUREITEM()
	ON_WM_MEASUREITEM_REFLECT()
END_MESSAGE_MAP()

/**************************************************************************
*  @Function Name :: CColorCtrl
*  @Description   :: CColorCtrl��Ĺ��캯��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CColorCtrl::CColorCtrl()
{
	colorlist.clear();
	m_nRowHeight = 30;//Ĭ��30
}

/**************************************************************************
*  @Function Name :: ~CColorCtrl
*  @Description   :: CColorCtrl�����������
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
*  @Description   :: ������ɫ�б���и�
*  @Author        :: zzw
*  @Parameters    :: nHeight:�����иߵĸ߶�
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
*  @Description   :: �����иߵ���Ӧ������
*  @Author        :: zzw
*  @Parameters    :: nIDCtl:OnMeasureItem����
*					 lpMeasureItemStruct:OnMeasureItem����
*  @Return        :: None
**************************************************************************/
void CColorCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

/**************************************************************************
*  @Function Name :: MeasureItem
*  @Description   :: ����item�ĸ�
*  @Author        :: zzw
*  @Parameters    :: lpMeasureItemStruct:����CListCtrl����Ľṹ��Ϣ
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
*  @Description   :: ����item�Ľṹ,�����麯��
*  @Author        :: zzw
*  @Parameters    :: lpDrawItemStruct:����λ����Ϣ
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
		{//ѡ��ĳһ��ɫ����֮�������ʾ
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
				{//����RGB���ȸ�����ʾ���Ⱦ���ĳһ����Ӧ����ʾʲô��ɫ
					int idx = int(iLength* iColStep);
					if (idx > 255) idx = 255;
					if (idx < 0)idx = 0;
					
					//������ɫ
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
		{//û��ѡ��֮��Ļָ�
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
				{//����RGB���ȸ�����ʾ���Ⱦ���ĳһ����Ӧ����ʾʲô��ɫ
					int idx = int(iLength* iColStep);
					if (idx > 255)
					{
						idx = 255;
					}
					if (idx < 0)
					{
						idx = 0;
					}

					//������ɫ
					CPen Pen(PS_SOLID, 1, RGB(onecolor.ColorTable[idx][0], onecolor.ColorTable[idx][1], onecolor.ColorTable[idx][2]));
					pDC->SelectObject(&Pen);
					pDC->MoveTo(iLength, rcTemp.top);
					pDC->LineTo(iLength, rcTemp.bottom);
				}
			}
			pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));//���ֵ���ɫ���лָ�
		}

		pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));
		UINT uFormat = DT_LEFT;

		::DrawText(lpDrawItemStruct->hDC, lpBuffer, (strlen((const char*)lpBuffer) / 2), &lpDrawItemStruct->rcItem, uFormat);//����֤
		pDC->SelectStockObject(SYSTEM_FONT);
	}
}

/**************************************************************************
*  @Function Name :: InsertOneColor
*  @Description   :: ��ĳһ�±����ɫ������и�ֵ�͸�������
*  @Author        :: zzw
*  @Parameters    :: nColorIdx:��ɫ�б���±� ColorTable:��ɫ����
*					 colorName:��ɫ������ַ�����
*  @Return        :: �Ƿ�ɹ�
**************************************************************************/
BOOL CColorCtrl::InsertOneColor(int nColorIdx, const unsigned char ColorTable[][4], LPCTSTR colorName)
{
	//����ɫ��������
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
		colorlist[nColorIdx] = onecolor;//���丳���±�ΪnColorIdx�ĵ�
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
*  @Description   :: �����±���col����ɫ����
*  @Author        :: zzw
*  @Parameters    :: col:��Ҫ���ص���ɫ������±�
*					 onecolor:��ɫ���������
*  @Return        :: �Ƿ�ɹ�
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