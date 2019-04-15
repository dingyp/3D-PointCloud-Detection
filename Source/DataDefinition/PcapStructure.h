#pragma once
#include "stdafx.h"
#include "BaseStructure.h"//基础的，pcapng里面也包含的文件

//提供了一些关于pcap文件中的结构定义
#define BLOCK_NUM				12			//每个pcap包包含几个块
#define BLOCK_POINTS			32			//每个块里面包含点的个数
#define MAX_ETH_FRAME			1514		//以太网帧最长长度
#define BLOCK_HEADER_VELODYNE	0xEEFF		//pcap包起始处的固定字符

#pragma pack(1)								//作用：取消结构体的自动对齐

/*MAC+IP+UDP报头*/
typedef struct all_header					//size:42
{
	mac_frame_header mfh;					//14
	ip_header ih;							//20
	udp_header uh;							//8
}all_header;

/*数据块*/									//size:100
typedef struct HLP16_Block
{
	unsigned short Header;					//状态
	unsigned short Angle;					//方位角
	struct _POINT_CH						//数据记录
	{
		unsigned short Distance;			//返回距离
		unsigned char Intensity;			//标准反射率
	}Channel[BLOCK_POINTS];
}HLP16_Block;

/*时间标记+工厂字节*/						//size:6
typedef struct tailer
{
	unsigned int TimeStamp;					//时间标记
	unsigned short Factory_Use;				//工厂字节
}tailer;

#pragma pack()