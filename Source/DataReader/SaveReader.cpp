#include "stdafx.h"
#include "SaveReader.h"
#include "WinPcap\pcap.h"
#include "DataDefinition\PcapStructure.h"
#include "ErrorManager\ErroCodeDefine.h"

using namespace DataReader;

/**************************************************************************
*  @Function Name :: SaveReader
*  @Description   :: SaveReader类的构造函数
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
	READER_DEBUG(_T("正在启动SaveReader"));
}

/**************************************************************************
*  @Function Name :: ~SaveReader
*  @Description   :: SaveReader类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
SaveReader::~SaveReader()
{
	destorySaveReader = true;
	pcap_saver = NULL;
	mutex_savePacketList.lock();				//操作pcap包队列互斥锁
	savePacket_list.clear();
	mutex_savePacketList.unlock();
	beginSaveID = NULL;
	FullSaveCallBack = NULL;
	RTAdress = NULL;
	READER_DEBUG(_T("正在注销SaveReader"));
}

/**************************************************************************
*  @Function Name :: Init
*  @Description   :: 文件保存的初始化
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
int SaveReader::Init()
{
	if (!destorySaveReader)						//没有Init便进入了保存文件线程，退出
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
	if (NULL == pcap_saver)						//创建保存文件流出错，退出
	{
		mutex_saveReader.unlock();
		return ER_CreatePcapSaver;
	}
	mutex_saveReader.unlock();
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: Destory
*  @Description   :: 退出文件保存连接，释放资源
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
*  @Description   :: 打开保存文件连接
*  @Author        :: zzw
*  @Parameters    :: savepath:保存文件的路径 LiDAR_Protocol:文件保存的协议
*					 SaveEndStop:文件存储太满的回调函数 pParam:回调函数里面的参数
*  @Return        :: 运行是否成功
**************************************************************************/
int SaveReader::SaveFileOpen(const char* savepath, DataDefinition::LiDAR_Protocol SetProtocol, void(*SaveEndStop)(LPVOID pParam), LPVOID pParam)
{
	mutex_saveReader.lock();
	if (NULL == pcap_saver)						//没有保存文件流，退出
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
*  @Description   :: 关闭保存文件连接
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
int SaveReader::SaveFileClose()
{
	mutex_saveReader.lock();
	if (NULL == pcap_saver)						//没有保存文件流，退出
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
*  @Description   :: 将原始pcap包添加到存储队列中
*  @Author        :: zzw
*  @Parameters    :: pInfo:原始数据包的一些信息
*
*  @Return        :: 运行是否成功
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
*  @Description   :: 开启一个线程进行等待写进文件
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 运行是否成功
**************************************************************************/
int SaveReader::Start()
{
	if (NULL != beginSaveID)					//只有文件保存线程没有创建的时候才开始创建
	{
		return BeginSaveIDIsNotNULL;
	}

	beginSaveID = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)BeginDataSave, this, 0, 0);//建立文件写线程
	return RaView_NO_ERROR;
}

/**************************************************************************
*  @Function Name :: BeginDataSave
*  @Description   :: 文件写线程函数
*  @Author        :: zzw
*  @Parameters    :: pParam:回调函数中的参数
*					
*  @Return        :: None
**************************************************************************/
void SaveReader::BeginDataSave(LPVOID pParam)
{
	if (NULL == pParam)							//FileReader为空，退出
	{
		return;
	}
	SaveReader *pSReader = (SaveReader *)pParam;
	pSReader->mutex_saveReader.lock();
	if (NULL == pSReader->pcap_saver)			//文件流没有打开，退出
	{
		pSReader->mutex_saveReader.unlock();
		pSReader->beginSaveID = NULL;				//将保存文件ID置空
		return;
	}
	pSReader->mutex_saveReader.unlock();
	if ((NULL == pSReader->FullSaveCallBack)||(NULL == pSReader->RTAdress))//保存太满回调函数为空
	{
		pSReader->mutex_saveReader.lock();
		pSReader->pcap_saver->close();	
		pSReader->mutex_saveReader.unlock();
		pSReader->beginSaveID = NULL;
		return;
	}

	if (LIDAR_HLP16 == pSReader->Protocol)//16线雷达文件头保存
	{
		pcap_file_header pfh;						//pcap文件头(24)
		/*设置默认的pcapheader*/
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
	else if (LIDAR_2D == pSReader->Protocol)//2D雷达文件头保存
	{
		BlockHead blockhead;							//pcapng中块的开头部分相同的结合，因为要判断是什么块，以及块有多大，所以先读取块头进行判断
		SHB section_header_block;//节头块
		IDB interface_description_block;//接口描述块
		blockhead.BlockType = 0x0a0d0d0a;
		blockhead.BlockLen = 32;
		section_header_block.Order = 0x1a2b3c4d;
		section_header_block.Major_Version = 1;
		section_header_block.Minor_Version = 0;
		section_header_block.Section_length[0] = 0xffffffff;
		section_header_block.Section_length[1] = 0xffffffff;
		//实现SHB的输入
		pSReader->mutex_saveReader.lock();
		pSReader->pcap_saver->write((char *)&blockhead, sizeof(blockhead));
		pSReader->pcap_saver->write((char *)&section_header_block, sizeof(section_header_block));
		unsigned int blockLen = 0;
		pSReader->pcap_saver->write((char *)&blockLen, sizeof(blockLen));//可选字段的结束选项，在一个块中不能重复
		blockLen = blockhead.BlockLen;
		pSReader->pcap_saver->write((char *)&blockLen, sizeof(blockLen));
		pSReader->mutex_saveReader.unlock();
		
		blockhead.BlockType = 0x00000001;
		blockhead.BlockLen = 24;
		interface_description_block.LinkType=1;
		interface_description_block.Reserved=0;
		interface_description_block.SnapLen=0x0000ffff;
		//实现IDB的输入
		pSReader->mutex_saveReader.lock();
		pSReader->pcap_saver->write((char *)&blockhead, sizeof(blockhead));
		pSReader->pcap_saver->write((char *)&interface_description_block, sizeof(interface_description_block));
		blockLen = 0;
		pSReader->pcap_saver->write((char *)&blockLen, sizeof(blockLen));//可选字段的结束选项，在一个块中不能重复
		blockLen = blockhead.BlockLen;
		pSReader->pcap_saver->write((char *)&blockLen, sizeof(blockLen));
		pSReader->mutex_saveReader.unlock();
	}
	else
	{
		pSReader->mutex_saveReader.lock();
		pSReader->pcap_saver->close();				//关闭保存文件流
		pSReader->mutex_saveReader.unlock();
		pSReader->beginSaveID = NULL;				//将保存文件ID置空
		return ;//无效的协议，返回
	}
	
	std::shared_ptr<raw_packetInfo> OnePacket = NULL;
	pSReader->savepcapNumber = 0;//pcap存储个数设置成0
	pSReader->destorySaveReader = false;
	//16线雷达
	if (LIDAR_HLP16 == pSReader->Protocol)
	{
		while (!pSReader->destorySaveReader)
		{
			OnePacket = NULL;
			pSReader->mutex_savePacketList.lock();	//从保存pcap包列表中取出文件，上锁
			if ((!pSReader->destorySaveReader) && (pSReader->savePacket_list.size()))
			{
				OnePacket = pSReader->savePacket_list.front();
				pSReader->savePacket_list.begin() = pSReader->savePacket_list.erase(pSReader->savePacket_list.begin());
			}
			pSReader->mutex_savePacketList.unlock();//解锁
			if ((!pSReader->destorySaveReader) && (NULL != OnePacket))//若取出pcap包则进行处理
			{
				if (pSReader->savepcapNumber>2500000)//大概3-4G之间
				//if (pSReader->savepcapNumber > 2000)//大概3-4G之间
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
		BlockHead blockhead;//pcapng中块的开头部分相同的结合，因为要判断是什么块，以及块有多大，所以先读取块头进行判断
		blockhead.BlockType = 0x00000006;
		blockhead.BlockLen = 1316;
		while (!pSReader->destorySaveReader)
		{
			OnePacket = NULL;
			pSReader->mutex_savePacketList.lock();	//从保存pcap包列表中取出文件，上锁
			if ((!pSReader->destorySaveReader) && (pSReader->savePacket_list.size()))
			{
				OnePacket = pSReader->savePacket_list.front();
				pSReader->savePacket_list.begin() = pSReader->savePacket_list.erase(pSReader->savePacket_list.begin());
			}
			pSReader->mutex_savePacketList.unlock();//解锁
			if ((!pSReader->destorySaveReader) && (NULL != OnePacket))//若取出pcap包则进行处理
			{
				if (pSReader->savepcapNumber>2500000)//大概3-4G之间
				//if (pSReader->savepcapNumber > 2000)//大概3-4G之间
				{
					pSReader->destorySaveReader = true;
				}
				pSReader->mutex_saveReader.lock();
				pSReader->pcap_saver->write((char *)&blockhead, sizeof(blockhead));
				unsigned int blockLen = 0;
				pSReader->pcap_saver->write((char *)&blockLen, sizeof(blockLen));//可选字段的结束选项，在一个块中不能重复
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
	pSReader->pcap_saver->close();				//关闭保存文件流
	pSReader->mutex_saveReader.unlock();
	pSReader->beginSaveID = NULL;				//将保存文件ID置空
	if (pSReader->savepcapNumber > 2500000)//大概3-4G之间
	{
		if ((NULL != pSReader->FullSaveCallBack) && (NULL != pSReader->RTAdress))
		{
			pSReader->FullSaveCallBack(pSReader->RTAdress);//调用保存满回调，进行一些操作
		}
		MessageBox(NULL,_T("防止存储文件过大，已经关闭文件保存"),0,0);
	}
	return;
}
