
// RadarViewDoc.cpp: CRadarViewDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "RadarView.h"
#endif

#include "RadarViewDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRadarViewDoc
#include "MainFrm.h"
#include "RadarViewView.h"
#include "csvReader.h"
#include "ErrorManager\ErroCodeDefine.h"

using namespace DataReader;

IMPLEMENT_DYNCREATE(CRadarViewDoc, CDocument)

BEGIN_MESSAGE_MAP(CRadarViewDoc, CDocument)
	ON_COMMAND(ID_PCAPFILE_OPEN, &CRadarViewDoc::StartPcapReader)
	ON_COMMAND(ID_SENSOR_OPEN, &CRadarViewDoc::StartSensorReader)
	ON_COMMAND(ID_CSVFILE_OPEN, &CRadarViewDoc::csvReaderdata)//打开csv文件
	ON_COMMAND(ID_SAVEAS_PCAP, &CRadarViewDoc::SaveSensorData)
	ON_COMMAND(ID_SAVEAS_CSV, &CRadarViewDoc::SaveFrameDataAsCSV)//将当前帧信息保存为csv文件
	ON_COMMAND(OpenFile_BUTTON, &CRadarViewDoc::StartPcapReader)
	ON_COMMAND(OpenRadar_BUTTON, &CRadarViewDoc::StartSensorReader)
	ON_COMMAND(SAVE_BUTTON, &CRadarViewDoc::SaveSensorData)

	ON_COMMAND(ID_BIN_OPEN, &CRadarViewDoc::OnBinOpen)
END_MESSAGE_MAP()

/**************************************************************************
*  @Function Name :: CRadarViewDoc
*  @Description   :: CRadarViewDoc类的默认构造函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CRadarViewDoc::CRadarViewDoc()
{
	pDataReader = NULL;
	pRawProcessor = NULL;
	pFrameProcessor = NULL;
	myReader.Init(&myRawProcessor);//将处理类提示给reader类
	memset(&mySensor, 0, sizeof(DataDefinition::SensorAttribute));//置空里面的数据
	isMruFile = 0;
	m_bFrameSize = FALSE;
	FilterFrameNumber = 3;
	m_bIsFilter = false;
}

/**************************************************************************
*  @Function Name :: ~CRadarViewDoc
*  @Description   :: CRadarViewDoc类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
CRadarViewDoc::~CRadarViewDoc()
{
}

/**************************************************************************
*  @Function Name :: OnNewDocument
*  @Description   :: 创建Document类时调用的函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 返回是否创建成功
**************************************************************************/
BOOL CRadarViewDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
	{
		return FALSE;
	}
	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}

/**************************************************************************
*  @Function Name :: SetTitle
*  @Description   :: 设置窗口的标题
*  @Author        :: zzw
*  @Parameters    :: lpszTitle:窗口标题
*
*  @Return        :: None
**************************************************************************/
void CRadarViewDoc::SetTitle(LPCTSTR lpszTitle)
{
	//注意MainFrame中也请设置
#ifdef _WIN64
	CDocument::SetTitle(L"OLamViewer - 1.1 64-bit");
#else
	CDocument::SetTitle(L"OLamViewer - 1.1 32-bit");
#endif
}

/**************************************************************************
*  @Function Name :: OnCloseDocument
*  @Description   :: 关闭Document类之前的清理工作
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewDoc::OnCloseDocument()
{
	//先对线程进行退出
	Frame = NULL;
	if (NULL != pDataReader)
	{
		myReader.Stop();
		myReader.Destory();
	}
	if (NULL != pRawProcessor)
	{
		myRawProcessor.End_ClearFrame();
		myRawProcessor.Destory();
	}
	if (NULL != pFrameProcessor)
	{
		myFrameProcessor.Stop();
		myFrameProcessor.Destory_Trasfrom();
	}
	while (NULL != pDataReader) { Sleep(1); };
	while (NULL != pRawProcessor) { Sleep(1); };
	while (NULL != pFrameProcessor) { Sleep(1); };
	Sleep(50);
	CDocument::OnCloseDocument();
}

/**************************************************************************
*  @Function Name :: Serialize
*  @Description   :: CRadarViewDoc 序列化
*  @Author        :: zzw
*  @Parameters    :: ar:Serialize参数
*
*  @Return        :: None
**************************************************************************/
void CRadarViewDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CRadarViewDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CRadarViewDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CRadarViewDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CRadarViewDoc 诊断

#ifdef _DEBUG
void CRadarViewDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRadarViewDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/**************************************************************************
*  @Function Name :: csvReaderdata
*  @Description   :: csv读取事件
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewDoc::csvReaderdata()
{
	//首先其他Reader在没有关闭的情况下先进行关闭
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	int iReaderStatus = pMain->Get_ReaderStatus();
	if ((1 == iReaderStatus)||(2 == iReaderStatus))
	{//文件读则关闭文件//传感器读则关闭传感器
		if (NULL != pDataReader)
		{
			myReader.Stop();
			myReader.DisConnection();
		}
		while (NULL != pDataReader) { Sleep(0); };
		myRawProcessor.End_ClearFrame();
		myFrameProcessor.ReTrasform();

		pMain->m_wndStatusBar.SetPaneText(1, (LPCTSTR)(_T("文件提示")));//清除状态栏上的显示
		pMain->m_wndStatusBar.SetPaneText(2, (LPCTSTR)(_T("取点信息")));
		pMain->m_wndStatusBar.SetPaneText(4, (LPCTSTR)(_T("帧数")));
		pMain->m_wndStatusBar.SetPaneText(5, (LPCTSTR)(_T("00.00.00")));

		pMain->SetTotalFrameNumber(0);//设置sliderRange
		pMain->SetPos(0);

		//将测量模式设置成不测量模式
		int MeasureStatus = pMain->Get_Measure_Bitmap();
		if (MeasureStatus != 0)
		{
			CRadarViewView *pView = (CRadarViewView *)pMain->GetActiveView();
			pView->SetRularStatus(0);
		}

		CRadarViewView *pView = (CRadarViewView *)pMain->GetActiveView();
		pView->ClearFrameShow();//清除所有的点和线
		if (1 == iReaderStatus)
		{//对关闭文件做日志
			FY_DEBUG(_T("关闭文件Reader"));
		}
		else
		{//对关闭传感器做日志
			FY_DEBUG(_T("关闭传感Reader"));
		}
	}

	/******进行文件的保存******/
	CString File_path("");
	//打开图像文件的种类
	LPCTSTR szFilter = _T("CSV(*.csv)|*.csv||");
	CFileDialog dlgFileSaveFile(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);//里面TRUE代表打开文件对话框，FALSE代表保存文件对话框

	if (dlgFileSaveFile.DoModal() == IDOK)
	{//获取保存的文件名
		File_path = dlgFileSaveFile.GetPathName();
	}
	else
	{
		return;
	}

	m_bFrameSize = TRUE;//重新调整尺度大小
	//声明标识符
	USES_CONVERSION;
	//调用函数，T2A和W2A均支持ATL和MFC中的字符转换
	char * pFileName = T2A(File_path);
	//myReader.BuildSaverConnection(pFileName);

	CString shortFilePath;
	CRadarViewApp *pViewApp = (CRadarViewApp *)AfxGetApp();
	//pViewApp->m_systemConfigVarManager.Instance().AddMruFile(File_path);//加文件到ini里面
	pViewApp->m_systemConfigVarManager.Instance().GetShortFilePath(File_path, shortFilePath);

	CString show;
	show.Format(_T("显示CSV:%s"), shortFilePath);
	pMain->m_wndStatusBar.SetPaneText(1, (LPCTSTR)(show));

	//对CSV文件进行读取并显示
	myReader.OpenCSVFile(pFileName ,React_DataTsfer ,this);

	FY_DEBUG(_T("进行CSVReader"));
}

/**************************************************************************
*  @Function Name :: SaveFrameDataAsCSV
*  @Description   :: 将当前帧保存为csv文件
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewDoc::SaveFrameDataAsCSV()
{//获取当前帧并保存，保存csv文件
	//首先保存文件必须在暂停状态下，先进行判断
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	bool isStartStop = pMain->Get_STARTSTOP_Bitmap();
	if (!isStartStop)
	{//播放
		MessageBox(NULL, _T("保存CSV文件之前请先暂停！"), 0, 0);
		return;
	}

	int iReaderStatus = pMain->Get_ReaderStatus();
	CString DateTime;//保存的文件名
	if (1 == iReaderStatus)
	{//文件读则命名方式为filename+frameNumber
		char pcapFileName[1000];//获取filename
		if (RaView_NO_ERROR != myReader.Get_FileName(pcapFileName))
		{
			return;
		}
		//找出文件路径中得文件名
		unsigned long long frameNumber = myFrameProcessor.GetFileFrameIndex();//获取帧数
		CString pcapname(pcapFileName);
		CString shortPcapName;
		CRadarViewApp *pViewApp = (CRadarViewApp *)AfxGetApp();
		pViewApp->m_systemConfigVarManager.Instance().GetFileName(pcapname, shortPcapName);
		DateTime.Format(_T("%s (Frame %04d)"), shortPcapName, frameNumber);
	}
	else if(2 == iReaderStatus)
	{//传感器读则命名方式为%Y-%m-%d-%H-%M-%S_OLam_Data
		CTime nowtime;
		nowtime = CTime::GetCurrentTime();								//获取当前时间日期
		DateTime = nowtime.Format(_T("%Y-%m-%d-%H-%M-%S_OLam_Data"));	//格式化日期时间
	}

	/******进行文件的保存******/
	CString File_path("");
	//打开图像文件的种类
	LPCTSTR szFilter = _T("CSV(*.csv)|*.csv||");
	
	CFileDialog dlgFileSaveFile(FALSE, _T(".pcap"), DateTime, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);

	if (dlgFileSaveFile.DoModal() == IDOK)
	{//获取保存的文件名
		File_path = dlgFileSaveFile.GetPathName();
	}
	else
	{
		return;
	}

	//声明标识符
	USES_CONVERSION;
	//调用函数，T2A和W2A均支持ATL和MFC中的字符转换
	char * pFileName = T2A(File_path);

	//CString show;
	//show.Format(_T("正在保存CSV文件:%s"), File_path);
	//pMain->m_wndStatusBar.SetPaneText(1, (LPCTSTR)(show));

	//对CSV文件进行读取并显示
	RadarPointList PointList;
	CRadarViewView *pView = (CRadarViewView *)pMain->GetActiveView();
	if (FALSE == pView->GetPointsData(PointList))
	{//显示窗口没有点，进行退出
		return;
	}
	myReader.WriteCSVFile(pFileName, PointList);

	FY_DEBUG(_T("正在保存CSV文件"));
}

/**************************************************************************
*  @Function Name :: StartPcapReader
*  @Description   :: 开启文件读取线程
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewDoc::StartPcapReader()
{//还原所有场景
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	//bool bSensorStatus = pMain->Get_Sensor_Bitmap();
	int iReaderStatus = pMain->Get_ReaderStatus();
	if (2 == iReaderStatus)
	{//传感器是打开状态，则结束线程
		if (NULL != pDataReader)
		{
			myReader.Stop();
			myReader.DisConnection();
		}
		while (NULL != pDataReader) { Sleep(0); };
		//pMain->Set_Sensor_Bitmap(false);
		myRawProcessor.End_ClearFrame();
		myFrameProcessor.ReTrasform();
		CRadarViewView *pView = (CRadarViewView *)pMain->GetActiveView();
		pView->ClearFrameShow();//清除所有的点和线
		FY_DEBUG(_T("关闭传感Reader"));
	}
	else if (1 == iReaderStatus)
	{//文件则关
		if (NULL != pDataReader)
		{
			myReader.Stop();
			myReader.DisConnection();
		}
		while (NULL != pDataReader) { Sleep(0); };
		myRawProcessor.End_ClearFrame();
		myFrameProcessor.ReTrasform();
		CRadarViewView *pView = (CRadarViewView *)pMain->GetActiveView();
		pView->ClearFrameShow();//清除所有的点和线
		//将读取协议置空
		mySensor.sensorKind = LIDAR_INVALID;
		pMain->m_wndStatusBar.SetPaneText(1, (LPCTSTR)(_T("文件提示")));//清除状态栏上的显示
		pMain->m_wndStatusBar.SetPaneText(2, (LPCTSTR)(_T("取点信息")));
		pMain->m_wndStatusBar.SetPaneText(4, (LPCTSTR)(_T("帧数")));
		pMain->m_wndStatusBar.SetPaneText(5, (LPCTSTR)(_T("00.00.00")));

		pMain->SetTotalFrameNumber(0);//设置sliderRange
		pMain->SetPos(0);

		//将测量模式设置成不测量模式
		int MeasureStatus = pMain->Get_Measure_Bitmap();
		if (MeasureStatus != 0)
		{//如果没有在测量，就使之测量
			CRadarViewView *pView = (CRadarViewView *)pMain->GetActiveView();
			pView->SetRularStatus(0);
		}
		FY_DEBUG(_T("关闭文件Reader"));
		return;
	}

	//置更新尺寸，这样读取第一张的时候会根据尺寸大小进行显示
	m_bFrameSize = TRUE;

	FY_DEBUG(_T("打开文件Reader"));
	//如果线程还没有创建,接收帧和转换帧一起创建
	if (NULL == pFrameProcessor)
	{//先创建帧转换线程
		pFrameProcessor = AfxBeginThread((AFX_THREADPROC)DataTsfer, this);
		FY_DEBUG(_T("开启DataTsfer线程"));
	}
	if (NULL == pRawProcessor)
	{//再创建原始buff转换线程
		pRawProcessor= AfxBeginThread((AFX_THREADPROC)RawProcessor, this);
		FY_DEBUG(_T("开启RawProcessor线程"));
	}
	if (NULL == pDataReader) 
	{//最后创建文件读取线程
		pDataReader = AfxBeginThread((AFX_THREADPROC)PcapReader, this);
		FY_DEBUG(_T("开启PcapReader线程"));
	}
}

/**************************************************************************
*  @Function Name :: StartSensorReader
*  @Description   :: 开启Sensor读取线程的响应函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewDoc::StartSensorReader()
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CRadarViewView *pView = (CRadarViewView *)pMain->GetActiveView();
	//bool bSensorStatus = pMain->Get_Sensor_Bitmap();
	int iReaderStatus = pMain->Get_ReaderStatus();
	if (2 == iReaderStatus)
	{//表示SensorReader正打开状态，则结束线程
		if (NULL != pDataReader)
		{
			myReader.Stop();
			myReader.DisConnection();
		}
		while (NULL != pDataReader) { Sleep(0); };
		myRawProcessor.End_ClearFrame();
		myFrameProcessor.ReTrasform();

		//将传感器协议置空
		mySensor.sensorKind = LIDAR_INVALID;
		//更改状态栏上的显示
		pMain->m_wndStatusBar.SetPaneText(1, (LPCTSTR)(_T("文件提示")));
		pMain->m_wndStatusBar.SetPaneText(2, (LPCTSTR)(_T("取点信息")));
		pMain->m_wndStatusBar.SetPaneText(4, (LPCTSTR)(_T("帧数")));
		pMain->m_wndStatusBar.SetPaneText(5, (LPCTSTR)(_T("00.00.00")));

		pMain->SetTotalFrameNumber(0);//设置sliderRange
		pMain->SetPos(0);

		//将测量模式设置成不测量模式
		int MeasureStatus = pMain->Get_Measure_Bitmap();
		if (MeasureStatus != 0)
		{//如果没有在测量，就使之测量
			pView->SetRularStatus(0);
		}

		FY_DEBUG(_T("关闭传感Reader"));

		pView->ClearFrameShow();//清除所有的点和线
		return;
	}
	else if (1 == iReaderStatus)
	{//文件读取状态，先等待文件读取关闭
		if (NULL != pDataReader)
		{//文件读取状态，先等待文件读取关闭
			myReader.Stop();
			myReader.DisConnection();
		}
		while (NULL != pDataReader) { Sleep(0); };
		myRawProcessor.End_ClearFrame();
		myFrameProcessor.ReTrasform();
		pView->ClearFrameShow();//清除所有的点和线
		FY_DEBUG(_T("关闭文件Reader"));
	}

	//先对传感器连接参数属性进行设置
	if (!pView->SetSensorConAttribute())
	{//cancel
		return;
	}

	//更改状态栏上的显示
	pMain->m_wndStatusBar.SetPaneText(1, (LPCTSTR)(_T("传感器正在显示")));
	pMain->m_wndStatusBar.SetPaneText(2, (LPCTSTR)(_T("取点信息")));
	pMain->m_wndStatusBar.SetPaneText(4, (LPCTSTR)(_T("帧数")));
	pMain->m_wndStatusBar.SetPaneText(5, (LPCTSTR)(_T("00.00.00")));

	//置更新尺寸，这样读取第一张的时候会根据尺寸大小进行显示
	m_bFrameSize = TRUE;

	FY_DEBUG(_T("打开传感Reader"));
	if (NULL == pFrameProcessor)
	{//先创建转换线程
		pFrameProcessor = AfxBeginThread((AFX_THREADPROC)DataTsfer, this);
		FY_DEBUG(_T("开启DataTsfer线程"));
	}
	if (NULL == pRawProcessor)
	{//再创建原始buff转换线程
		pRawProcessor = AfxBeginThread((AFX_THREADPROC)RawProcessor, this);
		FY_DEBUG(_T("开启RawProcessor线程"));
	}
	if (NULL == pDataReader)
	{//再创建网络数据读取线程
		pDataReader = AfxBeginThread((AFX_THREADPROC)SensorReader, this);
		FY_DEBUG(_T("开启SensorReader线程"));
	}

	//将测量模式设置成不测量模式
	int MeasureStatus = pMain->Get_Measure_Bitmap();
	if (MeasureStatus != 0)
	{//如果没有在不测量，就使之不测量
		CRadarViewView *pView = (CRadarViewView *)pMain->GetActiveView();
		pView->SetRularStatus(0);
	}
}

/**************************************************************************
*  @Function Name :: SaveReaderFull
*  @Description   :: 保存文件太满的返回函数，之后关闭保存线程
*  @Author        :: zzw
*  @Parameters    :: pParam:CRadarViewDoc指针，因为这是全局函数
*
*  @Return        :: None
**************************************************************************/
void CRadarViewDoc::SaveReaderFull(LPVOID pParam)
{
	CRadarViewDoc *pDoc = (CRadarViewDoc *)pParam;
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	//设置主窗口文件保存按钮为关闭
	pMain->Set_FileSave_Bitmap(false);
	pDoc->myReader.DisSaverConnection();//结束保存文件的线程
	//发送消息给状态栏，让状态栏改变
	HWND hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView()->GetSafeHwnd();
	CString shortFilePath=_T("传感器正在显示");
	::SendMessage(hwnd, MY_MSG_ChangStatusBar, 1, (LPARAM)LPCTSTR(shortFilePath));//对状态栏的第二格进行修改
	//pMain->m_wndStatusBar.SetPaneText(1, (LPCTSTR)(_T("传感器正在显示")));
}

/**************************************************************************
*  @Function Name :: SaveSensorData
*  @Description   :: 启动保存文件函数设置，在传感器时直接保存为pcap文件
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CRadarViewDoc::SaveSensorData()
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	bool bfilesaveStatus = pMain->Get_FileSave_Bitmap();
	if (bfilesaveStatus)
	{//文件写已经打开，现在关闭
		myReader.DisSaverConnection();//结束保存文件的线程
		pMain->Set_FileSave_Bitmap(!bfilesaveStatus);
		pMain->m_wndStatusBar.SetPaneText(1, (LPCTSTR)(_T("传感器正在显示")));
		FY_DEBUG(_T("关闭SaveReader"));
	}
	else
	{
		if (SENSOR_READER != myReader.GetReaderMode())
		{
			MessageBox(NULL, _T("请先打开传感器！"), _T("提示"), 0);
			return;
		}
		/******进行文件的保存******/
		CString File_path("");
		//打开图像文件的种类
		LPCTSTR szFilter = _T("PCAP(*.pcap)|*.pcap|ALLSUPORTFILE(*.*)|*.*||");

		CTime nowtime;
		nowtime = CTime::GetCurrentTime();		//获取当前时间日期
		CString DateTime_filename;
		CString filesuffix = _T(".pcap");
		if (LIDAR_HLP16 == mySensor.sensorKind)
		{
			DateTime_filename = nowtime.Format(_T("%Y-%m-%d-%H-%M-%S_OLam_HLP16_Data"));	//格式化日期时间
		}
		else if (LIDAR_2D== mySensor.sensorKind)
		{
			szFilter = _T("PCAPNG(*.pcapng)|*.pcapng|ALLSUPORTFILE(*.*)|*.*||");
			DateTime_filename = nowtime.Format(_T("%Y-%m-%d-%H-%M-%S_OLam_2D_Data"));	//格式化日期时间
			filesuffix = _T(".pcapng");
		}

		CFileDialog dlgFileSaveFile(FALSE, filesuffix, DateTime_filename, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);

		if (dlgFileSaveFile.DoModal() == IDOK)
		{//获取保存的文件名
			File_path = dlgFileSaveFile.GetPathName();
		}
		else
		{
			return;
		}

		//声明标识符
		USES_CONVERSION;
		//调用函数，T2A和W2A均支持ATL和MFC中的字符转换
		char * pFileName = T2A(File_path);
		myReader.BuildSaverConnection(pFileName, SaveReaderFull,this);//这里面会开启一个线程实时进行保存
		pMain->Set_FileSave_Bitmap(!bfilesaveStatus);


		CString shortFilePath;
		CRadarViewApp *pViewApp = (CRadarViewApp *)AfxGetApp();
		pViewApp->m_systemConfigVarManager.Instance().GetShortFilePath(File_path, shortFilePath);

		CString show;
		show.Format(_T("实时保存:%s"), shortFilePath);
		pMain->m_wndStatusBar.SetPaneText(1, (LPCTSTR)(show));
		FY_DEBUG(_T("打开SaveReader"));
	}
}

/**************************************************************************
*  @Function Name :: OnAlReadyReadFilePath
*  @Description   :: 已经设置路径成功
*  @Author        :: zzw
*  @Parameters    :: wParam:是否读取成功标志位
*					 lParam:暂无
*  @Return        :: None
**************************************************************************/
LRESULT CRadarViewDoc::OnAlReadyReadFilePath(WPARAM wParam, LPARAM lParam)
{
	int info = int(wParam);
	if (info == 0)
	{//读取成功
		//MessageBox(NULL, pcapFile_path, 0, 0);
		//pcapFile_path = *((CString*)lParam);
		isMruFile = 2;
	}
	else
	{//没有读取成功
		isMruFile = 3;
	}
	return 0;
}

/**************************************************************************
*  @Function Name :: PcapReader
*  @Description   :: 打开pcap文件线程
*  @Author        :: zzw
*  @Parameters    :: pParam:CRadarViewDoc类指针
*
*  @Return        :: None
**************************************************************************/
void CRadarViewDoc::PcapReader(LPVOID pParam)
{
	CRadarViewDoc *pDoc = (CRadarViewDoc *)pParam;
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;

	if (pDoc->isMruFile!=1)
	{//获取文件路径
		HWND hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView()->GetSafeHwnd();
		::SendMessage(hwnd, MY_MSG_FileReader, 0, (LPARAM)pParam);

		while (pDoc->isMruFile == 0)
		{//此时还是没有有效文件
			//MessageBox(NULL,_T("还是没有"),0,0);
		}
		if (pDoc->isMruFile == 3)
		{//获取失败
			pDoc->pDataReader = NULL;
		 	pMain->Set_STARTSTOP_Bitmap(true);
			pMain->Set_ReaderStatus(0);//代表没有Reader
			pDoc->isMruFile = 0;
			return;
		}
		pDoc->isMruFile = 0;

	}
	else
	{//直接已经有了路径了
		pDoc->isMruFile = 0;//恢复默认选择路径打开
	}
	FY_DEBUG(_T("文件路径成功获取。"));


	//打开的文件保存到ini里面的最近打开
	CRadarViewApp *pViewApp = (CRadarViewApp *)AfxGetApp();
	pViewApp->m_systemConfigVarManager.Instance().AddMruFile(pDoc->File_path);

	//将打开文件路径显示到状态栏里面
	HWND hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView()->GetSafeHwnd();
	//在显示之前，将路径进行智能缩短
	CString shortFilePath;
	pViewApp->m_systemConfigVarManager.Instance().GetShortFilePath(pDoc->File_path, shortFilePath);
	::SendMessage(hwnd, MY_MSG_ChangStatusBar, 1, (LPARAM)LPCTSTR(shortFilePath));//对状态栏的第二格进行修改

	//先得到文件的类型，然后分别设置默认的协议
	CString suffixName;
	pViewApp->m_systemConfigVarManager.Instance().GetFileSuffixName(suffixName, pDoc->File_path);

	if (suffixName == "pcap")
	{
		pDoc->mySensor.sensorKind = LIDAR_HLP16;
	}
	else if (suffixName == "pcapng")
	{
		pDoc->mySensor.sensorKind = LIDAR_2D;
	}

	CRadarViewView *pView = (CRadarViewView *)pMain->GetActiveView();
	pView->SetSensorAttribute(1);//参数1表示可以选中协议
	
	//声明标识符
	USES_CONVERSION;
	//调用函数，T2A和W2A均支持ATL和MFC中的字符转换
	char *pFileName = T2A(pDoc->File_path);
	
	if (RaView_NO_ERROR!= pDoc->myReader.BuildFileConnection(pFileName, pMain->Set_PlayStop_Bitmap, pMain, pDoc->mySensor.sensorKind))//进行连接的参数设置,读取完毕的回调函数
	{//初始化有错误
		//这里应该跳出提示信息
		pDoc->pDataReader = NULL;
		pMain->Set_STARTSTOP_Bitmap(true);
		pMain->Set_ReaderStatus(0);//代表没有Reader
		MessageBox(hwnd, _T("文件没有打开成功，请查看文件协议是否对应！"), 0, 0);
		return;
	}
	FY_DEBUG(_T("File连接参数设置成功。"));
	//////////////////total_FrameNumber生成不知道需不需要时间，这个地方之后需要注意///////////////////////////////
	//这个时候应是测量模式，因为文件读取一开始便是暂停
	int MeasureStatus = pMain->Get_Measure_Bitmap();
	if (MeasureStatus == 0)
	{//如果没有在测量，就使之测量
		pView->SetRularStatus(1);
	}
	//此时读取总帧数，然后进行设置
	unsigned int total_frameNumber = pDoc->myReader.GetFrameNumber();
	if (total_frameNumber > 0)
	{
		pMain->SetTotalFrameNumber(total_frameNumber);
	}

	pDoc->myReader.Stop();
	//按钮置停
	pMain->Set_STARTSTOP_Bitmap(true);
	pDoc->myReader.Stop();
	//在这之前将之前可能有的显示都清空
	pDoc->myRawProcessor.End_ClearFrame();
	pDoc->myFrameProcessor.ReTrasform();
	pDoc->myFrameProcessor.UnSetProtocol();//取消协议设置
	pDoc->myFrameProcessor.SetProtocol(pDoc->mySensor.sensorKind, pDoc->m_bIsFilter, pDoc->FilterFrameNumber);
	pDoc->myRawProcessor.UnSetProtocol();//取消协议设置
	pDoc->myRawProcessor.SetProtocol(pDoc->mySensor.sensorKind);
	//将工具栏中的编码属性恢复成intensity
	pMain->SetAttriComBoxCurSel(5,false);
	pMain->Set_ReaderStatus(1);//代表FileReader
	FY_DEBUG(_T("启动ReaderStart"));
	pDoc->myReader.Start();//Start一开始，内部便不停地做循环操作
	FY_DEBUG(_T("结束ReaderStart"));
	pDoc->pDataReader = NULL;
	pMain->Set_STARTSTOP_Bitmap(true);
	pMain->Set_LOOP_Bitmap(true);
	pMain->Set_ReaderStatus(0);//代表没有Reader
	FY_DEBUG(_T("关闭PcapReader线程"));
	return;
}

/**************************************************************************
*  @Function Name :: SensorReader
*  @Description   :: 打开sensor读取线程
*  @Author        :: zzw
*  @Parameters    :: pParam:CRadarViewDoc类指针
*
*  @Return        :: None
**************************************************************************/
void CRadarViewDoc::SensorReader(LPVOID pParam)
{
	CRadarViewDoc *pDoc = (CRadarViewDoc *)pParam;
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	//先跳出参数框进行参数设置
	if (pDoc->myReader.BuildSensorConnection(pDoc->mySensor.ip, pDoc->mySensor.sensorPort,\
		pDoc->mySensor.localPort, pDoc->mySensor.sensorKind, pDoc->mySensor.mask))
	{//根据网络的参数，没有能够连接上网络，那么后面的事情就直接退出
		MessageBox(NULL, _T("传感器没有打开成功！"), 0, 0);
		pDoc->pDataReader = NULL;
		pMain->Set_STARTSTOP_Bitmap(true);
		pMain->Set_LOOP_Bitmap(true);
		//pMain->Set_Sensor_Bitmap(false);
		pMain->Set_ReaderStatus(0);
		return;
	}
	pDoc->myFrameProcessor.UnSetProtocol();//取消协议设置
	pDoc->myFrameProcessor.SetProtocol(pDoc->mySensor.sensorKind, pDoc->m_bIsFilter, pDoc->FilterFrameNumber);//重新进行协议设置
	pDoc->myRawProcessor.UnSetProtocol();//取消协议设置
	pDoc->myRawProcessor.SetProtocol(pDoc->mySensor.sensorKind);

	pMain->Set_STARTSTOP_Bitmap(false);
	//pMain->Set_Sensor_Bitmap(true);
	pMain->SetAttriComBoxCurSel(5,false);
	pMain->Set_ReaderStatus(2);
	int result = pDoc->myReader.Start();//根据返回值判断是否是正常退出，进而是否需要进行框格提示
	pDoc->pDataReader = NULL;//这个感觉可以使用waitforSingleObject对线程句柄进行置空
	pMain->Set_STARTSTOP_Bitmap(true);
	pMain->Set_LOOP_Bitmap(true);
	//pMain->Set_Sensor_Bitmap(false);
	pMain->Set_ReaderStatus(0);
	pMain->Set_FileSave_Bitmap(false);//传感器关闭的时候，文件保存按钮也就关掉了

	switch (result)
	{
	case RaView_NO_ERROR://没有错误
		break;
	case ER_UnSureOfProtocol://选取协议不对
		MessageBox(NULL, _T("请查看协议是否选取正确！"), 0, 0);
		break;
	case ER_OutTimeOfConnect://传感器超时好久没有收到信息
		MessageBox(NULL, _T("传感器连接已经断开！"), 0, 0);
		break;
	case ER_pBuffNotCreated://内存不够
		MessageBox(NULL, _T("内存不够请检查！"), 0, 0);
		break;
	}

	return;
}

/**************************************************************************
*  @Function Name :: RawProcessor
*  @Description   :: 等待接收原始buff线程
*  @Author        :: zzw
*  @Parameters    :: pParam:CRadarViewDoc类指针
*
*  @Return        :: None
**************************************************************************/
void CRadarViewDoc::RawProcessor(LPVOID pParam)
{
	CRadarViewDoc *pDoc = (CRadarViewDoc *)pParam;
	pDoc->myRawProcessor.SetProtocol(pDoc->mySensor.sensorKind);
	pDoc->myRawProcessor.BindFullFrameCallbackFunc(React_OneFrame, pParam);//进行回调函数和相关对象注册
	pDoc->myRawProcessor.Start();
	pDoc->myRawProcessor.UnSetProtocol();//取消协议设置
	pDoc->pRawProcessor = NULL;
	FY_DEBUG(_T("关闭RawProcessor线程"));
	return;
}

/**************************************************************************
*  @Function Name :: DataTsfer
*  @Description   :: 一直都在转换帧线程
*  @Author        :: zzw
*  @Parameters    :: pParam:CRadarViewDoc类指针
*
*  @Return        :: None
**************************************************************************/
void CRadarViewDoc::DataTsfer(LPVOID pParam)
{
	CRadarViewDoc *pDoc = (CRadarViewDoc *)pParam;
	pDoc->myFrameProcessor.SetProtocol(pDoc->mySensor.sensorKind, pDoc->m_bIsFilter, pDoc->FilterFrameNumber);//LIDAR_INVALID
	pDoc->myFrameProcessor.BindTrasferOneFrameFunc(React_DataTsfer, pParam);//进行回调函数和相关对象注册
	pDoc->myFrameProcessor.Start();
	pDoc->myFrameProcessor.UnSetProtocol();//取消协议设置
	pDoc->pFrameProcessor = NULL;
	FY_DEBUG(_T("关闭DataTsfer线程"));
	return;
}

/**************************************************************************
*  @Function Name :: React_OneFrame
*  @Description   :: 满原始一帧的回调函数
*  @Author        :: zzw
*  @Parameters    :: lParam:一帧原始数据帧的智能指针
*					 pParam:CRadarViewDoc类指针
*  @Return        :: None
**************************************************************************/
void CRadarViewDoc::React_OneFrame(std::shared_ptr<RawFrame> &lParam, LPVOID pParam)
{
	std::shared_ptr<RawFrame> pRawFrame = lParam;
	CRadarViewDoc *pDoc = (CRadarViewDoc *)pParam;
	int restFrameNumber= pDoc->myFrameProcessor.AddFrame(pRawFrame);
	pRawFrame = nullptr;
}

/**************************************************************************
*  @Function Name :: React_DataTsfer
*  @Description   :: 数据转换好的回调函数
*  @Author        :: zzw
*  @Parameters    :: lParam:一帧坐标帧的智能指针
*					 pParam:CRadarViewDoc类指针
*  @Return        :: None
**************************************************************************/
void CRadarViewDoc::React_DataTsfer(std::shared_ptr<RadarPointList> &lParam, LPVOID pParam)
{//此处回调函数发送消息的原因是使回调函数可以尽快返回，从而减少生成帧那边的等待时间，优化了系统
	CRadarViewDoc *pDoc = (CRadarViewDoc *)pParam;
	pDoc->Frame = lParam;
	HWND hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView()->GetSafeHwnd();
	if (FALSE == pDoc->m_bFrameSize)
	{
		SendMessage(hwnd, MY_MSG_SensorReader, 0, (LPARAM)&pDoc->Frame);//0代表不需要进行尺度判断
	}
	else if (TRUE == pDoc->m_bFrameSize)
	{
		SendMessage(hwnd, MY_MSG_SensorReader, 1, (LPARAM)&pDoc->Frame);//1代表需要尺度判断
		pDoc->m_bFrameSize = FALSE;
	}
}

void CRadarViewDoc::OnBinOpen()
{
	
		//首先其他Reader在没有关闭的情况下先进行关闭
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	int iReaderStatus = pMain->Get_ReaderStatus();
	if ((1 == iReaderStatus) || (2 == iReaderStatus))
	{//文件读则关闭文件//传感器读则关闭传感器
		if (NULL != pDataReader)
		{
			myReader.Stop();
			myReader.DisConnection();
		}
		while (NULL != pDataReader) { Sleep(0); };
		myRawProcessor.End_ClearFrame();
		myFrameProcessor.ReTrasform();

		pMain->m_wndStatusBar.SetPaneText(1, (LPCTSTR)(_T("文件提示")));//清除状态栏上的显示
		pMain->m_wndStatusBar.SetPaneText(2, (LPCTSTR)(_T("取点信息")));
		pMain->m_wndStatusBar.SetPaneText(4, (LPCTSTR)(_T("帧数")));
		pMain->m_wndStatusBar.SetPaneText(5, (LPCTSTR)(_T("00.00.00")));

		pMain->SetTotalFrameNumber(0);//设置sliderRange
		pMain->SetPos(0);

		//将测量模式设置成不测量模式
		int MeasureStatus = pMain->Get_Measure_Bitmap();
		if (MeasureStatus != 0)
		{
			CRadarViewView *pView = (CRadarViewView *)pMain->GetActiveView();
			pView->SetRularStatus(0);
		}

		CRadarViewView *pView = (CRadarViewView *)pMain->GetActiveView();
		pView->ClearFrameShow();//清除所有的点和线
		if (1 == iReaderStatus)
		{//对关闭文件做日志
			FY_DEBUG(_T("关闭文件Reader"));
		}
		else
		{//对关闭传感器做日志
			FY_DEBUG(_T("关闭传感Reader"));
		}
	}

	/******进行文件的保存******/
	CString File_path("");
	//打开图像文件的种类
	LPCTSTR szFilter = _T("BIN(*.bin)|*.bin||");
	CFileDialog dlgFileSaveFile(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);//里面TRUE代表打开文件对话框，FALSE代表保存文件对话框

	if (dlgFileSaveFile.DoModal() == IDOK)
	{//获取保存的文件名
		File_path = dlgFileSaveFile.GetPathName();
	}
	else
	{
		return;
	}

	m_bFrameSize = TRUE;//重新调整尺度大小
	//声明标识符
	USES_CONVERSION;
	//调用函数，T2A和W2A均支持ATL和MFC中的字符转换
	char * pFileName = T2A(File_path);

	CString shortFilePath;
	CRadarViewApp *pViewApp = (CRadarViewApp *)AfxGetApp();
	//pViewApp->m_systemConfigVarManager.Instance().AddMruFile(File_path);//加文件到ini里面
	pViewApp->m_systemConfigVarManager.Instance().GetShortFilePath(File_path, shortFilePath);

	CString show;
	show.Format(_T("显示BIN:%s"), shortFilePath);
	pMain->m_wndStatusBar.SetPaneText(1, (LPCTSTR)(show));
	//对CSV文件进行读取并显示
	myReader.OpenBinFile(pFileName, React_DataTsfer, this);

	FY_DEBUG(_T("进行BINReader"));

}
