#pragma once
#include "..\SceneGraph\Scene3DMessageWnd.h"
#include "StructDefine.h"
#include "RadarPointsNode.h"
#include "Transform.h"
#include "afxmt.h"
#include <stdio.h>
#include "RadarAxisNode.h"
#include "STL_GeometryNode.h"
#include "RoomGeometryNode.h"

using namespace DataProcess;

class PointsAndSTL3DWnd :																	//对界面中显示的点以及STL进行管理的类
	public Scene3DMessageWnd
{
public:
	PointsAndSTL3DWnd();																	//Points3DWnd类的默认构造函数
	virtual ~PointsAndSTL3DWnd();															//Points3DWnd类的默认析构函数
	void SetProjMode(BOOL bProjMode);														//设置投影模式

protected:
	BOOL  m_bLButtonDown;																	//左键按下标志
	BOOL  m_bRButtonDown;																	//右键按下标志
	BOOL  m_bCtrlDown;																		//ctrl键按下标志
	float m_fAspect;																		//窗口的宽长比
	PointArray m_PointList;																	//显示点的列表
	RadarAxisNode * m_AxisNode;																//对坐标轴的操作类
	BOOL  m_bCurProjMode;																	//投影模式，TRUE--透视投影;FALSE--正交投影
	E3DMouseMode m_MouseMode;																//鼠标模式
	Transform transformer;														//进行坐标变换操作类

public:
	SceneGroup * section_group;																//所有节点都放在这个下面
	RadarPointsNode * m_RadarPointsNode;													//雷达的点云节点集
	STL_GeometryNode * m_STLGeometryNode;													//房间的STL文件读取显示操作类
	RoomGeometryNode * m_pRoomNode;															//房间的obj文件读取显示操作类

	CWnd *m_pParentWnd;																		//父窗口指针
	double m_rangex, m_rangey, m_rangez;													//范围,离0的最远距离，该坐标影响网格大小
	double m_rangeTotal;																	//最终的范围，它受x、y、z的影响
	float eyex, eyey, eyez;																	//重置视野的位置

protected:
	void InitScene();																		//初始化视野场景
	void SetupWndView();																	//重新设置窗口的视野

public:
	virtual BOOL Create(CRect rect, CWnd* pParent, UINT id);								//创建3D窗口
	void ClearData();																		//清除所有显示数据
	void SetSensorCenter(double sensor_x, double sensor_y, double sensor_z);				//设置传感器中心的坐标
	void SetSensorEuler(double sensor_pitch, double sensor_roll, double sensor_yaw);		//设置传感器的姿态角
	void GetSensorCenter(double &sensor_x, double &sensor_y, double &sensor_z);				//获取传感器的中心点坐标
	void GetSensorEuler(double &sensor_pitch, double &sensor_roll, double &sensor_yaw);		//获取传感器的姿态角
	void SetValueRange(double MinValue, double MaxValue);									//点属性值的最大最小,不光光指坐标值
	void GetValueRange(double &MinValue, double &MaxValue);									//获取点属性的最大最小值
	void ResetView();																		//重置视野的方向
	void SetBoundBox(double x1, double x2, double y1, double y2, double z1, double z2);		//设置显示界面的BoundBox
	void GetBoundBox(double &x1, double &x2, double &y1, double &y2, double &z1, double &z2);//自动求出BoundBox并返回默认的size,方便重置网格
	void SetAxisNodeSize(double setLength);													//设置坐标轴的大小

	void AttachDataList(CDataList * pDataList);												//添加STL数据list
	void SetPolygonMode(GLenum Mode);														//设置表面的绘制方式
	void AttachRoomModel(CModelList * pModellist);											//添加OBJ数据
	void AttachPointClouds(CPointClouds *pPointClouds);										//添加点云数据

	void AfterRigidRegistration();															//在点云和STL配准之后的事情

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

