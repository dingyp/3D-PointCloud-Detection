#pragma once
#include "stdafx.h"
#include <memory>
#include <mutex>
#include <mmsystem.h>
#include "DataDefinition\Packet.h"
#include "DataProcess\RawProcess.h"

#ifdef DATAREADER_EXPORTS
#define FILEREADER_API __declspec(dllexport)
#else
#define FILEREADER_API __declspec(dllimport)
#endif

namespace DataReader
{
	enum FILEREADER_API SPEED
	{																		//֡�Ĳ����ٶ�ö������
		READER_STOP,														//0�����ٶȣ�ֹͣ
		ONE_SPEED,															//1��λ���ٶ�==Ĭ�ϲ����ٶ�
		DOUBLE_SPEED,														//2��λ���ٶ�
		THREE_SPEED,														//3��λ���ٶ�
		FOUR_SPEED,															//4��λ���ٶ�
		EIGHT_SPEED,														//8��λ���ٶ�
		SIXTEEN_SPEED,														//16��λ���ٶ�
		ONE_POINT_FIVE_SPEED,												//1.5��λ���ٶ�
		HALF_SPEED,															//0.5��λ���ٶ�
		ONE_THRID_SPEED,													//0.33��λ���ٶ�
		QUARTER_SPEED,														//0.25��λ���ٶ�
		POINT_ONE_SPEED,													//0.1��λ���ٶ�
		DEFAULT_SPEED,														//Ĭ�ϲ����ٶ�==1��λ���ٶ�
	};

	enum FILEREADER_API PLAYTYPE											//֡�Ĳ���˳��
	{
		FORWORD,															//���򲥷�
		BACKWORD															//���˲���
	};

	enum FILEREADER_API PLAYLOOP											//�Ƿ�ѭ�����ŵ�ö������
	{
		NOLOOP,																//��ѭ������
		LOOP																//ѭ������
	};

	typedef struct FILEREADER_API offset_table								//����һ�������Ľṹ��
	{
		unsigned long long index_number;									//�ܵ�������Ŀ,0����û��ʼ��������
		unsigned long long *index_table;									//������
	}offset_table;

	class FILEREADER_API FileReader											//���ļ����в�������
	{
	public:
		FileReader();
		~FileReader();
		int Init(DataProcess::RawProcess *pRawProcess, void(*PlayEndStop)(\
			LPVOID pParam), LPVOID pParam, LiDAR_Protocol SetProtocol);		//��ʼ�����ŵ�ֹͣ�Ļص�����
		void Destory();														//�˳��ļ���ȡ�̣߳��ͷ���Դ
		bool GetIsStart();													//�鿴���߳��Ƿ��Ѿ�����
		int FileOpen(const char *filepath);									//����pcap�ļ�
		int FileClose();													//�ر�pcap�ļ�����
		int Start(int status);												//����pcap�ļ���ȡ�߳�
		void ReStart(int status = 0);										//���¿�����ȡ
		void Stop();														//��ͣ�ļ��Ķ�ȡ
		void PlayForward();													//���򲥷�
		void PlayBackward();												//���˲���
		PLAYTYPE GetPlayType();												//�õ���������
		void Loop();														//��ʼѭ������
		void EndLoop();														//����ѭ������
		int SetSpeed(const SPEED set_speed= DEFAULT_SPEED);					//�����ٶ�
		SPEED ACCSpeed();													//�ı��ٶȣ��ȼӿ�������
		//void DECSpeed();													//�����ٶ�
		void SetPlayState(const SPEED set_speed = DEFAULT_SPEED, \
			const PLAYTYPE set_type = FORWORD, const PLAYLOOP set_loop = NOLOOP);//���ò���״̬
		void RessetState();													//���ò����ٶ�ΪĬ�ϲ����ٶ�
		//int GetSpeed();													//�õ���ǰ�Ĳ����ٶ�
		int BeginSleep();													//�������õ��ٶȽ�������
		int GetFrameNumber();												//�õ��ļ����ܵ�֡��
		unsigned long long GetCurFrameNumber();								//�õ���ǰ֡���
		void SetFrameNumber(unsigned int frameNumber);						//��������ĳһ֡
		int GetNextFrame(bool reDraw);										//�õ���һ֡,�����Ƿ�ɹ�
		int GetBackwardFrame(bool reDraw);									//�õ���һ֡,reDraw�Ƿ�����ͣ��Ӱ��
		int GoToFrame(const int FrameNumber, bool reDraw);					//���������ĳһ֡
		void ReadPcapPacket();												//��ȡһ��pcap��
		void ReadPcapngPacket();											//��ȡһ��pcapng��
		int GoToStart();													//�ص�һ��ʼ
		int GoToEnd();														//�Ƶ����
	private:
		static void CALLBACK CallBackMilliSecondProc(UINT wTimerID, UINT msg, \
			DWORD_PTR dwUser, DWORD dw1, DWORD dw2);						//�ص�����
		int Init_offsetTable(std::vector<unsigned long long> &idx_table);	//��ʼ��������ֻ���޸�һ��
		static void StepUpOffsetTable(LPVOID pParam);						//����������
		void StepUpPcapOffsetTable();										//����pcap������
		void StepUpPcapngOffsetTable();										//����pcapng������
		void Free_offsetTable();											//�ÿ�������
	private:
		void(*PlayStop)(LPVOID pParam);										//���������һ֡�Ļص�����
		std::ifstream *file_reader;											//��ȡ�ļ�ָ��
		std::shared_ptr<DataDefinition::raw_packetInfo> Info;				//ÿ�δ�����pcap����Ϣָ��
		SPEED speed;														//�����ٶ�(��״̬�Լ���������״̬����Ӱ��)
		PLAYTYPE playType;													//��������
		PLAYLOOP loop;														//ѭ������λ������Ƿ�ѭ������
		LPVOID RTAdress;													//���ؾ��
		MMRESULT g_uTimerID;												//�����ٶȵĶ�ý���ʱ��ID
		int remain_number;													//��ý���ʱ����ʣ���ȡ����
		int add_number;														//һ�����ӵĴ����������ٶ�
		offset_table *offset_idxTable;										//֡������,ָ��ĳһ֡���ļ��еľ���λ��
		bool isOffsetOK;													//�������Ƿ��ܹ�������־λ
		unsigned long long cur_index;										//��ǰ�����ļ�������
		bool destoryFileReader;												//�˳��̱߳�־λ
		DataProcess::RawProcess *pRawProcess;								//����ԭʼ���ݵ���
		bool reDraw;														//�Ƿ����»��Ʊ�־λ
		std::mutex mutex_FileReader;										//�ļ���ȡ�Ļ������
		LiDAR_Protocol Protocol;											//�ļ���ȡЭ��
	};
}