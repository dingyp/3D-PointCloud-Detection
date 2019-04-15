#include <math.h>
#include "stdafx.h"
#include "Transform.h"
#define PI 3.14159265358979

using namespace DataProcess;

/**************************************************************************
*  @Function Name :: Transform
*  @Description   :: ������֡����3D�任��Ĺ��캯������ʼ���任����
*  @Author        :: zzw
*  @Parameters    :: None
*					
*  @Return        :: None
**************************************************************************/
Transform::Transform()
{
	m_addX = 0.0;
	m_addY = 0.0;
	m_addZ = 0.0;
	m_pitch = 0.0;
	m_roll = 0.0;
	m_yaw = 0.0;
	ischange = FALSE;
}

/**************************************************************************
*  @Function Name :: ~Transform
*  @Description   :: ������֡����3D�任�����������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
Transform::~Transform()
{
}

/**************************************************************************
*  @Function Name :: GetSensorCenter
*  @Description   :: ��ȡƽ����
*  @Author        :: zzw
*  @Parameters    :: addX:X����ƽ���� addY:Y����ƽ����
*					 addZ:Z����ƽ����
*  @Return        :: None
**************************************************************************/
void Transform::GetSensorCenter(double &addX, double &addY, double &addZ)
{
	addX = m_addX;
	addY = m_addY;
	addZ = m_addZ;
}

/**************************************************************************
*  @Function Name :: GetSensorEuler
*  @Description   :: ��ȡ��̬��
*  @Author        :: zzw
*  @Parameters    :: pitch:X����̬�� roll:Y����̬��
*					 yaw:Z����̬��
*  @Return        :: None
**************************************************************************/
void Transform::GetSensorEuler(double &pitch, double &roll, double &yaw)
{
	pitch = m_pitch; 
	roll = m_roll;
	yaw = m_yaw;
}

/**************************************************************************
*  @Function Name :: SetCenter
*  @Description   :: ����ƽ����
*  @Author        :: zzw
*  @Parameters    :: addX:X����ƽ���� addY:Y����ƽ����
*					 addZ:Z����ƽ����
*  @Return        :: None
**************************************************************************/
void Transform::SetSensorCenter(double addX, double addY, double addZ)
{
	m_addX = addX;
	m_addY = addY;
	m_addZ = addZ;
	ischange = TRUE;
}

/**************************************************************************
*  @Function Name :: SetSensorEuler
*  @Description   :: ������̬��
*  @Author        :: zzw
*  @Parameters    :: pitch:X����̬�� roll:Y����̬��
*					 yaw:Z����̬��
*  @Return        :: None
**************************************************************************/
void Transform::SetSensorEuler(double setPitch, double setRoll, double setYaw)
{
	m_pitch = setPitch;
	m_roll = setRoll;
	m_yaw = setYaw;
	ischange = TRUE;
}

/**************************************************************************
*  @Function Name :: IsChangeBefore
*  @Description   :: ֡��ʾǰ�ж��Ƿ���ҪԤ�Ƚ��б任
*  @Author        :: zzw
*  @Parameters    :: None
*					 
*  @Return        :: true��ҪԤ�Ƚ��б任��false����ҪԤ�Ƚ��б任
**************************************************************************/
BOOL Transform::IsChangeBefore()
{
	return (fabs(m_pitch)>1e-7 || fabs(m_roll)>1e-7 || fabs(m_yaw)>1e-7 || fabs(m_addX)>1e-7 || fabs(m_addY)>1e-7 || fabs(m_addZ)>1e-7);
}

/**************************************************************************
*  @Function Name :: ChangeOne
*  @Description   :: ֻ�Ե�ǰĳһ����֡���е�һ���Ա任������֡��Ӱ��
*  @Author        :: zzw
*  @Parameters    :: addX:X����ƽ���� addY:Y����ƽ���� addZ:Z����ƽ����
*					 pitch:X����̬�� roll:Y����̬�� yaw:Z����̬�� PointList:��ת��֡
*  @Return        :: None
**************************************************************************/
void Transform::ChangeOne(double addX, double addY, double addZ, double pitch, double roll, double yaw, RadarPointList &PointList)
{
	double oneTransform[4][4];//�任����
	InitTransform(oneTransform);
	//��任��ʱ���Ƚ���ƽ���ٽ�����ת
	if (fabs(addX) > 1e-7 || fabs(addY) > 1e-7 || fabs(addZ) > 1e-7)
	{
		Translation(oneTransform, addX, addY, addZ);
	}
	if (fabs(pitch) > 1e-7) { Rotation(oneTransform, 0, pitch); }
	if (fabs(roll) > 1e-7) { Rotation(oneTransform, 1, roll); }
	if (fabs(yaw) > 1e-7) { Rotation(oneTransform, 2, yaw); }

	int totalSize = PointList.size();
	double tmpX, tmpY, tmpZ;
	for (int i = 0; i < totalSize; i++)
	{
		tmpX = PointList[i].x*oneTransform[0][0] + PointList[i].y*oneTransform[1][0] + PointList[i].z*oneTransform[2][0] + oneTransform[3][0];
		tmpY = PointList[i].x*oneTransform[0][1] + PointList[i].y*oneTransform[1][1] + PointList[i].z*oneTransform[2][1] + oneTransform[3][1];
		tmpZ = PointList[i].x*oneTransform[0][2] + PointList[i].y*oneTransform[1][2] + PointList[i].z*oneTransform[2][2] + oneTransform[3][2];
		PointList[i].x = tmpX;
		PointList[i].y = tmpY;
		PointList[i].z = tmpZ;
	}
}

/**************************************************************************
*  @Function Name :: ChangeBefore
*  @Description   :: �Ե����ת��
*  @Author        :: zzw
*  @Parameters    :: PointList:��ת��֡
*					 
*  @Return        :: true:����ת���Ƿ�ɹ�
**************************************************************************/
BOOL Transform::ChangeBefore(RadarPointList &PointList)
{
	//�ж��Ƿ���Ҫ�����4*4�任����
	if (ischange)
	{//��Ҫ���¼���
		InitTransform(transform);
		if (fabs(m_pitch) > 1e-7) { Rotation(transform, 0, m_pitch);}
		if (fabs(m_roll) > 1e-7) { Rotation(transform, 1, m_roll); }
		if (fabs(m_yaw) > 1e-7) { Rotation(transform, 2, m_yaw); }
		if (fabs(m_addX) > 1e-7 || fabs(m_addY) > 1e-7 || fabs(m_addZ) > 1e-7)
		{
			Translation(transform, m_addX, m_addY, m_addZ);
		}
		ischange = FALSE;
	}

	//�Ե�֡����3D�任
	int totalSize = PointList.size();
	double tmpX, tmpY, tmpZ;
	for (int i = 0; i < totalSize; i++)
	{
		tmpX = PointList[i].x*transform[0][0] + PointList[i].y*transform[1][0] + PointList[i].z*transform[2][0] + transform[3][0];
		tmpY = PointList[i].x*transform[0][1] + PointList[i].y*transform[1][1] + PointList[i].z*transform[2][1] + transform[3][1];
		tmpZ = PointList[i].x*transform[0][2] + PointList[i].y*transform[1][2] + PointList[i].z*transform[2][2] + transform[3][2];
		PointList[i].x = tmpX;
		PointList[i].y = tmpY;
		PointList[i].z = tmpZ;
	}

	return TRUE;
}

/**************************************************************************
*  @Function Name :: InitTransform
*  @Description   :: ��ʼ���任����
*  @Author        :: zzw
*  @Parameters    :: transform:�任����
*
*  @Return        :: None
**************************************************************************/
void Transform::InitTransform(double transform[][4])
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (i != j)
			{
				transform[i][j] = 0;
			}
			else
			{//�Խ��߸�ֵ1
				transform[i][j] = 1;
			}
		}
	}
}

/**************************************************************************
*  @Function Name :: Translation
*  @Description   :: �ڱ任�����м���ƽ�Ʊ任
*  @Author        :: zzw
*  @Parameters    :: transform:�任����
*					 addX:X����ƽ���� addY:Y����ƽ���� addZ:Z����ƽ����
*  @Return        :: None
**************************************************************************/
void Transform::Translation(double transform[][4], double dx, double dy, double dz)
{
	double result[4][4];
	//��ʼ��ƽ����ʱ����
	double tmp_Matrix[4][4];
	InitTransform(tmp_Matrix);
	tmp_Matrix[3][0] = dx;
	tmp_Matrix[3][1] = dy;
	tmp_Matrix[3][2] = dz;

	//��ƽ�ƾ���ӵ��任����֮��
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i][j] = transform[i][0] * tmp_Matrix[0][j] + transform[i][1] * tmp_Matrix[1][j] \
				+ transform[i][2] * tmp_Matrix[2][j] + transform[i][3] * tmp_Matrix[3][j];
		}
	}
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			transform[i][j]=result[i][j];
		}
	}
}

/**************************************************************************
*  @Function Name :: Rotation
*  @Description   :: �ڱ任�����м���ĳһ��̬�Ǳ任
*  @Author        :: zzw
*  @Parameters    :: transform:�任���� axis:��ת����(0����X�ᣬ1����Y�ᣬ2����Z��)
*					 theta:��ת�ĽǶ�
*  @Return        :: None
**************************************************************************/
void Transform::Rotation(double transform[][4], int axis, double theta)
{
	double result[4][4];
	double tmp_Matrix[4][4];
	InitTransform(tmp_Matrix);
	if (0 == axis)
	{//��X����ת
		tmp_Matrix[0][0] = 1;
		tmp_Matrix[1][1] = cos(theta*PI /180.0);
		tmp_Matrix[1][2] = sin(theta*PI / 180.0);
		tmp_Matrix[2][1] = -sin(theta*PI / 180.0);
		tmp_Matrix[2][2] = cos(theta*PI / 180.0);
	}
	else if (1 == axis)
	{//��Y����ת
		tmp_Matrix[1][1] = 1;
		tmp_Matrix[0][0] = cos(theta*PI / 180.0);
		tmp_Matrix[0][2] = -sin(theta*PI / 180.0);
		tmp_Matrix[2][0] = sin(theta*PI / 180.0);
		tmp_Matrix[2][2] = cos(theta*PI / 180.0);
	}
	else
	{//��Z����ת
		tmp_Matrix[2][2] = 1;
		tmp_Matrix[0][0] = cos(theta*PI / 180.0);
		tmp_Matrix[0][1] = sin(theta*PI / 180.0);
		tmp_Matrix[1][0] = -sin(theta*PI / 180.0);
		tmp_Matrix[1][1] = cos(theta*PI / 180.0);
	}
	tmp_Matrix[3][3] = 1;

	//����ת����ӵ��任����֮��
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i][j] = transform[i][0] * tmp_Matrix[0][j] + transform[i][1] * tmp_Matrix[1][j] \
				+ transform[i][2] * tmp_Matrix[2][j] + transform[i][3] * tmp_Matrix[3][j];
		}
	}
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			transform[i][j] = result[i][j];
		}
	}
}
