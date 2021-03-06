// CCOLORLEGENDDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "ColorLegendDlg.h"
#include "afxdialogex.h"
#include "Resource.h"
#include "ColorTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CCOLORLEGENDDlg 对话框

IMPLEMENT_DYNAMIC(ColorLegendDlg, CDialogEx)

/**************************************************************************
*  @Function Name :: ColorLegendDlg
*  @Description   :: ColorLegendDlg类的构造函数
*  @Author        :: zzw
*  @Parameters    :: pParent:父窗口
*
*  @Return        :: None
**************************************************************************/
ColorLegendDlg::ColorLegendDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_COLORLEGEND, pParent)
{
	colorIdx = 0;
	AttrIdx = CODING_INTEN;
	legendMin = 0.0;
	legendMax = 10.0;
}

/**************************************************************************
*  @Function Name :: ~ColorLegendDlg
*  @Description   :: ColorLegendDlg类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
ColorLegendDlg::~ColorLegendDlg()
{
}

void ColorLegendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, PIC_COLORLEGEND, m_colorLegend);
	DDX_Control(pDX, IDC_TXT_LEGENDNAME, m_txtName);
	DDX_Control(pDX, IDC_TXT_UPSCALE, m_txtUp);
	DDX_Control(pDX, IDC_TXT_QUASCALE, m_txtQua);
	DDX_Control(pDX, IDC_TXT_MIDSCALE, m_txtMid);
	DDX_Control(pDX, IDC_TXT_BQUASCALE, m_txtBQua);
	DDX_Control(pDX, IDC_TXT_BOTTOMSCALE, m_txtbottom);
}

BEGIN_MESSAGE_MAP(ColorLegendDlg, CDialogEx)
	//ON_WM_PAINT()
	ON_WM_MOVING()
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CCOLORLEGENDDlg 消息处理程序

/**************************************************************************
*  @Function Name :: OnInitDialog
*  @Description   :: ColorLegendDlg初始化框格函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
BOOL ColorLegendDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
							
	::SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, ::GetWindowLongPtr(GetSafeHwnd(), GWL_EXSTYLE) ^ 0x80000);
	//::SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, ::GetWindowLongPtr(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);
	this->SetLayeredWindowAttributes(0, (255 * 70) / 400, LWA_ALPHA);

	COLORREF maskColor = RGB(255, 255, 255);
	HINSTANCE hInst = LoadLibrary(_T("User32.DLL"));
	if (hInst)
	{
		typedef BOOL(WINAPI * MYFUNC)(HWND, COLORREF, BYTE, DWORD);
		MYFUNC fun = NULL;
		fun = (MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if (fun)fun(this->GetSafeHwnd(), maskColor, 255, 1);
		FreeLibrary(hInst);
	}

	CFont font;
	font.CreatePointFont(20,_T("宋体"));
	m_txtName.SetFont(&font);

	//GetDlgItem(IDC_TXT_LEGENDNAME)->S

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/**************************************************************************
*  @Function Name :: OnNcHitTest
*  @Description   :: 鼠标点击非标题栏区拖动MFC对话框
*  @Author        :: zzw
*  @Parameters    :: pt:OnNcHitTest参数
*
*  @Return        :: 运行是否成功
**************************************************************************/
LRESULT ColorLegendDlg::OnNcHitTest(CPoint pt)
{
	LRESULT nHitTest = CDialog::OnNcHitTest(pt);
	if (nHitTest == HTCLIENT)
	{
		nHitTest = HTCAPTION;
	}
	return nHitTest;
}

/**************************************************************************
*  @Function Name :: OnCtlColor
*  @Description   :: 改变控件的颜色
*  @Author        :: zzw
*  @Parameters    :: pDC:设备描述表句柄 pWnd:窗口句柄
*					 nCtlColor:显示颜色的方式
*  @Return        :: None
**************************************************************************/
HBRUSH ColorLegendDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	CBrush m_brush(RGB(255, 255, 255));
	CRect m_rect;
	GetClientRect(m_rect);
	pDC->SelectObject(&m_brush);
	pDC->FillRect(m_rect, &m_brush);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		if (pWnd->GetDlgCtrlID() == PIC_COLORLEGEND)
		{
			CRect rcTemp;
			rcTemp = m_rect;
			//rcTemp.top += 6;
			//rcTemp.bottom -= 6;

			double iColStep = 256.0 / rcTemp.Height();
			for (int iHeight = 0; iHeight < rcTemp.Height(); iHeight++)
			{//计算RGB
				int idx = int(iHeight* iColStep);
				if (idx > 255)
				{
					idx = 255;
				}
				if (idx < 0)
				{
					idx = 0;
				}
				CBrush m_other(RGB(ColorsTable[colorIdx][idx][0], ColorsTable[colorIdx][idx][1], ColorsTable[colorIdx][idx][2]));

				CRect rcTemptmp;
				rcTemptmp.left = rcTemp.left;
				rcTemptmp.right = rcTemp.right;
				rcTemptmp.top = iHeight;
				rcTemptmp.bottom = iHeight+1;

				pDC->SelectObject(&m_other);
				pDC->FillRect(rcTemptmp, &m_other);
			}

			//将控件设置为透明  
			pDC->SetBkMode(TRANSPARENT);
			hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if (pWnd->GetDlgCtrlID() == IDC_TXT_LEGENDNAME)
		{//属性名字txt
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(255, 255, 0));
			CString m_show(ColorCodingName[int(AttrIdx)]);
			GetDlgItem(IDC_TXT_LEGENDNAME)->SetWindowTextW(m_show);
			hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if (pWnd->GetDlgCtrlID() == IDC_TXT_UPSCALE)
		{//顶
			SetScaleTXT(pDC, IDC_TXT_UPSCALE, legendMin);
			hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if (pWnd->GetDlgCtrlID() == IDC_TXT_QUASCALE)
		{//四分之一
			SetScaleTXT(pDC, IDC_TXT_QUASCALE, ((legendMin * 3 / 4) + (legendMax / 4)));
			hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if (pWnd->GetDlgCtrlID() == IDC_TXT_MIDSCALE)
		{//中间
			SetScaleTXT(pDC, IDC_TXT_MIDSCALE, (legendMin + legendMax) / 2);
			hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if (pWnd->GetDlgCtrlID() == IDC_TXT_BQUASCALE)
		{//四分之三
			SetScaleTXT(pDC, IDC_TXT_BQUASCALE, ((legendMin / 4) + (legendMax * 3 / 4)));
			hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else if (pWnd->GetDlgCtrlID() == IDC_TXT_BOTTOMSCALE)
		{//底
			SetScaleTXT(pDC, IDC_TXT_BOTTOMSCALE, legendMax);
			hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
		}
	}
	return m_brush;
}

/**************************************************************************
*  @Function Name :: SetAttribute
*  @Description   :: 设置颜色标尺的属性，以便之后对颜色标尺进行刷新的时候进行更新
*  @Author        :: zzw
*  @Parameters    :: setcolorIdx:设置的颜色编码下标 ColorCodingIdx:参加颜色编码的属性
*					 setMin:颜色编码的最小值 setMax:颜色编码的最大值
*  @Return        :: None
**************************************************************************/
void ColorLegendDlg::SetAttribute(int setcolorIdx, COLORCODING ColorCodingIdx,double setMin,double setMax)
{
	colorIdx = setcolorIdx;
	AttrIdx = ColorCodingIdx;
	legendMin = setMin;
	legendMax = setMax;
}

/**************************************************************************
*  @Function Name :: SetScaleTXT
*  @Description   :: 设置标尺显示的字
*  @Author        :: zzw
*  @Parameters    :: pDC:设备描述表句柄 m_ID:对象ID
*					 data:需要显示的数值
*  @Return        :: None
**************************************************************************/
void ColorLegendDlg::SetScaleTXT(CDC* pDC, UINT m_ID,double data)
{
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255, 255, 0));
	CString m_show;
	switch (AttrIdx)
	{//根据不同的属性显示不同的单位
	case CODING_ID:
		m_show.Format(_T(" %d"), int(data));
		break;
	case CODING_DIS:
		m_show.Format(_T(" %0.3f mm"), data);
		break;
	case CODING_X:
		m_show.Format(_T(" %0.3f m"), data);
		break;
	case CODING_Y:
		m_show.Format(_T(" %0.3f m"), data);
		break;
	case CODING_Z:
		m_show.Format(_T(" %0.3f m"), data);
		break;
	case CODING_INTEN:
		m_show.Format(_T(" %0.3f"), data);
		break;
	case CODING_HANGLE:
		m_show.Format(_T(" %0.3f °"), data/100.0);//显示单位是度
		break;
	case CODING_VANGLE:
		m_show.Format(_T(" %0.2f"), data);
		break;
	case CODING_CHAN:
		m_show.Format(_T(" %0.1f"), data);
		break;
	case CODING_TIME:
		m_show.Format(_T(" %0.2f"), data);
		break;
	}
	GetDlgItem(m_ID)->SetWindowTextW(m_show);
}

/**************************************************************************
*  @Function Name :: OnMoving
*  @Description   :: 移动过程中对颜色标尺移动范围进行限定
*  @Author        :: zzw
*  @Parameters    :: nSide:OnMoving参数
*					 lpRect:移动范围
*  @Return        :: None
**************************************************************************/
void ColorLegendDlg::OnMoving(UINT nSide, LPRECT lpRect)
{
	CRect rectParent;
	GetParent()->GetClientRect(&rectParent);
	GetParent()->ClientToScreen(&rectParent);
	rectParent.top += 20;//在主窗口的基础之上剃掉外围，使得显示更加美观
	rectParent.bottom -= 10;
	rectParent.right += 70;
	CRect rectChild;
	GetWindowRect(&rectChild);//子窗口的位置

	lpRect->left = min(lpRect->left, rectParent.right - rectChild.Width());
	lpRect->left = max(lpRect->left, rectParent.left);

	lpRect->top = min(lpRect->top, rectParent.bottom - rectChild.Height());
	lpRect->top = max(lpRect->top, rectParent.top);

	lpRect->right = min(lpRect->right, rectParent.right);
	lpRect->right = max(lpRect->right, rectParent.left + rectChild.Width());

	lpRect->bottom = min(lpRect->bottom, rectParent.bottom);
	lpRect->bottom = max(lpRect->bottom, rectParent.top + rectChild.Height());

	CDialogEx::OnMoving(nSide, lpRect);
}