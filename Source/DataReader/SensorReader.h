#pragma once
#include "stdafx.h"
#include "DataDefinition\PcapStructure.h"
#include "DataDefinition\Packet.h"
#include "DataProcess\RawProcess.h"
#define HAVE_REMOTE
//#include "WinPcap\pcap.h"
//#include "WinPcap\Packet32.h"
#include "SaveReader.h"
#include <WinSock2.h>
using namespace DataReader;

#ifdef DATAREADER_EXPORTS
#define SENSORREADER_API __declspec(dllexport)
#else
#define SENSORREADER_API __declspec(dllimport)
#endif

namespace DataReader
{
	enum SENSORREADER_API SENSORMODE												//�������ݵĴ���״̬
	{
		STOP,																		//��ͣ
		START																		//��ʼ
	};

	enum SENSORREADER_API SAVEMODE													//�ļ��Ƿ���Ҫд��״̬
	{
		NOSAVE,																		//����Ҫд
		SAVE																		//��Ҫд
	};

	class SENSORREADER_API SensorReader												//��������ȡ�ֻࣨ��ȡԭʼbuff,������rawprocess��
	{
	public:
		SensorReader();																//����Ĭ�Ϲ��캯������Ϊ���ܲ���ȷ����ʲô�����������ű�����
		virtual ~SensorReader();
		int Init(DataProcess::RawProcess *pRawProcess, LiDAR_Protocol SetProtocol);	//��ʼ���������ӣ���ԭʼpcap�����д���
		void Destory();																//�˳��������ӣ��ͷ���Դ
		int SensorOpen(const char *IP, unsigned short dest_port, \
			unsigned short local_port, const char *Mask);							//�򿪴���������
		void SensorClose();															//�رմ���������
		int Start();																//��ʼ����������
		int PcapStart();															//��ʼ����pcap����ʽ
		int PcapngStart();															//��ʼ����pcapng����ʽ
		void ReStart();																//���¿�ʼ
		void Stop();																//��ͣ
		int BeginSleep();															//�����Ƿ���ͣ�����������Ƿ����˯��
		int BuildSaverConnection(const char*savepath, void(*SaveEndStop)(LPVOID pParam),\
			LPVOID pParam);															//���������ļ�����,���������ļ��ﵽ���Ļص�����
		void DisSaverConnection();													//ȡ�������ļ�����
		bool GetIsStart();															//�����߳��Ƿ��Ѿ��������������
		//static pcap_if_t *FindSpecDev(pcap_if_t *alldevs, const char *IP,\
		//	const char *Mask);														//��ȡĬ��IP���豸��pcap_if_t,�����з���

	private:
		SaveReader *pcap_saver;														//�ļ�������
		std::shared_ptr<DataDefinition::raw_packetInfo> Info;						//ÿ�δ���ȥ��pbuffָ��
		//pcap_t *adhandle;															//������������
		SAVEMODE savemode;															//����ģʽ���λ
		SENSORMODE Sensormode;														//������ģʽ
		bool destorySensorReaderStart;												//�˳�whileѭ����־λ
		DataProcess::RawProcess *pRawProcess;										//����ԭʼ���ݵ���
		LiDAR_Protocol Protocol;													//��������ȡ����Э��

		int DestIPAddress;															//���ӵĴ�������IP��ַ
		int SrcIPAddress;															//��������udp���ӵ�IP��ַ
		unsigned short dest_connect_port;											//Ŀ�괫�����Ķ˿ں�
		unsigned short local_connect_port;											//���ش������Ķ˿ں�
		SOCKET m_hSocket;//�׽���
	};
}