#include "stdafx.h"
#include "Points3DWnd.h"
#include "RadarViewView.h"
#include "..\SceneGraph\SphereNode.h"
#include "..\SceneGraph\PathNode.h"
#include "MeasureLineNode.h"
#include "MainFrm.h"

#define ID_HAND MAKEINTRESOURCE(32649)//ID_HAND是VC中的一个资源

BEGIN_MESSAGE_MAP(Points3DWnd, Scene3DMessageWnd)
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
Points3DWnd::Points3DWnd()
{
	m_pParentWnd = NULL;
	section_group = NULL;
	m_BaseLineNode = NULL;
	m_AxisNode = NULL;
	m_bLButtonDown = FALSE;
	m_bRButtonDown = FALSE;
	m_bCtrlDown = FALSE;

	m_rangex = m_rangey = m_rangez = 10.0;
	eyex = 0.0f;
	eyey = -86.60f;
	eyez = 50.0f;

	m_bCurProjMode = TRUE;
	m_bGridType = GRID;

	m_MouseMode = MM_ROTAL;//默认在ctrl键之后是两点间的测量
	m_PointList.clear(); //清空
	m_DblClkPointIdx = -1;//一开始没有点

	m_GridX = m_GridY = m_GridZ = 0.0;
	m_GridLength=200.00;//网格一些属性设置
	m_GridResolution=20;
	m_GridRadius=100.00;
	m_GridConCircle=10;

	m_GridWidth = 1.0;
	m_GridColor.x = 0.5;
	m_GridColor.y = 0.5;
	m_GridColor.z = 0.5;

}

/**************************************************************************
*  @Function Name :: ~Points3DWnd
*  @Description   :: Points3DWnd类的默认析构函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
Points3DWnd::~Points3DWnd()
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
BOOL Points3DWnd::Create(CRect rect, CWnd *pParent, UINT idc)
{
	BOOL ret = Scene3DMessageWnd::Create(NULL, _T("Point3DWnd"), WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE , rect, pParent, idc);

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
void Points3DWnd::InitScene()
{
	SceneGroup * root = new SceneGroup;
	section_group = new SceneGroup;
	section_group->setName("root");

	m_BaseLineNode = new BaseLinesNode;
	m_BaseLineNode->SetLineParameters(200.0, 10.0);
	m_BaseLineNode->SetLinePlane(XOY);
	m_BaseLineNode->SetLineDraw(GRID);
	section_group->AddChild(m_BaseLineNode);

	m_AxisNode = new RadarAxisNode(m_Font.GetTextListID());
	section_group->AddChild(m_AxisNode);

	m_RadarPointsNode = new RadarPointsNode;
	section_group->AddChild(m_RadarPointsNode);
	root->AddChild(section_group);

	this->SetSceneRoot(root);
	this->m_OpenGL3D.selectNode(section_group);
	this->m_OpenGL3D.setMouseInteraction(MOUSE_ROTATE);

	SetupWndView();
}

/**************************************************************************
*  @Function Name :: GetGirdType
*  @Description   :: 获取网格类型
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 返回网格类型
**************************************************************************/
LINEDRAW_TYPE Points3DWnd::GetGirdType()
{
	return m_bGridType;
}

/**************************************************************************
*  @Function Name :: SetGridType
*  @Description   :: 设置网格类型
*  @Author        :: zzw
*  @Parameters    :: eLineDrawType:设置的网格类型
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetGridType(LINEDRAW_TYPE eLineDrawType)
{
	m_bGridType = eLineDrawType;
	m_BaseLineNode->SetLineDraw(m_bGridType);
	if (GRID == m_bGridType)
	{
		m_BaseLineNode->SetLineParameters(m_GridLength, m_GridLength / m_GridResolution);
	}
	else if (CON_CIRCLE == m_bGridType)
	{
		m_BaseLineNode->SetLineParameters(m_GridRadius * 2, m_GridRadius / m_GridConCircle);
	}
	SetFocus();
	ReDraw();
}

/**************************************************************************
*  @Function Name :: SetGridCenter
*  @Description   :: 设置网格中心点
*  @Author        :: zzw
*  @Parameters    :: addx:网格中心点的x坐标 addy:网格中心点的y坐标
*					 addz:网格中心点的z坐标
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetGridCenter(float addx, float addy, float addz)
{
	m_GridX = addx;
	m_GridY = addy;
	m_GridZ = addz;
	m_BaseLineNode->SetGridCenter(m_GridX, m_GridY, m_GridZ);
}

/**************************************************************************
*  @Function Name :: SetGridThickness
*  @Description   :: 设置网线粗细
*  @Author        :: zzw
*  @Parameters    :: LineWidth:设置网格线的粗细
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetGridThickness(float LineWidth)
{
	m_GridWidth = LineWidth;
	m_BaseLineNode->SetLineWidth(m_GridWidth);
}

/**************************************************************************
*  @Function Name :: SetGridColor
*  @Description   :: 设置网格颜色,里面的颜色是需要进行归一化
*  @Author        :: zzw
*  @Parameters    :: gridColor:网格线的RGB值
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetGridColor(VECTOR gridColor)
{
	m_GridColor.x = gridColor.x;
	m_GridColor.y = gridColor.y;
	m_GridColor.z = gridColor.z;
	m_BaseLineNode->SetGridColor(m_GridColor);
}

/**************************************************************************
*  @Function Name :: SetGridParameters
*  @Description   :: 设置网格的参数
*  @Author        :: zzw
*  @Parameters    :: LineDistance:网格的长度
*					 LineResolution:每一边分成的网格数
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetGridParameters(float LineDistance, int LineResolution)
{
	m_GridLength = LineDistance;
	m_GridResolution = LineResolution;
	if (GRID == m_bGridType)
	{
		m_BaseLineNode->SetLineParameters(m_GridLength, m_GridLength / m_GridResolution);
	}
	SetFocus();
	ReDraw();
}

/**************************************************************************
*  @Function Name :: GetGridParameters
*  @Description   :: 获取网格的参数
*  @Author        :: zzw
*  @Parameters    :: LineDistance:网格长度的引用
*					 LineResolution:每一边分割网格数的引用
*  @Return        :: None
**************************************************************************/
void Points3DWnd::GetGridParameters(float &LineDistance, int &LineResolution)
{
	LineDistance = m_GridLength;
	LineResolution = m_GridResolution;
}

/**************************************************************************
*  @Function Name :: SetCircleParameters
*  @Description   :: 设置同心圆的参数
*  @Author        :: zzw
*  @Parameters    :: LineDistance:最外层同心圆的直径
*					 Circlenumber:同心圆的个数
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetCircleParameters(float LineDistance, int Circlenumber)
{
	m_GridRadius = LineDistance;
	m_GridConCircle = Circlenumber;
	if (CON_CIRCLE == m_bGridType)
	{
		m_BaseLineNode->SetLineParameters(m_GridRadius * 2, m_GridRadius / m_GridConCircle);
	}
	SetFocus();
	ReDraw();
}

/**************************************************************************
*  @Function Name :: GetCircleParameters
*  @Description   :: 获取同心圆的参数
*  @Author        :: zzw
*  @Parameters    :: LineDistance:最外层同心圆的直径的引用
*					 Circlenumber:同心圆个数的引用
*  @Return        :: None
**************************************************************************/
void Points3DWnd::GetCircleParameters(float &LineDistance, int &Circlenumber)
{
	LineDistance = m_GridRadius;
	Circlenumber = m_GridConCircle;
}

/**************************************************************************
*  @Function Name :: SetGridDir
*  @Description   :: 设置网格朝向
*  @Author        :: zzw
*  @Parameters    :: eLinePlaneType:网格的朝向
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetGridDir(LINEPLANE_TYPE eLinePlaneType)
{
	m_BaseLineNode->SetLinePlane(eLinePlaneType);
	SetFocus();
	ReDraw();
}

/**************************************************************************
*  @Function Name :: SetDir
*  @Description   :: 设置视野的方向
*  @Author        :: zzw
*  @Parameters    :: viewDir:视野的方向
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetDir(VIEW_DIR viewDir)
{
	this->m_OpenGL3D.m_fScale = 1.0f;
	section_group->ResetTransform();


	switch (viewDir)
	{
	case XP: //+X方向
		section_group->Rotate(-90, 0.0, 1.0, 0.0);
		section_group->Rotate(-90, 1.0, 0.0, 0.0);
		break;
	case XS://-X方向
		section_group->Rotate(90, 0.0, 1.0, 0.0);
		section_group->Rotate(90, -1.0, 0.0, 0.0);
		break;
	case YP://+Y方向
		section_group->Rotate(90, 1.0, 0.0, 0.0);
		section_group->Rotate(180, 0.0, 1.0, 0.0);
		break;
	case YS://-Y方向
		section_group->Rotate(-90, 1.0, 0.0, 0.0);
		break;
	case ZP://+Z方向
		break;
	case ZS://-Z方向
		section_group->Rotate(180, 0.0, 1.0, 0.0);
		section_group->Rotate(180, 0.0, 0.0, 1.0);
		break;
	default:
		break;
	}

	eyex = eyey  = 0.0f;
	eyez = 200.0f;
	SetupWndView();
	SetFocus();
	ReDraw();
}

/**************************************************************************
*  @Function Name :: SetupWndView
*  @Description   :: 更新界面视图
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetupWndView()
{
	if (m_bCurProjMode)
	{
		this->SetWndProj(60.0f, m_fAspect, 0.1f, 10000.0f); //原始大小
		this->SetWndView(eyex* (m_GridLength / 200.0), eyey* (m_GridLength / 200.0), eyez* (m_GridLength / 200.0), 0.0, 0.0, 0.0, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		float WndCX = (float)(m_rangex> m_rangey?m_rangex:m_rangey);
		float max = (float)(WndCX>(float)m_rangez?WndCX:(float)m_rangez);
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
void Points3DWnd::ResetView()
{
	eyex = 0.0f;
	eyey = -86.60f;
	eyez = 50.0f;
	SetupWndView();

	this->m_OpenGL3D.m_pSelectedNode->ResetTransform();
	this->m_OpenGL3D.m_fScale = 1.0f;

	SetFocus();
	ReDraw();
}

/**************************************************************************
*  @Function Name :: SetColorMode
*  @Description   :: 设置颜色编码的模式
*  @Author        :: zzw
*  @Parameters    :: set_ColorMode:编码模式的ID号
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetColorMode(int set_ColorMode)
{
	m_RadarPointsNode->ChangeColorMode(set_ColorMode);
	SetFocus();
	ReDraw();
}

/**************************************************************************
*  @Function Name :: GetColorMode
*  @Description   :: 获取颜色编码的ID号
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: 返回颜色编码的ID号
**************************************************************************/
int Points3DWnd::GetColorMode()
{
	return m_RadarPointsNode->GetColorMode();
}

/**************************************************************************
*  @Function Name :: ReScaleColorRange
*  @Description   :: 重新测量数值的范围并对颜色重新进行编码
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::ReScaleColorRange()
{
	m_RadarPointsNode->ReScaleColorRange();
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
void Points3DWnd::GetDefaultBoundBox(double &GridLength, double &GridRadius)
{
	m_RadarPointsNode->GetDefaultBoundBox(GridLength, GridRadius);
}

/**************************************************************************
*  @Function Name :: SetBoundBox
*  @Description   :: 设置显示界面的BoundBox
*  @Author        :: zzw
*  @Parameters    :: x1:BoundBox的x最小值 x2:BoundBox的x最大值 y1:BoundBox的y最小值
*					 y2:BoundBox的y最大值 z1:BoundBox的z最小值 z2:BoundBox的z最大值
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetBoundBox(double x1, double x2, double y1, double y2, double z1, double z2)
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
	m_GridLength = ceil(smax / 10) * 20;
	m_GridRadius = ceil(smax / 10) * 10;

	m_BaseLineNode->SetLineDraw(m_bGridType);
	CString gridInfo;
	HWND hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView()->GetSafeHwnd();
	if (GRID == m_bGridType)//网格的种类
	{
		m_BaseLineNode->SetLineParameters(m_GridLength, m_GridLength / m_GridResolution);

		//将点的信息传递给状态栏
		gridInfo.Format(_T("栅格边长:%0.2f m"), m_GridLength / m_GridResolution);
		::SendMessage(hwnd, MY_MSG_ChangStatusBar, 3, (LPARAM)LPCTSTR(gridInfo));
	}
	else if (CON_CIRCLE == m_bGridType)
	{
		m_BaseLineNode->SetLineParameters(m_GridRadius * 2, m_GridRadius / m_GridConCircle);

		//将点的信息传递给状态栏
		gridInfo.Format(_T("同心圆半径:%0.2f m"), m_GridRadius / m_GridConCircle);
		::SendMessage(hwnd, MY_MSG_ChangStatusBar, 3, (LPARAM)LPCTSTR(gridInfo));
	}

	//调整视野的大小
	if (m_bCurProjMode)
	{
		this->SetWndProj(60.0f, m_fAspect, 0.1f, 10000.0f); //原始大小
		this->SetWndView(eyex* (m_GridLength / 200.0), eyey * (m_GridLength / 200.0), eyez * (m_GridLength / 200.0), 0.0, 0.0, 0.0, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		float WndCX = (float)(m_rangex> m_rangey?m_rangex:m_rangey);
		float max = (float)(WndCX> (float)m_rangez?WndCX:(float)m_rangez);
		float X = 1.1f*WndCX;
		this->SetWndProj(-X * m_fAspect, X*m_fAspect, -X, X, -10.0f*max, 10.0f*max); //原始大小
		this->SetWndView(0.0f, 0.0f, 2.0f*max, 0.0, 0.0, 0.0, 0.0f, 1.0f, 0.0f);
	}
	//调整坐标轴大小
	SetAxisNodeSize(m_GridLength / m_GridResolution * 2);
}

/**************************************************************************
*  @Function Name :: SetAxisNodeSize
*  @Description   :: 设置坐标轴的大小
*  @Author        :: zzw
*  @Parameters    :: setLength:坐标轴的长度
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetAxisNodeSize(double setLength)
{
	m_AxisNode->SetLineLength(setLength);
	SetFocus();
	ReDraw();
}

/**************************************************************************
*  @Function Name :: SetAxisNodeIsShow
*  @Description   :: 设置坐标轴是否显示
*  @Author        :: zzw
*  @Parameters    :: setIsDraw:坐标轴是否显示标志位
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetAxisNodeIsShow(bool setIsDraw)
{
	m_AxisNode->SetIsShowed(setIsDraw);
}

/**************************************************************************
*  @Function Name :: GetAxisNodeIsShow
*  @Description   :: 获取坐标轴是否显示标志位
*  @Author        :: zzw
*  @Parameters    :: setIsDraw:坐标轴是否显示标志位
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::GetAxisNodeIsShow(bool &setIsDraw)
{
	m_AxisNode->GetIsShowed(setIsDraw);
}

/**************************************************************************
*  @Function Name :: SetSensorCenter
*  @Description   :: 设置传感器中心的坐标
*  @Author        :: zzw
*  @Parameters    :: sensor_x:传感器中心的x坐标 sensor_y:传感器中心的y坐标
*					 sensor_z:传感器中心的z坐标
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetSensorCenter(double sensor_x, double sensor_y, double sensor_z)
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
void Points3DWnd::SetSensorEuler(double sensor_pitch, double sensor_roll, double sensor_yaw)
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
void Points3DWnd::GetSensorCenter(double &sensor_x, double &sensor_y, double &sensor_z)
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
void Points3DWnd::GetSensorEuler(double &sensor_pitch, double &sensor_roll, double &sensor_yaw)
{
	transformer.GetSensorEuler(sensor_pitch, sensor_roll, sensor_yaw);
}

/**************************************************************************
*  @Function Name :: AttachData
*  @Description   :: 更新显示的点帧
*  @Author        :: zzw
*  @Parameters    :: PointList:更新的点的队列
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::AttachData(RadarPointList &PointList)
{
	//在点加进来给PointsNode之前，先矫正传感器
	if (transformer.IsChangeBefore())
	{
		transformer.ChangeBefore(PointList);
	}

	m_RadarPointsNode->AttachPointsData(PointList);
	this->m_OpenGL3D.setClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	ReDraw();
	return;
}

/**************************************************************************
*  @Function Name :: GetPointListData
*  @Description   :: 得到显示的点帧
*  @Author        :: zzw
*  @Parameters    :: PointList:正在显示的点帧的引用
*
*  @Return        :: 返回是否获取成功
**************************************************************************/
bool Points3DWnd::GetPointListData(RadarPointList &PointList)
{
	return m_RadarPointsNode->GetPointListData(PointList);
}

/**************************************************************************
*  @Function Name :: GetColorCoding
*  @Description   :: 获取颜色编码的属性
*  @Author        :: zzw
*  @Parameters    :: ColorCoding:颜色编码属性的引用
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::GetColorCoding(COLORCODING &ColorCoding)
{
	m_RadarPointsNode->GetColorCoding(ColorCoding);
	return;
}

/**************************************************************************
*  @Function Name :: SetColorCoding
*  @Description   :: 设置颜色编码的属性
*  @Author        :: zzw
*  @Parameters    :: chooseColorCoding:设置的颜色编码
*					 bUpdate:表示更改颜色编码属性之后是否redraw
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetColorCoding(COLORCODING chooseColorCoding, bool bUpdate)
{
	m_RadarPointsNode->SetColorCoding(chooseColorCoding);
	SetFocus();
	if (bUpdate)
	{
		ReDraw();
	}
	return;
}

/**************************************************************************
*  @Function Name :: RefreshData
*  @Description   :: 变换显示帧的数据
*  @Author        :: zzw
*  @Parameters    :: addX:X轴方向移动的距离 addY:Y轴方向移动的距离 addZ:Z轴方向移动的距离
*					 pitch:X轴方向旋转的角度 pitch:Y轴方向旋转的角度 pitch:Z轴方向旋转的角度
*  @Return        :: None
**************************************************************************/
void Points3DWnd::RefreshData(double addX, double addY, double addZ, double pitch, double roll, double yaw)
{
	m_RadarPointsNode->RefreshPointData(addX, addY, addZ, pitch, roll, yaw, transformer);
	SetFocus();
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
void Points3DWnd::ClearData()
{
	m_RadarPointsNode->ClearData();
	ClearAllMark();
	ReDraw();
}

/**************************************************************************
*  @Function Name :: ClearLinesAndPoints
*  @Description   :: 清除所有显示的点和线
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::ClearLinesAndPoints()
{
	ClearAllMark();
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
void Points3DWnd::GetValueRange(double &MinValue, double &MaxValue)
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
void Points3DWnd::SetValueRange(double MinValue, double MaxValue)
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
*					 point:OnLButtonDblClk参数，鼠标点击的坐标
*  @Return        :: None
**************************************************************************/
void Points3DWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	VECTOR vector;
	RadarPoint pos;
	BOOL bFind = GetCurent3DPosition(point, pos, vector);
	if (bFind)
	{
		//遍历所有点，将和该点相同水平角度的总共16个点找到并和中心连线
		RadarPointList sameAngleH;
		m_RadarPointsNode->GetSameAngleH(sameAngleH, pos.angleH);
		int PosNumber = sameAngleH.size();

		if (-1 == m_DblClkPointIdx)//如果里面暂时还没有双击点的话
		{
			m_DblClkPointIdx = m_PointList.size();//应该是大于等于0的
			
			//将sameAngleH里面所有相同水平角的点都显示出来
			if (PosNumber > 16)//先暂时默认最多
			{
				PosNumber = 16;
			}
			for (int i = 0; i < PosNumber; i++)
			{
				m_PointList.push_back(VECTOR{ float(sameAngleH[i].x), float(sameAngleH[i].y), float(sameAngleH[i].z) });//这个点是需要加入的，加进去之后，以后都要对其进行判断
				m_PointList.push_back(VECTOR{ float(0.0), float(0.0), float(0.0) });//为了画线的时候便于计算，这里加上0点
			}
			if (PosNumber < 16)//没有找到16点，暂时按照16个点来计算
			{
				for (int i = PosNumber; i < 16; i++)
				{
					m_PointList.push_back(VECTOR{ float(0.0), float(0.0), float(0.0) });//因为不满足16个，所以先补上0
					m_PointList.push_back(VECTOR{ float(0.0), float(0.0), float(0.0) });
				}
			}
			//m_PointList.push_back(VECTOR{ float(pos.x), float(pos.y), float(pos.z) });//这个点是需要加入的，加进去之后，以后都要对其进行判断
		}
		else
		{//里面有双击点，那么只需要替换那个位置上的点
			//因为不能保证是16个点，所以需要进行一系列的操作
			if (PosNumber > 16)
			{
				PosNumber = 16;
			}
			for (int i = 0; i < PosNumber; i++)
			{
				m_PointList[m_DblClkPointIdx + i*2] = VECTOR{ float(sameAngleH[i].x), float(sameAngleH[i].y), float(sameAngleH[i].z) };
			}
			if (PosNumber < 16)
			{
				for (int i = PosNumber; i < 16; i++)
				{
					m_PointList[m_DblClkPointIdx + i*2] = VECTOR{ float(0.0), float(0.0), float(0.0) };//因为不满足16个，所以先补上0
				}
			}
			//m_PointList[m_DblClkPointIdx] = VECTOR{ float(pos.x), float(pos.y), float(pos.z) };
		}

		for (int i = 0; i < PosNumber; i++)
		{
			AddMark(VECTOR{ float(sameAngleH[i].x), float(sameAngleH[i].y), float(sameAngleH[i].z) }, m_DblClkPointIdx + i); //双击点的下标是m_DblClkPointIdx了
			AddLine(m_PointList[m_DblClkPointIdx + i * 2 + 1], m_PointList[m_DblClkPointIdx + i * 2], int((m_DblClkPointIdx + 2 + i * 2) / 2), 1.0f);
		}
		if (PosNumber < 16)
		{
			for (int i = PosNumber; i < 16; i++)
			{//将里面的进行删除
				DeleteMark(m_DblClkPointIdx + i);
				DeleteLine(int((m_DblClkPointIdx + 2 + i * 2) / 2));
			}
		}
		//AddMark( VECTOR{ float(pos.x), float(pos.y), float(pos.z) }, m_DblClkPointIdx); //双击点的下标是m_DblClkPointIdx了

		TransformPointInformation(pos);//将点的信息传送给状态栏
		
		SetFocus();
		ReDraw();
	}

	Scene3DMessageWnd::OnLButtonDblClk(nFlags, point);
}

/**************************************************************************
*  @Function Name :: TransformPointInformation
*  @Description   :: 给状态栏第2个框格发送关于点的消息
*  @Author        :: zzw
*  @Parameters    :: pos:给状态栏发送消息的点
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::TransformPointInformation(RadarPoint pos)
{
	CString pointInfo;
	pointInfo.Format(_T("Point(%0.2f,%0.2f,%0.2f) channel:%d  intensity:%d  angleH:%0.2f"), pos.x, pos.y, pos.z , pos.channel , int(pos.intensity) , pos.angleH/100.0);
	//将点的信息传递给状态栏
	HWND hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView()->GetSafeHwnd();
	::SendMessage(hwnd, MY_MSG_ChangStatusBar, 2, (LPARAM)LPCTSTR(pointInfo));//对状态栏的第二格进行修改
}

/**************************************************************************
*  @Function Name :: OnLButtonDown
*  @Description   :: 鼠标左键单击击下的响应函数
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnLButtonDown参数
*					 point:OnLButtonDown参数,鼠标点击的坐标
*  @Return        :: None
**************************************************************************/
void Points3DWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = TRUE;
	VECTOR vector;
	RadarPoint pos;
	int PointNum;
	int other_points=16;//额外多的相同水平角的点（16通道）

	if ((FALSE == m_bCtrlDown)||(MM_ROTAL == m_MouseMode)) //不取点时就旋转
	{
		this->SendMessage(WM_IM_NROTATE_BEGIN, (WPARAM)&point);
	}
	else if (MM_MEASURE == m_MouseMode && TRUE == m_bCtrlDown)
	{
		if (GetCurent3DPosition(point, pos, vector)) //得到一个点
		{
			PointNum = m_PointList.size();

			AddMark( VECTOR{ float(pos.x), float(pos.y), float(pos.z) }, PointNum); //添加点
			m_PointList.push_back( VECTOR{ float(pos.x), float(pos.y), float(pos.z) });
			PointNum++;
			if (-1 == m_DblClkPointIdx)//还没有这个点
			{
				if ((PointNum % 2) == 0) //偶数个点，添加线
				{
					AddMeasureLine(m_PointList[PointNum - 1], m_PointList[PointNum - 2], int(PointNum / 2));
					//此时需要取消相同通道点的高亮显示
					m_RadarPointsNode->CancelChannel();
					//ReDraw();
				}
				else
				{//奇数个点，此时需要将相同通道的地方进行高亮显示
					m_RadarPointsNode->SetSelectChannel(pos.channel);
					//ReDraw();
				}
			}
			else//有了双击点
			{
				if (((PointNum- other_points*2) % 2) == 0) //偶数个点，添加线
				{//下面一行判断单点是不是前面两个
					AddMeasureLine(m_PointList[PointNum - 1], (m_DblClkPointIdx+(other_points * 2-1) ==(PointNum - 2))?(m_PointList[PointNum - 2-(other_points * 2)]):(m_PointList[PointNum - 2]), int(PointNum / 2));
					m_RadarPointsNode->CancelChannel();
					//ReDraw();
				}
				else
				{//奇数个点，此时需要将相同通道的地方进行高亮显示
					m_RadarPointsNode->SetSelectChannel(pos.channel);
					//ReDraw();
				}
			}
			TransformPointInformation(pos);//将点的信息传送给状态栏
			ReDraw();
		}
	}
	else if (MM_MEASUREZERO == m_MouseMode && TRUE == m_bCtrlDown)
	{
		if (GetCurent3DPosition(point, pos, vector))//得到一个点
		{//往里面添加两个点
			PointNum = m_PointList.size();

			AddMark( VECTOR{ 0,0,0 }, PointNum); //添加{0,0,0}
			m_PointList.push_back( VECTOR{ 0,0,0 });
			PointNum++;
			AddMark( VECTOR{ float(pos.x), float(pos.y), float(pos.z) }, PointNum); //添加点
			m_PointList.push_back( VECTOR{ float(pos.x), float(pos.y), float(pos.z) });
			PointNum++;
			AddMeasureLine(m_PointList[PointNum - 1], m_PointList[PointNum - 2], int(PointNum / 2));
			//if (((-1 == m_DblClkPointIdx) ? (PointNum % 2) : ((PointNum - 1) % 2)) == 0) //偶数个点，添加线
			//{
			//	AddMeasureLine(m_PointList[PointNum - 1], m_PointList[PointNum - 2], int(PointNum / 2));
			//}
			TransformPointInformation(pos);//将点的信息传送给状态栏
			ReDraw();
		}
	}
}

/**************************************************************************
*  @Function Name :: OnLButtonUp
*  @Description   :: 鼠标左键单击回退的响应函数
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnLButtonUp参数
*					 point:OnLButtonUp参数
*  @Return        :: None
**************************************************************************/
void Points3DWnd::OnLButtonUp(UINT nFlags, CPoint point)
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
void Points3DWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_bRButtonDown = TRUE;
	this->SendMessage(WM_IM_TRANSLATE_BEGIN, (WPARAM)&point, (m_GridLength / m_GridResolution)*4);//步长随着网格的尺度变化着

	SetCursor(LoadCursor(NULL, ID_HAND));//在鼠标右键的时候箭头变成手的形状

}

/**************************************************************************
*  @Function Name :: OnRButtonUp
*  @Description   :: 鼠标右键单击回退的响应函数
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnRButtonUp参数
*					 point:OnRButtonUp参数
*  @Return        :: None
**************************************************************************/
void Points3DWnd::OnRButtonUp(UINT nFlags, CPoint point)
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
void Points3DWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	VIEW_DIR viewDir;
	pMain->Get_Dir_Bitmap(viewDir);

	if (m_bLButtonDown && ((FALSE == m_bCtrlDown) || (MM_ROTAL == m_MouseMode)))
	{
		this->SendMessage(WM_IM_NROTATE_PROGRESS, (WPARAM)&point);
		if (viewDir != RESET)
		{//恢复重置状态
			HWND hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView()->GetSafeHwnd();
			::SendMessage(hwnd, MY_MSG_RestDir, 0, 0);
		}
	}

	if (m_bRButtonDown)
	{
		this->SendMessage(WM_IM_TRANSLATE_PROGRESS, (WPARAM)&point);
		if (viewDir != RESET)
		{//恢复重置状态
			HWND hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView()->GetSafeHwnd();
			::SendMessage(hwnd, MY_MSG_RestDir, 0, 0);
		}
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
BOOL Points3DWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	SetFocus();
	this->SendMessage(WM_IM_SCALE_PROGRESS, (WPARAM)&pt, (LPARAM)zDelta);

	return Scene3DMessageWnd::OnMouseWheel(nFlags, zDelta, pt);
}

/**************************************************************************
*  @Function Name :: EnLargeView
*  @Description   :: 放大视野函数
*  @Author        :: zzw
*  @Parameters    :: 
*					 
*  @Return        :: None
**************************************************************************/
void Points3DWnd::EnLargeView()
{
	CPoint pt = CPoint(0, 0);
	short zDelta = 120;
	this->m_OpenGL3D.OnMouseWheel(zDelta, pt);
	ReDraw();
}

/**************************************************************************
*  @Function Name :: ReduceView
*  @Description   :: 缩小视野函数
*  @Author        :: zzw
*  @Parameters    :: 
*					
*  @Return        :: None
**************************************************************************/
void Points3DWnd::ReduceView()
{
	short zDelta = -120;
	CPoint pt= CPoint(0, 0);
	this->m_OpenGL3D.OnMouseWheel(zDelta, pt);
	ReDraw();
}


/**************************************************************************
*  @Function Name :: OnSize
*  @Description   :: 改变尺寸的响应函数
*  @Author        :: zzw
*  @Parameters    :: nType:OnSize参数 cx:OnSize参数
*					 cy:OnSize参数
*  @Return        :: None
**************************************************************************/
void Points3DWnd::OnSize(UINT nType, int cx, int cy)
{
	Scene3DMessageWnd::OnSize(nType, cx, cy);

	if (cx > 0 && cy > 0)
	{
		m_fAspect = (float)cx / (float)cy;
		//ReDraw();
		SetupWndView();
	}
}

/**************************************************************************
*  @Function Name :: AddMark
*  @Description   :: 往显示里面添加序号是id的点
*  @Author        :: zzw
*  @Parameters    :: pos:需要添加的点
*					 id:需要添加点进入的序号
*  @Return        :: None
**************************************************************************/
void Points3DWnd::AddMark(VECTOR pos, int id)
{
	char name[256] = { '\0' };
	float x, y, z;
	SphereNode *sphere = new SphereNode;
	sprintf_s(name, "P%d", id);
	//x = (float)pos.x - m_centerx;
	//y = (float)pos.y - m_centery;
	//z = (float)pos.z - m_centerz;
	sphere->setName(name);
	sphere->Translate((float)pos.x, (float)pos.y, (float)pos.z);
	sphere->SetSphere(0.05f);//设置选中点显示的球形大小

	sphere->SetColor(0.9f, 0.2f, 0.1f);

	m_CriticalSection.Lock();
	section_group->DeleteChild(name);//加入之前先进行了删除
	section_group->AddChild(sphere);
	m_CriticalSection.Unlock();
}

/**************************************************************************
*  @Function Name :: DeleteMark
*  @Description   :: 删除里面序号是id的点
*  @Author        :: zzw
*  @Parameters    :: id:需要删除点的id序号
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::DeleteMark(int id)
{
	char name[256] = { '\0' };
	sprintf_s(name, "P%d", id);

	m_CriticalSection.Lock();
	section_group->DeleteChild(name);//加入之前先进行了删除
	m_CriticalSection.Unlock();
}

/**************************************************************************
*  @Function Name :: AddLine
*  @Description   :: 往显示里面增加一条线
*  @Author        :: zzw
*  @Parameters    :: pt3DFrm:增加线的起始点 pt3DTo:增加线的终了点
*					 id:增加线的id号 linewidth:增加线的粗细
*  @Return        :: None
**************************************************************************/
void Points3DWnd::AddLine(VECTOR pt3DFrm, VECTOR pt3DTo, int id, float linewidth)
{
	char name[256] = { '\0' };
	sprintf_s(name, "LINE%d", id);
	MeasureLineNode * pLine = new MeasureLineNode(m_Font.GetTextListID());
	pLine->setName(name);
	//pMeasureLine->Translate(-m_centerx, -m_centery, -m_centerz);
	pLine->SetBeginPoint(pt3DFrm);
	pLine->SetEndPoint(pt3DTo);
	pLine->SetLineColor(0.9f, 0.9f, 0.1f);
	pLine->SetLineWidth(linewidth);//设置连线的粗细

	m_CriticalSection.Lock();
	section_group->DeleteChild(name);
	section_group->AddChild(pLine);
	m_CriticalSection.Unlock();
}

/**************************************************************************
*  @Function Name :: DeleteLine
*  @Description   :: 删除序号是id的点
*  @Author        :: zzw
*  @Parameters    :: id:需要删除点的id号
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::DeleteLine(int id)
{
	char name[256] = { '\0' };
	sprintf_s(name, "LINE%d", id);

	m_CriticalSection.Lock();
	section_group->DeleteChild(name);
	m_CriticalSection.Unlock();
}

/**************************************************************************
*  @Function Name :: AddMeasureLine
*  @Description   :: 增加标有长度的线
*  @Author        :: zzw
*  @Parameters    :: pt3DFrm:直线的3D起始点 pt3DTo:直线的3D终了点
*					 id:增加线的id号
*  @Return        :: None
**************************************************************************/
void Points3DWnd::AddMeasureLine(VECTOR pt3DFrm, VECTOR pt3DTo, int id)
{
	char name[256] = { '\0' };
	sprintf_s(name, "LINE%d", id);
	MeasureLineNode * pMeasureLine = new MeasureLineNode(m_Font.GetTextListID());
	pMeasureLine->setName(name);
	//pMeasureLine->Translate(-m_centerx, -m_centery, -m_centerz);
	pMeasureLine->SetBeginPoint(pt3DFrm);
	pMeasureLine->SetEndPoint(pt3DTo);
	pMeasureLine->SetLineColor(0.1f, 0.3f, 0.9f);
	pMeasureLine->SetLineWidth(2.0f);//设置连线的粗细
	if (m_bCurProjMode == TRUE)
		pMeasureLine->RotateText(60, 1.0, 0.0, 0.0);

	double dis = 0;
	double dx, dy, dz;
	dx = pt3DFrm.x - pt3DTo.x;
	dy = pt3DFrm.y - pt3DTo.y;
	dz = pt3DFrm.z - pt3DTo.z;
	dis = sqrt(dx*dx + dy * dy + dz * dz);

	CString strDis;
	strDis.Format(_T("%4.2f"), dis);

	pMeasureLine->SetText(strDis);

	VECTOR texPos;
	texPos.x = (pt3DFrm.x + pt3DTo.x) / 2.0f;
	texPos.y = (pt3DFrm.y + pt3DTo.y) / 2.0f;
	texPos.z = (pt3DFrm.z + pt3DTo.z) / 2.0f;

	pMeasureLine->SetTextPostion(texPos.x, texPos.y, texPos.z);
	pMeasureLine->SetTextColor(1.0f, 1.0f, 0.0f);
	pMeasureLine->SetTextSize(0.5f, 0.5f, 0.5f);

	m_CriticalSection.Lock();
	section_group->DeleteChild(name);
	section_group->AddChild(pMeasureLine);
	m_CriticalSection.Unlock();
}

/**************************************************************************
*  @Function Name :: ClearAllMark
*  @Description   :: 清除所有的选中的点和线
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::ClearAllMark()
{
	m_DblClkPointIdx = -1;//清空标志位
	int PointNum = m_PointList.size();
	m_PointList.clear();

	int i = 0;
	for (i = 0; i < PointNum; i++)
	{
		char name[256] = { '\0' };
		sprintf_s(name, "P%d", i);
		section_group->DeleteChild(name);
	}
	
	//删除里面的线,注意看添加的时候，所以这里删除的时候可以不用判断是哪种测量模式
	for (i = 0; i < PointNum / 2; i++)
	{
		char name[256] = { '\0' };
		sprintf_s(name, "LINE%d", i + 1);
		section_group->DeleteChild(name);
	}
}

/**************************************************************************
*  @Function Name :: Distance
*  @Description   :: 测量某一坐标到原点的距离
*  @Author        :: zzw
*  @Parameters    :: x:测量的x坐标 y:测量的y坐标
*					 z:测量的z坐标
*  @Return        :: None
**************************************************************************/
double Points3DWnd::Distance(double x, double y, double z)
{
	return sqrt(x*x + y*y + z*z);
}

/**************************************************************************
*  @Function Name :: GetCurent3DPosition
*  @Description   :: 获取距离选中位置最近的点的坐标
*  @Author        :: zzw
*  @Parameters    :: point:鼠标点击的坐标 pos:获得的点的引用
*					 vector:暂无
*  @Return        :: None
**************************************************************************/
BOOL Points3DWnd::GetCurent3DPosition(CPoint point, RadarPoint &pos, VECTOR &vector)
{
	double  nx = 0, ny = 0, nz = 0, fx = 0, fy = 0, fz = 0;
	double	modelMatrix[16];
	double	projectMatrix[16];
	int		viewport[4];
	double	outx = 0, outy = 0, outz = 0, foutx = 0, fouty = 0, foutz = 0;
	double  mindis = 100000;
	double  distance = 0;
	BOOL    bfind = FALSE;
	VECTOR pt1, pt2;
	RadarPoint pttemp;

	nx = fx = (double)point.x;
	ny = fy = (double)point.y;
	nz = 0;
	fz = 1;

	wglMakeCurrent(m_OpenGL3D.m_hDC, m_OpenGL3D.m_hRC);
	glGetDoublev(GL_PROJECTION_MATRIX, projectMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	int realy = m_WndHeight - point.y - 1;
	ny = fy = (double)realy;

	m_RadarPointsNode->getDrawMartrix(modelMatrix);

	gluUnProject(nx, ny, nz, modelMatrix, projectMatrix, viewport,
		&outx, &outy, &outz);

	gluUnProject(fx, fy, fz, modelMatrix, projectMatrix, viewport,
		&foutx, &fouty, &foutz);

	pt1.x = outx, pt1.y = outy, pt1.z = outz;
	pt2.x = foutx, pt2.y = fouty, pt2.z = foutz;

	if (m_RadarPointsNode->GetCrossPoint(pt1, pt2, pttemp)==true)               //得到直线和点云的交点
	{
		bfind = TRUE;
		distance = Distance(pttemp.x - pt1.x, pttemp.y - pt1.y, pttemp.z - pt1.z);
		if (distance < mindis)
		{
			pos = pttemp;
			mindis = distance;
		}
	}

	vector.x = (float)(foutx - outx);
	vector.y = (float)(fouty - outy);
	vector.z = (float)(foutz - outz);
	VectorNormalize(&vector);

	return bfind;

}

/**************************************************************************
*  @Function Name :: SetProjMode
*  @Description   :: 设置投影模式
*  @Author        :: zzw
*  @Parameters    :: bProjMode:设置的投影模式，TRUE--透视投影;FALSE--正交投影
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetProjMode(BOOL bProjMode)
{
	if (m_bCurProjMode == bProjMode)
		return;

	m_bCurProjMode = bProjMode;

	SetupWndView();

	ReDraw();
}

/**************************************************************************
*  @Function Name :: SetMeasureStatus
*  @Description   :: 设置测量的状态
*  @Author        :: zzw
*  @Parameters    :: set_MouseMode:设置的测量状态
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetMeasureStatus(E3DMouseMode set_MouseMode)
{
	ClearAllMark(); //开始之前清除之前的测量
	m_MouseMode = set_MouseMode;
	//如果有的话取消高亮显示
	m_RadarPointsNode->CancelChannel();
	//ReDraw();
}

/**************************************************************************
*  @Function Name :: OnKeyDown
*  @Description   :: 键盘击下的响应函数
*  @Author        :: zzw
*  @Parameters    :: nChar:OnKeyDown参数 nRepCnt:OnKeyDown参数
*					 nFlags:OnKeyDown参数
*  @Return        :: None
**************************************************************************/
void Points3DWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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
void Points3DWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nFlags && MK_CONTROL)
	{
		m_bCtrlDown = FALSE;
	}
	Scene3DMessageWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}
