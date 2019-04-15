#include "stdafx.h"
#include <iostream>
#include <fstream>//进行pcap文件的读取
#include <vector>
#include "FileReader.h"
#include "WinPcap\pcap.h"
#include "ErrorManager\ErroCodeDefine.h"

using namespace DataReader;

/**************************************************************************
*  @Function Name :: FileReader
*  @Description   :: 文件操作类的构造函数
*  @Author        :: zzw
*  @Parameters    :: None
*					
*  @Return        :: None
**************************************************************************/
FileReader::FileReader()
{
	destoryFileReader = true;
	SetPlayState(READER_STOP, FORWORD, NOLOOP);				//初始播放状态是STOP正向不循环
	file_reader = NULL;										//文件流置空
	offset_idxTable = NULL;
	cur_index = 0;											//帧下标置零（文件中从1开始）
	pRawProcess = NULL;
	Info = NULL;
	reDraw = true;
	g_uTimerID = NULL;
	remain_number = 0;
	PlayStop = NULL;										//播放到停止的回调函数置零
	RTAdress = NULL;
	isOffsetOK = true;
	Protocol = LIDAR_INVALID;								//初始读取协议为null
	READER_DEBUG(_T("正在启动FileReader"));
}

/**************************************************************************
*  @Function Name :: ~FileReader
*  @Description   :: 文件操作类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*					
*  @Return        :: None
**************************************************************************/
FileReader::~FileReader()
{
	destoryFileReader = true;
	SetPlayState(READER_STOP, FORWORD, NOLOOP);				//文件一开始的默认速度应该是STOP正向不循环
	if (file_reader)
	{
		file_reader->close();
		delete(file_reader);
		file_reader = NULL;
		cur_index = 0;										//帧下标置零（文件中从1开始）
		Free_offsetTable();									//释放索引表
	}
	pRawProcess = NULL;
	Info = NULL;
	reDraw = false;
	g_uTimerID = NULL;
	remain_number = 0;
	PlayStop = NULL;
	RTAdress = NULL;
	isOffsetOK = false;
	Protocol = LIDAR_INVALID;								//读取协议置null
	READER_DEBUG(_T("正在注销FileReader"));
}

/**************************************************************************
*  @Function Name :: Init
*  @Description   :: 初始化文件读取
*  @Author        :: zzw
*  @Parameters    :: RawProcessPoint:处理一个pcap包的类 PlayEndStop:初始化播放到停止的回调函数
*					 pParam:回调函数中的参数 SetProtocol:读文件使用的协议类型
*  @Return        :: 运行是否成功
**************************************************************************/
int FileReader::Init(DataProcess::RawProcess *RawProcessPoint, void(*PlayEndStop)(LPVOID pParam), LPVOID pParam, LiDAR_Protocol SetProtocol)
{
	if (!destoryFileReader)									//已经建立了File连接，退出
	{
		return ER_NODESTORYFILREADER;
	}
	if (NULL != file_reader)
	{
		return ER_OPENSTREAMNOTNULL;
	}
	
	file_reader = new std::ifstream();						//新建ifstream
	if (NULL == file_reader)
	{
		return ER_CREATEPCAPREADER;
	}
	if (NULL== RawProcessPoint)								//处理类指针为空，退出
	{
		return ER_InitPointRawProcess;
	}
	pRawProcess = RawProcessPoint;
	PlayStop = PlayEndStop;
	RTAdress = pParam;
	Protocol = SetProtocol;									//设置文件的读取协议
	reDraw = true;
	remain_number = 0;
	READER_DEBUG(_T("初始化FileReader完成"));
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: Destory
*  @Description   :: 退出文件读取线程，释放资源
*  @Author        :: zzw
*  @Parameters    :: None
*					 
*  @Return        :: None
**************************************************************************/
void FileReader::Destory()
{
	destoryFileReader = true;								//退出while循环
	reDraw = false;
	SetPlayState(READER_STOP, FORWORD, NOLOOP);
	if (file_reader)
	{
		file_reader->close();
		delete(file_reader);
		file_reader = NULL;
		cur_index = 0;										//下标应从1开始
		Free_offsetTable();									//释放索引表
	}
	
	if (NULL != g_uTimerID)									//释放多媒体计时器
	{
		timeKillEvent(g_uTimerID);
		g_uTimerID = NULL;
		READER_DEBUG(_T("关闭定时器"));
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
*  @Description   :: 连接读取文件
*  @Author        :: zzw
*  @Parameters    :: filepath:读取文件路径
*					 
*  @Return        :: 运行是否成功
**************************************************************************/
int FileReader::FileOpen(const char *filepath)
{
	if (NULL == file_reader)								//文件流为空，退出
	{
		return ER_PcapReaderIsNULL;
	}
	if (READER_STOP != speed)								//速度不为零，退出
	{
		return ER_OPENSTREAMNOTSTOP;
	}

	file_reader->open(filepath, std::ios_base::in | std::ios_base::binary);
	if (file_reader->bad())									//打开文件失败，退出
	{
		return ER_OpenPcapFile;
	}

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)StepUpOffsetTable, this, 0, 0);//建立文件索引表
	Sleep(0);												//主动休息

	while (0 == GetFrameNumber())							//等待索引表建立完成
	{
		Sleep(0);
		if (false == isOffsetOK)							//不能够创建索引表，退出
		{
			return ER_CannotCreateOffset;
		}
	}
	reDraw = true;
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: FileClose
*  @Description   :: 关闭文件连接，要释放索引表、文件流
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
int FileReader::FileClose()
{
	if (NULL == file_reader)								//文件流为空，退出
	{
		return ER_PcapReaderIsNULL;
	}
	if (READER_STOP != speed)								//速度不为零，退出
	{
		return ER_OPENSTREAMNOTSTOP;
	}
	reDraw = false;
	file_reader->close();									//关闭文件流
	Free_offsetTable();										//释放索引表
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: Start
*  @Description   :: 开启读取文件线程
*  @Author        :: zzw
*  @Parameters    :: status:复用标志位,0和1表示speed开始的时候不置播放，适合文件显示一帧后暂停
*					 2表示speed一开始就一直往后读，适合多帧
*  @Return        :: 运行是否成功
**************************************************************************/
int FileReader::Start(int status)
{
	READER_DEBUG(_T("进入FileReaderStart"));
	destoryFileReader = false;								//置线程开始标志位
	if (2 == status)
	{
		speed = ONE_SPEED;
	}
	while (!destoryFileReader)
	{
		if (NULL == file_reader)							//文件流为空，退出
		{
			return ER_PcapReaderIsNULL;
		}
		if (FORWORD == playType)
		{
			if (LOOP == loop)								//循环播放，正向播放到达最后一帧，回到开始
			{
				if (cur_index>= GetFrameNumber())
				{
					cur_index = 0;
					//GetNextFrame(reDraw);
					//reDraw = false;
				}
			}
			GetNextFrame(reDraw);							//播放下一帧
			reDraw = false;

			if ((LOOP != loop)&&(cur_index >= GetFrameNumber()))//非循环播放，判断是否需要暂停并汇报主函数
			{
				Sleep(10);
				speed = READER_STOP;
				PlayStop(RTAdress);							//回调函数，通知主函数已经暂停
			}
		}
		else if (BACKWORD == playType)
		{
			if (LOOP == loop)								//循环播放，逆向播放到达第一帧，回到最后
			{
				if (cur_index<=1)
				{
					cur_index = GetFrameNumber();
					//GetBackwardFrame(reDraw);
					//reDraw = false;
				}
			}
			GetBackwardFrame(reDraw);						//播放上一帧
			reDraw = false;

			if ((LOOP != loop) && (cur_index <= 1))			//非循环播放，判断是否置停，并汇报主函数
			{
				Sleep(10);
				speed = READER_STOP;
				PlayStop(RTAdress);							//回调函数，通知主函数已经暂停
			}
		}

		BeginSleep();										//每放一帧进行睡眠
	}
	READER_DEBUG(_T("退出FileReaderStart"));
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: GetIsStart
*  @Description   :: 获取线程是否正在运行
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 线程是否正在运行
**************************************************************************/
bool FileReader::GetIsStart()
{
	return !destoryFileReader;
}

/**************************************************************************
*  @Function Name :: ReStart
*  @Description   :: 重新开启文件读取
*  @Author        :: zzw
*  @Parameters    :: status:重新开始是否需要判断所处状态的标志位
*					 0代表如果刚好处于无法播放的边缘状态，先调整，再播放；1表示不进行预判
*  @Return        :: None
**************************************************************************/
void FileReader::ReStart(int status)
{
	if (0== status)											//重新开始前先看一下状态
	{
		if (FORWORD == playType)							//正向播放，若是最后一帧，回归开始
		{
			if (cur_index >= GetFrameNumber())
			{
				cur_index = 0;
			}
		}
		else if (BACKWORD == playType)						//逆向播放，若是第一帧，跳到最后
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
*  @Description   :: 暂停文件的读取
*  @Author        :: zzw
*  @Parameters    :: None
*					
*  @Return        :: None
**************************************************************************/
void FileReader::Stop()
{
	SetSpeed(READER_STOP);									//设置播放速度为0
}

/**************************************************************************
*  @Function Name :: SetPlayState
*  @Description   :: 设置播放状态
*  @Author        :: zzw
*  @Parameters    :: set_speed:播放速度 set_type:播放种类
*					 set_loop:是否循环播放
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
*  @Description   :: 重置播放状态为默认播放状态
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
*  @Description   :: 设置速度
*  @Author        :: zzw
*  @Parameters    :: set_speed:设置的播放速度
*
*  @Return        :: 运行是否成功
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
	default:												//其他速度，统一设置成1单位播放
		speed = ONE_SPEED; return INVAILDSPEED;
	}
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: ACCSpeed
*  @Description   :: 改变速度（先加快后减缓）
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 返回速度变化后的速度
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

////减缓速度
//void FileReader::DECSpeed()
//{
//}

/**************************************************************************
*  @Function Name :: BeginSleep
*  @Description   :: 根据设置的速度进行休眠
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
int FileReader::BeginSleep()
{
	switch (speed)
	{
	case READER_STOP:
		while ((READER_STOP == speed) && (!destoryFileReader))//暂停时会进行睡眠，直到速度改变
		{
			Sleep(1);
		}
		break;
	}
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: StepUpOffsetTable
*  @Description   :: 初始化索引表,只能修改一次
*  @Author        :: zzw
*  @Parameters    :: pParam:线程参数，对象指针
*
*  @Return        :: None
**************************************************************************/
void FileReader::StepUpOffsetTable(LPVOID pParam)
{
	if (NULL == pParam)								//FileReader为空，退出
	{
		return;
	}
	FileReader *pFReader = (FileReader *)pParam;
	if (NULL == pFReader->file_reader)				//文件流没有打开，退出
	{
		pFReader->isOffsetOK = false;
		return;
	}
	if (READER_STOP != pFReader->speed)				//播放速度不为零，退出
	{
		pFReader->isOffsetOK = false;
		return;
	}
	if (pFReader->GetFrameNumber())					//如果已经建立索引，退出
	{
		pFReader->isOffsetOK = false;
		return;
	}

	switch (pFReader->Protocol)
	{
	case LIDAR_HLP16:
		pFReader->StepUpPcapOffsetTable();			//进行pcap文件索引表的建立
		break;
	case LIDAR_2D:
		pFReader->StepUpPcapngOffsetTable();		//进行pcapng文件索引表的建立
		break;
	}

}

/**************************************************************************
*  @Function Name :: StepUpPcapOffsetTable
*  @Description   :: 建立pcap索引表
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FileReader::StepUpPcapOffsetTable()
{
	//与pcap文件相关
	pcap_file_header pfh;							//pcap文件头，24字节大小
	unsigned short Angle = MAXWORD;					//角度的判别记录变量

	std::vector<unsigned long long> index_table;	//偏移量动态增长数组
	unsigned long long offset_index = 0;			//文件偏移量计数器

	file_reader->clear();
	file_reader->seekg(0, std::ios::beg);	//移到文件开始处
	file_reader->read((char *)&pfh, sizeof(pfh));//读取pcap文件的头
	offset_index += sizeof(pfh);//24
	//进行判断，看是否符合pcap的文件格式
	if (!(pfh.magic == 0xa1b2c3d4 || pfh.magic == 0xd4c3b2a1))//不符合pcap文件格式，退出
	{
		isOffsetOK = false;
		return;
	}

	Info = std::make_shared<DataDefinition::raw_packetInfo>();
	if (Info == NULL)						//申请内存失败，退出
	{
		isOffsetOK = false;
		return;
	}
	memset(&(Info->pBuff.pcapPacket), 0, sizeof(pcap_packet));
	while (!file_reader->eof() && file_reader->good())
	{
		file_reader->read((char *)&(Info->pBuff.pcapPacket.pcap_head), sizeof(Info->pBuff.pcapPacket.pcap_head));
		offset_index += sizeof(Info->pBuff.pcapPacket.pcap_head);//16

		if (1248 == Info->pBuff.pcapPacket.pcap_head.capture_len)//先判断是否是1248(1206 + 42)
		{
			file_reader->read((char *)&(Info->pBuff.pcapPacket.mac_head), sizeof(Info->pBuff.pcapPacket.mac_head));
			offset_index += sizeof(Info->pBuff.pcapPacket.mac_head);//14
			file_reader->read((char *)&(Info->pBuff.pcapPacket.ip_head), sizeof(Info->pBuff.pcapPacket.ip_head));
			offset_index += sizeof(Info->pBuff.pcapPacket.ip_head);//20
			file_reader->read((char *)&(Info->pBuff.pcapPacket.udp_head), sizeof(Info->pBuff.pcapPacket.udp_head));
			offset_index += sizeof(Info->pBuff.pcapPacket.udp_head);//8
			unsigned short tmp_Length = ((Info->pBuff.pcapPacket.udp_head.m_usLength & 0x00FF) << 8) | ((Info->pBuff.pcapPacket.udp_head.m_usLength & 0xFF00) >> 8);//因为网络包，进行大小端转换
			if (1214 == tmp_Length)//1206 + 8
			{
				file_reader->read((char*)Info->pBuff.pcapPacket.block, BLOCK_NUM * sizeof(HLP16_Block));

				//进行下一帧的判定，读取帧中第一个角度和最后一个角度，判断是否是新帧
				HLP16_Block *pBlock = (HLP16_Block*)Info->pBuff.pcapPacket.block;
				for (int i = 0; i < BLOCK_NUM; i++)
				{
					if (MAXWORD == Angle)			//一帧的开始
					{
						Angle = pBlock->Angle;
					}
					if (pBlock->Angle < Angle)		//角度比前一个角度小，认为是新帧
					{
						index_table.push_back(offset_index - 58);//往前退58字节（pcap数据报头）
					}
					Angle = pBlock->Angle;
					pBlock++;
				}
				offset_index += BLOCK_NUM * sizeof(HLP16_Block);//1200

				file_reader->read((char *)&(Info->pBuff.pcapPacket.tail), sizeof(Info->pBuff.pcapPacket.tail));
				offset_index += sizeof(Info->pBuff.pcapPacket.tail);//6
			}
		}
		else										//不是1248的长度说明不是pcap包，计算长度之后继续读入
		{
			file_reader->read((char *)&(Info->pBuff.pcapPacket.mac_head), Info->pBuff.pcapPacket.pcap_head.capture_len);
			offset_index += Info->pBuff.pcapPacket.pcap_head.capture_len;//不固定
		}
		memset(&(Info->pBuff.pcapPacket), 0, sizeof(pcap_packet));//置空pFReader->Info的内容
	}
	Info = NULL;
	file_reader->clear();
	file_reader->seekg(0, std::ios::beg);//移到文件开始处
	Init_offsetTable(index_table);
	READER_DEBUG(_T("pcap文件索引创建完成"));
}

/**************************************************************************
*  @Function Name :: StepUpPcapngOffsetTable
*  @Description   :: 建立pcapng索引表
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FileReader::StepUpPcapngOffsetTable()
{
	//与pcapng文件相关,建立pcapng文件索引表
	BlockHead blockhead;							//pcapng中一些块的开头，因为要判断是什么块，以及块有多大，所以只有先读取块头进行判断
	EPB enhanced_packet_block;						//pcapng的EPB的剩余部分
	unsigned short Angle = MAXWORD;					//角度的判别记录变量

	std::vector<unsigned long long> index_table;	//偏移量动态增长数组
	unsigned long long offset_index = 0;			//文件偏移量计数器

	file_reader->clear();
	file_reader->seekg(0, std::ios::beg);			//移到文件开始处
	file_reader->read((char *)(&blockhead), sizeof(blockhead));
	offset_index += sizeof(blockhead);				//8
	if (blockhead.BlockType != 0x0a0d0d0a)			//pcapng的文件格式有错误
	{
		isOffsetOK = false;
		return;
	}
	Info = std::make_shared<DataDefinition::raw_packetInfo>();
	if (Info == NULL)								//申请内存失败，退出
	{
		isOffsetOK = false;
		return;
	}
	file_reader->read((char *)&(Info->pBuff.pcapngPacket), blockhead.BlockLen - sizeof(blockhead));//舍掉之后的数据
	offset_index += (blockhead.BlockLen - sizeof(blockhead));//暂时还不知道这个有多大，因为可选项不确定

	file_reader->read((char*)(&blockhead), sizeof(blockhead));	//IDB
	offset_index += sizeof(blockhead);							//8
	if (blockhead.BlockType != 0x00000001)
	{//pcapng文件格式有错误2
		isOffsetOK = false;
		return;
	}
	file_reader->read((char *)&(Info->pBuff.pcapngPacket), blockhead.BlockLen - sizeof(blockhead));
	offset_index += (blockhead.BlockLen - sizeof(blockhead));//暂时还不知道这个有多大，因为可选项不确定

	memset(&(Info->pBuff.pcapngPacket), 0, sizeof(pcap_packet));
	while (!file_reader->eof() && file_reader->good())
	{
		file_reader->read((char*)(&blockhead), sizeof(blockhead));
		offset_index += sizeof(blockhead);//8
		if (blockhead.BlockType != 0x00000006)
		{//pcapng中遇见了除了增强报文块的其他报文，暂时先跳过去
			file_reader->read((char *)&(Info->pBuff.pcapngPacket), blockhead.BlockLen - sizeof(blockhead));
			offset_index += (blockhead.BlockLen - sizeof(blockhead));//暂时还不知道这个有多大，因为可选项不确定
			continue;
		}

		//此时遇见的是增强报文，读取里面的信息
		file_reader->read((char *)&(enhanced_packet_block), sizeof(enhanced_packet_block));
		offset_index += sizeof(enhanced_packet_block);//20
		int releaseLen = blockhead.BlockLen - sizeof(blockhead) - sizeof(enhanced_packet_block) - enhanced_packet_block.CapLen;
		if (enhanced_packet_block.CapLen != 1282)
		{//增强报文块的尺寸不对，说明增强报文块里面的数据存在问题，所以进行跳过
			file_reader->read((char *)&(Info->pBuff.pcapngPacket), enhanced_packet_block.CapLen);
			offset_index += enhanced_packet_block.CapLen;
			file_reader->read((char *)&(Info->pBuff.pcapngPacket), releaseLen);
			offset_index += releaseLen;
			continue;
		}
		//没有读到数据块的开头PCAPNG_HEADER_SIGN(0xfef0010f),因为上面读到的
		file_reader->read((char*)&(Info->pBuff.pcapngPacket.mac_head), sizeof(Info->pBuff.pcapngPacket)-sizeof(Info->pBuff.pcapngPacket.pcap_head)\
			-sizeof(Info->pBuff.pcapngPacket.rest));//1282
		offset_index += (sizeof(Info->pBuff.pcapngPacket) - sizeof(Info->pBuff.pcapngPacket.pcap_head) - sizeof(Info->pBuff.pcapngPacket.rest));
		if (Info->pBuff.pcapngPacket.dataHeader.sign != PCAPNG_HEADER_SIGN)
		{//里面的值不是header标识符，说明读取数据有错误，所以进行跳过
			file_reader->read((char *)&(Info->pBuff.pcapngPacket), releaseLen);
			offset_index += releaseLen;
			continue;
		}
		//数据符合预期，进行读取
		//memcpy((char*)&(Info->pBuff.pcapngPacket.pcap_head), (char *)&(enhanced_packet_block.Timestamp_high), sizeof(Info->pBuff.pcapngPacket.pcap_head));//将EPB里面的类似pcap头进行提取
		for (int i = 0; i < 150; i++)
		{
			if (Info->pBuff.pcapngPacket.block[i].angle<=0xff00)//如果大于0xff00，则表示本数据块无效，必须忽略
			{
				if (MAXWORD == Angle)			//初始帧的开始
				{
					Angle = Info->pBuff.pcapngPacket.block[i].angle;
				}
				if (Info->pBuff.pcapngPacket.block[i].angle < Angle)//角度比前一个角度小，认为是新帧，保存的肯定是这个块的起始，因为不这样我们不知道这个点是第几个，以后没有办法根据索引进行读取
				{
					index_table.push_back(offset_index - 1310);//包括blockhead(8)+EPB里面的(20)+1282(mac头(14)+ip头(20)+udp头(8)+2D数据头(40)+2D数据块(1200))个长度
				}
				Angle = Info->pBuff.pcapngPacket.block[i].angle;
			}
		}
		
		file_reader->read((char *)&(Info->pBuff.pcapngPacket), releaseLen);
		offset_index += releaseLen;
	}

	Info = NULL;
	file_reader->clear();
	file_reader->seekg(0, std::ios::beg);//移到文件开始处
	Init_offsetTable(index_table);
	READER_DEBUG(_T("pcapng文件索引创建完成"));
}

/**************************************************************************
*  @Function Name :: GetFrameNumber
*  @Description   :: 得到文件中总的帧数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 文件总帧数
**************************************************************************/
int FileReader::GetFrameNumber()
{
	if (NULL == offset_idxTable)					//没有建立索引表
	{
		return NULL;
	}
	return offset_idxTable->index_number;
}

/**************************************************************************
*  @Function Name :: GetCurFrameNumber
*  @Description   :: 得到当前帧序号
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 当前帧序号
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
*  @Description   :: 跳到具体某一帧，其实只是置了标记位，到下一次读取便受到这个影响
*  @Author        :: zzw
*  @Parameters    :: frameNumber:具体某一帧的帧序号
*
*  @Return        :: None
**************************************************************************/
void FileReader::SetFrameNumber(unsigned int frameNumber)
{
	SPEED tmp_speed = speed;
	speed = READER_STOP;
	if ((frameNumber > 0) && (frameNumber<= GetFrameNumber()))//设置的帧号符合范围才赋值
	{
		cur_index = frameNumber;
		reDraw = true;
	}
	speed = tmp_speed;
}

/**************************************************************************
*  @Function Name :: Init_offsetTable
*  @Description   :: 初始化索引表，在没有销毁索引之前是不能再次初始化索引
*  @Author        :: zzw
*  @Parameters    :: idx_table:建立的索引vector
*
*  @Return        :: 创建是否成功
**************************************************************************/
int FileReader::Init_offsetTable(std::vector<unsigned long long> &idx_table)
{
	if (NULL != GetFrameNumber())						//索引表不为空，退出
	{
		isOffsetOK = false;
		return ER_ALREADYOFFSETTABLE;
	}
	if (0 == idx_table.size())							//索引vector大小为0，退出
	{
		isOffsetOK = false;
		return ER_IDX_TABLEEMPTY;
	}
	if (READER_STOP != speed)							//播放速度不为零，退出
	{
		isOffsetOK = false;
		return ER_OFFSETTABLENOTSTOP;
	}

	offset_idxTable = new offset_table();
	if (NULL == offset_idxTable)						//创建索引结构体不成功，退出
	{
		isOffsetOK = false;
		return ER_CREATEOFFSETTABLE;
	}
	offset_idxTable->index_number = 0;
	offset_idxTable->index_table = new unsigned long long[idx_table.size()];//建立索引表
	if (NULL == offset_idxTable->index_table)			//创建索引表不成功，退出
	{
		delete(offset_idxTable);						//释放索引结构体
		isOffsetOK = false;
		return ER_CREATEOFFSETTABLEA;
	}

	for (int i = 0; i < idx_table.size(); i++)
	{
		offset_idxTable->index_table[i] = idx_table[i];
	}
	cur_index = 0;										//当前文件帧置零
	offset_idxTable->index_number = idx_table.size();	//设置总大小
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: Free_offsetTable
*  @Description   :: 置空索引表
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FileReader::Free_offsetTable()
{
	cur_index = 0;										//文件帧置空
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
	READER_DEBUG(_T("文件索引成功释放"));
}

/**************************************************************************
*  @Function Name :: GoToFrame
*  @Description   :: 跳到具体的某一帧，这边使用定时器，读取一帧后退出
*  @Author        :: zzw
*  @Parameters    :: FrameNumber:帧号
*					 reDraw:本帧是否受到暂停的影响
*  @Return        :: 运行是否成功
**************************************************************************/
int FileReader::GoToFrame(const int FrameNumber, bool reDraw)
{
	if (destoryFileReader)								//退出while循环，结束线程
	{
		return ER_DESTORYFILEREADERSTART;
	}
	if (NULL == file_reader)
	{
		return ER_FILESTREAMNULL;
	}

	mutex_FileReader.lock();
	if (0 == GetFrameNumber())							//索引表出现错误，退出
	{
		mutex_FileReader.unlock();
		return ER_READFILENOOFFSET;
	}
	if (FrameNumber > GetFrameNumber() || FrameNumber <= 0)//假设FrameNumber的下标从1开始
	{
		mutex_FileReader.unlock();
		return ER_INDEXOUTRANGE;
	}
	
	file_reader->clear();								//设置pcapReader,调整前先进行clear()
	file_reader->seekg(offset_idxTable->index_table[FrameNumber - 1], std::ios::beg);
	cur_index = FrameNumber;

	remain_number = 0;									//默认剩余为0
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
	default:											//其他设置非法，播放速度设置成1单位播放
		add_number = 24; break;
	}

	if (NULL == g_uTimerID)
	{
		//READER_DEBUG(_T("开启定时器"));
		//READER_DEBUG(remain_number);
		//READER_DEBUG(add_number);
		g_uTimerID = timeSetEvent(24, 1, (LPTIMECALLBACK)CallBackMilliSecondProc, (DWORD_PTR)this, TIME_PERIODIC | TIME_KILL_SYNCHRONOUS);
		if (g_uTimerID == NULL)							//多媒体计时器创建失败，退出
		{
			mutex_FileReader.unlock();
			return ER_NotCreateTimer;
		}
	}

	while (destoryFileReader?(FALSE):(((cur_index<GetFrameNumber()) ? \
		(offset_idxTable->index_table[cur_index] >= file_reader->tellg()) : \
		(!file_reader->eof() && file_reader->good()))))	//没有达到文件末尾
	{
		if (READER_STOP == speed)
		{
			if (false == reDraw)						//如果暂停不继续，退出
			{
				mutex_FileReader.unlock();
				return RaView_NO_ERROR;
			}
		}

		if (remain_number)
		{
			remain_number--;
			Info = std::make_shared<DataDefinition::raw_packetInfo>();
			if (Info == NULL)							//申请内存失败，退出
			{
				mutex_FileReader.unlock();
				return ER_pBuffNotCreated;
			}

			switch (Protocol)
			{
			case LIDAR_HLP16:
				ReadPcapPacket();			//进行pcap文件索引表的建立
				break;
			case LIDAR_2D:
				ReadPcapngPacket();			//进行pcapng文件索引表的建立
				break;
			}

			Info = NULL;
		}
		
	}
	if (destoryFileReader?(FALSE):(file_reader->eof() || (!file_reader->good())))//文件读取到最后退出
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
*  @Description   :: 读取pcap一个块
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

	if (1248 == Info->pBuff.pcapPacket.pcap_head.capture_len)//先判断是否是1248(1206 + 42)
	{
		file_reader->read((char *)&(Info->pBuff.pcapPacket.mac_head), 42);//一次读3个
		Total_length += 42;

		unsigned short tmp_Length = ((Info->pBuff.pcapPacket.udp_head.m_usLength & 0x00FF) << 8) | \
			((Info->pBuff.pcapPacket.udp_head.m_usLength & 0xFF00) >> 8);//大小端转换
		if (1214 == tmp_Length)//1206 + 8
		{
			file_reader->read((char *)Info->pBuff.pcapPacket.block, 1206);
			Total_length += 1206;
			Info->size = Total_length;
			Info->isnewFrame = 0;				//0表示不创建新帧
			Info->FileCurFrameIndex = GetCurFrameNumber();
			pRawProcess->ProcessAddDataPacket(Info);
			//pRawProcess->DataProcess(Info);
		}
	}
	else										//不是1248的长度说明不是pcap包，计算长度之后继续读入
	{
		file_reader->read((char *)&(Info->pBuff.pcapPacket.mac_head), Info->pBuff.pcapPacket.pcap_head.capture_len);
	}
}

/**************************************************************************
*  @Function Name :: ReadPcapngPacket
*  @Description   :: 读取一个pcapng块
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FileReader::ReadPcapngPacket()
{
	BlockHead blockhead;							//pcapng中一些块的开头，因为要判断是什么块，以及块有多大，所以只有先读取块头进行判断
	EPB enhanced_packet_block;						//pcapng的EPB的剩余部分

	int Total_length = 0;
	file_reader->read((char *)(&blockhead), sizeof(blockhead));//这个不计入total_length里面
	if (blockhead.BlockType == 0x00000006)
	{//此时遇见的是增强报文，读取里面的信息
		file_reader->read((char *)&(enhanced_packet_block), sizeof(enhanced_packet_block));
		int releaseLen = blockhead.BlockLen - sizeof(blockhead) - sizeof(enhanced_packet_block) - enhanced_packet_block.CapLen;
		if (enhanced_packet_block.CapLen == 1282)
		{//没有读到数据块的开头PCAPNG_HEADER_SIGN(0xfef0010f),因为上面读到的
			file_reader->read((char*)&(Info->pBuff.pcapngPacket.mac_head), sizeof(Info->pBuff) - sizeof(Info->pBuff.pcapngPacket.pcap_head)\
				- sizeof(Info->pBuff.pcapngPacket.rest));//1282
			if (Info->pBuff.pcapngPacket.dataHeader.sign == PCAPNG_HEADER_SIGN)
			{//数据符合预期，进行读取
				memcpy((char*)&(Info->pBuff.pcapngPacket.pcap_head), (char *)&(enhanced_packet_block.Timestamp_high), sizeof(Info->pBuff.pcapngPacket.pcap_head));//将EPB里面的类似pcap头进行提取
				Total_length += sizeof(Info->pBuff.pcapngPacket) - sizeof(Info->pBuff.pcapngPacket.rest);//pcapheader(16)+mac(14)+ip(20)+udp(8)+2D数据包头(40)+2D数据块(1200)

				Info->size = Total_length;
				Info->isnewFrame = 0;				//0表示不创建新帧
				Info->FileCurFrameIndex = GetCurFrameNumber();
				pRawProcess->ProcessAddDataPacket(Info);
			}
		}
		else
		{//增强报文块的尺寸不对，说明增强报文块里面的数据存在问题，所以进行跳过
			file_reader->read((char *)&(Info->pBuff.pcapngPacket), enhanced_packet_block.CapLen);
		}
		file_reader->read((char*)&(Info->pBuff.pcapngPacket), releaseLen);//因为4字节对齐的缘故，所以还需要读取
	}
	else
	{//pcapng中遇见了除了增强报文块的其他报文，暂时先跳过去
		file_reader->read((char *)&(Info->pBuff.pcapngPacket), blockhead.BlockLen - sizeof(blockhead));
	}
}

/**************************************************************************
*  @Function Name :: CallBackMilliSecondProc
*  @Description   :: 多媒体计时器的计时操作
*  @Author        :: zzw
*  @Parameters    :: wTimerID:计时器ID msg:暂无
*					 dwUser:存放用户提供的回调函数 dw1:暂无 dw2:暂无
*  @Return        :: None
**************************************************************************/
void CALLBACK FileReader::CallBackMilliSecondProc(UINT wTimerID, UINT msg, DWORD_PTR dwUser, DWORD dw1, DWORD dw2)
{
	FileReader *pReader = (FileReader *)dwUser;
	pReader->remain_number += pReader->add_number;
}

/**************************************************************************
*  @Function Name :: GetNextFrame
*  @Description   :: 得到下一帧
*  @Author        :: zzw
*  @Parameters    :: reDraw:计时器本帧是否受到暂停的影响
*					 
*  @Return        :: 运行是否成功
**************************************************************************/
int FileReader::GetNextFrame(bool reDraw)
{
	cur_index++;//也就是第一帧没有读入
	if (cur_index > GetFrameNumber())
	{
		cur_index = GetFrameNumber();		//返回之前先将cur_index置空
		return ER_CURINDEXOUTRANGE;			
	}
	return GoToFrame(cur_index, reDraw);
}

/**************************************************************************
*  @Function Name :: GetBackwardFrame
*  @Description   :: 得到上一帧
*  @Author        :: zzw
*  @Parameters    :: reDraw:计时器本帧是否受到暂停的影响
*
*  @Return        :: 运行是否成功
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
*  @Description   :: 回到一开始
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
int FileReader::GoToStart()
{
	cur_index = 0;
	return GoToFrame(1,true);
}

/**************************************************************************
*  @Function Name :: GoToEnd
*  @Description   :: 移到最后
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
int FileReader::GoToEnd()
{
	cur_index = GetFrameNumber();
	//return RaView_NO_ERROR;
	return GoToFrame(GetFrameNumber(),true);
}

/**************************************************************************
*  @Function Name :: PlayForward
*  @Description   :: 正向播放
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
*  @Description   :: 逆向播放
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
*  @Description   :: 开始循环播放
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
*  @Description   :: 结束循环播放
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
*  @Description   :: 得到播放类型
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 播放类型
**************************************************************************/
PLAYTYPE FileReader::GetPlayType()
{
	return playType;
}