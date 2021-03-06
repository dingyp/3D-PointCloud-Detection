
// RadarViewView.h: CRadarViewView 类的接口
//

#pragma once
#include "Points3DWnd.h"
#include "RadarViewDoc.h"
#include "ColorLegendDlg.h"
#define MY_MSG_SensorReader WM_USER+1									//重新设置关于帧的信息的事件
#define MY_MSG_ChangStatusBar WM_USER+1019								//对状态栏中第wParam个框格显示字符串lParam事件
#define MY_MSG_RestDir WM_USER+1021										//重置工具栏dirView事件
#define MY_MSG_FileReader WM_USER+1022									//进行文件的路径读取

class CRadarViewView : public CView										//继承自CView，是应用程序CView类，对程序的显示进行管理
{
protected:
	DECLARE_DYNCREATE(CRadarViewView)
	CRadarViewView();													//构造函数
public:
	virtual ~CRadarViewView();											//析构函数
	CRadarViewDoc * GetDocument() const;								//获取GetDocument，内联函数
	Points3DWnd * Get3DWnd() { return m_pPoints3DWnd; }					//获取对点、线、网格进行管理的3DWnd类
	void ClearFrameShow();												//清除画面上所有的点，包括点的线
	void ClearLinesAndPoints();											//取消显示取点和画的线
	void SetRularStatus(int MeasureStatus);								//设置测量状态
	void SetGridView(LINEPLANE_TYPE eLinePlaneType);					//设置网格朝向
	virtual void OnDraw(CDC* pDC);										//重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);						//创建窗口前的设置工作
	BOOL GetIsLegendShow() { return m_bLegendShow; };					//获取颜色标尺是否显示
	void MoveLegend();													//对尺度进行移动
	void ReDrawLegend();												//对标尺进行重新绘制
	BOOL GetPointsData(RadarPointList &PointList);						//获取正在显示的点帧
	void SetPoint3DWndFocus();											//设置3D焦点

	void Get_IsFilter(bool &isFilter);									//获取是否进行滤波
	void Set_IsFilter(bool isFilter);									//设置是否进行滤波

	BOOL SetSensorAttribute(int Type);											//连接之后传感器属性设置
	BOOL SetSensorConAttribute();										//进行传感器连接属性设置
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
	Points3DWnd * m_pPoints3DWnd;										//对显示的点、线、网格等进行管理
	ColorLegendDlg *m_colorLegendDlg;									//颜色标尺的对象指针
	bool m_bLegendShow;													//颜色标尺是否显示的标志位
	CRect Relative_position;											//颜色标尺的相对位置

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);				//CRadarViewView类在被创建的时候调用
	afx_msg void OnSize(UINT nType, int cx, int cy);					//设置显示窗口的大小
	afx_msg void OnDestroy();											// CRadarViewView类在销毁前调用的响应函数
	afx_msg void OnColorLegned();										//点击标尺显示按钮的响应函数，可以显示或隐藏颜色标尺
	afx_msg void OnChangeColorMode();									//改变颜色编码的模式的响应函数
	afx_msg void OnCustomColorRange();									//手动改变数据范围
	afx_msg void OnReScaleColorRange();									//重置颜色范围的响应函数
	afx_msg void OnStart_Stop();										//播放暂停按钮事件的响应函数
	afx_msg void OnGoToStart();											//回归开始按钮事件
	afx_msg void OnSeekBackward();										//后退按钮事件
	afx_msg void OnSeekForward();										//前进按钮事件
	afx_msg void OnGoToEnd();											//跳到结束按钮事件
	afx_msg void OnLoop();												//循环播放按钮事件
	afx_msg void OnResetView();											//重置视野响应函数
	afx_msg void OnOrthoproj_Perspectiveproj();							//进行正交投影和透视投影的切换的响应函数
	afx_msg void OnRularButton();										//两点间测量的响应函数
	afx_msg void OnRularZeroButton();									//测量点到零点距离的响应函数
	afx_msg void OnXYView();											//进行xy平面的显示响应函数
	afx_msg void OnYZView();											//进行yz平面的显示响应函数
	afx_msg void OnXZView();											//进行xz平面的显示响应函数
	afx_msg void OnChooseDir(UINT nID);									//调整视野方向的响应事件

	afx_msg void OnEnLargeView();										//视野进行放大响应事件
	afx_msg void OnReduceView();										//视野进行缩小响应事件

	afx_msg void OnChooseGridDraw();									//网格类型切换的响应函数
	afx_msg void OnSensorSetting();										//传感器属性设置的响应函数
	afx_msg void OnGridSetting();										//网格设置的响应函数
	afx_msg LRESULT ChangStatusTEXT(WPARAM wParam, LPARAM lParam);		//对状态栏中第wParam个框格显示字符串lParam
	afx_msg LRESULT ChangeFrameMessage(WPARAM wParam, LPARAM lParam);	//重新设置关于帧的信息的事件响应函数
	afx_msg LRESULT ResetDirInfo(WPARAM wParam, LPARAM lParam);			//重置工具栏dirView
	afx_msg LRESULT GetFilePath(WPARAM wParam, LPARAM lParam);			//读取file路径并返回
};

#ifndef _DEBUG															// RadarViewView.cpp 中的调试版本
inline CRadarViewDoc* CRadarViewView::GetDocument() const
   { return reinterpret_cast<CRadarViewDoc*>(m_pDocument); }
#endif
