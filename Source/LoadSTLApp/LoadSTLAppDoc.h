// LoadSTLAppDoc.h: CLoadSTLAppDoc 类的接口
//
#pragma once
#include "CDataList.h"
#include "CPointClouds.h"
#include "Reader.h"
#include "FrameProcess.h"
#include "SensorAttribute.h"
#include "Transform.h"
#include "FilesLoadLib\obj.h"
#include "Calculate_Intersection.h"
#include "Calculate_PointClouds.h"
#include "PointCloud_RigidRegistration.h"

using namespace DataReader;
using namespace DataProcess;

class CLoadSTLAppDoc : public CDocument
{
protected: // 仅从序列化创建
	CLoadSTLAppDoc();
	DECLARE_DYNCREATE(CLoadSTLAppDoc)

	CWinThread *pDataReader;																//文件读取线程指针
	CWinThread *pRawProcessor;																//原始buff转换线程指针
	CWinThread *pFrameProcessor;															//帧转换线程指针
	std::shared_ptr<RadarPointList> Frame;													//点帧的智能指针
	SensorAttribute mySensor;																//传感器的属性结构体

	Transform transformer;																	//进行坐标变换操作类
	CDataList m_DataList;																	//存储STL的结构体
	CModelList m_RoomModelList;																//存储OBJ的结构体
	CPointClouds m_PointClouds;																//存储多帧点云的结构体
	unsigned int FilterFrameNumber;															//传感器的滤波帧数
	pPointClouds m_pInterSection;															//交点点云类

	
// 特性
public:
	Reader pcapReader;																		//读取原始buff类
	RawProcess pcapRawProcessor;															//处理原始数据类
	FrameProcess pcapFrameProcessor;														//转换帧类
	bool m_bIsFilter;																		//是否进行滤波
	Calcul_PointClouds m_calcul_pointclouds;												//点云计算类
	PointCloud_RigidRegistration m_rigid_registration;										//点云配准类
	Calcul_Intersection m_calcul_intersection;												//进行STL和点云求交点的类

	//下面是配准中用到的一些参数
	double m_parameter[4];																	//4个自由度
	double **m_parvector;																	//自由度优先级参数
	int m_iternum;																			//迭代次数
	double result;																			//互信息计算结果
	static double RegisByVar(void *pDoc, double *p);
	double CalVar(double *p);

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CLoadSTLAppDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

public:
	bool SetPointCloudNums(UINT pointCloudNums);												//设置点云数目，返回是否设置成功
	bool AttachPointCloud(pRadarPointList pPointCloud);											//添加一帧点云数据，返回是否添加成功
	void SetSensorCenter(double sensor_x, double sensor_y, double sensor_z);					//设置传感器中心的坐标
	void SetSensorEuler(double sensor_pitch, double sensor_roll, double sensor_yaw);			//设置传感器的姿态角

	void CalculatePointClouds();																//计算点云集，并最终显示一帧点云数据
	void RigidRegistration();																	//开始点云和STL之间的配准
	void SetShowValue(int PropertyNumber, int SubItemNumber, CString setValue);					//更改表格显示
	void SetShowValue(int PropertyNumber, int SubItemNumber, int int_setValue);
	void SetShowValue(int PropertyNumber, int SubItemNumber, double doub_setValue);
	void AddGridProperty(CMFCPropertyGridProperty *pProp, int PropertyNumber, int SubItemNumber);		//将pProp加到第PropertyNumber的SubItemNumber里面
	void DelGridProperty(int PropertyNumber, int SubItemNumber);									//删除里面内容

	afx_msg void StartPcapReader();																//创建线程函数,pcap读取事件函数
	afx_msg void OnOpenStlfile();																//打开stl文件事件
	afx_msg void OnOpenObjfile();																//打开obj文件事件
	afx_msg void OnBeginCalculation();															//开始进行计算,并输出计算结果
	afx_msg void OnShowDataGrid();																//展示计算过程的对话框
	afx_msg void OnSaveData();																	//保存计算结果响应函数

	static void PcapReader(LPVOID pParam);														//打开pcap文件线程函数
	static void RawProcessor(LPVOID pParam);													//保存文件线程函数
	static void DataTsfer(LPVOID pParam);														//一直都在转换帧线程函数
	static void EndPcapCallBackFun(LPVOID pParam);												//pcap文件读取结束的返回函数
	static void React_OneFrame(std::shared_ptr<RawFrame> &lParam, LPVOID pParam);				//进行实时显示帧的回调函数
	static void React_DataTsfer(std::shared_ptr<RadarPointList> &lParam, LPVOID pParam);		//数据转换好的回调函数

};
