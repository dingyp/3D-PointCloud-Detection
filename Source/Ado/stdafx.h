// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE ��
#include <afxodlgs.h>       // MFC OLE �Ի�����
#include <afxdisp.h>        // MFC �Զ�����
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC ���ݿ���
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO ���ݿ���
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT


#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#ifdef _DEBUG
#pragma comment(lib, "log4cplusD.lib")
#else
#pragma comment(lib, "log4cplus.lib")
#endif
using namespace log4cplus;

//#define DISABLE_DEBUGLOG
#ifndef FY_DEBUGLOG
#ifdef DISABLE_DEBUGLOG
#define FS_DEBUGLOG(logfunction, logevent) {}
#else 
#define FY_DEBUGLOG(logfunction, logevent) {\
	log4cplus::Logger _logger = log4cplus::Logger::getInstance(_T("ADO")); \
	logfunction(_logger, logevent);  };
#endif

#define ADOLOG_DEBUG(x) FY_DEBUGLOG(LOG4CPLUS_DEBUG, x)
#define ADOLOG_TRACE(x) FY_DEBUGLOG(LOG4CPLUS_TRACE, x)
#define ADOLOG_ERROR(x) FY_DEBUGLOG(LOG4CPLUS_ERROR, x)
#define ADOLOG_FATAL(x) FY_DEBUGLOG(LOG4CPLUS_FATAL, x)

#endif



