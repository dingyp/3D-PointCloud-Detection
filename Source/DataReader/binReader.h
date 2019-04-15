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
			LPVOID pParam), LPVOID pParam);													//��ʼ���Ļص�����);
		void Destroy();																		//�ͷ���Դ
		void BINOpen(const char *binpath);													//������bin�ļ�
		void BINWrite(const char *csv_savepath, RadarPointList& PointsList);				//��������bin�ļ�

	private:
		void(*TransferOneFrame)(std::shared_ptr<RadarPointList> &lParam, LPVOID pParam);	//�����һ֡�Ļص�����
		LPVOID RTAdress;																	//�ص������еĵ�ַ����
		std::shared_ptr<RadarPointList> pRadarPointList;									//ת�����һ֡����
	};
}