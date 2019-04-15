#include "stdafx.h"
#include "RawFrame.h"

using namespace DataDefinition;

/**************************************************************************
*  @Function Name :: RawFrame
*  @Description   :: ԭʼ����֡�Ĺ��캯��
*  @Author        :: zzw
*  @Parameters    :: MaxPoints:ԭʼ����֡��������
*					 
*  @Return        :: None
**************************************************************************/
RawFrame::RawFrame(unsigned int MaxPoints)
{
	if (MaxPoints > 0) 
	{
		CreateFrame(MaxPoints);
	}
	else
	{
		Points = NULL;
		PointsNumber = 0;
		MaxPointsInFrame = 0;
	}
}

/**************************************************************************
*  @Function Name :: ~RawFrame
*  @Description   :: ԭʼ����֡����������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
RawFrame::~RawFrame()
{
	DestroyFrame();
}

/**************************************************************************
*  @Function Name :: TestAddPoint
*  @Description   :: �ж�ԭʼ����֡���Ƿ������ӵ�
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: ԭʼ����֡�л�������ӵ��򷵻�true,���򷵻�false
**************************************************************************/
bool RawFrame::TestAddPoint()
{
	if ((PointsNumber < MaxPointsInFrame)&&(NULL!=Points))
	{
		return true;
	}
	return false;
}

/**************************************************************************
*  @Function Name :: CreateFrame
*  @Description   :: ˽�г�Ա�������ڹ���֡ʱ����
*  @Author        :: zzw
*  @Parameters    :: MaxPoints:ԭʼ����֡��������
*
*  @Return        :: None
**************************************************************************/
void RawFrame::CreateFrame(unsigned int MaxPoints)
{
	Points = new PointR[MaxPoints];
	PointsNumber = 0;
	MaxPointsInFrame = MaxPoints;
	isFrameIndexInited = false;
}

/**************************************************************************
*  @Function Name :: DestroyFrame
*  @Description   :: ˽�г�Ա����,������֡ʱ����
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void RawFrame::DestroyFrame()
{
	if (Points)
	{
		delete[] Points;
		Points = NULL;
		PointsNumber = 0;
		MaxPointsInFrame = 0;
		isFrameIndexInited = false;
	}
}

/**************************************************************************
*  @Function Name :: AddPoint
*  @Description   :: ��һ�㵽ԭʼ����֡�У�������һ
*  @Author        :: zzw
*  @Parameters    :: InPoint:��ӽ�����ԭʼ���ݵ�
*
*  @Return        :: ��ӳɹ�����true,���ʧ�ܷ���false
**************************************************************************/
bool RawFrame::AddPoint(PointR &InPoint)
{
	if (PointsNumber >= MaxPointsInFrame)
	{
		return false;
	}
	if (SetPoint(PointsNumber, InPoint))
	{
		PointsNumber++;
		return true;
	}
	return false;
}

/**************************************************************************
*  @Function Name :: GetFileFrameIndex
*  @Description   :: �õ��ļ���ţ�����֮���飬0��ʾ�����ȡ֡
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: �����ļ���֡���
**************************************************************************/
unsigned long long RawFrame::GetFileFrameIndex()
{
	if (true == isFrameIndexInited)
	{
		return FileFrameIndex;
	}
	return MAXULONGLONG;
}


/**************************************************************************
*  @Function Name :: SetFrameIndex
*  @Description   :: ����֡��ţ������Ƿ����óɹ���֡���±��1��ʼ��0��������֡
*  @Author        :: zzw
*  @Parameters    :: FileIndex:�ļ�֡���
*					 
*  @Return        :: �����Ƿ����óɹ�
**************************************************************************/
bool RawFrame::SetFrameIndex(unsigned long long FileIndex)
{
	if (false == isFrameIndexInited)
	{
		FileFrameIndex = FileIndex;
		isFrameIndexInited = true;
		return true;
	}
	return false;
}

/**************************************************************************
*  @Function Name :: SetPoint
*  @Description   :: ��InPoint�ĵ��ֵ��ֵ���±�ΪPointIndex�ĵ�
*  @Author        :: zzw
*  @Parameters    :: PointIndex:��Ҫ�ӽ������±�
*					 InPoint:��Ҫ�ӽ����ĵ�
*  @Return        :: �����Ƿ���ӳɹ�
**************************************************************************/
bool RawFrame::SetPoint(int PointIndex, PointR &InPoint)
{
	//������±�Խ������ʱ������ʧ��
	if ((PointIndex >= MaxPointsInFrame) || (PointIndex < 0))
	{
		return false;
	}
	*(Points + PointIndex) = InPoint;
	return true;
}

/**************************************************************************
*  @Function Name :: GetPoint
*  @Description   :: ȡ���±�ΪPointIndex�ĵ�,�����Ƿ�ֵ�ɹ�
*  @Author        :: zzw
*  @Parameters    :: PointIndex:�ȴ�ȡ������±�
*					 OutPoint:��Ҫȡ���ĵ�
*  @Return        :: �����Ƿ�ȡ���ɹ�
**************************************************************************/
bool RawFrame::GetPoint(int PointIndex, PointR &OutPoint)
{
	//������±�Խ����Ч�������ʱ������ʧ��
	if ((PointIndex >= PointsNumber) || (PointIndex < 0))
	{
		return false;
	}
	OutPoint = *(Points + PointIndex);
	return true;
}

/**************************************************************************
*  @Function Name :: GetMaxPointsInFrame
*  @Description   :: �õ�ÿ֡���ɴ洢����
*  @Author        :: zzw
*  @Parameters    :: None
*					 
*  @Return        :: ����ÿ֡�ɴ洢��������
**************************************************************************/
unsigned int RawFrame::GetMaxPointsInFrame()
{
	return MaxPointsInFrame;
}

/**************************************************************************
*  @Function Name :: GetPointsNumber
*  @Description   :: �õ���ǰ֡�����е�ĸ���
*  @Author        :: zzw
*  @Parameters    :: None
*					
*  @Return        :: ���ص�ǰ֡�����еĵ���
**************************************************************************/
unsigned int RawFrame::GetPointsNumber()
{
	return PointsNumber;
}