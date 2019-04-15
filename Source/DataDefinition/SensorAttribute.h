#pragma once
#include "stdafx.h"
#include "Packet.h"

//传感器本身的一些属性

namespace DataDefinition
{
	typedef struct SensorAttribute		//传感器属性的结构体
	{
		double sensorX;					//传感器x坐标
		double sensorY;					//传感器y坐标
		double sensorZ;					//传感器z坐标
		double sensorPitch;				//传感器的倾斜
		double sensorRoll;				//传感器的左右转动
		double sensorYaw;				//传感器的偏航
		unsigned short sensorPort;		//传感器的网络端口
		unsigned short localPort;		//传感器的本地端口
		LiDAR_Protocol sensorKind;		//传感器的类型
		char ip[16];					//传感器的IP号
		char mask[16];					//传感器的MASK
	}SensorAttribute;
}