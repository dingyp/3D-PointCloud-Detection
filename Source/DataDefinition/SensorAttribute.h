#pragma once
#include "stdafx.h"
#include "Packet.h"

//�����������һЩ����

namespace DataDefinition
{
	typedef struct SensorAttribute		//���������ԵĽṹ��
	{
		double sensorX;					//������x����
		double sensorY;					//������y����
		double sensorZ;					//������z����
		double sensorPitch;				//����������б
		double sensorRoll;				//������������ת��
		double sensorYaw;				//��������ƫ��
		unsigned short sensorPort;		//������������˿�
		unsigned short localPort;		//�������ı��ض˿�
		LiDAR_Protocol sensorKind;		//������������
		char ip[16];					//��������IP��
		char mask[16];					//��������MASK
	}SensorAttribute;
}