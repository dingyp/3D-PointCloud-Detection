#pragma once
#include "..\SceneGraph\SceneObjectNode.h"
#include "StructDefine.h"
#include "Transform.h"
#include "ColorTable.h" 
#include <Gl/GL.h>

class RadarPointsNode :	public SceneObjectNode							//�Ե����ʾ���в�������
{
public:
	RadarPointsNode();													//RadarPointsNode���Ĭ�Ϲ��캯��
	virtual ~RadarPointsNode();											//RadarPointsNode�����������
	virtual int getType(void) { return RADARPOINTS_NODE; }				//�������
	virtual void Draw();												//���Ƶ�
	void SetPointSize(float PointSize) { m_PointSize = PointSize; }		//���õ�Ĵ�С�ߴ�
	bool AttachPointsData(RadarPointList &DataList);					//����ӽ���
	bool GetPointListData(RadarPointList &PointList);					//�õ�PointList
	void RefreshPointData(double addX, double addY, double addZ, double pitch, double roll, \
		double yaw, DataProcess::Transform &transformer);				//����������ݽ��и���
	void ClearData();													//���������ʾ
	bool GetCrossPoint(VECTOR nearpt, VECTOR farpt, RadarPoint &pt);	//����nearpt��farpt���ɵ�������ƵĽ���������������ĵ�
	void ChangeColorMode(int set_ColorMode);							//�ı���ɫ�ı���ģʽ
	int GetColorMode();													//��ȡ��ɫ�ı���ģʽ
	void ReScaleColorRange();											//������ɫ�������ֵ��Χ
	void SetColorCoding(COLORCODING chooseColorCoding);					//�������ñ�������
	void GetColorCoding(COLORCODING &getColorCoding);					//�õ���������
	void SetDataRange(double DataMin, double DataMax);					//�������ݵ���С���ֵ
	void GetDataRange(double &DataMin, double &DataMax);				//��ȡ������ֵ�������Сֵ
	BOOL GetMaxAndMin(double &min,double &max);							//�õ����е�������Сֵ
	void GetSameAngleH(RadarPointList& sameAngleH, unsigned short set_AngleH);//�õ���ͬ��ֱ�ǵ����е�
	void SetSelectChannel(int SetChannelnumber);						//���ø�����ʾ��ͨ��
	void CancelChannel();												//ȡ��ͨ���ĸ�����ʾ
	void GetDefaultBoundBox(double &GridLength, double &GridRadius);	//��ȡ��ǰ��֡�ĵ�BoundBox������������
	RadarPointList & gerCurrentPointList();
protected:
	void DrawPoints();													//�Ե�֡���л���
	void GetColorFromData(RadarPoint onePoint,double fData, \
		GLubyte &R, GLubyte &G, GLubyte &B);							//��ȡĳһ���Ӧ����ɫ�����˸�����ɫ֮�⣬���滹�����һ������ض�ͨ�����и�����ʾ�Ĺ���

protected:
	RadarPointList m_PointsList;										//�洢һ֡�ĵ�֡����
	int  m_iColorMode;													//��ɫ���뷽ʽ
	double m_DataMin, m_DataMax;										//���ݵ��е������Сֵ
	int isSelectStatus;													//��ѡ��״̬��Ӧ��ͨ������и�����ʾ,-1����������ʾ
	float m_PointSize;													//��Ĵ�С
	COLORCODING ColorCoding;											//��������
};

