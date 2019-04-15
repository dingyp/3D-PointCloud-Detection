#pragma once
#include "..\SceneGraph\Scene3DMessageWnd.h"
#include "StructDefine.h"
#include "RadarPointsNode.h"
#include "BaseLinesNode.h"
#include "Transform.h"
#include "afxmt.h"
#include <stdio.h>
#include "RadarAxisNode.h"
#include <algorithm>
using namespace std;
class Points3DWnd :																			//对界面中显示的点、线、网格等进行管理的类
	public Scene3DMessageWnd
{
public:
	Points3DWnd();																			//Points3DWnd类的默认构造函数
	virtual ~Points3DWnd();																	//Points3DWnd类的默认析构函数
	void SetMeasureStatus(E3DMouseMode m_MouseMode);										//设置测量的状态
	void SetProjMode(BOOL bProjMode);														//设置投影模式

protected:
	BOOL  m_bLButtonDown;																	//左键按下标志
	BOOL  m_bRButtonDown;																	//右键按下标志
	BOOL  m_bCtrlDown;																		//ctrl键按下标志
	float m_fAspect;																		//窗口的宽长比
	PointArray m_PointList;																	//显示点的列表
	int m_DblClkPointIdx;																	//单独的一个双击点在Mark中下标的位置，-1代表没有，0...代表在里面的下标
	BaseLinesNode * m_BaseLineNode;															//对网格的操作类
	RadarAxisNode * m_AxisNode;																//对坐标轴的操作类
	CCriticalSection m_CriticalSection;														//数据同步问题
	BOOL  m_bCurProjMode;																	//投影模式，TRUE--透视投影;FALSE--正交投影
	E3DMouseMode m_MouseMode;																//鼠标模式
	DataProcess::Transform transformer;														//进行坐标变换操作类

public:
	SceneGroup * section_group;																//所有节点都放在这个下面
	RadarPointsNode * m_RadarPointsNode;													//雷达的点云节点集
	CWnd *m_pParentWnd;																		//父窗口指针
	double m_rangex, m_rangey, m_rangez;													//范围,离0的最远距离，该坐标影响网格大小
	double eyex, eyey, eyez;																//重置视野的位置
	double        m_GridX, m_GridY,m_GridZ;													//网格的中心坐标
	double m_GridLength;																	//网格的边长
	unsigned int m_GridResolution;															//网格的分辨率
	double m_GridRadius;																	//网格的半径
	unsigned int m_GridConCircle;															//网格同心圆的个数
	double m_GridWidth;																		//网线的粗细
	VECTOR m_GridColor;																		//网格的颜色
	LINEDRAW_TYPE m_bGridType;																//网格的种类

protected:
	void InitScene();																		//初始化视野场景
	void AddMark(VECTOR pos, int id);														//往显示里面添加序号是id的点
	void DeleteMark(int id);																//删除里面序号是id的点
	void AddMeasureLine(VECTOR pt3DFrm, VECTOR pt3DTo, int id);								//增添设置连线的粗细linewidth
	void AddLine(VECTOR pt3DFrm, VECTOR pt3DTo, int id, float linewidth);					//往显示里面增加一条线
	void DeleteLine(int id);																//删除序号是id的点
	void ClearAllMark();																	//清除的模式
	BOOL GetCurent3DPosition(CPoint point, RadarPoint &pos, VECTOR &vector);				//获取距离选中位置最近的点的坐标
	double Distance(double x, double y, double z);											//测量某一坐标到原点的距离
	void SetupWndView();																	//重新设置窗口的视野

public:
	virtual BOOL Create(CRect rect, CWnd* pParent, UINT id);								//创建3D窗口
	void AttachData(RadarPointList &PointList);												//更新显示的点帧
	bool GetPointListData(RadarPointList &PointList);										//得到PointList
	void RefreshData(double addX, double addY, double addZ, double pitch, double roll, double yaw);//对显示的数据进行更新
	void ClearData();																		//清除所有显示数据
	void ClearLinesAndPoints();																//清除所有的线
	void SetSensorCenter(double sensor_x, double sensor_y, double sensor_z);				//设置传感器中心的坐标
	void SetSensorEuler(double sensor_pitch, double sensor_roll, double sensor_yaw);		//设置传感器的姿态角
	void GetSensorCenter(double &sensor_x, double &sensor_y, double &sensor_z);				//获取传感器的中心点坐标
	void GetSensorEuler(double &sensor_pitch, double &sensor_roll, double &sensor_yaw);		//获取传感器的姿态角
	void SetValueRange(double MinValue, double MaxValue);									//点属性值的最大最小,不光光指坐标值
	void GetValueRange(double &MinValue, double &MaxValue);									//获取点属性的最大最小值
	void SetGridThickness(float LineWidth);													//设置网线粗细
	void SetGridColor(VECTOR gridColor);													//设置网格颜色,里面的颜色是需要进行归一化
	void SetGridCenter(float addx, float addy,float addz);									//设置网格中心点
	void SetGridParameters(float LineDistance, int LineResolution);							//设置网格的参数
	void GetGridParameters(float &LineDistance, int &LineResolution);						//获取网格的参数
	void SetCircleParameters(float LineDistance, int Circlenumber);							//设置同心圆的参数
	void GetCircleParameters(float &LineDistance, int &Circlenumber);						//获取同心圆的参数
	void SetGridDir(LINEPLANE_TYPE eLinePlaneType);											//设置网格朝向
	void SetDir(VIEW_DIR viewDir);															//设置视野方向
	void SetGridType(LINEDRAW_TYPE eLineDrawType);											//设置网格类型
	LINEDRAW_TYPE GetGirdType();															//获取网格类型
	void ResetView();																		//重置视野的方向
	void SetColorMode(int set_ColorMode);													//设置颜色编码的模式
	int GetColorMode();																		//获取颜色编码的ID号
	void ReScaleColorRange();																//重新测量数值的范围并对颜色重新进行编码
	void SetColorCoding(COLORCODING chooseColorCoding, bool bUpdate);						//设置颜色编码的属性,bUpdate表示是否现场更新
	void GetColorCoding(COLORCODING &ColorCoding);											//获取颜色编码的属性
	void TransformPointInformation(RadarPoint pos);											//给状态栏第2个框格发送关于点的消息
	void SetBoundBox(double x1, double x2, double y1, double y2, double z1, double z2);		//设置显示界面的BoundBox
	void GetDefaultBoundBox(double &GridLength, double &GridRadius);						//自动求出BoundBox并返回默认的size,方便重置网格
	void SetAxisNodeSize(double setLength);													//设置坐标轴的大小
	void SetAxisNodeIsShow(bool setIsDraw);													//设置坐标轴是否显示
	void GetAxisNodeIsShow(bool &setIsDraw);												//获取坐标轴是否显示标志位
	void EnLargeView();																		//放大视野函数
	void ReduceView();																		//缩小视野函数

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);									//鼠标左键单击击下的响应函数
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);									//鼠标左键单击回退的响应函数
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);									//鼠标移动的响应函数
	afx_msg void OnSize(UINT nType, int cx, int cy);										//改变尺寸的响应函数
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);						//鼠标滚轮滑动的响应函数
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);								//鼠标左键双击的响应函数
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);									//鼠标右键单击击下的响应函数
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);									//鼠标右键单击回退的响应函数
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);							//键盘击下的响应函数
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);							//键盘回退的响应函数
};
