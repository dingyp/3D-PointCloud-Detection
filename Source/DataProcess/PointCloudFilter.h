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
	typedef std::shared_ptr<RadarPointList> PointCloud_Ptr;								//���ƶ���
	typedef std::vector< PointCloud_Ptr > PointClouds;									//������ƶ���

	typedef struct PointCloudFilter_API AngleFilter
	{
		unsigned int OneAngle[2000][10];
		int sign[2000];
		bool b_isChange[2000];//��ֻ֡����һ����
	}AngleFilter;

	enum PointCloudFilter_API Filter_STATE												//����״̬
	{
		Filter_START,																	//����
		Filter_STOP																		//��ͣ
	};

	class PointCloudFilter_API PointCloudFilter
	{
	public:
		PointCloudFilter();
		~PointCloudFilter();

		void Init(LiDAR_Protocol Protocol,unsigned int FilterNumber);					//��ʼ���˲���
		void UnSetProtocol();															//ȡ��Э�������
		bool FilterPointCloud(PointCloud_Ptr nofilteredPointCloud, PointCloud_Ptr &filteredPointCloud, int FileFrameIndex);	//���ж�֡���˲�
		void ReStart();																	//���¿�ʼ�˲�
	private:
		void Clearall_AngleFilter();													//������нǶȻ���
	private:
		AngleFilter * pFilterList[128];													//����ͨ���˲���ָ��
		//AngleFilterList myFilterList;													//��ͬͨ�����˲���
		Filter_STATE filterStation;														//�˲����Ĺ���״̬
		PointCloud_Ptr m_pFilteredPointCloud;											//�˲��õ�һ֡��������
		UINT m_TotalPointCloudFromBegin;												//�����˲��Ѿ�������֡���������˲�֡��ֹͣ����
		int nextFileFrameIndex;															//��һ��֡������
		LiDAR_Protocol Conver_Protocol;													//LiDARת��Э��
		unsigned int LiDAR_Channels;													//LiDARͨ����
		unsigned int m_iFilterNumber;													//�˲���֡��
		std::mutex mutex_Filter;														//����myFilterList��
	};
}
