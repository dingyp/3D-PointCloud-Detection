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
	//点阵云数据集
	class CPOINTCLOUDS_API CPointClouds
	{
	public:
		CPointClouds();
		~CPointClouds();

		void ResetAttr();													//重置点云的属性，一般用于点云关闭数据，或者重新读取数据之前
		void SetProtocol(LiDAR_Protocol Protocol);							//设置协议
		void UnSetProtocol();												//取消设置协议
		bool LoadAnglesV(LiDAR_Protocol uProtocol);							//根据产品型号转换垂直角度数据，包括角度，弧度，Cos，Sin值
		void ClearAnglesV();												//清除角度数据
		bool AttachPointCloud(pRadarPointList pPointCloud);					//添加一帧点云数据，返回是否添加成功
		bool SetPointCloudNums(UINT pointCloudNums);						//设置点云总数，赋值一开始调用,返回是否调用成功
		void Clear();														//清除现存点云，包括总的点云数目
		static void GetOnePointCloud_BoundBox(pRadarPointList pPointCloud, float& xMin, float& xMax, float& yMin, \
			float& yMax, float& zMin, float& zMax);							//得到一帧点云的boundingBox,这个是全局函数

		bool GetArgPointCloud_BoundBox(float& xMin, float& xMax, float& yMin, float& yMax, float& zMin, float& zMax);//得到平均值的boundingBox
		void GetPointCloudsBoundBox(float& xMin, float& xMax, float& yMin, \
			float& yMax, float& zMin, float& zMax);							//得到全局点云的boundingBox
		void SetParameter(double *parameter);								//设置parameter函数
		void Setchoosechannel(int *set_arry, int number);					//设置配准所用通道

		pRadarPointList *m_PointCloud;										//点云数据数组，分帧数
		unsigned int m_uiCloudFrameNums;									//点云帧数目
		unsigned int m_uiNowCloudNums;										//已有点云数目

		bool is_Calcul;														//是否计算属性标志位，当有点云更新的时候重新置否

		double m_ISDistance[16][2000];										//和STL的交点的距离

		double m_MaxDistance[16][2000];										//距离最大值
		double m_MinDistance[16][2000];										//距离最小值
		double m_AvgDistance[16][2000];										//距离平均值
		unsigned int m_PointNumber[16][2000];								//点的数目
		double m_VarDistance[16][2000];										//距离方差

		double m_Arg_VarDistance[16];										//每一通道的距离方差的平均值
		
		int m_VarDistanceMaxChannel;										//距离方差平均值最大的通道号
		int m_VarDistanceMinChannel;										//距离方差平均值最小的通道号

		double m_parameter[4];												//旋转的矩阵，在显示之前可以进行一些刚性变换

		int m_choosechannel[16];											//配准选的通道号是哪些
		int m_choosechannel_number;											//配准所选通道的个数

		CPointClouds *m_pErrorPointClouds;									//针对每一个点云数据的误差点云数据
		pRadarPointList m_pSysErrorPointCloud;								//系统误差点云数据

		LiDAR_Protocol m_Protocol;											//点云处理协议
		unsigned int LiDAR_Channels;										//LiDAR通道数
		std::vector<int> AnglesV_Deg;										//垂直角度的数值,这个因为是固定的，所以可以设置成为整型
		std::vector<double> AnglesV_Rad;									//垂直角度的弧度
		std::vector<double> AnglesV_Cos;									//垂直角度的cos
		std::vector<double> AnglesV_Sin;									//垂直角度的sin
	};
}
