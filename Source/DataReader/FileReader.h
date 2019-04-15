#pragma once
#include "stdafx.h"
#include <memory>
#include <mutex>
#include <mmsystem.h>
#include "DataDefinition\Packet.h"
#include "DataProcess\RawProcess.h"

#ifdef DATAREADER_EXPORTS
#define FILEREADER_API __declspec(dllexport)
#else
#define FILEREADER_API __declspec(dllimport)
#endif

namespace DataReader
{
	enum FILEREADER_API SPEED
	{																		//帧的播放速度枚举类型
		READER_STOP,														//0播放速度，停止
		ONE_SPEED,															//1单位的速度==默认播放速度
		DOUBLE_SPEED,														//2单位的速度
		THREE_SPEED,														//3单位的速度
		FOUR_SPEED,															//4单位的速度
		EIGHT_SPEED,														//8单位的速度
		SIXTEEN_SPEED,														//16单位的速度
		ONE_POINT_FIVE_SPEED,												//1.5单位的速度
		HALF_SPEED,															//0.5单位的速度
		ONE_THRID_SPEED,													//0.33单位的速度
		QUARTER_SPEED,														//0.25单位的速度
		POINT_ONE_SPEED,													//0.1单位的速度
		DEFAULT_SPEED,														//默认播放速度==1单位的速度
	};

	enum FILEREADER_API PLAYTYPE											//帧的播放顺序
	{
		FORWORD,															//正序播放
		BACKWORD															//回退播放
	};

	enum FILEREADER_API PLAYLOOP											//是否循环播放的枚举类型
	{
		NOLOOP,																//不循环播放
		LOOP																//循环播放
	};

	typedef struct FILEREADER_API offset_table								//定义一个索引的结构体
	{
		unsigned long long index_number;									//总的索引数目,0代表还没开始建立索引
		unsigned long long *index_table;									//索引表
	}offset_table;

	class FILEREADER_API FileReader											//对文件进行操作的类
	{
	public:
		FileReader();
		~FileReader();
		int Init(DataProcess::RawProcess *pRawProcess, void(*PlayEndStop)(\
			LPVOID pParam), LPVOID pParam, LiDAR_Protocol SetProtocol);		//初始化播放到停止的回调函数
		void Destory();														//退出文件读取线程，释放资源
		bool GetIsStart();													//查看读线程是否已经开启
		int FileOpen(const char *filepath);									//连接pcap文件
		int FileClose();													//关闭pcap文件连接
		int Start(int status);												//开启pcap文件读取线程
		void ReStart(int status = 0);										//重新开启读取
		void Stop();														//暂停文件的读取
		void PlayForward();													//正向播放
		void PlayBackward();												//回退播放
		PLAYTYPE GetPlayType();												//得到播放类型
		void Loop();														//开始循环播放
		void EndLoop();														//结束循环播放
		int SetSpeed(const SPEED set_speed= DEFAULT_SPEED);					//设置速度
		SPEED ACCSpeed();													//改变速度（先加快后减缓）
		//void DECSpeed();													//减缓速度
		void SetPlayState(const SPEED set_speed = DEFAULT_SPEED, \
			const PLAYTYPE set_type = FORWORD, const PLAYLOOP set_loop = NOLOOP);//设置播放状态
		void RessetState();													//重置播放速度为默认播放速度
		//int GetSpeed();													//得到当前的播放速度
		int BeginSleep();													//根据设置的速度进行休眠
		int GetFrameNumber();												//得到文件中总的帧数
		unsigned long long GetCurFrameNumber();								//得到当前帧序号
		void SetFrameNumber(unsigned int frameNumber);						//跳到具体某一帧
		int GetNextFrame(bool reDraw);										//得到下一帧,返回是否成功
		int GetBackwardFrame(bool reDraw);									//得到上一帧,reDraw是否受暂停的影响
		int GoToFrame(const int FrameNumber, bool reDraw);					//跳到具体的某一帧
		void ReadPcapPacket();												//读取一个pcap块
		void ReadPcapngPacket();											//读取一个pcapng块
		int GoToStart();													//回到一开始
		int GoToEnd();														//移到最后
	private:
		static void CALLBACK CallBackMilliSecondProc(UINT wTimerID, UINT msg, \
			DWORD_PTR dwUser, DWORD dw1, DWORD dw2);						//回调函数
		int Init_offsetTable(std::vector<unsigned long long> &idx_table);	//初始化索引表，只能修改一次
		static void StepUpOffsetTable(LPVOID pParam);						//建立索引表
		void StepUpPcapOffsetTable();										//建立pcap索引表
		void StepUpPcapngOffsetTable();										//建立pcapng索引表
		void Free_offsetTable();											//置空索引表
	private:
		void(*PlayStop)(LPVOID pParam);										//播放至最后一帧的回调函数
		std::ifstream *file_reader;											//读取文件指针
		std::shared_ptr<DataDefinition::raw_packetInfo> Info;				//每次传出的pcap包信息指针
		SPEED speed;														//播放速度(本状态以及下面两个状态互不影响)
		PLAYTYPE playType;													//播放类型
		PLAYLOOP loop;														//循环播放位，标记是否循环播放
		LPVOID RTAdress;													//返回句柄
		MMRESULT g_uTimerID;												//控制速度的多媒体计时器ID
		int remain_number;													//多媒体计时器的剩余读取次数
		int add_number;														//一次增加的次数，调节速度
		offset_table *offset_idxTable;										//帧索引表,指明某一帧在文件中的绝对位置
		bool isOffsetOK;													//索引表是否能够创建标志位
		unsigned long long cur_index;										//当前读到文件的索引
		bool destoryFileReader;												//退出线程标志位
		DataProcess::RawProcess *pRawProcess;								//处理原始数据的类
		bool reDraw;														//是否重新绘制标志位
		std::mutex mutex_FileReader;										//文件读取的互斥变量
		LiDAR_Protocol Protocol;											//文件读取协议
	};
}