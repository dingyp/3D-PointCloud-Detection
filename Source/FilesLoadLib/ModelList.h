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
	void GetBoundBox(float& xMin, float& xMax, float& yMin, float& yMax, float& zMin, float& zMax);//�õ���Χ��

	int GetModelNum();									//�õ�ģ�͵�����
	int GetColorsNum();									//�õ���ɫ������
	int GetTexNum();									//�õ����������

	void AddModelData(CModelData *pModelData); //�����ָ�������������ͨ��new�õ���, delete�ɳ����Զ����
	void InsertModelData(int i, CModelData *pModelData); //�����ָ�������������ͨ��new�õ���, delete�ɳ����Զ����

	void AddMaterial(sample_MATERIAL Material);
	void InsertMaterial(int i, sample_MATERIAL Material);
	BOOL GetMaterial(int i, sample_MATERIAL &temp);

	void AddTexture(sample_TEXTURE Texture);
	void InsertTexture(int i, sample_TEXTURE Texture);
	BOOL GetTexture(int i, sample_TEXTURE &temp);

	void ClearAllModels();

	int GetTotalSurface();							//�õ�����ģ��������Ƭ������
	int GetTotalVertex();							//�õ�����ģ�Ͷ�������

public:
	vector <CModelData *> m_vpModels;	// The list of models  ģ���б� 
	vector <sample_MATERIAL> m_vMaterials;	// The list of material information (Textures and colors) ������Ϣ�б��������ɫ�� 
	vector <sample_TEXTURE> m_vTextureMaps;
};
