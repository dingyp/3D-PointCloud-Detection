#include "StdAfx.h"
#include "ModelList.h"

CModelList::CModelList(void)
{
	m_vpModels.clear();
	m_vMaterials.clear();
	m_vTextureMaps.clear();
}

CModelList::~CModelList(void)
{
	ClearAllModels();
}

//得到包围盒
void CModelList::GetBoundBox(float& xMin, float& xMax, float& yMin, float& yMax, float& zMin, float& zMax)
{
	//统计最大最小值
	CModelData *pCurModal;
	xMax = yMax = zMax = FLT_MIN;
	xMin = yMin = zMin = FLT_MAX;
	int ModelNum = GetModelNum();//获得模型的总数
	int SurfaceNum, TotalSurface = 0; //三角面片总数目
	int VertexNum, TotalVertex = 0; //顶点总数目
	for (int j = 0; j < ModelNum; j++)
	{
		pCurModal = m_vpModels[j];
		VECTOR *pVertex = pCurModal->GetPVertex();
		VertexNum = pCurModal->GetVertexNum();
		SurfaceNum = pCurModal->GetTriNum();
		TotalVertex += VertexNum;
		TotalSurface += SurfaceNum;
		for (int i = 0; i < VertexNum; i++)
		{
			if (pVertex[i].x < xMin)
				xMin = pVertex[i].x;
			if (pVertex[i].y < yMin)
				yMin = pVertex[i].y;
			if (pVertex[i].z < zMin)
				zMin = pVertex[i].z;

			if (pVertex[i].x > xMax)
				xMax = pVertex[i].x;
			if (pVertex[i].y > yMax)
				yMax = pVertex[i].y;
			if (pVertex[i].z > zMax)
				zMax = pVertex[i].z;

		}
	}
}

//得到所有模型三角面片的总数
int CModelList::GetTotalSurface()
{
	CModelData *pCurModal;
	int ModelNum = GetModelNum();//获得模型的总数
	int SurfaceNum, TotalSurface = 0; //三角面片总数目
	for (int j = 0; j < ModelNum; j++)
	{
		pCurModal = m_vpModels[j];
		SurfaceNum = pCurModal->GetTriNum();
		TotalSurface += SurfaceNum;
	}
	return TotalSurface;
}

//得到所有模型顶点总数
int CModelList::GetTotalVertex()
{
	CModelData *pCurModal;
	int ModelNum = GetModelNum();//获得模型的总数
	int VertexNum, TotalVertex = 0; //顶点总数目
	for (int j = 0; j < ModelNum; j++)
	{
		pCurModal = m_vpModels[j];
		VertexNum = pCurModal->GetVertexNum();
		TotalVertex += VertexNum;

	}
	return TotalVertex;
}

void CModelList::ClearAllModels()
{
	if(m_vpModels.empty()==false)
	{
		for(unsigned int i=0; i<m_vpModels.size(); i++)
		{
			delete m_vpModels[i];
		}
		m_vpModels.clear();
	}

	if(m_vMaterials.empty()==false)
	{
		m_vMaterials.clear();
	}

	if(m_vTextureMaps.empty()==false)
	{
		m_vTextureMaps.clear();
	}

}

int CModelList::GetColorsNum()
{
	if(m_vMaterials.empty()==true)
		return 0;

	int MatNum = m_vMaterials.size();

	return MatNum;

}

int CModelList::GetTexNum()
{
	if(m_vTextureMaps.empty()==true)
		return 0;

	int TexNum = m_vTextureMaps.size();

	return TexNum;
}


int CModelList::GetModelNum()
{
	if(m_vpModels.empty()==true)
		return 0;

	int ModelNum = m_vpModels.size();

	return ModelNum;

}

void CModelList::AddModelData(CModelData *pModelData)
{
	if(pModelData!=NULL && pModelData->GetTriNum()>0)
		m_vpModels.push_back(pModelData);
	else
		return;

}

void CModelList::InsertModelData(int i, CModelData *pModelData)
{
	vector <CModelData *>::iterator iter;
	iter = m_vpModels.begin()+i;

	if(pModelData!=NULL && pModelData->GetTriNum()>0)
	{
		m_vpModels.insert(iter, pModelData);
	}
	else
		return;
}

void CModelList::AddMaterial(sample_MATERIAL Material)
{
	m_vMaterials.push_back(Material);
}

void CModelList::InsertMaterial(int i, sample_MATERIAL Material)
{
	vector <sample_MATERIAL>::iterator iter;
	iter = m_vMaterials.begin()+i;

	m_vMaterials.insert(iter, Material);

}


void CModelList::AddTexture(sample_TEXTURE Texture)
{
	if(Texture.TexFileName.GetLength()>1)
		m_vTextureMaps.push_back(Texture);
	else
		return;

}

void CModelList::InsertTexture(int i, sample_TEXTURE Texture)
{
	vector <sample_TEXTURE>::iterator iter;
	iter = m_vTextureMaps.begin()+i;

	if(Texture.TexFileName.GetLength()>1)
		m_vTextureMaps.insert(iter, Texture);
	else
		return;

}

BOOL CModelList::GetMaterial(int i, sample_MATERIAL &temp)
{
	BOOL ret = FALSE;

	if( i < (int)m_vMaterials.size() && i>=0)
	{
		temp = m_vMaterials[i];
		ret = TRUE;
	}

	return ret;

}

BOOL CModelList::GetTexture(int i, sample_TEXTURE &temp)
{
	BOOL ret = FALSE;

	temp.id = -1;
	temp.TexFileName = "";

	if( i < (int)m_vTextureMaps.size() && i >= 0 )
	{
		temp.id = m_vTextureMaps[i].id;
		temp.TexFileName = m_vTextureMaps[i].TexFileName;
		ret = TRUE;
	}

	return ret;

}