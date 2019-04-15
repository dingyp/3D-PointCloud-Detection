#pragma once
#include "stdafx.h"
#include <fstream>
#include <mutex>
#include "DataDefinition\Packet.h"

using namespace DataDefinition;

#ifdef DATAREADER_EXPORTS
#define SAVEREADER_API __declspec(dllexport)
#else
#define SAVEREADER_API __declspec(dllimport)
#endif

namespace DataReader
{
	class SaveReader																			//负责对传进来的packet的文件写
	{
	public:
		SaveReader();
		~SaveReader();
		int Init();																				//文件保存的初始化
		int Start();																			//开启一个线程进行等待写进文件
		void Destory();																			//退出文件保存连接，释放资源
		int SaveFileOpen(const char* savepath, LiDAR_Protocol SetProtocol, \
			void(*SaveEndStop)(LPVOID pParam), LPVOID pParam);									//打开保存文件连接,包括文件存储太满的回调函数
		int SaveFileClose();																	//关闭保存文件连接
		int AddPcapPacket(std::shared_ptr<DataDefinition::raw_packetInfo> &pInfo);				//将原始pcap包添加到存储队列中
	private:
		static void BeginDataSave(LPVOID pParam);												//文件写线程函数
	private:
		void(*FullSaveCallBack)(LPVOID pParam);															//文件存储太满的回调函数
		LPVOID RTAdress;																		//回调函数的参数
		unsigned int savepcapNumber;
		std::ofstream *pcap_saver;																//保存文件流
		bool destorySaveReader;																	//退出文件保存线程标志位
		DataDefinition::PacketList savePacket_list;												//文件保存pcap包队列
		std::mutex mutex_savePacketList;														//操作savePacket_list锁
		std::mutex mutex_saveReader;															//写文件的互斥锁
		HANDLE beginSaveID;																		//开始的线程ID号
		LiDAR_Protocol Protocol;																//文件的保存协议
	};
}