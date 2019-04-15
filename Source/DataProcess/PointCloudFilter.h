#pragma once
#include "stdafx.h"
#include <memory>
#include <mutex>
#include "StructDefine.h"
#include "Packet.h"
using namespace DataDefinition;

#ifdef DATAPROCESS_EXPORTS
#define PointCloudFilter_API __declspec(dllexport)
#else
#define PointCloudFilter_API __declspec(dllimport)
#endif

namespace DataProcess
{
	typedef std::shared_ptr<RadarPointList> PointCloud_Ptr;								//点云队列
	typedef std::vector< PointCloud_Ptr > PointClouds;									//多个点云队列

	typedef struct PointCloudFilter_API AngleFilter
	{
		unsigned int OneAngle[2000][10];
		int sign[2000];
		bool b_isChange[2000];//本帧只加入一个点
	}AngleFilter;

	enum PointCloudFilter_API Filter_STATE												//工作状态
	{
		Filter_START,																	//播放
		Filter_STOP																		//暂停
	};

	class PointCloudFilter_API PointCloudFilter
	{
	public:
		PointCloudFilter();
		~PointCloudFilter();

		void Init(LiDAR_Protocol Protocol,unsigned int FilterNumber);					//初始化滤波类
		void UnSetProtocol();															//取消协议的设置
		bool FilterPointCloud(PointCloud_Ptr nofilteredPointCloud, PointCloud_Ptr &filteredPointCloud, int FileFrameIndex);	//进行多帧的滤波
		void ReStart();																	//重新开始滤波
	private:
		void Clearall_AngleFilter();													//清除所有角度积累
	private:
		AngleFilter * pFilterList[128];													//所有通道滤波器指针
		//AngleFilterList myFilterList;													//不同通道的滤波器
		Filter_STATE filterStation;														//滤波器的工作状态
		PointCloud_Ptr m_pFilteredPointCloud;											//滤波好的一帧点云数据
		UINT m_TotalPointCloudFromBegin;												//本次滤波已经进来的帧数，大于滤波帧则停止计数
		int nextFileFrameIndex;															//上一个帧索引号
		LiDAR_Protocol Conver_Protocol;													//LiDAR转换协议
		unsigned int LiDAR_Channels;													//LiDAR通道数
		unsigned int m_iFilterNumber;													//滤波的帧数
		std::mutex mutex_Filter;														//操作myFilterList锁
	};
}
