#pragma once
#include "CDataList.h"
#include "CPointClouds.h"
#include "FilesLoadLib\ModelList.h"

#ifdef DATAPROCESS_EXPORTS
#define Calcul_Intersection_API __declspec(dllexport)
#else
#define Calcul_Intersection_API __declspec(dllimport)
#endif

using namespace DataDefinition;

namespace DataProcess
{
	typedef struct STLBlockList										//STL�ֳ�ÿһ��Block�Ľṹ��
	{
		int index1;													//�������Ƭ��m_pVertex�е�����������index,index+1,index+2
		int index2;
		int index3;
		STLBlockList* next;											//ָ����һ���ṹ��
	}STLBlockList;

	class Calcul_Intersection_API Calcul_Intersection
	{
	public:
		Calcul_Intersection();
		~Calcul_Intersection();

		void GetSTL(CDataList *pDataLiat);					//�õ�STL����STLģ�ͽ�������
		void GetOBJ(CModelList *pModelList);						//�õ�OBJ����OBJģ�ͽ�������
		void DelBlockList();										//�ڼ����������Ҫ�Եõ���model��Դ�����ͷ�

		void Triangle_In_Block(UINT one,UINT two,UINT three);//����ĳ���������Ƿ����ڿ����棬����UINT�Ƕ�������

		bool Whether_Line_Intersect_Tri(VECTOR one, VECTOR two, VECTOR three, RadarPoint onePoint, \
			VECTOR& intersection_Point);						//�жϿռ���һ��ֱ�ߺ�������Ƭ�Ƿ��ཻ���ཻ������ʱ,����false..

		bool GetOnePointIntersection(RadarPoint onePoint, VECTOR& intersection_Point,int i);					//����һ���㽻�����,�����Ƿ��н���
		void GetPointCloudIntersection(pRadarPointList pPointCloud,VECTOR* intersection_PointCloud);			//����һ֡�������,����ÿ����Ľ��㣬û�н���Ϊ(0,0,0)
		void GetPointCloudsIntersection(CPointClouds * pointClouds_ptr,pPointClouds& intersection_PointClouds);	//���ж�֡�������

		void GetPointCloudIntersection(CPointClouds * pointClouds_ptr);//����ƽ��֡������㣬��󽻵���ֳɵ�ԭ��ľ���

		void GetPointCloudIntersection(CPointClouds * pointClouds_ptr, double *parameter);//parameter��4����ת�������ֱ�Ե��ƽ���ˮƽ��ת��x��y��zƽ��
	private:
		VECTOR * m_pVertex;											//�������
		long m_iTriNum;												//����������
		CDataList * m_pDataList;									//STLģ��ָ��
		CModelList *m_pModelList;									//OBJģ���б�ָ��
		STLBlockList* m_stlBlockList[72][18];						//ģ�ͷֿ�ͷ
		int conver_array[9][2];										//9����ת������
	};
}