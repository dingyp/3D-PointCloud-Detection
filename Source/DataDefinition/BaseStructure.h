#pragma once
#include "stdafx.h"

#pragma pack(1)//作用：取消结构体的自动对齐

/*pcap时间戳*/
typedef struct  timestamp {
	unsigned int timestamp_s;
	unsigned int timestamp_ms;
}timestamp;

/*pcap包头*/								//size:16
typedef struct pcap_header {
	timestamp ts;
	unsigned int capture_len;
	unsigned int len;

}pcap_header;

/*MAC帧头*/									//size:14
typedef struct mac_frame_header
{
	char DstMacAddress[6];					//目的mac地址
	char SrcMacAddress[6];					//源mac地址
	short Type;								//上一层协议类型，如0x0800代表上一层是IP协议，0x0806为arp，0x0835为RARP数据报
}mac_frame_header, *pMac_frame_header;

/*IP头*/									//size:20
typedef struct ip_header
{
	char VersionAndHeaderLen;				//版本信息(前4位)，头长度(后4位)
	char TypeOfService;						//服务类型8位
	short TotalLenOfPacket;					//数据包长度
	short PacketID;							//数据包标识
	short Sliceinfo;						//分片使用
	char TTL;								//存活时间
	char TypeOfProtocol;					//协议类型
	short CheckSum;							//校验和
	unsigned int SourIp;					//源ip
	unsigned int DestIp;					//目的ip
}ip_header, *pIp_header;

/*UDP头*/									//size:8
typedef struct udp_header
{
	unsigned short m_usSourPort;			//源端口号16bit
	unsigned short m_usDestPort;			//目的端口号16bit
	unsigned short m_usLength;				//数据包长度16bit
	unsigned short m_usCheckSum;			//校验和16bit
}udp_header, *pUdp_header;

#pragma pack()