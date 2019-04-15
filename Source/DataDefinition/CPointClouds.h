#pragma once
#include "stdafx.h"
#include<vector>
#include <memory> //shared_ptr
#include "StructDefine.h"
#include "Packet.h"

using namespace std;

#ifdef DATADEFINITION_EXPORTS
#define CPOINTCLOUDS_API __declspec(dllexport)
#else
#define CPOINTCLOUDS_API __declspec(dllimport)
#endif

namespace DataDefinition
{
	typedef shared_ptr<RadarPointList> pRadarPointList;
	//���������ݼ�
	class CPOINTCLOUDS_API CPointClouds
	{
	public:
		CPointClouds();
		~CPointClouds();

		void ResetAttr();													//���õ��Ƶ����ԣ�һ�����ڵ��ƹر����ݣ��������¶�ȡ����֮ǰ
		void SetProtocol(LiDAR_Protocol Protocol);							//����Э��
		void UnSetProtocol();												//ȡ������Э��
		bool LoadAnglesV(LiDAR_Protocol uProtocol);							//���ݲ�Ʒ�ͺ�ת����ֱ�Ƕ����ݣ������Ƕȣ����ȣ�Cos��Sinֵ
		void ClearAnglesV();												//����Ƕ�����
		bool AttachPointCloud(pRadarPointList pPointCloud);					//���һ֡�������ݣ������Ƿ���ӳɹ�
		bool SetPointCloudNums(UINT pointCloudNums);						//���õ�����������ֵһ��ʼ����,�����Ƿ���óɹ�
		void Clear();														//����ִ���ƣ������ܵĵ�����Ŀ
		static void GetOnePointCloud_BoundBox(pRadarPointList pPointCloud, float& xMin, float& xMax, float& yMin, \
			float& yMax, float& zMin, float& zMax);							//�õ�һ֡���Ƶ�boundingBox,�����ȫ�ֺ���

		bool GetArgPointCloud_BoundBox(float& xMin, float& xMax, float& yMin, float& yMax, float& zMin, float& zMax);//�õ�ƽ��ֵ��boundingBox
		void GetPointCloudsBoundBox(float& xMin, float& xMax, float& yMin, \
			float& yMax, float& zMin, float& zMax);							//�õ�ȫ�ֵ��Ƶ�boundingBox
		void SetParameter(double *parameter);								//����parameter����
		void Setchoosechannel(int *set_arry, int number);					//������׼����ͨ��

		pRadarPointList *m_PointCloud;										//�����������飬��֡��
		unsigned int m_uiCloudFrameNums;									//����֡��Ŀ
		unsigned int m_uiNowCloudNums;										//���е�����Ŀ

		bool is_Calcul;														//�Ƿ�������Ա�־λ�����е��Ƹ��µ�ʱ�������÷�

		double m_ISDistance[16][2000];										//��STL�Ľ���ľ���

		double m_MaxDistance[16][2000];										//�������ֵ
		double m_MinDistance[16][2000];										//������Сֵ
		double m_AvgDistance[16][2000];										//����ƽ��ֵ
		unsigned int m_PointNumber[16][2000];								//�����Ŀ
		double m_VarDistance[16][2000];										//���뷽��

		double m_Arg_VarDistance[16];										//ÿһͨ���ľ��뷽���ƽ��ֵ
		
		int m_VarDistanceMaxChannel;										//���뷽��ƽ��ֵ����ͨ����
		int m_VarDistanceMinChannel;										//���뷽��ƽ��ֵ��С��ͨ����

		double m_parameter[4];												//��ת�ľ�������ʾ֮ǰ���Խ���һЩ���Ա任

		int m_choosechannel[16];											//��׼ѡ��ͨ��������Щ
		int m_choosechannel_number;											//��׼��ѡͨ���ĸ���

		CPointClouds *m_pErrorPointClouds;									//���ÿһ���������ݵ�����������
		pRadarPointList m_pSysErrorPointCloud;								//ϵͳ����������

		LiDAR_Protocol m_Protocol;											//���ƴ���Э��
		unsigned int LiDAR_Channels;										//LiDARͨ����
		std::vector<int> AnglesV_Deg;										//��ֱ�Ƕȵ���ֵ,�����Ϊ�ǹ̶��ģ����Կ������ó�Ϊ����
		std::vector<double> AnglesV_Rad;									//��ֱ�ǶȵĻ���
		std::vector<double> AnglesV_Cos;									//��ֱ�Ƕȵ�cos
		std::vector<double> AnglesV_Sin;									//��ֱ�Ƕȵ�sin
	};
}
