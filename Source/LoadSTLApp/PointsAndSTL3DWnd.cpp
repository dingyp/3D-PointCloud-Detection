#include "stdafx.h"
#include "PointsAndSTL3DWnd.h"
#include "LoadSTLAppView.h"
#include "..\SceneGraph\SphereNode.h"
#include "..\SceneGraph\PathNode.h"
#include "MainFrm.h"

BEGIN_MESSAGE_MAP(PointsAndSTL3DWnd, Scene3DMessageWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()

/**************************************************************************
*  @Function Name :: Points3DWnd
*  @Description   :: Points3DWnd类的默认构造函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
PointsAndSTL3DWnd::PointsAndSTL3DWnd()
{
	m_pParentWnd = NULL;
	section_group = NULL;
	m_AxisNode = NULL;
	m_bLButtonDown = FALSE;
	m_bRButtonDown = FALSE;
	m_bCtrlDown = FALSE;

	m_rangex = m_rangey = m_rangez = 50.0;
	m_rangeTotal = 400.0;
	eyex = 0.0f;
	eyey = -86.60f;
	eyez = 50.0f;

	m_bCurProjMode = TRUE;

	m_MouseMode = MM_ROTAL;//默认在ctrl键之后是两点间的测量
	m_PointList.clear(); //清空

}

/**************************************************************************
*  @Function Name :: ~Points3DWnd
*  @Description   :: Points3DWnd类的默认析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
PointsAndSTL3DWnd::~PointsAndSTL3DWnd()
{
	m_PointList.clear(); //清空
}

//********************************************************
// 函数功能 ：创建3D窗口
// 入口参数 ：窗口位置和父窗口指针
//
// 返回参数 ：是否创建成功
// 创建日期 ： 2010-06-12 
//********************************************************
BOOL PointsAndSTL3DWnd::Create(CRect rect, CWnd *pParent, UINT idc)
{
	BOOL ret = Scene3DMessageWnd::Create(NULL, _T("Point3DWnd"), WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE, rect, pParent, idc);

	m_pParentWnd = pParent;

	InitScene();

	return ret;
}

/**************************************************************************
*  @Function Name :: InitScene
*  @Description   :: 初始化视野场景
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::InitScene()
{
	SceneGroup * root = new SceneGroup;
	section_group = new SceneGroup;
	section_group->setName("root");

	m_AxisNode = new RadarAxisNode(m_Font.GetTextListID());
	section_group->AddChild(m_AxisNode);

	m_STLGeometryNode = new STL_GeometryNode;
	section_group->AddChild(m_STLGeometryNode);

	m_RadarPointsNode = new RadarPointsNode;
	section_group->AddChild(m_RadarPointsNode);

	m_pRoomNode = new RoomGeometryNode;
	section_group->AddChild(m_pRoomNode);

	root->AddChild(section_group);

	this->SetSceneRoot(root);
	this->m_OpenGL3D.selectNode(section_group);
	this->m_OpenGL3D.setMouseInteraction(MOUSE_ROTATE);

	SetupWndView();
}

/**************************************************************************
*  @Function Name :: SetupWndView
*  @Description   :: 更新界面视图
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::SetupWndView()
{
	if (m_bCurProjMode)
	{
		this->SetWndProj(60.0f, m_fAspect, 0.1f, 10000.0f); //原始大小
		this->SetWndView(eyex*(m_rangeTotal / 100.0), eyey*(m_rangeTotal / 100.0), eyez*(m_rangeTotal / 100.0), 0.0, 0.0, 0.0, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		float WndCX = (float)max(m_rangex, m_rangey);
		float max = (float)max(WndCX, (float)m_rangez);
		float X = 1.1f*WndCX;
		this->SetWndProj(-X * m_fAspect, X*m_fAspect, -X, X, -10.0f*max, 10.0f*max); //原始大小
		this->SetWndView(0.0f, 0.0f, 2.0f*max, 0.0, 0.0, 0.0, 0.0f, 1.0f, 0.0f);
	}
}

/**************************************************************************
*  @Function Name :: ResetView
*  @Description   :: 重置视野的方向
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::ResetView()
{
	SetupWndView();

	//this->m_OpenGL3D.selectNode(section_group);
	this->m_OpenGL3D.m_pSelectedNode->ResetTransform();
	this->m_OpenGL3D.m_fScale = 1.0f;

	SetFocus();
	ReDraw();
}

/**************************************************************************
*  @Function Name :: GetDefaultBoundBox
*  @Description   :: 自动求出BoundBox并返回默认的size,方便重置网格
*  @Author        :: zzw
*  @Parameters    :: GridLength:网格的长度
*					 GridRadius:同心圆的半径
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::GetBoundBox(double &x1, double &x2, double &y1, double &y2, double &z1, double &z2)
{
	m_RadarPointsNode->GetDefaultBoundBox(x1, x2, y1, y2, z1, z2);
}

/**************************************************************************
*  @Function Name :: SetBoundBox
*  @Description   :: 设置显示界面的BoundBox
*  @Author        :: zzw
*  @Parameters    :: x1:BoundBox的x最小值 x2:BoundBox的x最大值 y1:BoundBox的y最小值
*					 y2:BoundBox的y最大值 z1:BoundBox的z最小值 z2:BoundBox的z最大值
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::SetBoundBox(double x1, double x2, double y1, double y2, double z1, double z2)
{
	m_rangex = x2 - x1;
	m_rangey = y2 - y1;
	m_rangez = z2 - z1;

	//找出6个数里面最大的数
	double xmax = max(fabs(x1), fabs(x2));
	double ymax = max(fabs(y1), fabs(y2));
	double zmax = max(fabs(z1), fabs(z2));
	double smax = max(fabs(xmax), fabs(ymax));
	smax = max(fabs(smax), fabs(zmax));
	m_rangeTotal = ceil(smax / 10) * 20;

	CString gridInfo;
	HWND hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView()->GetSafeHwnd();

	//调整坐标轴大小
	m_AxisNode->SetLineLength(ceil(smax / 10) * 2);
	//调整视野的大小
	if (m_bCurProjMode)
	{
		this->SetWndProj(60.0f, m_fAspect, 0.1f, 10000.0f); //原始大小
		this->SetWndView(eyex *(m_rangeTotal / 100.0), eyey *(m_rangeTotal / 100.0), eyez *(m_rangeTotal / 100.0), 0.0, 0.0, 0.0, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		float WndCX = (float)max(m_rangex, m_rangey);
		float max = (float)max(WndCX, (float)m_rangez);
		float X = 1.1f*WndCX;
		this->SetWndProj(-X * m_fAspect, X*m_fAspect, -X, X, -10.0f*max, 10.0f*max); //原始大小
		this->SetWndView(0.0f, 0.0f, 2.0f*max, 0.0, 0.0, 0.0, 0.0f, 1.0f, 0.0f);
	}
}

/**************************************************************************
*  @Function Name :: SetAxisNodeSize
*  @Description   :: 设置坐标轴的大小
*  @Author        :: zzw
*  @Parameters    :: setLength:坐标轴的长度
*
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::SetAxisNodeSize(double setLength)
{
	m_AxisNode->SetLineLength(setLength);
	//SetFocus();
	ReDraw();
}

/**************************************************************************
*  @Function Name :: SetSensorCenter
*  @Description   :: 设置传感器中心的坐标
*  @Author        :: zzw
*  @Parameters    :: sensor_x:传感器中心的x坐标 sensor_y:传感器中心的y坐标
*					 sensor_z:传感器中心的z坐标
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::SetSensorCenter(double sensor_x, double sensor_y, double sensor_z)
{
	transformer.SetSensorCenter(sensor_x, sensor_y, sensor_z);
}

/**************************************************************************
*  @Function Name :: SetSensorEuler
*  @Description   :: 设置传感器的姿态角
*  @Author        :: zzw
*  @Parameters    :: sensor_pitch:传感器x轴的旋转角度 sensor_roll:传感器y轴的旋转角度
*					 sensor_yaw:传感器z轴的旋转角度
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::SetSensorEuler(double sensor_pitch, double sensor_roll, double sensor_yaw)
{
	transformer.SetSensorEuler(sensor_pitch, sensor_roll, sensor_yaw);
}

/**************************************************************************
*  @Function Name :: GetSensorCenter
*  @Description   :: 获取传感器的中心点坐标
*  @Author        :: zzw
*  @Parameters    :: sensor_x:x轴中心点的坐标的引用 sensor_y:y轴中心点的坐标的引用
*					 sensor_z:z轴中心点的坐标的引用
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::GetSensorCenter(double &sensor_x, double &sensor_y, double &sensor_z)
{
	transformer.GetSensorCenter(sensor_x, sensor_y, sensor_z);
}

/**************************************************************************
*  @Function Name :: GetSensorEuler
*  @Description   :: 获取传感器的姿态角
*  @Author        :: zzw
*  @Parameters    :: sensor_pitch:传感器x轴姿态角的引用 sensor_roll:传感器y轴姿态角的引用
*					 sensor_yaw:传感器z轴姿态角的引用
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::GetSensorEuler(double &sensor_pitch, double &sensor_roll, double &sensor_yaw)
{
	transformer.GetSensorEuler(sensor_pitch, sensor_roll, sensor_yaw);
}

//在计算之后应该做的事情
void PointsAndSTL3DWnd::AfterRigidRegistration()
{

}

//添加点云数据
void PointsAndSTL3DWnd::AttachPointClouds(CPointClouds *pPointClouds)
{
	//在点加进来给PointsNode之前，先矫正传感器
	if (transformer.IsChangeBefore())
	{
		for (int i = 0; i < pPointClouds->m_uiNowCloudNums; i++)
		{
			transformer.ChangeBefore(*(pPointClouds->m_PointCloud[i]));
		}
	}
	////求取点云的平均帧，最大帧和最小帧
	//m_calcul_pointclouds.GetPointClouds(pPointClouds);//获取点云
	//m_calcul_pointclouds

	float xMin, xMax, yMin, yMax, zMin, zMax;
	xMax = yMax = zMax = FLT_MIN;
	xMin = yMin = zMin = FLT_MAX;
	if (true == pPointClouds->is_Calcul)
	{//只需要计算平均帧
		if (false==pPointClouds->GetArgPointCloud_BoundBox(xMin, xMax, yMin, yMax, zMin, zMax))
		{
			xMax = yMax = zMax = 10;
			xMin = yMin = zMin = -10;
		}
	}
	else
	{
		//通过最多获取前三帧，获取一个boudingBox
		int Total_Num = pPointClouds->m_uiNowCloudNums > 3 ? 3 : pPointClouds->m_uiNowCloudNums;
		for (int i = 0; i < Total_Num; i++)
		{
			float tmp_x1, tmp_x2, tmp_y1, tmp_y2, tmp_z1, tmp_z2;
			pPointClouds->GetOnePointCloud_BoundBox(pPointClouds->m_PointCloud[i], tmp_x1, tmp_x2, tmp_y1, tmp_y2, tmp_z1, tmp_z2);
			if (tmp_x1 < xMin)
			{
				xMin = tmp_x1;
			}
			if (tmp_x2 > xMax)
			{
				xMax = tmp_x2;
			}
			if (tmp_y1 < yMin)
			{
				yMin = tmp_y1;
			}
			if (tmp_y2 > yMax)
			{
				yMax = tmp_y2;
			}
			if (tmp_z1 < zMin)
			{
				zMin = tmp_z1;
			}
			if (tmp_z2 > zMax)
			{
				zMax = tmp_z2;
			}
		}
	}
	
	SetBoundBox(double(xMin), double(xMax), double(yMin), double(yMax), double(zMin), double(zMax));
	m_RadarPointsNode->AttachPointClouds(pPointClouds);
	ReDraw();
}

//添加STL数据list
void PointsAndSTL3DWnd::AttachDataList(CDataList * pDataList)
{
	if (NULL == pDataList)
	{
		return;
	}
	float xMin, xMax, yMin, yMax, zMin, zMax;
	pDataList->GetBoundBox(xMin, xMax, yMin, yMax, zMin, zMax);
	if ((FLT_MAX != xMin) && (FLT_MAX != yMin) && (FLT_MAX != zMin))//只有当得到的结果有意义才进行boundingbox的调整
	{
		SetBoundBox(double(xMin), double(xMax), double(yMin), double(yMax), double(zMin), double(zMax));
	}
	m_STLGeometryNode->AttachDataList(pDataList);
	ReDraw();
}

void PointsAndSTL3DWnd::AttachRoomModel(CModelList * pModellist)
{
	if (NULL == pModellist)
	{
		return;
	}
	float xMax, xMin, yMax, yMin, zMax, zMin;
	pModellist->GetBoundBox(xMin, xMax, yMin, yMax, zMin, zMax);
	if ((FLT_MAX != xMin) && (FLT_MAX != yMin) && (FLT_MAX != zMin))//只有当得到的结果有意义才进行boundingbox的调整
	{
		SetBoundBox(double(xMin), double(xMax), double(yMin), double(yMax), double(zMin), double(zMax));
	}
	m_pRoomNode->AttachRoomModel(pModellist);
	SetFocus();//因为这个显示之后好像没有了聚焦
	ReDraw();
}

//设置表面的绘制方式
void PointsAndSTL3DWnd::SetPolygonMode(GLenum Mode)
{
	m_STLGeometryNode->SetPolygonMode(Mode);
	ReDraw();
}


/**************************************************************************
*  @Function Name :: ClearData
*  @Description   :: 清除所有显示数据
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::ClearData()
{
	m_RadarPointsNode->ClearData();
	ReDraw();
}

/**************************************************************************
*  @Function Name :: GetValueRange
*  @Description   :: 获取点某一属性的最大最小值
*  @Author        :: zzw
*  @Parameters    :: MinValue:点帧某一属性的最小值的引用
*					 MaxValue:点帧某一属性的最大值的引用
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::GetValueRange(double &MinValue, double &MaxValue)
{
	m_RadarPointsNode->GetDataRange(MinValue, MaxValue);
}

/**************************************************************************
*  @Function Name :: SetValueRange
*  @Description   :: 设置点某一属性的最小值和最大值
*  @Author        :: zzw
*  @Parameters    :: MinValue:设置的最小值
*					 MaxValue:设置的最大值
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::SetValueRange(double MinValue, double MaxValue)
{
	m_RadarPointsNode->SetDataRange(MinValue, MaxValue);
	SetFocus();
	ReDraw();
}

/**************************************************************************
*  @Function Name :: OnLButtonDblClk
*  @Description   :: 鼠标左键双击的响应函数
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnLButtonDblClk参数
*					 point:OnLButtonDblClk参数
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	Scene3DMessageWnd::OnLButtonDblClk(nFlags, point);
}

/**************************************************************************
*  @Function Name :: OnLButtonDown
*  @Description   :: 鼠标左键单击击下的响应函数
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnLButtonDown参数
*					 point:OnLButtonDown参数,鼠标点击的坐标
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = TRUE;
	VECTOR vector;
	RadarPoint pos;
	int PointNum;
	int other_points = 16;//额外多的相同水平角的点（16通道）

	this->SendMessage(WM_IM_NROTATE_BEGIN, (WPARAM)&point);//现在单击鼠标左键只有旋转
}

/**************************************************************************
*  @Function Name :: OnLButtonUp
*  @Description   :: 鼠标左键单击回退的响应函数
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnLButtonUp参数
*					 point:OnLButtonUp参数
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	this->SendMessage(WM_IM_NROTATE_END, (WPARAM)&point);

	m_bLButtonDown = FALSE;
}

/**************************************************************************
*  @Function Name :: OnRButtonDown
*  @Description   :: 鼠标右键单击击下的响应函数
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnRButtonDown参数
*					 point:OnRButtonDown参数
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	this->SendMessage(WM_IM_TRANSLATE_BEGIN, (WPARAM)&point);

	m_bRButtonDown = TRUE;
}

/**************************************************************************
*  @Function Name :: OnRButtonUp
*  @Description   :: 鼠标右键单击回退的响应函数
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnRButtonUp参数
*					 point:OnRButtonUp参数
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	this->SendMessage(WM_IM_TRANSLATE_END, (WPARAM)&point);

	m_bRButtonDown = FALSE;
}

/**************************************************************************
*  @Function Name :: OnMouseMove
*  @Description   :: 鼠标移动的响应函数
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnMouseMove参数
*					 point:OnMouseMove参数
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;

	if (m_bLButtonDown && ((FALSE == m_bCtrlDown) || (MM_ROTAL == m_MouseMode)))
	{
		this->SendMessage(WM_IM_NROTATE_PROGRESS, (WPARAM)&point);
	}

	if (m_bRButtonDown)
	{
		this->SendMessage(WM_IM_TRANSLATE_PROGRESS, (WPARAM)&point);
	}
}

/**************************************************************************
*  @Function Name :: OnMouseWheel
*  @Description   :: 鼠标滚轮滑动的响应函数
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnMouseWheel参数 zDelta:OnMouseWheel参数
*					 pt:OnMouseWheel参数
*  @Return        :: 返回运行是否成功
**************************************************************************/
BOOL PointsAndSTL3DWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	this->SendMessage(WM_IM_SCALE_PROGRESS, (WPARAM)&pt, (LPARAM)zDelta);

	return Scene3DMessageWnd::OnMouseWheel(nFlags, zDelta, pt);
}

/**************************************************************************
*  @Function Name :: OnSize
*  @Description   :: 改变尺寸的响应函数
*  @Author        :: zzw
*  @Parameters    :: nType:OnSize参数 cx:OnSize参数
*					 cy:OnSize参数
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::OnSize(UINT nType, int cx, int cy)
{
	Scene3DMessageWnd::OnSize(nType, cx, cy);

	if (cx > 0 && cy > 0)
	{
		m_fAspect = (float)cx / (float)cy;
		SetupWndView();
	}
}

/**************************************************************************
*  @Function Name :: SetProjMode
*  @Description   :: 设置投影模式
*  @Author        :: zzw
*  @Parameters    :: bProjMode:设置的投影模式，TRUE--透视投影;FALSE--正交投影
*
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::SetProjMode(BOOL bProjMode)
{
	if (m_bCurProjMode == bProjMode)
		return;

	m_bCurProjMode = bProjMode;

	SetupWndView();

	ReDraw();
}

/**************************************************************************
*  @Function Name :: OnKeyDown
*  @Description   :: 键盘击下的响应函数
*  @Author        :: zzw
*  @Parameters    :: nChar:OnKeyDown参数 nRepCnt:OnKeyDown参数
*					 nFlags:OnKeyDown参数
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nFlags && MK_CONTROL)
	{
		m_bCtrlDown = TRUE;
	}

	Scene3DMessageWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

/**************************************************************************
*  @Function Name :: OnKeyUp
*  @Description   :: 键盘回退的响应函数
*  @Author        :: zzw
*  @Parameters    :: nChar:OnKeyUp参数 nRepCnt:OnKeyUp参数
*					 nFlags:OnKeyUp参数
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nFlags && MK_CONTROL)
	{
		m_bCtrlDown = FALSE;
	}

	Scene3DMessageWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}
