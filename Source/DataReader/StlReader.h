#pragma once
#include "stdafx.h"
#include <vector>
#include "CDataList.h"

#ifdef DATAREADER_EXPORTS
#define STLREADER_API __declspec(dllexport)
#else
#define STLREADER_API __declspec(dllimport)
#endif

using namespace std;
using namespace DataDefinition;

namespace DataReader
{
	typedef STLREADER_API struct _Point3f
	{
		float x;
		float y;
		float z;
	}Point3f;

	typedef vector<Point3f> Point3fList;

	class STLREADER_API STLReader
	{
	public:
		STLReader();
		~STLReader();

	public:
		bool ReadFile(const char *cfilename, CDataList& datalist);			//读取STL文件中的点到CDataList中,包括读取后的结构转换
		int NumTri();																//返回

	private:
		bool ReadASCII(const char *buffer, vector<Point3f>& pointList);				//用ascii的方式读取文件
		bool ReadBinary(const char *buffer, vector<Point3f>& pointList);			//用二进制的方式读取文件
		void Point3fList2CDataList(Point3fList& pointList, CDataList& datalist);	//将Point3fList里面的格式转换成CDataList格式

		Point3fList m_PointList;													//数据中三角片顶点的list
		unsigned int m_unTriangleNums;												//数据中三角形的个数

		char* memwriter;															//一个指针
		int cpyint(const char*& p);													//将4个字节的char读成一个int
		float cpyfloat(const char*& p);												//将4个字节的char读成一个float
	};
}
