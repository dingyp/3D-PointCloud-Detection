
// MainFrm.h: CMainFrame 类的接口
//

#pragma once

#include "CToolBar24.h"
#include "StructDefine.h"
#include "CMyDockablePane.h"
#include "CameraDialog.h"
class CMainFrame : public CFrameWnd
{
protected: // 仅从序列化创建
	CMainFrame();
	BOOL OnBarCheck(UINT nID);											//对菜单栏中工具栏选择部分进行打勾显示
	BOOL InitBar(CToolBar24* bar, UINT Bar_ID, UINT BITMap_ID, \
		UINT Disabled_ID, UINT Hot_ID);									//初始化工具栏
	void DockControlBarLeftOf(CToolBar24* bar, CToolBar24* leftof);		//将某个toolbar显示到另外一个的右边
	DECLARE_DYNCREATE(CMainFrame)

public:
	virtual void OnUpdateFrameTitle(BOOL NaDa);							//更新窗口标题
	void Set_ForWard_Bitmap(int setSpeed);								//设置前向播放速度状态
	void Set_Backward_Bitmap(int setSpeed);								//设置回退播放速度状态
	void Get_Speed_Bitmap(int &getSpeed);								//得到速度
	void Set_Dir_Bitmap(VIEW_DIR viewDir);								//设置视图方向
	void Get_Dir_Bitmap(VIEW_DIR &viewDir);								//得到视图方向
	void Set_GridView_Bitmap(LINEPLANE_TYPE eLinePlaneType);			//设置网格方向
	void Get_GridView_Bitmap(LINEPLANE_TYPE &eLinePlaneType) const;		//得到网格方向
	bool Get_GridDraw_Bitmap()const { return m_bGridDraw; };			//得到网格类型的状态
	void Set_GridDraw_Bitmap(bool Set_GridDraw);						//设置网格类型的状态
	bool Get_STARTSTOP_Bitmap()const { return m_bSTARTSTOPStatue; };	//得到播放按钮的状态
	void Set_STARTSTOP_Bitmap(bool Set_StartStopStatue);				//设置播放按钮的状态
	bool Get_LOOP_Bitmap()const { return m_bNOLOOPStatue; };			//得到循环播放位的状态
	void Set_LOOP_Bitmap(bool Set_LoopStatue);							//设置循环播放位的状态
	bool Get_FileSave_Bitmap() const { return m_bFileSaveStatue; };		//得到保存文件的状态
	void Set_FileSave_Bitmap(bool Set_FileSaveStatue);					//设置保存文件的状态
	int Get_ReaderStatus()const { return m_iReaderStatus; };			//得到Reader状态
	void Set_ReaderStatus(int Set_ReaderStatus);						//设置Reader状态
	int Get_Measure_Bitmap() const { return m_iMeasureStatue; };		//返回测量状态
	void Set_Measure_Bitmap(int Set_MeasureStatus);						//设置测量状态
	bool Get_ORTHOGONAL_Bitmap() const { return m_bCurProjMode; };		//得到视图的模式
	void Set_ORTHOGONAL_Bitmap(bool Set_OrthGonalStatus);				//设置视图模式（正交/透视）
	//int GetPos() const;												//得到播放工具栏滑动条的位置
	int SetPos(unsigned int nPos);										//设置播放工具栏滑动条的位置
	void SetTotalFrameNumber(unsigned int totalNumber);					//设置播放工具栏中总的帧数
	void SetAttriComBoxCurSel(int setSel, bool bUpdate=false);			//设置AttributeComBox的值
	void UpdateMenuMruFile(CString *mruFilesArray,int mruFilesNumber);	//更新菜单栏最近文件
	static void Set_PlayStop_Bitmap(LPVOID pParam);						//播放结束设置菜单栏

	CameraDialog cameraDialog;
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CStatusBar m_wndStatusBar;											//状态栏
protected:  
	CToolBar24 m_wndBasicBar;											//基本工具栏
	CColorToolBar m_wndColorBar;										//颜色工具栏
	CPlayToolBar m_wndPlaybackBar;										//播放工具栏
	CToolBar24 m_wndViewBar;											//视图工具栏
	BOOL m_isVisble[4];													//菜单栏中前面打勾是否显示
	BOOL m_isFloating[4];												//保存在工具栏窗口即将隐藏时候的窗口状态
	CRect m_rect[4];													//保存上面四个工具栏即将隐藏时刻窗口的位置
	VIEW_DIR m_dirViewStatus;											//视图方向的状态变量
	BOOL m_bSTARTSTOPStatue;											//开始/暂停按钮状态按钮
	BOOL m_bNOLOOPStatue;												//是否循环播放按钮状态
	BOOL m_bFileSaveStatue;												//是否是文件存储状态
	int m_iReaderStatus;												//是哪种Reader，0代表还没有，1代表FileReader,2代表SensorReader,3代表CSVReader
	BOOL m_bCurProjMode;												//TRUE--透视投影;FALSE--正交投影
	BOOL m_bGridDraw;													//True同心圆，false网格
	LINEPLANE_TYPE m_GridView;											//网格方向，包括NONEVIEW, XOY, XOZ, YOZ
	int m_iMeasureStatue;												//测量状态，0表示不测量，1表示两点间测量，2表示到零点的测量
	int nowPlaySpeed;													//现在的播放速度
	
// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);				//CMainFrame类被创建的时候调用
	afx_msg void OnFileMru(UINT nID);									//确定是哪个最近文件被选中,并判定文件是否还在
	afx_msg void OnSize(UINT nType, int cx, int cy);					//设置初始时候的大小
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);					//主窗口移动时候调用，主要用来移动颜色标尺
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);//失去焦点和得到焦点响应函数
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);				//鼠标左键单击击下的响应函数
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};
