#pragma once
#include "stdafx.h"
#include "DataDefinition\PcapStructure.h"
#include "DataDefinition\Packet.h"
#include "DataProcess\RawProcess.h"
#define HAVE_REMOTE
//#include "WinPcap\pcap.h"
//#include "WinPcap\Packet32.h"
#include "SaveReader.h"
#include <WinSock2.h>
using namespace DataReader;

#ifdef DATAREADER_EXPORTS
#define SENSORREADER_API __declspec(dllexport)
#else
#define SENSORREADER_API __declspec(dllimport)
#endif

namespace DataReader
{
	enum SENSORREADER_API SENSORMODE												//传感数据的处理状态
	{
		STOP,																		//暂停
		START																		//开始
	};

	enum SENSORREADER_API SAVEMODE													//文件是否需要写的状态
	{
		NOSAVE,																		//不需要写
		SAVE																		//需要写
	};

	class SENSORREADER_API SensorReader												//传感器读取类（只读取原始buff,处理交给rawprocess）
	{
	public:
		SensorReader();																//设置默认构造函数，因为功能不明确，将什么都不做，等着被析构
		virtual ~SensorReader();
		int Init(DataProcess::RawProcess *pRawProcess, LiDAR_Protocol SetProtocol);	//初始化传感连接（对原始pcap包进行处理）
		void Destory();																//退出传感连接，释放资源
		int SensorOpen(const char *IP, unsigned short dest_port, \
			unsigned short local_port, const char *Mask);							//打开传感器连接
		void SensorClose();															//关闭传感器连接
		int Start();																//开始传感器连接
		int PcapStart();															//开始接收pcap包格式
		int PcapngStart();															//开始接收pcapng包格式
		void ReStart();																//重新开始
		void Stop();																//暂停
		int BeginSleep();															//根据是否暂停播放来决定是否进行睡眠
		int BuildSaverConnection(const char*savepath, void(*SaveEndStop)(LPVOID pParam),\
			LPVOID pParam);															//建立保存文件连接,包括保存文件达到最大的回调函数
		void DisSaverConnection();													//取消保存文件连接
		bool GetIsStart();															//返回线程是否已经开启，方便管理
		//static pcap_if_t *FindSpecDev(pcap_if_t *alldevs, const char *IP,\
		//	const char *Mask);														//获取默认IP的设备的pcap_if_t,并进行返回

	private:
		SaveReader *pcap_saver;														//文件保存流
		std::shared_ptr<DataDefinition::raw_packetInfo> Info;						//每次传出去的pbuff指针
		//pcap_t *adhandle;															//网口窃听助手
		SAVEMODE savemode;															//保存模式标记位
		SENSORMODE Sensormode;														//传感器模式
		bool destorySensorReaderStart;												//退出while循环标志位
		DataProcess::RawProcess *pRawProcess;										//处理原始数据的类
		LiDAR_Protocol Protocol;													//传感器读取处理协议

		int DestIPAddress;															//连接的传感器的IP地址
		int SrcIPAddress;															//本地连接udp连接的IP地址
		unsigned short dest_connect_port;											//目标传感器的端口号
		unsigned short local_connect_port;											//本地传感器的端口号
		SOCKET m_hSocket;//套接字
	};
}