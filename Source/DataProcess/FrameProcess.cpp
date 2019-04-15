#include "stdafx.h"
#include "FrameProcess.h"

//对原始帧转换成坐标帧的数据处理
/********本程序进行数据的转换*********/
using namespace DataProcess;

/**************************************************************************
*  @Function Name :: FrameProcess
*  @Description   :: 原始帧转换成坐标帧类的构造函数，进行数据解析协议的初始化
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
FrameProcess::FrameProcess()
{
	Conver_Protocol = LIDAR_INVALID;
	LiDAR_Channels = 0;
	ClearAnglesV();
	state = TRASFER_STOP;
	pRadarPointList = nullptr;
	FilteredPointCloud = nullptr;
	m_bIsFilter = true;
	RTAdress = NULL;
	TransferOneFrame = NULL;
	frame_list.clear();
	destoryTrasfer = true;//如果没有开始是会退出的
	FileFrameIndex = 0;
}

/**************************************************************************
*  @Function Name :: ~FrameProcess
*  @Description   :: 原始帧转换成坐标帧类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
FrameProcess::~FrameProcess()
{
	UnSetProtocol();
	Conver_Protocol = LIDAR_INVALID;
	LiDAR_Channels = 0;
	state = TRASFER_STOP;
	pRadarPointList = nullptr;
	FilteredPointCloud = nullptr;
	RTAdress = NULL;
	TransferOneFrame = NULL;
	frame_list.clear();
	destoryTrasfer = true;
	FileFrameIndex = 0;
}

/**************************************************************************
*  @Function Name :: SetProtocol
*  @Description   :: 设置协议
*  @Author        :: zzw
*  @Parameters    :: unsigned int Protocol:协议类型
*
*  @Return        :: None
**************************************************************************/
void FrameProcess::SetProtocol(LiDAR_Protocol Protocol, bool IsFilter, unsigned int FilterFrameNumber)
{
	Conver_Protocol = Protocol;
	m_bIsFilter = IsFilter;
	m_pointCloudFilter.Init(Conver_Protocol, FilterFrameNumber);
	switch (Conver_Protocol)
	{
	case LIDAR_HLP16:
		LiDAR_Channels = 16;
		break;
	case LIDAR_2D:
		LiDAR_Channels = 1;
		break;
	}

	if (LoadAnglesV(Conver_Protocol))	//加载垂直角度数据
	{
		state = TRASFER_START;			//只有角度正确了，状态才能设为开始
	}
}

/**************************************************************************
*  @Function Name :: UnSetProtocol
*  @Description   :: 取消设置的协议
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FrameProcess::UnSetProtocol()
{
	m_pointCloudFilter.UnSetProtocol();
	Conver_Protocol = LIDAR_INVALID;
	ClearAnglesV();
	LiDAR_Channels = 0;
	state = TRASFER_STOP;
}

/**************************************************************************
*  @Function Name :: LoadAnglesV
*  @Description   :: 根据产品型号加载垂直角度数据，包括角度，弧度，Cos，Sin值
*  @Author        :: zzw
*  @Parameters    :: unsigned int Protocol:协议类型
*
*  @Return        :: true:加载成功，false：加载失败
**************************************************************************/
bool FrameProcess::LoadAnglesV(LiDAR_Protocol uProtocol)
{
	if (LIDAR_INVALID == uProtocol)
	{
		return false;
	}
	ClearAnglesV();
	if (LIDAR_HLP16 == uProtocol)
	{
		for (UINT i = 0; i < LiDAR_Channels; i++)
		{
			if (i & 0x01)//奇数
				AnglesV_Deg.push_back(i);
			else
				AnglesV_Deg.push_back((int)i - 15);
		}
	}
	else if (LIDAR_2D == uProtocol)
	{
		AnglesV_Deg.push_back(double(0.0));
	}

	for (int i = 0; i < AnglesV_Deg.size(); i++)
	{
		AnglesV_Rad.push_back(Deg2Rad(AnglesV_Deg[i]));
		AnglesV_Cos.push_back(cos(AnglesV_Rad[i]));
		AnglesV_Sin.push_back(sin(AnglesV_Rad[i]));
	}
	return true;
}

/**************************************************************************
*  @Function Name :: ClearAnglesV
*  @Description   :: 清除垂直角度数据
*  @Author        :: hgb
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FrameProcess::ClearAnglesV()
{
	AnglesV_Deg.clear();
	AnglesV_Rad.clear();
	AnglesV_Cos.clear();
	AnglesV_Sin.clear();
}


/**************************************************************************
*  @Function Name :: AddFrame
*  @Description   :: 添加一帧待转换的点数据到待转换帧队列中
*  @Author        :: zzw
*  @Parameters    :: OneFrame:待转换的帧
*					 大于30帧则进一帧丢弃两帧
*  @Return        :: restframe剩余的帧数
**************************************************************************/
int FrameProcess::AddFrame(std::shared_ptr<RawFrame> &OneFrame)
{
	int restframe;
	mutex_frame_list.lock();
	switch (Conver_Protocol)
	{
	case LIDAR_HLP16:
		restframe = AddPcapFrame(OneFrame);
		break;
	case LIDAR_2D:
		restframe = AddPcapngFrame(OneFrame);
		break;
	}
	mutex_frame_list.unlock();
	return restframe;//剩余帧数
}

/**************************************************************************
*  @Function Name :: AddPcapFrame
*  @Description   :: 添加一帧待转换pcap点数据到待转换帧队列中
*  @Author        :: zzw
*  @Parameters    :: OneFrame:待转换的pcap帧
*					 大于30帧则进一帧丢弃两帧
*  @Return        :: 返回添加后的总帧数
**************************************************************************/
int FrameProcess::AddPcapFrame(std::shared_ptr<RawFrame> &OneFrame)
{
	frame_list.push_back(OneFrame);
	//满30帧则进一帧丢两帧
	if (frame_list.size() >= 30)
	{
		frame_list.begin() = frame_list.erase(frame_list.begin());//删除第一个元素
		frame_list.begin() = frame_list.erase(frame_list.begin());//删除第一个元素
	}
	return frame_list.size();
}

/**************************************************************************
*  @Function Name :: AddPcapngFrame
*  @Description   :: 添加一帧待转换pcapng点数据到待转换帧队列中
*  @Author        :: zzw
*  @Parameters    :: OneFrame:待转换的pcapng帧
*					 大于30帧则进一帧丢弃两帧
*  @Return        :: 返回添加后的总帧数
**************************************************************************/
int FrameProcess::AddPcapngFrame(std::shared_ptr<RawFrame> &OneFrame)
{
	frame_list.push_back(OneFrame);
	//满30帧则进一帧丢两帧
	if (frame_list.size() >= 30)
	{
		frame_list.begin() = frame_list.erase(frame_list.begin());//删除第一个元素
		frame_list.begin() = frame_list.erase(frame_list.begin());//删除第一个元素
	}
	return frame_list.size();
}

/**************************************************************************
*  @Function Name :: GetFrameNumber
*  @Description   :: 获取转换队列中的帧数
*  @Author        :: zzw
*  @Parameters    :: None
*					
*  @Return        :: 转换队列中的帧数
**************************************************************************/
int FrameProcess::GetFrameNumber()
{
	return frame_list.size();
}

/**************************************************************************
*  @Function Name :: BindTrasferOneFrameFunc
*  @Description   :: 绑定转换满一帧的回调函数
*  @Author        :: zzw
*  @Parameters    :: Callback:满一帧的回调函数
*					 pParam:回调函数中的指针参数
*  @Return        :: None
**************************************************************************/
void FrameProcess::BindTrasferOneFrameFunc(void(*Callback)(std::shared_ptr<RadarPointList> &lParam, LPVOID pParam), LPVOID pParam)
{
	TransferOneFrame = Callback;
	RTAdress = pParam;
}

/**************************************************************************
*  @Function Name :: PointR2XYZ
*  @Description   :: 转换PointR格式为XYZ格式
*  @Author        :: hgb
*  @Parameters    :: pr:输入数据点格式
*                    x,y,z：输出点的x,y,z坐标
*  @Return        :: None
**************************************************************************/
void FrameProcess::PointR2XYZ(const PointR &pr, double &x, double &y, double &z)
{
	double xy;
	if (LiDAR_Channels)
	{
		double fDistance = pr.distance / 1000.0; //毫米转换成米
		unsigned int uChannel = pr.channel % LiDAR_Channels;

		z = (float)(fDistance * AnglesV_Sin[uChannel]);
		xy = (float)(fDistance * AnglesV_Cos[uChannel]);
		double angleH = pr.angleH / 100.0;

		y = (float)(xy * cos(Deg2Rad(angleH)));
		x = (float)(xy * sin(Deg2Rad(angleH)));
	}
}

/**************************************************************************
*  @Function Name :: SFrameToPFrame
*  @Description   :: 将雷达数据帧转换成坐标数据帧
*  @Author        :: zzw
*  @Parameters    :: OneFrame:待转换的原始帧
*                    
*  @Return        :: true转换成功，false转换失败
**************************************************************************/
bool FrameProcess::SFrameToPFrame(std::shared_ptr<RawFrame> &OneFrame)
{
	//确认旧帧已经转换掉
	if (nullptr != pRadarPointList)
	{
		return false;
	}

	//判断待转换帧是否为空,为空则返回
	if (nullptr == OneFrame)
	{
		return false;
	}

	pRadarPointList = std::make_shared<RadarPointList>();
	if (nullptr == pRadarPointList)
	{//申请内存失败,返回
		return false;
	}

	return PcapFrameToPFrame(OneFrame);

	//switch (Conver_Protocol)
	//{
	//case LIDAR_HLP16:
	//	return PcapFrameToPFrame(OneFrame);
	//	break;
	//case LIDAR_2D:
	//	return PcapngFrameToPFrame(OneFrame);
	//	break;
	//}
}

/**************************************************************************
*  @Function Name :: PcapFrameToPFrame
*  @Description   :: 将pcap帧转换成坐标数据帧
*  @Author        :: zzw
*  @Parameters    :: OneFrame:待转换的原始帧
*
*  @Return        :: true转换成功，false转换失败
**************************************************************************/
bool FrameProcess::PcapFrameToPFrame(std::shared_ptr<RawFrame> &OneFrame)
{
	//获取CurrentFrameIndex帧中点的数量
	int TotalPointsNumber = OneFrame->GetPointsNumber();

	for (int i = 0; i < TotalPointsNumber; i++) {
		if (destoryTrasfer)
		{
			pRadarPointList = nullptr;
			return false;
		}
		PointR PointReader;
		if (!OneFrame->GetPoint(i, PointReader))
		{
			//获取点失败
			continue;
		}

		//根据原始帧信息对点进行转换，并添加到新帧里面
		RadarPoint CurPoint;
		CurPoint.Point_ID = i;
		CurPoint.distance = PointReader.distance;
		PointR2XYZ(PointReader, CurPoint.x, CurPoint.y, CurPoint.z);
		CurPoint.intensity = double(PointReader.intensity);
		CurPoint.angleH = PointReader.angleH;
		CurPoint.angleV = int(AnglesV_Deg[PointReader.channel%LiDAR_Channels]);
		CurPoint.channel = PointReader.channel%LiDAR_Channels;
		CurPoint.timestamp = PointReader.timestamp_ms;
		pRadarPointList->push_back(CurPoint);//加到新帧中
	}
	return true;
}

/**************************************************************************
*  @Function Name :: PcapngFrameToPFrame
*  @Description   :: 将pcapng帧转换成坐标数据帧
*  @Author        :: zzw
*  @Parameters    :: OneFrame:待转换的原始帧
*
*  @Return        :: true转换成功，false转换失败
**************************************************************************/
bool FrameProcess::PcapngFrameToPFrame(std::shared_ptr<RawFrame> &OneFrame)
{
	//获取CurrentFrameIndex帧中点的数量
	int TotalPointsNumber = OneFrame->GetPointsNumber();

	for (int i = 0; i < TotalPointsNumber; i++) 
	{
		if (destoryTrasfer)
		{
			pRadarPointList = nullptr;
			return false;
		}
		PointR PointReader;
		if (!OneFrame->GetPoint(i, PointReader))
		{
			//获取点失败
			continue;
		}

		//根据原始帧信息对点进行转换，并添加到新帧里面
		RadarPoint CurPoint;
		CurPoint.Point_ID = i;
		CurPoint.distance = PointReader.distance;
		PointR2XYZ(PointReader, CurPoint.x, CurPoint.y, CurPoint.z);
		CurPoint.intensity = double(PointReader.intensity);
		CurPoint.angleH = PointReader.angleH;
		CurPoint.angleV = int(AnglesV_Deg[PointReader.channel%LiDAR_Channels]);
		CurPoint.channel = PointReader.channel%LiDAR_Channels;
		CurPoint.timestamp = PointReader.timestamp_ms;
		pRadarPointList->push_back(CurPoint);//加到新帧中
	}
	return false;
}

/**************************************************************************
*  @Function Name :: Start
*  @Description   :: 原始帧转换成点帧线程函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FrameProcess::Start()
{
	if ((NULL == TransferOneFrame) || (NULL == RTAdress))
	{//还没有绑定回调函数，调用失败
		return;
	}
	std::shared_ptr<RawFrame> OneFrame = NULL;
	destoryTrasfer = false;
	while (!destoryTrasfer)
	{
		OneFrame = NULL;
		mutex_frame_list.lock();
		if (frame_list.size()>0)
		{
			OneFrame = frame_list.front();
			frame_list.begin() = frame_list.erase(frame_list.begin());//删除第一个元素

		}
		mutex_frame_list.unlock();
		if (NULL != OneFrame)
		{
			if (SFrameToPFrame(OneFrame))
			{//如果转换成功
				FileFrameIndex = OneFrame->GetFileFrameIndex();
				if (false == m_bIsFilter)
				{//不需要滤波
					TransferOneFrame(pRadarPointList, RTAdress);
				}
				else
				{
					//进行滤波
					if (m_pointCloudFilter.FilterPointCloud(pRadarPointList, FilteredPointCloud, FileFrameIndex))//滤波成功才进行返回,加入文件帧号便于判别
					{
						TransferOneFrame(FilteredPointCloud, RTAdress);
					}
					FilteredPointCloud = nullptr;
				}
			}
			pRadarPointList = nullptr;//置空
			OneFrame = nullptr;
		}
		else
		{
			Sleep(0);
		}

		//进行状态检测
		while ((TRASFER_STOP == state) && (!destoryTrasfer))//停止的时候一直在此次睡眠，直到状态改变
		{//帧缓存过多则清除，主要是防止停止的时候不断有新帧过来
			mutex_frame_list.lock();
			//满30帧则进一帧丢两帧
			if (frame_list.size() >= 30)
			{
				frame_list.begin() = frame_list.erase(frame_list.begin());//删除一个元素
				frame_list.begin() = frame_list.erase(frame_list.begin());//再删除一个元素
			}
			mutex_frame_list.unlock();
			Sleep(1);
		}
	}
}

//设置是否进行滤波
void FrameProcess::Set_IsFilter(bool isFilter)
{
	m_bIsFilter = isFilter;
}

//获取是否进行滤波
void FrameProcess::Get_IsFilter(bool &isFilter)
{
	isFilter = m_bIsFilter;
}

/**************************************************************************
*  @Function Name :: Stop
*  @Description   :: 给原始帧转换成点帧线程函数置停止位
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FrameProcess::Stop()
{
	state = TRASFER_STOP;
}

/**************************************************************************
*  @Function Name :: Restart
*  @Description   :: 线程函数置停止之后重新开始
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FrameProcess::Restart()
{
	state = TRASFER_START;
	if (0 == FileFrameIndex)//如果一直是0,说明是传感器，则重新开始
	{
		m_pointCloudFilter.ReStart();
	}
}

/**************************************************************************
*  @Function Name :: Destory_Trasfrom
*  @Description   :: 退出线程函数，结束线程
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FrameProcess::Destory_Trasfrom()
{
	destoryTrasfer = true;
	RTAdress = NULL;
	TransferOneFrame = NULL;
}

/**************************************************************************
*  @Function Name :: ReTrasform
*  @Description   :: 重启转换，清空之前缓存
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FrameProcess::ReTrasform()
{
	state = TRASFER_STOP;//先停止
	mutex_frame_list.lock();
	while (frame_list.size()>0)
	{
		frame_list.begin() = frame_list.erase(frame_list.begin());
	}
	mutex_frame_list.unlock();
	if (0 == FileFrameIndex)//如果一直是0,说明是传感器，则重新开始
	{
		m_pointCloudFilter.ReStart();
	}
	state = TRASFER_START;//最后恢复状态
}

/**************************************************************************
*  @Function Name :: TrasfOneFrame
*  @Description   :: 往后转换一帧
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
//void FrameProcess::TrasfOneFrame()
//{
//	if ((NULL == TransferOneFrame) || (NULL == RTAdress))
//	{//没有初始化回调函数，返回
//		return;
//	}
//	int tmpstate = state;
//	state = TRASFER_START;
//	std::shared_ptr<RawFrame> OneFrame = NULL;
//	mutex_frame_list.lock();
//	if (frame_list.size()>0)
//	{
//		OneFrame = frame_list.front();
//		frame_list.begin() = frame_list.erase(frame_list.begin());//删除第一个元素
//	}
//	mutex_frame_list.unlock();
//	if (NULL != OneFrame)
//	{
//		if (SFrameToPFrame(OneFrame))
//		{//如果转换成功
//			FileFrameIndex = OneFrame->GetFileFrameIndex();
//			TransferOneFrame(pRadarPointList, RTAdress);
//			pRadarPointList = nullptr;
//			OneFrame = nullptr;
//		}
//		else
//		{//转换失败则不进行回调，继续下一帧
//
//		}
//	}
//	//state = TRASFER_START;
//	state = tmpstate;
//}

/**************************************************************************
*  @Function Name :: GetFileFrameIndex
*  @Description   :: 得到当前文件夹的帧数，方便进行播放控制
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
unsigned long long FrameProcess::GetFileFrameIndex()
{
	return FileFrameIndex;
}