#include "stdafx.h"
#include "STL_GeometryNode.h"

STL_GeometryNode::STL_GeometryNode()
{
	m_ePolygonMode = GL_FILL;
	m_pDataList = NULL;
}
STL_GeometryNode::~STL_GeometryNode()
{
}
void STL_GeometryNode::Draw()
{
	if (FALSE == IsShowed())
	{
		return;
	}
	if (NULL == m_pDataList)
	{
		return;
	}

	glPushMatrix();

	int index = 0;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, m_pDataList->m_pVertex);//��һ��������ͼ�εĶ��������һ��
	glNormalPointer(GL_FLOAT, 0, m_pDataList->m_pNormal);

	//glDrawArrays(GL_TRIANGLES, 0, m_iVertexNum);
	glBegin(GL_TRIANGLES);//�˴����������εĻ���
	for (unsigned int i = 0; i < m_pDataList->m_iTriNum; i++)
	{
		glArrayElement(index);//ָ����������һ�����������Ԫ��
		index++;
		glArrayElement(index);
		index++;
		glArrayElement(index);
		index++;
	}
	glEnd();

	glPopMatrix();

}

void STL_GeometryNode::SetPolygonMode(GLenum Mode)
{
	m_ePolygonMode = Mode;
}

void STL_GeometryNode::AttachDataList(CDataList * pDataList)
{
	if (NULL == pDataList)
	{
		return;
	}
	m_pDataList = pDataList;

}
