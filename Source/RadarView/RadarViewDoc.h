
// RadarViewDoc.h: CRadarViewDoc 类的接口
//
#pragma once
#include <memory>
#include "StructDefine.h"
#include "Reader.h"
#include "RawProcess.h"//处理原始数据buff
#include "FrameProcess.h"
#include "SensorAttribute.h"

#define MY_MSG_AlReadyReadFilePath WM_USER+1023//进行文件读取

using namespace DataReader;

class CRadarViewDoc : public CDocument
{
protected: // 仅从序列化创建
	CRadarViewDoc();													//CRadarViewDoc类的构造函数
	DECLARE_DYNCREATE(CRadarViewDoc)
	void SetTitle(LPCTSTR lpszTitle);									//设置窗口的标题
// 重写
public:
	virtual BOOL OnNewDocument();										//创建Document类时调用的函数
	virtual void Serialize(CArchive& ar);								//CRadarViewDoc 序列化
	
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CRadarViewDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

	CWinThread *pDataReader;											//文件读取线程指针
	CWinThread *pRawProcessor;											//原始buff转换线程指针
	CWinThread *pFrameProcessor;										//帧转换线程指针
	std::shared_ptr<RadarPointList> Frame;								//点帧的智能指针
	unsigned int FilterFrameNumber;										//传感器的滤波帧数

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnCloseDocument();										//关闭Document类之前的清理工作
	afx_msg void csvReaderdata();										//csv读取事件
	afx_msg void StartPcapReader();										//创建线程函数,pcap读取事件函数
	afx_msg void StartSensorReader();									//创建线程函数,Sensor读取事件函数
	afx_msg void SaveSensorData();										//启动保存文件函数设置
	afx_msg void SaveFrameDataAsCSV();									//csv保存事件，和读取事件相对应

	LRESULT OnAlReadyReadFilePath(WPARAM wParam, LPARAM lParam);		//已经设置路径成功
	static void PcapReader(LPVOID pParam);								//打开pcap文件线程函数
	static void SensorReader(LPVOID pParam);							//打开sensor线程函数
	static void RawProcessor(LPVOID pParam);							//保存文件线程函数
	static void DataTsfer(LPVOID pParam);								//一直都在转换帧线程函数
	static void React_OneFrame(std::shared_ptr<DataDefinition::RawFrame> &lParam, LPVOID pParam);//进行实时显示帧的回调函数
	static void React_DataTsfer(std::shared_ptr<RadarPointList> &lParam, LPVOID pParam);//数据转换好的回调函数
	static void SaveReaderFull(LPVOID pParam);							//保存文件太满的返回函数，之后关闭保存线程
public:
	DataReader::Reader myReader;										//读取原始buff类
	DataProcess::RawProcess myRawProcessor;								//处理原始数据类
	DataProcess::FrameProcess myFrameProcessor;							//转换帧类
	bool m_bIsFilter;													//是否进行滤波
	bool m_bFrameSize;													//帧显示尺寸更新标志位
	CString File_path;													//打开的文件的路径,主要是为了让打开最近文件的之后外面可以直接调用
	DataDefinition::SensorAttribute mySensor;							//传感器的属性结构体
	int isMruFile;														//标志位0表示还没有有效文件名，1表示最近文件，2表示选中文件开始有效，3表示获取文件失败
	afx_msg void OnBinOpen();
};
