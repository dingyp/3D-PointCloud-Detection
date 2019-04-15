#pragma once
#include "stdafx.h"
#include <mutex>
#include "DataDefinition\PcapStructure.h"
#include "DataDefinition\RawFrame.h"
#include "DataDefinition\Packet.h"
using namespace DataDefinition;
//������ԭʼ���ݰ��Ĵ���

#ifdef DATAPROCESS_EXPORTS
#define RawProcess_API __declspec(dllexport)
#else
#define RawProcess_API __declspec(dllimport)
#endif

namespace DataProcess
{
	class RawProcess_API RawProcess													//����ԭʼ���ݰ���������֡
	{
	public:
		RawProcess();
		~RawProcess();

		void SetProtocol(LiDAR_Protocol Protocol = LIDAR_HLP16);					//����Э��
		void UnSetProtocol();														//ȡ��Э�������
		void BindFullFrameCallbackFunc(void(*Callback)(std::shared_ptr<RawFrame> &lParam, \
			LPVOID pParam), LPVOID pParam);											//�Իص��������а�ע��,�൱��Init����
		int Start();																//����ԭʼ���ݰ����̺߳���
		void Destory();																//�˳��߳�
		int ProcessAddDataPacket(std::shared_ptr<raw_packetInfo> &pInfo);			//����ļ�����ԭʼ��������
		void AddPcapPacket(std::shared_ptr<raw_packetInfo> &pInfo);					//���pcap����ԭʼ��������
		void AddPcapngPacket(std::shared_ptr<raw_packetInfo> &pInfo);				//���pcapng����ԭʼ������
		int DataProcess(std::shared_ptr<raw_packetInfo> &pInfo);					//����ԭʼ���ݲ���ӵ�ԭʼ֡��,��־λ����Ƿ񴴽���֡
		int PcapPacketProcess(std::shared_ptr<raw_packetInfo> &pInfo);				//����ԭʼpcap������
		int PcapngPacketProcess(std::shared_ptr<raw_packetInfo> &pInfo);			//����ԭʼpcapng������
		int End_ClearFrame();														//����ִ�֡
		int End_TransFrame(bool isWait);											//����ǰ֡��Ϊ��֡�����ͳ�ȥ

	private:
		bool SwitchFrame(unsigned short Angle, unsigned int BlockMinPoints, \
			int FileCurFrameIndex, bool newFrame);									//�ж��Ƿ���Ҫ�����ɵ�֡��������֡

	private:
		unsigned short LastAngle_H;													//��һ�ε�ˮƽ�Ƕ�
		std::shared_ptr<RawFrame> pRawFrame;										//�״�ԭʼ����ָ֡��
		PacketList packet_list;														//�ɼ���ԭʼ���ݴ��������
		unsigned long long ALLFrameIndex;											//�ܹ���ȡ��֡�ĸ����������ֵ��һֱ���ϼ�
		void(*FullFrame)(std::shared_ptr<RawFrame> &lParam, LPVOID pParam);			//��һ֡�Ļص�����
		LPVOID RTAdress;															//���ؾ��
		LiDAR_Protocol Conver_Protocol;												//LiDARת��Э��
		bool destoryRawProcessStart;												//�˳�whileѭ����־λ
		std::mutex mutex_packetList;												//����packetList��
		std::mutex mutex_Frame;														//����֡����
	};
}