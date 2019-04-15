#include "stdafx.h"
#include "CEditVC.h"

IMPLEMENT_DYNAMIC(CEditVC, CEdit)

/**************************************************************************
*  @Function Name :: CEditVC
*  @Description   :: CEditVC��Ĺ��캯��
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
*  @Description   :: CEditVC�����������
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
*  @Description   :: CEditVC����create���������õ�ʱ�����
*  @Author        :: zzw
*  @Parameters    :: lpCreateStruct:������Ӧ�ó����д��ڹ��̵ĳ�ʼ������
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int CEditVC::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

/**************************************************************************
*  @Function Name :: OnNcCalcSize
*  @Description   :: �������ı߿���
*  @Author        :: zzw
*  @Parameters    :: bCalcValidRects:OnNcCalcSize�в���
*					 lpncsp:OnNcCalcSize�в���
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
		//ʹ�øú��������ѡ������ָ���ַ����ĸ߶ȺͿ��
		GetTextExtent(pDC->m_hDC, "�й�", _tcslen(L"�й�"), (HFONT)GetFont()->m_hObject, &szName);

		//��������߶��������Ҫ�ճ����پ���
		UINT nHeight = (rectWnd.Height() - szName.cy) / 2;

		if (rectWnd.Height() == 0)
		{//˵��һ��ʼ������Ǿ��У�����û�б�Ҫ�������д���
			return;
		}

		//������崰��ľ��о���
		m_rcClient = rectWnd;
		lpncsp->rgrc[0].top += nHeight;
		lpncsp->rgrc[0].bottom -= nHeight;
		lpncsp->rgrc[0].left += 2;
		lpncsp->rgrc[0].right -= 2;
	}
}

/**************************************************************************
*  @Function Name :: GetTextExtent
*  @Description   :: ʹ�øú��������ѡ������ָ���ַ����ĸ߶ȺͿ��
*  @Author        :: zzw
*  @Parameters    :: hDC:�豸�������� lpString:ָ��һ���ַ��� nCount:��Ҫ�����ַ���
*					 hFont:Ҫ����Ķ����� lpSize:װ���ִ���Χ�ĸ߶ȺͿ����Ϣ iDir:Ĭ����0
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
BOOL CEditVC::GetTextExtent(HDC hDC, LPCSTR lpString, int nCount, HFONT hFont, LPSIZE lpSize, int iDir)
{
	BOOL ret = FALSE;

	if (NULL != hDC)
	{
		//��һ������λͼ�����ʡ���ˢ�ȣ�ѡ��ָ�����豸������
		//hdc�豸����������hFontҪ����Ķ�����
		HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);

		if (iDir == 0)
		{//�ú���ȡ��һ��ָ���ַ�������ַ��������ַ���������һ��ָ���Ŀռ�
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
*  @Description   :: ʵ�ֻ��ƴ���
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
	dd.InflateRect(-1, -1, -1, -1);//������߼���ָ�����εĿ�͸�
	dc.FillRect(dd, &Brush2);
}

/**************************************************************************
*  @Function Name :: OnGetDlgCode
*  @Description   :: ���ÿؼ����Ա�ؼ����Դ�������tab��������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
UINT CEditVC::OnGetDlgCode()
{
	if (m_rcClient.IsRectEmpty())
	{//�ı�һ���Ӵ��ڣ�����ʽ���ڣ������㴰�ڸ�����������Ļ�ϳ��ֵ�˳������
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
	}

	return CEdit::OnGetDlgCode();
}