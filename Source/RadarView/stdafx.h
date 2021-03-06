
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>





#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif



#ifdef _DEBUG
#pragma comment(lib, "log4cplusD.lib")
#else
#pragma comment(lib, "log4cplus.lib")
#endif

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/FileAppender.h>
#include <log4cplus/Layout.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/loglog.h>
#include <afxcontrolbars.h>
#include <afxwin.h>

using namespace log4cplus;
//#define Scanning_Service_Merger

//如果打开该选项，表示关闭调试输出
//#define DISABLE_DEBUGLOG
#ifndef FY_DEBUGLOG
#ifdef DISABLE_DEBUGLOG
#define FY_DEBUGLOG(logfunction, logevent) {}
#else 
#define FY_DEBUGLOG(logfunction, logevent) {\
	log4cplus::Logger _logger = log4cplus::Logger::getInstance(_T("OLamViewer")); \
	logfunction(_logger, logevent);  };
#endif

#define FY_DEBUG(x) FY_DEBUGLOG(LOG4CPLUS_DEBUG, x)
#define FY_TRACE(x) FY_DEBUGLOG(LOG4CPLUS_TRACE, x)
#define FY_ERROR(x) FY_DEBUGLOG(LOG4CPLUS_ERROR, x)
#define FY_FATAL(x) FY_DEBUGLOG(LOG4CPLUS_FATAL, x)

#endif
