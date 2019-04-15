// obj.h: interface for the COBJ class.
//
//////////////////////////////////////////////////////////////////////


#if _MSC_VER > 1000
#endif
#pragma once
#include "stdafx.h"
#include <vector>
#include <list>
#include "ModelList.h"
#include <string>
#include "MyStruct.h"
using namespace std;

class FILESLOADLIB_API COBJ  
{

	std::vector<tVector3>  m_out_vertices;
	std::vector<tVector2>  m_out_uvs;
	std::vector<tVector3>  m_out_normals;

	t3DModel m_Model;

public:
	//返回值0---mtl文件不存在,但是obj读入成功,1--成功,2--文件不存在, 3---当前模型为四边形模型, 4--无法识别的数据格式
	int LoadObjFile(CString filename,CModelList &modellist); 
	bool SaveObjFile(CString filename,CString mtlfilename, CModelList &modellist);
	COBJ();
	~COBJ();

protected:
	int LoadOBJ(char *p_filename);
	bool LoadMtl(char *p_filename, char *p_mtl);
	void ReadString(FILE *file,char *str, char delim);
	bool SaveMtl();
	bool SaveOBJ(char *p_filename);

};

