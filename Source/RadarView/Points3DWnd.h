#pragma once
#include "..\SceneGraph\Scene3DMessageWnd.h"
#include "StructDefine.h"
#include "RadarPointsNode.h"
#include "BaseLinesNode.h"
#include "Transform.h"
#include "afxmt.h"
#include <stdio.h>
#include "RadarAxisNode.h"
#include <algorithm>
using namespace std;
class Points3DWnd :																			//�Խ�������ʾ�ĵ㡢�ߡ�����Ƚ��й������
	public Scene3DMessageWnd
{
public:
	Points3DWnd();																			//Points3DWnd���Ĭ�Ϲ��캯��
	virtual ~Points3DWnd();																	//Points3DWnd���Ĭ����������
	void SetMeasureStatus(E3DMouseMode m_MouseMode);										//���ò�����״̬
	void SetProjMode(BOOL bProjMode);														//����ͶӰģʽ

protected:
	BOOL  m_bLButtonDown;																	//������±�־
	BOOL  m_bRButtonDown;																	//�Ҽ����±�־
	BOOL  m_bCtrlDown;																		//ctrl�����±�־
	float m_fAspect;																		//���ڵĿ���
	PointArray m_PointList;																	//��ʾ����б�
	int m_DblClkPointIdx;																	//������һ��˫������Mark���±��λ�ã�-1����û�У�0...������������±�
	BaseLinesNode * m_BaseLineNode;															//������Ĳ�����
	RadarAxisNode * m_AxisNode;																//��������Ĳ�����
	CCriticalSection m_CriticalSection;														//����ͬ������
	BOOL  m_bCurProjMode;																	//ͶӰģʽ��TRUE--͸��ͶӰ;FALSE--����ͶӰ
	E3DMouseMode m_MouseMode;																//���ģʽ
	DataProcess::Transform transformer;														//��������任������

public:
	SceneGroup * section_group;																//���нڵ㶼�����������
	RadarPointsNode * m_RadarPointsNode;													//�״�ĵ��ƽڵ㼯
	CWnd *m_pParentWnd;																		//������ָ��
	double m_rangex, m_rangey, m_rangez;													//��Χ,��0����Զ���룬������Ӱ�������С
	double eyex, eyey, eyez;																//������Ұ��λ��
	double        m_GridX, m_GridY,m_GridZ;													//�������������
	double m_GridLength;																	//����ı߳�
	unsigned int m_GridResolution;															//����ķֱ���
	double m_GridRadius;																	//����İ뾶
	unsigned int m_GridConCircle;															//����ͬ��Բ�ĸ���
	double m_GridWidth;																		//���ߵĴ�ϸ
	VECTOR m_GridColor;																		//�������ɫ
	LINEDRAW_TYPE m_bGridType;																//���������

protected:
	void InitScene();																		//��ʼ����Ұ����
	void AddMark(VECTOR pos, int id);														//����ʾ������������id�ĵ�
	void DeleteMark(int id);																//ɾ�����������id�ĵ�
	void AddMeasureLine(VECTOR pt3DFrm, VECTOR pt3DTo, int id);								//�����������ߵĴ�ϸlinewidth
	void AddLine(VECTOR pt3DFrm, VECTOR pt3DTo, int id, float linewidth);					//����ʾ��������һ����
	void DeleteLine(int id);																//ɾ�������id�ĵ�
	void ClearAllMark();																	//�����ģʽ
	BOOL GetCurent3DPosition(CPoint point, RadarPoint &pos, VECTOR &vector);				//��ȡ����ѡ��λ������ĵ������
	double Distance(double x, double y, double z);											//����ĳһ���굽ԭ��ľ���
	void SetupWndView();																	//�������ô��ڵ���Ұ

public:
	virtual BOOL Create(CRect rect, CWnd* pParent, UINT id);								//����3D����
	void AttachData(RadarPointList &PointList);												//������ʾ�ĵ�֡
	bool GetPointListData(RadarPointList &PointList);										//�õ�PointList
	void RefreshData(double addX, double addY, double addZ, double pitch, double roll, double yaw);//����ʾ�����ݽ��и���
	void ClearData();																		//���������ʾ����
	void ClearLinesAndPoints();																//������е���
	void SetSensorCenter(double sensor_x, double sensor_y, double sensor_z);				//���ô��������ĵ�����
	void SetSensorEuler(double sensor_pitch, double sensor_roll, double sensor_yaw);		//���ô���������̬��
	void GetSensorCenter(double &sensor_x, double &sensor_y, double &sensor_z);				//��ȡ�����������ĵ�����
	void GetSensorEuler(double &sensor_pitch, double &sensor_roll, double &sensor_yaw);		//��ȡ����������̬��
	void SetValueRange(double MinValue, double MaxValue);									//������ֵ�������С,�����ָ����ֵ
	void GetValueRange(double &MinValue, double &MaxValue);									//��ȡ�����Ե������Сֵ
	void SetGridThickness(float LineWidth);													//�������ߴ�ϸ
	void SetGridColor(VECTOR gridColor);													//����������ɫ,�������ɫ����Ҫ���й�һ��
	void SetGridCenter(float addx, float addy,float addz);									//�����������ĵ�
	void SetGridParameters(float LineDistance, int LineResolution);							//��������Ĳ���
	void GetGridParameters(float &LineDistance, int &LineResolution);						//��ȡ����Ĳ���
	void SetCircleParameters(float LineDistance, int Circlenumber);							//����ͬ��Բ�Ĳ���
	void GetCircleParameters(float &LineDistance, int &Circlenumber);						//��ȡͬ��Բ�Ĳ���
	void SetGridDir(LINEPLANE_TYPE eLinePlaneType);											//����������
	void SetDir(VIEW_DIR viewDir);															//������Ұ����
	void SetGridType(LINEDRAW_TYPE eLineDrawType);											//������������
	LINEDRAW_TYPE GetGirdType();															//��ȡ��������
	void ResetView();																		//������Ұ�ķ���
	void SetColorMode(int set_ColorMode);													//������ɫ�����ģʽ
	int GetColorMode();																		//��ȡ��ɫ�����ID��
	void ReScaleColorRange();																//���²�����ֵ�ķ�Χ������ɫ���½��б���
	void SetColorCoding(COLORCODING chooseColorCoding, bool bUpdate);						//������ɫ���������,bUpdate��ʾ�Ƿ��ֳ�����
	void GetColorCoding(COLORCODING &ColorCoding);											//��ȡ��ɫ���������
	void TransformPointInformation(RadarPoint pos);											//��״̬����2������͹��ڵ����Ϣ
	void SetBoundBox(double x1, double x2, double y1, double y2, double z1, double z2);		//������ʾ�����BoundBox
	void GetDefaultBoundBox(double &GridLength, double &GridRadius);						//�Զ����BoundBox������Ĭ�ϵ�size,������������
	void SetAxisNodeSize(double setLength);													//����������Ĵ�С
	void SetAxisNodeIsShow(bool setIsDraw);													//�����������Ƿ���ʾ
	void GetAxisNodeIsShow(bool &setIsDraw);												//��ȡ�������Ƿ���ʾ��־λ
	void EnLargeView();																		//�Ŵ���Ұ����
	void ReduceView();																		//��С��Ұ����

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
