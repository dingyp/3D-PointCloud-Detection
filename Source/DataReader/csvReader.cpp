#include "stdafx.h"
#include "csvReader.h"
//#include <iostream>
#include <fstream>
#include <sstream>

using namespace DataReader;

/**************************************************************************
*  @Function Name :: CSVReader
*  @Description   :: CSVReader类的构造函数
*  @Author        :: zzw
*  @Parameters    :: None
*					
*  @Return        :: None
**************************************************************************/
CSVReader::CSVReader()
{
	TransferOneFrame = NULL;
	RTAdress = NULL;
	pRadarPointList = nullptr;
}

/**************************************************************************
*  @Function Name :: ~CSVReader
*  @Description   :: CSVReader类的析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*				
*  @Return        :: None
**************************************************************************/
CSVReader::~CSVReader()
{
	TransferOneFrame = NULL;
	RTAdress = NULL;
	pRadarPointList = nullptr;
}

/**************************************************************************
*  @Function Name :: Init
*  @Description   :: 初始化满一帧的回调函数
*  @Author        :: zzw
*  @Parameters    :: Callback:满一帧的回调函数
*					 pParam:回调函数中的参数
*  @Return        :: None
**************************************************************************/
void CSVReader::Init(void(*Callback)(std::shared_ptr<RadarPointList> &lParam, LPVOID pParam), LPVOID pParam)
{
	TransferOneFrame = Callback;
	RTAdress = pParam;
}

/**************************************************************************
*  @Function Name :: Destory
*  @Description   :: 置空初始化函数
*  @Author        :: zzw
*  @Parameters    :: None
*				
*  @Return        :: None
**************************************************************************/
void CSVReader::Destory()
{
	TransferOneFrame = NULL;
	RTAdress = NULL;
}

/**************************************************************************
*  @Function Name :: CSVOpen
*  @Description   :: 打开并显示csv文件
*  @Author        :: zzw
*  @Parameters    :: csvpath:csv文件的路径
*					 
*  @Return        :: None
**************************************************************************/
void CSVReader::CSVOpen(const char *csvpath)
{
	std::fstream fin;
	char tmp[1024] = { 0 };
	fin.open(csvpath, std::ios::in);
	fin.getline(tmp, sizeof(tmp));							//第一行在读取的时候并不需要
	if (nullptr != pRadarPointList)							//帧指针不为空，退出
	{
		return ;
	}
	pRadarPointList = std::make_shared<RadarPointList>();
	if (nullptr == pRadarPointList)							//申请内存失败，退出
	{
		return ;
	}

	if ((NULL == TransferOneFrame) || (NULL == RTAdress))	//没有初始化回调函数，退出
	{
		return;
	}

	RadarPoint CurPoint;
	while (fin.getline(tmp, sizeof(tmp))) {
		std::stringstream line(tmp);
		std::string str;

		std::getline(line, str,',');						//Point_ID
		CurPoint.Point_ID = std::stoi(str);
		std::getline(line, str, ',');						//distance
		CurPoint.distance = std::stoi(str);
		std::getline(line, str, ',');						//x
		CurPoint.x = std::stod(str);
		std::getline(line, str, ',');						//y
		CurPoint.y = std::stod(str);
		std::getline(line, str, ',');						//z
		CurPoint.z = std::stod(str);
		std::getline(line, str, ',');						//intensity
		CurPoint.intensity = std::stod(str);
		std::getline(line, str, ',');						//angleH
		CurPoint.angleH = (unsigned short)(std::stoi(str));
		std::getline(line, str, ',');						//angleV
		CurPoint.angleV = (unsigned short)(std::stoi(str));
		std::getline(line, str, ',');						//channel
		CurPoint.channel = (unsigned char)(std::stoi(str));
		std::getline(line, str, ',');						//timestamp
		CurPoint.timestamp = (unsigned int)(std::stoi(str));

		pRadarPointList->push_back(CurPoint);
	}

	if ((NULL != TransferOneFrame) && (NULL != RTAdress))	//只有初始化了才调用
	{
		TransferOneFrame(pRadarPointList, RTAdress);
		pRadarPointList = nullptr;
	}
	return;
}

/**************************************************************************
*  @Function Name :: CSVWrite
*  @Description   :: 用来保存csv文件
*  @Author        :: zzw
*  @Parameters    :: csv_savepath:csv文件保存路径
*					 PointsList:待存储的点帧
*  @Return        :: None
**************************************************************************/
void CSVReader::CSVWrite(const char *csv_savepath, RadarPointList& PointsList)
{
	std::fstream fout;
	char tmp[1024] = { 0 };
	fout.open(csv_savepath, std::ios::out);					//打开写文件流

	fout << "Point_id,Distance,X,Y,Z,Intensity,Horizontal_angle,Vertical_angle,Channel,Timestamp" << std::endl;

	int PointNum = PointsList.size();
	char Point_ID[20];
	char distance[20];
	char angleH[20];
	char angleV[20];
	char channel[20];
	char timestamp[20];
	for (int i = 0; i < PointNum; i++)						//遍历对所有点并保存
	{
		_itoa_s(PointsList[i].Point_ID, Point_ID,10);		//10代表十进制
		_itoa_s(PointsList[i].distance, distance, 10);
		_itoa_s(PointsList[i].angleH, angleH, 10);
		_itoa_s(PointsList[i].angleV, angleV, 10);
		_itoa_s(PointsList[i].channel, channel, 10);
		_itoa_s(PointsList[i].timestamp, timestamp, 10);

		fout << Point_ID << "," \
			<< distance << "," \
			<< PointsList[i].x << "," \
			<< PointsList[i].y << "," \
			<< PointsList[i].z << "," \
			<< PointsList[i].intensity << "," \
			<< angleH << "," \
			<< angleV << "," \
			<< channel << "," \
			<< timestamp << std::endl;
	}
	fout.close();
	return ;
}