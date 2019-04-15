#pragma once
#include "stdafx.h"
#include "BaseStructure.h"//�����ģ�pcapng����Ҳ�������ļ�

//�ṩ��һЩ����pcap�ļ��еĽṹ����
#define BLOCK_NUM				12			//ÿ��pcap������������
#define BLOCK_POINTS			32			//ÿ�������������ĸ���
#define MAX_ETH_FRAME			1514		//��̫��֡�����
#define BLOCK_HEADER_VELODYNE	0xEEFF		//pcap����ʼ���Ĺ̶��ַ�

#pragma pack(1)								//���ã�ȡ���ṹ����Զ�����

/*MAC+IP+UDP��ͷ*/
typedef struct all_header					//size:42
{
	mac_frame_header mfh;					//14
	ip_header ih;							//20
	udp_header uh;							//8
}all_header;

/*���ݿ�*/									//size:100
typedef struct HLP16_Block
{
	unsigned short Header;					//״̬
	unsigned short Angle;					//��λ��
	struct _POINT_CH						//���ݼ�¼
	{
		unsigned short Distance;			//���ؾ���
		unsigned char Intensity;			//��׼������
	}Channel[BLOCK_POINTS];
}HLP16_Block;

/*ʱ����+�����ֽ�*/						//size:6
typedef struct tailer
{
	unsigned int TimeStamp;					//ʱ����
	unsigned short Factory_Use;				//�����ֽ�
}tailer;

#pragma pack()