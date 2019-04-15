#pragma once
#include "stdafx.h"

#pragma pack(1)//���ã�ȡ���ṹ����Զ�����

/*pcapʱ���*/
typedef struct  timestamp {
	unsigned int timestamp_s;
	unsigned int timestamp_ms;
}timestamp;

/*pcap��ͷ*/								//size:16
typedef struct pcap_header {
	timestamp ts;
	unsigned int capture_len;
	unsigned int len;

}pcap_header;

/*MAC֡ͷ*/									//size:14
typedef struct mac_frame_header
{
	char DstMacAddress[6];					//Ŀ��mac��ַ
	char SrcMacAddress[6];					//Դmac��ַ
	short Type;								//��һ��Э�����ͣ���0x0800������һ����IPЭ�飬0x0806Ϊarp��0x0835ΪRARP���ݱ�
}mac_frame_header, *pMac_frame_header;

/*IPͷ*/									//size:20
typedef struct ip_header
{
	char VersionAndHeaderLen;				//�汾��Ϣ(ǰ4λ)��ͷ����(��4λ)
	char TypeOfService;						//��������8λ
	short TotalLenOfPacket;					//���ݰ�����
	short PacketID;							//���ݰ���ʶ
	short Sliceinfo;						//��Ƭʹ��
	char TTL;								//���ʱ��
	char TypeOfProtocol;					//Э������
	short CheckSum;							//У���
	unsigned int SourIp;					//Դip
	unsigned int DestIp;					//Ŀ��ip
}ip_header, *pIp_header;

/*UDPͷ*/									//size:8
typedef struct udp_header
{
	unsigned short m_usSourPort;			//Դ�˿ں�16bit
	unsigned short m_usDestPort;			//Ŀ�Ķ˿ں�16bit
	unsigned short m_usLength;				//���ݰ�����16bit
	unsigned short m_usCheckSum;			//У���16bit
}udp_header, *pUdp_header;

#pragma pack()