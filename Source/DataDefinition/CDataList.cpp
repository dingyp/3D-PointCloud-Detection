#include "stdafx.h"
#include "CDataList.h"

using namespace DataDefinition;

//数据list的构造函数
CDataList::CDataList()
{
	m_pVertex = NULL;
	m_pNormal = NULL;
	m_iTriNum = 0;
	m_iVertexNum = 0;
}

//数据list的析构函数
CDataList::~CDataList()
{
	ClearData();
}

//清除里面保存的数据
void CDataList::ClearData()
{
	m_iTriNum = 0;
	m_iVertexNum = 0;
	if (m_pVertex != NULL)
	{
		delete[]m_pVertex;
		m_pVertex = NULL;
	}
		
	if (m_pNormal != NULL)
	{
		delete[]m_pNormal;
		m_pNormal = NULL;
	}
}

void CDataList::GetBoundBox(float& xMin, float& xMax, float& yMin, float& yMax, float& zMin, float& zMax)
{
	xMin = yMin = zMin = FLT_MAX;
	xMax = yMax = zMax = FLT_MIN;

	for (int i = 0; i < m_iVertexNum; i++)
	{
		//得到
		if (m_pVertex[i].x < xMin)
			xMin = m_pVertex[i].x;
		if (m_pVertex[i].y < yMin)
			yMin = m_pVertex[i].y;
		if (m_pVertex[i].z < zMin)
			zMin = m_pVertex[i].z;
		if (m_pVertex[i].x > xMax)
			xMax = m_pVertex[i].x;
		if (m_pVertex[i].y > yMax)
			yMax = m_pVertex[i].y;
		if (m_pVertex[i].z > zMax)
			zMax = m_pVertex[i].z;
	}
}

