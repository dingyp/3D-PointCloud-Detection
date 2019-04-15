#if _MSC_VER > 1000
#endif

#pragma once
#include "stdafx.h"

struct tVector2 {
	float x, y;
};
struct tVector3 {
	float x, y, z;
};

struct tFace
{
	unsigned long a,b,c;//vertIndex[3];			// indicies for the verts that make up this triangle
	unsigned long na, nb, nc;
	unsigned long uva, uvb, uvc;
	short matIndex;		//The texture ID to use, which is the index into our texture array, for 3ds
};

struct tMaterialInfo
{
	char  strName[255];			// The texture name
	char  strFile[1024];			// The texture file name (If this is set it's a texture map)
	float ka[3];
	float kd[3];
	float ks[3];
	float ke[3];
	float d;
	float Ns;
	float illum;
	//CTexture cTxt;
} ;

struct t3DObject 
{
	int  numOfVerts;			// The number of verts in the model ģ���е�verts�� 
	int  numOfFaces;			// The number of faces in the model ģ���е�faces��
	int  numTexVertex;			// The number of texture coordinates ���������� 
	int  numNormal;				// The number of normals ������ 
	char strName[255];			// The name of the object ���������
	tVector3  *pVerts;			// The object's vertices ����Ķ��� 
	tVector3  *pNormals;		// The object's normals ����ķ��� 
	tVector2  *pTexVerts;		// The texture's UV coordinates �����UV���� 
	tFace *pFaces;				// The faces information of the object
	tVector3 tCenter; //���ĵ�
	float xLen,yLen,zLen;//��������ĳ��ȵ�һ��
	char strTexName[255];
	//  �������б�
	//	list<t3DObject* >lstObj;
};

// This holds our model information.  This should also turn into a robust class.
// We use STL's (Standard Template Library) vector class to ease our link list burdens. :)
struct t3DModel 
{
	vector<tMaterialInfo> vMaterials;	// The list of material information (Textures and colors)
	vector<t3DObject> vObject;			// The object list for our model
	char mtlName[256]; //mtl File name
	char mtlPath[1024];//mtl file path
};

