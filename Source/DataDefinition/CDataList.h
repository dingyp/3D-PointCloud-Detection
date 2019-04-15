#pragma once
#include "StructDefine.h"

#ifdef DATADEFINITION_EXPORTS
#define CDATALIST_API __declspec(dllexport)
#else
#define CDATALIST_API __declspec(dllimport)
#endif

namespace DataDefinition
{
	//����list
	class CDATALIST_API CDataList
	{
	public:
		CDataList();
		~CDataList();
		void ClearData();													//������汣�������
		void GetBoundBox(float& xMin, float& xMax, float& yMin, float& yMax, float& zMin, float& zMax);//�õ�ģ�͵�boundingBox

		//�������������ӣ������Ͳ���Ҫ��Wnd��������в���
		VECTOR * m_pVertex;													//�������
		VECTOR * m_pNormal;													//����������
		long m_iVertexNum;													//��������
		long m_iTriNum;														//����������
	};

}
