#pragma once
#include "StructDefine.h"
#include "..\SceneGraph\SceneObjectNode.h"
#include <Gl/GL.h>


class RadarAxisNode : public SceneObjectNode
{
public:
	RadarAxisNode(UINT TextListID);
	virtual ~RadarAxisNode();
	virtual int getType(void) { return RADARAXIS_NODE; }//�������
	virtual void Draw();								//���������������
	void SetLineLength(double setLineLength);			//����������ĳ���

protected:
	void DrawAxis();									//�����������ǣ����
	void DrawArrows();									//����������ļ�ͷ

private:
	void GenerateArrows();								//����Բ׶��
	void DestroyPointers();								//������������������

protected:
	UINT		m_TextListID;							//�ı��б���ID
	float		m_fLineLenth;							//������ĳߴ��С
	VECTOR		* m_pNormalsX;							//X������ָ��
	VECTOR		* m_pNormalsY;							//Y������ָ��
	VECTOR		* m_pNormalsZ;							//Z������ָ��
	VECTOR		* m_pVerticesX;							//X��������ָ��
	VECTOR		* m_pVerticesY;							//Y��������ָ��
	VECTOR		* m_pVerticesZ;							//Z��������ָ��
	unsigned int  m_iTriNum;							//ÿ��Բ׶��������Ŀ
};