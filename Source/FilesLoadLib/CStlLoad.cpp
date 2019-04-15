#include "stdafx.h"
#include "CStlLoad.h"
#include <sstream> 
#include <fstream>
#include <iostream>
#include <string>

CStlLoad::CStlLoad()
{
}


CStlLoad::~CStlLoad()
{
}

int CStlLoad::LoadSTLFile(CString filename, CModelList &modellist)
{

	return 1;
}

bool CStlLoad::ReadFile(const char *cfilename, vector<VECTOR>& pointList)
{
	FILE * pFile;
	long lSize;
	char* buffer;
	size_t result;

	/* 若要一个byte不漏地读入整个文件，只能采用二进制方式打开 */
	fopen_s(&pFile, cfilename, "rb");
	if (pFile == NULL)
	{
		fputs("File error", stderr);
		exit(1);
	}

	/* 获取文件大小 */
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	/* 分配内存存储整个文件 */
	buffer = (char*)malloc(sizeof(char)*lSize);
	if (buffer == NULL)
	{
		fputs("Memory error", stderr);
		exit(2);
	}

	/* 将文件拷贝到buffer中 */
	result = fread(buffer, 1, lSize, pFile);
	if (result != lSize)
	{
		fputs("Reading error", stderr);
		exit(3);
	}


	/* 结束演示，关闭文件并释放内存 */
	fclose(pFile);

	bool bRet = false;

	ios::sync_with_stdio(false);
	bRet = ReadBinary(buffer, pointList);
	ios::sync_with_stdio(true);

	free(buffer);
	return bRet;
}

bool CStlLoad::ReadASCII(const char *buffer, vector<VECTOR>& pointList)
{
	unTriangles = 0;
	bool bRet = true;
	int i;
	VECTOR tempPt;
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
		unTriangles++;
		getline(ss, useless);
		getline(ss, useless);
		getline(ss, useless);
	} while (1);
	return bRet;
}

bool CStlLoad::ReadBinary(const char *buffer, vector<VECTOR>& pointList)
{
	const char* p = buffer;
	VECTOR tempPt;

	char name[80];
	int i, j;
	memcpy(name, p, 80);
	p += 80;
	unTriangles = cpyint(p);
	for (i = 0; i < unTriangles; i++)
	{
		p += 12;//跳过头部法向量
		for (j = 0; j < 3; j++)//读取三顶点
		{
			tempPt.x = cpyfloat(p);
			tempPt.y = cpyfloat(p);
			tempPt.z = cpyfloat(p);

			pointList.push_back(tempPt);
		}
		p += 2;//跳过尾部标志
	}
	if (unTriangles > 1)
		return true;
	else
		return false;
}

int CStlLoad::cpyint(const char*& p)
{
	int cpy;
	memwriter = (char*)&cpy;
	memcpy(memwriter, p, 4);
	p += 4;
	return cpy;
}
float CStlLoad::cpyfloat(const char*& p)
{
	float cpy;
	memwriter = (char*)&cpy;
	memcpy(memwriter, p, 4);
	p += 4;
	return cpy;
}