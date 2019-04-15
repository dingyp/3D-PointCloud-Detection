#include "stdafx.h"
#include "SaveReader.h"
#include "WinPcap\pcap.h"
#include "DataDefinition\PcapStructure.h"
#include "ErrorManager\ErroCodeDefine.h"

using namespace DataReader;

/**************************************************************************
*  @Function Name :: SaveReader
*  @Description   :: SaveReader��Ĺ��캯��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
SaveReader::SaveReader()
{
	destorySaveReader = true;
	pcap_saver = NULL;
	mutex_savePacketList.lock();
	savePacket_list.clear();
	mutex_savePacketList.unlock();
	beginSaveID = NULL;
	FullSaveCallBack = NULL;
	RTAdress = NULL;
	READER_DEBUG(_T("��������SaveReader"));
}

/**************************************************************************
*  @Function Name :: ~SaveReader
*  @Description   :: SaveReader�����������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
SaveReader::~SaveReader()
{
	destorySaveReader = true;
	pcap_saver = NULL;
	mutex_savePacketList.lock();				//����pcap�����л�����
	savePacket_list.clear();
	mutex_savePacketList.unlock();
	beginSaveID = NULL;
	FullSaveCallBack = NULL;
	RTAdress = NULL;
	READER_DEBUG(_T("����ע��SaveReader"));
}

/**************************************************************************
*  @Function Name :: Init
*  @Description   :: �ļ�����ĳ�ʼ��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
int SaveReader::Init()
{
	if (!destorySaveReader)						//û��Init������˱����ļ��̣߳��˳�
	{
		return ER_SaveReaderNotNULL;
	}
	mutex_saveReader.lock();
	if (NULL != pcap_saver)
	{
		mutex_saveReader.unlock();
		return ER_SaveStreamNotNULL;
	}
	pcap_saver = new std::ofstream();
	if (NULL == pcap_saver)						//���������ļ��������˳�
	{
		mutex_saveReader.unlock();
		return ER_CreatePcapSaver;
	}
	mutex_saveReader.unlock();
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: Destory
*  @Description   :: �˳��ļ��������ӣ��ͷ���Դ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void SaveReader::Destory()
{
	destorySaveReader = true;
	while (NULL != beginSaveID) { Sleep(1); };
	mutex_saveReader.lock();
	if (pcap_saver)
	{
		pcap_saver->close();
		delete(pcap_saver);
		pcap_saver = NULL;
	}
	mutex_saveReader.unlock();
}

/**************************************************************************
*  @Function Name :: SaveFileOpen
*  @Description   :: �򿪱����ļ�����
*  @Author        :: zzw
*  @Parameters    :: savepath:�����ļ���·�� LiDAR_Protocol:�ļ������Э��
*					 SaveEndStop:�ļ��洢̫���Ļص����� pParam:�ص���������Ĳ���
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int SaveReader::SaveFileOpen(const char* savepath, DataDefinition::LiDAR_Protocol SetProtocol, void(*SaveEndStop)(LPVOID pParam), LPVOID pParam)
{
	mutex_saveReader.lock();
	if (NULL == pcap_saver)						//û�б����ļ������˳�
	{
		mutex_saveReader.unlock();
		return ER_SaveReaderIsNULL;
	}
	mutex_saveReader.unlock();
	Protocol = SetProtocol;
	if ((NULL == SaveEndStop) || (NULL == pParam))
	{
		return ER_SaveFullCallBackIsNULL;
	}
	FullSaveCallBack = SaveEndStop;
	RTAdress = pParam;
	mutex_saveReader.lock();
	pcap_saver->open(savepath, std::ios_base::out | std::ios_base::binary);
	if (pcap_saver->bad())
	{
		mutex_saveReader.unlock();
		return ER_OpenSaveFile;
	}
	mutex_saveReader.unlock();
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: SaveFileClose
*  @Description   :: �رձ����ļ�����
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int SaveReader::SaveFileClose()
{
	mutex_saveReader.lock();
	if (NULL == pcap_saver)						//û�б����ļ������˳�
	{
		mutex_saveReader.unlock();
		return ER_SaveReaderIsNULL;
	}
	pcap_saver->close();
	mutex_saveReader.unlock();
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: AddPcapPacket
*  @Description   :: ��ԭʼpcap����ӵ��洢������
*  @Author        :: zzw
*  @Parameters    :: pInfo:ԭʼ���ݰ���һЩ��Ϣ
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int SaveReader::AddPcapPacket(std::shared_ptr<DataDefinition::raw_packetInfo> &pInfo)
{
	mutex_savePacketList.lock();
	savePacket_list.push_back(pInfo);
	mutex_savePacketList.unlock();
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: Start
*  @Description   :: ����һ���߳̽��еȴ�д���ļ�
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int SaveReader::Start()
{
	if (NULL != beginSaveID)					//ֻ���ļ������߳�û�д�����ʱ��ſ�ʼ����
	{
		return BeginSaveIDIsNotNULL;
	}

	beginSaveID = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)BeginDataSave, this, 0, 0);//�����ļ�д�߳�
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: BeginDataSave
*  @Description   :: �ļ�д�̺߳���
*  @Author        :: zzw
*  @Parameters    :: pParam:�ص������еĲ���
*					
*  @Return        :: None
**************************************************************************/
void SaveReader::BeginDataSave(LPVOID pParam)
{
	if (NULL == pParam)							//FileReaderΪ�գ��˳�
	{
		return;
	}
	SaveReader *pSReader = (SaveReader *)pParam;
	pSReader->mutex_saveReader.lock();
	if (NULL == pSReader->pcap_saver)			//�ļ���û�д򿪣��˳�
	{
		pSReader->mutex_saveReader.unlock();
		pSReader->beginSaveID = NULL;				//�������ļ�ID�ÿ�
		return;
	}
	pSReader->mutex_saveReader.unlock();
	if ((NULL == pSReader->FullSaveCallBack)||(NULL == pSReader->RTAdress))//����̫���ص�����Ϊ��
	{
		pSReader->mutex_saveReader.lock();
		pSReader->pcap_saver->close();	
		pSReader->mutex_saveReader.unlock();
		pSReader->beginSaveID = NULL;
		return;
	}

	if (LIDAR_HLP16 == pSReader->Protocol)//16���״��ļ�ͷ����
	{
		pcap_file_header pfh;						//pcap�ļ�ͷ(24)
		/*����Ĭ�ϵ�pcapheader*/
		pfh.magic = 0xa1b2c3d4;
		pfh.version_major = 2;
		pfh.version_minor = 4;
		pfh.thiszone = 0;
		pfh.sigfigs = 0;
		pfh.snaplen = 65535;
		pfh.linktype = 1;
		pSReader->mutex_saveReader.lock();
		pSReader->pcap_saver->write((char *)&pfh, sizeof(pfh));
		pSReader->mutex_saveReader.unlock();
	}
	else if (LIDAR_2D == pSReader->Protocol)//2D�״��ļ�ͷ����
	{
		BlockHead blockhead;							//pcapng�п�Ŀ�ͷ������ͬ�Ľ�ϣ���ΪҪ�ж���ʲô�飬�Լ����ж�������ȶ�ȡ��ͷ�����ж�
		SHB section_header_block;//��ͷ��
		IDB interface_description_block;//�ӿ�������
		blockhead.BlockType = 0x0a0d0d0a;
		blockhead.BlockLen = 32;
		section_header_block.Order = 0x1a2b3c4d;
		section_header_block.Major_Version = 1;
		section_header_block.Minor_Version = 0;
		section_header_block.Section_length[0] = 0xffffffff;
		section_header_block.Section_length[1] = 0xffffffff;
		//ʵ��SHB������
		pSReader->mutex_saveReader.lock();
		pSReader->pcap_saver->write((char *)&blockhead, sizeof(blockhead));
		pSReader->pcap_saver->write((char *)&section_header_block, sizeof(section_header_block));
		unsigned int blockLen = 0;
		pSReader->pcap_saver->write((char *)&blockLen, sizeof(blockLen));//��ѡ�ֶεĽ���ѡ���һ�����в����ظ�
		blockLen = blockhead.BlockLen;
		pSReader->pcap_saver->write((char *)&blockLen, sizeof(blockLen));
		pSReader->mutex_saveReader.unlock();
		
		blockhead.BlockType = 0x00000001;
		blockhead.BlockLen = 24;
		interface_description_block.LinkType=1;
		interface_description_block.Reserved=0;
		interface_description_block.SnapLen=0x0000ffff;
		//ʵ��IDB������
		pSReader->mutex_saveReader.lock();
		pSReader->pcap_saver->write((char *)&blockhead, sizeof(blockhead));
		pSReader->pcap_saver->write((char *)&interface_description_block, sizeof(interface_description_block));
		blockLen = 0;
		pSReader->pcap_saver->write((char *)&blockLen, sizeof(blockLen));//��ѡ�ֶεĽ���ѡ���һ�����в����ظ�
		blockLen = blockhead.BlockLen;
		pSReader->pcap_saver->write((char *)&blockLen, sizeof(blockLen));
		pSReader->mutex_saveReader.unlock();
	}
	else
	{
		pSReader->mutex_saveReader.lock();
		pSReader->pcap_saver->close();				//�رձ����ļ���
		pSReader->mutex_saveReader.unlock();
		pSReader->beginSaveID = NULL;				//�������ļ�ID�ÿ�
		return ;//��Ч��Э�飬����
	}
	
	std::shared_ptr<raw_packetInfo> OnePacket = NULL;
	pSReader->savepcapNumber = 0;//pcap�洢�������ó�0
	pSReader->destorySaveReader = false;
	//16���״�
	if (LIDAR_HLP16 == pSReader->Protocol)
	{
		while (!pSReader->destorySaveReader)
		{
			OnePacket = NULL;
			pSReader->mutex_savePacketList.lock();	//�ӱ���pcap���б���ȡ���ļ�������
			if ((!pSReader->destorySaveReader) && (pSReader->savePacket_list.size()))
			{
				OnePacket = pSReader->savePacket_list.front();
				pSReader->savePacket_list.begin() = pSReader->savePacket_list.erase(pSReader->savePacket_list.begin());
			}
			pSReader->mutex_savePacketList.unlock();//����
			if ((!pSReader->destorySaveReader) && (NULL != OnePacket))//��ȡ��pcap������д���
			{
				if (pSReader->savepcapNumber>2500000)//���3-4G֮��
				//if (pSReader->savepcapNumber > 2000)//���3-4G֮��
				{
					pSReader->destorySaveReader = true;
				}
				pSReader->mutex_saveReader.lock();
				pSReader->pcap_saver->write((char *)&(OnePacket->pBuff), OnePacket->size);
				pSReader->pcap_saver->flush();
				pSReader->mutex_saveReader.unlock();
				pSReader->savepcapNumber++;
			}
		}
	}
	else
	{
		BlockHead blockhead;//pcapng�п�Ŀ�ͷ������ͬ�Ľ�ϣ���ΪҪ�ж���ʲô�飬�Լ����ж�������ȶ�ȡ��ͷ�����ж�
		blockhead.BlockType = 0x00000006;
		blockhead.BlockLen = 1316;
		while (!pSReader->destorySaveReader)
		{
			OnePacket = NULL;
			pSReader->mutex_savePacketList.lock();	//�ӱ���pcap���б���ȡ���ļ�������
			if ((!pSReader->destorySaveReader) && (pSReader->savePacket_list.size()))
			{
				OnePacket = pSReader->savePacket_list.front();
				pSReader->savePacket_list.begin() = pSReader->savePacket_list.erase(pSReader->savePacket_list.begin());
			}
			pSReader->mutex_savePacketList.unlock();//����
			if ((!pSReader->destorySaveReader) && (NULL != OnePacket))//��ȡ��pcap������д���
			{
				if (pSReader->savepcapNumber>2500000)//���3-4G֮��
				//if (pSReader->savepcapNumber > 2000)//���3-4G֮��
				{
					pSReader->destorySaveReader = true;
				}
				pSReader->mutex_saveReader.lock();
				pSReader->pcap_saver->write((char *)&blockhead, sizeof(blockhead));
				unsigned int blockLen = 0;
				pSReader->pcap_saver->write((char *)&blockLen, sizeof(blockLen));//��ѡ�ֶεĽ���ѡ���һ�����в����ظ�
				pSReader->pcap_saver->write((char *)&(OnePacket->pBuff), OnePacket->size);
				unsigned short ze_zero = 0;
				pSReader->pcap_saver->write((char *)&ze_zero, sizeof(ze_zero));
				blockLen = blockhead.BlockLen;
				pSReader->pcap_saver->write((char *)&blockLen, sizeof(blockLen));
				pSReader->pcap_saver->flush();
				pSReader->mutex_saveReader.unlock();
				pSReader->savepcapNumber++;
			}
		}
	}

	pSReader->mutex_saveReader.lock();
	pSReader->pcap_saver->close();				//�رձ����ļ���
	pSReader->mutex_saveReader.unlock();
	pSReader->beginSaveID = NULL;				//�������ļ�ID�ÿ�
	if (pSReader->savepcapNumber > 2500000)//���3-4G֮��
	{
		if ((NULL != pSReader->FullSaveCallBack) && (NULL != pSReader->RTAdress))
		{
			pSReader->FullSaveCallBack(pSReader->RTAdress);//���ñ������ص�������һЩ����
		}
		MessageBox(NULL,_T("��ֹ�洢�ļ������Ѿ��ر��ļ�����"),0,0);
	}
	return;
}
