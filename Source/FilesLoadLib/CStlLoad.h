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

	//����ֵ1--�ɹ�,2--�ļ�������, 3--�޷�ʶ������ݸ�ʽ
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

