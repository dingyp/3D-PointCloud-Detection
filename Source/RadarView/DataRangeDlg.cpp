#include "stdafx.h"
#include "resource.h"
#include "DataRangeDlg.h"
#include "RadarViewView.h"

BEGIN_MESSAGE_MAP(DataRangeDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &DataRangeDlg::OnBnClickedOk)
END_MESSAGE_MAP()

/**************************************************************************
*  @Function Name :: DataRangeDlg
*  @Description   :: DataRangeDlg类的构造函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
DataRangeDlg::DataRangeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DATARANGE, pParent)
{
}

/**************************************************************************
*  @Function Name :: ~DataRangeDlg
*  @Description   :: DataRangeDlg类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
DataRangeDlg::~DataRangeDlg()
{
}

/**************************************************************************
*  @Function Name :: OnInitDialog
*  @Description   :: DataRangeDlg的初始化函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 返回是否初始化成功
**************************************************************************/
BOOL DataRangeDlg::OnInitDialog()
{
	CRadarViewView *pView = (CRadarViewView *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView();
	Points3DWnd * m_pPoints3DWnd = pView->Get3DWnd();
	double dataMin = 0.0;
	double dataMax = 0.0;
	m_pPoints3DWnd->GetValueRange(dataMin, dataMax);

	COLORCODING AttrIdx;
	m_pPoints3DWnd->GetColorCoding(AttrIdx);
	CString m_show;
	TCHAR tcharshow[1000];
	int iLength;
	iLength = MultiByteToWideChar(CP_ACP, 0, ColorCodingName[int(AttrIdx)], strlen(ColorCodingName[int(AttrIdx)]) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, ColorCodingName[int(AttrIdx)], strlen(ColorCodingName[int(AttrIdx)]) + 1, tcharshow, iLength);
	m_show.Format(_T("编码属性："));
	m_show += tcharshow;

	switch (AttrIdx)
	{//根据不同的属性显示不同的单位
	case CODING_ID:
		break;
	case CODING_DIS:
		m_show += _T("  单位(mm)");
		break;
	case CODING_X:
		m_show += _T("  单位(m)");
		break;
	case CODING_Y:
		m_show += _T("  单位(m)");
		break;
	case CODING_Z:
		m_show += _T("  单位(m)");
		break;
	case CODING_INTEN:
		break;
	case CODING_HANGLE:
		m_show += _T("  单位( °)");
		break;
	case CODING_VANGLE:
		break;
	case CODING_CHAN:
		break;
	case CODING_TIME:
		break;
	}
	GetDlgItem(IDC_TXT_CODINGATTRIBUTE)->SetWindowTextW(m_show);

	m_show.Format(_T("%0.2f"), dataMin);
	GetDlgItem(IDC_EDIT_MIN)->SetWindowTextW(m_show);
	m_show.Format(_T("%0.2f"), dataMax);
	GetDlgItem(IDC_EDIT_MAX)->SetWindowTextW(m_show);

	return TRUE;
}

/**************************************************************************
*  @Function Name :: OnBnClickedOk
*  @Description   :: 确定按钮的响应事件
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void DataRangeDlg::OnBnClickedOk()
{
	CRadarViewView *pView = (CRadarViewView *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView();
	Points3DWnd * m_pPoints3DWnd = pView->Get3DWnd();
	CString m_show;
	GetDlgItem(IDC_EDIT_MIN)->GetWindowTextW(m_show);
	double dataMin = _ttof(m_show);
	GetDlgItem(IDC_EDIT_MAX)->GetWindowTextW(m_show);
	double dataMax = _ttof(m_show);
	m_pPoints3DWnd->SetValueRange(dataMin, dataMax);
	//对标尺进行重新绘制
	pView->ReDrawLegend();
	CDialogEx::OnOK();
}
