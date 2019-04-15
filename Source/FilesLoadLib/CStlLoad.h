#pragma once
#include "stdafx.h"
#include <vector>
#include "ModelList.h"

using namespace std;

class FILESLOADLIB_API CStlLoad
{
public:
	CStlLoad();
	virtual ~CStlLoad();

	//返回值1--成功,2--文件不存在, 3--无法识别的数据格式
	int LoadSTLFile(CString filename, CModelList &modellist);

private:
	unsigned int unTriangles;
	bool ReadFile(const char *cfilename, vector<VECTOR>& pointList);
	bool ReadASCII(const char *cfilename, vector<VECTOR>& pointList);
	bool ReadBinary(const char *cfilename, vector<VECTOR>& pointList);

	char* memwriter;
	int   cpyint(const char*& p);
	float cpyfloat(const char*& p);

};

