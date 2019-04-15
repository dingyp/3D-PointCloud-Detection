#include "stdafx.h"
#include <fstream>
#include <math.h>
#include "Calculate_Intersection.h"

using namespace DataProcess;

Calcul_Intersection::Calcul_Intersection()
{
	//一开始将三角片设置为空
	m_pVertex = NULL;
	m_iTriNum = 0;
	m_pDataList = NULL;
	m_pModelList = NULL;
	memset(m_stlBlockList, NULL, sizeof(STLBlockList*) * 72 * 18);
	//对9宫格转换矩阵进行初始化
	conver_array[0][0] = 0;conver_array[0][1] = 0;//自己
	conver_array[1][0] = -1;conver_array[1][1] = 0;//左
	conver_array[2][0] = 1;conver_array[2][1] = 0;//右
	conver_array[3][0] = 0;conver_array[3][1] = 1;//上
	conver_array[4][0] = 0;conver_array[4][1] = -1;//下
	conver_array[5][0] = -1;conver_array[5][1] = 1;//左上
	conver_array[6][0] = -1;conver_array[6][1] = -1;//左下
	conver_array[7][0] = 1;conver_array[7][1] = 1;//右上
	conver_array[8][0] = 1;conver_array[8][1] = -1;//右下
}

Calcul_Intersection::~Calcul_Intersection()
{
	DelBlockList();
}

//在计算结束后需要对得到STL的资源进行释放
void Calcul_Intersection::DelBlockList()
{
	//如果没有对m_stlBlockList进行释放，进行释放
	STLBlockList* p = NULL;
	STLBlockList* q = NULL;
	for (int i = 0; i < 72; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			p = m_stlBlockList[i][j];
			while (NULL != p)
			{
				q = p;
				p = p->next;
				delete(q);
			}
		}
	}
	m_pVertex = NULL;
	m_iTriNum = 0;
}

//得到OBJ并对OBJ模型进行整理
void Calcul_Intersection::GetOBJ(CModelList *pModelList)
{
	if (NULL == pModelList)
	{
		return;
	}
	m_pModelList = pModelList;
	STLBlockList* p = NULL;//操作指针
	int horizontal_index[4];//水平索引
	int vertical_index[4];//垂直索引
	double x[4], y[4], z[4];
	VECTOR9ui *pFaces = NULL;
	for (int i = 0; i < m_pModelList->GetModelNum(); i++)
	{
		//现阶段认为OBJ里面的obj只有一个
		m_pVertex = m_pModelList->m_vpModels[i]->GetPVertex();//返回顶点队列指针
		m_iTriNum = m_pModelList->m_vpModels[i]->GetTriNum();
		pFaces = m_pModelList->m_vpModels[i]->GetPTriangles();//返回索引队列指针
		for (int j = 0; j < m_iTriNum; j++)
		{//遍历这个三角形里面的顶点
			Triangle_In_Block(pFaces[j].x, pFaces[j].y, pFaces[j].z);
		}
	}

}

//得到STL并对STL模型进行整理
void Calcul_Intersection::GetSTL(CDataList *pDataLiat)
{
	if (NULL == pDataLiat)
	{
		return;
	}
	m_pDataList = pDataLiat;
	m_pVertex = m_pDataList->m_pVertex;
	m_iTriNum = m_pDataList->m_iTriNum;
	//进行面片整理，分成72(5度一格)个格子，上下18格(2度一格)
	for (int i = 0; i < m_pDataList->m_iTriNum * 3; i += 3)
	{
		//分别求出三角面片3个顶点落在哪个块里面
		Triangle_In_Block(i, i + 1, i + 2);
	}
}

//计算某个三角形是否落在块里面，里面UINT是顶点索引
void Calcul_Intersection::Triangle_In_Block(UINT one, UINT two, UINT three)
{
	double x[4], y[4], z[4];
	//one
	x[0] = m_pVertex[one].x;
	y[0] = m_pVertex[one].y;
	z[0] = m_pVertex[one].z;
	//two
	x[1] = m_pVertex[two].x;
	y[1] = m_pVertex[two].y;
	z[1] = m_pVertex[two].z;
	//three
	x[2] = m_pVertex[three].x;
	y[2] = m_pVertex[three].y;
	z[2] = m_pVertex[three].z;
	//求出三角面片的中心
	x[3] = (x[0] + x[1] + x[2]) / 3.0;
	y[3] = (y[0] + y[1] + y[2]) / 3.0;
	z[3] = (z[0] + z[1] + z[2]) / 3.0;

	STLBlockList* p = NULL;//操作指针
	int horizontal_index[4];//水平索引
	int vertical_index[4];//垂直索引
	for (int j = 0; j < 4; j++)
	{
		double r1 = sqrt(x[j] * x[j] + y[j] * y[j]);
		double r2 = Rad2Deg(atan(z[j] / sqrt(x[j] * x[j] + y[j] * y[j])));
		vertical_index[j] = int(Rad2Deg(atan(z[j] / sqrt(x[j] * x[j] + y[j] * y[j]))) + 18);
		if (vertical_index[j] < 36 && vertical_index[j] >= 0)
		{
			vertical_index[j] /= 2;
			horizontal_index[j] = int(Rad2Deg(atan(x[j] / y[j])));
			//考虑到tan()的周期是PI，所以还需要有约束条件
			if ((y[j]<0.0) || (fabs(y[j])<1e-7&&x[j]<0.0))
			{
				horizontal_index[j] += 180.0;
			}
			while (horizontal_index[j] >= 360.0)
			{
				horizontal_index[j] -= 360.0;
			}
			while (horizontal_index[j] < 0.0)
			{
				horizontal_index[j] += 360;
			}
			horizontal_index[j] /= 5;
		}
		else
		{
			vertical_index[j] = -1;
		}
	}

	//只有不重复的点才会进行添加
	for (int j = 0; j < 4; j++)
	{
		int sign = 0;
		//j只有不和j之后重复的时候才考虑
		for (int k = j + 1; k < 4; k++)
		{
			if ((vertical_index[j] == vertical_index[k]) && (horizontal_index[j] == horizontal_index[k]))
			{
				sign = 1;
				break;
			}
		}
		if (0 == sign)//所有都不重复则进行添加
		{
			if (-1 != vertical_index[j])
			{
				p = new STLBlockList();
				p->index1 = one;
				p->index2 = two;
				p->index3 = three;
				p->next = m_stlBlockList[horizontal_index[j]][vertical_index[j]];
				m_stlBlockList[horizontal_index[j]][vertical_index[j]] = p;
				p = NULL;
			}
		}
	}
}

//进行多帧交点计算
void Calcul_Intersection::GetPointCloudsIntersection(CPointClouds * pointClouds_ptr, pPointClouds& intersection_PointClouds)
{
	//在计算之前先对两个数据是否都有进行判断
	if (NULL == pointClouds_ptr)
	{
		return;
	}
	if (NULL == m_pDataList && NULL == m_pModelList)
	{//没有进行求交的模型自然要退出
		return;
	}
	//遍历所有点云数据
	for (int i = 0; i < pointClouds_ptr->m_uiNowCloudNums; i++)
	{
		GetPointCloudIntersection(pointClouds_ptr->m_PointCloud[i], intersection_PointClouds[i]);
	}
}

//parameter是4个旋转参数，分别对点云进行水平旋转，x、y、z平移
//parameter[0]的单位是0.05度，parameter[1]、parameter[2]、parameter[3]的单位是0.05m,
void Calcul_Intersection::GetPointCloudIntersection(CPointClouds * pointClouds_ptr, double *parameter)
{
	//这里面有旋转，所以所求的点应该要是旋转之后的
	if (NULL == pointClouds_ptr)
	{
		return;
	}
	if (NULL == m_pDataList && NULL == m_pModelList)
	{//没有进行求交的模型自然要退出
		return;
	}
	if (0==pointClouds_ptr->m_choosechannel_number)
	{//没有设置配准通道退出
		return;
	}

	//std::ofstream fout;

	//平均帧角度固定，每隔0.18度是一格，总共2000格
	//那么，我们用0.09、0.09+0.18=0.27、0.45、0.63、0.81、0.99等来表示
	//现在默认是16通道
	int point_vertical_index;
	int point_horizontal_index;
	//重置一下之后需要存放交点的地方,防止受之前求交的影响
	memset(pointClouds_ptr->m_ISDistance, 0, sizeof(double) * 16 * 2000);



	for (int channelnumber = 0; channelnumber<pointClouds_ptr->m_choosechannel_number; channelnumber++)
	{
		int channelIdx = pointClouds_ptr->m_choosechannel[channelnumber];
		for (int angleIdx = 0; angleIdx < 2000; angleIdx++)
		{

			//fout.open("F:\\常州房间文件\\block_tmp.csv", std::ios::out);
			//fout << "x1,y1,z1,horizontal1,vertical1,x2,y2,z2,horizontal2,vertical2,x3,y3,z3,horizontal3,vertical3,INTERSECTION" << endl;

			double angle_H = angleIdx * 0.18 + 0.09 + 0.05*parameter[0];

			STLBlockList* p = NULL;
			point_horizontal_index = int(angle_H/5.0);

			//channel和
			point_vertical_index = (pointClouds_ptr->AnglesV_Deg[channelIdx] + 18) / 2;

			VECTOR intersection_Point = { 0,0,0 };
			//默认onePoint长度是3米
			RadarPoint onePoint;
			onePoint.x = 3 * pointClouds_ptr->AnglesV_Cos[channelIdx] * sin(Deg2Rad(angle_H)) + 0.05 * parameter[1];
			onePoint.y = 3 * pointClouds_ptr->AnglesV_Cos[channelIdx] * cos(Deg2Rad(angle_H)) + 0.05 * parameter[2];
			onePoint.z = 3 * pointClouds_ptr->AnglesV_Sin[channelIdx] + 0.05 * parameter[3];
			int sign = 0;


			//fout << onePoint.x << "," << onePoint.y << "," << onePoint.z << "," << point_horizontal_index << "," << point_vertical_index << endl;

			for (int i = 0; i < 9; i++)
			{//依次遍历9宫格转换矩阵，得到有交点的三角形就退出
				int tmp_horizontal = point_horizontal_index + conver_array[i][0];//水平
				int tmp_vertical = point_vertical_index + conver_array[i][1];//垂直方向因为有冗余，所以不存在超出范围的可能性
				if (tmp_horizontal >= 72)
				{
					tmp_horizontal -= 72;
				}
				if (tmp_horizontal < 0)
				{
					tmp_horizontal += 72;
				}

				p = m_stlBlockList[tmp_horizontal][tmp_vertical];

				while (p)
				{
					//double tmp = Rad2Deg(atan(m_pVertex[p->index1].x / m_pVertex[p->index1].y));
					//if (m_pVertex[p->index1].y < 0.0 || (fabs(m_pVertex[p->index1].y) < 1e-7&&m_pVertex[p->index1].x < 0.0))
					//{
					//	tmp += 180.0;
					//}
					//fout << m_pVertex[p->index1].x << "," << m_pVertex[p->index1].y << "," << m_pVertex[p->index1].z << ","
					//	<< int(tmp / 5) << "," << int((Rad2Deg(atan(m_pVertex[p->index1].z / sqrt(m_pVertex[p->index1].x * m_pVertex[p->index1].x + \
					//		m_pVertex[p->index1].y *m_pVertex[p->index1].y))) + 18.0) / 2) << ",";

					//tmp = Rad2Deg(atan(m_pVertex[p->index2].x / m_pVertex[p->index2].y));
					//if (m_pVertex[p->index2].y < 0.0 || (fabs(m_pVertex[p->index2].y) < 1e-7&&m_pVertex[p->index2].x < 0.0))
					//{
					//	tmp += 180.0;
					//}
					//fout << m_pVertex[p->index2].x << "," << m_pVertex[p->index2].y << "," << m_pVertex[p->index2].z << ","
					//	<< int(tmp / 5) << "," << int((Rad2Deg(atan(m_pVertex[p->index2].z / sqrt(m_pVertex[p->index2].x * m_pVertex[p->index2].x + \
					//		m_pVertex[p->index2].y *m_pVertex[p->index2].y))) + 18.0) / 2) << ",";

					//tmp = Rad2Deg(atan(m_pVertex[p->index3].x / m_pVertex[p->index3].y));
					//if (m_pVertex[p->index3].y < 0.0 || (fabs(m_pVertex[p->index3].y) < 1e-7&&m_pVertex[p->index3].x < 0.0))
					//{
					//	tmp += 180.0;
					//}
					//fout << m_pVertex[p->index3].x << "," << m_pVertex[p->index3].y << "," << m_pVertex[p->index3].z << ","
					//	<< int(tmp / 5) << "," << int((Rad2Deg(atan(m_pVertex[p->index3].z / sqrt(m_pVertex[p->index3].x * m_pVertex[p->index3].x + \
					//		m_pVertex[p->index3].y *m_pVertex[p->index3].y))) + 18.0) / 2);


					//进行计算，有交点则统计下来，没有置0,计算下一个
					if (Whether_Line_Intersect_Tri(m_pVertex[p->index1], m_pVertex[p->index2], m_pVertex[p->index3], onePoint, intersection_Point))//有交点
					{
						//进行计算赋值然后开始下一个的计算
						pointClouds_ptr->m_ISDistance[channelIdx][angleIdx] = sqrt(intersection_Point.x*intersection_Point.x + \
							intersection_Point.y*intersection_Point.y + intersection_Point.z*intersection_Point.z);
						sign = 1;//置标志位，便于外层循环的退出
						break;
					}
					//fout << ",No" << endl;
					p = p->next;
				}
				if (1 == sign)
				{
					break;
				}
			}


			//fout.close();

		}
	}
}

//进行平均帧交点计算，最后交点表现成到原点的距离
void Calcul_Intersection::GetPointCloudIntersection(CPointClouds * pointClouds_ptr)
{
	if (NULL == pointClouds_ptr)
	{
		return;
	}
	if (NULL == m_pDataList && NULL == m_pModelList)
	{//没有进行求交的模型自然要退出
		return;
	}

	//平均帧角度固定，每隔0.18度是一格，总共2000格
	//那么，我们用0.09、0.09+0.18=0.27、0.45、0.63、0.81、0.99等来表示
	//现在默认是16通道
	int point_vertical_index;
	int point_horizontal_index;
	//重置一下之后需要存放交点的地方,防止受之前求交的影响
	memset(pointClouds_ptr->m_ISDistance, 0, sizeof(double)*16*2000);
	for (int channelIdx = 6; channelIdx<10; channelIdx++)
	{
		for (int angleIdx = 0; angleIdx < 2000; angleIdx++)
		{
			double angle_H = angleIdx * 0.18 + 0.09;
			STLBlockList* p = NULL;
			point_horizontal_index = int(angleIdx*0.036);

			//channel和
			point_vertical_index = (pointClouds_ptr->AnglesV_Deg[channelIdx] + 18) / 2;

			VECTOR intersection_Point = { 0,0,0 };
			//默认onePoint长度是3米
			RadarPoint onePoint;
			onePoint.x = 3 * pointClouds_ptr->AnglesV_Cos[channelIdx] * sin(Deg2Rad(angle_H));
			onePoint.y = 3 * pointClouds_ptr->AnglesV_Cos[channelIdx] * cos(Deg2Rad(angle_H));
			onePoint.z = 3 * pointClouds_ptr->AnglesV_Sin[channelIdx];
			int sign = 0;
			for (int i = 0; i < 9; i++)
			{//依次遍历9宫格转换矩阵，得到有交点的三角形就退出
				int tmp_horizontal = point_horizontal_index + conver_array[i][0];//水平
				int tmp_vertical = point_vertical_index + conver_array[i][1];//垂直方向因为有冗余，所以不存在超出范围的可能性
				if (tmp_horizontal >= 72)
				{
					tmp_horizontal -= 72;
				}
				if (tmp_horizontal < 0)
				{
					tmp_horizontal += 72;
				}

				p = m_stlBlockList[tmp_horizontal][tmp_vertical];

				while (p)
				{
					//进行计算，有交点则统计下来，没有置0,计算下一个
					if (Whether_Line_Intersect_Tri(m_pVertex[p->index1], m_pVertex[p->index2], m_pVertex[p->index3], onePoint, intersection_Point))//有交点
					{
						//进行计算赋值然后开始下一个的计算
						pointClouds_ptr->m_ISDistance[channelIdx][angleIdx] = sqrt(intersection_Point.x*intersection_Point.x + \
							intersection_Point.y*intersection_Point.y + intersection_Point.z*intersection_Point.z);
						sign = 1;//置标志位，便于外层循环的退出
						break;
					}
					p = p->next;
				}
				if (1 == sign)
				{
					break;
				}
			}
		}
	}
}

//进行一帧交点计算,返回每个点的交点，没有交点为(0,0,0)
void Calcul_Intersection::GetPointCloudIntersection(pRadarPointList pPointCloud, VECTOR* intersection_PointCloud)
{
	if (NULL == pPointCloud || NULL == intersection_PointCloud)
	{
		return;
	}
	if (NULL == m_pDataList && NULL == m_pModelList)
	{//没有进行求交的模型自然要退出
		return;
	}
	int size = pPointCloud->size();
	for (int i = 0; i < size; i++)
	{
		GetOnePointIntersection((*pPointCloud)[i], intersection_PointCloud[i], i);
	}
}

//进行一个点交点计算,返回是否有找到交点
bool Calcul_Intersection::GetOnePointIntersection(RadarPoint onePoint, VECTOR& intersection_Point,int all)
{
	if (NULL == m_pDataList && NULL == m_pModelList)
	{//没有进行求交的模型自然要退出
		return false;
	}
	//先计算这个点在哪个块中
	//int number = all;
	//if (6123 == number)
	//{
	//	int a = 11;
	//}
	//double my = onePoint.z / sqrt(onePoint.x * onePoint.x + onePoint.y *onePoint.y);
	//double thetaa = Rad2Deg(atan(onePoint.z / sqrt(onePoint.x * onePoint.x + onePoint.y *onePoint.y)));
	int point_vertical_index = int(Rad2Deg(atan(onePoint.z / sqrt(onePoint.x * onePoint.x + onePoint.y *onePoint.y)))+18.0);
	//point_vertical_index += 18;
	point_vertical_index /= 2;
	//double my = onePoint.y / onePoint.x;
	//double thetaa = Rad2Deg(atan(onePoint.y / onePoint.x));
	int point_horizontal_index = int(Rad2Deg(atan(onePoint.y / onePoint.x)));
	if (onePoint.x<0.0 || (fabs(onePoint.x)<1e-7&&onePoint.y<0.0))
	{
		point_horizontal_index += 180.0;
	}
	while (point_horizontal_index >= 360.0)
	{
		point_horizontal_index -= 360.0;
	}
	while (point_horizontal_index < 0.0)
	{
		point_horizontal_index += 360.0;
	}
	point_horizontal_index /= 5;
	//遍历STL当前在[point_horizontal_index][point_vertical_index]以及周围共9格，得到与之有交点的三角形

	//std::ofstream fout;
	//if (6123 == number)
	//{
	//	fout.open("F:\\常州房间文件\\block_tmp.csv", std::ios::out);
	//	fout << "index,x1,y1,z1,horizontal1,vertical1,x2,y2,z2,horizontal2,vertical2,x3,y3,z3,horizontal3,vertical3,INTERSECTION" << endl;
	//}
	
	STLBlockList* p = NULL;
	for (int i = 0; i < 9; i++)
	{//依次遍历9宫格转换矩阵，得到有交点的三角形就退出
		//if (6123 == number)
		//{
		//	fout << i << std::endl;
		//}
		int tmp_horizontal = point_horizontal_index + conver_array[i][0];//水平
		int tmp_vertical = point_vertical_index + conver_array[i][1];//垂直方向因为有冗余，所以不存在超出范围的可能性
		if (tmp_horizontal >= 72)
		{
			tmp_horizontal -= 72;
		}
		if (tmp_horizontal < 0)
		{
			tmp_horizontal += 72;
		}
		
		p = m_stlBlockList[tmp_horizontal][tmp_vertical];
		while (p)
		{
			//if (6123 == number)
			//{
			//	double tmp = Rad2Deg(atan(m_pVertex[p->index1].y / m_pVertex[p->index1].x));
			//	if (m_pVertex[p->index1].x < 0.0 || (fabs(m_pVertex[p->index1].x) < 1e-7&&m_pVertex[p->index1].y < 0.0))
			//	{
			//		tmp += 180.0;
			//	}
			//	fout << p->index1 << "," << m_pVertex[p->index1].x << "," << m_pVertex[p->index1].y << "," << m_pVertex[p->index1].z << ","
			//		<< tmp << "," << Rad2Deg(atan(m_pVertex[p->index1].z / sqrt(m_pVertex[p->index1].x * m_pVertex[p->index1].x + m_pVertex[p->index1].y *m_pVertex[p->index1].y))) + 18.0<<",";
			//	
			//	tmp = Rad2Deg(atan(m_pVertex[p->index2].y / m_pVertex[p->index2].x));
			//	if (m_pVertex[p->index2].x < 0.0 || (fabs(m_pVertex[p->index2].x) < 1e-7&&m_pVertex[p->index2].y < 0.0))
			//	{
			//		tmp += 180.0;
			//	}
			//	fout << m_pVertex[p->index2].x << "," << m_pVertex[p->index2].y << "," << m_pVertex[p->index2].z << ","
			//		<< tmp << "," << Rad2Deg(atan(m_pVertex[p->index2].z / sqrt(m_pVertex[p->index2].x * m_pVertex[p->index2].x + m_pVertex[p->index2].y *m_pVertex[p->index2].y))) + 18.0<<",";
			//	
			//	tmp = Rad2Deg(atan(m_pVertex[p->index3].y / m_pVertex[p->index3].x));
			//	if (m_pVertex[p->index3].x < 0.0 || (fabs(m_pVertex[p->index3].x) < 1e-7&&m_pVertex[p->index3].y < 0.0))
			//	{
			//		tmp += 180.0;
			//	}
			//	fout << m_pVertex[p->index3].x << "," << m_pVertex[p->index3].y << "," << m_pVertex[p->index3].z << ","
			//		<< tmp << "," << Rad2Deg(atan(m_pVertex[p->index3].z / sqrt(m_pVertex[p->index3].x * m_pVertex[p->index3].x + m_pVertex[p->index3].y *m_pVertex[p->index3].y))) + 18.0;
			//}
			
			if (Whether_Line_Intersect_Tri(m_pVertex[p->index1], m_pVertex[p->index2], m_pVertex[p->index3], onePoint, intersection_Point))//有交点
			{
				return true;
			}
			//if (6123 == number)
			//{
			//	fout << ",NO" << std::endl;
			//}
			p = p->next;
		}
	}
	//if (6123 == number)
	//{
	//	fout.close();
	//}
	return false;
}

// Determine whether a ray intersect with a triangle  确定是否有相交的射线与三角形 
// Parameters
// dir: direction of the ray  射线方向上的一个点
//pTri: vertices of triangle 三角形顶点所在数组的首地址
// t(out): weight of the intersection for the ray  射线交点权 
// u(out), v(out): barycentric coordinate of intersection  交点重心坐标[交点的坐标对应v0(1-u-v)+v v1+uv2]或者dir*t
//判断空间中一条直线和三角面片是否相交，相交无数点时,返回false..
bool Calcul_Intersection::Whether_Line_Intersect_Tri(VECTOR one, VECTOR two, VECTOR three, RadarPoint onePoint, VECTOR& intersection_Point)
{
	VECTOR E1 = two - one;// v1 - v0;// E1
	VECTOR E2 = three - one;// v2 - v0;// E2
	VECTOR dir;
	//因为起源于零点，所以方向坐标就是该点本身的坐标
	dir.x = onePoint.x;
	dir.y = onePoint.y;
	dir.z = onePoint.z;

	VECTOR P = dir * E2;//两个向量的叉积 P
	float det = E1 ^ P;//两个向量的点积  determinant 行列式

	VECTOR T;
	//确保det>0,如果det<0则取相反数，并调整T
	if (det >0)
	{
		T.x = -one.x;
		T.y = -one.y;
		T.z = -one.z;
	}
	else
	{
		T = one;
		det = -det;
	}

	// If determinant is near zero, ray lies in plane of triangle 如果行列式接近于零，则光线位于三角形平面内
	if (det < 0.0001f)
	{
		intersection_Point.x = 0.0;
		intersection_Point.y = 0.0;
		intersection_Point.z = 0.0;
		return false;
	}

	// Calculate u and make sure u <= 1
	float u = T ^ P;
	if (u < 0.0f || u > det)
	{
		intersection_Point.x = 0.0;
		intersection_Point.y = 0.0;
		intersection_Point.z = 0.0;
		return false;
	}

	// Q
	VECTOR Q = T * E1;

	// Calculate v and make sure u + v <= 1
	float v = dir ^ Q;
	if (v < 0.0f || u + v > det)
	{
		intersection_Point.x = 0.0;
		intersection_Point.y = 0.0;
		intersection_Point.z = 0.0;
		return false;
	}

	// Calculate t, scale parameters, ray intersects triangle
	float t = E2 ^ Q;

	float fInvDet = 1.0f / det;
	t *= fInvDet;
	//u *= fInvDet;
	//v *= fInvDet;
	intersection_Point = t * dir;

	return true;
}
