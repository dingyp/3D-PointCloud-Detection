// obj.cpp: implementation of the CObj class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "obj.h"
#include <stdio.h>
#include <io.h>
#include <fstream>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//#define INT_MAX 2147483647 
//#define INT_MIN 2147483647-1 
#define  FLT_MAX 3.402823466e+38F 
#define  FLT_MIN 1.175494351e-38F 
COBJ::COBJ()
{
	//	m_Model.numOfMaterials = 0;
	//	m_Model.numOfObjects = 0;
	m_Model.mtlPath[0] = '\0';
	m_Model.mtlName[0] = '\0';

}

COBJ::~COBJ()
{
	for(unsigned int i = 0;i<m_Model.vObject.size();i++)
	{
		if(m_Model.vObject[i].pVerts)
			delete []m_Model.vObject[i].pVerts;
		if(m_Model.vObject[i].pTexVerts)
			delete []m_Model.vObject[i].pTexVerts;
		if(m_Model.vObject[i].pFaces)
			delete []m_Model.vObject[i].pFaces;
		if(m_Model.vObject[i].pNormals)
			delete []m_Model.vObject[i].pNormals;
	}
	m_Model.vObject.clear();
	m_Model.vMaterials.clear();
}

//返回值0---mtl文件不存在,但是obj读入成功,1--成功,2--文件不存在, 3---当前模型为四边形模型, 4--无法识别的数据格式
int COBJ::LoadOBJ (char *p_filename)
{
	t3DModel *pModel = &m_Model;
	t3DObject *pObj = NULL;
	bool ObjCreat = false;

	std::vector<tFace> temp_Face;
	std::vector<tVector3> temp_vertices;
	std::vector<tVector2> temp_uvs;
	std::vector<tVector3> temp_normals;
	unsigned long temp1, temp2, temp3;


	FILE * file = fopen(p_filename, "r");
	if( file == NULL ){
		return 2;
	}

	bool bRetMTL = false;
	unsigned int i;

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if ( strcmp( lineHeader,"mtllib") == 0)
		{
			char mtlname[255];
			fscanf(file,"%s",mtlname);
			bRetMTL= LoadMtl(p_filename, mtlname);
		}

		if(bRetMTL==false) //只有对象,没有材质
		{
			if ( strcmp( lineHeader, "g") == 0 )
			{
				//save the information of previous object
				// For each vertex of each triangle
				if (ObjCreat)
				{	
					pObj = &m_Model.vObject.at(m_Model.vObject.size()-1);
					pObj->numOfFaces = temp_Face.size();
					if (pObj->numOfFaces)
						pObj->pFaces = new tFace[temp_Face.size()];
					else
						pObj->pFaces = NULL;

					pObj->numOfVerts = temp_vertices.size();
					if (pObj->numOfVerts)
						pObj->pVerts = new tVector3[temp_vertices.size()];
					else
						pObj->pVerts = NULL;

					pObj->numTexVertex = temp_uvs.size();
					if (pObj->numTexVertex)
						pObj->pTexVerts = new tVector2[temp_uvs.size()];
					else
						pObj->pTexVerts = NULL;

					pObj->numNormal = temp_normals.size();
					if (pObj->numNormal)
						pObj->pNormals = new tVector3[temp_normals.size()];
					else
						pObj->pNormals = NULL;

					for (i=0;i<temp_Face.size();i++)
						pObj->pFaces[i] = temp_Face.at(i);

					for (i=0;i<temp_vertices.size();i++)
						pObj->pVerts[i] = temp_vertices.at(i);

					for (i=0;i<temp_uvs.size();i++)
						pObj->pTexVerts[i] = temp_uvs.at(i);

					for (i=0;i<temp_normals.size();i++)
						pObj->pNormals[i] = temp_normals.at(i);

					temp_Face.clear();
					temp_vertices.clear();
					temp_uvs.clear();
					temp_normals.clear();			
					ObjCreat = false;

				}

				//read object name
				res = fscanf(file, "%s", lineHeader);
				if (res == EOF)
					break; // EOF = End Of File. Quit the loop.
				if(strcmp( lineHeader, "default")!=0 && strcmp(lineHeader, "#")!=0)
				{
					t3DObject obj;
					int i = 0;
					for (;lineHeader[i]!='\0';i++) 
					{
						obj.strName[i] = lineHeader[i];
					}
					obj.strName[i] = '\0';
					pModel->vObject.push_back(obj);
					ObjCreat = true;
				}

			}

			// else : parse lineHeader

			if ( strcmp( lineHeader, "v" ) == 0 )
			{
				cout<<"Get v"<<endl;
				tVector3 vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
				temp_vertices.push_back(vertex);
			}
			else if ( strcmp( lineHeader, "vt" ) == 0 )
			{
				cout<<"Get vt"<<endl;
				tVector2 uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y );
				uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
				temp_uvs.push_back(uv);
			}
			else if ( strcmp( lineHeader, "vn" ) == 0 )
			{
				cout<<"Get vn"<<endl;
				tVector3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
				temp_normals.push_back(normal);
			}
			else if ( strcmp( lineHeader, "f" ) == 0 )
			{
				cout<<"Get f"<<endl;
				//std::string vertex1, vertex2, vertex3;

				tFace face;
				face.a = face.b = face.c = 0;
				face.na = face.nb = face.nc = 0;
				face.uva = face.uvb = face.uvc = 0;
				long fp1 = ftell(file);
				int matches = fscanf(file, "%u/%u/%u %u/%u/%u %u/%u/%u %u/%u/%u\n", &face.a, &face.uva, &face.na, &face.b, &face.uvb, &face.nb, &face.c, &face.uvc, &face.nc, &temp1, &temp2, &temp3);
				if(matches==12)
				{
					fclose(file);

					temp_Face.clear();
					temp_vertices.clear();
					temp_uvs.clear();
					temp_normals.clear();
					return 3;

				}
				if (matches !=9)
				{
					long offset = fp1 - ftell(file);
					fseek(file,offset,SEEK_CUR);
					int matches = fscanf(file, "%u//%u %u//%u %u//%u\n", &face.a, &face.na, &face.b, &face.nb, &face.c, &face.nc);
					if (matches != 6)
					{
						fclose(file);

						temp_Face.clear();
						temp_vertices.clear();
						temp_uvs.clear();
						temp_normals.clear();
						return 4;
					}
				}
				if(face.a > 268435455) //不能为负数
				{
					fclose(file);

					temp_Face.clear();
					temp_vertices.clear();
					temp_uvs.clear();
					temp_normals.clear();
					return 4;

				}
				temp_Face.push_back(face);

			}
			else
			{
				// Probably a comment, eat up the rest of the line
				char stupidBuffer[1000];
				fgets(stupidBuffer, 1000, file);
			}

		}
		else
		{
			if ( strcmp( lineHeader,"usemtl") == 0 )
			{
				char matName[255];
				fscanf(file,"%s",matName);
				pObj = &m_Model.vObject.at(m_Model.vObject.size()-1);
				strcpy_s(pObj->strTexName, matName);
			}

			if ( strcmp( lineHeader, "g") == 0 )
			{
				//save the information of previous object
				// For each vertex of each triangle
				if (ObjCreat)
				{	
					pObj = &m_Model.vObject.at(m_Model.vObject.size()-1);
					pObj->numOfFaces = temp_Face.size();
					if (pObj->numOfFaces)
						pObj->pFaces = new tFace[temp_Face.size()];
					else
						pObj->pFaces = NULL;

					pObj->numOfVerts = temp_vertices.size();
					if (pObj->numOfVerts)
						pObj->pVerts = new tVector3[temp_vertices.size()];
					else
						pObj->pVerts = NULL;

					pObj->numTexVertex = temp_uvs.size();
					if (pObj->numTexVertex)
						pObj->pTexVerts = new tVector2[temp_uvs.size()];
					else
						pObj->pTexVerts = NULL;

					pObj->numNormal = temp_normals.size();
					if (pObj->numNormal)
						pObj->pNormals = new tVector3[temp_normals.size()];
					else
						pObj->pNormals = NULL;

					for (i=0;i<temp_Face.size();i++)
						pObj->pFaces[i] = temp_Face.at(i);

					for (i=0;i<temp_vertices.size();i++)
						pObj->pVerts[i] = temp_vertices.at(i);

					for (i=0;i<temp_uvs.size();i++)
						pObj->pTexVerts[i] = temp_uvs.at(i);

					for (i=0;i<temp_normals.size();i++)
						pObj->pNormals[i] = temp_normals.at(i);

					temp_Face.clear();
					temp_vertices.clear();
					temp_uvs.clear();
					temp_normals.clear();			
					ObjCreat = false;

				}

				//read object name
				res = fscanf(file, "%s", lineHeader);
				if (res == EOF)
					break; // EOF = End Of File. Quit the loop.
				if(strcmp( lineHeader, "default")!=0 && strcmp(lineHeader, "#")!=0)
				{
					t3DObject obj;
					int i = 0;
					for (;lineHeader[i]!='\0';i++) 
					{
						obj.strName[i] = lineHeader[i];
					}
					obj.strName[i] = '\0';
					pModel->vObject.push_back(obj);
					ObjCreat = true;
				}

			}

			// else : parse lineHeader

			if ( strcmp( lineHeader, "v" ) == 0 )
			{
				cout<<"Get v"<<endl;
				tVector3 vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
				temp_vertices.push_back(vertex);
			}
			else if ( strcmp( lineHeader, "vt" ) == 0 )
			{
				cout<<"Get vt"<<endl;
				tVector2 uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y );
				uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
				temp_uvs.push_back(uv);
			}
			else if ( strcmp( lineHeader, "vn" ) == 0 )
			{
				cout<<"Get vn"<<endl;
				tVector3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
				temp_normals.push_back(normal);
			}
			else if ( strcmp( lineHeader, "f" ) == 0 )
			{
				cout<<"Get f"<<endl;
				//std::string vertex1, vertex2, vertex3;

				tFace face;
				face.a = face.b = face.c = 0;
				face.na = face.nb = face.nc = 0;
				face.uva = face.uvb = face.uvc = 0;
				long fp1 = ftell(file);
				int matches = fscanf(file, "%u/%u/%u %u/%u/%u %u/%u/%u %u/%u/%u\n", &face.a, &face.uva, &face.na, &face.b, &face.uvb, &face.nb, &face.c, &face.uvc, &face.nc, &temp1, &temp2, &temp3);
				if(matches==12)
				{
					fclose(file);

					temp_Face.clear();
					temp_vertices.clear();
					temp_uvs.clear();
					temp_normals.clear();
					return 3;
				}
				if (matches !=9)
				{
					long offset = fp1 - ftell(file);
					fseek(file,offset,SEEK_CUR);
					int matches = fscanf(file, "%u//%u %u//%u %u//%u\n", &face.a, &face.na, &face.b, &face.nb, &face.c, &face.nc);
					if (matches != 6)
					{
						fclose(file);

						temp_Face.clear();
						temp_vertices.clear();
						temp_uvs.clear();
						temp_normals.clear();
						return 4;
					}
				}
				temp_Face.push_back(face);

			}
			else
			{
				// Probably a comment, eat up the rest of the line
				char stupidBuffer[1000];
				fgets(stupidBuffer, 1000, file);
			}

		}

	}

	if(ObjCreat)
	{
		pObj = &m_Model.vObject.at(m_Model.vObject.size()-1);
		pObj->numOfFaces = temp_Face.size();
		if (pObj->numOfFaces)
			pObj->pFaces = new tFace[temp_Face.size()];
		else
			pObj->pFaces = NULL;

		pObj->numOfVerts = temp_vertices.size();
		if (pObj->numOfVerts)
			pObj->pVerts = new tVector3[temp_vertices.size()];
		else
			pObj->pVerts = NULL;

		pObj->numTexVertex = temp_uvs.size();
		if (pObj->numTexVertex)
			pObj->pTexVerts = new tVector2[temp_uvs.size()];
		else
			pObj->pTexVerts = NULL;

		pObj->numNormal = temp_normals.size();
		if (pObj->numNormal)
			pObj->pNormals = new tVector3[temp_normals.size()];
		else
			pObj->pNormals = NULL;

		i=0;
		for (i=0;i<temp_Face.size();i++)
			pObj->pFaces[i] = temp_Face.at(i);

		for (i=0;i<temp_vertices.size();i++)
			pObj->pVerts[i] = temp_vertices.at(i);

		for (i=0;i<temp_uvs.size();i++)
			pObj->pTexVerts[i] = temp_uvs.at(i);

		for (i=0;i<temp_normals.size();i++)
			pObj->pNormals[i] = temp_normals.at(i);

		temp_Face.clear();
		temp_vertices.clear();
		temp_uvs.clear();
		temp_normals.clear();			

	}


	fclose(file);

	if(bRetMTL==true)
		return 1;
	else
		return 0;
}

bool COBJ::LoadMtl(char *p_filename, char *p_mtl)
{
	char mtl_path[1024];
	char* p_end = strrchr(p_filename, '\\');
	int pos=0;
	for (char* p  = p_filename; p<=p_end; p++, pos++)
		mtl_path[pos] = *p;
	for (int i=0; *(p_mtl+i) != '\0'; i++, pos++)
		mtl_path[pos] = *(p_mtl+i);
	mtl_path[pos] = '\0';

	FILE * file = fopen(mtl_path, "r");
	if( file == NULL ){
		return false;
	}

	while( 1 ){
		char lineHeader[128];
		// read the first word of the line
		long fp1 = ftell(file);
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.


		if ( strcmp( lineHeader,"newmtl") == 0)
		{
			char matName[255];
			fscanf(file, "%s", matName);
			tMaterialInfo matinfo;
			matinfo.ka[0] = matinfo.ka[1] = matinfo.ka[2] = 0;
			matinfo.kd[0] = matinfo.kd[1] = matinfo.kd[2] = 0;
			matinfo.ke[0] = matinfo.ke[1] = matinfo.ke[2] = 0;
			matinfo.ks[0] = matinfo.ks[1] = matinfo.ks[2] = 0;
			matinfo.Ns = 10.0;
			matinfo.strFile[0] = '\0';
			matinfo.strName[0] = '\0';
			strcpy_s(matinfo.strName, matName);
			m_Model.vMaterials.push_back(matinfo);
		}

		if ( strcmp( lineHeader,"Ka") == 0)
		{
			tMaterialInfo* pMat = &m_Model.vMaterials.at(m_Model.vMaterials.size()-1);
			int matches = fscanf(file,"%f %f %f", &pMat->ka[0], &pMat->ka[1], &pMat->ka[2]);
			if (matches < 3)
				pMat->ka[2] = pMat->ka[1] = pMat->ka[0];
		}

		if ( strcmp( lineHeader,"Kd") == 0)
		{
			tMaterialInfo* pMat = &m_Model.vMaterials.at(m_Model.vMaterials.size()-1);
			int matches = fscanf(file,"%f %f %f", &pMat->kd[0], &pMat->kd[1], &pMat->kd[2]);
			if (matches < 3)
				pMat->kd[2] = pMat->kd[1] = pMat->kd[0];
		}

		if ( strcmp( lineHeader,"Ks") == 0)
		{
			tMaterialInfo* pMat = &m_Model.vMaterials.at(m_Model.vMaterials.size()-1);
			int matches = fscanf(file,"%f %f %f", &pMat->ks[0], &pMat->ks[1], &pMat->ks[2]);
			if (matches < 3)
				pMat->ks[2] = pMat->ks[1] = pMat->ks[0];
		}

		if ( strcmp( lineHeader,"Ke") == 0)
		{
			tMaterialInfo* pMat = &m_Model.vMaterials.at(m_Model.vMaterials.size()-1);
			int matches = fscanf(file,"%f %f %f", &pMat->ke[0], &pMat->ke[1], &pMat->ke[2]);
			if (matches < 3)
				pMat->ke[2] = pMat->ke[1] = pMat->ke[0];
		}

		if ( strcmp( lineHeader,"Ns") == 0)
		{
			tMaterialInfo* pMat = &m_Model.vMaterials.at(m_Model.vMaterials.size()-1);
			fscanf(file,"%f", &pMat->Ns);
		}

		if ( strcmp( lineHeader,"d") == 0)
		{
			tMaterialInfo* pMat = &m_Model.vMaterials.at(m_Model.vMaterials.size()-1);
			fscanf(file,"%f", &pMat->d);
		}

		if ( strcmp( lineHeader,"illum") == 0)
		{
			tMaterialInfo* pMat = &m_Model.vMaterials.at(m_Model.vMaterials.size()-1);
			fscanf(file,"%f", &pMat->illum);
		}

		if ( strcmp( lineHeader,"map_Kd") == 0)
		{
			long offset = ftell(file) - fp1;
			//fseek(file,-offset,SEEK_CUR);

			tMaterialInfo* pMat = &m_Model.vMaterials.at(m_Model.vMaterials.size()-1);
			char fileName[1024];
			fileName[0] = '\0';
			ReadString(file,fileName,'\n');
			if (fileName[0])
			{
				int pos=0;
				for (char* p  = p_filename; p<=p_end; p++, pos++)
					pMat->strFile[pos] = *p;
				for (int i=0; *(fileName+i) != '\0'; i++, pos++)
					pMat->strFile[pos] = *(fileName+i);
				pMat->strFile[pos] = '\0';
			}
		}
	}
	fclose(file);

	int MtNum = m_Model.vMaterials.size();
	if(MtNum>0)
		return true;
	else
		return false;

}

void COBJ::ReadString(FILE *file,char *str, char delim)
{
	int i=0;char l_char;
	do{

		fread (&l_char, 1, 1, file);
	}while(l_char==' ');

	str[i] = l_char;
	i++;
	while(l_char != delim && i<255)
	{
		fread (&l_char, 1, 1, file);
		str[i]=l_char;
		i++;
	}
	str[i] = '\0';

}

//返回值0---mtl文件不存在,但是obj读入成功,1--成功,2--文件不存在, 3---当前模型为四边形模型, 4--无法识别的数据格式
int COBJ::LoadObjFile(CString filename,CModelList &modellist)
{
	USES_CONVERSION;
	int ret=LoadOBJ( W2A(filename.GetBuffer(0)));
	if(ret>1)
	{
		m_Model.vObject.clear();
		m_Model.vMaterials.clear();

		return ret;
	}

	short MatIndex=-1;
	CModelData *pCurModel;
	int sumVerts = 0;
	int sumNormals = 0;
	int sumUVs = 0;
	unsigned int i=0;
	int j=0;
	//模型数据
	for ( i = 0;i < m_Model.vObject.size(); i++ )
	{
		t3DObject Obj = m_Model.vObject.at(i);

		pCurModel = new CModelData;	
		pCurModel->SetModelName(Obj.strName);

		//顶点
		pCurModel->SetVertexNum(Obj.numOfVerts);
		VECTOR * pVertex = pCurModel->GetPVertex();
		ASSERT(pVertex!=NULL);
		for ( j = 0; j < Obj.numOfVerts; j++ )//精度是毫米
		{
			pVertex[j].x = Obj.pVerts[j].x / 1000.0;
			pVertex[j].y = Obj.pVerts[j].y / 1000.0;
			pVertex[j].z = Obj.pVerts[j].z / 1000.0 - 1.18;
		}

		//三角形
		pCurModel->SetTriNum(Obj.numOfFaces);
		VECTOR9ui * pTriangles = pCurModel->GetPTriangles();
		for (j = 0; j < Obj.numOfFaces; j++)
		{
			pTriangles[j].x = Obj.pFaces[j].a - sumVerts - 1;
			pTriangles[j].y = Obj.pFaces[j].b - sumVerts - 1;
			pTriangles[j].z = Obj.pFaces[j].c - sumVerts - 1;
			pTriangles[j].nx = Obj.pFaces[j].na - sumNormals - 1;
			pTriangles[j].ny = Obj.pFaces[j].nb - sumNormals - 1;
			pTriangles[j].nz = Obj.pFaces[j].nc - sumNormals - 1;
			pTriangles[j].uvx = Obj.pFaces[j].uva - sumUVs - 1;
			pTriangles[j].uvy = Obj.pFaces[j].uvb - sumUVs - 1;
			pTriangles[j].uvz = Obj.pFaces[j].uvc - sumUVs - 1;
		}

		//法向量
		if(Obj.numNormal>0)
		{
			pCurModel->SetNormalNum(Obj.numNormal);
			VECTOR * pNormal = pCurModel->GetPNormal();
			for(j=0; j<Obj.numNormal; j++)
			{
				pNormal[j].x = Obj.pNormals[j].x;
				pNormal[j].y = Obj.pNormals[j].y;
				pNormal[j].z = Obj.pNormals[j].z;	

			}

		}
		else //自己计算法向量
		{
			pCurModel->CalculateNormal();
			//法向量与三角形映射相同
			for(j=0; j<Obj.numOfFaces; j++)
			{
				pTriangles[j].nx = pTriangles[j].x;
				pTriangles[j].ny = pTriangles[j].y;
				pTriangles[j].nz = pTriangles[j].z;
			}
		}

		//纹理坐标
		if(Obj.numTexVertex>0)
		{
			pCurModel->SetUVNum(Obj.numTexVertex);
			VECTOR2f *pUVs = pCurModel->GetPUVCoordinates();
			for(j=0; j<Obj.numTexVertex; j++)
			{
				pUVs[j].x = Obj.pTexVerts[j].x;
				pUVs[j].y = Obj.pTexVerts[j].y;
			}
		}

		//对应mat
		for( j = 0; j < (int)m_Model.vMaterials.size(); j++ )
		{
			tMaterialInfo Mat = m_Model.vMaterials.at(j);
			if(strcmp(Obj.strTexName, Mat.strName)==0)
				MatIndex = j;
		}
		pCurModel->SetMatIndex(MatIndex);
		
		modellist.AddModelData(pCurModel);

		sumVerts += Obj.numOfVerts;
		sumNormals += Obj.numNormal;
		sumUVs += Obj.numTexVertex;

	}

	for(unsigned i=0; i<m_Model.vMaterials.size(); i++)
	{
		tMaterialInfo CurMat = m_Model.vMaterials.at(i);
		sample_MATERIAL CurSampleMat;

		strcpy_s(CurSampleMat.strMatName, CurMat.strName);

		for(j=0; j<3; j++)
		{
			CurSampleMat.ambient[j] = CurMat.ka[j];
			CurSampleMat.diffuse[j] = CurMat.kd[j];
			CurSampleMat.specular[j] = CurMat.ks[j];
			CurSampleMat.emission[j] = CurMat.ke[j];
		}
		CurSampleMat.alpha = CurMat.d;
		CurSampleMat.phExp = max(CurMat.Ns,CurMat.illum);
		CurSampleMat.texture = -1;

		int strcount = strlen(CurMat.strFile);
		if(CurMat.strFile[strcount-2]!='\\')
		{
			sample_TEXTURE CurTexture;
			CurTexture.TexFileName = CurMat.strFile;
			modellist.AddTexture(CurTexture);
			CurSampleMat.texture = modellist.GetTexNum()-1; //texture index
		}

		modellist.AddMaterial(CurSampleMat);
	}

	return ret;
}

bool COBJ::SaveObjFile(CString filename,CString mtlfilename, CModelList &modellist)
{

	//把ModelList转为Model
	unsigned int i=0;
	int j=0;
	//材质
	m_Model.vMaterials.clear();
	sample_MATERIAL CurSampleMat;
	sample_TEXTURE CurTexture;
	for(i=0; i<modellist.GetColorsNum(); i++)
	{
		tMaterialInfo CurMat;
		modellist.GetMaterial(i, CurSampleMat);

		strcpy_s(CurMat.strName, CurSampleMat.strMatName);

		for(j=0; j<3; j++)
		{
			CurMat.ka[j] = CurSampleMat.ambient[j];
			CurMat.kd[j] = CurSampleMat.diffuse[j];
			CurMat.ks[j] = CurSampleMat.specular[j];
			CurMat.ke[j] = CurSampleMat.emission[j];
		}
		CurMat.Ns = CurSampleMat.phExp;
		CurMat.illum = CurSampleMat.phExp;
		CurMat.d = CurSampleMat.alpha;

		if(CurSampleMat.texture>-1)
		{			
			modellist.GetTexture(CurSampleMat.texture, CurTexture);
			char temp ='\\';
			int distance = CurTexture.TexFileName.ReverseFind(temp);
			int templength = CurTexture.TexFileName.GetLength();
			distance = distance +1;
			for(j=distance; j<templength; j++)
			{
				CurMat.strFile[j-distance] = CurTexture.TexFileName[j];
			}
			CurMat.strFile[templength-distance] = '\0';
		}
		m_Model.vMaterials.push_back(CurMat);
	}


	//模型数据
	m_Model.vObject.clear();
	short MatIndex=-1;
	CModelData *pCurModel;
	int sumVerts = 0;
	int sumNormals = 0;
	int sumUVs = 0;
	for(i=0; i<modellist.GetModelNum(); i++)
	{
		t3DObject Obj;
		pCurModel = modellist.m_vpModels.at(i);

		//模型名字
		pCurModel->GetModelName(Obj.strName);

		//顶点
		Obj.numOfVerts = pCurModel->GetVertexNum();
		Obj.pVerts = new tVector3[Obj.numOfVerts];
		VECTOR * pVertex = pCurModel->GetPVertex();
		for(j=0; j<Obj.numOfVerts; j++)
		{
			Obj.pVerts[j].x = pVertex[j].x;
			Obj.pVerts[j].y = pVertex[j].y;
			Obj.pVerts[j].z = pVertex[j].z;
		}

		//三角形
		Obj.numOfFaces = pCurModel->GetTriNum();
		Obj.pFaces = new tFace[Obj.numOfFaces];
		VECTOR9ui * pTriangles = pCurModel->GetPTriangles();
		for(j=0; j<Obj.numOfFaces; j++)
		{
			Obj.pFaces[j].a = pTriangles[j].x + sumVerts +1;
			Obj.pFaces[j].b = pTriangles[j].y + sumVerts +1;
			Obj.pFaces[j].c = pTriangles[j].z + sumVerts +1;
			Obj.pFaces[j].na = pTriangles[j].nx + sumNormals +1;
			Obj.pFaces[j].nb = pTriangles[j].ny + sumNormals +1;
			Obj.pFaces[j].nc = pTriangles[j].nz + sumNormals +1;	
			Obj.pFaces[j].uva = pTriangles[j].uvx + sumUVs +1;
			Obj.pFaces[j].uvb = pTriangles[j].uvy + sumUVs +1;
			Obj.pFaces[j].uvc = pTriangles[j].uvz + sumUVs +1;	
		}

		//法向量
		Obj.numNormal = pCurModel->GetNormalNum();
		Obj.pNormals = new tVector3[Obj.numNormal];
		VECTOR * pNormal = pCurModel->GetPNormal();
		for(j=0; j<Obj.numNormal; j++)
		{
			Obj.pNormals[j].x = pNormal[j].x;
			Obj.pNormals[j].y = pNormal[j].y;
			Obj.pNormals[j].z = pNormal[j].z;

		}

		//纹理坐标
		if(pCurModel->GetUVNum()>0)
		{
			Obj.numTexVertex = pCurModel->GetUVNum();
			Obj.pTexVerts = new tVector2[Obj.numTexVertex];
			VECTOR2f *pUVs = pCurModel->GetPUVCoordinates();
			for(j=0; j<Obj.numTexVertex; j++)
			{
				Obj.pTexVerts[j].x = pUVs[j].x; 
				Obj.pTexVerts[j].y = -pUVs[j].y; 
			}
		}
		else
		{
			Obj.numTexVertex=0;
			Obj.pTexVerts = NULL;
		}

		//对应mat
		MatIndex = pCurModel->GetMatIndex();
		if(MatIndex>-1)
		{
			tMaterialInfo Mat = m_Model.vMaterials.at(MatIndex);
			strcpy_s(Obj.strTexName, Mat.strName);
		}
		else
			Obj.strTexName[0] = '\0';

		sumVerts += Obj.numOfVerts;
		sumNormals += Obj.numNormal;
		sumUVs += Obj.numTexVertex;

		m_Model.vObject.push_back(Obj);

	}

	USES_CONVERSION;

	//设置mtl文件名称和文件路径
	if(m_Model.vMaterials.size()>0)
	{
		strcpy_s(m_Model.mtlPath, W2A(mtlfilename.GetBuffer()));

		char temp ='\\';
		int distance = mtlfilename.ReverseFind(temp);
		int templength = mtlfilename.GetLength();
		distance = distance +1;
		for(j=distance; j<templength; j++)
		{
			m_Model.mtlName[j-distance] = mtlfilename[j];
		}
		m_Model.mtlName[templength-distance]='\0';

	}
	

	if( !SaveOBJ( W2A(filename.GetBuffer(0)) ) )
		return false;

	return true;
}



bool COBJ::SaveOBJ(char *p_filename)
{
	FILE * file = fopen(p_filename, "w");
	if( file == NULL ){
		return false;
	}

	if (strlen(m_Model.mtlPath) > 0)
	{
		fprintf(file,"mtllib %s\n", m_Model.mtlName);

		SaveMtl();
	}

	for (int model_idx =0; model_idx < m_Model.vObject.size(); model_idx++)
	{
		t3DObject *pObj = &m_Model.vObject.at(model_idx);
		fprintf(file, "#object %s\n", pObj->strName);
		fprintf(file,"g %s\n", pObj->strName);
		//write vertex
		for (int i=0;i<pObj->numOfVerts;i++)
			fprintf(file,"v %f %f %f\n", pObj->pVerts[i].x, pObj->pVerts[i].y, pObj->pVerts[i].z);
		fprintf(file, "# %d verticies\n", pObj->numOfVerts);
		//write texture vertex
		for (int i=0;i<pObj->numTexVertex;i++)
			fprintf(file,"vt %f %f\n", pObj->pTexVerts[i].x, pObj->pTexVerts[i].y);
		fprintf(file, "# %d texture verticies \n", pObj->numTexVertex);
		//write normals
		for (int i=0;i<pObj->numNormal;i++)
			fprintf(file,"vn %f %f %f\n", pObj->pNormals[i].x, pObj->pNormals[i].y, pObj->pNormals[i].z);
		fprintf(file, "# %d normals \n", pObj->numNormal);
		//write face
		if (strlen(pObj->strTexName)>0)
			fprintf(file, "usemtl %s\n", pObj->strTexName);
		for (int i=0;i<pObj->numOfFaces;i++)
		{
			if (pObj->numTexVertex > 0 && pObj->numNormal > 0)
				fprintf(file,"f %u/%u/%u %u/%u/%u %u/%u/%u\n", pObj->pFaces[i].a, pObj->pFaces[i].uva, pObj->pFaces[i].na, pObj->pFaces[i].b, pObj->pFaces[i].uvb, pObj->pFaces[i].nb, pObj->pFaces[i].c, pObj->pFaces[i].uvc, pObj->pFaces[i].nc);
			else if (pObj->numTexVertex > 0 && pObj->numNormal == 0)
				fprintf(file,"f %u/%u/ %u/%u/ %u/%u/\n", pObj->pFaces[i].a, pObj->pFaces[i].uva, pObj->pFaces[i].b, pObj->pFaces[i].uvb,pObj->pFaces[i].c, pObj->pFaces[i].uvc);
			else if (pObj->numTexVertex == 0 && pObj->numNormal > 0)
				fprintf(file,"f %u//%u %u//%u %u//%u\n", pObj->pFaces[i].a, pObj->pFaces[i].na, pObj->pFaces[i].b, pObj->pFaces[i].nb, pObj->pFaces[i].c, pObj->pFaces[i].nc);
			else if (pObj->numTexVertex == 0 && pObj->numNormal == 0)
				fprintf(file,"f %u// %u// %u//\n", pObj->pFaces[i].a, pObj->pFaces[i].b, pObj->pFaces[i].c);
		}
	}
	fclose(file);

	return true;
}

bool COBJ::SaveMtl()
{
	//newmtl pinktan
	//Ka 1 0.372549 0.313726
	//Kd 1 0.372549 0.313726
	//Ks 0.145098 0.145098 0.0941177
	//illum 2
	//Ns 238.856
	//map_Kd 
	//map_bump 
	//bump 
	//map_opacity 
	//map_d 
	//refl 
	//map_kS 
	//map_kA 
	//map_Ns

	FILE * file = fopen(m_Model.mtlPath, "w");
	if( file == NULL ){
		return false;
	}

	for (int mat_idx = 0; mat_idx<m_Model.vMaterials.size();mat_idx++)
	{
		tMaterialInfo *info = &m_Model.vMaterials.at(mat_idx);
		fprintf(file, "newmtl %s\n", info->strName);
		fprintf(file, "Ka %f %f %f\n", info->ka[0],info->ka[1],info->ka[2]);
		fprintf(file, "Kd %f %f %f\n", info->kd[0],info->kd[1],info->kd[2]);
		fprintf(file, "Ks %f %f %f\n", info->ks[0],info->ks[1],info->ks[2]);
		fprintf(file, "illum %f\n",info->illum);
		fprintf(file, "Ns %f\n", info->Ns );
		fprintf(file, "d %f\n", info->d );
		fprintf(file, "map_Kd %s\n", info->strFile);
		fprintf(file, "map_bump %s\n", info->strFile);
		fprintf(file, "bump %s\n", info->strFile);

	}
	fclose(file);
	return true;
}