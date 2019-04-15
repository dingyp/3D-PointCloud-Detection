#include "stdafx.h"
#include "binReader.h"
#include <fstream>
#include <sstream>

#define pi 3.1415926;
using namespace DataReader;

BINReader::BINReader()
{
	TransferOneFrame = NULL;
	RTAdress = NULL;
	pRadarPointList = nullptr;
}

BINReader::~BINReader()
{
	TransferOneFrame = NULL;
	RTAdress = NULL;
	pRadarPointList = nullptr;
}

void BINReader::Init(void(*Callback)(std::shared_ptr<RadarPointList> &lParam, \
	LPVOID pParam), LPVOID pParam)
{
	TransferOneFrame = Callback;
	RTAdress = pParam;
}

void BINReader::Destroy()
{
	TransferOneFrame = NULL;
	RTAdress = NULL;
}

void BINReader::BINOpen(const char *binpath)
{
	std::ifstream fin;
	fin.open(binpath, std::ios::in | std::ios::binary);
	fin.seekg(0, std::ios::beg);
	if (pRadarPointList != NULL)
	{
		return;
	}
	pRadarPointList = std::make_shared<RadarPointList>();
	if (nullptr == pRadarPointList)							//申请内存失败，退出
	{
		return;
	}
	if ((NULL == TransferOneFrame) || (NULL == RTAdress))	//没有初始化回调函数，退出
	{
		return;
	}
	RadarPoint curPoint;
	int pointID = 0;
	float x, y, z, intensity;
	while (fin.good() && !fin.eof())
	{

		fin.read((char*)&x, sizeof(float));
		fin.read((char*)&y, sizeof(float));
		fin.read((char*)&z, sizeof(float));
		fin.read((char*)&intensity, sizeof(float));
		curPoint.Point_ID = pointID++;
		curPoint.x = static_cast<double>(x);		//x
		curPoint.y = static_cast<double>(y);		//y
		curPoint.z = static_cast<double>(z);		//z
		curPoint.intensity = static_cast<double>(intensity);		//intensity
		curPoint.distance = sqrt(pow(curPoint.x, 2) + pow(curPoint.y, 2) + pow(curPoint.z, 2));		//distance
		curPoint.angleH = Rad2Deg(atan(curPoint.y / curPoint.x));		//angleH
		curPoint.angleV = Rad2Deg(atan2(sqrt(pow(curPoint.x, 2) + pow(curPoint.y, 2)), curPoint.z));		//angleV

		pRadarPointList->push_back(curPoint);
	}

	if ((NULL != TransferOneFrame) && (NULL != RTAdress))	//只有初始化了才调用
	{
		TransferOneFrame(pRadarPointList, RTAdress);
		pRadarPointList = nullptr;
	}

	return;
}

void BINReader::BINWrite(const char *csv_savepath, RadarPointList& PointsList)
{
	std::ofstream fout;
	fout.open(csv_savepath, std::ios::out | std::ios::binary);
	int pointNum = PointsList.size();
	for (int i = 0; i < pointNum; i++)
	{
		float x = PointsList[i].x;
		float y = PointsList[i].y;
		float z = PointsList[i].z;
		float intensity = (float)PointsList[i].intensity;
		fout.write((char*)(&x), sizeof(float));
		fout.write((char*)(&y), sizeof(float));
		fout.write((char*)(&z), sizeof(float));
		fout.write((char*)(&intensity), sizeof(float));
	}
	fout.close();
	return;
}