#include "stdafx.h"
#include "StlReader.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream> 

using namespace DataReader;

STLReader::STLReader()
{
}

STLReader::~STLReader()
{
	m_unTriangleNums = 0;
	if (m_PointList.empty() == false)
	{
		m_PointList.clear();
	}
}

//��ȡSTL�ļ��еĵ㵽Point3f��
bool STLReader::ReadFile(const char *cfilename, CDataList& datalist)
{
	FILE * pFile;
	long lSize;
	char* buffer;
	size_t result;

	/* ��Ҫһ��byte��©�ض��������ļ���ֻ�ܲ��ö����Ʒ�ʽ�� */
	fopen_s(&pFile, cfilename, "rb");
	if (pFile == NULL)
	{
		fputs("File error", stderr);
		exit(1);
	}

	/* ��ȡ�ļ���С */
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	/* �����ڴ�洢�����ļ� */
	buffer = (char*)malloc(sizeof(char)*lSize);
	if (buffer == NULL)
	{
		fputs("Memory error", stderr);
		exit(2);
	}

	/* ���ļ�������buffer�� */
	result = fread(buffer, 1, lSize, pFile);
	if (result != lSize)
	{
		fputs("Reading error", stderr);
		exit(3);
	}


	/* ������ʾ���ر��ļ����ͷ��ڴ� */
	fclose(pFile);

	bool bRet = false;

	ios::sync_with_stdio(false);
	bRet = ReadBinary(buffer, m_PointList);
	ios::sync_with_stdio(true);
	free(buffer);

	//����Point3fListת��ʽ��CDataList
	Point3fList2CDataList(m_PointList, datalist);
	return bRet;
}

//��Point3fList����ĸ�ʽת����CDataList��ʽ
void STLReader::Point3fList2CDataList(Point3fList& pointList, CDataList& datalist)
{
	if (datalist.m_pVertex != NULL)
	{
		delete[]datalist.m_pVertex;
		datalist.m_pVertex = NULL;
	}
		
	if (datalist.m_pNormal != NULL)
	{
		delete[] datalist.m_pNormal;
		datalist.m_pNormal = NULL;
	}
	
	datalist.m_iTriNum = pointList.size()/3;//����Ƭ����Ŀ
	datalist.m_iVertexNum = pointList.size();

	datalist.m_pVertex = new VECTOR[datalist.m_iVertexNum];
	datalist.m_pNormal = new VECTOR[datalist.m_iVertexNum];

	for (int i = 0; i < datalist.m_iTriNum; i++)
	{
		datalist.m_pVertex[i * 3].x = pointList[i * 3].x;
		datalist.m_pVertex[i * 3].y = pointList[i * 3].y;
		datalist.m_pVertex[i * 3].z = pointList[i * 3].z;

		datalist.m_pVertex[i * 3 + 1].x = pointList[i * 3 + 1].x;
		datalist.m_pVertex[i * 3 + 1].y = pointList[i * 3 + 1].y;
		datalist.m_pVertex[i * 3 + 1].z = pointList[i * 3 + 1].z;

		datalist.m_pVertex[i * 3 + 2].x = pointList[i * 3 + 2].x;
		datalist.m_pVertex[i * 3 + 2].y = pointList[i * 3 + 2].y;
		datalist.m_pVertex[i * 3 + 2].z = pointList[i * 3 + 2].z;

		//���㷨����
		VECTOR A, B, C, AB, BC, normal;
		A = datalist.m_pVertex[i * 3];
		B = datalist.m_pVertex[i * 3 + 1];
		C = datalist.m_pVertex[i * 3 + 2];

		AB = A - B;
		BC = B - C;
		normal = AB * BC;
		VectorNormalize(&normal);

		datalist.m_pNormal[i * 3] = datalist.m_pNormal[i * 3 + 1] = datalist.m_pNormal[i * 3 + 2] = normal;
	}
}

bool STLReader::ReadASCII(const char *buffer, vector<Point3f>& pointList)
{
	m_unTriangleNums = 0;
	bool bRet = true;
	int i;
	Point3f tempPt;
	string name, useless;
	stringstream ss(buffer);
	ss >> name >> name;
	ss.get();
	do {
		ss >> useless;
		if (useless != "facet")
		{
			bRet = false;
			break;

		}
		getline(ss, useless);
		getline(ss, useless);
		for (i = 0; i < 3; i++)
		{
			ss >> useless >> tempPt.x >> tempPt.y >> tempPt.z;
			pointList.push_back(tempPt);
		}
		m_unTriangleNums++;
		getline(ss, useless);
		getline(ss, useless);
		getline(ss, useless);
	} while (1);
	return bRet;
}

bool STLReader::ReadBinary(const char *buffer, vector<Point3f>& pointList)
{
	const char* p = buffer;
	Point3f tempPt;

	char name[80];
	int i, j;
	memcpy(name, p, 80);
	p += 80;
	m_unTriangleNums = cpyint(p);
	for (i = 0; i < m_unTriangleNums; i++)
	{
		p += 12;//����ͷ��������
		for (j = 0; j < 3; j++)//��ȡ������
		{
			tempPt.x = cpyfloat(p) / 1000.0;
			tempPt.y = cpyfloat(p) / 1000.0;
			tempPt.z = cpyfloat(p) / 1000.0 - 1.08;

			pointList.push_back(tempPt);
		}
		p += 2;//����β����־
	}
	if (m_unTriangleNums > 1)
		return true;
	else
		return false;
}

int STLReader::cpyint(const char*& p)
{
	int cpy;
	memwriter = (char*)&cpy;
	memcpy(memwriter, p, 4);
	p += 4;
	return cpy;
}

float STLReader::cpyfloat(const char*& p)
{
	float cpy;
	memwriter = (char*)&cpy;
	memcpy(memwriter, p, 4);
	p += 4;
	return cpy;
}

int STLReader::NumTri()
{
	return m_unTriangleNums;
}
