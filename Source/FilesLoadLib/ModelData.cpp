//*****************************************************************
// 文件名 :						ModelData.cpp
// 版本	 :						1.0
// 目的及主要功能 :				网格模型数据结构
// 创建日期 :					
// 修改日期 :					
// 作者 :						唐慧
// 修改者 :						
// 联系方式 :					王征,杨冠羽
//*****************************************************************/
#include "stdafx.h"
#include "ModelData.h"

CModelData::CModelData(void)
{
	m_iVertexNum = m_iTriNum = 0;
	m_iNormalNum = 0;
	m_iUVNum = 0;
	m_pVertex = NULL;
	m_pTriangles = NULL;
	m_pNormal = NULL;
	m_pUVCoordinates = NULL;
	m_iMatIndex = -1;
	memset(m_strName, 0, 255*sizeof(char));
}

CModelData::~CModelData(void)
{
	ClearCurModel();
}


/*****************************************************************
Name:			GetPVertex
Inputs:			none
Return Value:	none
Description:	返回顶点队列指针
*****************************************************************/
VECTOR * CModelData::GetPVertex()
{
	return m_pVertex;	
}/*GetPVertex*/


/*****************************************************************
Name:			GetPNormal
Inputs:			none
Return Value:	none
Description:	返回法向量队列指针
*****************************************************************/
VECTOR * CModelData::GetPNormal()
{
	return m_pNormal;
}/*GetPNormal*/


/*****************************************************************
Name:			GetPTriangles
Inputs:			none
Return Value:	none
Description:	返回面片队列指针
*****************************************************************/
VECTOR9ui * CModelData::GetPTriangles()
{
	return m_pTriangles;
}/*GetPTriangles*/

/*****************************************************************
Name:			GetPUVCoordinates
Inputs:			none
Return Value:	none
Description:	返回三角形队列指针
*****************************************************************/
VECTOR2f * CModelData::GetPUVCoordinates()
{
	return m_pUVCoordinates;
}/*GetPUVCoordinates*/

/*****************************************************************
Name:			GetVertexNum
Inputs:			none
Return Value:	none
Description:	返回顶点数量
*****************************************************************/
long CModelData::GetVertexNum()
{
	return m_iVertexNum;
}/*GetVertexNum*/

long CModelData::GetNormalNum()
{
	return m_iNormalNum;
}

/*****************************************************************
Name:			GetTriNum
Inputs:			none
Return Value:	none
Description:	返回三角形数量
*****************************************************************/
long CModelData::GetTriNum()
{
	return m_iTriNum;
}/*GetTriNum*/


long CModelData::GetUVNum()
{
	return m_iUVNum;
}

int CModelData::GetMatIndex()
{
	return m_iMatIndex;
}
/*****************************************************************
Name:			CalculateNormal
Inputs:			none
Return Value:	none
Description:	计算法向量
*****************************************************************/
void CModelData::CalculateNormal()
{
	if(m_pNormal==NULL || m_iNormalNum == 0)
	{
		if(m_pNormal!=NULL)
			delete []m_pNormal;

		m_iNormalNum = m_iVertexNum;
		m_pNormal = new VECTOR[m_iNormalNum];
	}

	ZeroMemory(m_pNormal,m_iNormalNum*sizeof(VECTOR));

	int i;

	//********以下为法向量的计算过程************
	//****每个顶点的法向量为其所在面的法向量之和
	for(i=0; i<m_iTriNum; i++)
	{
		VECTOR AB,BC,A,B,C,normal;
		A=m_pVertex[m_pTriangles[i].x];
		B=m_pVertex[m_pTriangles[i].y];
		C=m_pVertex[m_pTriangles[i].z];
		AB=B-A;
		BC=C-B;
		normal=AB*BC;
		m_pNormal[m_pTriangles[i].x]=normal+m_pNormal[m_pTriangles[i].x];
		m_pNormal[m_pTriangles[i].y]=normal+m_pNormal[m_pTriangles[i].y];
		m_pNormal[m_pTriangles[i].z]=normal+m_pNormal[m_pTriangles[i].z];
	}
	//*法向量归一化
	for(i=0; i<m_iNormalNum; i++)
	{
		VectorNormalize(&m_pNormal[i]);
	}
}/*CalculateNormal*/


/*****************************************************************
Name:			SetVertexNum
Inputs:			long VerNum	-	新的顶点数量
Return Value:	none
Description:	修改顶点数量,重新分配
				顶点和向量队列的空间,
				如果已经有该队列,则注
				销后重新分配空间

注：	fiki修改此处内容，包括后面的几个函数，
		以在数值为0时，避免开辟空间，返回null
*****************************************************************/
void CModelData::SetVertexNum(long VerNum)
{
	m_iVertexNum = VerNum;

	if(m_pVertex!=NULL)
	{
		delete m_pVertex;
	}
	m_pVertex = NULL;

	if ( m_iVertexNum > 0 )
	{
		m_pVertex = new VECTOR[m_iVertexNum];
		memset(m_pVertex, 0, m_iVertexNum*sizeof(VECTOR));
	}

}/*SetVertexNum*/

void CModelData::SetNormalNum(long NormalNum)
{
	m_iNormalNum = NormalNum;

	if(m_pNormal!=NULL)
	{
		delete m_pNormal;
	}
	m_pNormal = NULL;

	if ( m_iNormalNum > 0 )
	{
		m_pNormal = new VECTOR[m_iNormalNum];
		memset(m_pNormal, 0, m_iNormalNum*sizeof(VECTOR));
	}

}

/*****************************************************************
Name:			SetTriNum
Inputs:			long TriNum	-	新的三角形数量
Return Value:	none
Description:	修改三角形数量,重新分配
				三角形队列的空间,如果已
				经有该队列,则注销后重新
				分配空间
*****************************************************************/
void CModelData::SetTriNum(long TriNum)
{
	m_iTriNum = TriNum;

	if(m_pTriangles!=NULL)
	{
		delete m_pTriangles;
	}
	m_pTriangles = NULL;

	if ( m_iTriNum > 0 )
	{
		m_pTriangles = new VECTOR9ui[m_iTriNum];
		memset(m_pTriangles, 0, m_iTriNum*sizeof(VECTOR9ui));
	}

}/*SetTriNum*/

void CModelData::SetUVNum(long TexNum)
{
	m_iUVNum = TexNum;

	if(m_pUVCoordinates!=NULL)
	{
		delete m_pUVCoordinates;
	}
	m_pUVCoordinates = NULL;

	if ( m_iUVNum > 0 )
	{
		m_pUVCoordinates = new VECTOR2f[m_iUVNum];
		memset(m_pUVCoordinates, 0, m_iUVNum*sizeof(VECTOR2f));
	}
}

void CModelData::SetMatIndex(int MatIndex)
{
	if(MatIndex>=0)
		m_iMatIndex = MatIndex;
}

void CModelData::ClearCurModel()
{
	if(m_pVertex!=NULL)
		delete []m_pVertex;
	if(m_pTriangles!=NULL)
		delete []m_pTriangles;
	if(m_pNormal!=NULL)
		delete []m_pNormal;
	if(m_pUVCoordinates!=NULL)
		delete []m_pUVCoordinates;
}

void CModelData::CopyModel(CModelData OriModel)
{
	ClearCurModel();

	m_iVertexNum = OriModel.GetVertexNum();
	m_iNormalNum = OriModel.GetNormalNum();
	m_iTriNum = OriModel.GetTriNum();
	m_iUVNum = OriModel.GetUVNum();

	m_pVertex = new VECTOR[m_iVertexNum];
	m_pNormal = new VECTOR[m_iNormalNum];
	m_pTriangles = new VECTOR9ui[m_iTriNum];

	if(m_iUVNum>0)
		m_pUVCoordinates = new VECTOR2f[m_iUVNum];
	else
		m_pUVCoordinates = NULL;

	//数据拷贝
	memcpy(m_pVertex, OriModel.GetPVertex(), m_iVertexNum*sizeof(VECTOR));
	memcpy(m_pNormal, OriModel.GetPNormal(), m_iNormalNum*sizeof(VECTOR));
	memcpy(m_pTriangles, OriModel.GetPTriangles(), m_iVertexNum*sizeof(VECTOR9ui));
	
	if(m_iUVNum>0)
		memcpy(m_pUVCoordinates, OriModel.GetPUVCoordinates(), m_iUVNum*sizeof(VECTOR2f));

}

void CModelData::SetModelName(char *strName)
{
	strcpy(m_strName, strName);
}

void CModelData::GetModelName(char *strName)
{
	if(strName==NULL || strlen(m_strName)==0)
		return;

	strcpy(strName, m_strName);
}
