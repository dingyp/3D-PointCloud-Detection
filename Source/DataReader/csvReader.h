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
			LPVOID pParam), LPVOID pParam);													//��ʼ����һ֡�Ļص�����
		void Destory();																		//��Դ�ͷ�
		void CSVOpen(const char *csvpath);													//�����򿪲���ʾcsv�ļ�
		void CSVWrite(const char *csv_savepath, RadarPointList& PointsList);				//��������csv�ļ�
	private:
		void(*TransferOneFrame)(std::shared_ptr<RadarPointList> &lParam, LPVOID pParam);	//�����һ֡�Ļص�����
		LPVOID RTAdress;																	//�ص������еĵ�ַ����
		std::shared_ptr<RadarPointList> pRadarPointList;									//ת�����һ֡����
	};

}