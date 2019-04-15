#include "stdafx.h"
#include <fstream>
#include <math.h>
#include "Calculate_Intersection.h"

using namespace DataProcess;

Calcul_Intersection::Calcul_Intersection()
{
	//һ��ʼ������Ƭ����Ϊ��
	m_pVertex = NULL;
	m_iTriNum = 0;
	m_pDataList = NULL;
	m_pModelList = NULL;
	memset(m_stlBlockList, NULL, sizeof(STLBlockList*) * 72 * 18);
	//��9����ת��������г�ʼ��
	conver_array[0][0] = 0;conver_array[0][1] = 0;//�Լ�
	conver_array[1][0] = -1;conver_array[1][1] = 0;//��
	conver_array[2][0] = 1;conver_array[2][1] = 0;//��
	conver_array[3][0] = 0;conver_array[3][1] = 1;//��
	conver_array[4][0] = 0;conver_array[4][1] = -1;//��
	conver_array[5][0] = -1;conver_array[5][1] = 1;//����
	conver_array[6][0] = -1;conver_array[6][1] = -1;//����
	conver_array[7][0] = 1;conver_array[7][1] = 1;//����
	conver_array[8][0] = 1;conver_array[8][1] = -1;//����
}

Calcul_Intersection::~Calcul_Intersection()
{
	DelBlockList();
}

//�ڼ����������Ҫ�Եõ�STL����Դ�����ͷ�
void Calcul_Intersection::DelBlockList()
{
	//���û�ж�m_stlBlockList�����ͷţ������ͷ�
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

//�õ�OBJ����OBJģ�ͽ�������
void Calcul_Intersection::GetOBJ(CModelList *pModelList)
{
	if (NULL == pModelList)
	{
		return;
	}
	m_pModelList = pModelList;
	STLBlockList* p = NULL;//����ָ��
	int horizontal_index[4];//ˮƽ����
	int vertical_index[4];//��ֱ����
	double x[4], y[4], z[4];
	VECTOR9ui *pFaces = NULL;
	for (int i = 0; i < m_pModelList->GetModelNum(); i++)
	{
		//�ֽ׶���ΪOBJ�����objֻ��һ��
		m_pVertex = m_pModelList->m_vpModels[i]->GetPVertex();//���ض������ָ��
		m_iTriNum = m_pModelList->m_vpModels[i]->GetTriNum();
		pFaces = m_pModelList->m_vpModels[i]->GetPTriangles();//������������ָ��
		for (int j = 0; j < m_iTriNum; j++)
		{//�����������������Ķ���
			Triangle_In_Block(pFaces[j].x, pFaces[j].y, pFaces[j].z);
		}
	}

}

//�õ�STL����STLģ�ͽ�������
void Calcul_Intersection::GetSTL(CDataList *pDataLiat)
{
	if (NULL == pDataLiat)
	{
		return;
	}
	m_pDataList = pDataLiat;
	m_pVertex = m_pDataList->m_pVertex;
	m_iTriNum = m_pDataList->m_iTriNum;
	//������Ƭ�����ֳ�72(5��һ��)�����ӣ�����18��(2��һ��)
	for (int i = 0; i < m_pDataList->m_iTriNum * 3; i += 3)
	{
		//�ֱ����������Ƭ3�����������ĸ�������
		Triangle_In_Block(i, i + 1, i + 2);
	}
}

//����ĳ���������Ƿ����ڿ����棬����UINT�Ƕ�������
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
	//���������Ƭ������
	x[3] = (x[0] + x[1] + x[2]) / 3.0;
	y[3] = (y[0] + y[1] + y[2]) / 3.0;
	z[3] = (z[0] + z[1] + z[2]) / 3.0;

	STLBlockList* p = NULL;//����ָ��
	int horizontal_index[4];//ˮƽ����
	int vertical_index[4];//��ֱ����
	for (int j = 0; j < 4; j++)
	{
		double r1 = sqrt(x[j] * x[j] + y[j] * y[j]);
		double r2 = Rad2Deg(atan(z[j] / sqrt(x[j] * x[j] + y[j] * y[j])));
		vertical_index[j] = int(Rad2Deg(atan(z[j] / sqrt(x[j] * x[j] + y[j] * y[j]))) + 18);
		if (vertical_index[j] < 36 && vertical_index[j] >= 0)
		{
			vertical_index[j] /= 2;
			horizontal_index[j] = int(Rad2Deg(atan(x[j] / y[j])));
			//���ǵ�tan()��������PI�����Ի���Ҫ��Լ������
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

	//ֻ�в��ظ��ĵ�Ż�������
	for (int j = 0; j < 4; j++)
	{
		int sign = 0;
		//jֻ�в���j֮���ظ���ʱ��ſ���
		for (int k = j + 1; k < 4; k++)
		{
			if ((vertical_index[j] == vertical_index[k]) && (horizontal_index[j] == horizontal_index[k]))
			{
				sign = 1;
				break;
			}
		}
		if (0 == sign)//���ж����ظ���������
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

//���ж�֡�������
void Calcul_Intersection::GetPointCloudsIntersection(CPointClouds * pointClouds_ptr, pPointClouds& intersection_PointClouds)
{
	//�ڼ���֮ǰ�ȶ����������Ƿ��н����ж�
	if (NULL == pointClouds_ptr)
	{
		return;
	}
	if (NULL == m_pDataList && NULL == m_pModelList)
	{//û�н����󽻵�ģ����ȻҪ�˳�
		return;
	}
	//�������е�������
	for (int i = 0; i < pointClouds_ptr->m_uiNowCloudNums; i++)
	{
		GetPointCloudIntersection(pointClouds_ptr->m_PointCloud[i], intersection_PointClouds[i]);
	}
}

//parameter��4����ת�������ֱ�Ե��ƽ���ˮƽ��ת��x��y��zƽ��
//parameter[0]�ĵ�λ��0.05�ȣ�parameter[1]��parameter[2]��parameter[3]�ĵ�λ��0.05m,
void Calcul_Intersection::GetPointCloudIntersection(CPointClouds * pointClouds_ptr, double *parameter)
{
	//����������ת����������ĵ�Ӧ��Ҫ����ת֮���
	if (NULL == pointClouds_ptr)
	{
		return;
	}
	if (NULL == m_pDataList && NULL == m_pModelList)
	{//û�н����󽻵�ģ����ȻҪ�˳�
		return;
	}
	if (0==pointClouds_ptr->m_choosechannel_number)
	{//û��������׼ͨ���˳�
		return;
	}

	//std::ofstream fout;

	//ƽ��֡�Ƕȹ̶���ÿ��0.18����һ���ܹ�2000��
	//��ô��������0.09��0.09+0.18=0.27��0.45��0.63��0.81��0.99������ʾ
	//����Ĭ����16ͨ��
	int point_vertical_index;
	int point_horizontal_index;
	//����һ��֮����Ҫ��Ž���ĵط�,��ֹ��֮ǰ�󽻵�Ӱ��
	memset(pointClouds_ptr->m_ISDistance, 0, sizeof(double) * 16 * 2000);



	for (int channelnumber = 0; channelnumber<pointClouds_ptr->m_choosechannel_number; channelnumber++)
	{
		int channelIdx = pointClouds_ptr->m_choosechannel[channelnumber];
		for (int angleIdx = 0; angleIdx < 2000; angleIdx++)
		{

			//fout.open("F:\\���ݷ����ļ�\\block_tmp.csv", std::ios::out);
			//fout << "x1,y1,z1,horizontal1,vertical1,x2,y2,z2,horizontal2,vertical2,x3,y3,z3,horizontal3,vertical3,INTERSECTION" << endl;

			double angle_H = angleIdx * 0.18 + 0.09 + 0.05*parameter[0];

			STLBlockList* p = NULL;
			point_horizontal_index = int(angle_H/5.0);

			//channel��
			point_vertical_index = (pointClouds_ptr->AnglesV_Deg[channelIdx] + 18) / 2;

			VECTOR intersection_Point = { 0,0,0 };
			//Ĭ��onePoint������3��
			RadarPoint onePoint;
			onePoint.x = 3 * pointClouds_ptr->AnglesV_Cos[channelIdx] * sin(Deg2Rad(angle_H)) + 0.05 * parameter[1];
			onePoint.y = 3 * pointClouds_ptr->AnglesV_Cos[channelIdx] * cos(Deg2Rad(angle_H)) + 0.05 * parameter[2];
			onePoint.z = 3 * pointClouds_ptr->AnglesV_Sin[channelIdx] + 0.05 * parameter[3];
			int sign = 0;


			//fout << onePoint.x << "," << onePoint.y << "," << onePoint.z << "," << point_horizontal_index << "," << point_vertical_index << endl;

			for (int i = 0; i < 9; i++)
			{//���α���9����ת�����󣬵õ��н���������ξ��˳�
				int tmp_horizontal = point_horizontal_index + conver_array[i][0];//ˮƽ
				int tmp_vertical = point_vertical_index + conver_array[i][1];//��ֱ������Ϊ�����࣬���Բ����ڳ�����Χ�Ŀ�����
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


					//���м��㣬�н�����ͳ��������û����0,������һ��
					if (Whether_Line_Intersect_Tri(m_pVertex[p->index1], m_pVertex[p->index2], m_pVertex[p->index3], onePoint, intersection_Point))//�н���
					{
						//���м��㸳ֵȻ��ʼ��һ���ļ���
						pointClouds_ptr->m_ISDistance[channelIdx][angleIdx] = sqrt(intersection_Point.x*intersection_Point.x + \
							intersection_Point.y*intersection_Point.y + intersection_Point.z*intersection_Point.z);
						sign = 1;//�ñ�־λ���������ѭ�����˳�
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

//����ƽ��֡������㣬��󽻵���ֳɵ�ԭ��ľ���
void Calcul_Intersection::GetPointCloudIntersection(CPointClouds * pointClouds_ptr)
{
	if (NULL == pointClouds_ptr)
	{
		return;
	}
	if (NULL == m_pDataList && NULL == m_pModelList)
	{//û�н����󽻵�ģ����ȻҪ�˳�
		return;
	}

	//ƽ��֡�Ƕȹ̶���ÿ��0.18����һ���ܹ�2000��
	//��ô��������0.09��0.09+0.18=0.27��0.45��0.63��0.81��0.99������ʾ
	//����Ĭ����16ͨ��
	int point_vertical_index;
	int point_horizontal_index;
	//����һ��֮����Ҫ��Ž���ĵط�,��ֹ��֮ǰ�󽻵�Ӱ��
	memset(pointClouds_ptr->m_ISDistance, 0, sizeof(double)*16*2000);
	for (int channelIdx = 6; channelIdx<10; channelIdx++)
	{
		for (int angleIdx = 0; angleIdx < 2000; angleIdx++)
		{
			double angle_H = angleIdx * 0.18 + 0.09;
			STLBlockList* p = NULL;
			point_horizontal_index = int(angleIdx*0.036);

			//channel��
			point_vertical_index = (pointClouds_ptr->AnglesV_Deg[channelIdx] + 18) / 2;

			VECTOR intersection_Point = { 0,0,0 };
			//Ĭ��onePoint������3��
			RadarPoint onePoint;
			onePoint.x = 3 * pointClouds_ptr->AnglesV_Cos[channelIdx] * sin(Deg2Rad(angle_H));
			onePoint.y = 3 * pointClouds_ptr->AnglesV_Cos[channelIdx] * cos(Deg2Rad(angle_H));
			onePoint.z = 3 * pointClouds_ptr->AnglesV_Sin[channelIdx];
			int sign = 0;
			for (int i = 0; i < 9; i++)
			{//���α���9����ת�����󣬵õ��н���������ξ��˳�
				int tmp_horizontal = point_horizontal_index + conver_array[i][0];//ˮƽ
				int tmp_vertical = point_vertical_index + conver_array[i][1];//��ֱ������Ϊ�����࣬���Բ����ڳ�����Χ�Ŀ�����
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
					//���м��㣬�н�����ͳ��������û����0,������һ��
					if (Whether_Line_Intersect_Tri(m_pVertex[p->index1], m_pVertex[p->index2], m_pVertex[p->index3], onePoint, intersection_Point))//�н���
					{
						//���м��㸳ֵȻ��ʼ��һ���ļ���
						pointClouds_ptr->m_ISDistance[channelIdx][angleIdx] = sqrt(intersection_Point.x*intersection_Point.x + \
							intersection_Point.y*intersection_Point.y + intersection_Point.z*intersection_Point.z);
						sign = 1;//�ñ�־λ���������ѭ�����˳�
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

//����һ֡�������,����ÿ����Ľ��㣬û�н���Ϊ(0,0,0)
void Calcul_Intersection::GetPointCloudIntersection(pRadarPointList pPointCloud, VECTOR* intersection_PointCloud)
{
	if (NULL == pPointCloud || NULL == intersection_PointCloud)
	{
		return;
	}
	if (NULL == m_pDataList && NULL == m_pModelList)
	{//û�н����󽻵�ģ����ȻҪ�˳�
		return;
	}
	int size = pPointCloud->size();
	for (int i = 0; i < size; i++)
	{
		GetOnePointIntersection((*pPointCloud)[i], intersection_PointCloud[i], i);
	}
}

//����һ���㽻�����,�����Ƿ����ҵ�����
bool Calcul_Intersection::GetOnePointIntersection(RadarPoint onePoint, VECTOR& intersection_Point,int all)
{
	if (NULL == m_pDataList && NULL == m_pModelList)
	{//û�н����󽻵�ģ����ȻҪ�˳�
		return false;
	}
	//�ȼ�����������ĸ�����
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
	//����STL��ǰ��[point_horizontal_index][point_vertical_index]�Լ���Χ��9�񣬵õ���֮�н����������

	//std::ofstream fout;
	//if (6123 == number)
	//{
	//	fout.open("F:\\���ݷ����ļ�\\block_tmp.csv", std::ios::out);
	//	fout << "index,x1,y1,z1,horizontal1,vertical1,x2,y2,z2,horizontal2,vertical2,x3,y3,z3,horizontal3,vertical3,INTERSECTION" << endl;
	//}
	
	STLBlockList* p = NULL;
	for (int i = 0; i < 9; i++)
	{//���α���9����ת�����󣬵õ��н���������ξ��˳�
		//if (6123 == number)
		//{
		//	fout << i << std::endl;
		//}
		int tmp_horizontal = point_horizontal_index + conver_array[i][0];//ˮƽ
		int tmp_vertical = point_vertical_index + conver_array[i][1];//��ֱ������Ϊ�����࣬���Բ����ڳ�����Χ�Ŀ�����
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
			
			if (Whether_Line_Intersect_Tri(m_pVertex[p->index1], m_pVertex[p->index2], m_pVertex[p->index3], onePoint, intersection_Point))//�н���
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

// Determine whether a ray intersect with a triangle  ȷ���Ƿ����ཻ�������������� 
// Parameters
// dir: direction of the ray  ���߷����ϵ�һ����
//pTri: vertices of triangle �����ζ�������������׵�ַ
// t(out): weight of the intersection for the ray  ���߽���Ȩ 
// u(out), v(out): barycentric coordinate of intersection  ������������[����������Ӧv0(1-u-v)+v v1+uv2]����dir*t
//�жϿռ���һ��ֱ�ߺ�������Ƭ�Ƿ��ཻ���ཻ������ʱ,����false..
bool Calcul_Intersection::Whether_Line_Intersect_Tri(VECTOR one, VECTOR two, VECTOR three, RadarPoint onePoint, VECTOR& intersection_Point)
{
	VECTOR E1 = two - one;// v1 - v0;// E1
	VECTOR E2 = three - one;// v2 - v0;// E2
	VECTOR dir;
	//��Ϊ��Դ����㣬���Է���������Ǹõ㱾�������
	dir.x = onePoint.x;
	dir.y = onePoint.y;
	dir.z = onePoint.z;

	VECTOR P = dir * E2;//���������Ĳ�� P
	float det = E1 ^ P;//���������ĵ��  determinant ����ʽ

	VECTOR T;
	//ȷ��det>0,���det<0��ȡ�෴����������T
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

	// If determinant is near zero, ray lies in plane of triangle �������ʽ�ӽ����㣬�����λ��������ƽ����
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
