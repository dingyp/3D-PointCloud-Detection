#include "stdafx.h"
#include "Points3DWnd.h"
#include "RadarViewView.h"
#include "..\SceneGraph\SphereNode.h"
#include "..\SceneGraph\PathNode.h"
#include "MeasureLineNode.h"
#include "MainFrm.h"

#define ID_HAND MAKEINTRESOURCE(32649)//ID_HAND��VC�е�һ����Դ

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
*  @Description   :: Points3DWnd���Ĭ�Ϲ��캯��
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

	m_MouseMode = MM_ROTAL;//Ĭ����ctrl��֮���������Ĳ���
	m_PointList.clear(); //���
	m_DblClkPointIdx = -1;//һ��ʼû�е�

	m_GridX = m_GridY = m_GridZ = 0.0;
	m_GridLength=200.00;//����һЩ��������
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
*  @Description   :: Points3DWnd���Ĭ����������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
Points3DWnd::~Points3DWnd()
{
	m_PointList.clear(); //���
}

//********************************************************
// �������� ������3D����
// ��ڲ��� ������λ�ú͸�����ָ��
//
// ���ز��� ���Ƿ񴴽��ɹ�
// �������� �� 2010-06-12 
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
*  @Description   :: ��ʼ����Ұ����
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
*  @Description   :: ��ȡ��������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: ������������
**************************************************************************/
LINEDRAW_TYPE Points3DWnd::GetGirdType()
{
	return m_bGridType;
}

/**************************************************************************
*  @Function Name :: SetGridType
*  @Description   :: ������������
*  @Author        :: zzw
*  @Parameters    :: eLineDrawType:���õ���������
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
*  @Description   :: �����������ĵ�
*  @Author        :: zzw
*  @Parameters    :: addx:�������ĵ��x���� addy:�������ĵ��y����
*					 addz:�������ĵ��z����
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
*  @Description   :: �������ߴ�ϸ
*  @Author        :: zzw
*  @Parameters    :: LineWidth:���������ߵĴ�ϸ
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
*  @Description   :: ����������ɫ,�������ɫ����Ҫ���й�һ��
*  @Author        :: zzw
*  @Parameters    :: gridColor:�����ߵ�RGBֵ
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
*  @Description   :: ��������Ĳ���
*  @Author        :: zzw
*  @Parameters    :: LineDistance:����ĳ���
*					 LineResolution:ÿһ�߷ֳɵ�������
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
*  @Description   :: ��ȡ����Ĳ���
*  @Author        :: zzw
*  @Parameters    :: LineDistance:���񳤶ȵ�����
*					 LineResolution:ÿһ�߷ָ�������������
*  @Return        :: None
**************************************************************************/
void Points3DWnd::GetGridParameters(float &LineDistance, int &LineResolution)
{
	LineDistance = m_GridLength;
	LineResolution = m_GridResolution;
}

/**************************************************************************
*  @Function Name :: SetCircleParameters
*  @Description   :: ����ͬ��Բ�Ĳ���
*  @Author        :: zzw
*  @Parameters    :: LineDistance:�����ͬ��Բ��ֱ��
*					 Circlenumber:ͬ��Բ�ĸ���
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
*  @Description   :: ��ȡͬ��Բ�Ĳ���
*  @Author        :: zzw
*  @Parameters    :: LineDistance:�����ͬ��Բ��ֱ��������
*					 Circlenumber:ͬ��Բ����������
*  @Return        :: None
**************************************************************************/
void Points3DWnd::GetCircleParameters(float &LineDistance, int &Circlenumber)
{
	LineDistance = m_GridRadius;
	Circlenumber = m_GridConCircle;
}

/**************************************************************************
*  @Function Name :: SetGridDir
*  @Description   :: ����������
*  @Author        :: zzw
*  @Parameters    :: eLinePlaneType:����ĳ���
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
*  @Description   :: ������Ұ�ķ���
*  @Author        :: zzw
*  @Parameters    :: viewDir:��Ұ�ķ���
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetDir(VIEW_DIR viewDir)
{
	this->m_OpenGL3D.m_fScale = 1.0f;
	section_group->ResetTransform();


	switch (viewDir)
	{
	case XP: //+X����
		section_group->Rotate(-90, 0.0, 1.0, 0.0);
		section_group->Rotate(-90, 1.0, 0.0, 0.0);
		break;
	case XS://-X����
		section_group->Rotate(90, 0.0, 1.0, 0.0);
		section_group->Rotate(90, -1.0, 0.0, 0.0);
		break;
	case YP://+Y����
		section_group->Rotate(90, 1.0, 0.0, 0.0);
		section_group->Rotate(180, 0.0, 1.0, 0.0);
		break;
	case YS://-Y����
		section_group->Rotate(-90, 1.0, 0.0, 0.0);
		break;
	case ZP://+Z����
		break;
	case ZS://-Z����
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
*  @Description   :: ���½�����ͼ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetupWndView()
{
	if (m_bCurProjMode)
	{
		this->SetWndProj(60.0f, m_fAspect, 0.1f, 10000.0f); //ԭʼ��С
		this->SetWndView(eyex* (m_GridLength / 200.0), eyey* (m_GridLength / 200.0), eyez* (m_GridLength / 200.0), 0.0, 0.0, 0.0, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		float WndCX = (float)(m_rangex> m_rangey?m_rangex:m_rangey);
		float max = (float)(WndCX>(float)m_rangez?WndCX:(float)m_rangez);
		float X = 1.1f*WndCX;
		this->SetWndProj(-X * m_fAspect, X*m_fAspect, -X, X, -10.0f*max, 10.0f*max); //ԭʼ��С
		this->SetWndView(0.0f, 0.0f, 2.0f*max, 0.0, 0.0, 0.0, 0.0f, 1.0f, 0.0f);
	}
}

/**************************************************************************
*  @Function Name :: ResetView
*  @Description   :: ������Ұ�ķ���
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
*  @Description   :: ������ɫ�����ģʽ
*  @Author        :: zzw
*  @Parameters    :: set_ColorMode:����ģʽ��ID��
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
*  @Description   :: ��ȡ��ɫ�����ID��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: ������ɫ�����ID��
**************************************************************************/
int Points3DWnd::GetColorMode()
{
	return m_RadarPointsNode->GetColorMode();
}

/**************************************************************************
*  @Function Name :: ReScaleColorRange
*  @Description   :: ���²�����ֵ�ķ�Χ������ɫ���½��б���
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
*  @Description   :: �Զ����BoundBox������Ĭ�ϵ�size,������������
*  @Author        :: zzw
*  @Parameters    :: GridLength:����ĳ���
*					 GridRadius:ͬ��Բ�İ뾶
*  @Return        :: None
**************************************************************************/
void Points3DWnd::GetDefaultBoundBox(double &GridLength, double &GridRadius)
{
	m_RadarPointsNode->GetDefaultBoundBox(GridLength, GridRadius);
}

/**************************************************************************
*  @Function Name :: SetBoundBox
*  @Description   :: ������ʾ�����BoundBox
*  @Author        :: zzw
*  @Parameters    :: x1:BoundBox��x��Сֵ x2:BoundBox��x���ֵ y1:BoundBox��y��Сֵ
*					 y2:BoundBox��y���ֵ z1:BoundBox��z��Сֵ z2:BoundBox��z���ֵ
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetBoundBox(double x1, double x2, double y1, double y2, double z1, double z2)
{
	m_rangex = x2 - x1;
	m_rangey = y2 - y1;
	m_rangez = z2 - z1;

	//�ҳ�6��������������
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
	if (GRID == m_bGridType)//���������
	{
		m_BaseLineNode->SetLineParameters(m_GridLength, m_GridLength / m_GridResolution);

		//�������Ϣ���ݸ�״̬��
		gridInfo.Format(_T("դ��߳�:%0.2f m"), m_GridLength / m_GridResolution);
		::SendMessage(hwnd, MY_MSG_ChangStatusBar, 3, (LPARAM)LPCTSTR(gridInfo));
	}
	else if (CON_CIRCLE == m_bGridType)
	{
		m_BaseLineNode->SetLineParameters(m_GridRadius * 2, m_GridRadius / m_GridConCircle);

		//�������Ϣ���ݸ�״̬��
		gridInfo.Format(_T("ͬ��Բ�뾶:%0.2f m"), m_GridRadius / m_GridConCircle);
		::SendMessage(hwnd, MY_MSG_ChangStatusBar, 3, (LPARAM)LPCTSTR(gridInfo));
	}

	//������Ұ�Ĵ�С
	if (m_bCurProjMode)
	{
		this->SetWndProj(60.0f, m_fAspect, 0.1f, 10000.0f); //ԭʼ��С
		this->SetWndView(eyex* (m_GridLength / 200.0), eyey * (m_GridLength / 200.0), eyez * (m_GridLength / 200.0), 0.0, 0.0, 0.0, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		float WndCX = (float)(m_rangex> m_rangey?m_rangex:m_rangey);
		float max = (float)(WndCX> (float)m_rangez?WndCX:(float)m_rangez);
		float X = 1.1f*WndCX;
		this->SetWndProj(-X * m_fAspect, X*m_fAspect, -X, X, -10.0f*max, 10.0f*max); //ԭʼ��С
		this->SetWndView(0.0f, 0.0f, 2.0f*max, 0.0, 0.0, 0.0, 0.0f, 1.0f, 0.0f);
	}
	//�����������С
	SetAxisNodeSize(m_GridLength / m_GridResolution * 2);
}

/**************************************************************************
*  @Function Name :: SetAxisNodeSize
*  @Description   :: ����������Ĵ�С
*  @Author        :: zzw
*  @Parameters    :: setLength:������ĳ���
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
*  @Description   :: �����������Ƿ���ʾ
*  @Author        :: zzw
*  @Parameters    :: setIsDraw:�������Ƿ���ʾ��־λ
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetAxisNodeIsShow(bool setIsDraw)
{
	m_AxisNode->SetIsShowed(setIsDraw);
}

/**************************************************************************
*  @Function Name :: GetAxisNodeIsShow
*  @Description   :: ��ȡ�������Ƿ���ʾ��־λ
*  @Author        :: zzw
*  @Parameters    :: setIsDraw:�������Ƿ���ʾ��־λ
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::GetAxisNodeIsShow(bool &setIsDraw)
{
	m_AxisNode->GetIsShowed(setIsDraw);
}

/**************************************************************************
*  @Function Name :: SetSensorCenter
*  @Description   :: ���ô��������ĵ�����
*  @Author        :: zzw
*  @Parameters    :: sensor_x:���������ĵ�x���� sensor_y:���������ĵ�y����
*					 sensor_z:���������ĵ�z����
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetSensorCenter(double sensor_x, double sensor_y, double sensor_z)
{
	transformer.SetSensorCenter(sensor_x, sensor_y, sensor_z);
}

/**************************************************************************
*  @Function Name :: SetSensorEuler
*  @Description   :: ���ô���������̬��
*  @Author        :: zzw
*  @Parameters    :: sensor_pitch:������x�����ת�Ƕ� sensor_roll:������y�����ת�Ƕ�
*					 sensor_yaw:������z�����ת�Ƕ�
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetSensorEuler(double sensor_pitch, double sensor_roll, double sensor_yaw)
{
	transformer.SetSensorEuler(sensor_pitch, sensor_roll, sensor_yaw);
}

/**************************************************************************
*  @Function Name :: GetSensorCenter
*  @Description   :: ��ȡ�����������ĵ�����
*  @Author        :: zzw
*  @Parameters    :: sensor_x:x�����ĵ����������� sensor_y:y�����ĵ�����������
*					 sensor_z:z�����ĵ�����������
*  @Return        :: None
**************************************************************************/
void Points3DWnd::GetSensorCenter(double &sensor_x, double &sensor_y, double &sensor_z)
{
	transformer.GetSensorCenter(sensor_x, sensor_y, sensor_z);
}

/**************************************************************************
*  @Function Name :: GetSensorEuler
*  @Description   :: ��ȡ����������̬��
*  @Author        :: zzw
*  @Parameters    :: sensor_pitch:������x����̬�ǵ����� sensor_roll:������y����̬�ǵ�����
*					 sensor_yaw:������z����̬�ǵ�����
*  @Return        :: None
**************************************************************************/
void Points3DWnd::GetSensorEuler(double &sensor_pitch, double &sensor_roll, double &sensor_yaw)
{
	transformer.GetSensorEuler(sensor_pitch, sensor_roll, sensor_yaw);
}

/**************************************************************************
*  @Function Name :: AttachData
*  @Description   :: ������ʾ�ĵ�֡
*  @Author        :: zzw
*  @Parameters    :: PointList:���µĵ�Ķ���
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::AttachData(RadarPointList &PointList)
{
	//�ڵ�ӽ�����PointsNode֮ǰ���Ƚ���������
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
*  @Description   :: �õ���ʾ�ĵ�֡
*  @Author        :: zzw
*  @Parameters    :: PointList:������ʾ�ĵ�֡������
*
*  @Return        :: �����Ƿ��ȡ�ɹ�
**************************************************************************/
bool Points3DWnd::GetPointListData(RadarPointList &PointList)
{
	return m_RadarPointsNode->GetPointListData(PointList);
}

/**************************************************************************
*  @Function Name :: GetColorCoding
*  @Description   :: ��ȡ��ɫ���������
*  @Author        :: zzw
*  @Parameters    :: ColorCoding:��ɫ�������Ե�����
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
*  @Description   :: ������ɫ���������
*  @Author        :: zzw
*  @Parameters    :: chooseColorCoding:���õ���ɫ����
*					 bUpdate:��ʾ������ɫ��������֮���Ƿ�redraw
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
*  @Description   :: �任��ʾ֡������
*  @Author        :: zzw
*  @Parameters    :: addX:X�᷽���ƶ��ľ��� addY:Y�᷽���ƶ��ľ��� addZ:Z�᷽���ƶ��ľ���
*					 pitch:X�᷽����ת�ĽǶ� pitch:Y�᷽����ת�ĽǶ� pitch:Z�᷽����ת�ĽǶ�
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
*  @Description   :: ���������ʾ����
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
*  @Description   :: ���������ʾ�ĵ����
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
*  @Description   :: ��ȡ��ĳһ���Ե������Сֵ
*  @Author        :: zzw
*  @Parameters    :: MinValue:��֡ĳһ���Ե���Сֵ������
*					 MaxValue:��֡ĳһ���Ե����ֵ������
*  @Return        :: None
**************************************************************************/
void Points3DWnd::GetValueRange(double &MinValue, double &MaxValue)
{
	m_RadarPointsNode->GetDataRange(MinValue, MaxValue);
}

/**************************************************************************
*  @Function Name :: SetValueRange
*  @Description   :: ���õ�ĳһ���Ե���Сֵ�����ֵ
*  @Author        :: zzw
*  @Parameters    :: MinValue:���õ���Сֵ
*					 MaxValue:���õ����ֵ
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
*  @Description   :: ������˫������Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnLButtonDblClk����
*					 point:OnLButtonDblClk�����������������
*  @Return        :: None
**************************************************************************/
void Points3DWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	VECTOR vector;
	RadarPoint pos;
	BOOL bFind = GetCurent3DPosition(point, pos, vector);
	if (bFind)
	{
		//�������е㣬���͸õ���ͬˮƽ�Ƕȵ��ܹ�16�����ҵ�������������
		RadarPointList sameAngleH;
		m_RadarPointsNode->GetSameAngleH(sameAngleH, pos.angleH);
		int PosNumber = sameAngleH.size();

		if (-1 == m_DblClkPointIdx)//���������ʱ��û��˫����Ļ�
		{
			m_DblClkPointIdx = m_PointList.size();//Ӧ���Ǵ��ڵ���0��
			
			//��sameAngleH����������ͬˮƽ�ǵĵ㶼��ʾ����
			if (PosNumber > 16)//����ʱĬ�����
			{
				PosNumber = 16;
			}
			for (int i = 0; i < PosNumber; i++)
			{
				m_PointList.push_back(VECTOR{ float(sameAngleH[i].x), float(sameAngleH[i].y), float(sameAngleH[i].z) });//���������Ҫ����ģ��ӽ�ȥ֮���Ժ�Ҫ��������ж�
				m_PointList.push_back(VECTOR{ float(0.0), float(0.0), float(0.0) });//Ϊ�˻��ߵ�ʱ����ڼ��㣬�������0��
			}
			if (PosNumber < 16)//û���ҵ�16�㣬��ʱ����16����������
			{
				for (int i = PosNumber; i < 16; i++)
				{
					m_PointList.push_back(VECTOR{ float(0.0), float(0.0), float(0.0) });//��Ϊ������16���������Ȳ���0
					m_PointList.push_back(VECTOR{ float(0.0), float(0.0), float(0.0) });
				}
			}
			//m_PointList.push_back(VECTOR{ float(pos.x), float(pos.y), float(pos.z) });//���������Ҫ����ģ��ӽ�ȥ֮���Ժ�Ҫ��������ж�
		}
		else
		{//������˫���㣬��ôֻ��Ҫ�滻�Ǹ�λ���ϵĵ�
			//��Ϊ���ܱ�֤��16���㣬������Ҫ����һϵ�еĲ���
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
					m_PointList[m_DblClkPointIdx + i*2] = VECTOR{ float(0.0), float(0.0), float(0.0) };//��Ϊ������16���������Ȳ���0
				}
			}
			//m_PointList[m_DblClkPointIdx] = VECTOR{ float(pos.x), float(pos.y), float(pos.z) };
		}

		for (int i = 0; i < PosNumber; i++)
		{
			AddMark(VECTOR{ float(sameAngleH[i].x), float(sameAngleH[i].y), float(sameAngleH[i].z) }, m_DblClkPointIdx + i); //˫������±���m_DblClkPointIdx��
			AddLine(m_PointList[m_DblClkPointIdx + i * 2 + 1], m_PointList[m_DblClkPointIdx + i * 2], int((m_DblClkPointIdx + 2 + i * 2) / 2), 1.0f);
		}
		if (PosNumber < 16)
		{
			for (int i = PosNumber; i < 16; i++)
			{//������Ľ���ɾ��
				DeleteMark(m_DblClkPointIdx + i);
				DeleteLine(int((m_DblClkPointIdx + 2 + i * 2) / 2));
			}
		}
		//AddMark( VECTOR{ float(pos.x), float(pos.y), float(pos.z) }, m_DblClkPointIdx); //˫������±���m_DblClkPointIdx��

		TransformPointInformation(pos);//�������Ϣ���͸�״̬��
		
		SetFocus();
		ReDraw();
	}

	Scene3DMessageWnd::OnLButtonDblClk(nFlags, point);
}

/**************************************************************************
*  @Function Name :: TransformPointInformation
*  @Description   :: ��״̬����2������͹��ڵ����Ϣ
*  @Author        :: zzw
*  @Parameters    :: pos:��״̬��������Ϣ�ĵ�
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::TransformPointInformation(RadarPoint pos)
{
	CString pointInfo;
	pointInfo.Format(_T("Point(%0.2f,%0.2f,%0.2f) channel:%d  intensity:%d  angleH:%0.2f"), pos.x, pos.y, pos.z , pos.channel , int(pos.intensity) , pos.angleH/100.0);
	//�������Ϣ���ݸ�״̬��
	HWND hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView()->GetSafeHwnd();
	::SendMessage(hwnd, MY_MSG_ChangStatusBar, 2, (LPARAM)LPCTSTR(pointInfo));//��״̬���ĵڶ�������޸�
}

/**************************************************************************
*  @Function Name :: OnLButtonDown
*  @Description   :: �������������µ���Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnLButtonDown����
*					 point:OnLButtonDown����,�����������
*  @Return        :: None
**************************************************************************/
void Points3DWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = TRUE;
	VECTOR vector;
	RadarPoint pos;
	int PointNum;
	int other_points=16;//��������ͬˮƽ�ǵĵ㣨16ͨ����

	if ((FALSE == m_bCtrlDown)||(MM_ROTAL == m_MouseMode)) //��ȡ��ʱ����ת
	{
		this->SendMessage(WM_IM_NROTATE_BEGIN, (WPARAM)&point);
	}
	else if (MM_MEASURE == m_MouseMode && TRUE == m_bCtrlDown)
	{
		if (GetCurent3DPosition(point, pos, vector)) //�õ�һ����
		{
			PointNum = m_PointList.size();

			AddMark( VECTOR{ float(pos.x), float(pos.y), float(pos.z) }, PointNum); //��ӵ�
			m_PointList.push_back( VECTOR{ float(pos.x), float(pos.y), float(pos.z) });
			PointNum++;
			if (-1 == m_DblClkPointIdx)//��û�������
			{
				if ((PointNum % 2) == 0) //ż�����㣬�����
				{
					AddMeasureLine(m_PointList[PointNum - 1], m_PointList[PointNum - 2], int(PointNum / 2));
					//��ʱ��Ҫȡ����ͬͨ����ĸ�����ʾ
					m_RadarPointsNode->CancelChannel();
					//ReDraw();
				}
				else
				{//�������㣬��ʱ��Ҫ����ͬͨ���ĵط����и�����ʾ
					m_RadarPointsNode->SetSelectChannel(pos.channel);
					//ReDraw();
				}
			}
			else//����˫����
			{
				if (((PointNum- other_points*2) % 2) == 0) //ż�����㣬�����
				{//����һ���жϵ����ǲ���ǰ������
					AddMeasureLine(m_PointList[PointNum - 1], (m_DblClkPointIdx+(other_points * 2-1) ==(PointNum - 2))?(m_PointList[PointNum - 2-(other_points * 2)]):(m_PointList[PointNum - 2]), int(PointNum / 2));
					m_RadarPointsNode->CancelChannel();
					//ReDraw();
				}
				else
				{//�������㣬��ʱ��Ҫ����ͬͨ���ĵط����и�����ʾ
					m_RadarPointsNode->SetSelectChannel(pos.channel);
					//ReDraw();
				}
			}
			TransformPointInformation(pos);//�������Ϣ���͸�״̬��
			ReDraw();
		}
	}
	else if (MM_MEASUREZERO == m_MouseMode && TRUE == m_bCtrlDown)
	{
		if (GetCurent3DPosition(point, pos, vector))//�õ�һ����
		{//���������������
			PointNum = m_PointList.size();

			AddMark( VECTOR{ 0,0,0 }, PointNum); //���{0,0,0}
			m_PointList.push_back( VECTOR{ 0,0,0 });
			PointNum++;
			AddMark( VECTOR{ float(pos.x), float(pos.y), float(pos.z) }, PointNum); //��ӵ�
			m_PointList.push_back( VECTOR{ float(pos.x), float(pos.y), float(pos.z) });
			PointNum++;
			AddMeasureLine(m_PointList[PointNum - 1], m_PointList[PointNum - 2], int(PointNum / 2));
			//if (((-1 == m_DblClkPointIdx) ? (PointNum % 2) : ((PointNum - 1) % 2)) == 0) //ż�����㣬�����
			//{
			//	AddMeasureLine(m_PointList[PointNum - 1], m_PointList[PointNum - 2], int(PointNum / 2));
			//}
			TransformPointInformation(pos);//�������Ϣ���͸�״̬��
			ReDraw();
		}
	}
}

/**************************************************************************
*  @Function Name :: OnLButtonUp
*  @Description   :: �������������˵���Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnLButtonUp����
*					 point:OnLButtonUp����
*  @Return        :: None
**************************************************************************/
void Points3DWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	this->SendMessage(WM_IM_NROTATE_END, (WPARAM)&point);

	m_bLButtonDown = FALSE;
}

/**************************************************************************
*  @Function Name :: OnRButtonDown
*  @Description   :: ����Ҽ��������µ���Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnRButtonDown����
*					 point:OnRButtonDown����
*  @Return        :: None
**************************************************************************/
void Points3DWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_bRButtonDown = TRUE;
	this->SendMessage(WM_IM_TRANSLATE_BEGIN, (WPARAM)&point, (m_GridLength / m_GridResolution)*4);//������������ĳ߶ȱ仯��

	SetCursor(LoadCursor(NULL, ID_HAND));//������Ҽ���ʱ���ͷ����ֵ���״

}

/**************************************************************************
*  @Function Name :: OnRButtonUp
*  @Description   :: ����Ҽ��������˵���Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnRButtonUp����
*					 point:OnRButtonUp����
*  @Return        :: None
**************************************************************************/
void Points3DWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	this->SendMessage(WM_IM_TRANSLATE_END, (WPARAM)&point);

	m_bRButtonDown = FALSE;
}

/**************************************************************************
*  @Function Name :: OnMouseMove
*  @Description   :: ����ƶ�����Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnMouseMove����
*					 point:OnMouseMove����
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
		{//�ָ�����״̬
			HWND hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView()->GetSafeHwnd();
			::SendMessage(hwnd, MY_MSG_RestDir, 0, 0);
		}
	}

	if (m_bRButtonDown)
	{
		this->SendMessage(WM_IM_TRANSLATE_PROGRESS, (WPARAM)&point);
		if (viewDir != RESET)
		{//�ָ�����״̬
			HWND hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView()->GetSafeHwnd();
			::SendMessage(hwnd, MY_MSG_RestDir, 0, 0);
		}
	}
}

/**************************************************************************
*  @Function Name :: OnMouseWheel
*  @Description   :: �����ֻ�������Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnMouseWheel���� zDelta:OnMouseWheel����
*					 pt:OnMouseWheel����
*  @Return        :: ���������Ƿ�ɹ�
**************************************************************************/
BOOL Points3DWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	SetFocus();
	this->SendMessage(WM_IM_SCALE_PROGRESS, (WPARAM)&pt, (LPARAM)zDelta);

	return Scene3DMessageWnd::OnMouseWheel(nFlags, zDelta, pt);
}

/**************************************************************************
*  @Function Name :: EnLargeView
*  @Description   :: �Ŵ���Ұ����
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
*  @Description   :: ��С��Ұ����
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
*  @Description   :: �ı�ߴ����Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nType:OnSize���� cx:OnSize����
*					 cy:OnSize����
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
*  @Description   :: ����ʾ������������id�ĵ�
*  @Author        :: zzw
*  @Parameters    :: pos:��Ҫ��ӵĵ�
*					 id:��Ҫ��ӵ��������
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
	sphere->SetSphere(0.05f);//����ѡ�е���ʾ�����δ�С

	sphere->SetColor(0.9f, 0.2f, 0.1f);

	m_CriticalSection.Lock();
	section_group->DeleteChild(name);//����֮ǰ�Ƚ�����ɾ��
	section_group->AddChild(sphere);
	m_CriticalSection.Unlock();
}

/**************************************************************************
*  @Function Name :: DeleteMark
*  @Description   :: ɾ�����������id�ĵ�
*  @Author        :: zzw
*  @Parameters    :: id:��Ҫɾ�����id���
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::DeleteMark(int id)
{
	char name[256] = { '\0' };
	sprintf_s(name, "P%d", id);

	m_CriticalSection.Lock();
	section_group->DeleteChild(name);//����֮ǰ�Ƚ�����ɾ��
	m_CriticalSection.Unlock();
}

/**************************************************************************
*  @Function Name :: AddLine
*  @Description   :: ����ʾ��������һ����
*  @Author        :: zzw
*  @Parameters    :: pt3DFrm:�����ߵ���ʼ�� pt3DTo:�����ߵ����˵�
*					 id:�����ߵ�id�� linewidth:�����ߵĴ�ϸ
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
	pLine->SetLineWidth(linewidth);//�������ߵĴ�ϸ

	m_CriticalSection.Lock();
	section_group->DeleteChild(name);
	section_group->AddChild(pLine);
	m_CriticalSection.Unlock();
}

/**************************************************************************
*  @Function Name :: DeleteLine
*  @Description   :: ɾ�������id�ĵ�
*  @Author        :: zzw
*  @Parameters    :: id:��Ҫɾ�����id��
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
*  @Description   :: ���ӱ��г��ȵ���
*  @Author        :: zzw
*  @Parameters    :: pt3DFrm:ֱ�ߵ�3D��ʼ�� pt3DTo:ֱ�ߵ�3D���˵�
*					 id:�����ߵ�id��
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
	pMeasureLine->SetLineWidth(2.0f);//�������ߵĴ�ϸ
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
*  @Description   :: ������е�ѡ�еĵ����
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::ClearAllMark()
{
	m_DblClkPointIdx = -1;//��ձ�־λ
	int PointNum = m_PointList.size();
	m_PointList.clear();

	int i = 0;
	for (i = 0; i < PointNum; i++)
	{
		char name[256] = { '\0' };
		sprintf_s(name, "P%d", i);
		section_group->DeleteChild(name);
	}
	
	//ɾ���������,ע�⿴��ӵ�ʱ����������ɾ����ʱ����Բ����ж������ֲ���ģʽ
	for (i = 0; i < PointNum / 2; i++)
	{
		char name[256] = { '\0' };
		sprintf_s(name, "LINE%d", i + 1);
		section_group->DeleteChild(name);
	}
}

/**************************************************************************
*  @Function Name :: Distance
*  @Description   :: ����ĳһ���굽ԭ��ľ���
*  @Author        :: zzw
*  @Parameters    :: x:������x���� y:������y����
*					 z:������z����
*  @Return        :: None
**************************************************************************/
double Points3DWnd::Distance(double x, double y, double z)
{
	return sqrt(x*x + y*y + z*z);
}

/**************************************************************************
*  @Function Name :: GetCurent3DPosition
*  @Description   :: ��ȡ����ѡ��λ������ĵ������
*  @Author        :: zzw
*  @Parameters    :: point:����������� pos:��õĵ������
*					 vector:����
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

	if (m_RadarPointsNode->GetCrossPoint(pt1, pt2, pttemp)==true)               //�õ�ֱ�ߺ͵��ƵĽ���
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
*  @Description   :: ����ͶӰģʽ
*  @Author        :: zzw
*  @Parameters    :: bProjMode:���õ�ͶӰģʽ��TRUE--͸��ͶӰ;FALSE--����ͶӰ
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
*  @Description   :: ���ò�����״̬
*  @Author        :: zzw
*  @Parameters    :: set_MouseMode:���õĲ���״̬
*
*  @Return        :: None
**************************************************************************/
void Points3DWnd::SetMeasureStatus(E3DMouseMode set_MouseMode)
{
	ClearAllMark(); //��ʼ֮ǰ���֮ǰ�Ĳ���
	m_MouseMode = set_MouseMode;
	//����еĻ�ȡ��������ʾ
	m_RadarPointsNode->CancelChannel();
	//ReDraw();
}

/**************************************************************************
*  @Function Name :: OnKeyDown
*  @Description   :: ���̻��µ���Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nChar:OnKeyDown���� nRepCnt:OnKeyDown����
*					 nFlags:OnKeyDown����
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
*  @Description   :: ���̻��˵���Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nChar:OnKeyUp���� nRepCnt:OnKeyUp����
*					 nFlags:OnKeyUp����
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
