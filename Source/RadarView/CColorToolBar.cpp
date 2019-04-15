#include "stdafx.h"
#include "CToolBar24.h"
#include "resource.h"
#include "RadarViewView.h"

BEGIN_MESSAGE_MAP(CColorToolBar, CToolBar)
	ON_WM_WINDOWPOSCHANGED()
	ON_CBN_SELENDOK(COLOR_ATTRIBUTE, &CColorToolBar::OnSelchangeCombo)//ѡ����������
END_MESSAGE_MAP()

/**************************************************************************
*  @Function Name :: CColorToolBar
*  @Description   :: CColorToolBar��Ĺ��캯��
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
*  @Description   :: CColorToolBar�����������
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
*  @Description   :: ��charת����tchar
*  @Author        :: zzw
*  @Parameters    :: _char:��ת����char*
*					 tchar:ת��֮���tchar*
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
*  @Description   :: ��ʼ����ɫ����
*  @Author        :: zzw
*  @Parameters    :: ID_Attribute:��ʾ��ɫ���Ե�combox��ID
*
*  @Return        :: None
**************************************************************************/
BOOL CColorToolBar::InitColorAttribute(UINT ID_Attribute)
{
	int index = CommandToIndex(ID_Attribute);
	SetButtonInfo(index, ID_Attribute, TBBS_SEPARATOR, 120);
	CRect rect;
	GetItemRect(index, &rect);

	rect.top += 3;//������������������һ��
	if (!AttributeComBox.Create(WS_VISIBLE | WS_CHILD | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST,
		rect, this, ID_Attribute))
	{
		TRACE0("Failed to create ComBox\n");
		return FALSE;
	}

	//�������
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
	AttributeComBox.SetCurSel(5);//Ĭ��ѡ��

	return TRUE;
}

/**************************************************************************
*  @Function Name :: OnSelchangeCombo
*  @Description   :: combobox����Ӧ�¼�
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
		//�Ա�߽������»���
		pView->ReDrawLegend();

		m_pPoints3DWnd->SetFocus();
	}
}

/**************************************************************************
*  @Function Name :: SetAttriComBoxCurSel
*  @Description   :: ����AttributeComBox��ֵ
*  @Author        :: zzw
*  @Parameters    :: setSel:�������Ե��±�
*					 bUpdate:��ʾ������ɫ��������֮���Ƿ�redraw,Ĭ�Ϸ�
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
		//�Ա�߽������»���
		pView->ReDrawLegend();

		m_pPoints3DWnd->SetFocus();
	}
}