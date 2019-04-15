#pragma once
#include "stdafx.h"
#include <vector>
#include <memory> //shared_ptr
#include "PcapStructure.h"
#include "PcapngStructure.h"

#ifdef DATADEFINITION_EXPORTS
#define RAWPACKET_API __declspec(dllexport)
#else
#define RAWPACKET_API __declspec(dllimport)
#endif

//里面是各种文件packet的一些结构体
namespace DataDefinition
{
#pragma pack(1)

	enum RAWPACKET_API LiDAR_Protocol										//读取文件协议
	{
		LIDAR_INVALID,														//没有雷达
		LIDAR_HLP16,														//16列雷达
		LIDAR_2D,															//2D雷达
	};

	typedef struct RAWPACKET_API pcap_packet			//pcap一个包的文件格式
	{
		pcap_header pcap_head;							//pcap包的头：16
		mac_frame_header mac_head;						//mac头：14
		ip_header ip_head;								//ip头：20
		udp_header udp_head;							//udp报头：8
		HLP16_Block block[BLOCK_NUM];					//BLOCK_NUM(12)个块：100*BLOCK_NUM(12)
		tailer tail;									//时间标记+工厂字节：6
		unsigned char rest[266];						//以太网帧最长1514，这个时填充数组大小(1514-1248)，因为里面不包括pcap包头
	}pcap_packet;

	typedef struct RAWPACKET_API pcapng_packet			//pcapng一个文件格式
	{
		pcap_header pcap_head;							//pcap包的头：16
		mac_frame_header mac_head;						//mac头：14
		ip_header ip_head;								//ip头：20
		udp_header udp_head;							//udp报头：8
		data_header dataHeader;							//2D数据包头：40
		Block block[PCAPNG_BLOCK_NUM];					//2D数据块：1200
		unsigned char rest[232];						//以太网帧最长1514，这个时候填充数组大小(1514-1282)，因为里面不包括pcap包头
	}pcapng_packet;

	typedef union data_packet							//联合体，pcap和pcapng毕竟只能使用里面中的一个
	{
		pcap_packet pcapPacket;							//pcap包
		pcapng_packet pcapngPacket;						//pcapng包
	}data_packet;

	typedef struct RAWPACKET_API raw_packetInfo			//原始一个pcap包的传递结构体
	{
		unsigned int size;								//本结构体中数据的多少sizeof(char)
		char isnewFrame;								//是否是新的帧
		unsigned int FileCurFrameIndex;					//当前帧在文件中是第几帧
		data_packet pBuff;								//当前文件packet
	}raw_packetInfo;

	typedef std::vector< std::shared_ptr<raw_packetInfo>> PacketList;				//packet队列
#pragma pack()
}