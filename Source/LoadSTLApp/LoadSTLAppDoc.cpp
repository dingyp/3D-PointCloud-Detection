
// LoadSTLAppDoc.cpp: CLoadSTLAppDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "LoadSTLApp.h"
#endif

#include "LoadSTLAppDoc.h"

#include <propkey.h>

#include "MainFrm.h"
#include "LoadSTLAppView.h"
#include "PcapFileSet.h"
#include "ErroCodeDefine.h"
#include "StlReader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLoadSTLAppDoc

IMPLEMENT_DYNCREATE(CLoadSTLAppDoc, CDocument)

BEGIN_MESSAGE_MAP(CLoadSTLAppDoc, CDocument)
	ON_COMMAND(ID_PCAP_NGOPEN, &CLoadSTLAppDoc::StartPcapReader)//打开pcap(ng)文件
	ON_COMMAND(ID_STLOPEN, &CLoadSTLAppDoc::OnOpenStlfile)//打开stl文件
	ON_COMMAND(ID_OBJOPEN, &CLoadSTLAppDoc::OnOpenObjfile)//打开obj文件

	ON_COMMAND(ID_OPEN_PCAPFILE, &CLoadSTLAppDoc::StartPcapReader)
	ON_COMMAND(ID_OPEN_STLFILE, &CLoadSTLAppDoc::OnOpenStlfile)
	ON_COMMAND(ID_OPEN_OBJFILE, &CLoadSTLAppDoc::OnOpenObjfile)
	ON_COMMAND(ID_BEGIN_CALCULATION, &CLoadSTLAppDoc::OnBeginCalculation)
	ON_COMMAND(ID_SHOWDATA, &CLoadSTLAppDoc::OnShowDataGrid)
	ON_COMMAND(ID_SAVEDATA, &CLoadSTLAppDoc::OnSaveData)
END_MESSAGE_MAP()

// CLoadSTLAppDoc 构造/析构

CLoadSTLAppDoc::CLoadSTLAppDoc()
{
	// TODO: 在此添加一次性构造代码
	pDataReader = NULL;
	pRawProcessor = NULL;
	pFrameProcessor = NULL;
	pcapReader.Init(&pcapRawProcessor);//将处理类提示给reader类
	memset(&mySensor, 0, sizeof(SensorAttribute));//置空里面的数据
	m_RoomModelList.ClearAllModels();
	FilterFrameNumber = 3;
	m_bIsFilter = true;//一开始就进行滤波
	m_pInterSection.clear();

	m_parvector = NULL;
	//m_datashowDlg = NULL;
}

CLoadSTLAppDoc::~CLoadSTLAppDoc()
{
	//if (NULL != m_datashowDlg)
	//{
	//	m_datashowDlg->DestroyWindow();
	//	delete m_datashowDlg;
	//	m_datashowDlg = NULL;
	//}
}

BOOL CLoadSTLAppDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	
	return TRUE;
}

void CLoadSTLAppDoc::OnCloseDocument()
{
	//先对线程进行退出
	if (NULL != pDataReader)
	{
		pcapReader.Stop();
		pcapReader.Destory();
	}
	if (NULL != pRawProcessor)
	{
		pcapRawProcessor.End_ClearFrame();
		pcapRawProcessor.Destory();
	}
	if (NULL != pFrameProcessor)
	{
		pcapFrameProcessor.Stop();
		pcapFrameProcessor.Destory_Trasfrom();
	}
	while (NULL != pFrameProcessor) { Sleep(1); };
	STLApp_DEBUG(_T("DataTsfer已经关闭"));
	while (NULL != pRawProcessor) { Sleep(1); };
	STLApp_DEBUG(_T("RawProcessor已经关闭"));
	while (NULL != pDataReader) { Sleep(1); };
	STLApp_DEBUG(_T("DataReader已经关闭"));
	Sleep(50);
	STLApp_DEBUG(_T("OnCloseDocument里面的睡眠结束"));
	CDocument::OnCloseDocument();
}


// CLoadSTLAppDoc 序列化

void CLoadSTLAppDoc::Serialize(CArchive& ar)
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
void CLoadSTLAppDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CLoadSTLAppDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CLoadSTLAppDoc::SetSearchContent(const CString& value)
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

// CLoadSTLAppDoc 诊断

#ifdef _DEBUG
void CLoadSTLAppDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLoadSTLAppDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

//打开obj文件事件
void CLoadSTLAppDoc::OnOpenObjfile()
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	BOOL objReaderStatus = pMain->Get_OBJFILEStatus();
	pMain->Set_OBJFILEStatus(!objReaderStatus);

	if (!objReaderStatus)
	{//打开一个文件
		m_RoomModelList.ClearAllModels();//清除当前模型

		CFileDialog filedlg(TRUE, _T("obj"), NULL, OFN_HIDEREADONLY, _T("model file(*.obj)|*.obj||"), NULL);
		filedlg.m_ofn.lpstrTitle = _T("Load OBJ Model");

		if (filedlg.DoModal() == IDOK)
		{
			CString filename = filedlg.GetPathName();

			COBJ fileLoad;
			int iRet = fileLoad.LoadObjFile(filename, m_RoomModelList);

			//返回值0---mtl文件不存在,但是obj读入成功,1--成功,2--文件不存在, 3---当前模型为四边形模型, 4--无法识别的数据格式
			if (iRet > 1)
			{
				switch (iRet)
				{
				case 2:
					AfxMessageBox(_T("当前obj文件无法打开!"));
					break;
				case 3:
					AfxMessageBox(_T("当前obj文件为四边形模型,本软件不支持!"));
					break;
				case 4:
					AfxMessageBox(_T("当前obj文件格式无法识别!"));
					break;
				default:
					AfxMessageBox(_T("当前obj文件不是标准格式,无法打开!"));
					break;
				}
				return ;
			}
			else
			{
				CLoadSTLAppView *pCSTLView = (CLoadSTLAppView *)pMain->GetActiveView();
				PointsAndSTL3DWnd *pPointsAndSTL3DWnd = pCSTLView->GetShow3DWnd();

				SetShowValue(1, 0, _T("OBJ"));//模型类型
				SetShowValue(1, 1, m_RoomModelList.GetTotalVertex());//顶点总数
				SetShowValue(1, 2, m_RoomModelList.GetTotalSurface());//三角面片总数

				pPointsAndSTL3DWnd->AttachRoomModel(&m_RoomModelList);
			}
		}
	}
	else
	{
		m_RoomModelList.ClearAllModels();//清除当前模型
		CLoadSTLAppView *pCSTLView = (CLoadSTLAppView *)pMain->GetActiveView();
		PointsAndSTL3DWnd *pPointsAndSTL3DWnd = pCSTLView->GetShow3DWnd();

		SetShowValue(1, 0, _T(""));//模型类型
		SetShowValue(1, 1, _T(""));
		SetShowValue(1, 2, _T(""));

		pPointsAndSTL3DWnd->AttachRoomModel(&m_RoomModelList);
	}
}

//打开stl文件事件
void CLoadSTLAppDoc::OnOpenStlfile()
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	BOOL stlReaderStatus = pMain->Get_STLFILEStatus();
	pMain->Set_STLFILEStatus(!stlReaderStatus);

	if (!stlReaderStatus)
	{//打开一个文件，准备使用多线程进行打开
		CFileDialog filedlg(TRUE, _T("stl"), NULL, OFN_HIDEREADONLY, _T("model file(*.stl)|*.stl||"), NULL);
		filedlg.m_ofn.lpstrTitle = _T("Load STL Model");

		if (filedlg.DoModal() == IDOK)
		{
			CString filename = filedlg.GetPathName();

			if (m_DataList.m_pVertex != NULL)
			{
				delete[]m_DataList.m_pVertex;
				m_DataList.m_pVertex = NULL;
			}
				
			if (m_DataList.m_pNormal != NULL)
			{
				delete[] m_DataList.m_pNormal;
				m_DataList.m_pNormal = NULL;
			}
			
			bool bRet = false;
			STLReader sltReader;

			USES_CONVERSION;
			bRet = sltReader.ReadFile(W2A(filename.GetBuffer()), m_DataList);

			CLoadSTLAppView *pCSTLView = (CLoadSTLAppView *)pMain->GetActiveView();
			PointsAndSTL3DWnd *pPointsAndSTL3DWnd = pCSTLView->GetShow3DWnd();

			if (bRet == true && pPointsAndSTL3DWnd != NULL)
			{
				SetShowValue(1, 0, _T("STL"));//模型类型
				SetShowValue(1, 1, m_DataList.m_iVertexNum);//顶点总数
				SetShowValue(1, 2, m_DataList.m_iTriNum);//三角面片总数

				pPointsAndSTL3DWnd->AttachDataList(&m_DataList);
			}

		}
	}
	else
	{//关闭一个文件
		CLoadSTLAppView *pCSTLView = (CLoadSTLAppView *)pMain->GetActiveView();
		PointsAndSTL3DWnd *pPointsAndSTL3DWnd = pCSTLView->GetShow3DWnd();

		m_DataList.ClearData();
		SetShowValue(1, 0, _T(""));//模型类型
		SetShowValue(1, 1, _T(""));
		SetShowValue(1, 2, _T(""));
		pPointsAndSTL3DWnd->AttachDataList(&m_DataList);
	}
}

/**************************************************************************
*  @Function Name :: StartPcapReader
*  @Description   :: 开启文件读取线程
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void CLoadSTLAppDoc::StartPcapReader()
{//还原所有场景
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	BOOL PcapReaderStatus = pMain->Get_PCAPFILEStatus();
	if (PcapReaderStatus)//打开，准备关闭，清空点
	{
		//将表格的显示恢复空
		SetShowValue(0, 0, _T(""));//帧数
		SetShowValue(0, 1, _T(""));//是否显示平均帧
		SetShowValue(0, 2, _T(""));//平均帧点云数
		SetShowValue(0, 3, _T(""));//通道数
		SetShowValue(0, 4, _T(""));//最大方差通道
		SetShowValue(0, 5, _T(""));//最小方差通道
		for (int i = 0; i < m_PointClouds.LiDAR_Channels; i++)//这个是计数总共多少个
		{
			DelGridProperty(0, 6);
		}

		if (NULL != pDataReader)
		{
			pcapReader.Stop();
			pcapReader.DisConnection();
		}
		while (NULL != pDataReader) { Sleep(0); };
		pcapRawProcessor.End_ClearFrame();
		pcapFrameProcessor.ReTrasform();
		CLoadSTLAppView *pCSTLView = (CLoadSTLAppView *)pMain->GetActiveView();
		pCSTLView->ClearFrameShow();//清除所有的点和线
		pMain->Set_PCAPFILEStatus(!PcapReaderStatus);

		STLApp_DEBUG(_T("关闭文件Reader"));
		return;
	}
	
	//没有打开，准备打开
	//如果线程还没有创建,接收帧和转换帧一起创建
	if (NULL == pFrameProcessor)
	{//先创建帧转换线程
		pFrameProcessor = AfxBeginThread((AFX_THREADPROC)DataTsfer, this);
		STLApp_DEBUG(_T("开启DataTsfer线程"));
	}
	if (NULL == pRawProcessor)
	{//再创建原始buff转换线程
		pRawProcessor = AfxBeginThread((AFX_THREADPROC)RawProcessor, this);
		STLApp_DEBUG(_T("开启RawProcessor线程"));
	}
	if (NULL == pDataReader)
	{//最后创建文件读取线程
		pDataReader = AfxBeginThread((AFX_THREADPROC)PcapReader, this);
		STLApp_DEBUG(_T("开启PcapReader线程"));
	}
}

/**************************************************************************
*  @Function Name :: DataTsfer
*  @Description   :: 一直都在转换帧线程
*  @Author        :: zzw
*  @Parameters    :: pParam:CRadarViewDoc类指针
*
*  @Return        :: None
**************************************************************************/
void CLoadSTLAppDoc::DataTsfer(LPVOID pParam)
{
	CLoadSTLAppDoc *pCSTLDoc = (CLoadSTLAppDoc *)pParam;
	pCSTLDoc->pcapFrameProcessor.SetProtocol(pCSTLDoc->mySensor.sensorKind, pCSTLDoc->m_bIsFilter, pCSTLDoc->FilterFrameNumber);//LIDAR_INVALID
	pCSTLDoc->pcapFrameProcessor.BindTrasferOneFrameFunc(React_DataTsfer, pParam);//进行回调函数和相关对象注册
	pCSTLDoc->pcapFrameProcessor.Start();
	pCSTLDoc->pcapFrameProcessor.UnSetProtocol();//取消协议设置
	pCSTLDoc->pFrameProcessor = NULL;
	STLApp_DEBUG(_T("关闭RawProcessor线程"));
	return;
}

/**************************************************************************
*  @Function Name :: React_DataTsfer
*  @Description   :: 数据转换好的回调函数
*  @Author        :: zzw
*  @Parameters    :: lParam:一帧坐标帧的智能指针
*					 pParam:CRadarViewDoc类指针
*  @Return        :: None
**************************************************************************/
void CLoadSTLAppDoc::React_DataTsfer(std::shared_ptr<RadarPointList> &lParam, LPVOID pParam)
{
	CLoadSTLAppDoc *pCSTLDoc = (CLoadSTLAppDoc *)pParam;
	pCSTLDoc->Frame = lParam;

	//发送消息进行满一帧处理
	HWND hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView()->GetSafeHwnd();

	SendMessage(hwnd, MY_MSG_FullOneFrame, 0, (LPARAM)&pCSTLDoc->Frame);//1代表需要尺度判断
}

/**************************************************************************
*  @Function Name :: React_OneFrame
*  @Description   :: 满原始一帧的回调函数
*  @Author        :: zzw
*  @Parameters    :: lParam:一帧原始数据帧的智能指针
*					 pParam:CRadarViewDoc类指针
*  @Return        :: None
**************************************************************************/
void CLoadSTLAppDoc::React_OneFrame(std::shared_ptr<RawFrame> &lParam, LPVOID pParam)
{
	std::shared_ptr<RawFrame> pRawFrame = lParam;
	CLoadSTLAppDoc *pSTLDoc = (CLoadSTLAppDoc *)pParam;
	int restFrameNumber = pSTLDoc->pcapFrameProcessor.AddFrame(pRawFrame);
	pRawFrame = nullptr;
}

/**************************************************************************
*  @Function Name :: RawProcessor
*  @Description   :: 等待接收原始buff线程
*  @Author        :: zzw
*  @Parameters    :: pParam:CRadarViewDoc类指针
*
*  @Return        :: None
**************************************************************************/
void CLoadSTLAppDoc::RawProcessor(LPVOID pParam)
{
	CLoadSTLAppDoc *pCSTLDoc = (CLoadSTLAppDoc *)pParam;
	pCSTLDoc->pcapRawProcessor.SetProtocol(pCSTLDoc->mySensor.sensorKind);
	pCSTLDoc->pcapRawProcessor.BindFullFrameCallbackFunc(React_OneFrame, pParam);//进行回调函数和相关对象注册
	pCSTLDoc->pcapRawProcessor.Start();
	pCSTLDoc->pcapRawProcessor.UnSetProtocol();//取消协议设置
	pCSTLDoc->pRawProcessor = NULL;
	STLApp_DEBUG(_T("关闭RawProcessor线程"));
	return;
}

/**************************************************************************
*  @Function Name :: PcapReader
*  @Description   :: 打开pcap文件线程
*  @Author        :: zzw
*  @Parameters    :: pParam:CRadarViewDoc类指针
*
*  @Return        :: None
**************************************************************************/
void CLoadSTLAppDoc::PcapReader(LPVOID pParam)
{
	CLoadSTLAppDoc *pCSTLDoc = (CLoadSTLAppDoc *)pParam;
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;

	//打开图像文件，获取文件路径名
	TCHAR szFilter[] = L"PCAP(*.pcap)|*.pcap|PCAPNG(*.pcapng)|*.pcapng||";
	//Sleep(10);
	CFileDialog dlgFileOpenFile(TRUE, _T("pcap"), _T("*.pcap"), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter, 0);
	//Sleep(10);
	HWND hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView()->GetSafeHwnd();
	//打开图像
	CString File_path;
	if (dlgFileOpenFile.DoModal() == IDOK)
	{
		//读取图像文件名
		File_path = dlgFileOpenFile.GetPathName();
		STLApp_DEBUG(_T("pcap文件路径成功获取。"));
	}
	else
	{
		STLApp_DEBUG(_T("pcap文件路径没有获取成功"));
		pCSTLDoc->pDataReader = NULL;
		return;
	}

	PcapFileSet setDlg(File_path);
	setDlg.DoModal();
	pCSTLDoc->mySensor = setDlg.GetSensorAtt();

								 
	USES_CONVERSION;//声明标识符
	//调用函数，T2A和W2A均支持ATL和MFC中的字符转换
	char *pFileName = T2A(File_path);

	if (RaView_NO_ERROR != pCSTLDoc->pcapReader.BuildFileConnection(pFileName, EndPcapCallBackFun, pMain, pCSTLDoc->mySensor.sensorKind))//进行连接的参数设置,读取完毕的回调函数
	{//初始化有错误
		pCSTLDoc->pDataReader = NULL;
		MessageBox(hwnd, _T("文件没有打开成功！"), 0, 0);
		pCSTLDoc->pDataReader = NULL;
		return;
	}
	STLApp_DEBUG(_T("File连接参数设置成功。"));

	pMain->Set_PCAPFILEStatus(true);

	//此时读取总帧数，然后进行设置
	unsigned int total_frameNumber = pCSTLDoc->pcapReader.GetFrameNumber();
	if (total_frameNumber > 0)
	{
		pCSTLDoc->m_PointClouds.SetPointCloudNums(total_frameNumber);
	}

	pCSTLDoc->pcapReader.Stop();
	//在这之前将之前可能有的显示都清空
	pCSTLDoc->pcapRawProcessor.End_ClearFrame();
	pCSTLDoc->pcapFrameProcessor.ReTrasform();
	pCSTLDoc->pcapFrameProcessor.UnSetProtocol();//取消协议设置
	pCSTLDoc->pcapFrameProcessor.SetProtocol(pCSTLDoc->mySensor.sensorKind, pCSTLDoc->m_bIsFilter, pCSTLDoc->FilterFrameNumber);
	pCSTLDoc->pcapRawProcessor.UnSetProtocol();//取消协议设置
	pCSTLDoc->pcapRawProcessor.SetProtocol(pCSTLDoc->mySensor.sensorKind);

	STLApp_DEBUG(_T("启动ReaderStart"));
	pCSTLDoc->pcapReader.Start(2);//Start线程一开始，内部便不停地做循环操作
	STLApp_DEBUG(_T("结束ReaderStart"));
	pCSTLDoc->m_PointClouds.Clear();
	pCSTLDoc->pDataReader = NULL;
	STLApp_DEBUG(_T("关闭PcapReader线程"));
	return;
}

//设置点云数目，返回是否设置成功
bool CLoadSTLAppDoc::SetPointCloudNums(UINT pointCloudNums)
{
	return m_PointClouds.SetPointCloudNums(pointCloudNums);
}

//添加一帧点云数据，返回是否添加成功
bool CLoadSTLAppDoc::AttachPointCloud(pRadarPointList pPointCloud)
{
	return m_PointClouds.AttachPointCloud(pPointCloud);
}

//设置传感器中心的坐标
void CLoadSTLAppDoc::SetSensorCenter(double sensor_x, double sensor_y, double sensor_z)
{
	transformer.SetSensorCenter(sensor_x, sensor_y, sensor_z);
}

//设置传感器的姿态角
void CLoadSTLAppDoc::SetSensorEuler(double sensor_pitch, double sensor_roll, double sensor_yaw)
{
	transformer.SetSensorEuler(sensor_pitch, sensor_roll, sensor_yaw);
}

/**************************************************************************
*  @Function Name :: EndPcapCallBackFun
*  @Description   :: pcap文件读取结束的返回函数
*  @Author        :: zzw
*  @Parameters    :: pParam:主窗口指针，因为这是全局函数
*
*  @Return        :: None
**************************************************************************/
void CLoadSTLAppDoc::EndPcapCallBackFun(LPVOID pParam)
{
	//发送读完文件的消息
	HWND hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView()->GetSafeHwnd();
	//等到所有的帧都过来了，再发消息
	CLoadSTLAppDoc *pCSTLDoc = (CLoadSTLAppDoc *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView()->GetDocument();
	int FrameNumber = pCSTLDoc->m_PointClouds.m_uiNowCloudNums;
	int TotalNumber = pCSTLDoc->pcapReader.GetFrameNumber();
	int tmp = FrameNumber;
	int sign = 0;
	while (FrameNumber < TotalNumber)
	{
		//考虑到可能的丢帧情况
		Sleep(1);
		FrameNumber = pCSTLDoc->m_PointClouds.m_uiNowCloudNums;
		if (tmp == FrameNumber)
		{
			sign++;
		}
		else
		{
			sign = 0;
		}
		if (sign >= 50)
		{
			break;//如果过了很长时间FrameNumber还没有变化，则认为是有丢帧情况，此时退出循环
		}
	}
	SendMessage(hwnd, MY_MSG_EndPcapFresh, 0, 0);//1代表需要尺度判断
}

//更改表格显示
void CLoadSTLAppDoc::SetShowValue(int PropertyNumber, int SubItemNumber, CString setValue)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->SetShowValue(PropertyNumber,SubItemNumber, setValue);
}

//更改表格显示
void CLoadSTLAppDoc::SetShowValue(int PropertyNumber, int SubItemNumber, int setValue)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->SetShowValue(PropertyNumber, SubItemNumber, setValue);
}

void CLoadSTLAppDoc::SetShowValue(int PropertyNumber, int SubItemNumber, double doub_setValue)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->SetShowValue(PropertyNumber, SubItemNumber, doub_setValue);
}

//将pProp加到第PropertyNumber的SubItemNumber里面
void CLoadSTLAppDoc::AddGridProperty(CMFCPropertyGridProperty *pProp, int PropertyNumber, int SubItemNumber)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->AddGridProperty(pProp, PropertyNumber, SubItemNumber);
}

//删除里面内容
void CLoadSTLAppDoc::DelGridProperty(int PropertyNumber, int SubItemNumber)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->DelGridProperty(PropertyNumber, SubItemNumber);
}

//开始进行计算
void CLoadSTLAppDoc::OnBeginCalculation()
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CLoadSTLAppView *pCSTLView = (CLoadSTLAppView *)pMain->GetActiveView();
	PointsAndSTL3DWnd *pPointsAndSTL3DWnd = pCSTLView->GetShow3DWnd();
	//开始计算将图标设置成计算
	BOOL Calculate_Status = pMain->Get_CalculationStatus();
	if (false == Calculate_Status)
	{//没有开始计算则开始计算
		pMain->Set_CalculationStatus(true);
		//进行配准
		RigidRegistration();
		//将计算结果传递给Wnd进行显示
		pPointsAndSTL3DWnd->AfterRigidRegistration();

		//结束计算将图标设置成结束计算
		pMain->Set_CalculationStatus(false);
	}
	else
	{
		MessageBox(NULL, _T("正在计算请稍后！"),_T("提示"),NULL);
	}
}

//展示计算过程的对话框
void CLoadSTLAppDoc::OnShowDataGrid()
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	BOOL ShowDataGrid_Status = pMain->Get_ShowDataGrid();
	if (false == ShowDataGrid_Status)
	{//进行显示
		pMain->Set_ShowDataGrid(true);
	}
	else
	{//开始取消当前显示
		pMain->Set_ShowDataGrid(false);
	}
	pMain->ReDrawShow();//更新界面显示
}

//保存计算结果响应函数
void CLoadSTLAppDoc::OnSaveData()
{//进行保存
 //VECTOR *tmp_intersection_pointcloud = NULL;//临时的点云数据(用来计算点云的交点)
 //for (int i = 0; i < m_RadarPointsNode->m_pPointClouds->m_uiNowCloudNums; i++)//为这么多数目new很多数据
 //{
 //	int tmp_size = m_RadarPointsNode->m_pPointClouds->m_PointCloud[i]->size();
 //	tmp_intersection_pointcloud = new VECTOR[tmp_size];
 //	m_pInterSection.push_back(tmp_intersection_pointcloud);
 //	tmp_intersection_pointcloud = NULL;
 //}

 //m_calcul_intersection.GetPointCloudsIntersection(m_RadarPointsNode->m_pPointClouds, m_pInterSection);

 ////计算好之后对数据进行保存
 //ofstream fout;
 //fout.open("F:\\常州房间文件\\data.csv", ios::out);
 //fout << "index,x,y,z,intersection_x,intersection_y,intersection_z" << endl;
 //for (int i = 0; i < 1; i++)
 //{
 //	pRadarPointList tmp_PointCloud = m_RadarPointsNode->m_pPointClouds->m_PointCloud[i];
 //	VECTOR* tmp_intersection_PointCloud = m_pInterSection[i];
 //	for (int j = 0; j < tmp_PointCloud->size(); j++)
 //	{
 //		fout << j << "," << (*tmp_PointCloud)[j].x << ","\
 	//			<< (*tmp_PointCloud)[j].y << ","\
	//			<< (*tmp_PointCloud)[j].z << ","\
	//			<< tmp_intersection_PointCloud[j].x << ","\
	//			<< tmp_intersection_PointCloud[j].y << ","\
	//			<< tmp_intersection_PointCloud[j].z << endl;
//	}
//}
//fout.close();

////结尾释放申请的内存
//if (m_pInterSection.size() > 0)
//{
//	for (int i = 0; i < m_pInterSection.size(); i++)
//	{
//		delete(m_pInterSection[i]);
//		m_pInterSection[i] = NULL;
//	}
//	m_pInterSection.clear();
//}
}

//计算点云的一些特征，并最终显示一帧点云数据
void CLoadSTLAppDoc::CalculatePointClouds()
{
	//文件中所有帧已经读取完毕
	m_calcul_pointclouds.Init(mySensor.sensorKind);
	m_calcul_pointclouds.GetPointClouds(&m_PointClouds);
	m_calcul_pointclouds.Compute_attribution();//计算点云帧属性
}

//计算点云和STL的交点
//因为点云需要配准，所以对点云进行配准，在点云配准之后，点云和STL的交点的计算工作也就相应完成了
void CLoadSTLAppDoc::RigidRegistration()
{
	if (((NULL == m_DataList.m_pVertex) || (0 == m_DataList.m_iTriNum)) && (0 == m_RoomModelList.GetModelNum()))
	{//没有STL，返回
		MessageBox(NULL,_T("请打开有效的OBJ或者STL模型！"),0,0);
		return;
	}
	if (0 == m_PointClouds.m_uiNowCloudNums)
	{//没有点云帧，返回
		MessageBox(NULL,_T("请打开点阵云"),0,0);
		return;
	}
	if (m_RoomModelList.GetModelNum() != 0)
	{//使用OBJ模型
		m_calcul_intersection.GetOBJ(&m_RoomModelList);
	}
	else
	{//使用STL模型
		m_calcul_intersection.GetSTL(&m_DataList);//得到STL并对STL进行整理
	}

	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CLoadSTLAppView *pCSTLView = (CLoadSTLAppView *)pMain->GetActiveView();
	PointsAndSTL3DWnd *pPointsAndSTL3DWnd = pCSTLView->GetShow3DWnd();

	//首先进行点云简化
	

	//先为m_parvector分配大小4*4
	m_parvector = new double*[4];
	for (int i = 0; i < 4; i++)
	{
		m_parvector[i] = new double[4];
		for (int j = 0; j < 4; j++)
		{
			if (i == j)
			{
				m_parvector[i][j] = 1;
			}
			else
			{
				m_parvector[i][j] = 0;
			}
		}
	}

	m_iternum = 1;
	//因为点云数据进行过计算，所以配准的时候只需要计算和平均帧之间的关系
	memset(m_parameter, 0.0, sizeof(double) * 4);

	//m_parameter[0] = 11.38;
	//m_parameter[1] = 2.031379;
	//m_parameter[2] = 0.24055;
	//m_parameter[3] = 0.1545;
	m_PointClouds.SetParameter(m_parameter);
	pPointsAndSTL3DWnd->AttachPointClouds(&m_PointClouds);
	SetShowValue(2, 0, _T("Powell"));
	//设置配准所用通道
	//int choosechannel[4] = {2,6,9,14};
	int choosechannel[4] = { 6,7,8,9 };
	m_PointClouds.Setchoosechannel(choosechannel, 4);//配准前需要先设置配准所用通道
	CString channelshow;
	channelshow.Format(_T("%d,%d,%d,%d"), choosechannel[0], choosechannel[1], choosechannel[2], choosechannel[3]);
	SetShowValue(2, 1, channelshow);//配准所用通道

	DWORD dwStart, m_registime = 0;;//配准计时变量
	dwStart = GetTickCount();
	
	m_rigid_registration.powell(this, m_parameter, m_parvector, 4, 1.0e-7, &m_iternum, &result, RegisByVar);
	
	m_registime = GetTickCount() - dwStart;
	CString myshow;
	int sec = (int)m_registime / 1000;
	int minu = sec / 60;
	sec = sec % 60;
	myshow.Format(_T("%d分%d秒\n"), minu, sec);
	SetShowValue(2, 6, myshow);//计算所用时间

	myshow.Format(_T("%f度"), m_parameter[0]*0.05);
	SetShowValue(2, 2, myshow);
	myshow.Format(_T("%f米"), m_parameter[1]*0.05);
	SetShowValue(2, 3, myshow);
	myshow.Format(_T("%f米"), m_parameter[2]*0.05);
	SetShowValue(2, 4, myshow);
	myshow.Format(_T("%f米"), m_parameter[3]*0.05);
	SetShowValue(2, 5, myshow);


	//因为有一个标志位，标志了有没有计算平均帧，Wnd那边显示会根据这个标志位选择显示平均帧还是所有帧
	m_PointClouds.SetParameter(m_parameter);
	pPointsAndSTL3DWnd->AttachPointClouds(&m_PointClouds);

	//释放4*4
	for (int i = 0; i < 4; i++)
	{
		delete(m_parvector[i]);
	}
	delete(m_parvector);
	
	//计算好之后开始进行系统误差和随机误差的显示
	double result1[17] = { 0.0 };
	int tmp_choosechannel[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	//int choosechannel[4] = { 6,7,8,9 };
	m_PointClouds.Setchoosechannel(tmp_choosechannel, 16);//配准前需要先设置配准所用通道
	m_calcul_intersection.GetPointCloudIntersection(&m_PointClouds, m_PointClouds.m_parameter);
	m_calcul_pointclouds.Compute_Evaluation(result1,17);
	SetShowValue(3, 0, result1[16]);//平均系统误差
	int tmp_max = 0;
	int tmp_min = 0;
	for (int i = 0; i < m_PointClouds.LiDAR_Channels; i++)
	{
		if (result1[tmp_max] < result1[i])
		{
			tmp_max = i;
		}
		if (result1[tmp_min] > result1[i])
		{
			tmp_min = i;
		}
	}
	SetShowValue(3, 1, tmp_max);//平均系统误差最大通道
	SetShowValue(3, 2, tmp_min);//平均系统误差最小通道

	for (int i = 0; i < m_PointClouds.LiDAR_Channels; i++)
	{
		//求出每一通道的系统方差
		CString tmp_show1;
		CString tmp_show2;
		tmp_show1.Format(_T("%d"), i);
		tmp_show2.Format(_T("%.7f"), result1[i]);
		CMFCPropertyGridProperty *ptmp_Prop = new CMFCPropertyGridProperty(tmp_show1, tmp_show2);//5
		ptmp_Prop->AllowEdit(FALSE);
		AddGridProperty(ptmp_Prop, 3, 3);

	}
}

double CLoadSTLAppDoc::RegisByVar(void *pDoc, double *p)
{
	return ((CLoadSTLAppDoc*)pDoc)->CalVar(p);
}

//计算点云和STL之间的互信息
double CLoadSTLAppDoc::CalVar(double *p)
{
	double result[5] = { 0.0 };
	//先计算点云和模型之间的交点
	m_calcul_intersection.GetPointCloudIntersection(&m_PointClouds, p);
	//求出类似互信息的评价指标并返回
	m_calcul_pointclouds.Compute_Evaluation(result,5);

	return result[4];
}