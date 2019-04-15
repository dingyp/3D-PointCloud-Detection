#pragma once
#include "..\SceneGraph\SceneObjectNode.h"
#include "StructDefine.h"
#include "Transform.h"
#include "ColorTable.h" 
#include <Gl/GL.h>

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
	void RefreshPointData(double addX, double addY, double addZ, double pitch, double roll, \
		double yaw, DataProcess::Transform &transformer);				//对里面的数据进行更新
	void ClearData();													//清除所有显示
	bool GetCrossPoint(VECTOR nearpt, VECTOR farpt, RadarPoint &pt);	//搜索nearpt和farpt连成的线与点云的交点中离视线最近的点
	void ChangeColorMode(int set_ColorMode);							//改变颜色的编码模式
	int GetColorMode();													//获取颜色的编码模式
	void ReScaleColorRange();											//重置颜色编码的数值范围
	void SetColorCoding(COLORCODING chooseColorCoding);					//重新设置编码属性
	void GetColorCoding(COLORCODING &getColorCoding);					//得到编码属性
	void SetDataRange(double DataMin, double DataMax);					//设置数据的最小最大值
	void GetDataRange(double &DataMin, double &DataMax);				//获取点中数值的最大最小值
	BOOL GetMaxAndMin(double &min,double &max);							//得到所有点的最大最小值
	void GetSameAngleH(RadarPointList& sameAngleH, unsigned short set_AngleH);//得到相同竖直角的所有点
	void SetSelectChannel(int SetChannelnumber);						//设置高亮显示的通道
	void CancelChannel();												//取消通道的高亮显示
	void GetDefaultBoundBox(double &GridLength, double &GridRadius);	//获取当前点帧的的BoundBox方面的相关属性
	RadarPointList & gerCurrentPointList();
protected:
	void DrawPoints();													//对点帧进行绘制
	void GetColorFromData(RadarPoint onePoint,double fData, \
		GLubyte &R, GLubyte &G, GLubyte &B);							//获取某一点对应的颜色，除了赋予颜色之外，里面还添加了一个针对特定通道进行高亮显示的功能

protected:
	RadarPointList m_PointsList;										//存储一帧的点帧队列
	int  m_iColorMode;													//颜色编码方式
	double m_DataMin, m_DataMax;										//数据点中的最大最小值
	int isSelectStatus;													//在选择状态对应的通道会进行高亮显示,-1代表不高亮显示
	float m_PointSize;													//点的大小
	COLORCODING ColorCoding;											//编码属性
};

