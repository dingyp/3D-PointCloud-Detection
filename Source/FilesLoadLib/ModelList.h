#pragma once
#ifdef FILESLOADLIB_EXPORTS
#define CMODELLIST_API __declspec(dllexport)
#else
#define CMODELLIST_API __declspec(dllimport)
#endif

#include <vector>
#include "ModelData.h"

using namespace std;

struct CMODELLIST_API sample_MATERIAL
{
	char strMatName[256];
	GLfloat ambient[3];
	GLfloat diffuse[3];
	GLfloat specular[3];
	GLfloat emission[3];
	GLfloat alpha;
	GLfloat phExp;
	int texture;
};

struct CMODELLIST_API sample_TEXTURE
{
	CString TexFileName;
	GLint id;
};

class CMODELLIST_API CModelList
{
public:
	CModelList(void);
	virtual ~CModelList(void);
	void GetBoundBox(float& xMin, float& xMax, float& yMin, float& yMax, float& zMin, float& zMax);//得到包围盒

	int GetModelNum();									//得到模型的总数
	int GetColorsNum();									//得到颜色的总数
	int GetTexNum();									//得到纹理的总数

	void AddModelData(CModelData *pModelData); //这里的指针必须是在外面通过new得到的, delete由程序自动完成
	void InsertModelData(int i, CModelData *pModelData); //这里的指针必须是在外面通过new得到的, delete由程序自动完成

	void AddMaterial(sample_MATERIAL Material);
	void InsertMaterial(int i, sample_MATERIAL Material);
	BOOL GetMaterial(int i, sample_MATERIAL &temp);

	void AddTexture(sample_TEXTURE Texture);
	void InsertTexture(int i, sample_TEXTURE Texture);
	BOOL GetTexture(int i, sample_TEXTURE &temp);

	void ClearAllModels();

	int GetTotalSurface();							//得到所有模型三角面片的总数
	int GetTotalVertex();							//得到所有模型顶点总数

public:
	vector <CModelData *> m_vpModels;	// The list of models  模型列表 
	vector <sample_MATERIAL> m_vMaterials;	// The list of material information (Textures and colors) 材料信息列表（纹理和颜色） 
	vector <sample_TEXTURE> m_vTextureMaps;
};
