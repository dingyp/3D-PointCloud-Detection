// RaderSet.cpp: 实现文件
//

#include "stdafx.h"
#include "RadarView.h"
#include "RaderSet.h"
#include "afxdialogex.h"


// RaderSet 对话框

IMPLEMENT_DYNAMIC(RaderSet, CDialogEx)

RaderSet::RaderSet(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RADERSET, pParent)
{

}

RaderSet::~RaderSet()
{
}

void RaderSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(RaderSet, CDialogEx)

END_MESSAGE_MAP()


// RaderSet 消息处理程序
