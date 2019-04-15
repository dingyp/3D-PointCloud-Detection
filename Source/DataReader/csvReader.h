#pragma once
#include "stdafx.h"
#include <memory>
#include "StructDefine.h"

#ifdef DATAREADER_EXPORTS
#define CSVREADER_API __declspec(dllexport)
#else
#define CSVREADER_API __declspec(dllimport)
#endif

namespace DataReader
{
	class CSVREADER_API CSVReader
	{
	public:
		CSVReader();
		~CSVReader();

		void Init(void(*Callback)(std::shared_ptr<RadarPointList> &lParam, \
			LPVOID pParam), LPVOID pParam);													//初始化满一帧的回调函数
		void Destory();																		//资源释放
		void CSVOpen(const char *csvpath);													//用来打开并显示csv文件
		void CSVWrite(const char *csv_savepath, RadarPointList& PointsList);				//用来保存csv文件
	private:
		void(*TransferOneFrame)(std::shared_ptr<RadarPointList> &lParam, LPVOID pParam);	//翻译好一帧的回调函数
		LPVOID RTAdress;																	//回调函数中的地址参数
		std::shared_ptr<RadarPointList> pRadarPointList;									//转换后的一帧数据
	};

}