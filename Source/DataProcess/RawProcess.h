#pragma once
#include "stdafx.h"
#include <mutex>
#include "DataDefinition\PcapStructure.h"
#include "DataDefinition\RawFrame.h"
#include "DataDefinition\Packet.h"
using namespace DataDefinition;
//包含对原始数据包的处理

#ifdef DATAPROCESS_EXPORTS
#define RawProcess_API __declspec(dllexport)
#else
#define RawProcess_API __declspec(dllimport)
#endif

namespace DataProcess
{
	class RawProcess_API RawProcess													//处理原始数据包，并生成帧
	{
	public:
		RawProcess();
		~RawProcess();

		void SetProtocol(LiDAR_Protocol Protocol = LIDAR_HLP16);					//设置协议
		void UnSetProtocol();														//取消协议的设置
		void BindFullFrameCallbackFunc(void(*Callback)(std::shared_ptr<RawFrame> &lParam, \
			LPVOID pParam), LPVOID pParam);											//对回调函数进行绑定注册,相当于Init函数
		int Start();																//处理原始数据包的线程函数
		void Destory();																//退出线程
		int ProcessAddDataPacket(std::shared_ptr<raw_packetInfo> &pInfo);			//添加文件包到原始包队列中
		void AddPcapPacket(std::shared_ptr<raw_packetInfo> &pInfo);					//添加pcap包到原始包队列中
		void AddPcapngPacket(std::shared_ptr<raw_packetInfo> &pInfo);				//添加pcapng包到原始队列中
		int DataProcess(std::shared_ptr<raw_packetInfo> &pInfo);					//处理原始数据并添加到原始帧中,标志位标记是否创建新帧
		int PcapPacketProcess(std::shared_ptr<raw_packetInfo> &pInfo);				//处理原始pcap包数据
		int PcapngPacketProcess(std::shared_ptr<raw_packetInfo> &pInfo);			//处理原始pcapng包数据
		int End_ClearFrame();														//清空现存帧
		int End_TransFrame(bool isWait);											//将当前帧认为满帧，发送出去

	private:
		bool SwitchFrame(unsigned short Angle, unsigned int BlockMinPoints, \
			int FileCurFrameIndex, bool newFrame);									//判断是否需要结束旧的帧并创建新帧

	private:
		unsigned short LastAngle_H;													//上一次的水平角度
		std::shared_ptr<RawFrame> pRawFrame;										//雷达原始数据帧指针
		PacketList packet_list;														//采集的原始数据待处理队列
		unsigned long long ALLFrameIndex;											//总共读取的帧的个数，这个数值会一直往上加
		void(*FullFrame)(std::shared_ptr<RawFrame> &lParam, LPVOID pParam);			//满一帧的回调函数
		LPVOID RTAdress;															//返回句柄
		LiDAR_Protocol Conver_Protocol;												//LiDAR转换协议
		bool destoryRawProcessStart;												//退出while循环标志位
		std::mutex mutex_packetList;												//操作packetList锁
		std::mutex mutex_Frame;														//操作帧的锁
	};
}