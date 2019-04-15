#pragma once
#include "stdafx.h"
#include <memory>
#include "StructDefine.h"

#ifdef DATAREADER_EXPORTS
#define BINREADER_API __declspec(dllexport)
#else
#define BINREADER_API __declspec(dllimport)
#endif

namespace DataReader
{
	class BINREADER_API BINReader
	{
	public:
		BINReader();
		~BINReader();

		void Init(void(*Callback)(std::shared_ptr<RadarPointList> &lParam, \
			LPVOID pParam), LPVOID pParam);													//初始化的回调函数);
		void Destroy();																		//释放资源
		void BINOpen(const char *binpath);													//用来打开bin文件
		void BINWrite(const char *csv_savepath, RadarPointList& PointsList);				//用来保存bin文件

	private:
		void(*TransferOneFrame)(std::shared_ptr<RadarPointList> &lParam, LPVOID pParam);	//翻译好一帧的回调函数
		LPVOID RTAdress;																	//回调函数中的地址参数
		std::shared_ptr<RadarPointList> pRadarPointList;									//转换后的一帧数据
	};
}