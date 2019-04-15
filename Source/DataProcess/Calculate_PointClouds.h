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

		void Init(LiDAR_Protocol Protocol);									//点云协议初始化
		void GetPointClouds(CPointClouds * pPointClouds);					//得到多帧点云

		void Compute_attribution();											//计算点云帧的属性
		void Compute_Evaluation(double *result, int arrayNumber);			//计算误差并返回评价指标

		CPointClouds * m_pPointClouds;										//点云数据
	private:
		void Compute_HLP16_attribution();									//计算16通道点云帧的属性
		void Compute_2D_attribution();										//计算2D点云帧的属性

		void Compute_HLP16_Evaluation(double *result, int arrayNumber);		//计算16通道误差评价指标
		void Compute_2D_Evaluation(double *result, int arrayNumber);		//计算2D点云帧误差评价指标

		LiDAR_Protocol m_Protocol;											//点云处理协议
		unsigned int LiDAR_Channels;										//LiDAR通道数
	};
}