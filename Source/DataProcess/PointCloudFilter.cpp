#include "stdafx.h"
#include "PointCloudFilter.h"

using namespace DataProcess;

PointCloudFilter::PointCloudFilter()
{
	Conver_Protocol = LIDAR_INVALID;
	LiDAR_Channels = 0;
	m_iFilterNumber = 0;
	m_TotalPointCloudFromBegin = 0;
	nextFileFrameIndex = 0;
	m_pFilteredPointCloud = nullptr;
	//myFilterList.clear();
	memset(pFilterList, 0, sizeof(AngleFilter *) * 128);
	Clearall_AngleFilter();
	filterStation = Filter_STOP;
}

PointCloudFilter::~PointCloudFilter()
{
	mutex_Filter.lock();
	for (int i = 0; i < LiDAR_Channels; i++)
	{
		if (pFilterList[i] != NULL)
		{
			delete(pFilterList[i]);
		}
	}
	LiDAR_Channels = 0;
	mutex_Filter.unlock();
	m_iFilterNumber = 0;
	m_TotalPointCloudFromBegin = 0;
	nextFileFrameIndex = 0;
}

//设置协议
void PointCloudFilter::Init(LiDAR_Protocol Protocol, unsigned int FilterNumber)
{
	Conver_Protocol = Protocol;
	if (FilterNumber <= 0)
	{
		return;
	}
	m_iFilterNumber = (FilterNumber>10 ? 10 : FilterNumber);

	switch (Conver_Protocol)
	{
	case LIDAR_HLP16:
		LiDAR_Channels = 16;
		break;
	case LIDAR_2D:
		LiDAR_Channels = 1;
		break;
	default:
		return;
	}

	////std::shared_ptr<AngleFilter> tmp_pAngleFilter = nullptr;
	//for (int i = 0; i < LiDAR_Channels; i++)
	//{
	//	pFilterList[i] = new AngleFilter;
	//	//tmp_pAngleFilter = std::make_shared<AngleFilter>();
	//	for (int j = 0; j < 2000; j++)
	//	{
	//		for (int n = 0; n < m_iFilterNumber; n++)
	//		{
	//			tmp_pAngleFilter->OneAngle[j][n] = 0;
	//		}
	//		tmp_pAngleFilter->sign[j] = -1;
	//	}
	//	//myFilterList.push_back(tmp_pAngleFilter);
	//}
	

	for (int i = 0; i < LiDAR_Channels; i++)
	{
		pFilterList[i] = new AngleFilter;
		for (int j = 0; j < 2000; j++)
		{
			for (int n = 0; n < m_iFilterNumber; n++)
			{
				pFilterList[i]->OneAngle[j][n] = 0;
			}
			pFilterList[i]->sign[j] = -1;
		}
		//myFilterList.push_back(tmp_pAngleFilter);
	}

	filterStation = Filter_START;
}

//取消协议的设置
void PointCloudFilter::UnSetProtocol()
{
	filterStation = Filter_STOP;
	Conver_Protocol = LIDAR_INVALID;
	//std::shared_ptr<AngleFilter> tmp_pAngleFilter = nullptr;
	mutex_Filter.lock();
	//for (int i = 0; i < myFilterList.size(); i++)
	//{
	//	myFilterList[i] = nullptr;
	//}
	//myFilterList.clear();

	for (int i = 0; i < LiDAR_Channels; i++)
	{
		if (pFilterList[i] != NULL)
		{
			delete(pFilterList[i]);
		}
	}
	LiDAR_Channels = 0;
	mutex_Filter.unlock();
	PROCESS_DEBUG(_T("FilterList已经释放"));
	m_iFilterNumber = 0;
}

//清除所有角度积累
void PointCloudFilter::Clearall_AngleFilter()
{
	mutex_Filter.lock();
	//for (int i = 0; i < myFilterList.size(); i++)
	//{
	//	for (int j = 0; j < 2000; j++)
	//	{
	//		//清除角度和sign
	//		for (int n = 0; n < m_iFilterNumber; n++)
	//		{
	//			myFilterList[i]->OneAngle[j][n] = 0;
	//		}
	//		myFilterList[i]->sign[j] = -1;
	//	}
	//}

	for (int i = 0; i < LiDAR_Channels; i++)
	{
		if (NULL!= pFilterList[i])
		{
			for (int j = 0; j < 2000; j++)
			{
				//清除角度和sign
				for (int n = 0; n < m_iFilterNumber; n++)
				{
					pFilterList[i]->OneAngle[j][n] = 0;
				}
				pFilterList[i]->sign[j] = -1;
			}
		}
	}
	mutex_Filter.unlock();
}

//进行多帧的滤波
bool PointCloudFilter::FilterPointCloud(PointCloud_Ptr nofilteredPointCloud, PointCloud_Ptr &filteredPointCloud,int FileFrameIndex)
{
	if (Filter_STOP == filterStation)
	{
		return false;
	}
	//旧的滤波帧还没有滤波完成
	if (nullptr!= filteredPointCloud)
	{
		return false;
	}
	//待转换帧为空，返回滤波失败
	if (nullptr == nofilteredPointCloud)
	{
		return false;
	}
	filteredPointCloud = std::make_shared<RadarPointList>();
	//申请内存失败，返回失败
	if (nullptr == filteredPointCloud)
	{
		return false;
	}

	if (LiDAR_Channels <= 0)
	{
		return false;
	}

	if (0== m_iFilterNumber)
	{
		return false;
	}
	int PointNum = nofilteredPointCloud->size();
	UINT channelIdx = 0;
	UINT angleIdx = 0;

	for (int i = 0; i < LiDAR_Channels; i++)
	{
		for (int j = 0; j < 2000; j++)
		{
			pFilterList[i]->b_isChange[j] = false;
		}
	}

	if (abs(nextFileFrameIndex - FileFrameIndex) >= m_iFilterNumber)
	{
		m_TotalPointCloudFromBegin = 0;
	}
	nextFileFrameIndex = FileFrameIndex;
	m_TotalPointCloudFromBegin++;
	for (int i = 0; (i < PointNum)&&(Filter_START ==filterStation); i++)
	{
		AngleFilter* oneChannel = NULL;
		channelIdx = UINT((*nofilteredPointCloud)[i].channel);
		angleIdx = UINT((*nofilteredPointCloud)[i].angleH / 20);

		mutex_Filter.lock();
		if (LiDAR_Channels > 0)
		{
			if (NULL == pFilterList[channelIdx])
			{
				mutex_Filter.unlock();
				return false;
			}
		}
		else
		{
			mutex_Filter.unlock();
			return false;
		}
		oneChannel = pFilterList[channelIdx];
		if (angleIdx >= 2000)
		{
			angleIdx = 1999;
		}
		if (false == oneChannel->b_isChange[angleIdx])
		{
			oneChannel->sign[angleIdx] = (oneChannel->sign[angleIdx] + 1) % m_iFilterNumber;
			oneChannel->b_isChange[angleIdx] = true;
		}
		oneChannel->OneAngle[angleIdx][oneChannel->sign[angleIdx]] = (*nofilteredPointCloud)[i].distance;

		//进行第几帧判断，如果是一开始几帧，则不进行滤波,直接进行输出
		if (m_TotalPointCloudFromBegin <= m_iFilterNumber)
		{//直接加入这个点
			filteredPointCloud->push_back((*nofilteredPointCloud)[i]);
			mutex_Filter.unlock();
			continue;
		}


		//开始滤波，求出方差进行滤波
		double ArgDistance = 0;
		for (int j = 0; j < m_iFilterNumber; j++)
		{
			ArgDistance += oneChannel->OneAngle[angleIdx][j] * 1.0 / m_iFilterNumber;
		}
		double set_var = 0; 
		for (int j = 0; j < m_iFilterNumber; j++)
		{
			set_var += (oneChannel->OneAngle[angleIdx][j] - ArgDistance)*(oneChannel->OneAngle[angleIdx][j] - ArgDistance)/ m_iFilterNumber;
		}
		if (set_var < 2000)
		{
			unsigned int disMin = 0;
			unsigned int disMax = 0;
			disMin = disMax = oneChannel->OneAngle[angleIdx][0];
			for (int j = 1; j < m_iFilterNumber; j++)
			{
				if (oneChannel->OneAngle[angleIdx][j] < disMin)
				{
					disMin = oneChannel->OneAngle[angleIdx][j];
				}
				else if (oneChannel->OneAngle[angleIdx][j] > disMax)
				{
					disMax = oneChannel->OneAngle[angleIdx][j];
				}
			}
			if ((disMax - disMin)*1.0 / disMin < 0.1)
			{
				filteredPointCloud->push_back((*nofilteredPointCloud)[i]);//如果点满足滤波的条件，则加入该点
			} 
			//filteredPointCloud->push_back((*nofilteredPointCloud)[i]);//如果点满足滤波的条件，则加入该点
		}

		//unsigned int disMin = 0;
		//unsigned int disMax = 0;
		//disMin = disMax = oneChannel->OneAngle[angleIdx][0];
		//for (int j = 1; j < m_iFilterNumber; j++)
		//{
		//	if (oneChannel->OneAngle[angleIdx][j] < disMin)
		//	{
		//		disMin = oneChannel->OneAngle[angleIdx][j];
		//	}
		//	else if (oneChannel->OneAngle[angleIdx][j] > disMax)
		//	{
		//		disMax = oneChannel->OneAngle[angleIdx][j];
		//	}
		//}
		//if ((disMax - disMin)*1.0 / disMin < 0.1)
		//{
		//	filteredPointCloud->push_back((*nofilteredPointCloud)[i]);//如果点满足滤波的条件，则加入该点
		//}

		mutex_Filter.unlock();

	}
	if (filterStation != Filter_START)
	{
		return false;
	}
	channelIdx = 0;
	angleIdx = 0;
	return true;
}

//重新开始滤波
void PointCloudFilter::ReStart()
{
	m_TotalPointCloudFromBegin = 0;
}
