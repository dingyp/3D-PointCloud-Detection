#include "stdafx.h"
#include <fstream>
#include "SensorReader.h"
#include "ErrorManager\ErroCodeDefine.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif  // _WIND32

// ����64λ����
#if defined(_WIN32) && !defined(CYGWIN)
typedef __int64 int64_t;
#else
typedef long long int64t;
#endif  // _WIN32

// ��ȡϵͳ�ĵ�ǰʱ�䣬��λ΢��(us)
int64_t GetSysTimeMicros()
{
#ifdef _WIN32
	// ��1601��1��1��0:0:0:000��1970��1��1��0:0:0:000��ʱ��(��λ100ns)
#define EPOCHFILETIME   (116444736000000000UL)
	FILETIME ft;
	LARGE_INTEGER li;
	int64_t tt = 0;
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	// ��1970��1��1��0:0:0:000�����ڵ�΢����(UTCʱ��)
	tt = (li.QuadPart - EPOCHFILETIME) / 10;
	return tt;
#else
	timeval tv;
	gettimeofday(&tv, 0);
	return (int64_t)tv.tv_sec * 1000000 + (int64_t)tv.tv_usec;
#endif // _WIN32
	return 0;
}


#define BUFFER_LEN 1514//��̫���֡
using namespace DataReader;

/**************************************************************************
*  @Function Name :: SensorReader
*  @Description   :: SensorReader��Ĺ��캯��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
SensorReader::SensorReader()
{
	destorySensorReaderStart = true;
	pcap_saver = NULL;//���캯���ȶ��ļ���ȡ��ָ���ÿ�
	savemode = NOSAVE;
	//adhandle = NULL;
	m_hSocket = INVALID_SOCKET;
	pRawProcess = NULL;
	Info = NULL;
	Protocol = LIDAR_INVALID;
	dest_connect_port = 0;
	local_connect_port = 0;
	READER_DEBUG(_T("���ڿ���SensorReader"));
}

/**************************************************************************
*  @Function Name :: ~SensorReader
*  @Description   :: SensorReader��Ĺ��캯��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
SensorReader::~SensorReader()
{
	destorySensorReaderStart = true;
	pcap_saver = NULL;
	savemode = NOSAVE;
	//adhandle = NULL;
	m_hSocket = INVALID_SOCKET;
	pRawProcess = NULL;
	Info = NULL;
	Protocol = LIDAR_INVALID;
	dest_connect_port = 0;
	local_connect_port = 0;
	READER_DEBUG(_T("����ע��SensorReader"));
}

/**************************************************************************
*  @Function Name :: Init
*  @Description   :: ��ʼ���������ӣ���ԭʼpcap�����д���,�����˶԰������Э��
*  @Author        :: zzw
*  @Parameters    :: RawProcessPoint:��ʼ��ԭʼ���ݰ�������
*					 SetProtocol:����Э��
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int SensorReader::Init(DataProcess::RawProcess *RawProcessPoint, LiDAR_Protocol SetProtocol)
{
	if (!destorySensorReaderStart)
	{//��ʼ��Ӧ�û�û��start
		return ER_noDestorySensorReader;
	}
	if (NULL == RawProcessPoint)
	{//������ָ��ΪNULL
		return ER_InitPointRawProcess;
	}
	pRawProcess = RawProcessPoint;
	Protocol = SetProtocol;
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: BuildSaverConnection
*  @Description   :: ����һ�������ļ����߳�
*  @Author        :: zzw
*  @Parameters    :: savepath:�����ļ���·�� SaveEndStop:�ļ��洢̫���Ļص����� 
*					 pParam:�ص���������Ĳ���
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int SensorReader::BuildSaverConnection(const char*savepath, void(*SaveEndStop)(LPVOID pParam), LPVOID pParam)
{
	if (LIDAR_INVALID == Protocol)
	{
		return ER_UnSetTheProtocol;
	}
	savemode = SAVE;
	if (NULL != pcap_saver)
	{//�Ѿ��б����ļ����߳�
		return ALReadySaveReader;
	}

	pcap_saver = new SaveReader();
	if (NULL == pcap_saver)
	{
		return ER_BuildSaverNULL;
	}
	pcap_saver->Init();
	pcap_saver->SaveFileOpen(savepath, Protocol, SaveEndStop, pParam);
	pcap_saver->Start();//�������濪��һ���߳�
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: DisSaverConnection
*  @Description   :: ȡ�������ļ�����
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void SensorReader::DisSaverConnection()
{
	savemode = NOSAVE;
	if (NULL == pcap_saver)
	{//�����ļ����߳��Ѿ�û��
		return ;
	}
	pcap_saver->SaveFileClose();
	pcap_saver->Destory();
	delete(pcap_saver);
	pcap_saver = NULL;
	return;
}

/**************************************************************************
*  @Function Name :: Destory
*  @Description   :: �˳��������ӣ��ͷ���Դ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void SensorReader::Destory()
{
	destorySensorReaderStart = true;//�˳�whileѭ��
	if (NULL != pcap_saver)
	{//�ͷ�
		DisSaverConnection();
	}
	Info = NULL;
	Protocol = LIDAR_INVALID;
	Sleep(10);//�������˯����Ϊ�˸�Start()�˳���ʱ�䣬��Ȼ�ᱨ������Ҫ
}

/**************************************************************************
*  @Function Name :: GetIsStart
*  @Description   :: �����߳��Ƿ��Ѿ��������������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �߳��Ƿ���������
**************************************************************************/
bool SensorReader::GetIsStart()
{
	return !destorySensorReaderStart;
}

/**************************************************************************
*  @Function Name :: Start
*  @Description   :: ��ʼ����������
*  @Author        :: zzw
*  @Parameters    :: None
*					 
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int SensorReader::Start()
{
	READER_DEBUG(_T("�״��ȡ�߳̿���"));
	destorySensorReaderStart = false;
	int result= RaView_NO_ERROR;
	if (LIDAR_HLP16 == Protocol)
	{
		result = PcapStart();
	}
	else if (LIDAR_2D == Protocol)
	{
		result = PcapngStart();
	}
	else
	{
		result = ER_UnSureOfProtocol;
	}
	READER_DEBUG(_T("�״��ȡ�̹߳ر�"));
	return result;
}

/**************************************************************************
*  @Function Name :: PcapStart
*  @Description   :: ��ʼ����pcap����ʽ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int SensorReader::PcapStart()
{
	READER_DEBUG(_T("����16���״��ȡ"));
	int ErrorDataFormat = 0;//��¼��������ݸ�ʽ�Ĵ���
	int ErrorRecvFrom = 0;//recvfrom���մ���

	SOCKADDR_IN addr_from;
	int n = 0;
	int len = sizeof(SOCKADDR);
	char buf[BUFFER_LEN];

	while (!destorySensorReaderStart)
	{
		if (INVALID_SOCKET!= m_hSocket)
		{
			memset(buf, 0, sizeof(buf));
			n = recvfrom(m_hSocket, buf, BUFFER_LEN, 0, (SOCKADDR*)&addr_from, &len);

			//������յ������ݣ��ͽ������ݵĴ������ݴ������֡�Ĵ���
			if (n>0)
			{
				if (1206 == n)//�͵�������UDP��������ݣ������С����ȷ���Ĵ�С���������
				{
					ErrorDataFormat = 0;//֮ǰ�Ĵ������ݸ�ʽ���Բ���
					ErrorRecvFrom = 0;
					Info = std::make_shared<DataDefinition::raw_packetInfo>();//����
					if (Info == NULL)				//�����ڴ�ʧ�ܣ��˳�
					{
						//READER_DEBUG(_T("�ڴ�����ʧ�ܽ����˳�"));
						return ER_pBuffNotCreated;
					}
					Info->size = 1206 + 42 + 16;//����+(mac+ip+udp)+pcap��ͷ
					Info->isnewFrame = 1;
					Info->FileCurFrameIndex = 0;
					//��ɶ�pcapͷ������
					int64_t nowtime = GetSysTimeMicros();//��ȡϵͳʱ�䣬��ȷ��΢��
					if (0 == nowtime)
					{
						//READER_DEBUG(_T("��ȡnowtimeʧ�ܽ����˳�"));
						return 1;//ϵͳʱ��δ�ܻ�ȡ
					}
					Info->pBuff.pcapPacket.pcap_head.ts.timestamp_s = (long)(nowtime / (1000 * 1000));
					Info->pBuff.pcapPacket.pcap_head.ts.timestamp_ms = (long)((nowtime % (1000 * 1000)) / 10);
					Info->pBuff.pcapPacket.pcap_head.capture_len = 1248;
					Info->pBuff.pcapPacket.pcap_head.len = 1248;
					//��ɶ�mac֡ͷ�����ɣ�ע��Դmac��ַ�˴�ΪĬ�ϵ�0x000000,000000
					memset(&(Info->pBuff.pcapPacket.mac_head.SrcMacAddress), 0, sizeof(Info->pBuff.pcapPacket.mac_head.SrcMacAddress));
					memset(&(Info->pBuff.pcapPacket.mac_head.DstMacAddress), 0xff, sizeof(Info->pBuff.pcapPacket.mac_head.DstMacAddress));
					Info->pBuff.pcapPacket.mac_head.Type = htons(0x0800);//��һ����IPЭ��,˳����UDP
					//��ɶ�IP֡ͷ������
					Info->pBuff.pcapPacket.ip_head.VersionAndHeaderLen = 0x45;
					Info->pBuff.pcapPacket.ip_head.TypeOfService = 0x00;
					Info->pBuff.pcapPacket.ip_head.TotalLenOfPacket = htons(1234);//1234,˳��С�˴洢
					Info->pBuff.pcapPacket.ip_head.PacketID = 0;
					Info->pBuff.pcapPacket.ip_head.Sliceinfo = htons(0x4000);
					Info->pBuff.pcapPacket.ip_head.TTL = 255;
					Info->pBuff.pcapPacket.ip_head.TypeOfProtocol = 17;
					Info->pBuff.pcapPacket.ip_head.CheckSum = htons(0xb4aa);//��ͷУ���,��ʱ���������
					Info->pBuff.pcapPacket.ip_head.SourIp = htonl(DestIPAddress);
					Info->pBuff.pcapPacket.ip_head.DestIp = ntohl(inet_addr("255.255.255.255"));
					//��ɶ�UDP��ͷ������
					Info->pBuff.pcapPacket.udp_head.m_usSourPort = htons(dest_connect_port);
					Info->pBuff.pcapPacket.udp_head.m_usDestPort = htons(local_connect_port);
					Info->pBuff.pcapPacket.udp_head.m_usLength = htons(1214);//1206+8
					Info->pBuff.pcapPacket.udp_head.m_usCheckSum = 0x0000;
					//��ɶ����ݵĿ���
					memcpy(&(Info->pBuff.pcapPacket.block), buf, 1206);//���ݳ�����1206

					if ((NULL != pRawProcess) && (!destorySensorReaderStart))
					{
						pRawProcess->ProcessAddDataPacket(Info);
						//READER_DEBUG(_T("�Ѿ���pRawProcess���淢����Ϣ"));
					}
					if ((SAVE == savemode) && (NULL != pcap_saver) && (!destorySensorReaderStart))
					{//�Ǳ���ģʽ����saver���Ա���
						pcap_saver->AddPcapPacket(Info);
					}
					Info = NULL;
				}
				else//��һ�µ����ݸ�ʽ
				{
					ErrorDataFormat++;
				}
			}
			if (-1 == n)//����ʧ��
			{
				ErrorRecvFrom++;
				Sleep(1);
			}
			n = 0;
		}
		//READER_DEBUG(_T("�˳�SensorReader_whileѭ��"));
		if (ErrorRecvFrom > 100)
		{
			return ER_OutTimeOfConnect;//��ʱ
		}

		//��һЩ״̬�����ж�
		if (ErrorDataFormat > 200)
		{//����200�εĴ������ݸ�ʽ����Ϊ��Э��ѡȡ����
			//READER_DEBUG(_T("16���״��ȡ����Э�鲻�Ե����"));
			return ER_UnSureOfProtocol;
		}
		BeginSleep();//����һ֮֡�����˯��
	}
	READER_DEBUG(_T("�˳�16���״��ȡ"));
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: PcapngStart
*  @Description   :: ��ʼ����pcapng����ʽ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
int SensorReader::PcapngStart()
{
	READER_DEBUG(_T("����2D�״��ȡ"));
	int ErrorDataFormat = 0;//��¼��������ݸ�ʽ�Ĵ���
	int ErrorRecvFrom = 0;//recvfrom���մ���
	SOCKADDR_IN addr_from;
	int n = 0;
	int len = sizeof(SOCKADDR);
	char buf[BUFFER_LEN];
	while (!destorySensorReaderStart)
	{
		if (INVALID_SOCKET != m_hSocket)
		{
			memset(buf, 0, sizeof(buf));
			n = recvfrom(m_hSocket, buf, BUFFER_LEN, 0, (SOCKADDR*)&addr_from, &len);
			//������յ������ݣ��ͽ������ݵĴ������ݴ������֡�Ĵ���
			if (n>0)
			{
				if (1240 == n)//�͵�������UDP��������ݣ������С����ȷ���Ĵ�С���������
				{
					ErrorDataFormat = 0;//֮ǰ�Ĵ������ݸ�ʽ���Բ���
					Info = std::make_shared<DataDefinition::raw_packetInfo>();//����
					if (Info == NULL)				//�����ڴ�ʧ�ܣ��˳�
					{
						return ER_pBuffNotCreated;
					}
					Info->size = 1240 + 42 + 16;//����+(mac+ip+udp)+pcap��ͷ
					Info->isnewFrame = 1;
					Info->FileCurFrameIndex = 0;
					//��ɶ�pcapͷ������
					int64_t nowtime = GetSysTimeMicros();//��ȡϵͳʱ�䣬��ȷ��΢��
					if (0 == nowtime)
					{
						return 1;//ϵͳʱ��δ�ܻ�ȡ
					}
					Info->pBuff.pcapPacket.pcap_head.ts.timestamp_s = (long)(nowtime / (1000 * 1000));
					Info->pBuff.pcapPacket.pcap_head.ts.timestamp_ms = (long)((nowtime % (1000 * 1000)) / 10);
					Info->pBuff.pcapPacket.pcap_head.capture_len = 1282;
					Info->pBuff.pcapPacket.pcap_head.len = 1282;
					//��ɶ�mac֡ͷ�����ɣ�ע��Դmac��ַ�˴�ΪĬ�ϵ�0x000000,000000
					memset(&(Info->pBuff.pcapPacket.mac_head.SrcMacAddress), 0, sizeof(Info->pBuff.pcapPacket.mac_head.SrcMacAddress));
					memset(&(Info->pBuff.pcapPacket.mac_head.DstMacAddress), 0xff, sizeof(Info->pBuff.pcapPacket.mac_head.DstMacAddress));
					Info->pBuff.pcapPacket.mac_head.Type = htons(0x0800);//��һ����IPЭ��
																  //��ɶ�IP֡ͷ������
					Info->pBuff.pcapPacket.ip_head.VersionAndHeaderLen = 0x45;
					Info->pBuff.pcapPacket.ip_head.TypeOfService = 0x00;
					Info->pBuff.pcapPacket.ip_head.TotalLenOfPacket = htons(1268);
					Info->pBuff.pcapPacket.ip_head.PacketID = 0;//��ʱ����Ĭ��ֵ0
					Info->pBuff.pcapPacket.ip_head.Sliceinfo = htons(0x4000);
					Info->pBuff.pcapPacket.ip_head.TTL = 255;
					Info->pBuff.pcapPacket.ip_head.TypeOfProtocol = 17;
					Info->pBuff.pcapPacket.ip_head.CheckSum = htons(0x5d5f);//��ͷУ���,��ʱ���������
					Info->pBuff.pcapPacket.ip_head.SourIp = htons(DestIPAddress);
					Info->pBuff.pcapPacket.ip_head.DestIp = htons(SrcIPAddress);//��ȡ������ip��
					//��ɶ�UDP��ͷ������
					Info->pBuff.pcapPacket.udp_head.m_usSourPort = htons(dest_connect_port);
					Info->pBuff.pcapPacket.udp_head.m_usDestPort = htons(local_connect_port);
					Info->pBuff.pcapPacket.udp_head.m_usLength = htons(1248);//1240+8
					Info->pBuff.pcapPacket.udp_head.m_usCheckSum = htons(0x0000);
					//��ɶ����ݵĿ���
					memcpy(&(Info->pBuff.pcapPacket.block), buf, 1240);//���ݳ�����1206

					if ((NULL != pRawProcess) && (!destorySensorReaderStart))
					{
						pRawProcess->ProcessAddDataPacket(Info);
					}
					if ((SAVE == savemode) && (NULL != pcap_saver) && (!destorySensorReaderStart))
					{//�Ǳ���ģʽ����saver���Ա���
						pcap_saver->AddPcapPacket(Info);
					}
					Info = NULL;
				}
				else//��һ�µ����ݸ�ʽ
				{
					ErrorDataFormat++;
				}
			}
			else if (-1 == n)//����ʧ��
			{
				ErrorRecvFrom++;
				Sleep(1);
			}
			n = 0;
		}

		if (ErrorRecvFrom > 100)
		{
			return ER_OutTimeOfConnect;//��ʱ
		}

		//��һЩ״̬�����ж�
		if (ErrorDataFormat > 200)
		{//����200�εĴ������ݸ�ʽ����Ϊ��Э��ѡȡ����
			return ER_UnSureOfProtocol;
		}
		BeginSleep();//����һ֮֡�����˯��
	}
	READER_DEBUG(_T("�˳�2D�״��ȡ"));
	return RaView_NO_ERROR;
}

///**************************************************************************
//*  @Function Name :: PcapStart
//*  @Description   :: ��ʼ����pcap����ʽ
//*  @Author        :: zzw
//*  @Parameters    :: None
//*
//*  @Return        :: �����Ƿ�ɹ�
//**************************************************************************/
//int SensorReader::PcapStart()
//{
//	int outTimeNumber = 0;//��¼��ȡ��ʱ�Ĵ���
//	int ErrorDataFormat = 0;//��¼��������ݸ�ʽ�Ĵ���
//	while (!destorySensorReaderStart)
//	{
//		if (NULL != adhandle)						//�������ֲ�Ϊ�ղſ��Լ���
//		{
//			struct pcap_pkthdr *header;
//			const u_char *pkt_data;
//			int res = pcap_next_ex(adhandle, &header, &pkt_data);
//			if (res > 0)							//��ȡ�ɹ�
//			{
//				outTimeNumber = 0;//��ʱ�����Ѿ����Բ���
//				if (1248 == header->caplen)			//���ж��Ƿ���1248(1206 + 42)
//				{
//					ErrorDataFormat = 0;//֮ǰ�Ĵ������ݸ�ʽ���Բ���
//					Info = std::make_shared<DataDefinition::raw_packetInfo>();//����
//					if (Info == NULL)				//�����ڴ�ʧ�ܣ��˳�
//					{
//						return ER_pBuffNotCreated;
//					}
//					Info->size = 1248 + 16;
//					Info->isnewFrame = 1;
//					Info->FileCurFrameIndex = 0;
//					memcpy(&(Info->pBuff.pcapPacket.pcap_head), header, sizeof(Info->pBuff.pcapPacket.pcap_head));//һ����С��
//					memcpy(&(Info->pBuff.pcapPacket.mac_head), pkt_data, header->caplen);//caplen==1248
//					if ((NULL != pRawProcess) && (!destorySensorReaderStart))
//					{
//						pRawProcess->ProcessAddDataPacket(Info);
//						//pRawProcess->DataProcess(Info);
//					}
//					if ((SAVE == savemode) && (NULL != pcap_saver) && (!destorySensorReaderStart))
//					{//�Ǳ���ģʽ����saver���Ա���
//						pcap_saver->AddPcapPacket(Info);
//					}
//					Info = NULL;
//				}
//				else//��������ݸ�ʽ
//				{
//					ErrorDataFormat++;
//				}
//			}
//			else if (res == 0)//��ʱ
//			{
//				outTimeNumber++;
//			}
//			else if (res == -1)//��ȡ���ݰ�����
//			{
//			}
//		}
//		//��һЩ״̬�����ж�
//		if (ErrorDataFormat > 200)
//		{//����200�εĴ������ݸ�ʽ����Ϊ��Э��ѡȡ����
//			return ER_UnSureOfProtocol;
//		}
//		if (outTimeNumber > 20)
//		{//����20�εĳ�ʱ������Ǵ������Ѿ���������������ʱ��Ҫ�л���
//			return ER_OutTimeOfConnect;
//		}
//		BeginSleep();//����һ֮֡�����˯��
//	}
//	return RaView_NO_ERROR;
//}
//
///**************************************************************************
//*  @Function Name :: PcapngStart
//*  @Description   :: ��ʼ����pcapng����ʽ
//*  @Author        :: zzw
//*  @Parameters    :: None
//*
//*  @Return        :: None
//**************************************************************************/
//int SensorReader::PcapngStart()
//{
//	int outTimeNumber = 0;//��¼��ȡ��ʱ�Ĵ���
//	int ErrorDataFormat = 0;//��¼��������ݸ�ʽ�Ĵ���
//	while (!destorySensorReaderStart)
//	{
//		if (NULL != adhandle)						//�������ֲ�Ϊ�ղſ��Լ���
//		{
//			struct pcap_pkthdr *header;
//			const u_char *pkt_data;
//			int res = pcap_next_ex(adhandle, &header, &pkt_data);
//			if (res > 0)							//��ȡ�ɹ�
//			{
//				outTimeNumber = 0;//��ʱ�����Ѿ����Բ���
//				if (1282 == header->caplen)			//���ж��Ƿ���1248(1206 + 42)
//				{
//					ErrorDataFormat = 0;//֮ǰ�Ĵ������ݸ�ʽ���Բ���
//					//Info = std::make_shared<DataDefinition::raw_packetInfo>();//����
//					//if (Info == NULL)				//�����ڴ�ʧ�ܣ��˳�
//					//{
//					//	return ER_pBuffNotCreated;
//					//}
//					//Info->size = 1248 + 16;
//					//Info->isnewFrame = 1;
//					//Info->FileCurFrameIndex = 0;
//					//memcpy(&(Info->pBuff.pcapngPacket.pcap_head), header, sizeof(Info->pBuff.pcapngPacket.pcap_head));//һ����С��
//					//memcpy(&(Info->pBuff.pcapngPacket.mac_head), pkt_data, header->caplen);//caplen==1282
//					//if ((NULL != pRawProcess) && (!destorySensorReaderStart))
//					//{
//					//	pRawProcess->ProcessAddDataPacket(Info);
//					//	//pRawProcess->DataProcess(Info);
//					//}
//					//if ((SAVE == savemode) && (NULL != pcap_saver) && (!destorySensorReaderStart))
//					//{//�Ǳ���ģʽ����saver���Ա���
//					//	pcap_saver->AddPcapPacket(Info);
//					//}
//					Info = NULL;
//				}
//				else//��������ݸ�ʽ
//				{
//					ErrorDataFormat++;
//				}
//			}
//			else if (res == 0)//��ʱ
//			{
//				outTimeNumber++;
//			}
//			else if (res == -1)//��ȡ���ݰ�����
//			{
//			}
//		}
//		//��һЩ״̬�����ж�
//		if (ErrorDataFormat > 200)
//		{//����200�εĴ������ݸ�ʽ����Ϊ��Э��ѡȡ����
//			return ER_UnSureOfProtocol;
//		}
//		if (outTimeNumber > 20)
//		{//����20�εĳ�ʱ������Ǵ������Ѿ���������������ʱ��Ҫ�л���
//			return ER_OutTimeOfConnect;
//		}
//		BeginSleep();//����һ֮֡�����˯��
//	}
//	return RaView_NO_ERROR;
//}

/**************************************************************************
*  @Function Name :: BeginSleep
*  @Description   :: �����Ƿ���ͣ�����������Ƿ����˯��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int SensorReader::BeginSleep()
{
	switch (Sensormode)
	{
	case STOP:
		while ((STOP == Sensormode) && (!destorySensorReaderStart))//ֹͣ��ʱ��һֱ�ڴ˴�˯�ߣ�ֱ���ٶȱ��ı�
		{
			Sleep(1);
		}
		break;
	case START:
		break;
	default://�������÷Ƿ��������ٶ����ó�1��λ����
		Sleep(0); return ER_InvaildSensorMode;
	}
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: ReStart
*  @Description   :: ���¿�ʼ��������ȡ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void SensorReader::ReStart()
{
	Sensormode = START;
}

/**************************************************************************
*  @Function Name :: Stop
*  @Description   :: ��ͣ��������ȡ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void SensorReader::Stop()
{
	Sensormode = STOP;
}

///**************************************************************************
//*  @Function Name :: SensorOpen
//*  @Description   :: �򿪴��������ӣ�ʹ��winpcap�Թ̶��˿ڽ��м���
//*  @Author        :: zzw
//*  @Parameters    :: IP:������IP port:�������˿�
//*					 Mask:����������
//*  @Return        :: �����Ƿ�ɹ�
//**************************************************************************/
//int SensorReader::SensorOpen(const char *IP, unsigned short port, const char *Mask)
//{
//	pcap_if_t *alldevs;
//	u_int netmask;
//	struct bpf_program fcode;
//	char errbuf[PCAP_ERRBUF_SIZE];
//
//	char str_netport[20];							//�˿��ַ������Ҳ�������й©
//	sprintf_s(str_netport, "%d", port);
//	char packet_filter[100];						//���ղ��Ľ��"udp and port 2368 and src 192.168.1.100"
//	memset(packet_filter, 0, sizeof(char) * 100);
//	strcat_s(packet_filter,50,"udp and port ");
//	strcat_s(packet_filter, 50, str_netport);
//	strcat_s(packet_filter, 50, " and src ");
//	strcat_s(packet_filter, 50, IP);
//																	 
//	if (pcap_findalldevs_ex((char *)PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)//���б����
//	{
//		return ER_PCAP_FindAllDevs;
//	}
//	pcap_if_t *d = FindSpecDev(alldevs, IP, Mask);	//��ȡĬ��IP���豸��pcap_if_t,������
//	if (NULL == d)									//Ѱ���ض��豸ʧ��
//	{
//		pcap_freealldevs(alldevs);
//		return ER_NotFindSpecDev;
//	}
//
//	//��������(���������ơ�����ȫ�����ݡ�����ģʽ����ʱʱ�䡢Զ�̻�����֤�����󻺳��)
//	adhandle = pcap_open(d->name,65536,PCAP_OPENFLAG_PROMISCUOUS,1000,NULL,errbuf);
//
//	if (pcap_datalink(adhandle) != DLT_EN10MB)		//����ֻ����̫������
//	{
//		pcap_freealldevs(alldevs);
//		adhandle = NULL;
//		return ER_OnlyWorkInEthernet;
//	}
//
//	if (d->addresses != NULL)						//��ýӿڵ�һ����ַ������
//	{
//		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
//	}
//	else //����ӿ�û�е�ַ����ô���Ǽ���һ��C�������
//	{
//		netmask = 0xffffff;
//	}
//
//	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) <0)//���������
//	{//�޷����룬����﷨
//		pcap_freealldevs(alldevs);
//		adhandle = NULL;
//		return ER_CannotCompileInPcap;
//	}
//
//	if (pcap_setfilter(adhandle, &fcode)<0)			//���ù�����
//	{
//		pcap_freealldevs(alldevs);
//		adhandle = NULL;
//		return ER_CannotSetFilterInPcap;
//	}
//
//	//�ڴ�ʱ�������������û�д���һ��UDP�����ͷ���û���ܹ���ȷ���Ӵ������ľ���
//	int iddx1 = 0;//��¼��ȡ�ɹ��Ĵ���
//	int iddx2 = 0;//��¼��ȡ��ʱ�Ĵ���
//	while (TRUE)
//	{
//		if (NULL != adhandle)						//�������ֲ�Ϊ�ղſ��Լ���
//		{
//			struct pcap_pkthdr *header;
//			const u_char *pkt_data;
//			int res = pcap_next_ex(adhandle, &header, &pkt_data);
//			if ((res > 0))							//��ȡ�ɹ�
//			{
//				iddx1++;
//			}
//			else if (res == 0)						//����10�γ�ʱ
//			{
//				iddx2++;
//			}
//			else if (res == -1)						//��ȡ���ݰ�����
//			{
//			}
//		}
//		if (iddx1 >= 6)
//		{
//			break;
//		}
//		if (iddx2 >= 6)
//		{
//			pcap_freealldevs(alldevs);
//			adhandle = NULL;
//			return ER_OutTimeOfConnect;				//������ʱ
//		}
//	}
//
//	pcap_freealldevs(alldevs);
//	return RaView_NO_ERROR;
//}

/**************************************************************************
*  @Function Name :: SensorOpen
*  @Description   :: �򿪴��������ӣ�ʹ��ʹ��UDP��������
*  @Author        :: zzw
*  @Parameters    :: IP:������IP port:�������˿�
*					 Mask:����������
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int SensorReader::SensorOpen(const char *IP, unsigned short dest_port, unsigned short local_port, const char *Mask)
{
	SOCKADDR_IN m_addr_local;
	//SOCKADDR_IN m_addr_dest;

	WORD wVersionRequested;
	WSADATA wsaData;

	//DestIPAddress = ntohl(inet_addr(IP));
	//dest_connect_port = dest_port;
	local_connect_port = local_port;
	wVersionRequested = MAKEWORD(2, 2);

	//��Ҫ��İ汾(Winsock����߰汾)��������ϵͳ֧�ֵ���װ汾(����),��ô�ú���   
	//�����ɹ� ������WSADATA.WhighVersion�з���ϵͳ֧�ֵ���߰汾,��WSADATA.wVersion��   
	//����ϵͳ֧�ֵ���߰�(����)�� wVersionRequested ֮��Ľ�Сֵ��
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return 1;//Windows Sockets ���ʼ��ʧ��.
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return 1;
	}

	//����udp ���׽���
	m_hSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//m_hSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_hSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return 1;//����socketʧ��
	}
	int nRecvBufSize;
	int nLen = sizeof(int);

	//�����׽��ֵ�ĳ��ѡ�
	getsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nRecvBufSize, &nLen);
	if (nRecvBufSize < 64 * BUFFER_LEN)
	{
		int nRecvBufSizeNew = 64 * BUFFER_LEN;
		setsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (char *)&nRecvBufSizeNew, sizeof(int));//���û������Ĵ�С
	}

	//timeval tv_out;
	//tv_out.tv_sec = 3;//�ȴ�3��
	//tv_out.tv_usec = 0;
	//setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv_out, sizeof(tv_out));

	//m_addr_dest.sin_family = AF_INET;
	//m_addr_dest.sin_addr.S_un.S_addr = htonl(DestIPAddress);
	////m_addr_dest.sin_addr.s_addr = htonl(DestIPAddress);
	////m_addr_dest.sin_port = htons(connect_port);
	//m_addr_dest.sin_port = htons(dest_connect_port);

	m_addr_local.sin_family = AF_INET;
	m_addr_local.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//m_addr_local.sin_addr.S_un.S_addr = inet_addr("192.168.1.10");
	//m_addr_local.sin_addr.s_addr = htonl(INADDR_ANY);
	m_addr_local.sin_port = htons(local_connect_port);//��һ������ָ���˿�

	err = bind(m_hSocket, (SOCKADDR*)&m_addr_local, sizeof(SOCKADDR));//��һ�����ض˿ڣ�sendto��ʱ���local�˿ڷ��͵�dest�˿�
	if (err != 0)
	{
		closesocket(m_hSocket);
		WSACleanup();
		return 1;
	}
	////��ȡ������udpͨ�ŵ�ip��
	//err = connect(m_hSocket, (struct sockaddr *)&DestIPAddress, sizeof(DestIPAddress));
	//if (err != 0) {
	//	closesocket(m_hSocket);
	//	WSACleanup();
	//	return 1;
	//}
	//int address_len = sizeof(SrcIPAddress);
	//getsockname(m_hSocket, (struct sockaddr *)&SrcIPAddress, &address_len);//��ȡudpͨ�ŵı���ip��ַ
	//int a = address_len;
	return RaView_NO_ERROR;
}

///**************************************************************************
//*  @Function Name :: SensorClose
//*  @Description   :: �رմ���������
//*  @Author        :: zzw
//*  @Parameters    :: None
//*
//*  @Return        :: None
//**************************************************************************/
//void SensorReader::SensorClose()
//{//ֻ�Ǽ�����ֻ��Ҫ����������ӿ��ÿհ�
//	adhandle = NULL;
//}

/**************************************************************************
*  @Function Name :: SensorClose
*  @Description   :: �رմ���������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void SensorReader::SensorClose()
{//ֻ�Ǽ�����ֻ��Ҫ����������ӿ��ÿհ�
	if (INVALID_SOCKET != m_hSocket) {
		closesocket(m_hSocket);
	}
	m_hSocket = INVALID_SOCKET;
	WSACleanup();
}

///**************************************************************************
//*  @Function Name :: FindSpecDev
//*  @Description   :: ��ȡĬ��IP���豸��pcap_if_t,�����з���,����NULL˵��ʧ��
//*  @Author        :: zzw
//*  @Parameters    :: alldevs:ָ�����п��������豸��ָ�� IP:��Ҫ���ӵ�ip��ַ
//*					 Mask:��Ҫ���ӵ�����
//*  @Return        :: None
//**************************************************************************/
//pcap_if_t *SensorReader::FindSpecDev(pcap_if_t *alldevs, const char *IP, const char *Mask)
//{
//	pcap_if_t *d;
//	unsigned long To_Connect_IP = inet_addr(IP);
//	unsigned long Default_gateway = inet_addr(Mask); //����ӿ�û�е�ַ����ô������һ��C������
//	for (d = alldevs; d; d = d->next)
//	{//���豸һ��һ�����IP
//		pcap_addr_t *a = NULL;
//
//		/* IP addresses */
//		for (a = d->addresses; a; a = a->next) {
//			if (AF_INET == a->addr->sa_family)
//			{
//				if (a->addr)
//				{//�����ַ���ڵĻ�
//					unsigned long ul_ip = ((struct sockaddr_in *)a->addr)->sin_addr.s_addr;
//					if (a->netmask)
//					{//������ش��ڣ�ʹ������
//						unsigned long ul_gateway = ((struct sockaddr_in *)a->netmask)->sin_addr.s_addr;
//						if ((To_Connect_IP & ul_gateway) == (ul_ip & ul_gateway))
//						{
//							return d;
//						}
//					}
//					else
//					{//ʹ��Ĭ�ϵ�255.255.255.0
//						if ((To_Connect_IP & Default_gateway) == (ul_ip & Default_gateway))
//						{//ȷ������ͬһ������֮��
//							return d;
//						}
//					}
//				}
//			}
//		}
//	}
//	//Ѱ��ʧ��
//	return NULL;
//}
