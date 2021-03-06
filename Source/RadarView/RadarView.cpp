
// RadarView.cpp: 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "RadarView.h"
#include "MainFrm.h"

#include "RadarViewDoc.h"
#include "RadarViewView.h"

#include "SysPathManager.h"
#include "PathStringFuntion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRadarViewApp
BEGIN_MESSAGE_MAP(CRadarViewApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CRadarViewApp::OnAppAbout)
END_MESSAGE_MAP()

/**************************************************************************
*  @Function Name :: CRadarViewApp
*  @Description   :: CRadarViewApp类的构造函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CRadarViewApp::CRadarViewApp()
{
	
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则: 
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("RadarView.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的 CRadarViewApp 对象

CRadarViewApp theApp;

/**************************************************************************
*  @Function Name :: InitInstance
*  @Description   :: CRadarViewApp初始化
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 返回是否初始化成功
**************************************************************************/
BOOL CRadarViewApp::InitInstance()
{
	//::CreateThread(NULL, 0, ShowInit, this, 0, 0);
	//AfxBeginThread((AFX_THREADPROC)ShowInit, this);
	ShowInit(this);

	SetDefaultFileAppender();
	//FY_DEBUG(_T("CommonScanAnalysisProcess::Start()"));

	CString strSysConfigPath = fsutility::CSysPathManager::Instance().GetExePath() + _T("config");//文件要保存的目录
	CString strSysConfigFile = fsutility::CSysPathManager::Instance().GetExePath() + _T("config\\setting.ini");
	fsutility::CPathStringFunction::CheckAndCreatePath(strSysConfigPath);
	m_systemConfigVarManager.Instance().LoadConfig(strSysConfigFile);//包含不存在创建

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要 AfxInitRichEdit2()
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	LoadStdProfileSettings();  // 加载标准 INI 文件选项(包括 MRU)

	//g_pRecentFileList = m_pRecentFileList;
	// 注册应用程序的文档模板。  文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CRadarViewDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CRadarViewView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	
	return TRUE;
}

/**************************************************************************
*  @Function Name :: SetDefaultFileAppender
*  @Description   :: 设置默认的参数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewApp::SetDefaultFileAppender()
{
	SetCurrentDirectory(fsutility::CSysPathManager::Instance().GetExePath());
	std::locale::global(std::locale("chs"));
	CString strLogConfigPath;
	strLogConfigPath = fsutility::CSysPathManager::Instance().GetConfigPath() + _T("ConfigLog_Main.txt");
	PropertyConfigurator::doConfigure(strLogConfigPath.GetBuffer(0));
	//FY_DEBUG(_T("start Main Module......"));
	FY_DEBUG(_T("主模块启动......"));
}

/**************************************************************************
*  @Function Name :: ExitInstance
*  @Description   :: 退出实例化
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
int CRadarViewApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

/**************************************************************************
*  @Function Name :: ShowInit
*  @Description   :: 初始化的log展示
*  @Author        :: zzw
*  @Parameters    :: pParam:程序App指针
*
*  @Return        :: None
**************************************************************************/
void CRadarViewApp::ShowInit(LPVOID pParam)
{
	CRadarViewApp * myApp = (CRadarViewApp *)pParam;
	myApp->InitFun();
	//SetFocus(AfxGetApp()->m_pMainWnd->GetSafeHwnd());//将焦点聚回来
}

/**************************************************************************
*  @Function Name :: InitFun
*  @Description   :: 初始化CInitSystem函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewApp::InitFun()
{
	CInitSystem dlgInitSystem;
	dlgInitSystem.DoModal();
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	SetFocus(pMain->GetSafeHwnd());//将焦点聚回来
}

// CRadarViewApp 消息处理程序
BEGIN_MESSAGE_MAP(CInitSystem, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/**************************************************************************
*  @Function Name :: CInitSystem
*  @Description   :: CInitSystem类的构造函数
*  @Author        :: zzw
*  @Parameters    :: pParent:父窗口指针，默认置空
*
*  @Return        :: None
**************************************************************************/
CInitSystem::CInitSystem(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INITSYSTEM, pParent)
{
}

/**************************************************************************
*  @Function Name :: OnInitDialog
*  @Description   :: 初始化窗口函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 返回是否初始化成功
**************************************************************************/
BOOL CInitSystem::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetTimer(IDTIMER1, 1000, NULL);
	return TRUE;
}

/**************************************************************************
*  @Function Name :: OnTimer
*  @Description   :: 设置显示的定时器
*  @Author        :: zzw
*  @Parameters    :: nIDEvent:定时器编号
*
*  @Return        :: None
**************************************************************************/
void CInitSystem::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == IDTIMER1)
	{
		KillTimer(IDTIMER1);
		CDialogEx::OnOK();
	}
	CDialogEx::OnTimer(nIDEvent);
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CRadarViewApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CRadarViewApp 消息处理程序
