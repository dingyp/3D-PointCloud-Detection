#pragma once
#include "stdafx.h"

#define MAX_POINTS_PERFRAME 80000								//原始帧中包含最大点的个数

#ifdef DATADEFINITION_EXPORTS
#define SENSORFRAME_API __declspec(dllexport)
#else
#define SENSORFRAME_API __declspec(dllimport)
#endif

namespace DataDefinition 
{
	typedef SENSORFRAME_API struct _tag_PointR					//传感器帧的帧结构
	{
		unsigned long packID;									//每组数据在一个UDP包中的位置BLOCK_NUM
		unsigned long distance;									//距离，单位：mm
		unsigned short angleH;									//水平角度，单位：0.01°
		unsigned char intensity;								//反射率
		unsigned char channel;									//通道号
		unsigned int timestamp_ms;								//时间戳
	}PointR;

	//雷达数据的帧结构定义
	//设想帧数据不光包含众多点，还包含帧标记序号
	class SENSORFRAME_API RawFrame 
	{
	public:
		RawFrame(unsigned int MaxPoints= MAX_POINTS_PERFRAME);	//原始数据帧的构造函数
		~RawFrame();											//原始数据帧的析构函数
		unsigned int GetMaxPointsInFrame();						//得到每帧最大可存储点数
		unsigned int GetPointsNumber();							//得到当前帧中已有点的个数
		bool SetPoint(int PointIndex, PointR &InPoint);			//给下标为PointIndex的点赋值InPoint,返回是否赋值成功
		bool GetPoint(int PointIndex, PointR &OutPoint);		//取出下标为PointIndex的点,返回是否赋值成功
		bool TestAddPoint();									//返回Frame中是否可以增加点
		bool AddPoint(PointR &InPoint);							//增加一点到Frame中，点数加一
		//unsigned long long GetFrameIndex();					//得到帧序号,0表示帧没有初始化
		unsigned long long GetFileFrameIndex();					//得到文件序号，便于之后处理，0表示网络读取
		bool SetFrameIndex(unsigned long long FileIndex=0);		//修该帧序号，只能在帧初始化的时候修改一次,返回是否修改成功

	private:
		void CreateFrame(unsigned int PointsNumber);			//构造帧的时候调用
		void DestroyFrame();									//销毁帧的时候调用

		unsigned long long FileFrameIndex;						//当前帧在文件中的序号，便于播放的时候进行各种操作
		//unsigned long long FrameIndex;						//当前帧序号，便于后期进行帧的错序排列
		bool isFrameIndexInited;								//帧序号的锁死，确保帧号只修改一次
		unsigned int MaxPointsInFrame;							//每帧最大可存储点数
		unsigned int PointsNumber;								//当前帧中已有点的个数(1,2,..)
		PointR *Points;											//存储池
	};
}