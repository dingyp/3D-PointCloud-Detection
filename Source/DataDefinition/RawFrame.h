#pragma once
#include "stdafx.h"

#define MAX_POINTS_PERFRAME 80000								//ԭʼ֡�а�������ĸ���

#ifdef DATADEFINITION_EXPORTS
#define SENSORFRAME_API __declspec(dllexport)
#else
#define SENSORFRAME_API __declspec(dllimport)
#endif

namespace DataDefinition 
{
	typedef SENSORFRAME_API struct _tag_PointR					//������֡��֡�ṹ
	{
		unsigned long packID;									//ÿ��������һ��UDP���е�λ��BLOCK_NUM
		unsigned long distance;									//���룬��λ��mm
		unsigned short angleH;									//ˮƽ�Ƕȣ���λ��0.01��
		unsigned char intensity;								//������
		unsigned char channel;									//ͨ����
		unsigned int timestamp_ms;								//ʱ���
	}PointR;

	//�״����ݵ�֡�ṹ����
	//����֡���ݲ�������ڶ�㣬������֡������
	class SENSORFRAME_API RawFrame 
	{
	public:
		RawFrame(unsigned int MaxPoints= MAX_POINTS_PERFRAME);	//ԭʼ����֡�Ĺ��캯��
		~RawFrame();											//ԭʼ����֡����������
		unsigned int GetMaxPointsInFrame();						//�õ�ÿ֡���ɴ洢����
		unsigned int GetPointsNumber();							//�õ���ǰ֡�����е�ĸ���
		bool SetPoint(int PointIndex, PointR &InPoint);			//���±�ΪPointIndex�ĵ㸳ֵInPoint,�����Ƿ�ֵ�ɹ�
		bool GetPoint(int PointIndex, PointR &OutPoint);		//ȡ���±�ΪPointIndex�ĵ�,�����Ƿ�ֵ�ɹ�
		bool TestAddPoint();									//����Frame���Ƿ�������ӵ�
		bool AddPoint(PointR &InPoint);							//����һ�㵽Frame�У�������һ
		//unsigned long long GetFrameIndex();					//�õ�֡���,0��ʾ֡û�г�ʼ��
		unsigned long long GetFileFrameIndex();					//�õ��ļ���ţ�����֮����0��ʾ�����ȡ
		bool SetFrameIndex(unsigned long long FileIndex=0);		//�޸�֡��ţ�ֻ����֡��ʼ����ʱ���޸�һ��,�����Ƿ��޸ĳɹ�

	private:
		void CreateFrame(unsigned int PointsNumber);			//����֡��ʱ�����
		void DestroyFrame();									//����֡��ʱ�����

		unsigned long long FileFrameIndex;						//��ǰ֡���ļ��е���ţ����ڲ��ŵ�ʱ����и��ֲ���
		//unsigned long long FrameIndex;						//��ǰ֡��ţ����ں��ڽ���֡�Ĵ�������
		bool isFrameIndexInited;								//֡��ŵ�������ȷ��֡��ֻ�޸�һ��
		unsigned int MaxPointsInFrame;							//ÿ֡���ɴ洢����
		unsigned int PointsNumber;								//��ǰ֡�����е�ĸ���(1,2,..)
		PointR *Points;											//�洢��
	};
}