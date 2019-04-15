#pragma once
#include "stdafx.h"
#include "DataDefinition\RawFrame.h"
#include <memory>
#include "StructDefine.h"
#include "Packet.h"
#include <vector>
#include <mutex>//����vector��ȡ
#include "PointCloudFilter.h"

using namespace DataDefinition;

#ifdef DATAPROCESS_EXPORTS
#define FrameProcess_API __declspec(dllexport)
#else
#define FrameProcess_API __declspec(dllimport)
#endif

namespace DataProcess
{
	typedef std::vector< std::shared_ptr<RawFrame> > FrameList;				//֡�������

	enum FrameProcess_API STATE												//����״̬
	{
		TRASFER_START,														//����
		TRASFER_STOP														//��ͣ
	};

	class FrameProcess_API FrameProcess										//����ת��ģ��
	{
	public:
		FrameProcess();
		~FrameProcess();

		void SetProtocol(LiDAR_Protocol Protocol, bool IsFilter,\
			unsigned int FilterFrameNumber);								//����Э�飬�Ƿ�����˲�
		void UnSetProtocol();												//ȡ��Э�������
		bool LoadAnglesV(LiDAR_Protocol uProtocol);							//���ݲ�Ʒ�ͺ�ת����ֱ�Ƕ����ݣ������Ƕȣ����ȣ�Cos��Sinֵ
		void ClearAnglesV();												//�����ֱ�Ƕ�����
		void PointR2XYZ(const PointR &pr, double &x, double &y, double &z);	//ת��PointR��ʽΪXYZ��ʽ
		int AddFrame(std::shared_ptr<RawFrame> &OneFrame);					//���һ֡����,������Ӻ��ܵ�֡��
		int AddPcapFrame(std::shared_ptr<RawFrame> &OneFrame);				//���һ֡pcap���ݣ�������Ӻ��ܵ�֡��
		int AddPcapngFrame(std::shared_ptr<RawFrame> &OneFrame);			//���һ֡pcapng���ݣ�������Ӻ��ܵ�֡��
		int GetFrameNumber();												//����ת�����л�ʣ��֡��
		void BindTrasferOneFrameFunc(void(*Callback)(std::shared_ptr<RadarPointList> &lParam, \
			LPVOID pParam), LPVOID pParam);									//�Իص��������а�ע��
		void Start();														//��ʼ
		void Restart();														//ֹ֮ͣ�����¿�ʼ
		void ReTrasform();													//���֮ǰ�Ļ��棬֮���Զ���ʼ����
		void Stop();														//ֹͣ����
		void Destory_Trasfrom();											//�˳��̺߳����������߳�
		bool SFrameToPFrame(std::shared_ptr<RawFrame> &OneFrame);			//���״�����֡ת������������֡
		bool PcapFrameToPFrame(std::shared_ptr<RawFrame> &OneFrame);		//��pcap֡ת������������֡
		bool PcapngFrameToPFrame(std::shared_ptr<RawFrame> &OneFrame);		//��pcapng֡ת������������֡
		void Set_IsFilter(bool isFilter);									//�����Ƿ�����˲�
		void Get_IsFilter(bool &isFilter);									//��ȡ�Ƿ�����˲�
		//void TrasfOneFrame();												//��������ת��һ֡
		unsigned long long GetFileFrameIndex();								//�õ���ǰ�ļ��е�֡��
	private:
		PointCloudFilter m_pointCloudFilter;								//�����˲���
		bool m_bIsFilter;													//�Ƿ���Ҫ�˲���־λ

		FrameList frame_list;												//֡�������
		STATE state;														//��ǰת������״̬
		unsigned int LiDAR_Channels;										//LiDARͨ����
		LiDAR_Protocol Conver_Protocol;										//LiDARת��Э��
		std::vector<double> AnglesV_Deg;									//��ֱ�Ƕȵ���ֵ
		std::vector<double> AnglesV_Rad;									//��ֱ�ǶȵĻ���
		std::vector<double> AnglesV_Cos;									//��ֱ�Ƕȵ�cos
		std::vector<double> AnglesV_Sin;									//��ֱ�Ƕȵ�sin
		void(*TransferOneFrame)(std::shared_ptr<RadarPointList> &lParam, LPVOID pParam); //�����һ֡�Ļص�����
		LPVOID RTAdress;													//�ص������е�ָ�����
		std::shared_ptr<RadarPointList> pRadarPointList;					//ת�����һ֡����
		std::shared_ptr<RadarPointList> FilteredPointCloud;					//�˲����һ֡����
		bool destoryTrasfer;												//�˳�whileѭ����־λ
		bool eraseFrame;													//ȡһ֡��
		unsigned long long FileFrameIndex;									//���ڶ�ȡ�ļ�֡�ı�ţ����������֡���Ϊ0
		std::mutex mutex_frame_list;										//����frameList��
	};
}