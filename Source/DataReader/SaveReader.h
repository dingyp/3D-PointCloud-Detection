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
	class SaveReader																			//����Դ�������packet���ļ�д
	{
	public:
		SaveReader();
		~SaveReader();
		int Init();																				//�ļ�����ĳ�ʼ��
		int Start();																			//����һ���߳̽��еȴ�д���ļ�
		void Destory();																			//�˳��ļ��������ӣ��ͷ���Դ
		int SaveFileOpen(const char* savepath, LiDAR_Protocol SetProtocol, \
			void(*SaveEndStop)(LPVOID pParam), LPVOID pParam);									//�򿪱����ļ�����,�����ļ��洢̫���Ļص�����
		int SaveFileClose();																	//�رձ����ļ�����
		int AddPcapPacket(std::shared_ptr<DataDefinition::raw_packetInfo> &pInfo);				//��ԭʼpcap����ӵ��洢������
	private:
		static void BeginDataSave(LPVOID pParam);												//�ļ�д�̺߳���
	private:
		void(*FullSaveCallBack)(LPVOID pParam);															//�ļ��洢̫���Ļص�����
		LPVOID RTAdress;																		//�ص������Ĳ���
		unsigned int savepcapNumber;
		std::ofstream *pcap_saver;																//�����ļ���
		bool destorySaveReader;																	//�˳��ļ������̱߳�־λ
		DataDefinition::PacketList savePacket_list;												//�ļ�����pcap������
		std::mutex mutex_savePacketList;														//����savePacket_list��
		std::mutex mutex_saveReader;															//д�ļ��Ļ�����
		HANDLE beginSaveID;																		//��ʼ���߳�ID��
		LiDAR_Protocol Protocol;																//�ļ��ı���Э��
	};
}