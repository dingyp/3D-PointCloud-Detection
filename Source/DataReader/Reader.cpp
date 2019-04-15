#include "stdafx.h"
#include"Reader.h"
#include "ErrorManager/ErroCodeDefine.h"

using namespace DataReader;

/**************************************************************************
*  @Function Name :: Reader
*  @Description   :: Reader���Ĭ�Ϲ��캯������ʱ��δ��ȷReader����
*  @Author        :: zzw
*  @Parameters    :: None
*					
*  @Return        :: None
**************************************************************************/
Reader::Reader()
{
	pSensorReader = NULL;
	pFileReader = NULL;
	mode = INVALID_READER;//��Ч��ȡ��ʽ
	pRawProcess = NULL;
	memset(filepath, 0, sizeof(filepath));
	READER_DEBUG(_T("��������Reader"));
}

/**************************************************************************
*  @Function Name :: ~Reader
*  @Description   :: ��������е���Դ�������һ�εļ���ͷ�
*  @Author        :: zzw
*  @Parameters    :: None
*					
*  @Return        :: None
**************************************************************************/
Reader::~Reader()
{
	Destory();
	pSensorReader = NULL;
	pFileReader = NULL;
	mode = INVALID_READER;
	pRawProcess = NULL;
	READER_DEBUG(_T("����ע��Reader"));
}

/**************************************************************************
*  @Function Name :: BuildSensorConnection
*  @Description   :: ���������������ӣ��������ӽ��ж�
*  @Author        :: zzw
*  @Parameters    :: IP:��������IP��ַ port:�������Ķ˿ں�
*					 SetProtocol:�������ݵ�Э�� Mask:�������ĵ�ַ���� 
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int Reader::BuildSensorConnection(const char *IP, unsigned short dest_port, unsigned short local_port, LiDAR_Protocol SetProtocol, const char*Mask)
{
	if (NULL == pRawProcess)						//û��pcap�������࣬�˳�
	{
		return ER_BuildBeforeInit;
	}
	if (NULL != pSensorReader)						//�������Ѿ������ӣ��˳�
	{
		return ALREADY_Sen_Con;
	}
	if (NULL != pFileReader)						//�����ļ����ӣ��ر��ļ�����
	{
		DisFileConnection();
		delete(pFileReader);
		pFileReader = NULL;
	}
	pSensorReader = new SensorReader();
	if (NULL == pSensorReader)						//������������ʧ�ܣ��˳�
	{
		return ER_CREATESENCON;
	}
	pSensorReader->Init(pRawProcess, SetProtocol);
	int result = pSensorReader->SensorOpen(IP, dest_port, local_port, Mask);
	if (result)	//��������ʧ��,ע�����������ӣ��˳�
	{
		DisSensorConnection();
		delete(pSensorReader);
		pSensorReader = NULL;
		return ER_NOTConnectToSensor;
	}
	mode = SENSOR_READER;
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: BuildFileConnection
*  @Description   :: ������ȡ�ļ�����
*  @Author        :: zzw
*  @Parameters    :: path:�ļ�·�� PlayEndStop:��ʼ�����ŵ�ֹͣ�Ļص�����
*					 pParam:�ص������еĲ��� SetProtocol:���ļ�ʹ�õ�Э������
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int Reader::BuildFileConnection(const char *path, void(*PlayEndStop)(LPVOID pParam), LPVOID pParam, LiDAR_Protocol SetProtocol)
{
	if (NULL == pRawProcess)						//û��pcap�������࣬�˳�
	{
		return ER_BuildBeforeInit;
	}
	if (NULL != pFileReader)						//�����ļ����ӣ��˳�
	{
		return ALReadyFileConnection;
	}
	if (NULL != pSensorReader)						//���д��ж�ȡ�����ر�����
	{
		DisSensorConnection();
		delete(pSensorReader);
		pSensorReader = NULL;
	}
	pFileReader = new FileReader();
	if (NULL==pFileReader)
	{
		return ER_CreateFileConnection;
	}
	strcpy_s(filepath, 1000, path);					//�����ļ���������csv�ļ������õ���
	int statusInit = pFileReader->Init(pRawProcess, PlayEndStop, pParam, SetProtocol);
	if (statusInit != RaView_NO_ERROR)				//����init״̬�ж�
	{
		//�����ļ�ʧ����Ҫ��new���ļ�ָ���ͷ�
		DisFileConnection();
		delete(pFileReader);
		pFileReader = NULL;
		return statusInit;
	}
	int statusopen = pFileReader->FileOpen(path);
	if (RaView_NO_ERROR != statusopen)				//����fileopen״̬�ж�
	{
		DisFileConnection();
		delete(pFileReader);
		pFileReader = NULL;
		return statusopen;
	}
	mode = FILE_READER;
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: Start
*  @Description   :: ��ʼ��ȡ
*  @Author        :: zzw
*  @Parameters    :: status:�ļ�����ʱ�����¿�ʼ�Ƿ���Ҫ�ж�����״̬�ı�־λ
*					
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int Reader::Start(int status)
{
	int result = RaView_NO_ERROR;
	switch (mode)
	{
	case FILE_READER:
		if (NULL == pFileReader)
		{
			return ER_FileReaderPointIsNULL;
		}
		if (pFileReader->GetIsStart())				//�ļ��߳��ѿ���,���¿�ʼ����
		{
			pFileReader->ReStart(status);
		}
		else										//�ļ��̵߳�һ�ο���
		{
			pFileReader->Start(status);
			DisConnection();
		}
		break;
	case SENSOR_READER:
		if (NULL == pSensorReader)
		{
			return ER_SensorReaderPointIsNULL;
		}
		if (pSensorReader->GetIsStart())			//�������߳��Ѿ�����,���¿�ʼ
		{
			pSensorReader->ReStart();
		}
		else										//�����̵߳�һ�ο���
		{
			pSensorReader->ReStart();
			result = pSensorReader->Start();
			DisConnection();
		}
		break;
	case INVALID_READER:
		break;
	}
	return result;
}

/**************************************************************************
*  @Function Name :: Stop
*  @Description   :: ��ȡ��ͣ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void Reader::Stop()
{
	switch (mode)
	{
	case FILE_READER:
		if (NULL == pFileReader)
		{
			return;
		}
		pFileReader->Stop();
		break;
	case SENSOR_READER:
		if (NULL == pSensorReader)
		{
			return;
		}
		pSensorReader->Stop();//����ͣλ
		break;
	case INVALID_READER:
		break;
	}
	return;
}

/**************************************************************************
*  @Function Name :: DisConnection
*  @Description   :: �Ͽ���������,֮����Ҫ���½�������Destory��������ÿ�Processor��һ��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void Reader::DisConnection()
{
	if (INVALID_READER == mode)
	{
		return;
	}
	if (pSensorReader)
	{
		if (ALReadyDisSenConnection != DisSensorConnection())
		{
			delete(pSensorReader);
			pSensorReader = NULL;
		}
	}
	if (pFileReader)
	{
		if (ALReadyDisFileConnection != DisFileConnection())
		{
			delete(pFileReader);
			pFileReader = NULL;
		}
	}
	mode = INVALID_READER;
}

/**************************************************************************
*  @Function Name :: DisSensorConnection
*  @Description   :: ȡ������������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int Reader::DisSensorConnection()
{
	mode = INVALID_READER;
	if (NULL == pSensorReader)
	{
		return ALReadyDisSenConnection;
	}
	pSensorReader->Stop();
	pSensorReader->Destory();
	pSensorReader->SensorClose();
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: DisFileConnection
*  @Description   :: ȡ����ȡ�ļ�����
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int Reader::DisFileConnection()
{
	mode = INVALID_READER;
	if (NULL == pFileReader)
	{//�Ѿ��ر���
		return ALReadyDisFileConnection;
	}
	pFileReader->Stop();
	pFileReader->FileClose();
	pFileReader->Destory();
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: OpenCSVFile
*  @Description   :: ��CSV�ļ�����ʾ
*  @Author        :: zzw
*  @Parameters    :: csvFilePath:csv�ļ�·�� Callback:��ȡ��ϵĻص�����
*					 pParam:�ص������еĲ���
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int Reader::OpenCSVFile(const char *csvFilePath, void(*Callback)(std::shared_ptr<RadarPointList> &lParam, LPVOID pParam), LPVOID pParam)
{
	if (NULL != pFileReader)
	{//�ļ���ȡ��û�رգ�����
		return ALReadyFileConnection;
	}
	if (NULL != pSensorReader)
	{//��������ȡ��û�رգ�����
		return ALREADY_Sen_Con;
	}
	mode = CSV_READER;
	
	csvReader.Init(Callback, pParam);
	csvReader.CSVOpen(csvFilePath);
	csvReader.Destory();//��ȡ��csv�ļ�֮������ÿ�
	mode = INVALID_READER;//��ȡ֮��Ϳ������ó�csvģʽ
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: WriteCSVFile
*  @Description   :: ����csv�ļ�
*  @Author        :: zzw
*  @Parameters    :: csv_savepath:csv�ļ�����·��
*					 PointsList:��Ҫ����ĵ�֡
*  @Return        :: None
**************************************************************************/
void Reader::WriteCSVFile(const char *csv_savepath, RadarPointList& PointsList)
{//����csv�ļ�Ӧ���ǲ��ܴ��ں���ģʽ֮�¶����Խ��й���
	csvReader.CSVWrite(csv_savepath, PointsList);
}

/**************************************************************************
*  @Function Name :: Get_FileName
*  @Description   :: �õ�pcap�ļ���
*  @Author        :: zzw
*  @Parameters    :: pcapFileName:pcap�ļ�����
*				
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int Reader::Get_FileName(char *pcapFileName)
{//�����ļ���ȡģʽ
	if (NULL != pSensorReader)
	{//��������ȡ��ģʽ������
		return ALREADY_Sen_Con;
	}
	if (mode != FILE_READER)
	{//����pcap�Ķ�ģʽ������
		return ER_GetFileNameNotInPcap;
	}
	strcpy_s(pcapFileName, 1000,filepath);
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: Init
*  @Description   :: ��ʼ��ԭʼpcap���Ĵ�����
*  @Author        :: zzw
*  @Parameters    :: RawProcessPoint:��ʼ��ԭʼ���ݰ�������
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int Reader::Init(DataProcess::RawProcess *RawProcessPoint)
{
	mode = INVALID_READER;
	if (NULL == RawProcessPoint)
	{//������ָ��ΪNULL
		return ER_InitPointRawProcess;
	}
	pRawProcess = RawProcessPoint;
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: Destory
*  @Description   :: �˳��������ӣ��ͷ���Դ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void Reader::Destory()
{
	mode = INVALID_READER;
	pRawProcess = NULL;
	if (NULL != pSensorReader)
	{
		DisSensorConnection();
		delete(pSensorReader);
		pSensorReader = NULL;
	}
	if (NULL != pFileReader)
	{
		DisFileConnection();
		delete(pFileReader);
		pFileReader = NULL;
	}
}

/**************************************************************************
*  @Function Name :: GoToStart
*  @Description   :: ���ļ�����ʱ���Ƶ�һ��ʼ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int Reader::GoToStart()
{
	if (NULL == pFileReader) { return ER_FileReaderIsNULL; }
	return pFileReader->GoToStart();
}

/**************************************************************************
*  @Function Name :: GoToEnd
*  @Description   :: ���ļ�����ʱ���Ƶ����
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int Reader::GoToEnd()
{
	if (NULL == pFileReader) { return ER_FileReaderIsNULL; }
	return pFileReader->GoToEnd();
}

/**************************************************************************
*  @Function Name :: GetNextFrame
*  @Description   :: �õ���һ֡,�����Ƿ�ɹ�
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int Reader::GetNextFrame()
{
	if (NULL == pFileReader) { return ER_FileReaderIsNULL; }
	return pFileReader->GetNextFrame(true);

}

/**************************************************************************
*  @Function Name :: GetBackwardFrame
*  @Description   :: �õ���һ֡,�����Ƿ�ɹ�
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int Reader::GetBackwardFrame()
{
	if (NULL == pFileReader) { return ER_FileReaderIsNULL; }
	return pFileReader->GetBackwardFrame(true);
}

/**************************************************************************
*  @Function Name :: GoToFrame
*  @Description   :: ���������ĳһ֡
*  @Author        :: zzw
*  @Parameters    :: FrameNumber:������֡���
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int Reader::GoToFrame(const int FrameNumber)
{
	if (NULL == pFileReader) { return ER_FileReaderIsNULL; }
	return pFileReader->GoToFrame(FrameNumber,true);
}

////��������ĳһ֡��ֻ���ñ�־λ
//void Reader::SetFrameNumber(unsigned int frameNumber)
//{
//	if (NULL == pFileReader) { return; }
//	pFileReader->SetFrameNumber(frameNumber);
//}

/**************************************************************************
*  @Function Name :: GetFrameNumber
*  @Description   :: �õ��ļ����ܵ�֡��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int Reader::GetFrameNumber()
{
	if (NULL == pFileReader) { return 0; }
	return pFileReader->GetFrameNumber();
}

/**************************************************************************
*  @Function Name :: Loop
*  @Description   :: ��ʼѭ������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
void Reader::Loop()
{
	if (NULL == pFileReader){return;}
	pFileReader->Loop();
}

/**************************************************************************
*  @Function Name :: EndLoop
*  @Description   :: ����ѭ������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
void Reader::EndLoop()
{
	if (NULL == pFileReader){return;}
	pFileReader->EndLoop();
}

/**************************************************************************
*  @Function Name :: PlayForward
*  @Description   :: ���򲥷�
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
void Reader::PlayForward()
{
	if (NULL == pFileReader) { return; }
	pFileReader->PlayForward();
}

/**************************************************************************
*  @Function Name :: PlayBackward
*  @Description   :: ���˲���
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
void Reader::PlayBackward()
{
	if (NULL == pFileReader) { return; }
	pFileReader->PlayBackward();
}

/**************************************************************************
*  @Function Name :: ACCSpeed
*  @Description   :: �ӿ��ٶ�
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
SPEED Reader::ACCSpeed()
{
	if (NULL == pFileReader) { return READER_STOP; }
	return pFileReader->ACCSpeed();
}

/**************************************************************************
*  @Function Name :: BuildSaverConnection
*  @Description   :: ���������ļ�����
*  @Author        :: zzw
*  @Parameters    :: savepath:�����ļ���·�� SaveEndStop:�ļ��洢̫���Ļص����� 
*					 pParam:�ص���������Ĳ���
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int Reader::BuildSaverConnection(const char*savepath, void(*SaveEndStop)(LPVOID pParam), LPVOID pParam)
{
	if (NULL == pSensorReader) { return ER_SensorReaderIsNULL; }
	return pSensorReader->BuildSaverConnection(savepath, SaveEndStop, pParam);
}

/**************************************************************************
*  @Function Name :: DisSaverConnection
*  @Description   :: ȡ�������ļ�����
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
void Reader::DisSaverConnection()
{
	if (NULL == pSensorReader) { return ; }
	pSensorReader->DisSaverConnection();
}

int DataReader::Reader::OpenBinFile(const char * binFilePath, void(*Callback)(std::shared_ptr<RadarPointList>&lParam, LPVOID pParam), LPVOID pParam)
{
	if (NULL != pFileReader)
	{//�ļ���ȡ��û�رգ�����
		return ALReadyFileConnection;
	}
	if (NULL != pSensorReader)
	{//��������ȡ��û�رգ�����
		return ALREADY_Sen_Con;
	}
	mode = BIN_READER;

	binReader.Init(Callback, pParam);
	binReader.BINOpen(binFilePath);
	binReader.Destroy();//��ȡ��csv�ļ�֮������ÿ�
	mode = INVALID_READER;//��ȡ֮��Ϳ������ó�csvģʽ
	return RaView_NO_ERROR;
	
}

/**************************************************************************
*  @Function Name :: GetReaderMode
*  @Description   :: ��ȡ������ģʽ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
MODE Reader::GetReaderMode()
{
	return mode;
}

/**************************************************************************
*  @Function Name :: GetPlayType
*  @Description   :: ȡ�ò�������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
PLAYTYPE Reader::GetPlayType()
{
	if (NULL == pFileReader) { return FORWORD; }
	return pFileReader->GetPlayType();
}

/**************************************************************************
*  @Function Name :: InitSpeed
*  @Description   :: ��ʼ�������ٶ�
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
void Reader::InitSpeed()
{
	if (NULL == pFileReader) { return ; }
	pFileReader->SetSpeed(ONE_SPEED);
}