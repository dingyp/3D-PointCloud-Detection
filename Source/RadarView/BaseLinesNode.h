#pragma once
#include "..\SceneGraph\SceneObjectNode.h"
#include "StructDefine.h"

class BaseLinesNode :	public SceneObjectNode
{
public:
	BaseLinesNode();
	virtual ~BaseLinesNode();
	virtual void Draw();													//����������
	void SetLineWidth(float LineWidth) { m_fLineWidth = LineWidth; }		//�������ߵĴ�ϸ
	void SetGridCenter(float SetX, float SetY, float SetZ);					//�������������
	void SetGridColor(VECTOR SetGridColor);									//����������ɫ
	void SetLineParameters(float LineDistance, float LineStep);				//���������С
	void SetLinePlane(LINEPLANE_TYPE eLinePlaneType);						//����������
	void SetLineDraw(LINEDRAW_TYPE eLineDrawType);							//������������
	void SetIsShowed(bool setIsDraw);										//���������Ƿ���ʾ
	void GetIsShowed(bool &setIsDraw);										//��ȡ�����Ƿ���ʾ��־λ

protected:
	void DrawLines();														//��������
	void GenerateGridLines();												//���ɸ�����
	void GenerateConCricles();												//����ͬ��Բ

protected:
	VECTOR m_vColor;														//������ɫ
	float  m_fLineWidth;													//���ߵĿ��
	float  m_fLineDistance;													//���߿�ı߳�
	float  m_fLineStep;														//���ߵļ��
	LineArray m_LineList;													//���ߵ��б�
	LINEPLANE_TYPE m_eLinePlaneType;										//������
	LINEDRAW_TYPE m_eLineDrawType;											//��������
	float addx, addy, addz;													//�������������
};

