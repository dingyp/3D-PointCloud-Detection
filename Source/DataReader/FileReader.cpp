#include "stdafx.h"
#include <iostream>
#include <fstream>//����pcap�ļ��Ķ�ȡ
#include <vector>
#include "FileReader.h"
#include "WinPcap\pcap.h"
#include "ErrorManager\ErroCodeDefine.h"

using namespace DataReader;

/**************************************************************************
*  @Function Name :: FileReader
*  @Description   :: �ļ�������Ĺ��캯��
*  @Author        :: zzw
*  @Parameters    :: None
*					
*  @Return        :: None
**************************************************************************/
FileReader::FileReader()
{
	destoryFileReader = true;
	SetPlayState(READER_STOP, FORWORD, NOLOOP);				//��ʼ����״̬��STOP����ѭ��
	file_reader = NULL;										//�ļ����ÿ�
	offset_idxTable = NULL;
	cur_index = 0;											//֡�±����㣨�ļ��д�1��ʼ��
	pRawProcess = NULL;
	Info = NULL;
	reDraw = true;
	g_uTimerID = NULL;
	remain_number = 0;
	PlayStop = NULL;										//���ŵ�ֹͣ�Ļص���������
	RTAdress = NULL;
	isOffsetOK = true;
	Protocol = LIDAR_INVALID;								//��ʼ��ȡЭ��Ϊnull
	READER_DEBUG(_T("��������FileReader"));
}

/**************************************************************************
*  @Function Name :: ~FileReader
*  @Description   :: �ļ����������������
*  @Author        :: zzw
*  @Parameters    :: None
*					
*  @Return        :: None
**************************************************************************/
FileReader::~FileReader()
{
	destoryFileReader = true;
	SetPlayState(READER_STOP, FORWORD, NOLOOP);				//�ļ�һ��ʼ��Ĭ���ٶ�Ӧ����STOP����ѭ��
	if (file_reader)
	{
		file_reader->close();
		delete(file_reader);
		file_reader = NULL;
		cur_index = 0;										//֡�±����㣨�ļ��д�1��ʼ��
		Free_offsetTable();									//�ͷ�������
	}
	pRawProcess = NULL;
	Info = NULL;
	reDraw = false;
	g_uTimerID = NULL;
	remain_number = 0;
	PlayStop = NULL;
	RTAdress = NULL;
	isOffsetOK = false;
	Protocol = LIDAR_INVALID;								//��ȡЭ����null
	READER_DEBUG(_T("����ע��FileReader"));
}

/**************************************************************************
*  @Function Name :: Init
*  @Description   :: ��ʼ���ļ���ȡ
*  @Author        :: zzw
*  @Parameters    :: RawProcessPoint:����һ��pcap������ PlayEndStop:��ʼ�����ŵ�ֹͣ�Ļص�����
*					 pParam:�ص������еĲ��� SetProtocol:���ļ�ʹ�õ�Э������
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int FileReader::Init(DataProcess::RawProcess *RawProcessPoint, void(*PlayEndStop)(LPVOID pParam), LPVOID pParam, LiDAR_Protocol SetProtocol)
{
	if (!destoryFileReader)									//�Ѿ�������File���ӣ��˳�
	{
		return ER_NODESTORYFILREADER;
	}
	if (NULL != file_reader)
	{
		return ER_OPENSTREAMNOTNULL;
	}
	
	file_reader = new std::ifstream();						//�½�ifstream
	if (NULL == file_reader)
	{
		return ER_CREATEPCAPREADER;
	}
	if (NULL== RawProcessPoint)								//������ָ��Ϊ�գ��˳�
	{
		return ER_InitPointRawProcess;
	}
	pRawProcess = RawProcessPoint;
	PlayStop = PlayEndStop;
	RTAdress = pParam;
	Protocol = SetProtocol;									//�����ļ��Ķ�ȡЭ��
	reDraw = true;
	remain_number = 0;
	READER_DEBUG(_T("��ʼ��FileReader���"));
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: Destory
*  @Description   :: �˳��ļ���ȡ�̣߳��ͷ���Դ
*  @Author        :: zzw
*  @Parameters    :: None
*					 
*  @Return        :: None
**************************************************************************/
void FileReader::Destory()
{
	destoryFileReader = true;								//�˳�whileѭ��
	reDraw = false;
	SetPlayState(READER_STOP, FORWORD, NOLOOP);
	if (file_reader)
	{
		file_reader->close();
		delete(file_reader);
		file_reader = NULL;
		cur_index = 0;										//�±�Ӧ��1��ʼ
		Free_offsetTable();									//�ͷ�������
	}
	
	if (NULL != g_uTimerID)									//�ͷŶ�ý���ʱ��
	{
		timeKillEvent(g_uTimerID);
		g_uTimerID = NULL;
		READER_DEBUG(_T("�رն�ʱ��"));
	}

	pRawProcess = NULL;
	Info = NULL;
	remain_number = 0;
	PlayStop = NULL;
	RTAdress = NULL;
	isOffsetOK = false;
	Protocol = LIDAR_INVALID;
}

/**************************************************************************
*  @Function Name :: FileOpen
*  @Description   :: ���Ӷ�ȡ�ļ�
*  @Author        :: zzw
*  @Parameters    :: filepath:��ȡ�ļ�·��
*					 
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int FileReader::FileOpen(const char *filepath)
{
	if (NULL == file_reader)								//�ļ���Ϊ�գ��˳�
	{
		return ER_PcapReaderIsNULL;
	}
	if (READER_STOP != speed)								//�ٶȲ�Ϊ�㣬�˳�
	{
		return ER_OPENSTREAMNOTSTOP;
	}

	file_reader->open(filepath, std::ios_base::in | std::ios_base::binary);
	if (file_reader->bad())									//���ļ�ʧ�ܣ��˳�
	{
		return ER_OpenPcapFile;
	}

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)StepUpOffsetTable, this, 0, 0);//�����ļ�������
	Sleep(0);												//������Ϣ

	while (0 == GetFrameNumber())							//�ȴ������������
	{
		Sleep(0);
		if (false == isOffsetOK)							//���ܹ������������˳�
		{
			return ER_CannotCreateOffset;
		}
	}
	reDraw = true;
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: FileClose
*  @Description   :: �ر��ļ����ӣ�Ҫ�ͷ��������ļ���
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int FileReader::FileClose()
{
	if (NULL == file_reader)								//�ļ���Ϊ�գ��˳�
	{
		return ER_PcapReaderIsNULL;
	}
	if (READER_STOP != speed)								//�ٶȲ�Ϊ�㣬�˳�
	{
		return ER_OPENSTREAMNOTSTOP;
	}
	reDraw = false;
	file_reader->close();									//�ر��ļ���
	Free_offsetTable();										//�ͷ�������
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: Start
*  @Description   :: ������ȡ�ļ��߳�
*  @Author        :: zzw
*  @Parameters    :: status:���ñ�־λ,0��1��ʾspeed��ʼ��ʱ���ò��ţ��ʺ��ļ���ʾһ֡����ͣ
*					 2��ʾspeedһ��ʼ��һֱ��������ʺ϶�֡
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int FileReader::Start(int status)
{
	READER_DEBUG(_T("����FileReaderStart"));
	destoryFileReader = false;								//���߳̿�ʼ��־λ
	if (2 == status)
	{
		speed = ONE_SPEED;
	}
	while (!destoryFileReader)
	{
		if (NULL == file_reader)							//�ļ���Ϊ�գ��˳�
		{
			return ER_PcapReaderIsNULL;
		}
		if (FORWORD == playType)
		{
			if (LOOP == loop)								//ѭ�����ţ����򲥷ŵ������һ֡���ص���ʼ
			{
				if (cur_index>= GetFrameNumber())
				{
					cur_index = 0;
					//GetNextFrame(reDraw);
					//reDraw = false;
				}
			}
			GetNextFrame(reDraw);							//������һ֡
			reDraw = false;

			if ((LOOP != loop)&&(cur_index >= GetFrameNumber()))//��ѭ�����ţ��ж��Ƿ���Ҫ��ͣ���㱨������
			{
				Sleep(10);
				speed = READER_STOP;
				PlayStop(RTAdress);							//�ص�������֪ͨ�������Ѿ���ͣ
			}
		}
		else if (BACKWORD == playType)
		{
			if (LOOP == loop)								//ѭ�����ţ����򲥷ŵ����һ֡���ص����
			{
				if (cur_index<=1)
				{
					cur_index = GetFrameNumber();
					//GetBackwardFrame(reDraw);
					//reDraw = false;
				}
			}
			GetBackwardFrame(reDraw);						//������һ֡
			reDraw = false;

			if ((LOOP != loop) && (cur_index <= 1))			//��ѭ�����ţ��ж��Ƿ���ͣ�����㱨������
			{
				Sleep(10);
				speed = READER_STOP;
				PlayStop(RTAdress);							//�ص�������֪ͨ�������Ѿ���ͣ
			}
		}

		BeginSleep();										//ÿ��һ֡����˯��
	}
	READER_DEBUG(_T("�˳�FileReaderStart"));
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: GetIsStart
*  @Description   :: ��ȡ�߳��Ƿ���������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �߳��Ƿ���������
**************************************************************************/
bool FileReader::GetIsStart()
{
	return !destoryFileReader;
}

/**************************************************************************
*  @Function Name :: ReStart
*  @Description   :: ���¿����ļ���ȡ
*  @Author        :: zzw
*  @Parameters    :: status:���¿�ʼ�Ƿ���Ҫ�ж�����״̬�ı�־λ
*					 0��������պô����޷����ŵı�Ե״̬���ȵ������ٲ��ţ�1��ʾ������Ԥ��
*  @Return        :: None
**************************************************************************/
void FileReader::ReStart(int status)
{
	if (0== status)											//���¿�ʼǰ�ȿ�һ��״̬
	{
		if (FORWORD == playType)							//���򲥷ţ��������һ֡���ع鿪ʼ
		{
			if (cur_index >= GetFrameNumber())
			{
				cur_index = 0;
			}
		}
		else if (BACKWORD == playType)						//���򲥷ţ����ǵ�һ֡���������
		{
			if (cur_index <= 1)
			{
				cur_index = GetFrameNumber();
			}
		}
	}
	SetSpeed(DEFAULT_SPEED);
}

/**************************************************************************
*  @Function Name :: Stop
*  @Description   :: ��ͣ�ļ��Ķ�ȡ
*  @Author        :: zzw
*  @Parameters    :: None
*					
*  @Return        :: None
**************************************************************************/
void FileReader::Stop()
{
	SetSpeed(READER_STOP);									//���ò����ٶ�Ϊ0
}

/**************************************************************************
*  @Function Name :: SetPlayState
*  @Description   :: ���ò���״̬
*  @Author        :: zzw
*  @Parameters    :: set_speed:�����ٶ� set_type:��������
*					 set_loop:�Ƿ�ѭ������
*  @Return        :: None
**************************************************************************/
void FileReader::SetPlayState(const SPEED set_speed, \
	const PLAYTYPE set_type, const PLAYLOOP set_loop)
{
	SetSpeed(set_speed);
	switch (set_loop)
	{
	case NOLOOP:
		loop = NOLOOP; break;
	case LOOP:
		loop = LOOP; break;
	default:
		loop = NOLOOP; break;
	}
	switch (set_type)
	{
	case FORWORD:
		playType = FORWORD; break;
	case BACKWORD:
		playType = BACKWORD; break;
	default:
		playType = FORWORD; break;
	}
}

/**************************************************************************
*  @Function Name :: RessetState
*  @Description   :: ���ò���״̬ΪĬ�ϲ���״̬
*  @Author        :: zzw
*  @Parameters    :: None
*					 
*  @Return        :: None
**************************************************************************/
void FileReader::RessetState()
{
	SetPlayState();
}

/**************************************************************************
*  @Function Name :: SetSpeed
*  @Description   :: �����ٶ�
*  @Author        :: zzw
*  @Parameters    :: set_speed:���õĲ����ٶ�
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int FileReader::SetSpeed(const SPEED set_speed)
{
	switch (set_speed)
	{
	case READER_STOP:
		speed = READER_STOP; break;
	case ONE_SPEED:
		speed = ONE_SPEED; break;
	case DOUBLE_SPEED:
		speed = DOUBLE_SPEED; break;
	case THREE_SPEED:
		speed = THREE_SPEED; break;
	case FOUR_SPEED:
		speed = FOUR_SPEED; break;
	case EIGHT_SPEED:
		speed = EIGHT_SPEED; break;
	case SIXTEEN_SPEED:
		speed = SIXTEEN_SPEED; break;
	case ONE_POINT_FIVE_SPEED:
		speed = ONE_POINT_FIVE_SPEED; break;
	case HALF_SPEED:
		speed = HALF_SPEED; break;
	case QUARTER_SPEED:
		speed = QUARTER_SPEED; break;
	case POINT_ONE_SPEED:
		speed = POINT_ONE_SPEED; break;
	case DEFAULT_SPEED:
		speed = DEFAULT_SPEED; break;
	default:												//�����ٶȣ�ͳһ���ó�1��λ����
		speed = ONE_SPEED; return INVAILDSPEED;
	}
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: ACCSpeed
*  @Description   :: �ı��ٶȣ��ȼӿ�������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����ٶȱ仯����ٶ�
**************************************************************************/
SPEED FileReader::ACCSpeed()
{
	if (ONE_SPEED == speed|| DEFAULT_SPEED==speed)
	{
		speed = DOUBLE_SPEED;
	}
	else if (DOUBLE_SPEED == speed)
	{
		speed = THREE_SPEED;
	}
	else if (THREE_SPEED == speed)
	{
		speed = HALF_SPEED;
	}
	else if (HALF_SPEED == speed)
	{
		speed = ONE_THRID_SPEED;
	}
	else if (ONE_THRID_SPEED == speed)
	{
		speed = ONE_SPEED;
	}
	return speed;
}

////�����ٶ�
//void FileReader::DECSpeed()
//{
//}

/**************************************************************************
*  @Function Name :: BeginSleep
*  @Description   :: �������õ��ٶȽ�������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int FileReader::BeginSleep()
{
	switch (speed)
	{
	case READER_STOP:
		while ((READER_STOP == speed) && (!destoryFileReader))//��ͣʱ�����˯�ߣ�ֱ���ٶȸı�
		{
			Sleep(1);
		}
		break;
	}
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: StepUpOffsetTable
*  @Description   :: ��ʼ��������,ֻ���޸�һ��
*  @Author        :: zzw
*  @Parameters    :: pParam:�̲߳���������ָ��
*
*  @Return        :: None
**************************************************************************/
void FileReader::StepUpOffsetTable(LPVOID pParam)
{
	if (NULL == pParam)								//FileReaderΪ�գ��˳�
	{
		return;
	}
	FileReader *pFReader = (FileReader *)pParam;
	if (NULL == pFReader->file_reader)				//�ļ���û�д򿪣��˳�
	{
		pFReader->isOffsetOK = false;
		return;
	}
	if (READER_STOP != pFReader->speed)				//�����ٶȲ�Ϊ�㣬�˳�
	{
		pFReader->isOffsetOK = false;
		return;
	}
	if (pFReader->GetFrameNumber())					//����Ѿ������������˳�
	{
		pFReader->isOffsetOK = false;
		return;
	}

	switch (pFReader->Protocol)
	{
	case LIDAR_HLP16:
		pFReader->StepUpPcapOffsetTable();			//����pcap�ļ�������Ľ���
		break;
	case LIDAR_2D:
		pFReader->StepUpPcapngOffsetTable();		//����pcapng�ļ�������Ľ���
		break;
	}

}

/**************************************************************************
*  @Function Name :: StepUpPcapOffsetTable
*  @Description   :: ����pcap������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FileReader::StepUpPcapOffsetTable()
{
	//��pcap�ļ����
	pcap_file_header pfh;							//pcap�ļ�ͷ��24�ֽڴ�С
	unsigned short Angle = MAXWORD;					//�Ƕȵ��б��¼����

	std::vector<unsigned long long> index_table;	//ƫ������̬��������
	unsigned long long offset_index = 0;			//�ļ�ƫ����������

	file_reader->clear();
	file_reader->seekg(0, std::ios::beg);	//�Ƶ��ļ���ʼ��
	file_reader->read((char *)&pfh, sizeof(pfh));//��ȡpcap�ļ���ͷ
	offset_index += sizeof(pfh);//24
	//�����жϣ����Ƿ����pcap���ļ���ʽ
	if (!(pfh.magic == 0xa1b2c3d4 || pfh.magic == 0xd4c3b2a1))//������pcap�ļ���ʽ���˳�
	{
		isOffsetOK = false;
		return;
	}

	Info = std::make_shared<DataDefinition::raw_packetInfo>();
	if (Info == NULL)						//�����ڴ�ʧ�ܣ��˳�
	{
		isOffsetOK = false;
		return;
	}
	memset(&(Info->pBuff.pcapPacket), 0, sizeof(pcap_packet));
	while (!file_reader->eof() && file_reader->good())
	{
		file_reader->read((char *)&(Info->pBuff.pcapPacket.pcap_head), sizeof(Info->pBuff.pcapPacket.pcap_head));
		offset_index += sizeof(Info->pBuff.pcapPacket.pcap_head);//16

		if (1248 == Info->pBuff.pcapPacket.pcap_head.capture_len)//���ж��Ƿ���1248(1206 + 42)
		{
			file_reader->read((char *)&(Info->pBuff.pcapPacket.mac_head), sizeof(Info->pBuff.pcapPacket.mac_head));
			offset_index += sizeof(Info->pBuff.pcapPacket.mac_head);//14
			file_reader->read((char *)&(Info->pBuff.pcapPacket.ip_head), sizeof(Info->pBuff.pcapPacket.ip_head));
			offset_index += sizeof(Info->pBuff.pcapPacket.ip_head);//20
			file_reader->read((char *)&(Info->pBuff.pcapPacket.udp_head), sizeof(Info->pBuff.pcapPacket.udp_head));
			offset_index += sizeof(Info->pBuff.pcapPacket.udp_head);//8
			unsigned short tmp_Length = ((Info->pBuff.pcapPacket.udp_head.m_usLength & 0x00FF) << 8) | ((Info->pBuff.pcapPacket.udp_head.m_usLength & 0xFF00) >> 8);//��Ϊ����������д�С��ת��
			if (1214 == tmp_Length)//1206 + 8
			{
				file_reader->read((char*)Info->pBuff.pcapPacket.block, BLOCK_NUM * sizeof(HLP16_Block));

				//������һ֡���ж�����ȡ֡�е�һ���ǶȺ����һ���Ƕȣ��ж��Ƿ�����֡
				HLP16_Block *pBlock = (HLP16_Block*)Info->pBuff.pcapPacket.block;
				for (int i = 0; i < BLOCK_NUM; i++)
				{
					if (MAXWORD == Angle)			//һ֡�Ŀ�ʼ
					{
						Angle = pBlock->Angle;
					}
					if (pBlock->Angle < Angle)		//�Ƕȱ�ǰһ���Ƕ�С����Ϊ����֡
					{
						index_table.push_back(offset_index - 58);//��ǰ��58�ֽڣ�pcap���ݱ�ͷ��
					}
					Angle = pBlock->Angle;
					pBlock++;
				}
				offset_index += BLOCK_NUM * sizeof(HLP16_Block);//1200

				file_reader->read((char *)&(Info->pBuff.pcapPacket.tail), sizeof(Info->pBuff.pcapPacket.tail));
				offset_index += sizeof(Info->pBuff.pcapPacket.tail);//6
			}
		}
		else										//����1248�ĳ���˵������pcap�������㳤��֮���������
		{
			file_reader->read((char *)&(Info->pBuff.pcapPacket.mac_head), Info->pBuff.pcapPacket.pcap_head.capture_len);
			offset_index += Info->pBuff.pcapPacket.pcap_head.capture_len;//���̶�
		}
		memset(&(Info->pBuff.pcapPacket), 0, sizeof(pcap_packet));//�ÿ�pFReader->Info������
	}
	Info = NULL;
	file_reader->clear();
	file_reader->seekg(0, std::ios::beg);//�Ƶ��ļ���ʼ��
	Init_offsetTable(index_table);
	READER_DEBUG(_T("pcap�ļ������������"));
}

/**************************************************************************
*  @Function Name :: StepUpPcapngOffsetTable
*  @Description   :: ����pcapng������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FileReader::StepUpPcapngOffsetTable()
{
	//��pcapng�ļ����,����pcapng�ļ�������
	BlockHead blockhead;							//pcapng��һЩ��Ŀ�ͷ����ΪҪ�ж���ʲô�飬�Լ����ж������ֻ���ȶ�ȡ��ͷ�����ж�
	EPB enhanced_packet_block;						//pcapng��EPB��ʣ�ಿ��
	unsigned short Angle = MAXWORD;					//�Ƕȵ��б��¼����

	std::vector<unsigned long long> index_table;	//ƫ������̬��������
	unsigned long long offset_index = 0;			//�ļ�ƫ����������

	file_reader->clear();
	file_reader->seekg(0, std::ios::beg);			//�Ƶ��ļ���ʼ��
	file_reader->read((char *)(&blockhead), sizeof(blockhead));
	offset_index += sizeof(blockhead);				//8
	if (blockhead.BlockType != 0x0a0d0d0a)			//pcapng���ļ���ʽ�д���
	{
		isOffsetOK = false;
		return;
	}
	Info = std::make_shared<DataDefinition::raw_packetInfo>();
	if (Info == NULL)								//�����ڴ�ʧ�ܣ��˳�
	{
		isOffsetOK = false;
		return;
	}
	file_reader->read((char *)&(Info->pBuff.pcapngPacket), blockhead.BlockLen - sizeof(blockhead));//���֮�������
	offset_index += (blockhead.BlockLen - sizeof(blockhead));//��ʱ����֪������ж����Ϊ��ѡ�ȷ��

	file_reader->read((char*)(&blockhead), sizeof(blockhead));	//IDB
	offset_index += sizeof(blockhead);							//8
	if (blockhead.BlockType != 0x00000001)
	{//pcapng�ļ���ʽ�д���2
		isOffsetOK = false;
		return;
	}
	file_reader->read((char *)&(Info->pBuff.pcapngPacket), blockhead.BlockLen - sizeof(blockhead));
	offset_index += (blockhead.BlockLen - sizeof(blockhead));//��ʱ����֪������ж����Ϊ��ѡ�ȷ��

	memset(&(Info->pBuff.pcapngPacket), 0, sizeof(pcap_packet));
	while (!file_reader->eof() && file_reader->good())
	{
		file_reader->read((char*)(&blockhead), sizeof(blockhead));
		offset_index += sizeof(blockhead);//8
		if (blockhead.BlockType != 0x00000006)
		{//pcapng�������˳�����ǿ���Ŀ���������ģ���ʱ������ȥ
			file_reader->read((char *)&(Info->pBuff.pcapngPacket), blockhead.BlockLen - sizeof(blockhead));
			offset_index += (blockhead.BlockLen - sizeof(blockhead));//��ʱ����֪������ж����Ϊ��ѡ�ȷ��
			continue;
		}

		//��ʱ����������ǿ���ģ���ȡ�������Ϣ
		file_reader->read((char *)&(enhanced_packet_block), sizeof(enhanced_packet_block));
		offset_index += sizeof(enhanced_packet_block);//20
		int releaseLen = blockhead.BlockLen - sizeof(blockhead) - sizeof(enhanced_packet_block) - enhanced_packet_block.CapLen;
		if (enhanced_packet_block.CapLen != 1282)
		{//��ǿ���Ŀ�ĳߴ粻�ԣ�˵����ǿ���Ŀ���������ݴ������⣬���Խ�������
			file_reader->read((char *)&(Info->pBuff.pcapngPacket), enhanced_packet_block.CapLen);
			offset_index += enhanced_packet_block.CapLen;
			file_reader->read((char *)&(Info->pBuff.pcapngPacket), releaseLen);
			offset_index += releaseLen;
			continue;
		}
		//û�ж������ݿ�Ŀ�ͷPCAPNG_HEADER_SIGN(0xfef0010f),��Ϊ���������
		file_reader->read((char*)&(Info->pBuff.pcapngPacket.mac_head), sizeof(Info->pBuff.pcapngPacket)-sizeof(Info->pBuff.pcapngPacket.pcap_head)\
			-sizeof(Info->pBuff.pcapngPacket.rest));//1282
		offset_index += (sizeof(Info->pBuff.pcapngPacket) - sizeof(Info->pBuff.pcapngPacket.pcap_head) - sizeof(Info->pBuff.pcapngPacket.rest));
		if (Info->pBuff.pcapngPacket.dataHeader.sign != PCAPNG_HEADER_SIGN)
		{//�����ֵ����header��ʶ����˵����ȡ�����д������Խ�������
			file_reader->read((char *)&(Info->pBuff.pcapngPacket), releaseLen);
			offset_index += releaseLen;
			continue;
		}
		//���ݷ���Ԥ�ڣ����ж�ȡ
		//memcpy((char*)&(Info->pBuff.pcapngPacket.pcap_head), (char *)&(enhanced_packet_block.Timestamp_high), sizeof(Info->pBuff.pcapngPacket.pcap_head));//��EPB���������pcapͷ������ȡ
		for (int i = 0; i < 150; i++)
		{
			if (Info->pBuff.pcapngPacket.block[i].angle<=0xff00)//�������0xff00�����ʾ�����ݿ���Ч���������
			{
				if (MAXWORD == Angle)			//��ʼ֡�Ŀ�ʼ
				{
					Angle = Info->pBuff.pcapngPacket.block[i].angle;
				}
				if (Info->pBuff.pcapngPacket.block[i].angle < Angle)//�Ƕȱ�ǰһ���Ƕ�С����Ϊ����֡������Ŀ϶�����������ʼ����Ϊ���������ǲ�֪��������ǵڼ������Ժ�û�а취�����������ж�ȡ
				{
					index_table.push_back(offset_index - 1310);//����blockhead(8)+EPB�����(20)+1282(macͷ(14)+ipͷ(20)+udpͷ(8)+2D����ͷ(40)+2D���ݿ�(1200))������
				}
				Angle = Info->pBuff.pcapngPacket.block[i].angle;
			}
		}
		
		file_reader->read((char *)&(Info->pBuff.pcapngPacket), releaseLen);
		offset_index += releaseLen;
	}

	Info = NULL;
	file_reader->clear();
	file_reader->seekg(0, std::ios::beg);//�Ƶ��ļ���ʼ��
	Init_offsetTable(index_table);
	READER_DEBUG(_T("pcapng�ļ������������"));
}

/**************************************************************************
*  @Function Name :: GetFrameNumber
*  @Description   :: �õ��ļ����ܵ�֡��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �ļ���֡��
**************************************************************************/
int FileReader::GetFrameNumber()
{
	if (NULL == offset_idxTable)					//û�н���������
	{
		return NULL;
	}
	return offset_idxTable->index_number;
}

/**************************************************************************
*  @Function Name :: GetCurFrameNumber
*  @Description   :: �õ���ǰ֡���
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: ��ǰ֡���
**************************************************************************/
unsigned long long FileReader::GetCurFrameNumber()
{
	if (cur_index <= 0)
	{
		cur_index = 0;
	}
	if (cur_index > GetFrameNumber())
	{
		cur_index = GetFrameNumber();
	}
	return cur_index;
}

/**************************************************************************
*  @Function Name :: SetFrameNumber
*  @Description   :: ��������ĳһ֡����ʵֻ�����˱��λ������һ�ζ�ȡ���ܵ����Ӱ��
*  @Author        :: zzw
*  @Parameters    :: frameNumber:����ĳһ֡��֡���
*
*  @Return        :: None
**************************************************************************/
void FileReader::SetFrameNumber(unsigned int frameNumber)
{
	SPEED tmp_speed = speed;
	speed = READER_STOP;
	if ((frameNumber > 0) && (frameNumber<= GetFrameNumber()))//���õ�֡�ŷ��Ϸ�Χ�Ÿ�ֵ
	{
		cur_index = frameNumber;
		reDraw = true;
	}
	speed = tmp_speed;
}

/**************************************************************************
*  @Function Name :: Init_offsetTable
*  @Description   :: ��ʼ����������û����������֮ǰ�ǲ����ٴγ�ʼ������
*  @Author        :: zzw
*  @Parameters    :: idx_table:����������vector
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int FileReader::Init_offsetTable(std::vector<unsigned long long> &idx_table)
{
	if (NULL != GetFrameNumber())						//������Ϊ�գ��˳�
	{
		isOffsetOK = false;
		return ER_ALREADYOFFSETTABLE;
	}
	if (0 == idx_table.size())							//����vector��СΪ0���˳�
	{
		isOffsetOK = false;
		return ER_IDX_TABLEEMPTY;
	}
	if (READER_STOP != speed)							//�����ٶȲ�Ϊ�㣬�˳�
	{
		isOffsetOK = false;
		return ER_OFFSETTABLENOTSTOP;
	}

	offset_idxTable = new offset_table();
	if (NULL == offset_idxTable)						//���������ṹ�岻�ɹ����˳�
	{
		isOffsetOK = false;
		return ER_CREATEOFFSETTABLE;
	}
	offset_idxTable->index_number = 0;
	offset_idxTable->index_table = new unsigned long long[idx_table.size()];//����������
	if (NULL == offset_idxTable->index_table)			//�����������ɹ����˳�
	{
		delete(offset_idxTable);						//�ͷ������ṹ��
		isOffsetOK = false;
		return ER_CREATEOFFSETTABLEA;
	}

	for (int i = 0; i < idx_table.size(); i++)
	{
		offset_idxTable->index_table[i] = idx_table[i];
	}
	cur_index = 0;										//��ǰ�ļ�֡����
	offset_idxTable->index_number = idx_table.size();	//�����ܴ�С
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: Free_offsetTable
*  @Description   :: �ÿ�������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FileReader::Free_offsetTable()
{
	cur_index = 0;										//�ļ�֡�ÿ�
	if (offset_idxTable)
	{
		if (offset_idxTable->index_table)
		{
			delete(offset_idxTable->index_table);
			offset_idxTable->index_table = NULL;
		}
		delete(offset_idxTable);
		offset_idxTable = NULL;
	}
	READER_DEBUG(_T("�ļ������ɹ��ͷ�"));
}

/**************************************************************************
*  @Function Name :: GoToFrame
*  @Description   :: ���������ĳһ֡�����ʹ�ö�ʱ������ȡһ֡���˳�
*  @Author        :: zzw
*  @Parameters    :: FrameNumber:֡��
*					 reDraw:��֡�Ƿ��ܵ���ͣ��Ӱ��
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int FileReader::GoToFrame(const int FrameNumber, bool reDraw)
{
	if (destoryFileReader)								//�˳�whileѭ���������߳�
	{
		return ER_DESTORYFILEREADERSTART;
	}
	if (NULL == file_reader)
	{
		return ER_FILESTREAMNULL;
	}

	mutex_FileReader.lock();
	if (0 == GetFrameNumber())							//��������ִ����˳�
	{
		mutex_FileReader.unlock();
		return ER_READFILENOOFFSET;
	}
	if (FrameNumber > GetFrameNumber() || FrameNumber <= 0)//����FrameNumber���±��1��ʼ
	{
		mutex_FileReader.unlock();
		return ER_INDEXOUTRANGE;
	}
	
	file_reader->clear();								//����pcapReader,����ǰ�Ƚ���clear()
	file_reader->seekg(offset_idxTable->index_table[FrameNumber - 1], std::ios::beg);
	cur_index = FrameNumber;

	remain_number = 0;									//Ĭ��ʣ��Ϊ0
	switch (speed)
	{
	case ONE_SPEED:
		add_number = 24; break;
	case DOUBLE_SPEED:
		add_number = 48; break;
	case THREE_SPEED:
		add_number = 72; break;
	case HALF_SPEED:
		add_number = 12; break;
	case ONE_THRID_SPEED:
		add_number = 8; break;
	case DEFAULT_SPEED:
		add_number = 24; break;
	default:											//�������÷Ƿ��������ٶ����ó�1��λ����
		add_number = 24; break;
	}

	if (NULL == g_uTimerID)
	{
		//READER_DEBUG(_T("������ʱ��"));
		//READER_DEBUG(remain_number);
		//READER_DEBUG(add_number);
		g_uTimerID = timeSetEvent(24, 1, (LPTIMECALLBACK)CallBackMilliSecondProc, (DWORD_PTR)this, TIME_PERIODIC | TIME_KILL_SYNCHRONOUS);
		if (g_uTimerID == NULL)							//��ý���ʱ������ʧ�ܣ��˳�
		{
			mutex_FileReader.unlock();
			return ER_NotCreateTimer;
		}
	}

	while (destoryFileReader?(FALSE):(((cur_index<GetFrameNumber()) ? \
		(offset_idxTable->index_table[cur_index] >= file_reader->tellg()) : \
		(!file_reader->eof() && file_reader->good()))))	//û�дﵽ�ļ�ĩβ
	{
		if (READER_STOP == speed)
		{
			if (false == reDraw)						//�����ͣ���������˳�
			{
				mutex_FileReader.unlock();
				return RaView_NO_ERROR;
			}
		}

		if (remain_number)
		{
			remain_number--;
			Info = std::make_shared<DataDefinition::raw_packetInfo>();
			if (Info == NULL)							//�����ڴ�ʧ�ܣ��˳�
			{
				mutex_FileReader.unlock();
				return ER_pBuffNotCreated;
			}

			switch (Protocol)
			{
			case LIDAR_HLP16:
				ReadPcapPacket();			//����pcap�ļ�������Ľ���
				break;
			case LIDAR_2D:
				ReadPcapngPacket();			//����pcapng�ļ�������Ľ���
				break;
			}

			Info = NULL;
		}
		
	}
	if (destoryFileReader?(FALSE):(file_reader->eof() || (!file_reader->good())))//�ļ���ȡ������˳�
	{
		pRawProcess->End_TransFrame(true);
	}
	add_number = 0;
	remain_number = 0;

	mutex_FileReader.unlock();
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: ReadPcapPacket
*  @Description   :: ��ȡpcapһ����
*  @Author        :: zzw
*  @Parameters    :: None
*					 
*  @Return        :: None
**************************************************************************/
void FileReader::ReadPcapPacket()
{
	int Total_length = 0;
	file_reader->read((char *)&(Info->pBuff.pcapPacket.pcap_head), sizeof(Info->pBuff.pcapPacket.pcap_head));
	Total_length += sizeof(Info->pBuff.pcapPacket.pcap_head);

	if (1248 == Info->pBuff.pcapPacket.pcap_head.capture_len)//���ж��Ƿ���1248(1206 + 42)
	{
		file_reader->read((char *)&(Info->pBuff.pcapPacket.mac_head), 42);//һ�ζ�3��
		Total_length += 42;

		unsigned short tmp_Length = ((Info->pBuff.pcapPacket.udp_head.m_usLength & 0x00FF) << 8) | \
			((Info->pBuff.pcapPacket.udp_head.m_usLength & 0xFF00) >> 8);//��С��ת��
		if (1214 == tmp_Length)//1206 + 8
		{
			file_reader->read((char *)Info->pBuff.pcapPacket.block, 1206);
			Total_length += 1206;
			Info->size = Total_length;
			Info->isnewFrame = 0;				//0��ʾ��������֡
			Info->FileCurFrameIndex = GetCurFrameNumber();
			pRawProcess->ProcessAddDataPacket(Info);
			//pRawProcess->DataProcess(Info);
		}
	}
	else										//����1248�ĳ���˵������pcap�������㳤��֮���������
	{
		file_reader->read((char *)&(Info->pBuff.pcapPacket.mac_head), Info->pBuff.pcapPacket.pcap_head.capture_len);
	}
}

/**************************************************************************
*  @Function Name :: ReadPcapngPacket
*  @Description   :: ��ȡһ��pcapng��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FileReader::ReadPcapngPacket()
{
	BlockHead blockhead;							//pcapng��һЩ��Ŀ�ͷ����ΪҪ�ж���ʲô�飬�Լ����ж������ֻ���ȶ�ȡ��ͷ�����ж�
	EPB enhanced_packet_block;						//pcapng��EPB��ʣ�ಿ��

	int Total_length = 0;
	file_reader->read((char *)(&blockhead), sizeof(blockhead));//���������total_length����
	if (blockhead.BlockType == 0x00000006)
	{//��ʱ����������ǿ���ģ���ȡ�������Ϣ
		file_reader->read((char *)&(enhanced_packet_block), sizeof(enhanced_packet_block));
		int releaseLen = blockhead.BlockLen - sizeof(blockhead) - sizeof(enhanced_packet_block) - enhanced_packet_block.CapLen;
		if (enhanced_packet_block.CapLen == 1282)
		{//û�ж������ݿ�Ŀ�ͷPCAPNG_HEADER_SIGN(0xfef0010f),��Ϊ���������
			file_reader->read((char*)&(Info->pBuff.pcapngPacket.mac_head), sizeof(Info->pBuff) - sizeof(Info->pBuff.pcapngPacket.pcap_head)\
				- sizeof(Info->pBuff.pcapngPacket.rest));//1282
			if (Info->pBuff.pcapngPacket.dataHeader.sign == PCAPNG_HEADER_SIGN)
			{//���ݷ���Ԥ�ڣ����ж�ȡ
				memcpy((char*)&(Info->pBuff.pcapngPacket.pcap_head), (char *)&(enhanced_packet_block.Timestamp_high), sizeof(Info->pBuff.pcapngPacket.pcap_head));//��EPB���������pcapͷ������ȡ
				Total_length += sizeof(Info->pBuff.pcapngPacket) - sizeof(Info->pBuff.pcapngPacket.rest);//pcapheader(16)+mac(14)+ip(20)+udp(8)+2D���ݰ�ͷ(40)+2D���ݿ�(1200)

				Info->size = Total_length;
				Info->isnewFrame = 0;				//0��ʾ��������֡
				Info->FileCurFrameIndex = GetCurFrameNumber();
				pRawProcess->ProcessAddDataPacket(Info);
			}
		}
		else
		{//��ǿ���Ŀ�ĳߴ粻�ԣ�˵����ǿ���Ŀ���������ݴ������⣬���Խ�������
			file_reader->read((char *)&(Info->pBuff.pcapngPacket), enhanced_packet_block.CapLen);
		}
		file_reader->read((char*)&(Info->pBuff.pcapngPacket), releaseLen);//��Ϊ4�ֽڶ����Ե�ʣ����Ի���Ҫ��ȡ
	}
	else
	{//pcapng�������˳�����ǿ���Ŀ���������ģ���ʱ������ȥ
		file_reader->read((char *)&(Info->pBuff.pcapngPacket), blockhead.BlockLen - sizeof(blockhead));
	}
}

/**************************************************************************
*  @Function Name :: CallBackMilliSecondProc
*  @Description   :: ��ý���ʱ���ļ�ʱ����
*  @Author        :: zzw
*  @Parameters    :: wTimerID:��ʱ��ID msg:����
*					 dwUser:����û��ṩ�Ļص����� dw1:���� dw2:����
*  @Return        :: None
**************************************************************************/
void CALLBACK FileReader::CallBackMilliSecondProc(UINT wTimerID, UINT msg, DWORD_PTR dwUser, DWORD dw1, DWORD dw2)
{
	FileReader *pReader = (FileReader *)dwUser;
	pReader->remain_number += pReader->add_number;
}

/**************************************************************************
*  @Function Name :: GetNextFrame
*  @Description   :: �õ���һ֡
*  @Author        :: zzw
*  @Parameters    :: reDraw:��ʱ����֡�Ƿ��ܵ���ͣ��Ӱ��
*					 
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int FileReader::GetNextFrame(bool reDraw)
{
	cur_index++;//Ҳ���ǵ�һ֡û�ж���
	if (cur_index > GetFrameNumber())
	{
		cur_index = GetFrameNumber();		//����֮ǰ�Ƚ�cur_index�ÿ�
		return ER_CURINDEXOUTRANGE;			
	}
	return GoToFrame(cur_index, reDraw);
}

/**************************************************************************
*  @Function Name :: GetBackwardFrame
*  @Description   :: �õ���һ֡
*  @Author        :: zzw
*  @Parameters    :: reDraw:��ʱ����֡�Ƿ��ܵ���ͣ��Ӱ��
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int FileReader::GetBackwardFrame(bool reDraw)
{
	if ((0== cur_index)|| (1 == cur_index))
	{
		cur_index = 0;
		return ER_CURINDEXOUTRANGE;
	}
	cur_index--;
	return GoToFrame(cur_index, reDraw);
}

/**************************************************************************
*  @Function Name :: GoToStart
*  @Description   :: �ص�һ��ʼ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int FileReader::GoToStart()
{
	cur_index = 0;
	return GoToFrame(1,true);
}

/**************************************************************************
*  @Function Name :: GoToEnd
*  @Description   :: �Ƶ����
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int FileReader::GoToEnd()
{
	cur_index = GetFrameNumber();
	//return RaView_NO_ERROR;
	return GoToFrame(GetFrameNumber(),true);
}

/**************************************************************************
*  @Function Name :: PlayForward
*  @Description   :: ���򲥷�
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FileReader::PlayForward()
{
	playType = FORWORD;
}

/**************************************************************************
*  @Function Name :: PlayBackward
*  @Description   :: ���򲥷�
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FileReader::PlayBackward()
{
	playType = BACKWORD;
}

/**************************************************************************
*  @Function Name :: Loop
*  @Description   :: ��ʼѭ������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FileReader::Loop()
{
	loop = LOOP;
}

/**************************************************************************
*  @Function Name :: EndLoop
*  @Description   :: ����ѭ������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FileReader::EndLoop()
{
	loop = NOLOOP;
}

/**************************************************************************
*  @Function Name :: GetPlayType
*  @Description   :: �õ���������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: ��������
**************************************************************************/
PLAYTYPE FileReader::GetPlayType()
{
	return playType;
}