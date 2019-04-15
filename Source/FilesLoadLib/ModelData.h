//*****************************************************************
// 文件名 :						ModelData.h
// 版本	 :						1.0
// 目的及主要功能 :				网格模型数据结构
// 创建日期 :					
// 修改日期 :					
// 作者 :						唐慧
// 修改者 :						
// 联系方式 :					王征,杨冠羽
//*****************************************************************/
#pragma once
/*****************************************************************
Library Files Included
*****************************************************************/
#include "FilesLoadLib.h"
#include "..\SceneGraph\Vector.h"
#ifdef FILESLOADLIB_EXPORTS
#define CMODELDATA_API __declspec(dllexport)
#else
#define CMODELDATA_API __declspec(dllimport)
#endif

//整型三维向量VECTOR9ui
struct CMODELDATA_API VECTOR9ui
{
	unsigned int x, y, z; //三角形index
	unsigned int nx, ny, nz; //法向量index
	unsigned int uvx, uvy, uvz; //纹理坐标index
};

//浮点型二维向量
struct CMODELDATA_API VECTOR2f
{
	float x, y;
};

class CMODELDATA_API CModelData
{
/*****************************************************************
						Routine Definitions
*****************************************************************/
public:
	CModelData(void);
	virtual ~CModelData(void);

	/*获取数据队列指针*/
	VECTOR * GetPVertex();
	VECTOR * GetPNormal();
	VECTOR9ui * GetPTriangles();
	VECTOR2f * GetPUVCoordinates();

	/*获取基本单元数量*/
	long GetVertexNum();
	long GetNormalNum();
	long GetTriNum();
	long GetUVNum();
	int  GetMatIndex();

	/*设置/修改基本单元数量*/
	void SetVertexNum(long VerNum);
	void SetNormalNum(long NormalNum);
	void SetTriNum(long TriNum);
	void SetUVNum(long TexNum);
	void SetMatIndex(int MatIndex);

	void SetModelName(char *strName);
	void GetModelName(char *strName);

	/*计算基本单元特征*/
	void CalculateNormal();		//计算三角形队列向量	

	void CopyModel(CModelData OriModel);

protected:
	void ClearCurModel();
/*****************************************************************
							Variables
*****************************************************************/
protected:
	char m_strName[255];		// 当前模型名字
	VECTOR * m_pVertex;		//顶点队列
	VECTOR * m_pNormal;		//向量队列
	VECTOR9ui * m_pTriangles;	//三角形队列
	VECTOR2f * m_pUVCoordinates; //纹理坐标队列

	long m_iVertexNum;		//顶点数量
	long m_iNormalNum; //法向量数量
	long m_iTriNum;			//三角型数量
	long m_iUVNum; //纹理坐标数目
	int  m_iMatIndex; //材质标号索引

};
