#pragma once
#include "stdafx.h"
#include "StructDefine.h"
//������֡���ݽ���3Dת��

#ifdef DATAPROCESS_EXPORTS
#define Transform_API __declspec(dllexport)
#else
#define Transform_API __declspec(dllimport)
#endif

namespace DataProcess
{
	//��ʾǰ��3D�任����תƽ�ƣ�
	class Transform_API Transform
	{
	public:
		Transform();
		~Transform();
		void SetSensorCenter(double addX, double addY, double addZ);				//����ƽ����
		void SetSensorEuler(double setPitch, double setRoll, double setYaw);		//������̬��
		void GetSensorCenter(double &addX, double &addY, double &addZ);				//��ȡƽ����
		void GetSensorEuler(double &pitch, double &roll, double &yaw);				//��ȡ��̬��
		BOOL IsChangeBefore();														//֡��ʾǰ�ж��Ƿ���ҪԤ�Ƚ��б任
		BOOL ChangeBefore(RadarPointList &PointList);								//�Ե����ת��
		void ChangeOne(double addX, double addY, double addZ, double pitch, \
			double roll, double yaw,RadarPointList &PointList);						//ֻ�Ե�ǰ����֡���еı任������֡��Ӱ��
		void Translation(double transform[][4],double dx,double dy,double dz);		//�ڱ任�����м���ƽ�Ʊ任
		void Rotation(double transform[][4], int axis, double theta);				//�ڱ任�����м���ĳһ��̬�Ǳ任
		void InitTransform(double transform[][4]);									//��ʼ���任����

	private:
		double transform[4][4];														//�任����
		double m_addX, m_addY, m_addZ;												//����ƽ����
		double m_pitch, m_roll, m_yaw;												//���ô���������̬��
		BOOL ischange;																//ƽ��������̬���Ƿ�ı��־λ
	};
}