#include "stdafx.h"
#include "RawProcess.h"
#include "ErrorManager\ErroCodeDefine.h"
//��ԭʼ����ת����֡���ݵĴ���

using namespace DataProcess;

/**************************************************************************
*  @Function Name :: RawProcess
*  @Description   :: ԭʼ���ݰ�ת����ԭʼ֡��Ĺ��캯��
*  @Author        :: zzw
*  @Parameters    :: None
*                    
*  @Return        :: None
**************************************************************************/
RawProcess::RawProcess()
{
	destoryRawProcessStart = true;
	FullFrame = NULL;
	RTAdress = NULL;
	LastAngle_H = MAXWORD;
	pRawFrame = NULL;
	packet_list.clear();
	ALLFrameIndex = 0;
	Conver_Protocol = LIDAR_INVALID;					//���ö�ԭʼ���ݵ�ת��Э��
}

/**************************************************************************
*  @Function Name :: ~RawProcess
*  @Description   :: ԭʼ���ݰ�ת����ԭʼ֡�����������
*  @Author        :: zzw
*  @Parameters    :: None
*                  
*  @Return        :: None
**************************************************************************/
RawProcess::~RawProcess()
{
	UnSetProtocol();
	destoryRawProcessStart = true;
	FullFrame = NULL;
	RTAdress = NULL;
	LastAngle_H = MAXWORD;
	pRawFrame = NULL;
	packet_list.clear();
	ALLFrameIndex = 0;
	Conver_Protocol = LIDAR_INVALID;
}

/**************************************************************************
*  @Function Name :: SetProtocol
*  @Description   :: ����ת��Э��
*  @Author        :: zzw
*  @Parameters    :: Protocol:ת��Э������
*                   
*  @Return        :: None
**************************************************************************/
void RawProcess::SetProtocol(LiDAR_Protocol Protocol)
{
	Conver_Protocol = Protocol;
}

/**************************************************************************
*  @Function Name :: UnSetProtocol
*  @Description   :: ȡ��Э�������
*  @Author        :: zzw
*  @Parameters    :: None
*                    
*  @Return        :: None
**************************************************************************/
void RawProcess::UnSetProtocol()
{
	Conver_Protocol = LIDAR_INVALID;
}

/**************************************************************************
*  @Function Name :: BindFullFrameCallbackFunc
*  @Description   :: �Իص��������а�ע��,�൱��Init����
*  @Author        :: zzw
*  @Parameters    :: Callback:��һԭʼ֡�Ļص�����
*                    pParam:�ص������еĲ���
*  @Return        :: None
**************************************************************************/
void RawProcess::BindFullFrameCallbackFunc(void(*Callback)(std::shared_ptr<RawFrame> &lParam, LPVOID pParam), LPVOID pParam)
{
	FullFrame = Callback;
	RTAdress = pParam;
}

/**************************************************************************
*  @Function Name :: Start
*  @Description   :: ����ԭʼ���ݰ����̺߳���
*  @Author        :: zzw
*  @Parameters    :: None
*                    
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int RawProcess::Start()
{
	if ((NULL == FullFrame) || (NULL == RTAdress))
	{
		return RE_NotBindCallBackFunc;					//û�г�ʼ���ص��������˳�
	}
	std::shared_ptr<raw_packetInfo> OnePacket = NULL;
	destoryRawProcessStart = false;
	while (!destoryRawProcessStart)
	{
		mutex_packetList.lock();						//�Զ��в���������
		if (packet_list.size()>0)
		{
			OnePacket = packet_list.front();
			PacketList::iterator idx = packet_list.begin();
			packet_list.erase(idx);
		}
		mutex_packetList.unlock();						//����

		if (NULL != OnePacket)
		{
			DataProcess(OnePacket);						//�����pcap���д���
		}
		OnePacket = NULL;
	}
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: Destory
*  @Description   :: �˳��߳�
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void RawProcess::Destory()
{
	destoryRawProcessStart = true;
	FullFrame = NULL;
	RTAdress = NULL;
}

/**************************************************************************
*  @Function Name :: ProcessAddPcapPacket
*  @Description   :: ���ԭʼ���ݰ���ԭʼ��������
*  @Author        :: zzw
*  @Parameters    :: pInfo:ԭʼ���ݰ���һЩ��Ϣ
*
*  @Return        :: ������ԭʼ���ݰ�ʣ�����
**************************************************************************/
int RawProcess::ProcessAddDataPacket(std::shared_ptr<raw_packetInfo> &pInfo)
{
	mutex_packetList.lock();							//��ԭʼ���������У�����
	switch (Conver_Protocol)
	{
	case LIDAR_HLP16:
		AddPcapPacket(pInfo);
		break;
	case LIDAR_2D:
		AddPcapngPacket(pInfo);
		break;
	}
	int rest = packet_list.size();
	mutex_packetList.unlock();							//����
	return rest;
}

/**************************************************************************
*  @Function Name :: AddPcapPacket
*  @Description   :: ���pcap����ԭʼ��������
*  @Author        :: zzw
*  @Parameters    :: pInfo:ԭʼ���ݰ���һЩ��Ϣ
*
*  @Return        :: ������pcap��ʣ�����
**************************************************************************/
void RawProcess::AddPcapPacket(std::shared_ptr<raw_packetInfo> &pInfo)
{
	packet_list.push_back(pInfo);
	if (packet_list.size() > 100)						//��֡�жϣ����������ǹؼ�֡���ǶȲ�����340-10�ȣ�
	{
		PacketList::iterator iter;
		int count = 0;
		for (iter = packet_list.begin(); iter != packet_list.end(); iter++)
		{
			HLP16_Block *pBlock = (HLP16_Block*)((*iter)->pBuff.pcapPacket.block);
			if ((pBlock->Angle <= 34000) && (pBlock->Angle >= 1000))//�����ǹؼ�֡
			{
				iter = packet_list.erase(iter);
				count++;
			}
			if (2 == count)								//����֡�˳�
			{
				break;
			}
		}
	}
}

/**************************************************************************
*  @Function Name :: AddPcapngPacket
*  @Description   :: ���pcapng����ԭʼ������
*  @Author        :: zzw
*  @Parameters    :: pInfo:ԭʼ���ݰ���һЩ��Ϣ
*
*  @Return        :: ������pcapng��ʣ�����
**************************************************************************/
void RawProcess::AddPcapngPacket(std::shared_ptr<raw_packetInfo> &pInfo)
{
	packet_list.push_back(pInfo);
	if (packet_list.size() > 120)						//��֡�жϣ����������ǹؼ�֡���ǶȲ�����340-10�ȣ�
	{
		PacketList::iterator iter;
		int count = 0;
		for (iter = packet_list.begin(); iter != packet_list.end(); iter++)
		{
			Block *pBlock = (Block *)((*iter)->pBuff.pcapngPacket.block);
			if ((pBlock[149].angle < 34000) && (pBlock[0].angle > 1000))//�����ǹؼ�֡
			{
				iter = packet_list.erase(iter);
				count++;
			}
			if (2 == count)								//����֡�˳�
			{
				break;
			}
		}
	}
}

/**************************************************************************
*  @Function Name :: PcapPacketProcess
*  @Description   :: ����ԭʼpcap������
*  @Author        :: zzw
*  @Parameters    :: pInfo:ԭʼ���ݰ���һЩ��Ϣ
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int RawProcess::PcapPacketProcess(std::shared_ptr<raw_packetInfo> &pInfo)
{
	if (1264 != pInfo->size)								//��С����������֡���˳�
	{
		pInfo = NULL;
		return ER_InfoSizeIsNotRight;
	}
	HLP16_Block *pBlock = (HLP16_Block*)(pInfo->pBuff.pcapPacket.block);
	if (pBlock->Header != BLOCK_HEADER_VELODYNE)		//blockͷ����������������0xEEFF
	{
		return ER_BlockHeadIsNotRight;
	}

	bool cur_new_frame = false;							//��֡���λ

	mutex_Frame.lock();									//������֡������
	if (nullptr == pRawFrame)
	{
		cur_new_frame = true;
		ALLFrameIndex++;
		pRawFrame = std::make_shared<RawFrame>();
		pRawFrame->SetFrameIndex(pInfo->FileCurFrameIndex);//���õ�ǰ֡���±�
		LastAngle_H = MAXWORD;
	}
	mutex_Frame.unlock();								//����

	int newFrameIndex = 0;

	if (cur_new_frame)									//�������֡���ҳ���֡��ʼ��i(0-11)�����Ƕ���С���Ǹ�
	{
		HLP16_Block *pfindBlock = pBlock;
		unsigned short findMinAngle = pfindBlock->Angle;
		pfindBlock++;									//�±��1��ʼ��Ϊ�˶�Ӧ��������Ҫ��һ

		for (int idx = 1; idx < BLOCK_NUM; idx++)
		{
			if (findMinAngle > pfindBlock->Angle)		//ǰ��Ƕȴ��ڱ��Ƕȣ�˵���ҵ���һ֡��ʼ���λ���˳�
			{
				newFrameIndex = idx;
				for (int i = 0; i < newFrameIndex; i++)	//pBlock����ƶ���ͬBlock��
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
	for (int i = newFrameIndex; i < BLOCK_NUM; i++)		//����֡��ʼ����ʼ
	{
		PointR pr;
		pr.angleH = pBlock->Angle;

		if (!SwitchFrame(pr.angleH, BLOCK_POINTS, pInfo->FileCurFrameIndex, pInfo->isnewFrame))//����Ҫ���������˳�
		{
			pInfo = NULL;
			return RaView_NO_ERROR;
		}

		if (i < (BLOCK_NUM - 1))//�����һ����������һ������һ�����ˮƽ�Ƕ����������һ��������ǰһ������
		{
			nAngleAdd = (pBlock + 1)->Angle - (pBlock->Angle);
			if (nAngleAdd < 0)
			{
				nAngleAdd += 36000;
			}
			nAngleAdd /= 2;
		}

		for (int j = 0; j < BLOCK_POINTS; j++)
		{
			pr.distance = pBlock->Channel[j].Distance * 2ul;

			if (pr.distance <= 10)						//�����С�������������ݽ�������
			{
				continue;
			}
			pr.packID = i;								//���������ʱ�ò���
			pr.channel = j;
			pr.intensity = pBlock->Channel[j].Intensity;
			pr.timestamp_ms = pInfo->pBuff.pcapPacket.pcap_head.ts.timestamp_ms;

			if (j == 16)								//��16������ʱ����ˮƽ�Ƕ�ֵ
			{
				pr.angleH += (WORD)nAngleAdd;
			}
			if (pr.angleH >= 36000)						// ����360���Ժ�Ҫ��ȥ360����ֻ��һ�Σ��Ա��ڼ����������Ƿ�����
			{
				pr.angleH -= 36000;
			}

			mutex_Frame.lock();							//��֡����ӵ㣬����
			if (NULL != pRawFrame)
			{
				if (true == pRawFrame->TestAddPoint())//֡���滹�ɼӵ�
				{
					pRawFrame->AddPoint(pr);
				}
			}
			else
			{
				mutex_Frame.unlock();
				pInfo = NULL;
				return ER_SensorFrameAleadyClear;
			}
			mutex_Frame.unlock();						//����
		}
		pBlock++;
	}
	pInfo = NULL;
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: PcapngPacketProcess
*  @Description   :: ����ԭʼpcapng������
*  @Author        :: zzw
*  @Parameters    :: pInfo:ԭʼ���ݰ���һЩ��Ϣ
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int RawProcess::PcapngPacketProcess(std::shared_ptr<raw_packetInfo> &pInfo)
{
	if (1298 != pInfo->size)									//��С����������֡���˳�
	{
		pInfo = NULL;
		return ER_InfoSizeIsNotRight;
	}
	Block *pBlock = (Block *)(pInfo->pBuff.pcapngPacket.block);
	if (pInfo->pBuff.pcapngPacket.dataHeader.sign != PCAPNG_HEADER_SIGN)//blockͷ����������������0xFEF0010F
	{
		return ER_BlockHeadIsNotRight;
	}

	bool cur_new_frame = false;									//��֡���λ
	mutex_Frame.lock();											//������֡������
	if (nullptr == pRawFrame)
	{
		cur_new_frame = true;
		ALLFrameIndex++;
		pRawFrame = std::make_shared<RawFrame>();
		pRawFrame->SetFrameIndex(pInfo->FileCurFrameIndex);//���õ�ǰ֡���±�
		LastAngle_H = MAXWORD;
	}
	mutex_Frame.unlock();										//����
	int newFrameIndex = 0;
	if (cur_new_frame)											//�������֡���ҳ���֡��ʼ��i(0-149)�����Ƕ���С���Ǹ�
	{
		unsigned short findMinAngle = pBlock[0].angle;
		for (int idx = 1; idx < PCAPNG_BLOCK_NUM; idx++)
		{
			if (pBlock[idx].angle <= 0xff00)					//�������0xff00�����ʾ�����ݿ���Ч���������
			{
				if (findMinAngle > pBlock[idx].angle)			//ǰ��Ƕȴ��ڱ��Ƕȣ�˵���ҵ���һ֡��ʼ���λ���˳�
				{
					newFrameIndex = idx;
					break;
				}
				findMinAngle = pBlock[idx].angle;
			}
		}
	}
	for (int i = newFrameIndex; i < PCAPNG_BLOCK_NUM; i++)
	{
		if (pBlock[i].angle <= 0xff00)							//�������0xff00�����ʾ�����ݿ���Ч���������
		{
			PointR pr[2];										//�п�����˫����������
			pr[0].angleH = pBlock[i].angle;

			int HavingPointsNumber = 1;							//�ж����ǲ���˫���䣬�Դ��Ͳ�����SwitchFrame
			if (pBlock[i].far_range != 0)
			{
				HavingPointsNumber = 2;
				pr[1].angleH = pBlock[i].angle;
			}

			if (!SwitchFrame(pr[0].angleH, HavingPointsNumber, pInfo->FileCurFrameIndex, pInfo->isnewFrame))//����Ҫ���������˳�
			{
				pInfo = NULL;
				return RaView_NO_ERROR;
			}

			pr[0].distance = pBlock[i].close_range * pInfo->pBuff.pcapngPacket.dataHeader.ratio_length;
			pr[0].packID = 0;									//����ò�������ʱ���ó�0
			pr[0].channel = 0;									//ͨ����Ϊֻ��һ�����������ó�0
			pr[0].intensity = pBlock[i].close_intensity;
			pr[0].timestamp_ms = pInfo->pBuff.pcapngPacket.pcap_head.ts.timestamp_ms;

			if (2 == HavingPointsNumber)
			{
				pr[1].distance = pBlock[i].far_range * pInfo->pBuff.pcapngPacket.dataHeader.ratio_length;
				pr[1].packID = 0;								//����ò�������ʱ���ó�0
				pr[1].channel = 0;								//ͨ����Ϊֻ��һ�����������ó�0
				pr[1].intensity = pBlock[i].far_intensity;
				pr[1].timestamp_ms = pInfo->pBuff.pcapngPacket.pcap_head.ts.timestamp_ms;
			}

			mutex_Frame.lock();									//��֡����ӵ㣬����
			if (NULL != pRawFrame)
			{
				if (true == pRawFrame->TestAddPoint())			//֡���滹�ɼӵ�
				{
					if ((pr[0].distance >= 10)&&(pr[0].distance <=65000))//��������̫С�Լ�����̫Զ�ĵ㣬��Ϊû��������
					{
						pRawFrame->AddPoint(pr[0]);
					}
					if (2 == HavingPointsNumber)				//˫�����ʱ�����ʾ������
					{
						if ((pr[1].distance >= 10)&&(pr[1].distance <=65000))
						{
							pRawFrame->AddPoint(pr[1]);
						}
					}
				}
			}
			else
			{
				mutex_Frame.unlock();
				pInfo = NULL;
				return ER_SensorFrameAleadyClear;
			}
			mutex_Frame.unlock();								//����
		}
	}
	pInfo = NULL;
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: DataProcess
*  @Description   :: ����ԭʼ���ݰ�,pInfo�����־λ����Ƿ񴴽���֡
*  @Author        :: zzw
*  @Parameters    :: Info:ԭʼ���ݰ���һЩ��Ϣ
*                    
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int RawProcess::DataProcess(std::shared_ptr<raw_packetInfo> &pInfo)
{
	switch (Conver_Protocol)
	{
	case LIDAR_HLP16:
		return PcapPacketProcess(pInfo);
		break;
	case LIDAR_2D:
		return PcapngPacketProcess(pInfo);
		break;
	}
}

/**************************************************************************
*  @Function Name :: SwitchFrame
*  @Description   :: �ж��Ƿ���Ҫ�����ɵ�֡������������֡
*  @Author        :: zzw
*  @Parameters    :: Angle:��һ�����ݵ�ˮƽ�� BlockMinPoints:һ��Block�����ĸ���
*					 FileCurFrameIndex:��֡���±� newFrame:��һ֡���Ƿ���Ҫ������ȡ��һ֡
*  @Return        :: true:����������ȥ��false:���ٽ��д���
**************************************************************************/
bool RawProcess::SwitchFrame(unsigned short Angle, unsigned int BlockMinPoints, \
	int FileCurFrameIndex, bool newFrame)
{
	if (MAXWORD == LastAngle_H)
	{
		LastAngle_H = Angle;
	}

	mutex_Frame.lock();
	if (NULL == pRawFrame)
	{
		mutex_Frame.unlock();
		return ER_SensorFrameAleadyClear;
	}

	if (Angle < LastAngle_H								//����Ƕȱ�ǰһ��С������Ϊ���µ�һ֡
		|| pRawFrame->GetPointsNumber() >= (pRawFrame->GetMaxPointsInFrame() - BlockMinPoints))//���Frame��������һ��BLOCK��Ҳ�л�֡
	{
		mutex_Frame.unlock();

		End_TransFrame(false);							//�л�֡���׳���֡,���ó�ʼ״̬
		if (newFrame)									//������һ֡
		{
			ALLFrameIndex++;
			mutex_Frame.lock();
			pRawFrame = std::make_shared<RawFrame>();
			pRawFrame->SetFrameIndex(FileCurFrameIndex);//������֡���±�
			mutex_Frame.unlock();
			LastAngle_H = Angle;
		}
		return newFrame;
	}

	mutex_Frame.unlock();
	LastAngle_H = Angle;
	return true;
}

/**************************************************************************
*  @Function Name :: End_ClearFrame
*  @Description   :: ����ִ�֡
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int RawProcess::End_ClearFrame()
{
	LastAngle_H = MAXWORD;
	mutex_Frame.lock();
	if (nullptr == pRawFrame)						//֡�ǿյ�,�˳�
	{
		mutex_Frame.unlock();
		return ER_SensorFrameAleadyClear;
	}
	pRawFrame = nullptr;
	mutex_Frame.unlock();
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: End_TransFrame
*  @Description   :: ����ǰ֡��Ϊ��֡�����ͳ�ȥ
*  @Author        :: zzw
*  @Parameters    :: isWait:���ͳ�ȥ֮ǰ�Ƿ���Ҫ�ȵȴ�һ��ʱ�䣬��ֹ��ǰ֡����û�����ɺõ�
*
*  @Return        :: �����Ƿ�ɹ�
**************************************************************************/
int RawProcess::End_TransFrame(bool isWait)
{
	if (isWait)
	{
		Sleep(10);
	}
	LastAngle_H = MAXWORD;
	mutex_Frame.lock();
	if (nullptr== pRawFrame)							//֡�ǿյ�,�˳�
	{
		mutex_Frame.unlock();
		return ER_SensorFrameAleadyClear;
	}
	FullFrame(pRawFrame, RTAdress);
	pRawFrame = nullptr;
	mutex_Frame.unlock();
	return RaView_NO_ERROR;
}