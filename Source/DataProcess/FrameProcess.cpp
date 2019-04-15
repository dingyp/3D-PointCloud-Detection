#include "stdafx.h"
#include "FrameProcess.h"

//��ԭʼ֡ת��������֡�����ݴ���
/********������������ݵ�ת��*********/
using namespace DataProcess;

/**************************************************************************
*  @Function Name :: FrameProcess
*  @Description   :: ԭʼ֡ת��������֡��Ĺ��캯�����������ݽ���Э��ĳ�ʼ��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
FrameProcess::FrameProcess()
{
	Conver_Protocol = LIDAR_INVALID;
	LiDAR_Channels = 0;
	ClearAnglesV();
	state = TRASFER_STOP;
	pRadarPointList = nullptr;
	FilteredPointCloud = nullptr;
	m_bIsFilter = true;
	RTAdress = NULL;
	TransferOneFrame = NULL;
	frame_list.clear();
	destoryTrasfer = true;//���û�п�ʼ�ǻ��˳���
	FileFrameIndex = 0;
}

/**************************************************************************
*  @Function Name :: ~FrameProcess
*  @Description   :: ԭʼ֡ת��������֡�����������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
FrameProcess::~FrameProcess()
{
	UnSetProtocol();
	Conver_Protocol = LIDAR_INVALID;
	LiDAR_Channels = 0;
	state = TRASFER_STOP;
	pRadarPointList = nullptr;
	FilteredPointCloud = nullptr;
	RTAdress = NULL;
	TransferOneFrame = NULL;
	frame_list.clear();
	destoryTrasfer = true;
	FileFrameIndex = 0;
}

/**************************************************************************
*  @Function Name :: SetProtocol
*  @Description   :: ����Э��
*  @Author        :: zzw
*  @Parameters    :: unsigned int Protocol:Э������
*
*  @Return        :: None
**************************************************************************/
void FrameProcess::SetProtocol(LiDAR_Protocol Protocol, bool IsFilter, unsigned int FilterFrameNumber)
{
	Conver_Protocol = Protocol;
	m_bIsFilter = IsFilter;
	m_pointCloudFilter.Init(Conver_Protocol, FilterFrameNumber);
	switch (Conver_Protocol)
	{
	case LIDAR_HLP16:
		LiDAR_Channels = 16;
		break;
	case LIDAR_2D:
		LiDAR_Channels = 1;
		break;
	}

	if (LoadAnglesV(Conver_Protocol))	//���ش�ֱ�Ƕ�����
	{
		state = TRASFER_START;			//ֻ�нǶ���ȷ�ˣ�״̬������Ϊ��ʼ
	}
}

/**************************************************************************
*  @Function Name :: UnSetProtocol
*  @Description   :: ȡ�����õ�Э��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FrameProcess::UnSetProtocol()
{
	m_pointCloudFilter.UnSetProtocol();
	Conver_Protocol = LIDAR_INVALID;
	ClearAnglesV();
	LiDAR_Channels = 0;
	state = TRASFER_STOP;
}

/**************************************************************************
*  @Function Name :: LoadAnglesV
*  @Description   :: ���ݲ�Ʒ�ͺż��ش�ֱ�Ƕ����ݣ������Ƕȣ����ȣ�Cos��Sinֵ
*  @Author        :: zzw
*  @Parameters    :: unsigned int Protocol:Э������
*
*  @Return        :: true:���سɹ���false������ʧ��
**************************************************************************/
bool FrameProcess::LoadAnglesV(LiDAR_Protocol uProtocol)
{
	if (LIDAR_INVALID == uProtocol)
	{
		return false;
	}
	ClearAnglesV();
	if (LIDAR_HLP16 == uProtocol)
	{
		for (UINT i = 0; i < LiDAR_Channels; i++)
		{
			if (i & 0x01)//����
				AnglesV_Deg.push_back(i);
			else
				AnglesV_Deg.push_back((int)i - 15);
		}
	}
	else if (LIDAR_2D == uProtocol)
	{
		AnglesV_Deg.push_back(double(0.0));
	}

	for (int i = 0; i < AnglesV_Deg.size(); i++)
	{
		AnglesV_Rad.push_back(Deg2Rad(AnglesV_Deg[i]));
		AnglesV_Cos.push_back(cos(AnglesV_Rad[i]));
		AnglesV_Sin.push_back(sin(AnglesV_Rad[i]));
	}
	return true;
}

/**************************************************************************
*  @Function Name :: ClearAnglesV
*  @Description   :: �����ֱ�Ƕ�����
*  @Author        :: hgb
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FrameProcess::ClearAnglesV()
{
	AnglesV_Deg.clear();
	AnglesV_Rad.clear();
	AnglesV_Cos.clear();
	AnglesV_Sin.clear();
}


/**************************************************************************
*  @Function Name :: AddFrame
*  @Description   :: ���һ֡��ת���ĵ����ݵ���ת��֡������
*  @Author        :: zzw
*  @Parameters    :: OneFrame:��ת����֡
*					 ����30֡���һ֡������֡
*  @Return        :: restframeʣ���֡��
**************************************************************************/
int FrameProcess::AddFrame(std::shared_ptr<RawFrame> &OneFrame)
{
	int restframe;
	mutex_frame_list.lock();
	switch (Conver_Protocol)
	{
	case LIDAR_HLP16:
		restframe = AddPcapFrame(OneFrame);
		break;
	case LIDAR_2D:
		restframe = AddPcapngFrame(OneFrame);
		break;
	}
	mutex_frame_list.unlock();
	return restframe;//ʣ��֡��
}

/**************************************************************************
*  @Function Name :: AddPcapFrame
*  @Description   :: ���һ֡��ת��pcap�����ݵ���ת��֡������
*  @Author        :: zzw
*  @Parameters    :: OneFrame:��ת����pcap֡
*					 ����30֡���һ֡������֡
*  @Return        :: ������Ӻ����֡��
**************************************************************************/
int FrameProcess::AddPcapFrame(std::shared_ptr<RawFrame> &OneFrame)
{
	frame_list.push_back(OneFrame);
	//��30֡���һ֡����֡
	if (frame_list.size() >= 30)
	{
		frame_list.begin() = frame_list.erase(frame_list.begin());//ɾ����һ��Ԫ��
		frame_list.begin() = frame_list.erase(frame_list.begin());//ɾ����һ��Ԫ��
	}
	return frame_list.size();
}

/**************************************************************************
*  @Function Name :: AddPcapngFrame
*  @Description   :: ���һ֡��ת��pcapng�����ݵ���ת��֡������
*  @Author        :: zzw
*  @Parameters    :: OneFrame:��ת����pcapng֡
*					 ����30֡���һ֡������֡
*  @Return        :: ������Ӻ����֡��
**************************************************************************/
int FrameProcess::AddPcapngFrame(std::shared_ptr<RawFrame> &OneFrame)
{
	frame_list.push_back(OneFrame);
	//��30֡���һ֡����֡
	if (frame_list.size() >= 30)
	{
		frame_list.begin() = frame_list.erase(frame_list.begin());//ɾ����һ��Ԫ��
		frame_list.begin() = frame_list.erase(frame_list.begin());//ɾ����һ��Ԫ��
	}
	return frame_list.size();
}

/**************************************************************************
*  @Function Name :: GetFrameNumber
*  @Description   :: ��ȡת�������е�֡��
*  @Author        :: zzw
*  @Parameters    :: None
*					
*  @Return        :: ת�������е�֡��
**************************************************************************/
int FrameProcess::GetFrameNumber()
{
	return frame_list.size();
}

/**************************************************************************
*  @Function Name :: BindTrasferOneFrameFunc
*  @Description   :: ��ת����һ֡�Ļص�����
*  @Author        :: zzw
*  @Parameters    :: Callback:��һ֡�Ļص�����
*					 pParam:�ص������е�ָ�����
*  @Return        :: None
**************************************************************************/
void FrameProcess::BindTrasferOneFrameFunc(void(*Callback)(std::shared_ptr<RadarPointList> &lParam, LPVOID pParam), LPVOID pParam)
{
	TransferOneFrame = Callback;
	RTAdress = pParam;
}

/**************************************************************************
*  @Function Name :: PointR2XYZ
*  @Description   :: ת��PointR��ʽΪXYZ��ʽ
*  @Author        :: hgb
*  @Parameters    :: pr:�������ݵ��ʽ
*                    x,y,z��������x,y,z����
*  @Return        :: None
**************************************************************************/
void FrameProcess::PointR2XYZ(const PointR &pr, double &x, double &y, double &z)
{
	double xy;
	if (LiDAR_Channels)
	{
		double fDistance = pr.distance / 1000.0; //����ת������
		unsigned int uChannel = pr.channel % LiDAR_Channels;

		z = (float)(fDistance * AnglesV_Sin[uChannel]);
		xy = (float)(fDistance * AnglesV_Cos[uChannel]);
		double angleH = pr.angleH / 100.0;

		y = (float)(xy * cos(Deg2Rad(angleH)));
		x = (float)(xy * sin(Deg2Rad(angleH)));
	}
}

/**************************************************************************
*  @Function Name :: SFrameToPFrame
*  @Description   :: ���״�����֡ת������������֡
*  @Author        :: zzw
*  @Parameters    :: OneFrame:��ת����ԭʼ֡
*                    
*  @Return        :: trueת���ɹ���falseת��ʧ��
**************************************************************************/
bool FrameProcess::SFrameToPFrame(std::shared_ptr<RawFrame> &OneFrame)
{
	//ȷ�Ͼ�֡�Ѿ�ת����
	if (nullptr != pRadarPointList)
	{
		return false;
	}

	//�жϴ�ת��֡�Ƿ�Ϊ��,Ϊ���򷵻�
	if (nullptr == OneFrame)
	{
		return false;
	}

	pRadarPointList = std::make_shared<RadarPointList>();
	if (nullptr == pRadarPointList)
	{//�����ڴ�ʧ��,����
		return false;
	}

	return PcapFrameToPFrame(OneFrame);

	//switch (Conver_Protocol)
	//{
	//case LIDAR_HLP16:
	//	return PcapFrameToPFrame(OneFrame);
	//	break;
	//case LIDAR_2D:
	//	return PcapngFrameToPFrame(OneFrame);
	//	break;
	//}
}

/**************************************************************************
*  @Function Name :: PcapFrameToPFrame
*  @Description   :: ��pcap֡ת������������֡
*  @Author        :: zzw
*  @Parameters    :: OneFrame:��ת����ԭʼ֡
*
*  @Return        :: trueת���ɹ���falseת��ʧ��
**************************************************************************/
bool FrameProcess::PcapFrameToPFrame(std::shared_ptr<RawFrame> &OneFrame)
{
	//��ȡCurrentFrameIndex֡�е������
	int TotalPointsNumber = OneFrame->GetPointsNumber();

	for (int i = 0; i < TotalPointsNumber; i++) {
		if (destoryTrasfer)
		{
			pRadarPointList = nullptr;
			return false;
		}
		PointR PointReader;
		if (!OneFrame->GetPoint(i, PointReader))
		{
			//��ȡ��ʧ��
			continue;
		}

		//����ԭʼ֡��Ϣ�Ե����ת��������ӵ���֡����
		RadarPoint CurPoint;
		CurPoint.Point_ID = i;
		CurPoint.distance = PointReader.distance;
		PointR2XYZ(PointReader, CurPoint.x, CurPoint.y, CurPoint.z);
		CurPoint.intensity = double(PointReader.intensity);
		CurPoint.angleH = PointReader.angleH;
		CurPoint.angleV = int(AnglesV_Deg[PointReader.channel%LiDAR_Channels]);
		CurPoint.channel = PointReader.channel%LiDAR_Channels;
		CurPoint.timestamp = PointReader.timestamp_ms;
		pRadarPointList->push_back(CurPoint);//�ӵ���֡��
	}
	return true;
}

/**************************************************************************
*  @Function Name :: PcapngFrameToPFrame
*  @Description   :: ��pcapng֡ת������������֡
*  @Author        :: zzw
*  @Parameters    :: OneFrame:��ת����ԭʼ֡
*
*  @Return        :: trueת���ɹ���falseת��ʧ��
**************************************************************************/
bool FrameProcess::PcapngFrameToPFrame(std::shared_ptr<RawFrame> &OneFrame)
{
	//��ȡCurrentFrameIndex֡�е������
	int TotalPointsNumber = OneFrame->GetPointsNumber();

	for (int i = 0; i < TotalPointsNumber; i++) 
	{
		if (destoryTrasfer)
		{
			pRadarPointList = nullptr;
			return false;
		}
		PointR PointReader;
		if (!OneFrame->GetPoint(i, PointReader))
		{
			//��ȡ��ʧ��
			continue;
		}

		//����ԭʼ֡��Ϣ�Ե����ת��������ӵ���֡����
		RadarPoint CurPoint;
		CurPoint.Point_ID = i;
		CurPoint.distance = PointReader.distance;
		PointR2XYZ(PointReader, CurPoint.x, CurPoint.y, CurPoint.z);
		CurPoint.intensity = double(PointReader.intensity);
		CurPoint.angleH = PointReader.angleH;
		CurPoint.angleV = int(AnglesV_Deg[PointReader.channel%LiDAR_Channels]);
		CurPoint.channel = PointReader.channel%LiDAR_Channels;
		CurPoint.timestamp = PointReader.timestamp_ms;
		pRadarPointList->push_back(CurPoint);//�ӵ���֡��
	}
	return false;
}

/**************************************************************************
*  @Function Name :: Start
*  @Description   :: ԭʼ֡ת���ɵ�֡�̺߳���
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FrameProcess::Start()
{
	if ((NULL == TransferOneFrame) || (NULL == RTAdress))
	{//��û�а󶨻ص�����������ʧ��
		return;
	}
	std::shared_ptr<RawFrame> OneFrame = NULL;
	destoryTrasfer = false;
	while (!destoryTrasfer)
	{
		OneFrame = NULL;
		mutex_frame_list.lock();
		if (frame_list.size()>0)
		{
			OneFrame = frame_list.front();
			frame_list.begin() = frame_list.erase(frame_list.begin());//ɾ����һ��Ԫ��

		}
		mutex_frame_list.unlock();
		if (NULL != OneFrame)
		{
			if (SFrameToPFrame(OneFrame))
			{//���ת���ɹ�
				FileFrameIndex = OneFrame->GetFileFrameIndex();
				if (false == m_bIsFilter)
				{//����Ҫ�˲�
					TransferOneFrame(pRadarPointList, RTAdress);
				}
				else
				{
					//�����˲�
					if (m_pointCloudFilter.FilterPointCloud(pRadarPointList, FilteredPointCloud, FileFrameIndex))//�˲��ɹ��Ž��з���,�����ļ�֡�ű����б�
					{
						TransferOneFrame(FilteredPointCloud, RTAdress);
					}
					FilteredPointCloud = nullptr;
				}
			}
			pRadarPointList = nullptr;//�ÿ�
			OneFrame = nullptr;
		}
		else
		{
			Sleep(0);
		}

		//����״̬���
		while ((TRASFER_STOP == state) && (!destoryTrasfer))//ֹͣ��ʱ��һֱ�ڴ˴�˯�ߣ�ֱ��״̬�ı�
		{//֡����������������Ҫ�Ƿ�ֹֹͣ��ʱ�򲻶�����֡����
			mutex_frame_list.lock();
			//��30֡���һ֡����֡
			if (frame_list.size() >= 30)
			{
				frame_list.begin() = frame_list.erase(frame_list.begin());//ɾ��һ��Ԫ��
				frame_list.begin() = frame_list.erase(frame_list.begin());//��ɾ��һ��Ԫ��
			}
			mutex_frame_list.unlock();
			Sleep(1);
		}
	}
}

//�����Ƿ�����˲�
void FrameProcess::Set_IsFilter(bool isFilter)
{
	m_bIsFilter = isFilter;
}

//��ȡ�Ƿ�����˲�
void FrameProcess::Get_IsFilter(bool &isFilter)
{
	isFilter = m_bIsFilter;
}

/**************************************************************************
*  @Function Name :: Stop
*  @Description   :: ��ԭʼ֡ת���ɵ�֡�̺߳�����ֹͣλ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FrameProcess::Stop()
{
	state = TRASFER_STOP;
}

/**************************************************************************
*  @Function Name :: Restart
*  @Description   :: �̺߳�����ֹ֮ͣ�����¿�ʼ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FrameProcess::Restart()
{
	state = TRASFER_START;
	if (0 == FileFrameIndex)//���һֱ��0,˵���Ǵ������������¿�ʼ
	{
		m_pointCloudFilter.ReStart();
	}
}

/**************************************************************************
*  @Function Name :: Destory_Trasfrom
*  @Description   :: �˳��̺߳����������߳�
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FrameProcess::Destory_Trasfrom()
{
	destoryTrasfer = true;
	RTAdress = NULL;
	TransferOneFrame = NULL;
}

/**************************************************************************
*  @Function Name :: ReTrasform
*  @Description   :: ����ת�������֮ǰ����
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void FrameProcess::ReTrasform()
{
	state = TRASFER_STOP;//��ֹͣ
	mutex_frame_list.lock();
	while (frame_list.size()>0)
	{
		frame_list.begin() = frame_list.erase(frame_list.begin());
	}
	mutex_frame_list.unlock();
	if (0 == FileFrameIndex)//���һֱ��0,˵���Ǵ������������¿�ʼ
	{
		m_pointCloudFilter.ReStart();
	}
	state = TRASFER_START;//���ָ�״̬
}

/**************************************************************************
*  @Function Name :: TrasfOneFrame
*  @Description   :: ����ת��һ֡
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
//void FrameProcess::TrasfOneFrame()
//{
//	if ((NULL == TransferOneFrame) || (NULL == RTAdress))
//	{//û�г�ʼ���ص�����������
//		return;
//	}
//	int tmpstate = state;
//	state = TRASFER_START;
//	std::shared_ptr<RawFrame> OneFrame = NULL;
//	mutex_frame_list.lock();
//	if (frame_list.size()>0)
//	{
//		OneFrame = frame_list.front();
//		frame_list.begin() = frame_list.erase(frame_list.begin());//ɾ����һ��Ԫ��
//	}
//	mutex_frame_list.unlock();
//	if (NULL != OneFrame)
//	{
//		if (SFrameToPFrame(OneFrame))
//		{//���ת���ɹ�
//			FileFrameIndex = OneFrame->GetFileFrameIndex();
//			TransferOneFrame(pRadarPointList, RTAdress);
//			pRadarPointList = nullptr;
//			OneFrame = nullptr;
//		}
//		else
//		{//ת��ʧ���򲻽��лص���������һ֡
//
//		}
//	}
//	//state = TRASFER_START;
//	state = tmpstate;
//}

/**************************************************************************
*  @Function Name :: GetFileFrameIndex
*  @Description   :: �õ���ǰ�ļ��е�֡����������в��ſ���
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
unsigned long long FrameProcess::GetFileFrameIndex()
{
	return FileFrameIndex;
}