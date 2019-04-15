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
		bool ReadFile(const char *cfilename, CDataList& datalist);			//��ȡSTL�ļ��еĵ㵽CDataList��,������ȡ��Ľṹת��
		int NumTri();																//����

	private:
		bool ReadASCII(const char *buffer, vector<Point3f>& pointList);				//��ascii�ķ�ʽ��ȡ�ļ�
		bool ReadBinary(const char *buffer, vector<Point3f>& pointList);			//�ö����Ƶķ�ʽ��ȡ�ļ�
		void Point3fList2CDataList(Point3fList& pointList, CDataList& datalist);	//��Point3fList����ĸ�ʽת����CDataList��ʽ

		Point3fList m_PointList;													//����������Ƭ�����list
		unsigned int m_unTriangleNums;												//�����������εĸ���

		char* memwriter;															//һ��ָ��
		int cpyint(const char*& p);													//��4���ֽڵ�char����һ��int
		float cpyfloat(const char*& p);												//��4���ֽڵ�char����һ��float
	};
}
