#pragma once
#include "stdafx.h"
#include "DataProcess\RawProcess.h"
#include "SensorReader.h"
#include "FileReader.h"
#include "csvReader.h"
#include "binReader.h"
//包含所有的Reader的种类，本类的功能只是对每种Reader进行相应的管理，并不进行任何与Reader相关的工作
using namespace DataReader;

#ifdef DATAREADER_EXPORTS
#define READER_API __declspec(dllexport)
#else
#define READER_API __declspec(dllimport)
#endif

namespace DataReader
{
	enum READER_API MODE													//管理读取模式
	{
		INVALID_READER,														//无效读取模式，将自动释放退出
		FILE_READER,														//文件读取模式，转化成PointFrame
		SENSOR_READER,														//传感器读取模式
		CSV_READER,															//csv读取模式
		BIN_READER
	};

	class READER_API Reader													//对每一种Reader进行管理
	{
	public:
		Reader();															//默认构造函数，还未明确功能
		virtual ~Reader();													//对最后所有的资源进行最后一次的检查释放
		int Init(DataProcess::RawProcess *rawProcess);						//初始化原始pcap包的处理类
		void Destory();														//退出所有连接，释放资源
		int Start(int status=0);											//开始
		void Stop();														//暂停
		void DisConnection();												//退出所有连接,之后需要重新建立
		int BuildSensorConnection(const char *IP, unsigned short dest_port, unsigned short local_port, \
			LiDAR_Protocol SetProtocol, const char*Mask="255.255.255.0");	//建立传感器的连接
		int DisSensorConnection();											//取消当前传感器连接
		int BuildFileConnection(const char *path, void(*PlayEndStop)(LPVOID pParam),\
			LPVOID pParam, LiDAR_Protocol SetProtocol);						//建立读取文件连接
		int DisFileConnection();											//取消当前文件连接
		int OpenCSVFile(const char *csvFilePath, void(*Callback)(std::shared_ptr<RadarPointList> &lParam,\
			LPVOID pParam), LPVOID pParam);									//打开CSV文件并显示
		void WriteCSVFile(const char *csv_savepath, RadarPointList& PointsList);//保存csv文件


		int OpenBinFile(const char *binFilePath, void(*Callback)(std::shared_ptr<RadarPointList> &lParam, \
			LPVOID pParam), LPVOID pParam);
		MODE GetReaderMode();												//获取传感器模式
		/*文件阅读器使用的函数*/
		int GoToStart();													//移到一开始
		int GoToEnd();														//移到最后
		int GetNextFrame();													//得到下一帧,返回是否成功
		int GetBackwardFrame();												//得到上一帧
		int GoToFrame(const int FrameNumber);								//跳到具体的某一帧
		//void SetFrameNumber(unsigned int frameNumber);					//跳到具体某一帧，只是置标志位
		int GetFrameNumber();												//得到文件中总的帧数
		void Loop();														//开始循环播放
		void EndLoop();														//结束循环播放
		void PlayForward();													//正向播放
		void PlayBackward();												//回退播放
		PLAYTYPE GetPlayType();												//取得播放类型
		SPEED ACCSpeed();													//加快速度
		void InitSpeed();													//初始化播放速度
		int Get_FileName(char *pcapFileName);								//得到pcap文件名
		/*传感读取中使用函数*/
		int BuildSaverConnection(const char*savepath,void(*SaveEndStop)(LPVOID pParam),\
			LPVOID pParam);													//建立保存文件连接
		void DisSaverConnection();											//取消保存文件连接
	private:
		SensorReader * pSensorReader;										//传感器读取类
		FileReader * pFileReader;											//文件读取类
		CSVReader csvReader;												//csv读取类


		BINReader binReader;												//bin读取类
		char filepath[1000];												//用来保存文件名
		DataProcess::RawProcess *pRawProcess;								//原始数据处理类
		MODE mode;															//数据模式标记变量
	};
}