#include "stdafx.h"
#include <fstream>
#include "SensorReader.h"
#include "ErrorManager\ErroCodeDefine.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif  // _WIND32

// 定义64位整形
#if defined(_WIN32) && !defined(CYGWIN)
typedef __int64 int64_t;
#else
typedef long long int64t;
#endif  // _WIN32

// 获取系统的当前时间，单位微秒(us)
int64_t GetSysTimeMicros()
{
#ifdef _WIN32
	// 从1601年1月1日0:0:0:000到1970年1月1日0:0:0:000的时间(单位100ns)
#define EPOCHFILETIME   (116444736000000000UL)
	FILETIME ft;
	LARGE_INTEGER li;
	int64_t tt = 0;
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	// 从1970年1月1日0:0:0:000到现在的微秒数(UTC时间)
	tt = (li.QuadPart - EPOCHFILETIME) / 10;
	return tt;
#else
	timeval tv;
	gettimeofday(&tv, 0);
	return (int64_t)tv.tv_sec * 1000000 + (int64_t)tv.tv_usec;
#endif // _WIN32
	return 0;
}


#define BUFFER_LEN 1514//以太网最长帧
using namespace DataReader;

/**************************************************************************
*  @Function Name :: SensorReader
*  @Description   :: SensorReader类的构造函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
SensorReader::SensorReader()
{
	destorySensorReaderStart = true;
	pcap_saver = NULL;//构造函数先对文件读取类指针置空
	savemode = NOSAVE;
	//adhandle = NULL;
	m_hSocket = INVALID_SOCKET;
	pRawProcess = NULL;
	Info = NULL;
	Protocol = LIDAR_INVALID;
	dest_connect_port = 0;
	local_connect_port = 0;
	READER_DEBUG(_T("正在开启SensorReader"));
}

/**************************************************************************
*  @Function Name :: ~SensorReader
*  @Description   :: SensorReader类的构造函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
SensorReader::~SensorReader()
{
	destorySensorReaderStart = true;
	pcap_saver = NULL;
	savemode = NOSAVE;
	//adhandle = NULL;
	m_hSocket = INVALID_SOCKET;
	pRawProcess = NULL;
	Info = NULL;
	Protocol = LIDAR_INVALID;
	dest_connect_port = 0;
	local_connect_port = 0;
	READER_DEBUG(_T("正在注销SensorReader"));
}

/**************************************************************************
*  @Function Name :: Init
*  @Description   :: 初始化传感连接（对原始pcap包进行处理）,设置了对包处理的协议
*  @Author        :: zzw
*  @Parameters    :: RawProcessPoint:初始化原始数据包处理类
*					 SetProtocol:处理协议
*  @Return        :: 运行是否成功
**************************************************************************/
int SensorReader::Init(DataProcess::RawProcess *RawProcessPoint, LiDAR_Protocol SetProtocol)
{
	if (!destorySensorReaderStart)
	{//初始化应该还没有start
		return ER_noDestorySensorReader;
	}
	if (NULL == RawProcessPoint)
	{//处理类指针为NULL
		return ER_InitPointRawProcess;
	}
	pRawProcess = RawProcessPoint;
	Protocol = SetProtocol;
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: BuildSaverConnection
*  @Description   :: 建立一个保存文件的线程
*  @Author        :: zzw
*  @Parameters    :: savepath:保存文件的路径 SaveEndStop:文件存储太满的回调函数 
*					 pParam:回调函数里面的参数
*  @Return        :: 运行是否成功
**************************************************************************/
int SensorReader::BuildSaverConnection(const char*savepath, void(*SaveEndStop)(LPVOID pParam), LPVOID pParam)
{
	if (LIDAR_INVALID == Protocol)
	{
		return ER_UnSetTheProtocol;
	}
	savemode = SAVE;
	if (NULL != pcap_saver)
	{//已经有保存文件的线程
		return ALReadySaveReader;
	}

	pcap_saver = new SaveReader();
	if (NULL == pcap_saver)
	{
		return ER_BuildSaverNULL;
	}
	pcap_saver->Init();
	pcap_saver->SaveFileOpen(savepath, Protocol, SaveEndStop, pParam);
	pcap_saver->Start();//函数里面开启一个线程
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: DisSaverConnection
*  @Description   :: 取消保存文件连接
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void SensorReader::DisSaverConnection()
{
	savemode = NOSAVE;
	if (NULL == pcap_saver)
	{//保存文件的线程已经没有
		return ;
	}
	pcap_saver->SaveFileClose();
	pcap_saver->Destory();
	delete(pcap_saver);
	pcap_saver = NULL;
	return;
}

/**************************************************************************
*  @Function Name :: Destory
*  @Description   :: 退出传感连接，释放资源
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void SensorReader::Destory()
{
	destorySensorReaderStart = true;//退出while循环
	if (NULL != pcap_saver)
	{//释放
		DisSaverConnection();
	}
	Info = NULL;
	Protocol = LIDAR_INVALID;
	Sleep(10);//这里进行睡眠是为了给Start()退出的时间，不然会报错，很重要
}

/**************************************************************************
*  @Function Name :: GetIsStart
*  @Description   :: 返回线程是否已经开启，方便管理
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 线程是否正在运行
**************************************************************************/
bool SensorReader::GetIsStart()
{
	return !destorySensorReaderStart;
}

/**************************************************************************
*  @Function Name :: Start
*  @Description   :: 开始传感器连接
*  @Author        :: zzw
*  @Parameters    :: None
*					 
*  @Return        :: 运行是否成功
**************************************************************************/
int SensorReader::Start()
{
	READER_DEBUG(_T("雷达读取线程开启"));
	destorySensorReaderStart = false;
	int result= RaView_NO_ERROR;
	if (LIDAR_HLP16 == Protocol)
	{
		result = PcapStart();
	}
	else if (LIDAR_2D == Protocol)
	{
		result = PcapngStart();
	}
	else
	{
		result = ER_UnSureOfProtocol;
	}
	READER_DEBUG(_T("雷达读取线程关闭"));
	return result;
}

/**************************************************************************
*  @Function Name :: PcapStart
*  @Description   :: 开始接收pcap包格式
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
int SensorReader::PcapStart()
{
	READER_DEBUG(_T("开启16线雷达读取"));
	int ErrorDataFormat = 0;//记录错误的数据格式的次数
	int ErrorRecvFrom = 0;//recvfrom接收错误

	SOCKADDR_IN addr_from;
	int n = 0;
	int len = sizeof(SOCKADDR);
	char buf[BUFFER_LEN];

	while (!destorySensorReaderStart)
	{
		if (INVALID_SOCKET!= m_hSocket)
		{
			memset(buf, 0, sizeof(buf));
			n = recvfrom(m_hSocket, buf, BUFFER_LEN, 0, (SOCKADDR*)&addr_from, &len);

			//如果接收到了数据，就进行数据的处理，数据处理包括帧的创建
			if (n>0)
			{
				if (1206 == n)//就单单包括UDP里面的数据，如果大小是正确包的大小则继续处理
				{
					ErrorDataFormat = 0;//之前的错误数据格式忽略不计
					ErrorRecvFrom = 0;
					Info = std::make_shared<DataDefinition::raw_packetInfo>();//拷贝
					if (Info == NULL)				//申请内存失败，退出
					{
						//READER_DEBUG(_T("内存申请失败进行退出"));
						return ER_pBuffNotCreated;
					}
					Info->size = 1206 + 42 + 16;//数据+(mac+ip+udp)+pcap报头
					Info->isnewFrame = 1;
					Info->FileCurFrameIndex = 0;
					//完成对pcap头的生成
					int64_t nowtime = GetSysTimeMicros();//获取系统时间，精确到微妙
					if (0 == nowtime)
					{
						//READER_DEBUG(_T("获取nowtime失败进行退出"));
						return 1;//系统时间未能获取
					}
					Info->pBuff.pcapPacket.pcap_head.ts.timestamp_s = (long)(nowtime / (1000 * 1000));
					Info->pBuff.pcapPacket.pcap_head.ts.timestamp_ms = (long)((nowtime % (1000 * 1000)) / 10);
					Info->pBuff.pcapPacket.pcap_head.capture_len = 1248;
					Info->pBuff.pcapPacket.pcap_head.len = 1248;
					//完成对mac帧头的生成，注意源mac地址此处为默认的0x000000,000000
					memset(&(Info->pBuff.pcapPacket.mac_head.SrcMacAddress), 0, sizeof(Info->pBuff.pcapPacket.mac_head.SrcMacAddress));
					memset(&(Info->pBuff.pcapPacket.mac_head.DstMacAddress), 0xff, sizeof(Info->pBuff.pcapPacket.mac_head.DstMacAddress));
					Info->pBuff.pcapPacket.mac_head.Type = htons(0x0800);//上一层是IP协议,顺序变成UDP
					//完成对IP帧头的生成
					Info->pBuff.pcapPacket.ip_head.VersionAndHeaderLen = 0x45;
					Info->pBuff.pcapPacket.ip_head.TypeOfService = 0x00;
					Info->pBuff.pcapPacket.ip_head.TotalLenOfPacket = htons(1234);//1234,顺序小端存储
					Info->pBuff.pcapPacket.ip_head.PacketID = 0;
					Info->pBuff.pcapPacket.ip_head.Sliceinfo = htons(0x4000);
					Info->pBuff.pcapPacket.ip_head.TTL = 255;
					Info->pBuff.pcapPacket.ip_head.TypeOfProtocol = 17;
					Info->pBuff.pcapPacket.ip_head.CheckSum = htons(0xb4aa);//报头校验和,暂时先设置这个
					Info->pBuff.pcapPacket.ip_head.SourIp = htonl(DestIPAddress);
					Info->pBuff.pcapPacket.ip_head.DestIp = ntohl(inet_addr("255.255.255.255"));
					//完成对UDP枕头的生成
					Info->pBuff.pcapPacket.udp_head.m_usSourPort = htons(dest_connect_port);
					Info->pBuff.pcapPacket.udp_head.m_usDestPort = htons(local_connect_port);
					Info->pBuff.pcapPacket.udp_head.m_usLength = htons(1214);//1206+8
					Info->pBuff.pcapPacket.udp_head.m_usCheckSum = 0x0000;
					//完成对数据的拷贝
					memcpy(&(Info->pBuff.pcapPacket.block), buf, 1206);//数据长度是1206

					if ((NULL != pRawProcess) && (!destorySensorReaderStart))
					{
						pRawProcess->ProcessAddDataPacket(Info);
						//READER_DEBUG(_T("已经向pRawProcess里面发送信息"));
					}
					if ((SAVE == savemode) && (NULL != pcap_saver) && (!destorySensorReaderStart))
					{//是保存模式，且saver可以保存
						pcap_saver->AddPcapPacket(Info);
					}
					Info = NULL;
				}
				else//不一致的数据格式
				{
					ErrorDataFormat++;
				}
			}
			if (-1 == n)//返回失败
			{
				ErrorRecvFrom++;
				Sleep(1);
			}
			n = 0;
		}
		//READER_DEBUG(_T("退出SensorReader_while循环"));
		if (ErrorRecvFrom > 100)
		{
			return ER_OutTimeOfConnect;//超时
		}

		//对一些状态进行判断
		if (ErrorDataFormat > 200)
		{//连续200次的错误数据格式则认为是协议选取不对
			//READER_DEBUG(_T("16线雷达读取出现协议不对的情况"));
			return ER_UnSureOfProtocol;
		}
		BeginSleep();//播放一帧之后进行睡眠
	}
	READER_DEBUG(_T("退出16线雷达读取"));
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: PcapngStart
*  @Description   :: 开始接收pcapng包格式
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
int SensorReader::PcapngStart()
{
	READER_DEBUG(_T("开启2D雷达读取"));
	int ErrorDataFormat = 0;//记录错误的数据格式的次数
	int ErrorRecvFrom = 0;//recvfrom接收错误
	SOCKADDR_IN addr_from;
	int n = 0;
	int len = sizeof(SOCKADDR);
	char buf[BUFFER_LEN];
	while (!destorySensorReaderStart)
	{
		if (INVALID_SOCKET != m_hSocket)
		{
			memset(buf, 0, sizeof(buf));
			n = recvfrom(m_hSocket, buf, BUFFER_LEN, 0, (SOCKADDR*)&addr_from, &len);
			//如果接收到了数据，就进行数据的处理，数据处理包括帧的创建
			if (n>0)
			{
				if (1240 == n)//就单单包括UDP里面的数据，如果大小是正确包的大小则继续处理
				{
					ErrorDataFormat = 0;//之前的错误数据格式忽略不计
					Info = std::make_shared<DataDefinition::raw_packetInfo>();//拷贝
					if (Info == NULL)				//申请内存失败，退出
					{
						return ER_pBuffNotCreated;
					}
					Info->size = 1240 + 42 + 16;//数据+(mac+ip+udp)+pcap报头
					Info->isnewFrame = 1;
					Info->FileCurFrameIndex = 0;
					//完成对pcap头的生成
					int64_t nowtime = GetSysTimeMicros();//获取系统时间，精确到微妙
					if (0 == nowtime)
					{
						return 1;//系统时间未能获取
					}
					Info->pBuff.pcapPacket.pcap_head.ts.timestamp_s = (long)(nowtime / (1000 * 1000));
					Info->pBuff.pcapPacket.pcap_head.ts.timestamp_ms = (long)((nowtime % (1000 * 1000)) / 10);
					Info->pBuff.pcapPacket.pcap_head.capture_len = 1282;
					Info->pBuff.pcapPacket.pcap_head.len = 1282;
					//完成对mac帧头的生成，注意源mac地址此处为默认的0x000000,000000
					memset(&(Info->pBuff.pcapPacket.mac_head.SrcMacAddress), 0, sizeof(Info->pBuff.pcapPacket.mac_head.SrcMacAddress));
					memset(&(Info->pBuff.pcapPacket.mac_head.DstMacAddress), 0xff, sizeof(Info->pBuff.pcapPacket.mac_head.DstMacAddress));
					Info->pBuff.pcapPacket.mac_head.Type = htons(0x0800);//上一层是IP协议
																  //完成对IP帧头的生成
					Info->pBuff.pcapPacket.ip_head.VersionAndHeaderLen = 0x45;
					Info->pBuff.pcapPacket.ip_head.TypeOfService = 0x00;
					Info->pBuff.pcapPacket.ip_head.TotalLenOfPacket = htons(1268);
					Info->pBuff.pcapPacket.ip_head.PacketID = 0;//暂时先是默认值0
					Info->pBuff.pcapPacket.ip_head.Sliceinfo = htons(0x4000);
					Info->pBuff.pcapPacket.ip_head.TTL = 255;
					Info->pBuff.pcapPacket.ip_head.TypeOfProtocol = 17;
					Info->pBuff.pcapPacket.ip_head.CheckSum = htons(0x5d5f);//报头校验和,暂时先设置这个
					Info->pBuff.pcapPacket.ip_head.SourIp = htons(DestIPAddress);
					Info->pBuff.pcapPacket.ip_head.DestIp = htons(SrcIPAddress);//获取本机的ip号
					//完成对UDP枕头的生成
					Info->pBuff.pcapPacket.udp_head.m_usSourPort = htons(dest_connect_port);
					Info->pBuff.pcapPacket.udp_head.m_usDestPort = htons(local_connect_port);
					Info->pBuff.pcapPacket.udp_head.m_usLength = htons(1248);//1240+8
					Info->pBuff.pcapPacket.udp_head.m_usCheckSum = htons(0x0000);
					//完成对数据的拷贝
					memcpy(&(Info->pBuff.pcapPacket.block), buf, 1240);//数据长度是1206

					if ((NULL != pRawProcess) && (!destorySensorReaderStart))
					{
						pRawProcess->ProcessAddDataPacket(Info);
					}
					if ((SAVE == savemode) && (NULL != pcap_saver) && (!destorySensorReaderStart))
					{//是保存模式，且saver可以保存
						pcap_saver->AddPcapPacket(Info);
					}
					Info = NULL;
				}
				else//不一致的数据格式
				{
					ErrorDataFormat++;
				}
			}
			else if (-1 == n)//返回失败
			{
				ErrorRecvFrom++;
				Sleep(1);
			}
			n = 0;
		}

		if (ErrorRecvFrom > 100)
		{
			return ER_OutTimeOfConnect;//超时
		}

		//对一些状态进行判断
		if (ErrorDataFormat > 200)
		{//连续200次的错误数据格式则认为是协议选取不对
			return ER_UnSureOfProtocol;
		}
		BeginSleep();//播放一帧之后进行睡眠
	}
	READER_DEBUG(_T("退出2D雷达读取"));
	return RaView_NO_ERROR;
}

///**************************************************************************
//*  @Function Name :: PcapStart
//*  @Description   :: 开始接收pcap包格式
//*  @Author        :: zzw
//*  @Parameters    :: None
//*
//*  @Return        :: 运行是否成功
//**************************************************************************/
//int SensorReader::PcapStart()
//{
//	int outTimeNumber = 0;//记录读取超时的次数
//	int ErrorDataFormat = 0;//记录错误的数据格式的次数
//	while (!destorySensorReaderStart)
//	{
//		if (NULL != adhandle)						//窃听助手不为空才可以监听
//		{
//			struct pcap_pkthdr *header;
//			const u_char *pkt_data;
//			int res = pcap_next_ex(adhandle, &header, &pkt_data);
//			if (res > 0)							//读取成功
//			{
//				outTimeNumber = 0;//超时部分已经忽略不计
//				if (1248 == header->caplen)			//先判断是否是1248(1206 + 42)
//				{
//					ErrorDataFormat = 0;//之前的错误数据格式忽略不计
//					Info = std::make_shared<DataDefinition::raw_packetInfo>();//拷贝
//					if (Info == NULL)				//申请内存失败，退出
//					{
//						return ER_pBuffNotCreated;
//					}
//					Info->size = 1248 + 16;
//					Info->isnewFrame = 1;
//					Info->FileCurFrameIndex = 0;
//					memcpy(&(Info->pBuff.pcapPacket.pcap_head), header, sizeof(Info->pBuff.pcapPacket.pcap_head));//一样大小的
//					memcpy(&(Info->pBuff.pcapPacket.mac_head), pkt_data, header->caplen);//caplen==1248
//					if ((NULL != pRawProcess) && (!destorySensorReaderStart))
//					{
//						pRawProcess->ProcessAddDataPacket(Info);
//						//pRawProcess->DataProcess(Info);
//					}
//					if ((SAVE == savemode) && (NULL != pcap_saver) && (!destorySensorReaderStart))
//					{//是保存模式，且saver可以保存
//						pcap_saver->AddPcapPacket(Info);
//					}
//					Info = NULL;
//				}
//				else//错误的数据格式
//				{
//					ErrorDataFormat++;
//				}
//			}
//			else if (res == 0)//超时
//			{
//				outTimeNumber++;
//			}
//			else if (res == -1)//读取数据包出错
//			{
//			}
//		}
//		//对一些状态进行判断
//		if (ErrorDataFormat > 200)
//		{//连续200次的错误数据格式则认为是协议选取不对
//			return ER_UnSureOfProtocol;
//		}
//		if (outTimeNumber > 20)
//		{//连续20次的超时则可能是传感器已经脱离了主机，此时需要有回馈
//			return ER_OutTimeOfConnect;
//		}
//		BeginSleep();//播放一帧之后进行睡眠
//	}
//	return RaView_NO_ERROR;
//}
//
///**************************************************************************
//*  @Function Name :: PcapngStart
//*  @Description   :: 开始接收pcapng包格式
//*  @Author        :: zzw
//*  @Parameters    :: None
//*
//*  @Return        :: None
//**************************************************************************/
//int SensorReader::PcapngStart()
//{
//	int outTimeNumber = 0;//记录读取超时的次数
//	int ErrorDataFormat = 0;//记录错误的数据格式的次数
//	while (!destorySensorReaderStart)
//	{
//		if (NULL != adhandle)						//窃听助手不为空才可以监听
//		{
//			struct pcap_pkthdr *header;
//			const u_char *pkt_data;
//			int res = pcap_next_ex(adhandle, &header, &pkt_data);
//			if (res > 0)							//读取成功
//			{
//				outTimeNumber = 0;//超时部分已经忽略不计
//				if (1282 == header->caplen)			//先判断是否是1248(1206 + 42)
//				{
//					ErrorDataFormat = 0;//之前的错误数据格式忽略不计
//					//Info = std::make_shared<DataDefinition::raw_packetInfo>();//拷贝
//					//if (Info == NULL)				//申请内存失败，退出
//					//{
//					//	return ER_pBuffNotCreated;
//					//}
//					//Info->size = 1248 + 16;
//					//Info->isnewFrame = 1;
//					//Info->FileCurFrameIndex = 0;
//					//memcpy(&(Info->pBuff.pcapngPacket.pcap_head), header, sizeof(Info->pBuff.pcapngPacket.pcap_head));//一样大小的
//					//memcpy(&(Info->pBuff.pcapngPacket.mac_head), pkt_data, header->caplen);//caplen==1282
//					//if ((NULL != pRawProcess) && (!destorySensorReaderStart))
//					//{
//					//	pRawProcess->ProcessAddDataPacket(Info);
//					//	//pRawProcess->DataProcess(Info);
//					//}
//					//if ((SAVE == savemode) && (NULL != pcap_saver) && (!destorySensorReaderStart))
//					//{//是保存模式，且saver可以保存
//					//	pcap_saver->AddPcapPacket(Info);
//					//}
//					Info = NULL;
//				}
//				else//错误的数据格式
//				{
//					ErrorDataFormat++;
//				}
//			}
//			else if (res == 0)//超时
//			{
//				outTimeNumber++;
//			}
//			else if (res == -1)//读取数据包出错
//			{
//			}
//		}
//		//对一些状态进行判断
//		if (ErrorDataFormat > 200)
//		{//连续200次的错误数据格式则认为是协议选取不对
//			return ER_UnSureOfProtocol;
//		}
//		if (outTimeNumber > 20)
//		{//连续20次的超时则可能是传感器已经脱离了主机，此时需要有回馈
//			return ER_OutTimeOfConnect;
//		}
//		BeginSleep();//播放一帧之后进行睡眠
//	}
//	return RaView_NO_ERROR;
//}

/**************************************************************************
*  @Function Name :: BeginSleep
*  @Description   :: 根据是否暂停播放来决定是否进行睡眠
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
int SensorReader::BeginSleep()
{
	switch (Sensormode)
	{
	case STOP:
		while ((STOP == Sensormode) && (!destorySensorReaderStart))//停止的时候一直在此次睡眠，直到速度被改变
		{
			Sleep(1);
		}
		break;
	case START:
		break;
	default://其他设置非法，播放速度设置成1单位播放
		Sleep(0); return ER_InvaildSensorMode;
	}
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: ReStart
*  @Description   :: 重新开始传感器读取
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void SensorReader::ReStart()
{
	Sensormode = START;
}

/**************************************************************************
*  @Function Name :: Stop
*  @Description   :: 暂停传感器读取
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void SensorReader::Stop()
{
	Sensormode = STOP;
}

///**************************************************************************
//*  @Function Name :: SensorOpen
//*  @Description   :: 打开传感器连接，使用winpcap对固定端口进行监听
//*  @Author        :: zzw
//*  @Parameters    :: IP:传感器IP port:传感器端口
//*					 Mask:传感器掩码
//*  @Return        :: 运行是否成功
//**************************************************************************/
//int SensorReader::SensorOpen(const char *IP, unsigned short port, const char *Mask)
//{
//	pcap_if_t *alldevs;
//	u_int netmask;
//	struct bpf_program fcode;
//	char errbuf[PCAP_ERRBUF_SIZE];
//
//	char str_netport[20];							//端口字符串最大也不会出现泄漏
//	sprintf_s(str_netport, "%d", port);
//	char packet_filter[100];						//最终差不多的结果"udp and port 2368 and src 192.168.1.100"
//	memset(packet_filter, 0, sizeof(char) * 100);
//	strcat_s(packet_filter,50,"udp and port ");
//	strcat_s(packet_filter, 50, str_netport);
//	strcat_s(packet_filter, 50, " and src ");
//	strcat_s(packet_filter, 50, IP);
//																	 
//	if (pcap_findalldevs_ex((char *)PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)//获列表出错
//	{
//		return ER_PCAP_FindAllDevs;
//	}
//	pcap_if_t *d = FindSpecDev(alldevs, IP, Mask);	//获取默认IP的设备的pcap_if_t,并返回
//	if (NULL == d)									//寻找特定设备失败
//	{
//		pcap_freealldevs(alldevs);
//		return ER_NotFindSpecDev;
//	}
//
//	//打开适配器(适配器名称、捕获全部内容、混杂模式、超时时间、远程机器验证、错误缓冲池)
//	adhandle = pcap_open(d->name,65536,PCAP_OPENFLAG_PROMISCUOUS,1000,NULL,errbuf);
//
//	if (pcap_datalink(adhandle) != DLT_EN10MB)		//程序只在以太网工作
//	{
//		pcap_freealldevs(alldevs);
//		adhandle = NULL;
//		return ER_OnlyWorkInEthernet;
//	}
//
//	if (d->addresses != NULL)						//获得接口第一个地址的掩码
//	{
//		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
//	}
//	else //如果接口没有地址，那么我们假设一个C类的掩码
//	{
//		netmask = 0xffffff;
//	}
//
//	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) <0)//编译过滤器
//	{//无法编译，检查语法
//		pcap_freealldevs(alldevs);
//		adhandle = NULL;
//		return ER_CannotCompileInPcap;
//	}
//
//	if (pcap_setfilter(adhandle, &fcode)<0)			//设置过滤器
//	{
//		pcap_freealldevs(alldevs);
//		adhandle = NULL;
//		return ER_CannotSetFilterInPcap;
//	}
//
//	//在此时，如果传感器都没有传来一个UDP报，就返回没有能够正确连接传感器的警告
//	int iddx1 = 0;//记录读取成功的次数
//	int iddx2 = 0;//记录读取超时的次数
//	while (TRUE)
//	{
//		if (NULL != adhandle)						//窃听助手不为空才可以监听
//		{
//			struct pcap_pkthdr *header;
//			const u_char *pkt_data;
//			int res = pcap_next_ex(adhandle, &header, &pkt_data);
//			if ((res > 0))							//读取成功
//			{
//				iddx1++;
//			}
//			else if (res == 0)						//超过10次超时
//			{
//				iddx2++;
//			}
//			else if (res == -1)						//读取数据包出错
//			{
//			}
//		}
//		if (iddx1 >= 6)
//		{
//			break;
//		}
//		if (iddx2 >= 6)
//		{
//			pcap_freealldevs(alldevs);
//			adhandle = NULL;
//			return ER_OutTimeOfConnect;				//窃听超时
//		}
//	}
//
//	pcap_freealldevs(alldevs);
//	return RaView_NO_ERROR;
//}

/**************************************************************************
*  @Function Name :: SensorOpen
*  @Description   :: 打开传感器连接，使用使用UDP进行连接
*  @Author        :: zzw
*  @Parameters    :: IP:传感器IP port:传感器端口
*					 Mask:传感器掩码
*  @Return        :: 运行是否成功
**************************************************************************/
int SensorReader::SensorOpen(const char *IP, unsigned short dest_port, unsigned short local_port, const char *Mask)
{
	SOCKADDR_IN m_addr_local;
	//SOCKADDR_IN m_addr_dest;

	WORD wVersionRequested;
	WSADATA wsaData;

	//DestIPAddress = ntohl(inet_addr(IP));
	//dest_connect_port = dest_port;
	local_connect_port = local_port;
	wVersionRequested = MAKEWORD(2, 2);

	//当要求的版本(Winsock的最高版本)等与或高于系统支持的最底版本(下限),那么该函数   
	//操作成功 并且在WSADATA.WhighVersion中返回系统支持的最高版本,在WSADATA.wVersion中   
	//返回系统支持的最高版(上限)和 wVersionRequested 之间的较小值。
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return 1;//Windows Sockets 库初始化失败.
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return 1;
	}

	//创建udp 的套接字
	m_hSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//m_hSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_hSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return 1;//创建socket失败
	}
	int nRecvBufSize;
	int nLen = sizeof(int);

	//设置套接字的某个选项，
	getsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nRecvBufSize, &nLen);
	if (nRecvBufSize < 64 * BUFFER_LEN)
	{
		int nRecvBufSizeNew = 64 * BUFFER_LEN;
		setsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (char *)&nRecvBufSizeNew, sizeof(int));//设置缓冲区的大小
	}

	//timeval tv_out;
	//tv_out.tv_sec = 3;//等待3秒
	//tv_out.tv_usec = 0;
	//setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv_out, sizeof(tv_out));

	//m_addr_dest.sin_family = AF_INET;
	//m_addr_dest.sin_addr.S_un.S_addr = htonl(DestIPAddress);
	////m_addr_dest.sin_addr.s_addr = htonl(DestIPAddress);
	////m_addr_dest.sin_port = htons(connect_port);
	//m_addr_dest.sin_port = htons(dest_connect_port);

	m_addr_local.sin_family = AF_INET;
	m_addr_local.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//m_addr_local.sin_addr.S_un.S_addr = inet_addr("192.168.1.10");
	//m_addr_local.sin_addr.s_addr = htonl(INADDR_ANY);
	m_addr_local.sin_port = htons(local_connect_port);//绑定一个本地指定端口

	err = bind(m_hSocket, (SOCKADDR*)&m_addr_local, sizeof(SOCKADDR));//绑定一个本地端口，sendto的时候从local端口发送到dest端口
	if (err != 0)
	{
		closesocket(m_hSocket);
		WSACleanup();
		return 1;
	}
	////获取本机的udp通信的ip号
	//err = connect(m_hSocket, (struct sockaddr *)&DestIPAddress, sizeof(DestIPAddress));
	//if (err != 0) {
	//	closesocket(m_hSocket);
	//	WSACleanup();
	//	return 1;
	//}
	//int address_len = sizeof(SrcIPAddress);
	//getsockname(m_hSocket, (struct sockaddr *)&SrcIPAddress, &address_len);//获取udp通信的本机ip地址
	//int a = address_len;
	return RaView_NO_ERROR;
}

///**************************************************************************
//*  @Function Name :: SensorClose
//*  @Description   :: 关闭传感器连接
//*  @Author        :: zzw
//*  @Parameters    :: None
//*
//*  @Return        :: None
//**************************************************************************/
//void SensorReader::SensorClose()
//{//只是监听，只需要把这个监听接口置空吧
//	adhandle = NULL;
//}

/**************************************************************************
*  @Function Name :: SensorClose
*  @Description   :: 关闭传感器连接
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void SensorReader::SensorClose()
{//只是监听，只需要把这个监听接口置空吧
	if (INVALID_SOCKET != m_hSocket) {
		closesocket(m_hSocket);
	}
	m_hSocket = INVALID_SOCKET;
	WSACleanup();
}

///**************************************************************************
//*  @Function Name :: FindSpecDev
//*  @Description   :: 获取默认IP的设备的pcap_if_t,并进行返回,返回NULL说明失败
//*  @Author        :: zzw
//*  @Parameters    :: alldevs:指向所有可连网口设备的指针 IP:需要连接的ip地址
//*					 Mask:需要连接的掩码
//*  @Return        :: None
//**************************************************************************/
//pcap_if_t *SensorReader::FindSpecDev(pcap_if_t *alldevs, const char *IP, const char *Mask)
//{
//	pcap_if_t *d;
//	unsigned long To_Connect_IP = inet_addr(IP);
//	unsigned long Default_gateway = inet_addr(Mask); //如果接口没有地址，那么假设是一个C类掩码
//	for (d = alldevs; d; d = d->next)
//	{//对设备一个一个检查IP
//		pcap_addr_t *a = NULL;
//
//		/* IP addresses */
//		for (a = d->addresses; a; a = a->next) {
//			if (AF_INET == a->addr->sa_family)
//			{
//				if (a->addr)
//				{//如果地址存在的话
//					unsigned long ul_ip = ((struct sockaddr_in *)a->addr)->sin_addr.s_addr;
//					if (a->netmask)
//					{//如果网关存在，使用网关
//						unsigned long ul_gateway = ((struct sockaddr_in *)a->netmask)->sin_addr.s_addr;
//						if ((To_Connect_IP & ul_gateway) == (ul_ip & ul_gateway))
//						{
//							return d;
//						}
//					}
//					else
//					{//使用默认的255.255.255.0
//						if ((To_Connect_IP & Default_gateway) == (ul_ip & Default_gateway))
//						{//确定是在同一个网关之下
//							return d;
//						}
//					}
//				}
//			}
//		}
//	}
//	//寻找失败
//	return NULL;
//}
