#pragma once
#include "../SceneGraph/SceneGroup.h"
#include "../SceneGraph/PathNode.h"

class MeasureLineNode :public SceneGroup
{
public:

	MeasureLineNode(void);											//MeasureLineNode���Ĭ�Ϲ��캯��
	MeasureLineNode(UINT TextListID);								//MeasureLineNode��Ĵ���TextListID�Ĺ��캯��
	virtual ~MeasureLineNode();										//MeasureLineNode�����������
	virtual void Draw();											//���Ʋ�����ֱ��
	void SetBeginPoint(VECTOR beginp);								//���ò���ֱ�ߵ���ʼ��
	void SetEndPoint(VECTOR endp);									//���ò���ֱ�ߵ����˵�
	void SetLineWidth(float width);									//����ֱ�ߵĿ��
	void SetLineColor(float red, float green, float blue);			//����ֱ�ߵ���ɫ
	void SetText(CString str);										//���������ı�
	//��Postion�������Sphere��λ��
	void SetTextPostion(float px, float py, float pz);				//�����ı���λ��
	void SetTextColor(float red, float green, float blue);			//�����ı�����ɫ
	void SetTextSize(float sx, float sy, float sz);					//�����ı��ĳߴ�
	void RotateText(float angle, float x, float y, float z);		//����text����ת����

protected:
	PathNode * m_Line;												//������ֱ��
	TextNode * m_text;												//����������ı�
};

