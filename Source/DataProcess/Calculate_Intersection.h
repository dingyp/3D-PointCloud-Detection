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
	typedef struct STLBlockList										//STL分成每一个Block的结构体
	{
		int index1;													//这个三角片在m_pVertex中的索引，包括index,index+1,index+2
		int index2;
		int index3;
		STLBlockList* next;											//指向下一个结构体
	}STLBlockList;

	class Calcul_Intersection_API Calcul_Intersection
	{
	public:
		Calcul_Intersection();
		~Calcul_Intersection();

		void GetSTL(CDataList *pDataLiat);					//得到STL并对STL模型进行整理
		void GetOBJ(CModelList *pModelList);						//得到OBJ并对OBJ模型进行整理
		void DelBlockList();										//在计算结束后需要对得到的model资源进行释放

		void Triangle_In_Block(UINT one,UINT two,UINT three);//计算某个三角形是否落在块里面，里面UINT是顶点索引

		bool Whether_Line_Intersect_Tri(VECTOR one, VECTOR two, VECTOR three, RadarPoint onePoint, \
			VECTOR& intersection_Point);						//判断空间中一条直线和三角面片是否相交，相交无数点时,返回false..

		bool GetOnePointIntersection(RadarPoint onePoint, VECTOR& intersection_Point,int i);					//进行一个点交点计算,返回是否有交点
		void GetPointCloudIntersection(pRadarPointList pPointCloud,VECTOR* intersection_PointCloud);			//进行一帧交点计算,返回每个点的交点，没有交点为(0,0,0)
		void GetPointCloudsIntersection(CPointClouds * pointClouds_ptr,pPointClouds& intersection_PointClouds);	//进行多帧交点计算

		void GetPointCloudIntersection(CPointClouds * pointClouds_ptr);//进行平均帧交点计算，最后交点表现成到原点的距离

		void GetPointCloudIntersection(CPointClouds * pointClouds_ptr, double *parameter);//parameter是4个旋转参数，分别对点云进行水平旋转，x、y、z平移
	private:
		VECTOR * m_pVertex;											//顶点队列
		long m_iTriNum;												//三角形数量
		CDataList * m_pDataList;									//STL模型指针
		CModelList *m_pModelList;									//OBJ模型列表指针
		STLBlockList* m_stlBlockList[72][18];						//模型分块头
		int conver_array[9][2];										//9宫格转换矩阵
	};
}