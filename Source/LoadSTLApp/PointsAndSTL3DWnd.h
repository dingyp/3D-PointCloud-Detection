#pragma once
#include "..\SceneGraph\Scene3DMessageWnd.h"
#include "StructDefine.h"
#include "RadarPointsNode.h"
#include "Transform.h"
#include "afxmt.h"
#include <stdio.h>
#include "RadarAxisNode.h"
#include "STL_GeometryNode.h"
#include "RoomGeometryNode.h"

using namespace DataProcess;

class PointsAndSTL3DWnd :																	//�Խ�������ʾ�ĵ��Լ�STL���й������
	public Scene3DMessageWnd
{
public:
	PointsAndSTL3DWnd();																	//Points3DWnd���Ĭ�Ϲ��캯��
	virtual ~PointsAndSTL3DWnd();															//Points3DWnd���Ĭ����������
	void SetProjMode(BOOL bProjMode);														//����ͶӰģʽ

protected:
	BOOL  m_bLButtonDown;																	//������±�־
	BOOL  m_bRButtonDown;																	//�Ҽ����±�־
	BOOL  m_bCtrlDown;																		//ctrl�����±�־
	float m_fAspect;																		//���ڵĿ���
	PointArray m_PointList;																	//��ʾ����б�
	RadarAxisNode * m_AxisNode;																//��������Ĳ�����
	BOOL  m_bCurProjMode;																	//ͶӰģʽ��TRUE--͸��ͶӰ;FALSE--����ͶӰ
	E3DMouseMode m_MouseMode;																//���ģʽ
	Transform transformer;														//��������任������

public:
	SceneGroup * section_group;																//���нڵ㶼�����������
	RadarPointsNode * m_RadarPointsNode;													//�״�ĵ��ƽڵ㼯
	STL_GeometryNode * m_STLGeometryNode;													//�����STL�ļ���ȡ��ʾ������
	RoomGeometryNode * m_pRoomNode;															//�����obj�ļ���ȡ��ʾ������

	CWnd *m_pParentWnd;																		//������ָ��
	double m_rangex, m_rangey, m_rangez;													//��Χ,��0����Զ���룬������Ӱ�������С
	double m_rangeTotal;																	//���յķ�Χ������x��y��z��Ӱ��
	float eyex, eyey, eyez;																	//������Ұ��λ��

protected:
	void InitScene();																		//��ʼ����Ұ����
	void SetupWndView();																	//�������ô��ڵ���Ұ

public:
	virtual BOOL Create(CRect rect, CWnd* pParent, UINT id);								//����3D����
	void ClearData();																		//���������ʾ����
	void SetSensorCenter(double sensor_x, double sensor_y, double sensor_z);				//���ô��������ĵ�����
	void SetSensorEuler(double sensor_pitch, double sensor_roll, double sensor_yaw);		//���ô���������̬��
	void GetSensorCenter(double &sensor_x, double &sensor_y, double &sensor_z);				//��ȡ�����������ĵ�����
	void GetSensorEuler(double &sensor_pitch, double &sensor_roll, double &sensor_yaw);		//��ȡ����������̬��
	void SetValueRange(double MinValue, double MaxValue);									//������ֵ�������С,�����ָ����ֵ
	void GetValueRange(double &MinValue, double &MaxValue);									//��ȡ�����Ե������Сֵ
	void ResetView();																		//������Ұ�ķ���
	void SetBoundBox(double x1, double x2, double y1, double y2, double z1, double z2);		//������ʾ�����BoundBox
	void GetBoundBox(double &x1, double &x2, double &y1, double &y2, double &z1, double &z2);//�Զ����BoundBox������Ĭ�ϵ�size,������������
	void SetAxisNodeSize(double setLength);													//����������Ĵ�С

	void AttachDataList(CDataList * pDataList);												//���STL����list
	void SetPolygonMode(GLenum Mode);														//���ñ���Ļ��Ʒ�ʽ
	void AttachRoomModel(CModelList * pModellist);											//���OBJ����
	void AttachPointClouds(CPointClouds *pPointClouds);										//��ӵ�������

	void AfterRigidRegistration();															//�ڵ��ƺ�STL��׼֮�������

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);									//�������������µ���Ӧ����
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);									//�������������˵���Ӧ����
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);									//����ƶ�����Ӧ����
	afx_msg void OnSize(UINT nType, int cx, int cy);										//�ı�ߴ����Ӧ����
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);						//�����ֻ�������Ӧ����
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);								//������˫������Ӧ����
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);									//����Ҽ��������µ���Ӧ����
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);									//����Ҽ��������˵���Ӧ����
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);							//���̻��µ���Ӧ����
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);							//���̻��˵���Ӧ����
};

