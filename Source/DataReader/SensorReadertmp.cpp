//#include "stdafx.h"
//#include <iostream>
//#include <fstream>//����pcap�ļ��Ķ�ȡ
//#include <memory> //shared_ptr
//#include <windows.h>
//#include "SensorReader.h"
//#include "ErrorManager/ErroCodeDefine.h"
//
//using namespace DataReader;


//��ʵ��1206
#define BUFFER_LEN 1300

//����Ĭ�Ϲ��캯������Ϊ���ܲ���ȷ����ʲô�����������ű�����
SensorReader::SensorReader()
{
	destorySensorReader = true;
	mode = INVALID_READER;				//����ʱģʽ����ȷ
	SetSpeed(READER_STOP);				//��ʼ���ٶ�
	Init_Frame();						//��ʼ��֡��������
	offset_idxTable.index_number = 0;	//�ٵ�һ�εĹ����ʱ����Ҫ��ʼ��0
	InitConnection();					//��ʼ����������
	pcap_reader = NULL;					//��ȡ�ļ�ָ���ÿ�
	pcap_saver = NULL;					//�����ļ�ָ���ÿ�
}

SensorReader::~SensorReader()
{
	destorySensorReader = true;
	DisConnection();					//����ʲô���ӣ��Ƚ���ȡ������
	SetSpeed(READER_STOP);				//�ٶ���ͣ
	Finish_Frame();						//�ͷ�֡������
	mode = INVALID_READER;				//ģʽ����
}

//����false˵������ʧ��
bool SensorReader::BuildUDPConnection(const char *IP, int port)
{
	//����֮ǰ��ȡ����������
	DisConnection();

	Init_IP_Port(IP, port, port);		//������������

	SOCKADDR_IN m_addr_local;
	SOCKADDR_IN m_addr_dest;

	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	//��Ҫ��İ汾(Winsock����߰汾)��������ϵͳ֧�ֵ���װ汾(����),��ô�ú���   
	//�����ɹ� ������WSADATA.WhighVersion�з���ϵͳ֧�ֵ���߰汾,��WSADATA.wVersion��   
	//����ϵͳ֧�ֵ���߰�(����)�� wVersionRequested ֮��Ľ�Сֵ��
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return false;//Windows Sockets ���ʼ��ʧ��.
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return false;
	}

	//����udp ���׽���
	m_hSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (m_hSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return false;//����socketʧ��.
	}
	m_addr_dest.sin_family = AF_INET;
	m_addr_dest.sin_addr.s_addr = htonl(DestIPAddress);
	m_addr_dest.sin_port = htons(DestPort);

	m_addr_local.sin_family = AF_INET;
	m_addr_local.sin_addr.s_addr = htonl(INADDR_ANY);
	m_addr_local.sin_port = htons(LocalPort);////��һ������ָ���˿�

	err = bind(m_hSocket, (SOCKADDR*)&m_addr_local, sizeof(SOCKADDR));//��һ�����ض˿ڣ�sendto��ʱ���local�˿ڷ��͵�dest�˿�
	if (err != 0)
	{
		closesocket(m_hSocket);
		WSACleanup();
		return false;
	}

	//������״̬�ĸı�
	mode = ONLY_SENSOR_READER;
	SetSpeed(DEFAULT_SPEED);			//�ı��ٶ�����
	return true;
}

//�ÿ�������������
void SensorReader::Free_IP_Port()
{
	DestIPAddress = 0;
	DestPort = 0;
	LocalPort = 0;
}

//��ʼ��������
void SensorReader::Init_IP_Port(const char *De_IP, int De_Port, int Lo_Port)
{
	DestIPAddress = ntohl(inet_addr(De_IP));
	DestPort = De_Port;
	LocalPort = Lo_Port;
}

//ȡ����ǰ������UDP����
void SensorReader::DisUDPConnection()
{
	if (INVALID_SOCKET != m_hSocket)
	{
		closesocket(m_hSocket);
	}
	m_hSocket = INVALID_SOCKET;
	WSACleanup();						//�ر�����
	Free_IP_Port();						//�ÿ�IP�Ͷ˿�
}


//ȡ�������ı����ļ�����
void SensorReader::DisSaverFileConnection()
{
	if (pcap_saver != NULL)
	{
		pcap_saver->close();
		delete(pcap_saver);
		pcap_saver = NULL;
	}
}


//ֻ��Դ�������ֹ֡ͣ����
void SensorReader::SensorStop()
{
	//���ò����ٶ�Ϊ0
	SetSpeed(READER_STOP);
}


//ֻ��Դ�����֡����
void SensorReader::SensorStart()
{
	//���û��ע��ص�����������
	if ((NULL == FullFrame) || (NULL == RTAdress))
	{
		return;
	}
	if (NULL == m_hSocket)
	{//��δ�����׽���
		return;
	}

	SOCKADDR_IN addr_from;
	int n = 0;
	int len = sizeof(SOCKADDR);
	char buf[BUFFER_LEN];
	unsigned char ubuf[BUFFER_LEN];
	int nRecvBufSize = 50 * BUFFER_LEN;//���仺��������ֹ�ж�UDP�����������
	setsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (char *)&nRecvBufSize, sizeof(int));//���û������Ĵ�С

	destorySensorReader = false;
	while (!destorySensorReader) {
		memset(buf, 0, sizeof(buf));
		n = recvfrom(m_hSocket, buf, BUFFER_LEN, 0, (SOCKADDR*)&addr_from, &len);


		//������յ������ݣ��ͽ������ݵĴ������ݴ������֡�Ĵ���
		if (n >0) {
			memcpy(ubuf, buf, n);
			//��buf�е����ݽ�����pSensorFrame��,��һ֡����һ����������ת��pPointFrame֡��
			DataParse(ubuf, n, true);//��Ҫ������֡����
		}
		else
		{
			int errnumber = WSAGetLastError();
			int number = errnumber;
		}
		n = 0;


		//���ݽ���֮�󣬽���˯�߲���
		BeginSleep();//�������õ��ٶȽ�������
	}
}

//��Sensor���ݴ���savepath��
void SensorReader::SensorToFile(const char*savepath)
{
	//..
}

//�ڴ�������ʾ״̬�£������ļ��ı��棬�������Ƿ�����ɹ�
bool SensorReader::BeginFileSave(const char *path)
{
	//���֮ǰ�Ƿ��Ǵ�����������ʾ
	if (ONLY_SENSOR_READER != mode)
	{//���Ƿ���ʧ��
		return false;
	}

	//�����ļ������Լ�����
	DisSaverFileConnection();
	pcap_saver = new std::ofstream();//�½�һ���ļ�������
	if (NULL == pcap_saver)
	{//�ļ�����ʧ��
		return false;
	}
	pcap_saver->open(path, std::ios_base::out | std::ios_base::binary);

	if (pcap_saver->bad())
	{//�ļ���ʧ��
		delete(pcap_saver);
		pcap_saver = NULL;
		MessageBox(NULL, L"�½������ļ�ʧ��", L"����", MB_ICONEXCLAMATION);
		return false;
	}
	//�����ж�Ū��֮�󣬽���״̬�ĸı�
	mode = FILE_SENSOR_READER;
	return true;
}


//ȡ��������winpcap����
void SensorReader::DisSaveConnection()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//�����ļ�����
bool SensorReader::BuildFileConnection(const char *path)
{
	//����֮ǰ��ȡ����������
	DisConnection();

	MultiByteToWideChar(CP_ACP, 0, path, -1, pcap_path, 1000);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)StepUpOffsetTable, this, 0, 0);//�����ļ�������
																					//�ڶ�ȡ�ļ�֮ǰ���ȴ�������Ľ���
	while (0 == GetFrameNumber())
	{//˵�����ڽ���֡��������
		Sleep(5);
	}

	pcap_reader = new std::ifstream();//�½�һ���ļ�������
	if (NULL == pcap_reader)
	{//�ļ�����ʧ��
		return false;
	}
	pcap_reader->open(path, std::ios_base::in | std::ios_base::binary);

	if (pcap_reader->bad())
	{//�ļ���ʧ��
		delete(pcap_reader);
		pcap_reader = NULL;
		return false;
	}

	//������״̬�ĸı�
	mode = PCAP_READER;
	SetSpeed(READER_STOP);						//�ļ�һ��ʼ��Ĭ���ٶ�Ӧ����STOP����ѭ��
	return true;
}

//�ÿ�������,ͬʱ�ļ�ָ��Ҳ�ر�
void SensorReader::Free_offsetTable()
{
	cur_index = 0;
	if (0 != GetFrameNumber())//���֮ǰ�д������������Ƚ����ͷ�
	{
		offset_idxTable.index_number = 0;
		delete(offset_idxTable.index_table);
	}
	offset_idxTable.index_table = NULL;
}

//��ʼ�������������Ƿ񴴽��ɹ�����û����������֮ǰ�ǲ����ٴγ�ʼ������
void SensorReader::Init_offsetTable(std::vector<unsigned long long> &idx_table)
{
	if (0 != GetFrameNumber())
	{//�Ѿ�������������
		return;
	}
	//��û�н���������
	offset_idxTable.index_table = new unsigned long long[idx_table.size()];
	if (NULL == offset_idxTable.index_table)
	{
		MessageBox(NULL, L"�ڴ治����", 0, 0);			//---------�˴��ǲ�����Ҫ�˳�----------
		return;
	}
	for (int i = 0; i < idx_table.size(); i++)
	{
		offset_idxTable.index_table[i] = idx_table[i];
	}
	cur_index = 0;//û�ж�ȡ��һ֡
	offset_idxTable.index_number = idx_table.size();//�Ȱ��������ݸ���ȥ�����޸�size
}

//ȡ����ǰ�������ļ�����
void SensorReader::DisFileConnection()
{
	Free_offsetTable();					//�ļ��������涼�ÿ�
	if (pcap_reader != NULL)
	{
		pcap_reader->close();				//close�ļ���
		delete(pcap_reader);
		pcap_reader = NULL;
	}
}

//֡����ĳ�ʼ��,�������Ͷ���Ҫ�ĳ�ʼ��
void SensorReader::Init_Frame()
{
	FullFrame = NULL;
	RTAdress = NULL;					//�����ĸ����ܶ���Ҫ�õ��ص�����
	LastAngle_H = MAXWORD;
	CurrentFrameIndex = 0;
	pSensorFrame = nullptr;				//����reader��ʱ��֡��ȷ��NULL
}

void SensorReader::Finish_Frame()
{
	pSensorFrame = nullptr;
	CurrentFrameIndex = 0;
	LastAngle_H = MAXWORD;
	FullFrame = NULL;
}

//�Իص��������а�ע��
void SensorReader::BindFullFrameCallbackFunc(void(*Callback)(std::shared_ptr<SensorFrame> &lParam, LPVOID pParam), LPVOID pParam)
{
	FullFrame = Callback;
	RTAdress = pParam;
}


//��������֮ǰ�Ƚ���ȡ�����ӣ�����ȡ���Ƿ�ɹ����ڲ������
bool SensorReader::DisConnection()
{
	SetSpeed(READER_STOP);								//�ٶ���ͣ
	switch (mode)
	{
	case PCAP_READER:
		DisFileConnection();
		break;
	case ONLY_SENSOR_READER:
		DisUDPConnection();
		break;
	case FILE_SENSOR_READER:
		DisSaverFileConnection();
		DisUDPConnection();
		break;
	case INVALID_READER:
		break;
	default://����Υ��������в���������
		return false;
	}
	mode = INVALID_READER;						//ģʽ����
	return true;
}

//Start�������ǽ���ÿһ֡�Ļ�ȡ,��ȷ�ص�����,�ڲ������
void SensorReader::Start()
{
	switch (mode)
	{
	case ONLY_SENSOR_READER:
		SensorStart();
		break;
	case PCAP_READER:
		FileStart();
		break;
	case FILE_SENSOR_READER:
		break;
	default://Ĭ�Ϲ��캯��
		break;
	}
}

//ֹͣ֡�Ĳ���
void SensorReader::Stop()
{
	switch (mode)
	{
	case ONLY_SENSOR_READER:
		SensorStop();
		break;
	case PCAP_READER:
		FileStop();
		break;
	case FILE_SENSOR_READER:
		//SensorToFile();
		break;
	default://Ĭ�Ϲ��캯��
		break;
	}
}

//�˳�while(true)ѭ���ĺ���
void SensorReader::Destory_SensorReader()
{
	destorySensorReader = true;
}

//�������õ��ٶȽ�������
void SensorReader::BeginSleep()
{
	if (PCAP_READER == mode)
	{
		switch (speed)
		{
		case READER_STOP:
			while ((READER_STOP == speed) && (!destorySensorReader))//ֹͣ��ʱ��һֱ�ڴ˴�˯�ߣ�ֱ���ٶȱ��ı�
			{
				Sleep(1000);
			}
			break;
		case ONE_SPEED:
			Sleep(100); break;
		case DOUBLE_SPEED:
			Sleep(50); break;
		case THREE_SPEED:
			Sleep(33); break;
		case FOUR_SPEED:
			Sleep(25); break;
		case EIGHT_SPEED:
			Sleep(13); break;
		case SIXTEEN_SPEED:
			Sleep(6); break;
		case ONE_POINT_FIVE_SPEED:
			Sleep(67); break;
		case HALF_SPEED:
			Sleep(200); break;
		case QUARTER_SPEED:
			Sleep(400); break;
		case POINT_ONE_SPEED:
			Sleep(1000); break;
		case DEFAULT_SPEED:
			Sleep(100); break;
		default://�������÷Ƿ��������ٶ����ó�1��λ����
			Sleep(100); break;
		}
	}
	else if (ONLY_SENSOR_READER == mode)
	{
		switch (speed)
		{
		case READER_STOP:
			while ((READER_STOP == speed) && (!destorySensorReader))//ֹͣ��ʱ��һֱ�ڴ˴�˯�ߣ�ֱ���ٶȱ��ı�
			{
				Sleep(1000);
			}
			break;
		default://�������÷Ƿ�����Ϊ��UDPͨ�ţ����Բ���������
			break;
		}
	}
	else if (FILE_SENSOR_READER == mode)//��������ʾ�ͱ���
	{//����û��ֹͣ�������..
	 //..
	}
}



/**************************************************************************
*  @Function Name :: SwitchFrame
*  @Description   :: �ж��Ƿ���Ҫ�����ɵ�֡������������֡
*  @Author        :: zzw
*  @Parameters    :: Angle:��һ�����ݵ�ˮƽ��
*
*  @Return        :: true:����������ȥ��false:���ٽ��д���
**************************************************************************/
bool SensorReader::SwitchFrame(WORD Angle, UINT BlockMinPoints, bool newFrame)//��֡�Ƿ���Ҫ��������ʱ����newFrame
{
	//����һ֮֡���Ƿ������ȡ��������ݣ���Ϊ�ļ��еĻطţ������������ͣ������Ҫ�������´���
	if (MAXWORD == LastAngle_H)
	{
		LastAngle_H = Angle;
	}

	if (Angle < LastAngle_H  //����Ƕȱ�ǰһ��С������Ϊ���µ�һ֡
		|| pSensorFrame->GetPointsNumber() >= (pSensorFrame->GetMaxPointsInFrame() - BlockMinPoints))//���Frame��������һ��BLOCK��Ҳ�л�֡
	{
		//�л�֡�������߳̽��ɵ�֡�׳�������ģʽ��ѡ��ͬ���׳���ʽ
		switch (mode)
		{
		case ONLY_SENSOR_READER:
			FullFrame(pSensorFrame, RTAdress);
			break;
		case PCAP_READER:
			FullFrame(pSensorFrame, RTAdress);
			break;
		case FILE_SENSOR_READER:
			//�ڽ����ļ�����֮�󣬻�����Ҫ�����ڵ�����֡���з���
			FullFrame(pSensorFrame, RTAdress);
			break;
		default://Ĭ�Ϲ��캯��
			break;
		}

		pSensorFrame = nullptr;//֮ǰ��ָ�����
		LastAngle_H = MAXWORD;

		if (newFrame)//����������һ֡
		{
			CurrentFrameIndex++;
			pSensorFrame = std::make_shared<SensorFrame>();
			pSensorFrame->SetFrameIndex(CurrentFrameIndex, GetCurFrameNumber());//������֡���±�
			LastAngle_H = Angle;
		}
		return newFrame;
	}
	LastAngle_H = Angle;
	return true;
}

/**************************************************************************
*  @Function Name :: DataParse
*  @Description   :: ���ݽ���,��־λ����Ƿ񴴽���֡
*  @Author        ::
*  @Parameters    :: pBuf:���ݰ���ַ
*                    dwLen:���ݰ����ȣ��ֽڣ�
*  @Return        :: �����н��������ݵ������
**************************************************************************/
int SensorReader::DataParse(void* pBuf, size_t dwLen, bool newFrame)//newFrame��ʾ��֮֡���ټ�������
{
	HLP16_Block *pBlock = (HLP16_Block*)pBuf;
	bool cur_new_frame = false;//��ǰ��֡���λ
	if (pBlock->Header != BLOCK_HEADER_VELODYNE)
		return 0;
	if (dwLen != 1206)//HLP-16���ݰ��Ľṹ12*��2+2+96����ʱ����4�ֽڣ������ֶ�2�ֽ�
		return 0;

	if (nullptr == pSensorFrame)//������֡�����ݽǶ���Ϣ���Ͼ�֡�����ų�
	{
		cur_new_frame = true;
		CurrentFrameIndex++;
		pSensorFrame = std::make_shared<SensorFrame>();
		pSensorFrame->SetFrameIndex(CurrentFrameIndex, GetCurFrameNumber());//���õ�ǰ֡���±�
		LastAngle_H = MAXWORD;
	}

	int newFrameIndex = 0;
	//����cur_new_frame����������ҵ�����i����ʼ��newFrameIndex(0-11)�Ƕ���
	if (cur_new_frame)
	{//�������֡���ҵ��Ƕ���С����һ֡��
		HLP16_Block *pfindBlock = pBlock;
		unsigned short findMinAngle = pfindBlock->Angle;
		pfindBlock++;//��Ϊ������±��1��ʼ
		for (int idx = 1; idx < BLOCK_NUM; idx++)
		{
			if (findMinAngle > pfindBlock->Angle)
			{//�������ǰ��һ���Ƕȴ�������Ƕȣ�˵��������һ֡,���ñ��λ���˴�
				newFrameIndex = idx;
				for (int i = 0; i < newFrameIndex; i++)//��pBlockҲ����ƶ���ͬ��Block����
				{
					pBlock++;
				}
				break;
			}
			findMinAngle = pfindBlock->Angle;
			pfindBlock++;
		}
	}

	int nAngleAdd = 0;
	for (int i = newFrameIndex; i < BLOCK_NUM; i++)//����֡��ʼ����ʼ
	{
		PointR pr;
		pr.angleH = pBlock->Angle;

		//�������Ҫ�����������˳�
		if (!SwitchFrame(pr.angleH, BLOCK_POINTS, newFrame))
		{//����������,�˴����ε����ݴ���
			return BLOCK_POINTS * (i - newFrameIndex);//�ȷ��صõ��ĵ���
		}

		if (i < (BLOCK_NUM - 1))//�����һ����������һ������һ�����ˮƽ�Ƕ����������һ��������ǰһ������
		{
			nAngleAdd = (pBlock + 1)->Angle - (pBlock->Angle);
			if (nAngleAdd < 0)
				nAngleAdd += 36000;
			nAngleAdd /= 2;
		}

		for (int j = 0; j < BLOCK_POINTS; j++)
		{
			pr.packID = i;
			pr.channel = j;
			pr.distance = pBlock->Channel[j].Distance * 2ul;
			pr.intensity = pBlock->Channel[j].Intensity;

			if (j == 16) //��16������ʱ����ˮƽ�Ƕ�ֵ
				pr.angleH += (WORD)nAngleAdd;
			if (pr.angleH >= 36000) // ����360���Ժ�Ҫ��ȥ360����ֻ��һ�Σ��Ա��ڼ����������Ƿ�����
				pr.angleH -= 36000;

			if (true == pSensorFrame->TestAddPoint())//���ؿ��Լӵ�
			{
				pSensorFrame->AddPoint(pr);
			}
		}
		pBlock++;
	}

	//DWORD *pTimestamp = (DWORD*)(void*)pBlock;
	////	ATLTRACE(_T("Time:%u\n"),*pTimestamp);
	//pTimestamp++;
	//WORD *pFactoryBytes = (WORD*)(void*)pTimestamp;
	////	ATLTRACE(_T("Factory:0x%04X\n"), *pFactoryBytes);

	return BLOCK_POINTS * (BLOCK_NUM - newFrameIndex);
}
