#pragma once
#include "stdafx.h"
#include "DataProcess\RawProcess.h"
#include "SensorReader.h"
#include "FileReader.h"
#include "csvReader.h"
#include "binReader.h"
//�������е�Reader�����࣬����Ĺ���ֻ�Ƕ�ÿ��Reader������Ӧ�Ĺ������������κ���Reader��صĹ���
using namespace DataReader;

#ifdef DATAREADER_EXPORTS
#define READER_API __declspec(dllexport)
#else
#define READER_API __declspec(dllimport)
#endif

namespace DataReader
{
	enum READER_API MODE													//�����ȡģʽ
	{
		INVALID_READER,														//��Ч��ȡģʽ�����Զ��ͷ��˳�
		FILE_READER,														//�ļ���ȡģʽ��ת����PointFrame
		SENSOR_READER,														//��������ȡģʽ
		CSV_READER,															//csv��ȡģʽ
		BIN_READER
	};

	class READER_API Reader													//��ÿһ��Reader���й���
	{
	public:
		Reader();															//Ĭ�Ϲ��캯������δ��ȷ����
		virtual ~Reader();													//��������е���Դ�������һ�εļ���ͷ�
		int Init(DataProcess::RawProcess *rawProcess);						//��ʼ��ԭʼpcap���Ĵ�����
		void Destory();														//�˳��������ӣ��ͷ���Դ
		int Start(int status=0);											//��ʼ
		void Stop();														//��ͣ
		void DisConnection();												//�˳���������,֮����Ҫ���½���
		int BuildSensorConnection(const char *IP, unsigned short dest_port, unsigned short local_port, \
			LiDAR_Protocol SetProtocol, const char*Mask="255.255.255.0");	//����������������
		int DisSensorConnection();											//ȡ����ǰ����������
		int BuildFileConnection(const char *path, void(*PlayEndStop)(LPVOID pParam),\
			LPVOID pParam, LiDAR_Protocol SetProtocol);						//������ȡ�ļ�����
		int DisFileConnection();											//ȡ����ǰ�ļ�����
		int OpenCSVFile(const char *csvFilePath, void(*Callback)(std::shared_ptr<RadarPointList> &lParam,\
			LPVOID pParam), LPVOID pParam);									//��CSV�ļ�����ʾ
		void WriteCSVFile(const char *csv_savepath, RadarPointList& PointsList);//����csv�ļ�


		int OpenBinFile(const char *binFilePath, void(*Callback)(std::shared_ptr<RadarPointList> &lParam, \
			LPVOID pParam), LPVOID pParam);
		MODE GetReaderMode();												//��ȡ������ģʽ
		/*�ļ��Ķ���ʹ�õĺ���*/
		int GoToStart();													//�Ƶ�һ��ʼ
		int GoToEnd();														//�Ƶ����
		int GetNextFrame();													//�õ���һ֡,�����Ƿ�ɹ�
		int GetBackwardFrame();												//�õ���һ֡
		int GoToFrame(const int FrameNumber);								//���������ĳһ֡
		//void SetFrameNumber(unsigned int frameNumber);					//��������ĳһ֡��ֻ���ñ�־λ
		int GetFrameNumber();												//�õ��ļ����ܵ�֡��
		void Loop();														//��ʼѭ������
		void EndLoop();														//����ѭ������
		void PlayForward();													//���򲥷�
		void PlayBackward();												//���˲���
		PLAYTYPE GetPlayType();												//ȡ�ò�������
		SPEED ACCSpeed();													//�ӿ��ٶ�
		void InitSpeed();													//��ʼ�������ٶ�
		int Get_FileName(char *pcapFileName);								//�õ�pcap�ļ���
		/*���ж�ȡ��ʹ�ú���*/
		int BuildSaverConnection(const char*savepath,void(*SaveEndStop)(LPVOID pParam),\
			LPVOID pParam);													//���������ļ�����
		void DisSaverConnection();											//ȡ�������ļ�����
	private:
		SensorReader * pSensorReader;										//��������ȡ��
		FileReader * pFileReader;											//�ļ���ȡ��
		CSVReader csvReader;												//csv��ȡ��


		BINReader binReader;												//bin��ȡ��
		char filepath[1000];												//���������ļ���
		DataProcess::RawProcess *pRawProcess;								//ԭʼ���ݴ�����
		MODE mode;															//����ģʽ��Ǳ���
	};
}