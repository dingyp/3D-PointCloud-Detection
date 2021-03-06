#pragma once
#include <vector>

/*存储颜色的数组*/
typedef struct OneColorTable
{
	unsigned char ColorTable[256][3];
}OneColor;

typedef struct std::vector<OneColor> ColorList;//在列表里面的颜色存储

class CColorCtrl :public CListCtrl
{
public:
	CColorCtrl();
	virtual ~CColorCtrl();
	void SetRowHeight(int nHeight);																//设置行高
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);			//设置行高的响应处理函数
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);									//设置item的高
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);									//画出item的结构
	BOOL InsertOneColor(int nColorIdx, const unsigned char ColorTable[][4], LPCTSTR colorName);	//拷贝颜色列表和字
	BOOL GetColColor(int col, OneColor& onecolor);												//返回下标是col的颜色编码数组
	DECLARE_MESSAGE_MAP()
private:
	int m_nRowHeight;																			//行高
	ColorList colorlist;																		//颜色编码列表
};

// ColorCodingDlg 对话框
class ColorCodingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ColorCodingDlg)

public:
	ColorCodingDlg(CWnd* pParent = nullptr);													// 标准构造函数
	virtual ~ColorCodingDlg();
	BOOL OnInitDialog();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COLORCODINGTABLE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);											// DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

	CColorCtrl m_ColorlistCtr;																	//对颜色列表进行管理
public:
	afx_msg void OnBnClickedOk();																//点击确定之后重新显示某一种颜色编码
};

