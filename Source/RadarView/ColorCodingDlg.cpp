// ColorCodingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "ColorCodingDlg.h"
#include "afxdialogex.h"
#include "Resource.h"
#include "ColorTable.h"
#include "RadarViewView.h"

// ColorCodingDlg 对话框

IMPLEMENT_DYNAMIC(ColorCodingDlg, CDialogEx)

BEGIN_MESSAGE_MAP(ColorCodingDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &ColorCodingDlg::OnBnClickedOk)
END_MESSAGE_MAP()

/**************************************************************************
*  @Function Name :: ColorCodingDlg
*  @Description   :: ColorCodingDlg类的构造函数
*  @Author        :: zzw
*  @Parameters    :: pParent:父窗口
*
*  @Return        :: None
**************************************************************************/
ColorCodingDlg::ColorCodingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_COLORCODINGTABLE, pParent)
{
}

/**************************************************************************
*  @Function Name :: ~ColorCodingDlg
*  @Description   :: ColorCodingDlg类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
ColorCodingDlg::~ColorCodingDlg()
{
}

void ColorCodingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COLORLIST, m_ColorlistCtr);
}

/**************************************************************************
*  @Function Name :: OnInitDialog
*  @Description   :: 初始化框格
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 初始化是否成功
**************************************************************************/
BOOL ColorCodingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_ColorlistCtr.SetRowHeight(30);									//设置显示颜色中每一行的高度
	m_ColorlistCtr.InsertColumn(0, _T("颜色"), LVCFMT_CENTER, 190);		//设置第一列的一些属性
	m_ColorlistCtr.InsertColumn(1, _T("名称"), LVCFMT_CENTER, 170);		//设置第二列的一些属性

	for (int i = 0; i < COLOR_SCALE_COUNT; i++)
	{//显示colortable里面所有的颜色编码和字体							//将字体转换成宽字节，然后方便显示
		
		int length= MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ColorScaleName[i], -1, NULL, 0);
		wchar_t * pColorName = new wchar_t[length];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ColorScaleName[i], -1, pColorName, length);

		
		m_ColorlistCtr.InsertOneColor(i, ColorsTable[i], pColorName);	//插入第i个颜色数组和名称
		delete(pColorName);
	}

	//显示初始默认选中的颜色列表
	CRadarViewView *pView = (CRadarViewView *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView();
	Points3DWnd * m_pPoints3DWnd = pView->Get3DWnd();
	int idx = m_pPoints3DWnd->GetColorMode();
	if ((idx < COLOR_SCALE_COUNT) && (idx >= 0))
	{
		m_ColorlistCtr.SetItemState(idx, LVIS_SELECTED, LVIS_SELECTED);
	}
	return TRUE;
}

/**************************************************************************
*  @Function Name :: OnBnClickedOk
*  @Description   :: 点击确定之后重新显示某一种颜色编码
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void ColorCodingDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	int idx = m_ColorlistCtr.GetSelectionMark();
	CRadarViewView *pView = (CRadarViewView *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView();
	Points3DWnd * m_pPoints3DWnd = pView->Get3DWnd();
	if ((idx < COLOR_SCALE_COUNT) && (idx >= 0))
	{
		m_pPoints3DWnd->SetColorMode(idx);
		//对标尺进行重新绘制
		pView->ReDrawLegend();
		m_pPoints3DWnd->SetFocus();
		m_pPoints3DWnd->ReDraw();
	}
	CDialogEx::OnOK();
}
