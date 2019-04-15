#include "stdafx.h"
#include "CToolBar24.h"
#include "resource.h"
#include "RadarViewView.h"

BEGIN_MESSAGE_MAP(CColorToolBar, CToolBar)
	ON_WM_WINDOWPOSCHANGED()
	ON_CBN_SELENDOK(COLOR_ATTRIBUTE, &CColorToolBar::OnSelchangeCombo)//选择编码的属性
END_MESSAGE_MAP()

/**************************************************************************
*  @Function Name :: CColorToolBar
*  @Description   :: CColorToolBar类的构造函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CColorToolBar::CColorToolBar()
{
}

/**************************************************************************
*  @Function Name :: ~CColorToolBar
*  @Description   :: CColorToolBar类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CColorToolBar::~CColorToolBar()
{
}

/**************************************************************************
*  @Function Name :: CharToTchar
*  @Description   :: 将char转换成tchar
*  @Author        :: zzw
*  @Parameters    :: _char:待转换的char*
*					 tchar:转换之后的tchar*
*  @Return        :: None
**************************************************************************/
void CharToTchar(const char * _char, TCHAR * tchar)
{
	int iLength;
	iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
}

/**************************************************************************
*  @Function Name :: InitColorAttribute
*  @Description   :: 初始化颜色属性
*  @Author        :: zzw
*  @Parameters    :: ID_Attribute:显示颜色属性的combox的ID
*
*  @Return        :: None
**************************************************************************/
BOOL CColorToolBar::InitColorAttribute(UINT ID_Attribute)
{
	int index = CommandToIndex(ID_Attribute);
	SetButtonInfo(index, ID_Attribute, TBBS_SEPARATOR, 120);
	CRect rect;
	GetItemRect(index, &rect);

	rect.top += 3;//这样看起来更加美观一点
	if (!AttributeComBox.Create(WS_VISIBLE | WS_CHILD | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST,
		rect, this, ID_Attribute))
	{
		TRACE0("Failed to create ComBox\n");
		return FALSE;
	}

	//添加内容
	CString tmpshow;
	TCHAR tcharshow[1000];
	CharToTchar(ColorCodingName[0],tcharshow);//"point id"
	tmpshow.Format(_T("%s"), tcharshow);
	AttributeComBox.AddString(tmpshow);//0
	CharToTchar(ColorCodingName[1], tcharshow);//"distance"
	tmpshow.Format(_T("%s"), tcharshow);
	AttributeComBox.AddString(tmpshow);//1
	CharToTchar(ColorCodingName[2],tcharshow);//"x"
	tmpshow.Format(_T("%s"), tcharshow);
	AttributeComBox.AddString(tmpshow);//2
	CharToTchar(ColorCodingName[3], tcharshow);//"y"
	tmpshow.Format(_T("%s"), tcharshow);
	AttributeComBox.AddString(tmpshow);//3
	CharToTchar(ColorCodingName[4], tcharshow);//"z"
	tmpshow.Format(_T("%s"), tcharshow);
	AttributeComBox.AddString(tmpshow);//4
	CharToTchar(ColorCodingName[5], tcharshow);//"intensity"
	tmpshow.Format(_T("%s"), tcharshow);
	AttributeComBox.AddString(tmpshow);//5
	CharToTchar(ColorCodingName[6], tcharshow);//"horizontal angle"
	tmpshow.Format(_T("%s"), tcharshow);
	AttributeComBox.AddString(tmpshow);//6
	CharToTchar(ColorCodingName[7], tcharshow);//"vertical angle"
	tmpshow.Format(_T("%s"), tcharshow);
	AttributeComBox.AddString(tmpshow);//7
	CharToTchar(ColorCodingName[8], tcharshow);//"channel"
	tmpshow.Format(_T("%s"), tcharshow);
	AttributeComBox.AddString(tmpshow);//8
	CharToTchar(ColorCodingName[9], tcharshow);//"timestamp"
	tmpshow.Format(_T("%s"), tcharshow);
	AttributeComBox.AddString(tmpshow);//9
	AttributeComBox.SetCurSel(5);//默认选中

	return TRUE;
}

/**************************************************************************
*  @Function Name :: OnSelchangeCombo
*  @Description   :: combobox的响应事件
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CColorToolBar::OnSelchangeCombo()
{
	int bnt, receivenum;
	bnt = AttributeComBox.GetCurSel();
	if (CB_ERR != bnt)
	{
		CRadarViewView *pView = (CRadarViewView *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView();
		Points3DWnd * m_pPoints3DWnd = pView->Get3DWnd();

		COLORCODING chooseColorCoding = COLORCODING(bnt);
		m_pPoints3DWnd->SetColorCoding(chooseColorCoding, true);
		//对标尺进行重新绘制
		pView->ReDrawLegend();

		m_pPoints3DWnd->SetFocus();
	}
}

/**************************************************************************
*  @Function Name :: SetAttriComBoxCurSel
*  @Description   :: 设置AttributeComBox的值
*  @Author        :: zzw
*  @Parameters    :: setSel:设置属性的下标
*					 bUpdate:表示更改颜色编码属性之后是否redraw,默认否
*  @Return        :: None
**************************************************************************/
void CColorToolBar::SetAttriComBoxCurSel(int setSel,bool bUpdate)
{
	AttributeComBox.SetCurSel(setSel);
	if (CB_ERR != setSel)
	{
		CRadarViewView *pView = (CRadarViewView *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView();
		Points3DWnd * m_pPoints3DWnd = pView->Get3DWnd();

		COLORCODING chooseColorCoding = COLORCODING(setSel);
		m_pPoints3DWnd->SetColorCoding(chooseColorCoding, bUpdate);
		//对标尺进行重新绘制
		pView->ReDrawLegend();

		m_pPoints3DWnd->SetFocus();
	}
}