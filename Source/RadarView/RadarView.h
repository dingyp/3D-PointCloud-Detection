
// RadarView.h: RadarView 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号

#include "SystemConfigVarManager.h"

// CRadarViewApp:
// 有关此类的实现，请参阅 RadarView.cpp
//
//#define WM_MSG_ReturnFocus  WM_USER+1020

class CInitSystem :public CDialogEx
{
public:
	CInitSystem(CWnd* pParent = nullptr);   // 标准构造函数
	BOOL OnInitDialog();					//初始化窗口函数
protected:
	DECLARE_MESSAGE_MAP()
	void OnTimer(UINT_PTR nIDEvent);		//设置显示的定时器
};

class CRadarViewApp : public CWinApp
{
public:
	CRadarViewApp();

	SystemConfigVarManager m_systemConfigVarManager;
// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void SetDefaultFileAppender();

// 实现
	afx_msg void OnAppAbout();
	//afx_msg LRESULT ResetFocus(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	static void ShowInit(LPVOID pParam);
	void InitFun();
	//CInitSystem dlgInitSystem;
};

extern CRadarViewApp theApp;

