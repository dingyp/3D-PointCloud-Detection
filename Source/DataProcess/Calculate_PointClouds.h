#pragma once
#include "CPointClouds.h"
#include "Packet.h"

#ifdef DATAPROCESS_EXPORTS
#define Calcul_PointClouds_API __declspec(dllexport)
#else
#define Calcul_PointClouds_API __declspec(dllimport)
#endif

using namespace DataDefinition;

namespace DataProcess
{
	class Calcul_PointClouds_API Calcul_PointClouds
	{
	public:
		Calcul_PointClouds();
		~Calcul_PointClouds();

		void Init(LiDAR_Protocol Protocol);									//����Э���ʼ��
		void GetPointClouds(CPointClouds * pPointClouds);					//�õ���֡����

		void Compute_attribution();											//�������֡������
		void Compute_Evaluation(double *result, int arrayNumber);			//��������������ָ��

		CPointClouds * m_pPointClouds;										//��������
	private:
		void Compute_HLP16_attribution();									//����16ͨ������֡������
		void Compute_2D_attribution();										//����2D����֡������

		void Compute_HLP16_Evaluation(double *result, int arrayNumber);		//����16ͨ���������ָ��
		void Compute_2D_Evaluation(double *result, int arrayNumber);		//����2D����֡�������ָ��

		LiDAR_Protocol m_Protocol;											//���ƴ���Э��
		unsigned int LiDAR_Channels;										//LiDARͨ����
	};
}