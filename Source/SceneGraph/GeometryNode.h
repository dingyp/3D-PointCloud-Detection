// GeometryNode.h: interface for the GeometryNode class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "SceneObjectNode.h"

class AFX_EXT_CLASS GeometryNode : public SceneObjectNode  
{
public:
	virtual int getType(void){return GEOMETRY_NODE;}

	virtual ~GeometryNode(void);
	GeometryNode(void);

protected:
	void		ComputeNormals(void);//���ݸ����Ķ�����������������㷨����

	virtual void ComputeBoundBox(void);//���ݶ������ݼ����Χ��
	virtual void RenderStart(void); //����֮ǰ�ĳ�ʼ�����Ȼ��������ComputeNormals������֮ǰ���㷨����
	virtual void DrawObject(void); //����ľ�����ƺ���

	virtual void DrawBoundBox(); //���ư�Χ��

public:
	void setPVertices(VECTOR *pVertices,unsigned int iVerNum);
	void setPIndices(VECTOR3ui *pIndices,unsigned int iTriNum);
	void setPNormals(VECTOR *pNormals, unsigned int iVerNum);
	void setIndexed(BOOL  indexed);

protected:
	BOOL		m_bVerticesSeted; //����ָ���Ƿ��Ѿ����ñ�־
	BOOL		m_bIndexed;       //�Ƿ�ʹ�������ı�־
	BOOL		m_bNormalSeted;   //�������Ƿ��趨��־��ȱʡΪFALSE���ɳ�����㣬�趨������ʱ��ΪTRUE


	VECTOR3ui	* m_pIndices;  //��������ָ��
	VECTOR		* m_pNormals;  //������ָ��
	VECTOR		* m_pVertices; //��������ָ��
	unsigned int  m_iVerNum;     //������Ŀ
	unsigned int  m_iTriNum;     //��������Ŀ
};

