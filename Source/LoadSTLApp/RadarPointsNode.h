#pragma once
#include "..\SceneGraph\SceneObjectNode.h"
#include "StructDefine.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include "CDataList.h"
#include "CPointClouds.h"

using namespace DataDefinition;

class RadarPointsNode :	public SceneObjectNode							//对点的显示进行操作的类
{
public:
	RadarPointsNode();													//RadarPointsNode类的默认构造函数
	virtual ~RadarPointsNode();											//RadarPointsNode类的析构函数
	virtual int getType(void) { return RADARPOINTS_NODE; }				//获得类型
	virtual void Draw();												//绘制点
	void SetPointSize(float PointSize) { m_PointSize = PointSize; }		//设置点的大小尺寸
	bool AttachPointsData(RadarPointList &DataList);					//将点加进来
	bool GetPointListData(RadarPointList &PointList);					//得到PointList

	void ClearData();													//清除所有显示
	void SetDataRange(double DataMin, double DataMax);					//设置数据的最小最大值
	void GetDataRange(double &DataMin, double &DataMax);				//获取点中数值的最大最小值
	void GetDefaultBoundBox(double &x1,double &x2,double &y1,double &y2,double &z1,double &z2);	//获取当前点帧的的BoundBox方面的相关属性

	bool AttachPointClouds(CPointClouds *pPointClouds);
	void BeginCalculation();											//计算，并最终显示一帧点云数据

	void StructuralOptimization();										//进行结构优化，比如下一个点的赋值
protected:
	void DrawPoints();													//对点帧进行绘制

public:
	//RadarPointList m_PointsList;										//存储一帧的点帧队列
	CPointClouds *m_pPointClouds;										//点云数据

	//pRadarPointList m_pMaxPointCloud;									//最大值点云数据
	//pRadarPointList m_pMinPointCloud;									//最小值点云数据
	//pRadarPointList m_pAvgPointCloud;									//平均值点云数据
	//pRadarPointList m_pVarPointCloud;									//方差点云数据
	//
	//CPointClouds *m_pErrorPointClouds;									//针对每一个点云数据的误差点云数据
	//pRadarPointList m_pSysErrorPointCloud;								//系统误差点云数据

protected:
	int  m_iColorMode;													//颜色编码方式
	double m_DataMin, m_DataMax;										//数据点中的最大最小值
	int isSelectStatus;													//在选择状态对应的通道会进行高亮显示,-1代表不高亮显示
	float m_PointSize;													//点的大小
};

