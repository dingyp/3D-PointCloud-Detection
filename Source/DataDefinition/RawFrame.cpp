#include "stdafx.h"
#include "RawFrame.h"

using namespace DataDefinition;

/**************************************************************************
*  @Function Name :: RawFrame
*  @Description   :: 原始数据帧的构造函数
*  @Author        :: zzw
*  @Parameters    :: MaxPoints:原始数据帧的最大点数
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
*  @Description   :: 原始数据帧的析构函数
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
*  @Description   :: 判断原始数据帧中是否可以添加点
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 原始数据帧中还可以添加点则返回true,否则返回false
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
*  @Description   :: 私有成员方法，在构造帧时调用
*  @Author        :: zzw
*  @Parameters    :: MaxPoints:原始数据帧的最大点数
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
*  @Description   :: 私有成员方法,在销毁帧时调用
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
*  @Description   :: 加一点到原始数据帧中，点数加一
*  @Author        :: zzw
*  @Parameters    :: InPoint:添加进来的原始数据点
*
*  @Return        :: 添加成功返回true,添加失败返回false
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
*  @Description   :: 得到文件序号，便于之后检查，0表示网络读取帧
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 返回文件的帧序号
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
*  @Description   :: 设置帧序号，返回是否设置成功，帧号下标从1开始，0代表网络帧
*  @Author        :: zzw
*  @Parameters    :: FileIndex:文件帧序号
*					 
*  @Return        :: 返回是否设置成功
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
*  @Description   :: 将InPoint的点的值赋值给下标为PointIndex的点
*  @Author        :: zzw
*  @Parameters    :: PointIndex:需要加进来的下标
*					 InPoint:需要加进来的点
*  @Return        :: 返回是否添加成功
**************************************************************************/
bool RawFrame::SetPoint(int PointIndex, PointR &InPoint)
{
	//当点的下标越出两边时，返回失败
	if ((PointIndex >= MaxPointsInFrame) || (PointIndex < 0))
	{
		return false;
	}
	*(Points + PointIndex) = InPoint;
	return true;
}

/**************************************************************************
*  @Function Name :: GetPoint
*  @Description   :: 取出下标为PointIndex的点,返回是否赋值成功
*  @Author        :: zzw
*  @Parameters    :: PointIndex:等待取出点的下标
*					 OutPoint:需要取出的点
*  @Return        :: 返回是否取出成功
**************************************************************************/
bool RawFrame::GetPoint(int PointIndex, PointR &OutPoint)
{
	//当点的下标越出有效点的两边时，返回失败
	if ((PointIndex >= PointsNumber) || (PointIndex < 0))
	{
		return false;
	}
	OutPoint = *(Points + PointIndex);
	return true;
}

/**************************************************************************
*  @Function Name :: GetMaxPointsInFrame
*  @Description   :: 得到每帧最大可存储点数
*  @Author        :: zzw
*  @Parameters    :: None
*					 
*  @Return        :: 返回每帧可存储的最大点数
**************************************************************************/
unsigned int RawFrame::GetMaxPointsInFrame()
{
	return MaxPointsInFrame;
}

/**************************************************************************
*  @Function Name :: GetPointsNumber
*  @Description   :: 得到当前帧中已有点的个数
*  @Author        :: zzw
*  @Parameters    :: None
*					
*  @Return        :: 返回当前帧中已有的点数
**************************************************************************/
unsigned int RawFrame::GetPointsNumber()
{
	return PointsNumber;
}