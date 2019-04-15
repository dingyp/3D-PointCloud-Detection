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

//�����Ǹ����ļ�packet��һЩ�ṹ��
namespace DataDefinition
{
#pragma pack(1)

	enum RAWPACKET_API LiDAR_Protocol										//��ȡ�ļ�Э��
	{
		LIDAR_INVALID,														//û���״�
		LIDAR_HLP16,														//16���״�
		LIDAR_2D,															//2D�״�
	};

	typedef struct RAWPACKET_API pcap_packet			//pcapһ�������ļ���ʽ
	{
		pcap_header pcap_head;							//pcap����ͷ��16
		mac_frame_header mac_head;						//macͷ��14
		ip_header ip_head;								//ipͷ��20
		udp_header udp_head;							//udp��ͷ��8
		HLP16_Block block[BLOCK_NUM];					//BLOCK_NUM(12)���飺100*BLOCK_NUM(12)
		tailer tail;									//ʱ����+�����ֽڣ�6
		unsigned char rest[266];						//��̫��֡�1514�����ʱ��������С(1514-1248)����Ϊ���治����pcap��ͷ
	}pcap_packet;

	typedef struct RAWPACKET_API pcapng_packet			//pcapngһ���ļ���ʽ
	{
		pcap_header pcap_head;							//pcap����ͷ��16
		mac_frame_header mac_head;						//macͷ��14
		ip_header ip_head;								//ipͷ��20
		udp_header udp_head;							//udp��ͷ��8
		data_header dataHeader;							//2D���ݰ�ͷ��40
		Block block[PCAPNG_BLOCK_NUM];					//2D���ݿ飺1200
		unsigned char rest[232];						//��̫��֡�1514�����ʱ����������С(1514-1282)����Ϊ���治����pcap��ͷ
	}pcapng_packet;

	typedef union data_packet							//�����壬pcap��pcapng�Ͼ�ֻ��ʹ�������е�һ��
	{
		pcap_packet pcapPacket;							//pcap��
		pcapng_packet pcapngPacket;						//pcapng��
	}data_packet;

	typedef struct RAWPACKET_API raw_packetInfo			//ԭʼһ��pcap���Ĵ��ݽṹ��
	{
		unsigned int size;								//���ṹ�������ݵĶ���sizeof(char)
		char isnewFrame;								//�Ƿ����µ�֡
		unsigned int FileCurFrameIndex;					//��ǰ֡���ļ����ǵڼ�֡
		data_packet pBuff;								//��ǰ�ļ�packet
	}raw_packetInfo;

	typedef std::vector< std::shared_ptr<raw_packetInfo>> PacketList;				//packet����
#pragma pack()
}