#pragma once
#include "stdafx.h"
#include "StructDefine.h"
//对坐标帧数据进行3D转换

#ifdef DATAPROCESS_EXPORTS
#define Transform_API __declspec(dllexport)
#else
#define Transform_API __declspec(dllimport)
#endif

namespace DataProcess
{
	//显示前的3D变换（旋转平移）
	class Transform_API Transform
	{
	public:
		Transform();
		~Transform();
		void SetSensorCenter(double addX, double addY, double addZ);				//设置平移量
		void SetSensorEuler(double setPitch, double setRoll, double setYaw);		//设置姿态角
		void GetSensorCenter(double &addX, double &addY, double &addZ);				//获取平移量
		void GetSensorEuler(double &pitch, double &roll, double &yaw);				//获取姿态角
		BOOL IsChangeBefore();														//帧显示前判断是否需要预先进行变换
		BOOL ChangeBefore(RadarPointList &PointList);								//对点进行转换
		void ChangeOne(double addX, double addY, double addZ, double pitch, \
			double roll, double yaw,RadarPointList &PointList);						//只对当前输入帧进行的变换，后续帧无影响
		void Translation(double transform[][4],double dx,double dy,double dz);		//在变换矩阵中加入平移变换
		void Rotation(double transform[][4], int axis, double theta);				//在变换矩阵中加入某一姿态角变换
		void InitTransform(double transform[][4]);									//初始化变换矩阵

	private:
		double transform[4][4];														//变换矩阵
		double m_addX, m_addY, m_addZ;												//坐标平移量
		double m_pitch, m_roll, m_yaw;												//设置传感器的姿态角
		BOOL ischange;																//平移量、姿态角是否改变标志位
	};
}