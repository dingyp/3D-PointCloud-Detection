#include "stdafx.h"
#include"Reader.h"
#include "ErrorManager/ErroCodeDefine.h"

using namespace DataReader;

/**************************************************************************
*  @Function Name :: Reader
*  @Description   :: Reader类的默认构造函数，此时还未明确Reader功能
*  @Author        :: zzw
*  @Parameters    :: None
*					
*  @Return        :: None
**************************************************************************/
Reader::Reader()
{
	pSensorReader = NULL;
	pFileReader = NULL;
	mode = INVALID_READER;//无效读取方式
	pRawProcess = NULL;
	memset(filepath, 0, sizeof(filepath));
	READER_DEBUG(_T("正在启动Reader"));
}

/**************************************************************************
*  @Function Name :: ~Reader
*  @Description   :: 对最后所有的资源进行最后一次的检查释放
*  @Author        :: zzw
*  @Parameters    :: None
*					
*  @Return        :: None
**************************************************************************/
Reader::~Reader()
{
	Destory();
	pSensorReader = NULL;
	pFileReader = NULL;
	mode = INVALID_READER;
	pRawProcess = NULL;
	READER_DEBUG(_T("正在注销Reader"));
}

/**************************************************************************
*  @Function Name :: BuildSensorConnection
*  @Description   :: 建立传感器的连接，其他连接将中断
*  @Author        :: zzw
*  @Parameters    :: IP:传感器的IP地址 port:传感器的端口号
*					 SetProtocol:接收数据的协议 Mask:传感器的地址掩码 
*  @Return        :: 运行是否成功
**************************************************************************/
int Reader::BuildSensorConnection(const char *IP, unsigned short dest_port, unsigned short local_port, LiDAR_Protocol SetProtocol, const char*Mask)
{
	if (NULL == pRawProcess)						//没有pcap包处理类，退出
	{
		return ER_BuildBeforeInit;
	}
	if (NULL != pSensorReader)						//传感器已经有连接，退出
	{
		return ALREADY_Sen_Con;
	}
	if (NULL != pFileReader)						//若有文件连接，关闭文件连接
	{
		DisFileConnection();
		delete(pFileReader);
		pFileReader = NULL;
	}
	pSensorReader = new SensorReader();
	if (NULL == pSensorReader)						//创建传感连接失败，退出
	{
		return ER_CREATESENCON;
	}
	pSensorReader->Init(pRawProcess, SetProtocol);
	int result = pSensorReader->SensorOpen(IP, dest_port, local_port, Mask);
	if (result)	//传感器打开失败,注销传感器连接，退出
	{
		DisSensorConnection();
		delete(pSensorReader);
		pSensorReader = NULL;
		return ER_NOTConnectToSensor;
	}
	mode = SENSOR_READER;
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: BuildFileConnection
*  @Description   :: 建立读取文件连接
*  @Author        :: zzw
*  @Parameters    :: path:文件路径 PlayEndStop:初始化播放到停止的回调函数
*					 pParam:回调函数中的参数 SetProtocol:读文件使用的协议类型
*  @Return        :: 运行是否成功
**************************************************************************/
int Reader::BuildFileConnection(const char *path, void(*PlayEndStop)(LPVOID pParam), LPVOID pParam, LiDAR_Protocol SetProtocol)
{
	if (NULL == pRawProcess)						//没有pcap包处理类，退出
	{
		return ER_BuildBeforeInit;
	}
	if (NULL != pFileReader)						//若有文件连接，退出
	{
		return ALReadyFileConnection;
	}
	if (NULL != pSensorReader)						//若有传感读取器，关闭连接
	{
		DisSensorConnection();
		delete(pSensorReader);
		pSensorReader = NULL;
	}
	pFileReader = new FileReader();
	if (NULL==pFileReader)
	{
		return ER_CreateFileConnection;
	}
	strcpy_s(filepath, 1000, path);					//保存文件名（保存csv文件可能用到）
	int statusInit = pFileReader->Init(pRawProcess, PlayEndStop, pParam, SetProtocol);
	if (statusInit != RaView_NO_ERROR)				//进行init状态判断
	{
		//连接文件失败需要将new的文件指针释放
		DisFileConnection();
		delete(pFileReader);
		pFileReader = NULL;
		return statusInit;
	}
	int statusopen = pFileReader->FileOpen(path);
	if (RaView_NO_ERROR != statusopen)				//进行fileopen状态判断
	{
		DisFileConnection();
		delete(pFileReader);
		pFileReader = NULL;
		return statusopen;
	}
	mode = FILE_READER;
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: Start
*  @Description   :: 开始读取
*  @Author        :: zzw
*  @Parameters    :: status:文件连接时，重新开始是否需要判断所处状态的标志位
*					
*  @Return        :: 运行是否成功
**************************************************************************/
int Reader::Start(int status)
{
	int result = RaView_NO_ERROR;
	switch (mode)
	{
	case FILE_READER:
		if (NULL == pFileReader)
		{
			return ER_FileReaderPointIsNULL;
		}
		if (pFileReader->GetIsStart())				//文件线程已开启,重新开始播放
		{
			pFileReader->ReStart(status);
		}
		else										//文件线程第一次开启
		{
			pFileReader->Start(status);
			DisConnection();
		}
		break;
	case SENSOR_READER:
		if (NULL == pSensorReader)
		{
			return ER_SensorReaderPointIsNULL;
		}
		if (pSensorReader->GetIsStart())			//传感器线程已经开启,重新开始
		{
			pSensorReader->ReStart();
		}
		else										//传感线程第一次开启
		{
			pSensorReader->ReStart();
			result = pSensorReader->Start();
			DisConnection();
		}
		break;
	case INVALID_READER:
		break;
	}
	return result;
}

/**************************************************************************
*  @Function Name :: Stop
*  @Description   :: 读取暂停
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void Reader::Stop()
{
	switch (mode)
	{
	case FILE_READER:
		if (NULL == pFileReader)
		{
			return;
		}
		pFileReader->Stop();
		break;
	case SENSOR_READER:
		if (NULL == pSensorReader)
		{
			return;
		}
		pSensorReader->Stop();//置暂停位
		break;
	case INVALID_READER:
		break;
	}
	return;
}

/**************************************************************************
*  @Function Name :: DisConnection
*  @Description   :: 断开所有连接,之后需要重新建立；和Destory相比少了置空Processor那一步
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void Reader::DisConnection()
{
	if (INVALID_READER == mode)
	{
		return;
	}
	if (pSensorReader)
	{
		if (ALReadyDisSenConnection != DisSensorConnection())
		{
			delete(pSensorReader);
			pSensorReader = NULL;
		}
	}
	if (pFileReader)
	{
		if (ALReadyDisFileConnection != DisFileConnection())
		{
			delete(pFileReader);
			pFileReader = NULL;
		}
	}
	mode = INVALID_READER;
}

/**************************************************************************
*  @Function Name :: DisSensorConnection
*  @Description   :: 取消传感器连接
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
int Reader::DisSensorConnection()
{
	mode = INVALID_READER;
	if (NULL == pSensorReader)
	{
		return ALReadyDisSenConnection;
	}
	pSensorReader->Stop();
	pSensorReader->Destory();
	pSensorReader->SensorClose();
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: DisFileConnection
*  @Description   :: 取消读取文件连接
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
int Reader::DisFileConnection()
{
	mode = INVALID_READER;
	if (NULL == pFileReader)
	{//已经关闭了
		return ALReadyDisFileConnection;
	}
	pFileReader->Stop();
	pFileReader->FileClose();
	pFileReader->Destory();
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: OpenCSVFile
*  @Description   :: 打开CSV文件并显示
*  @Author        :: zzw
*  @Parameters    :: csvFilePath:csv文件路径 Callback:读取完毕的回调函数
*					 pParam:回调函数中的参数
*  @Return        :: 运行是否成功
**************************************************************************/
int Reader::OpenCSVFile(const char *csvFilePath, void(*Callback)(std::shared_ptr<RadarPointList> &lParam, LPVOID pParam), LPVOID pParam)
{
	if (NULL != pFileReader)
	{//文件读取器没关闭，返回
		return ALReadyFileConnection;
	}
	if (NULL != pSensorReader)
	{//传感器读取器没关闭，返回
		return ALREADY_Sen_Con;
	}
	mode = CSV_READER;
	
	csvReader.Init(Callback, pParam);
	csvReader.CSVOpen(csvFilePath);
	csvReader.Destory();//读取完csv文件之后进行置空
	mode = INVALID_READER;//读取之后就可以设置成csv模式
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: WriteCSVFile
*  @Description   :: 保存csv文件
*  @Author        :: zzw
*  @Parameters    :: csv_savepath:csv文件保存路径
*					 PointsList:需要保存的点帧
*  @Return        :: None
**************************************************************************/
void Reader::WriteCSVFile(const char *csv_savepath, RadarPointList& PointsList)
{//保存csv文件应该是不管处于何种模式之下都可以进行工作
	csvReader.CSVWrite(csv_savepath, PointsList);
}

/**************************************************************************
*  @Function Name :: Get_FileName
*  @Description   :: 得到pcap文件名
*  @Author        :: zzw
*  @Parameters    :: pcapFileName:pcap文件名称
*				
*  @Return        :: 运行是否成功
**************************************************************************/
int Reader::Get_FileName(char *pcapFileName)
{//得在文件读取模式
	if (NULL != pSensorReader)
	{//传感器读取器模式，返回
		return ALREADY_Sen_Con;
	}
	if (mode != FILE_READER)
	{//不是pcap阅读模式，返回
		return ER_GetFileNameNotInPcap;
	}
	strcpy_s(pcapFileName, 1000,filepath);
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: Init
*  @Description   :: 初始化原始pcap包的处理类
*  @Author        :: zzw
*  @Parameters    :: RawProcessPoint:初始化原始数据包处理类
*
*  @Return        :: 运行是否成功
**************************************************************************/
int Reader::Init(DataProcess::RawProcess *RawProcessPoint)
{
	mode = INVALID_READER;
	if (NULL == RawProcessPoint)
	{//处理类指针为NULL
		return ER_InitPointRawProcess;
	}
	pRawProcess = RawProcessPoint;
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: Destory
*  @Description   :: 退出所有连接，释放资源
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void Reader::Destory()
{
	mode = INVALID_READER;
	pRawProcess = NULL;
	if (NULL != pSensorReader)
	{
		DisSensorConnection();
		delete(pSensorReader);
		pSensorReader = NULL;
	}
	if (NULL != pFileReader)
	{
		DisFileConnection();
		delete(pFileReader);
		pFileReader = NULL;
	}
}

/**************************************************************************
*  @Function Name :: GoToStart
*  @Description   :: 在文件连接时，移到一开始
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
int Reader::GoToStart()
{
	if (NULL == pFileReader) { return ER_FileReaderIsNULL; }
	return pFileReader->GoToStart();
}

/**************************************************************************
*  @Function Name :: GoToEnd
*  @Description   :: 在文件连接时，移到最后
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
int Reader::GoToEnd()
{
	if (NULL == pFileReader) { return ER_FileReaderIsNULL; }
	return pFileReader->GoToEnd();
}

/**************************************************************************
*  @Function Name :: GetNextFrame
*  @Description   :: 得到下一帧,返回是否成功
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
int Reader::GetNextFrame()
{
	if (NULL == pFileReader) { return ER_FileReaderIsNULL; }
	return pFileReader->GetNextFrame(true);

}

/**************************************************************************
*  @Function Name :: GetBackwardFrame
*  @Description   :: 得到上一帧,返回是否成功
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
int Reader::GetBackwardFrame()
{
	if (NULL == pFileReader) { return ER_FileReaderIsNULL; }
	return pFileReader->GetBackwardFrame(true);
}

/**************************************************************************
*  @Function Name :: GoToFrame
*  @Description   :: 跳到具体的某一帧
*  @Author        :: zzw
*  @Parameters    :: FrameNumber:跳到的帧序号
*
*  @Return        :: 运行是否成功
**************************************************************************/
int Reader::GoToFrame(const int FrameNumber)
{
	if (NULL == pFileReader) { return ER_FileReaderIsNULL; }
	return pFileReader->GoToFrame(FrameNumber,true);
}

////跳到具体某一帧，只是置标志位
//void Reader::SetFrameNumber(unsigned int frameNumber)
//{
//	if (NULL == pFileReader) { return; }
//	pFileReader->SetFrameNumber(frameNumber);
//}

/**************************************************************************
*  @Function Name :: GetFrameNumber
*  @Description   :: 得到文件中总的帧数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
int Reader::GetFrameNumber()
{
	if (NULL == pFileReader) { return 0; }
	return pFileReader->GetFrameNumber();
}

/**************************************************************************
*  @Function Name :: Loop
*  @Description   :: 开始循环播放
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
void Reader::Loop()
{
	if (NULL == pFileReader){return;}
	pFileReader->Loop();
}

/**************************************************************************
*  @Function Name :: EndLoop
*  @Description   :: 结束循环播放
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
void Reader::EndLoop()
{
	if (NULL == pFileReader){return;}
	pFileReader->EndLoop();
}

/**************************************************************************
*  @Function Name :: PlayForward
*  @Description   :: 正向播放
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
void Reader::PlayForward()
{
	if (NULL == pFileReader) { return; }
	pFileReader->PlayForward();
}

/**************************************************************************
*  @Function Name :: PlayBackward
*  @Description   :: 回退播放
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
void Reader::PlayBackward()
{
	if (NULL == pFileReader) { return; }
	pFileReader->PlayBackward();
}

/**************************************************************************
*  @Function Name :: ACCSpeed
*  @Description   :: 加快速度
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
SPEED Reader::ACCSpeed()
{
	if (NULL == pFileReader) { return READER_STOP; }
	return pFileReader->ACCSpeed();
}

/**************************************************************************
*  @Function Name :: BuildSaverConnection
*  @Description   :: 建立保存文件连接
*  @Author        :: zzw
*  @Parameters    :: savepath:保存文件的路径 SaveEndStop:文件存储太满的回调函数 
*					 pParam:回调函数里面的参数
*  @Return        :: 运行是否成功
**************************************************************************/
int Reader::BuildSaverConnection(const char*savepath, void(*SaveEndStop)(LPVOID pParam), LPVOID pParam)
{
	if (NULL == pSensorReader) { return ER_SensorReaderIsNULL; }
	return pSensorReader->BuildSaverConnection(savepath, SaveEndStop, pParam);
}

/**************************************************************************
*  @Function Name :: DisSaverConnection
*  @Description   :: 取消保存文件连接
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
void Reader::DisSaverConnection()
{
	if (NULL == pSensorReader) { return ; }
	pSensorReader->DisSaverConnection();
}

int DataReader::Reader::OpenBinFile(const char * binFilePath, void(*Callback)(std::shared_ptr<RadarPointList>&lParam, LPVOID pParam), LPVOID pParam)
{
	if (NULL != pFileReader)
	{//文件读取器没关闭，返回
		return ALReadyFileConnection;
	}
	if (NULL != pSensorReader)
	{//传感器读取器没关闭，返回
		return ALREADY_Sen_Con;
	}
	mode = BIN_READER;

	binReader.Init(Callback, pParam);
	binReader.BINOpen(binFilePath);
	binReader.Destroy();//读取完csv文件之后进行置空
	mode = INVALID_READER;//读取之后就可以设置成csv模式
	return RaView_NO_ERROR;
	
}

/**************************************************************************
*  @Function Name :: GetReaderMode
*  @Description   :: 获取传感器模式
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
MODE Reader::GetReaderMode()
{
	return mode;
}

/**************************************************************************
*  @Function Name :: GetPlayType
*  @Description   :: 取得播放类型
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
PLAYTYPE Reader::GetPlayType()
{
	if (NULL == pFileReader) { return FORWORD; }
	return pFileReader->GetPlayType();
}

/**************************************************************************
*  @Function Name :: InitSpeed
*  @Description   :: 初始化播放速度
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
void Reader::InitSpeed()
{
	if (NULL == pFileReader) { return ; }
	pFileReader->SetSpeed(ONE_SPEED);
}