#include "stdafx.h"
#include "csvReader.h"
//#include <iostream>
#include <fstream>
#include <sstream>

using namespace DataReader;

/**************************************************************************
*  @Function Name :: CSVReader
*  @Description   :: CSVReader��Ĺ��캯��
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
*  @Description   :: CSVReader�����������
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
*  @Description   :: ��ʼ����һ֡�Ļص�����
*  @Author        :: zzw
*  @Parameters    :: Callback:��һ֡�Ļص�����
*					 pParam:�ص������еĲ���
*  @Return        :: None
**************************************************************************/
void CSVReader::Init(void(*Callback)(std::shared_ptr<RadarPointList> &lParam, LPVOID pParam), LPVOID pParam)
{
	TransferOneFrame = Callback;
	RTAdress = pParam;
}

/**************************************************************************
*  @Function Name :: Destory
*  @Description   :: �ÿճ�ʼ������
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
*  @Description   :: �򿪲���ʾcsv�ļ�
*  @Author        :: zzw
*  @Parameters    :: csvpath:csv�ļ���·��
*					 
*  @Return        :: None
**************************************************************************/
void CSVReader::CSVOpen(const char *csvpath)
{
	std::fstream fin;
	char tmp[1024] = { 0 };
	fin.open(csvpath, std::ios::in);
	fin.getline(tmp, sizeof(tmp));							//��һ���ڶ�ȡ��ʱ�򲢲���Ҫ
	if (nullptr != pRadarPointList)							//ָ֡�벻Ϊ�գ��˳�
	{
		return ;
	}
	pRadarPointList = std::make_shared<RadarPointList>();
	if (nullptr == pRadarPointList)							//�����ڴ�ʧ�ܣ��˳�
	{
		return ;
	}

	if ((NULL == TransferOneFrame) || (NULL == RTAdress))	//û�г�ʼ���ص��������˳�
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

	if ((NULL != TransferOneFrame) && (NULL != RTAdress))	//ֻ�г�ʼ���˲ŵ���
	{
		TransferOneFrame(pRadarPointList, RTAdress);
		pRadarPointList = nullptr;
	}
	return;
}

/**************************************************************************
*  @Function Name :: CSVWrite
*  @Description   :: ��������csv�ļ�
*  @Author        :: zzw
*  @Parameters    :: csv_savepath:csv�ļ�����·��
*					 PointsList:���洢�ĵ�֡
*  @Return        :: None
**************************************************************************/
void CSVReader::CSVWrite(const char *csv_savepath, RadarPointList& PointsList)
{
	std::fstream fout;
	char tmp[1024] = { 0 };
	fout.open(csv_savepath, std::ios::out);					//��д�ļ���

	fout << "Point_id,Distance,X,Y,Z,Intensity,Horizontal_angle,Vertical_angle,Channel,Timestamp" << std::endl;

	int PointNum = PointsList.size();
	char Point_ID[20];
	char distance[20];
	char angleH[20];
	char angleV[20];
	char channel[20];
	char timestamp[20];
	for (int i = 0; i < PointNum; i++)						//���������е㲢����
	{
		_itoa_s(PointsList[i].Point_ID, Point_ID,10);		//10����ʮ����
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