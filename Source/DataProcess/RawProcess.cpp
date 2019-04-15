#include "stdafx.h"
#include "RawProcess.h"
#include "ErrorManager\ErroCodeDefine.h"
//对原始数据转换成帧数据的处理

using namespace DataProcess;

/**************************************************************************
*  @Function Name :: RawProcess
*  @Description   :: 原始数据包转换成原始帧类的构造函数
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
	Conver_Protocol = LIDAR_INVALID;					//设置对原始数据的转换协议
}

/**************************************************************************
*  @Function Name :: ~RawProcess
*  @Description   :: 原始数据包转换成原始帧类的析构函数
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
*  @Description   :: 设置转换协议
*  @Author        :: zzw
*  @Parameters    :: Protocol:转换协议类型
*                   
*  @Return        :: None
**************************************************************************/
void RawProcess::SetProtocol(LiDAR_Protocol Protocol)
{
	Conver_Protocol = Protocol;
}

/**************************************************************************
*  @Function Name :: UnSetProtocol
*  @Description   :: 取消协议的设置
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
*  @Description   :: 对回调函数进行绑定注册,相当于Init函数
*  @Author        :: zzw
*  @Parameters    :: Callback:满一原始帧的回调函数
*                    pParam:回调函数中的参数
*  @Return        :: None
**************************************************************************/
void RawProcess::BindFullFrameCallbackFunc(void(*Callback)(std::shared_ptr<RawFrame> &lParam, LPVOID pParam), LPVOID pParam)
{
	FullFrame = Callback;
	RTAdress = pParam;
}

/**************************************************************************
*  @Function Name :: Start
*  @Description   :: 处理原始数据包的线程函数
*  @Author        :: zzw
*  @Parameters    :: None
*                    
*  @Return        :: 运行是否成功
**************************************************************************/
int RawProcess::Start()
{
	if ((NULL == FullFrame) || (NULL == RTAdress))
	{
		return RE_NotBindCallBackFunc;					//没有初始化回调函数，退出
	}
	std::shared_ptr<raw_packetInfo> OnePacket = NULL;
	destoryRawProcessStart = false;
	while (!destoryRawProcessStart)
	{
		mutex_packetList.lock();						//对队列操作，加锁
		if (packet_list.size()>0)
		{
			OnePacket = packet_list.front();
			PacketList::iterator idx = packet_list.begin();
			packet_list.erase(idx);
		}
		mutex_packetList.unlock();						//解锁

		if (NULL != OnePacket)
		{
			DataProcess(OnePacket);						//对这个pcap进行处理
		}
		OnePacket = NULL;
	}
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: Destory
*  @Description   :: 退出线程
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
*  @Description   :: 添加原始数据包到原始包队列中
*  @Author        :: zzw
*  @Parameters    :: pInfo:原始数据包的一些信息
*
*  @Return        :: 队列中原始数据包剩余个数
**************************************************************************/
int RawProcess::ProcessAddDataPacket(std::shared_ptr<raw_packetInfo> &pInfo)
{
	mutex_packetList.lock();							//加原始包到队列中，加锁
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
	mutex_packetList.unlock();							//解锁
	return rest;
}

/**************************************************************************
*  @Function Name :: AddPcapPacket
*  @Description   :: 添加pcap包到原始包队列中
*  @Author        :: zzw
*  @Parameters    :: pInfo:原始数据包的一些信息
*
*  @Return        :: 队列中pcap包剩余个数
**************************************************************************/
void RawProcess::AddPcapPacket(std::shared_ptr<raw_packetInfo> &pInfo)
{
	packet_list.push_back(pInfo);
	if (packet_list.size() > 100)						//满帧判断，舍弃两个非关键帧（角度不落在340-10度）
	{
		PacketList::iterator iter;
		int count = 0;
		for (iter = packet_list.begin(); iter != packet_list.end(); iter++)
		{
			HLP16_Block *pBlock = (HLP16_Block*)((*iter)->pBuff.pcapPacket.block);
			if ((pBlock->Angle <= 34000) && (pBlock->Angle >= 1000))//舍弃非关键帧
			{
				iter = packet_list.erase(iter);
				count++;
			}
			if (2 == count)								//满两帧退出
			{
				break;
			}
		}
	}
}

/**************************************************************************
*  @Function Name :: AddPcapngPacket
*  @Description   :: 添加pcapng包到原始队列中
*  @Author        :: zzw
*  @Parameters    :: pInfo:原始数据包的一些信息
*
*  @Return        :: 队列中pcapng包剩余个数
**************************************************************************/
void RawProcess::AddPcapngPacket(std::shared_ptr<raw_packetInfo> &pInfo)
{
	packet_list.push_back(pInfo);
	if (packet_list.size() > 120)						//满帧判断，舍弃两个非关键帧（角度不落在340-10度）
	{
		PacketList::iterator iter;
		int count = 0;
		for (iter = packet_list.begin(); iter != packet_list.end(); iter++)
		{
			Block *pBlock = (Block *)((*iter)->pBuff.pcapngPacket.block);
			if ((pBlock[149].angle < 34000) && (pBlock[0].angle > 1000))//舍弃非关键帧
			{
				iter = packet_list.erase(iter);
				count++;
			}
			if (2 == count)								//满两帧退出
			{
				break;
			}
		}
	}
}

/**************************************************************************
*  @Function Name :: PcapPacketProcess
*  @Description   :: 处理原始pcap包数据
*  @Author        :: zzw
*  @Parameters    :: pInfo:原始数据包的一些信息
*
*  @Return        :: 运行是否成功
**************************************************************************/
int RawProcess::PcapPacketProcess(std::shared_ptr<raw_packetInfo> &pInfo)
{
	if (1264 != pInfo->size)								//大小不符合正常帧，退出
	{
		pInfo = NULL;
		return ER_InfoSizeIsNotRight;
	}
	HLP16_Block *pBlock = (HLP16_Block*)(pInfo->pBuff.pcapPacket.block);
	if (pBlock->Header != BLOCK_HEADER_VELODYNE)		//block头部不符合正常定义0xEEFF
	{
		return ER_BlockHeadIsNotRight;
	}

	bool cur_new_frame = false;							//新帧标记位

	mutex_Frame.lock();									//创建新帧，加锁
	if (nullptr == pRawFrame)
	{
		cur_new_frame = true;
		ALLFrameIndex++;
		pRawFrame = std::make_shared<RawFrame>();
		pRawFrame->SetFrameIndex(pInfo->FileCurFrameIndex);//设置当前帧的下标
		LastAngle_H = MAXWORD;
	}
	mutex_Frame.unlock();								//解锁

	int newFrameIndex = 0;

	if (cur_new_frame)									//如果是新帧，找出新帧起始号i(0-11)，即角度最小的那个
	{
		HLP16_Block *pfindBlock = pBlock;
		unsigned short findMinAngle = pfindBlock->Angle;
		pfindBlock++;									//下标从1开始，为了对应，这里需要加一

		for (int idx = 1; idx < BLOCK_NUM; idx++)
		{
			if (findMinAngle > pfindBlock->Angle)		//前面角度大于本角度，说明找到下一帧起始标记位，退出
			{
				newFrameIndex = idx;
				for (int i = 0; i < newFrameIndex; i++)	//pBlock向后移动相同Block数
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
	for (int i = newFrameIndex; i < BLOCK_NUM; i++)		//从新帧起始处开始
	{
		PointR pr;
		pr.angleH = pBlock->Angle;

		if (!SwitchFrame(pr.angleH, BLOCK_POINTS, pInfo->FileCurFrameIndex, pInfo->isnewFrame))//不需要继续处理，退出
		{
			pInfo = NULL;
			return RaView_NO_ERROR;
		}

		if (i < (BLOCK_NUM - 1))//非最后一组数据与下一组数据一起计算水平角度增量，最后一组数据用前一次增量
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

			if (pr.distance <= 10)						//距离很小，视作无用数据进行舍弃
			{
				continue;
			}
			pr.packID = i;								//这个好像暂时用不到
			pr.channel = j;
			pr.intensity = pBlock->Channel[j].Intensity;
			pr.timestamp_ms = pInfo->pBuff.pcapPacket.pcap_head.ts.timestamp_ms;

			if (j == 16)								//第16个数据时增加水平角度值
			{
				pr.angleH += (WORD)nAngleAdd;
			}
			if (pr.angleH >= 36000)						// 大于360°以后要送去360，但只减一次，以便于检查接收数据是否有误
			{
				pr.angleH -= 36000;
			}

			mutex_Frame.lock();							//往帧里面加点，加锁
			if (NULL != pRawFrame)
			{
				if (true == pRawFrame->TestAddPoint())//帧里面还可加点
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
			mutex_Frame.unlock();						//解锁
		}
		pBlock++;
	}
	pInfo = NULL;
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: PcapngPacketProcess
*  @Description   :: 处理原始pcapng包数据
*  @Author        :: zzw
*  @Parameters    :: pInfo:原始数据包的一些信息
*
*  @Return        :: 运行是否成功
**************************************************************************/
int RawProcess::PcapngPacketProcess(std::shared_ptr<raw_packetInfo> &pInfo)
{
	if (1298 != pInfo->size)									//大小不符合正常帧，退出
	{
		pInfo = NULL;
		return ER_InfoSizeIsNotRight;
	}
	Block *pBlock = (Block *)(pInfo->pBuff.pcapngPacket.block);
	if (pInfo->pBuff.pcapngPacket.dataHeader.sign != PCAPNG_HEADER_SIGN)//block头部不符合正常定义0xFEF0010F
	{
		return ER_BlockHeadIsNotRight;
	}

	bool cur_new_frame = false;									//新帧标记位
	mutex_Frame.lock();											//创建新帧，加锁
	if (nullptr == pRawFrame)
	{
		cur_new_frame = true;
		ALLFrameIndex++;
		pRawFrame = std::make_shared<RawFrame>();
		pRawFrame->SetFrameIndex(pInfo->FileCurFrameIndex);//设置当前帧的下标
		LastAngle_H = MAXWORD;
	}
	mutex_Frame.unlock();										//解锁
	int newFrameIndex = 0;
	if (cur_new_frame)											//如果是新帧，找出新帧起始号i(0-149)，即角度最小的那个
	{
		unsigned short findMinAngle = pBlock[0].angle;
		for (int idx = 1; idx < PCAPNG_BLOCK_NUM; idx++)
		{
			if (pBlock[idx].angle <= 0xff00)					//如果大于0xff00，则表示本数据块无效，必须忽略
			{
				if (findMinAngle > pBlock[idx].angle)			//前面角度大于本角度，说明找到下一帧起始标记位，退出
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
		if (pBlock[i].angle <= 0xff00)							//如果大于0xff00，则表示本数据块无效，必须忽略
		{
			PointR pr[2];										//有可能是双反射两个点
			pr[0].angleH = pBlock[i].angle;

			int HavingPointsNumber = 1;							//判断是是不是双反射，以传送参数给SwitchFrame
			if (pBlock[i].far_range != 0)
			{
				HavingPointsNumber = 2;
				pr[1].angleH = pBlock[i].angle;
			}

			if (!SwitchFrame(pr[0].angleH, HavingPointsNumber, pInfo->FileCurFrameIndex, pInfo->isnewFrame))//不需要继续处理，退出
			{
				pInfo = NULL;
				return RaView_NO_ERROR;
			}

			pr[0].distance = pBlock[i].close_range * pInfo->pBuff.pcapngPacket.dataHeader.ratio_length;
			pr[0].packID = 0;									//这个用不到，暂时设置成0
			pr[0].channel = 0;									//通道因为只有一个，所以设置成0
			pr[0].intensity = pBlock[i].close_intensity;
			pr[0].timestamp_ms = pInfo->pBuff.pcapngPacket.pcap_head.ts.timestamp_ms;

			if (2 == HavingPointsNumber)
			{
				pr[1].distance = pBlock[i].far_range * pInfo->pBuff.pcapngPacket.dataHeader.ratio_length;
				pr[1].packID = 0;								//这个用不到，暂时设置成0
				pr[1].channel = 0;								//通道因为只有一个，所以设置成0
				pr[1].intensity = pBlock[i].far_intensity;
				pr[1].timestamp_ms = pInfo->pBuff.pcapngPacket.pcap_head.ts.timestamp_ms;
			}

			mutex_Frame.lock();									//往帧里面加点，加锁
			if (NULL != pRawFrame)
			{
				if (true == pRawFrame->TestAddPoint())			//帧里面还可加点
				{
					if ((pr[0].distance >= 10)&&(pr[0].distance <=65000))//舍弃距离太小以及距离太远的点，因为没有意义了
					{
						pRawFrame->AddPoint(pr[0]);
					}
					if (2 == HavingPointsNumber)				//双反射的时候会显示两个点
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
			mutex_Frame.unlock();								//解锁
		}
	}
	pInfo = NULL;
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: DataProcess
*  @Description   :: 解析原始数据包,pInfo里面标志位标记是否创建新帧
*  @Author        :: zzw
*  @Parameters    :: Info:原始数据包的一些信息
*                    
*  @Return        :: 运行是否成功
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
*  @Description   :: 判断是否需要结束旧的帧并继续创建新帧
*  @Author        :: zzw
*  @Parameters    :: Angle:新一组数据的水平角 BlockMinPoints:一个Block里面点的个数
*					 FileCurFrameIndex:新帧的下标 newFrame:满一帧后是否需要继续读取下一帧
*  @Return        :: true:继续处理下去，false:不再进行处理
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

	if (Angle < LastAngle_H								//如果角度比前一个小，则认为是新的一帧
		|| pRawFrame->GetPointsNumber() >= (pRawFrame->GetMaxPointsInFrame() - BlockMinPoints))//如果Frame不够保存一个BLOCK，也切换帧
	{
		mutex_Frame.unlock();

		End_TransFrame(false);							//切换帧先抛出旧帧,并置初始状态
		if (newFrame)									//创建下一帧
		{
			ALLFrameIndex++;
			mutex_Frame.lock();
			pRawFrame = std::make_shared<RawFrame>();
			pRawFrame->SetFrameIndex(FileCurFrameIndex);//设置新帧的下标
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
*  @Description   :: 清空现存帧
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
int RawProcess::End_ClearFrame()
{
	LastAngle_H = MAXWORD;
	mutex_Frame.lock();
	if (nullptr == pRawFrame)						//帧是空的,退出
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
*  @Description   :: 将当前帧认为满帧，发送出去
*  @Author        :: zzw
*  @Parameters    :: isWait:发送出去之前是否需要先等待一段时间，防止当前帧还有没有生成好的
*
*  @Return        :: 运行是否成功
**************************************************************************/
int RawProcess::End_TransFrame(bool isWait)
{
	if (isWait)
	{
		Sleep(10);
	}
	LastAngle_H = MAXWORD;
	mutex_Frame.lock();
	if (nullptr== pRawFrame)							//帧是空的,退出
	{
		mutex_Frame.unlock();
		return ER_SensorFrameAleadyClear;
	}
	FullFrame(pRawFrame, RTAdress);
	pRawFrame = nullptr;
	mutex_Frame.unlock();
	return RaView_NO_ERROR;
}