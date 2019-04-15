#pragma once
#include "..\SceneGraph\SceneObjectNode.h"
#include "StructDefine.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include "CDataList.h"
#include "CPointClouds.h"

using namespace DataDefinition;

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

	void ClearData();													//���������ʾ
	void SetDataRange(double DataMin, double DataMax);					//�������ݵ���С���ֵ
	void GetDataRange(double &DataMin, double &DataMax);				//��ȡ������ֵ�������Сֵ
	void GetDefaultBoundBox(double &x1,double &x2,double &y1,double &y2,double &z1,double &z2);	//��ȡ��ǰ��֡�ĵ�BoundBox������������

	bool AttachPointClouds(CPointClouds *pPointClouds);
	void BeginCalculation();											//���㣬��������ʾһ֡��������

	void StructuralOptimization();										//���нṹ�Ż���������һ����ĸ�ֵ
protected:
	void DrawPoints();													//�Ե�֡���л���

public:
	//RadarPointList m_PointsList;										//�洢һ֡�ĵ�֡����
	CPointClouds *m_pPointClouds;										//��������

	//pRadarPointList m_pMaxPointCloud;									//���ֵ��������
	//pRadarPointList m_pMinPointCloud;									//��Сֵ��������
	//pRadarPointList m_pAvgPointCloud;									//ƽ��ֵ��������
	//pRadarPointList m_pVarPointCloud;									//�����������
	//
	//CPointClouds *m_pErrorPointClouds;									//���ÿһ���������ݵ�����������
	//pRadarPointList m_pSysErrorPointCloud;								//ϵͳ����������

protected:
	int  m_iColorMode;													//��ɫ���뷽ʽ
	double m_DataMin, m_DataMax;										//���ݵ��е������Сֵ
	int isSelectStatus;													//��ѡ��״̬��Ӧ��ͨ������и�����ʾ,-1����������ʾ
	float m_PointSize;													//��Ĵ�С
};

