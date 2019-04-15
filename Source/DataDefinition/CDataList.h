#pragma once
#include "StructDefine.h"

#ifdef DATADEFINITION_EXPORTS
#define CDATALIST_API __declspec(dllexport)
#else
#define CDATALIST_API __declspec(dllimport)
#endif

namespace DataDefinition
{
	//数据list
	class CDATALIST_API CDataList
	{
	public:
		CDataList();
		~CDataList();
		void ClearData();													//清除里面保存的数据
		void GetBoundBox(float& xMin, float& xMax, float& yMin, float& yMax, float& zMin, float& zMax);//得到模型的boundingBox

		//在这里面进行添加，这样就不需要再Wnd类里面进行操作
		VECTOR * m_pVertex;													//顶点队列
		VECTOR * m_pNormal;													//法向量队列
		long m_iVertexNum;													//顶点数量
		long m_iTriNum;														//三角形数量
	};

}
