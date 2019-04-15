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
*  @Description   :: Points3DWnd���Ĭ�Ϲ��캯��
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

	m_MouseMode = MM_ROTAL;//Ĭ����ctrl��֮���������Ĳ���
	m_PointList.clear(); //���

}

/**************************************************************************
*  @Function Name :: ~Points3DWnd
*  @Description   :: Points3DWnd���Ĭ����������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
PointsAndSTL3DWnd::~PointsAndSTL3DWnd()
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
BOOL PointsAndSTL3DWnd::Create(CRect rect, CWnd *pParent, UINT idc)
{
	BOOL ret = Scene3DMessageWnd::Create(NULL, _T("Point3DWnd"), WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE, rect, pParent, idc);

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
*  @Description   :: ���½�����ͼ
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::SetupWndView()
{
	if (m_bCurProjMode)
	{
		this->SetWndProj(60.0f, m_fAspect, 0.1f, 10000.0f); //ԭʼ��С
		this->SetWndView(eyex*(m_rangeTotal / 100.0), eyey*(m_rangeTotal / 100.0), eyez*(m_rangeTotal / 100.0), 0.0, 0.0, 0.0, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		float WndCX = (float)max(m_rangex, m_rangey);
		float max = (float)max(WndCX, (float)m_rangez);
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
*  @Description   :: �Զ����BoundBox������Ĭ�ϵ�size,������������
*  @Author        :: zzw
*  @Parameters    :: GridLength:����ĳ���
*					 GridRadius:ͬ��Բ�İ뾶
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::GetBoundBox(double &x1, double &x2, double &y1, double &y2, double &z1, double &z2)
{
	m_RadarPointsNode->GetDefaultBoundBox(x1, x2, y1, y2, z1, z2);
}

/**************************************************************************
*  @Function Name :: SetBoundBox
*  @Description   :: ������ʾ�����BoundBox
*  @Author        :: zzw
*  @Parameters    :: x1:BoundBox��x��Сֵ x2:BoundBox��x���ֵ y1:BoundBox��y��Сֵ
*					 y2:BoundBox��y���ֵ z1:BoundBox��z��Сֵ z2:BoundBox��z���ֵ
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::SetBoundBox(double x1, double x2, double y1, double y2, double z1, double z2)
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
	m_rangeTotal = ceil(smax / 10) * 20;

	CString gridInfo;
	HWND hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView()->GetSafeHwnd();

	//�����������С
	m_AxisNode->SetLineLength(ceil(smax / 10) * 2);
	//������Ұ�Ĵ�С
	if (m_bCurProjMode)
	{
		this->SetWndProj(60.0f, m_fAspect, 0.1f, 10000.0f); //ԭʼ��С
		this->SetWndView(eyex *(m_rangeTotal / 100.0), eyey *(m_rangeTotal / 100.0), eyez *(m_rangeTotal / 100.0), 0.0, 0.0, 0.0, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		float WndCX = (float)max(m_rangex, m_rangey);
		float max = (float)max(WndCX, (float)m_rangez);
		float X = 1.1f*WndCX;
		this->SetWndProj(-X * m_fAspect, X*m_fAspect, -X, X, -10.0f*max, 10.0f*max); //ԭʼ��С
		this->SetWndView(0.0f, 0.0f, 2.0f*max, 0.0, 0.0, 0.0, 0.0f, 1.0f, 0.0f);
	}
}

/**************************************************************************
*  @Function Name :: SetAxisNodeSize
*  @Description   :: ����������Ĵ�С
*  @Author        :: zzw
*  @Parameters    :: setLength:������ĳ���
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
*  @Description   :: ���ô��������ĵ�����
*  @Author        :: zzw
*  @Parameters    :: sensor_x:���������ĵ�x���� sensor_y:���������ĵ�y����
*					 sensor_z:���������ĵ�z����
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::SetSensorCenter(double sensor_x, double sensor_y, double sensor_z)
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
void PointsAndSTL3DWnd::SetSensorEuler(double sensor_pitch, double sensor_roll, double sensor_yaw)
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
void PointsAndSTL3DWnd::GetSensorCenter(double &sensor_x, double &sensor_y, double &sensor_z)
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
void PointsAndSTL3DWnd::GetSensorEuler(double &sensor_pitch, double &sensor_roll, double &sensor_yaw)
{
	transformer.GetSensorEuler(sensor_pitch, sensor_roll, sensor_yaw);
}

//�ڼ���֮��Ӧ����������
void PointsAndSTL3DWnd::AfterRigidRegistration()
{

}

//��ӵ�������
void PointsAndSTL3DWnd::AttachPointClouds(CPointClouds *pPointClouds)
{
	//�ڵ�ӽ�����PointsNode֮ǰ���Ƚ���������
	if (transformer.IsChangeBefore())
	{
		for (int i = 0; i < pPointClouds->m_uiNowCloudNums; i++)
		{
			transformer.ChangeBefore(*(pPointClouds->m_PointCloud[i]));
		}
	}
	////��ȡ���Ƶ�ƽ��֡�����֡����С֡
	//m_calcul_pointclouds.GetPointClouds(pPointClouds);//��ȡ����
	//m_calcul_pointclouds

	float xMin, xMax, yMin, yMax, zMin, zMax;
	xMax = yMax = zMax = FLT_MIN;
	xMin = yMin = zMin = FLT_MAX;
	if (true == pPointClouds->is_Calcul)
	{//ֻ��Ҫ����ƽ��֡
		if (false==pPointClouds->GetArgPointCloud_BoundBox(xMin, xMax, yMin, yMax, zMin, zMax))
		{
			xMax = yMax = zMax = 10;
			xMin = yMin = zMin = -10;
		}
	}
	else
	{
		//ͨ������ȡǰ��֡����ȡһ��boudingBox
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

//���STL����list
void PointsAndSTL3DWnd::AttachDataList(CDataList * pDataList)
{
	if (NULL == pDataList)
	{
		return;
	}
	float xMin, xMax, yMin, yMax, zMin, zMax;
	pDataList->GetBoundBox(xMin, xMax, yMin, yMax, zMin, zMax);
	if ((FLT_MAX != xMin) && (FLT_MAX != yMin) && (FLT_MAX != zMin))//ֻ�е��õ��Ľ��������Ž���boundingbox�ĵ���
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
	if ((FLT_MAX != xMin) && (FLT_MAX != yMin) && (FLT_MAX != zMin))//ֻ�е��õ��Ľ��������Ž���boundingbox�ĵ���
	{
		SetBoundBox(double(xMin), double(xMax), double(yMin), double(yMax), double(zMin), double(zMax));
	}
	m_pRoomNode->AttachRoomModel(pModellist);
	SetFocus();//��Ϊ�����ʾ֮�����û���˾۽�
	ReDraw();
}

//���ñ���Ļ��Ʒ�ʽ
void PointsAndSTL3DWnd::SetPolygonMode(GLenum Mode)
{
	m_STLGeometryNode->SetPolygonMode(Mode);
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
void PointsAndSTL3DWnd::ClearData()
{
	m_RadarPointsNode->ClearData();
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
void PointsAndSTL3DWnd::GetValueRange(double &MinValue, double &MaxValue)
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
void PointsAndSTL3DWnd::SetValueRange(double MinValue, double MaxValue)
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
*					 point:OnLButtonDblClk����
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	Scene3DMessageWnd::OnLButtonDblClk(nFlags, point);
}

/**************************************************************************
*  @Function Name :: OnLButtonDown
*  @Description   :: �������������µ���Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnLButtonDown����
*					 point:OnLButtonDown����,�����������
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = TRUE;
	VECTOR vector;
	RadarPoint pos;
	int PointNum;
	int other_points = 16;//��������ͬˮƽ�ǵĵ㣨16ͨ����

	this->SendMessage(WM_IM_NROTATE_BEGIN, (WPARAM)&point);//���ڵ���������ֻ����ת
}

/**************************************************************************
*  @Function Name :: OnLButtonUp
*  @Description   :: �������������˵���Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnLButtonUp����
*					 point:OnLButtonUp����
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::OnLButtonUp(UINT nFlags, CPoint point)
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
void PointsAndSTL3DWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	this->SendMessage(WM_IM_TRANSLATE_BEGIN, (WPARAM)&point);

	m_bRButtonDown = TRUE;
}

/**************************************************************************
*  @Function Name :: OnRButtonUp
*  @Description   :: ����Ҽ��������˵���Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnRButtonUp����
*					 point:OnRButtonUp����
*  @Return        :: None
**************************************************************************/
void PointsAndSTL3DWnd::OnRButtonUp(UINT nFlags, CPoint point)
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
*  @Description   :: �����ֻ�������Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nFlags:OnMouseWheel���� zDelta:OnMouseWheel����
*					 pt:OnMouseWheel����
*  @Return        :: ���������Ƿ�ɹ�
**************************************************************************/
BOOL PointsAndSTL3DWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	this->SendMessage(WM_IM_SCALE_PROGRESS, (WPARAM)&pt, (LPARAM)zDelta);

	return Scene3DMessageWnd::OnMouseWheel(nFlags, zDelta, pt);
}

/**************************************************************************
*  @Function Name :: OnSize
*  @Description   :: �ı�ߴ����Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nType:OnSize���� cx:OnSize����
*					 cy:OnSize����
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
*  @Description   :: ����ͶӰģʽ
*  @Author        :: zzw
*  @Parameters    :: bProjMode:���õ�ͶӰģʽ��TRUE--͸��ͶӰ;FALSE--����ͶӰ
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
*  @Description   :: ���̻��µ���Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nChar:OnKeyDown���� nRepCnt:OnKeyDown����
*					 nFlags:OnKeyDown����
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
*  @Description   :: ���̻��˵���Ӧ����
*  @Author        :: zzw
*  @Parameters    :: nChar:OnKeyUp���� nRepCnt:OnKeyUp����
*					 nFlags:OnKeyUp����
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
