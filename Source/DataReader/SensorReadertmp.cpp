//#include "stdafx.h"
//#include <iostream>
//#include <fstream>//进行pcap文件的读取
//#include <memory> //shared_ptr
//#include <windows.h>
//#include "SensorReader.h"
//#include "ErrorManager/ErroCodeDefine.h"
//
//using namespace DataReader;


//其实是1206
#define BUFFER_LEN 1300

//设置默认构造函数，因为功能不明确，将什么都不做，等着被析构
SensorReader::SensorReader()
{
	destorySensorReader = true;
	mode = INVALID_READER;				//构造时模式不明确
	SetSpeed(READER_STOP);				//初始化速度
	Init_Frame();						//初始化帧方面属性
	offset_idxTable.index_number = 0;	//再第一次的构造的时候需要初始化0
	InitConnection();					//初始化连接属性
	pcap_reader = NULL;					//读取文件指针置空
	pcap_saver = NULL;					//保存文件指针置空
}

SensorReader::~SensorReader()
{
	destorySensorReader = true;
	DisConnection();					//不管什么连接，先进行取消连接
	SetSpeed(READER_STOP);				//速度置停
	Finish_Frame();						//释放帧的属性
	mode = INVALID_READER;				//模式置无
}

//返回false说明连接失败
bool SensorReader::BuildUDPConnection(const char *IP, int port)
{
	//连接之前先取消其他连接
	DisConnection();

	Init_IP_Port(IP, port, port);		//设置网络连接

	SOCKADDR_IN m_addr_local;
	SOCKADDR_IN m_addr_dest;

	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	//当要求的版本(Winsock的最高版本)等与或高于系统支持的最底版本(下限),那么该函数   
	//操作成功 并且在WSADATA.WhighVersion中返回系统支持的最高版本,在WSADATA.wVersion中   
	//返回系统支持的最高版(上限)和 wVersionRequested 之间的较小值。
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return false;//Windows Sockets 库初始化失败.
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return false;
	}

	//创建udp 的套接字
	m_hSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (m_hSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return false;//创建socket失败.
	}
	m_addr_dest.sin_family = AF_INET;
	m_addr_dest.sin_addr.s_addr = htonl(DestIPAddress);
	m_addr_dest.sin_port = htons(DestPort);

	m_addr_local.sin_family = AF_INET;
	m_addr_local.sin_addr.s_addr = htonl(INADDR_ANY);
	m_addr_local.sin_port = htons(LocalPort);////绑定一个本地指定端口

	err = bind(m_hSocket, (SOCKADDR*)&m_addr_local, sizeof(SOCKADDR));//绑定一个本地端口，sendto的时候从local端口发送到dest端口
	if (err != 0)
	{
		closesocket(m_hSocket);
		WSACleanup();
		return false;
	}

	//最后进行状态的改变
	mode = ONLY_SENSOR_READER;
	SetSpeed(DEFAULT_SPEED);			//改变速度属性
	return true;
}

//置空网络连接属性
void SensorReader::Free_IP_Port()
{
	DestIPAddress = 0;
	DestPort = 0;
	LocalPort = 0;
}

//初始化索引表
void SensorReader::Init_IP_Port(const char *De_IP, int De_Port, int Lo_Port)
{
	DestIPAddress = ntohl(inet_addr(De_IP));
	DestPort = De_Port;
	LocalPort = Lo_Port;
}

//取消当前建立的UDP连接
void SensorReader::DisUDPConnection()
{
	if (INVALID_SOCKET != m_hSocket)
	{
		closesocket(m_hSocket);
	}
	m_hSocket = INVALID_SOCKET;
	WSACleanup();						//关闭连接
	Free_IP_Port();						//置空IP和端口
}


//取消建立的保存文件连接
void SensorReader::DisSaverFileConnection()
{
	if (pcap_saver != NULL)
	{
		pcap_saver->close();
		delete(pcap_saver);
		pcap_saver = NULL;
	}
}


//只针对传感器的帧停止播放
void SensorReader::SensorStop()
{
	//设置播放速度为0
	SetSpeed(READER_STOP);
}


//只针对传感器帧播放
void SensorReader::SensorStart()
{
	//如果没有注册回调函数，返回
	if ((NULL == FullFrame) || (NULL == RTAdress))
	{
		return;
	}
	if (NULL == m_hSocket)
	{//还未创建套接字
		return;
	}

	SOCKADDR_IN addr_from;
	int n = 0;
	int len = sizeof(SOCKADDR);
	char buf[BUFFER_LEN];
	unsigned char ubuf[BUFFER_LEN];
	int nRecvBufSize = 50 * BUFFER_LEN;//扩充缓冲区，防止有丢UDP包的现象出现
	setsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (char *)&nRecvBufSize, sizeof(int));//设置缓冲区的大小

	destorySensorReader = false;
	while (!destorySensorReader) {
		memset(buf, 0, sizeof(buf));
		n = recvfrom(m_hSocket, buf, BUFFER_LEN, 0, (SOCKADDR*)&addr_from, &len);


		//如果接收到了数据，就进行数据的处理，数据处理包括帧的创建
		if (n >0) {
			memcpy(ubuf, buf, n);
			//将buf中的数据解析到pSensorFrame中,满一帧会有一个触发函数转到pPointFrame帧中
			DataParse(ubuf, n, true);//需要进行新帧创建
		}
		else
		{
			int errnumber = WSAGetLastError();
			int number = errnumber;
		}
		n = 0;


		//数据解析之后，进行睡眠操作
		BeginSleep();//根据设置的速度进行休眠
	}
}

//将Sensor数据储到savepath中
void SensorReader::SensorToFile(const char*savepath)
{
	//..
}

//在传感器显示状态下，进行文件的保存，并返回是否操作成功
bool SensorReader::BeginFileSave(const char *path)
{
	//检查之前是否是传感器类型显示
	if (ONLY_SENSOR_READER != mode)
	{//不是返回失败
		return false;
	}

	//进行文件创建以及连接
	DisSaverFileConnection();
	pcap_saver = new std::ofstream();//新建一个文件输入流
	if (NULL == pcap_saver)
	{//文件申请失败
		return false;
	}
	pcap_saver->open(path, std::ios_base::out | std::ios_base::binary);

	if (pcap_saver->bad())
	{//文件打开失败
		delete(pcap_saver);
		pcap_saver = NULL;
		MessageBox(NULL, L"新建保存文件失败", L"错误", MB_ICONEXCLAMATION);
		return false;
	}
	//在所有都弄好之后，进行状态的改变
	mode = FILE_SENSOR_READER;
	return true;
}


//取消建立的winpcap连接
void SensorReader::DisSaveConnection()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//建立文件连接
bool SensorReader::BuildFileConnection(const char *path)
{
	//连接之前先取消其他连接
	DisConnection();

	MultiByteToWideChar(CP_ACP, 0, path, -1, pcap_path, 1000);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)StepUpOffsetTable, this, 0, 0);//建立文件索引表
																					//在读取文件之前，等待索引表的建立
	while (0 == GetFrameNumber())
	{//说明还在建立帧索引表中
		Sleep(5);
	}

	pcap_reader = new std::ifstream();//新建一个文件输入流
	if (NULL == pcap_reader)
	{//文件创建失败
		return false;
	}
	pcap_reader->open(path, std::ios_base::in | std::ios_base::binary);

	if (pcap_reader->bad())
	{//文件打开失败
		delete(pcap_reader);
		pcap_reader = NULL;
		return false;
	}

	//最后进行状态的改变
	mode = PCAP_READER;
	SetSpeed(READER_STOP);						//文件一开始的默认速度应该是STOP正向不循环
	return true;
}

//置空索引表,同时文件指针也关闭
void SensorReader::Free_offsetTable()
{
	cur_index = 0;
	if (0 != GetFrameNumber())//如果之前有创建过索引表，先进行释放
	{
		offset_idxTable.index_number = 0;
		delete(offset_idxTable.index_table);
	}
	offset_idxTable.index_table = NULL;
}

//初始化索引表，返回是否创建成功，在没有销毁索引之前是不能再次初始化索引
void SensorReader::Init_offsetTable(std::vector<unsigned long long> &idx_table)
{
	if (0 != GetFrameNumber())
	{//已经建立了索引表
		return;
	}
	//还没有建立索引表
	offset_idxTable.index_table = new unsigned long long[idx_table.size()];
	if (NULL == offset_idxTable.index_table)
	{
		MessageBox(NULL, L"内存不够！", 0, 0);			//---------此处是不是需要退出----------
		return;
	}
	for (int i = 0; i < idx_table.size(); i++)
	{
		offset_idxTable.index_table[i] = idx_table[i];
	}
	cur_index = 0;//没有读取过一帧
	offset_idxTable.index_number = idx_table.size();//先把所有数据赋进去，再修改size
}

//取消当前建立的文件连接
void SensorReader::DisFileConnection()
{
	Free_offsetTable();					//文件索引方面都置空
	if (pcap_reader != NULL)
	{
		pcap_reader->close();				//close文件流
		delete(pcap_reader);
		pcap_reader = NULL;
	}
}

//帧管理的初始化,所有类型都需要的初始化
void SensorReader::Init_Frame()
{
	FullFrame = NULL;
	RTAdress = NULL;					//不管哪个功能都需要用到回调函数
	LastAngle_H = MAXWORD;
	CurrentFrameIndex = 0;
	pSensorFrame = nullptr;				//创建reader的时候帧的确是NULL
}

void SensorReader::Finish_Frame()
{
	pSensorFrame = nullptr;
	CurrentFrameIndex = 0;
	LastAngle_H = MAXWORD;
	FullFrame = NULL;
}

//对回调函数进行绑定注册
void SensorReader::BindFullFrameCallbackFunc(void(*Callback)(std::shared_ptr<SensorFrame> &lParam, LPVOID pParam), LPVOID pParam)
{
	FullFrame = Callback;
	RTAdress = pParam;
}


//建立连接之前先进行取消连接，返回取消是否成功，内部分情况
bool SensorReader::DisConnection()
{
	SetSpeed(READER_STOP);								//速度置停
	switch (mode)
	{
	case PCAP_READER:
		DisFileConnection();
		break;
	case ONLY_SENSOR_READER:
		DisUDPConnection();
		break;
	case FILE_SENSOR_READER:
		DisSaverFileConnection();
		DisUDPConnection();
		break;
	case INVALID_READER:
		break;
	default://其他违规操作进行不正常返回
		return false;
	}
	mode = INVALID_READER;						//模式置无
	return true;
}

//Start的作用是进行每一帧的获取,明确回调函数,内部分情况
void SensorReader::Start()
{
	switch (mode)
	{
	case ONLY_SENSOR_READER:
		SensorStart();
		break;
	case PCAP_READER:
		FileStart();
		break;
	case FILE_SENSOR_READER:
		break;
	default://默认构造函数
		break;
	}
}

//停止帧的播放
void SensorReader::Stop()
{
	switch (mode)
	{
	case ONLY_SENSOR_READER:
		SensorStop();
		break;
	case PCAP_READER:
		FileStop();
		break;
	case FILE_SENSOR_READER:
		//SensorToFile();
		break;
	default://默认构造函数
		break;
	}
}

//退出while(true)循环的函数
void SensorReader::Destory_SensorReader()
{
	destorySensorReader = true;
}

//根据设置的速度进行休眠
void SensorReader::BeginSleep()
{
	if (PCAP_READER == mode)
	{
		switch (speed)
		{
		case READER_STOP:
			while ((READER_STOP == speed) && (!destorySensorReader))//停止的时候一直在此次睡眠，直到速度被改变
			{
				Sleep(1000);
			}
			break;
		case ONE_SPEED:
			Sleep(100); break;
		case DOUBLE_SPEED:
			Sleep(50); break;
		case THREE_SPEED:
			Sleep(33); break;
		case FOUR_SPEED:
			Sleep(25); break;
		case EIGHT_SPEED:
			Sleep(13); break;
		case SIXTEEN_SPEED:
			Sleep(6); break;
		case ONE_POINT_FIVE_SPEED:
			Sleep(67); break;
		case HALF_SPEED:
			Sleep(200); break;
		case QUARTER_SPEED:
			Sleep(400); break;
		case POINT_ONE_SPEED:
			Sleep(1000); break;
		case DEFAULT_SPEED:
			Sleep(100); break;
		default://其他设置非法，播放速度设置成1单位播放
			Sleep(100); break;
		}
	}
	else if (ONLY_SENSOR_READER == mode)
	{
		switch (speed)
		{
		case READER_STOP:
			while ((READER_STOP == speed) && (!destorySensorReader))//停止的时候一直在此次睡眠，直到速度被改变
			{
				Sleep(1000);
			}
			break;
		default://其他设置非法，因为是UDP通信，所以不进行休眠
			break;
		}
	}
	else if (FILE_SENSOR_READER == mode)//传感器显示和保存
	{//网络没有停止这个功能..
	 //..
	}
}



/**************************************************************************
*  @Function Name :: SwitchFrame
*  @Description   :: 判断是否需要结束旧的帧并继续创建新帧
*  @Author        :: zzw
*  @Parameters    :: Angle:新一组数据的水平角
*
*  @Return        :: true:继续处理下去，false:不再进行处理
**************************************************************************/
bool SensorReader::SwitchFrame(WORD Angle, UINT BlockMinPoints, bool newFrame)//新帧是否需要创建先暂时交给newFrame
{
	//在满一帧之后，是否继续读取下面的数据，因为文件中的回放，或者网络的暂停都不需要继续往下处理
	if (MAXWORD == LastAngle_H)
	{
		LastAngle_H = Angle;
	}

	if (Angle < LastAngle_H  //如果角度比前一个小，则认为是新的一帧
		|| pSensorFrame->GetPointsNumber() >= (pSensorFrame->GetMaxPointsInFrame() - BlockMinPoints))//如果Frame不够保存一个BLOCK，也切换帧
	{
		//切换帧先利用线程将旧的帧抛出，根据模式，选择不同的抛出方式
		switch (mode)
		{
		case ONLY_SENSOR_READER:
			FullFrame(pSensorFrame, RTAdress);
			break;
		case PCAP_READER:
			FullFrame(pSensorFrame, RTAdress);
			break;
		case FILE_SENSOR_READER:
			//在建立文件连接之后，还是需要对网口的数据帧进行返回
			FullFrame(pSensorFrame, RTAdress);
			break;
		default://默认构造函数
			break;
		}

		pSensorFrame = nullptr;//之前的指针清除
		LastAngle_H = MAXWORD;

		if (newFrame)//继续创建下一帧
		{
			CurrentFrameIndex++;
			pSensorFrame = std::make_shared<SensorFrame>();
			pSensorFrame->SetFrameIndex(CurrentFrameIndex, GetCurFrameNumber());//设置新帧的下标
			LastAngle_H = Angle;
		}
		return newFrame;
	}
	LastAngle_H = Angle;
	return true;
}

/**************************************************************************
*  @Function Name :: DataParse
*  @Description   :: 数据解析,标志位标记是否创建新帧
*  @Author        ::
*  @Parameters    :: pBuf:数据包地址
*                    dwLen:数据包长度（字节）
*  @Return        :: 本包中解析到数据点的数量
**************************************************************************/
int SensorReader::DataParse(void* pBuf, size_t dwLen, bool newFrame)//newFrame表示新帧之后不再继续创建
{
	HLP16_Block *pBlock = (HLP16_Block*)pBuf;
	bool cur_new_frame = false;//当前新帧标记位
	if (pBlock->Header != BLOCK_HEADER_VELODYNE)
		return 0;
	if (dwLen != 1206)//HLP-16数据包的结构12*（2+2+96），时间标记4字节，工厂字段2字节
		return 0;

	if (nullptr == pSensorFrame)//创建新帧，根据角度信息将老旧帧进行排除
	{
		cur_new_frame = true;
		CurrentFrameIndex++;
		pSensorFrame = std::make_shared<SensorFrame>();
		pSensorFrame->SetFrameIndex(CurrentFrameIndex, GetCurFrameNumber());//设置当前帧的下标
		LastAngle_H = MAXWORD;
	}

	int newFrameIndex = 0;
	//根据cur_new_frame的情况，先找到下面i的起始号newFrameIndex(0-11)是多少
	if (cur_new_frame)
	{//如果是新帧，找到角度最小的那一帧。
		HLP16_Block *pfindBlock = pBlock;
		unsigned short findMinAngle = pfindBlock->Angle;
		pfindBlock++;//因为下面的下标从1开始
		for (int idx = 1; idx < BLOCK_NUM; idx++)
		{
			if (findMinAngle > pfindBlock->Angle)
			{//如果发现前面一个角度大于这个角度，说明到了下一帧,设置标记位，退处
				newFrameIndex = idx;
				for (int i = 0; i < newFrameIndex; i++)//将pBlock也向后移动相同的Block个数
				{
					pBlock++;
				}
				break;
			}
			findMinAngle = pfindBlock->Angle;
			pfindBlock++;
		}
	}

	int nAngleAdd = 0;
	for (int i = newFrameIndex; i < BLOCK_NUM; i++)//从新帧起始处开始
	{
		PointR pr;
		pr.angleH = pBlock->Angle;

		//如果不需要继续处理，则退出
		if (!SwitchFrame(pr.angleH, BLOCK_POINTS, newFrame))
		{//不继续处理,退处本次的数据处理
			return BLOCK_POINTS * (i - newFrameIndex);//先返回得到的点数
		}

		if (i < (BLOCK_NUM - 1))//非最后一组数据与下一组数据一起计算水平角度增量，最后一组数据用前一次增量
		{
			nAngleAdd = (pBlock + 1)->Angle - (pBlock->Angle);
			if (nAngleAdd < 0)
				nAngleAdd += 36000;
			nAngleAdd /= 2;
		}

		for (int j = 0; j < BLOCK_POINTS; j++)
		{
			pr.packID = i;
			pr.channel = j;
			pr.distance = pBlock->Channel[j].Distance * 2ul;
			pr.intensity = pBlock->Channel[j].Intensity;

			if (j == 16) //第16个数据时增加水平角度值
				pr.angleH += (WORD)nAngleAdd;
			if (pr.angleH >= 36000) // 大于360°以后要送去360，但只减一次，以便于检查接收数据是否有误
				pr.angleH -= 36000;

			if (true == pSensorFrame->TestAddPoint())//返回可以加点
			{
				pSensorFrame->AddPoint(pr);
			}
		}
		pBlock++;
	}

	//DWORD *pTimestamp = (DWORD*)(void*)pBlock;
	////	ATLTRACE(_T("Time:%u\n"),*pTimestamp);
	//pTimestamp++;
	//WORD *pFactoryBytes = (WORD*)(void*)pTimestamp;
	////	ATLTRACE(_T("Factory:0x%04X\n"), *pFactoryBytes);

	return BLOCK_POINTS * (BLOCK_NUM - newFrameIndex);
}
