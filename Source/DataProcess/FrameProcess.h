#pragma once
#include "stdafx.h"
#include "DataDefinition\RawFrame.h"
#include <memory>
#include "StructDefine.h"
#include "Packet.h"
#include <vector>
#include <mutex>//互斥vector读取
#include "PointCloudFilter.h"

using namespace DataDefinition;

#ifdef DATAPROCESS_EXPORTS
#define FrameProcess_API __declspec(dllexport)
#else
#define FrameProcess_API __declspec(dllimport)
#endif

namespace DataProcess
{
	typedef std::vector< std::shared_ptr<RawFrame> > FrameList;				//帧管理队列

	enum FrameProcess_API STATE												//工作状态
	{
		TRASFER_START,														//播放
		TRASFER_STOP														//暂停
	};

	class FrameProcess_API FrameProcess										//数据转换模块
	{
	public:
		FrameProcess();
		~FrameProcess();

		void SetProtocol(LiDAR_Protocol Protocol, bool IsFilter,\
			unsigned int FilterFrameNumber);								//设置协议，是否进行滤波
		void UnSetProtocol();												//取消协议的设置
		bool LoadAnglesV(LiDAR_Protocol uProtocol);							//根据产品型号转换垂直角度数据，包括角度，弧度，Cos，Sin值
		void ClearAnglesV();												//清除垂直角度数据
		void PointR2XYZ(const PointR &pr, double &x, double &y, double &z);	//转换PointR格式为XYZ格式
		int AddFrame(std::shared_ptr<RawFrame> &OneFrame);					//添加一帧数据,返回添加后总得帧数
		int AddPcapFrame(std::shared_ptr<RawFrame> &OneFrame);				//添加一帧pcap数据，返回添加后总得帧数
		int AddPcapngFrame(std::shared_ptr<RawFrame> &OneFrame);			//添加一帧pcapng数据，返回添加后总得帧数
		int GetFrameNumber();												//返回转换器中还剩的帧数
		void BindTrasferOneFrameFunc(void(*Callback)(std::shared_ptr<RadarPointList> &lParam, \
			LPVOID pParam), LPVOID pParam);									//对回调函数进行绑定注册
		void Start();														//开始
		void Restart();														//停止之后重新开始
		void ReTrasform();													//清空之前的缓存，之后自动开始播放
		void Stop();														//停止播放
		void Destory_Trasfrom();											//退出线程函数，结束线程
		bool SFrameToPFrame(std::shared_ptr<RawFrame> &OneFrame);			//将雷达数据帧转换成坐标数据帧
		bool PcapFrameToPFrame(std::shared_ptr<RawFrame> &OneFrame);		//将pcap帧转换成坐标数据帧
		bool PcapngFrameToPFrame(std::shared_ptr<RawFrame> &OneFrame);		//将pcapng帧转换成坐标数据帧
		void Set_IsFilter(bool isFilter);									//设置是否进行滤波
		void Get_IsFilter(bool &isFilter);									//获取是否进行滤波
		//void TrasfOneFrame();												//仅往后再转换一帧
		unsigned long long GetFileFrameIndex();								//得到当前文件夹的帧数
	private:
		PointCloudFilter m_pointCloudFilter;								//点云滤波器
		bool m_bIsFilter;													//是否需要滤波标志位

		FrameList frame_list;												//帧管理队列
		STATE state;														//当前转换器的状态
		unsigned int LiDAR_Channels;										//LiDAR通道数
		LiDAR_Protocol Conver_Protocol;										//LiDAR转换协议
		std::vector<double> AnglesV_Deg;									//垂直角度的数值
		std::vector<double> AnglesV_Rad;									//垂直角度的弧度
		std::vector<double> AnglesV_Cos;									//垂直角度的cos
		std::vector<double> AnglesV_Sin;									//垂直角度的sin
		void(*TransferOneFrame)(std::shared_ptr<RadarPointList> &lParam, LPVOID pParam); //翻译好一帧的回调函数
		LPVOID RTAdress;													//回调函数中的指针参数
		std::shared_ptr<RadarPointList> pRadarPointList;					//转换后的一帧数据
		std::shared_ptr<RadarPointList> FilteredPointCloud;					//滤波后的一帧数据
		bool destoryTrasfer;												//退出while循环标志位
		bool eraseFrame;													//取一帧锁
		unsigned long long FileFrameIndex;									//现在读取文件帧的编号，如果是网络帧编号为0
		std::mutex mutex_frame_list;										//操作frameList锁
	};
}