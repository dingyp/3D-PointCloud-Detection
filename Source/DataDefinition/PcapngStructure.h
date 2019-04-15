#pragma once
#include "stdafx.h"
#include "BaseStructure.h"//�����ģ�pcap����Ҳ�������ļ�
#define PCAPNG_BLOCK_NUM				150
#define PCAPNG_HEADER_SIGN				0xFEF0010F

//�ÿ������п��ܹ�����ǿ��ת��
#pragma pack(1)

typedef struct BlockHead {
	unsigned int BlockType;
	unsigned int BlockLen;
}BlockHead;

/*�ֽڿ飨ǿ�ƣ�ʣ�ಿ�� 0x0a0d0d0a*/
typedef struct SHB {
	unsigned int Order;
	unsigned short Major_Version;
	unsigned short Minor_Version;
	unsigned int Section_length[2];
}SHB;

/*�ӿ������飨ǿ�ƣ�ʣ�ಿ�� 0x00000001*/
typedef struct IDB {
	unsigned short LinkType;
	unsigned short Reserved;
	unsigned int SnapLen;
}IDB;

/*��ǿ���Ŀ�ʣ�ಿ�� 0x00000006*/
typedef struct EPB {
	unsigned int Interface_ID;
	unsigned int Timestamp_high;
	unsigned int Timestamp_low;
	unsigned int CapLen;
	unsigned int PacketLen;
}EPB;

/*�򵥱��Ŀ�ʣ�ಿ�� 0x00000003*/
typedef struct SPB {
	unsigned int PacketLen;
}SPB;

/*���ƽ�����ʣ�ಿ�� 0x00000004*/
typedef struct NRB {
	unsigned short Record_Type;
	unsigned short Record_Len;
	//....
};

/*�ӿ�ͳ�ƿ�ʣ�ಿ�� 0x00000005*/
typedef struct ISB {
	unsigned int Interface_ID;
	unsigned int Timestamp_high;
	unsigned int Timpstamp_low;
}ISB;

/*���ݵı�ͷ  Header ����*/
typedef struct data_header {
	unsigned int sign;								//��ʶ�����̶�ΪPCAPNG_HEADER_SIGN(0xFEF0010F)
	unsigned short protocol;						//Э��汾����ǰ
	unsigned char ratio_length;						//���������ʵ�ʾ���ֵ=�������ݡ������� (mm)
	unsigned char brand[3];							//Ʒ���̴��룬��д��ĸ�����ֱ�ʾ�����Ȳ���ĩβ����\0��OLE��ŷ�أ� HDL������ƽݣ� GST�����ǣ� GZR�����Ի�����
	unsigned char type[12];							//�����ͺ��ַ������ַ����ԡ�\0����β
	unsigned short intra_type;						//�ڲ��ͺŴ���
	unsigned short version_hard;					//Ӳ���汾
	unsigned short version_soft;					//����汾
	unsigned int timestamp;							//ʱ���������������λΪ ms����ʾ 24 Сʱ�ڵ�ʱ���֡��롢����
	unsigned char system_status[8];					//ϵͳ״̬�루��ϸ���������
}data_header;

/*���ݿ鶨��*/
typedef struct Block {
	unsigned short angle;							//�Ƕȣ��޷�����������Ч��Χ��0~35999
													//��ʾ��λ 0.01�� / LSB����ʾ��Χ 0��~359.99��
													//ע������ֵ������� 0xFF00�����ʾ�����ݿ���Ч���������

	unsigned short close_range;						//�����룬�޷���������
													//��ʾ�����ɰ�ͷ�ľ������������������ȡ����ֵ����ͷ�ľ�
													//��������õ�ʵ�ʾ��루��λ��mm����
													//0xFF00~0xFFFF ��������ʾ�������塣
													//0xFFFF�����������Ч��
													//0xFFFE�����볬�����̡�
													//0xFFFD���������������ä����

	unsigned char close_intensity;					//�����뷴���ʣ���ʾ��Χ 0~100��128 �����ϵ�ֵ��ʾ�߷�����Ŀ��
	unsigned short far_range;						//Զ���룬����ͬ�����뷴���ʡ����ڵ�����Ŀ�꣬������ 0
	unsigned char far_intensity;					//Զ���뷴���ʣ�����ͬ�����뷴���ʡ����ڵ�����Ŀ�꣬������ 0
}Block;

#pragma pack()