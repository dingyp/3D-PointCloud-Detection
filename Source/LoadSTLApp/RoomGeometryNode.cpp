#include "stdafx.h"
#include "RoomGeometryNode.h"


RoomGeometryNode::RoomGeometryNode()
{
	m_pRoomModelList = NULL;
}


RoomGeometryNode::~RoomGeometryNode()
{
}

void RoomGeometryNode::AttachRoomModel(CModelList * pModellist)
{
	if (NULL == pModellist)
	{
		return;
	}
	m_pRoomModelList = pModellist;

}

void RoomGeometryNode::Draw()
{
	if (FALSE == IsShowed())
	{
		return;
	}

	glPushMatrix();
	LoadMatrix();	// Translate, Rotate, Scale

	glGetDoublev(GL_MODELVIEW_MATRIX, m_DrawMartirx);

	DrawRoom();
	glPopMatrix();
}

void RoomGeometryNode::DrawRoom()
{
	if (NULL == m_pRoomModelList || m_pRoomModelList->GetModelNum() == 0)
	{
		return;
	}

	VECTOR *pVertex = NULL;					//����
	VECTOR *pNormal = NULL;					//������
	VECTOR9ui *pFaces = NULL;				//����
	VECTOR2f *pUVs = NULL;					//�����ζ���

	int MatIndex = -1;
	CModelData * pCurModel;
	if (m_pRoomModelList->GetModelNum()>0)
	{
		for (int j = 0; j < m_pRoomModelList->GetModelNum(); j++)
		{
			glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

			pCurModel = m_pRoomModelList->m_vpModels[j];

			pVertex = pCurModel->GetPVertex();						//���ض������ָ�룬�޸������ֵ��Լ�����޸������ֵ
			pNormal = pCurModel->GetPNormal();						//���ط���������ָ��
			pFaces = pCurModel->GetPTriangles();					//������������ָ��
			pUVs = pCurModel->GetPUVCoordinates();					//���������ζ���ָ��

				glEnable(GL_LIGHTING);
				glEnable(GL_LIGHT0);

				MatIndex = pCurModel->GetMatIndex();
				if (MatIndex >= 0) //�в��ʵĻ�
				{
					SelectMaterial(MatIndex);
				}
				else //ȱʡ����
				{
					float defaultAmb[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
					float defaultDif[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
					float defaultSpec[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
					float defaultem[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, defaultAmb);
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, defaultDif);
					glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, defaultSpec);
					glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, defaultem);
					glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1.0);

				}

				glBegin(GL_TRIANGLES);
				for (int i = 0; i < pCurModel->GetTriNum(); i++)
				{
					glNormal3f(pNormal[pFaces[i].nx].x, pNormal[pFaces[i].nx].y, pNormal[pFaces[i].nx].z);//������
					if (pUVs != NULL)
						glTexCoord2f(pUVs[pFaces[i].uvx].x, pUVs[pFaces[i].uvx].y);//����
					glVertex3f(pVertex[pFaces[i].x].x, pVertex[pFaces[i].x].y, pVertex[pFaces[i].x].z);//��

					glNormal3f(pNormal[pFaces[i].ny].x, pNormal[pFaces[i].ny].y, pNormal[pFaces[i].ny].z);
					if (pUVs != NULL)
						glTexCoord2f(pUVs[pFaces[i].uvy].x, pUVs[pFaces[i].uvy].y);
					glVertex3f(pVertex[pFaces[i].y].x, pVertex[pFaces[i].y].y, pVertex[pFaces[i].y].z);

					glNormal3f(pNormal[pFaces[i].nz].x, pNormal[pFaces[i].nz].y, pNormal[pFaces[i].nz].z);
					if (pUVs != NULL)
						glTexCoord2f(pUVs[pFaces[i].uvz].x, pUVs[pFaces[i].uvz].y);
					glVertex3f(pVertex[pFaces[i].z].x, pVertex[pFaces[i].z].y, pVertex[pFaces[i].z].z);
				}
				glEnd();

			glPopClientAttrib();

		}
	}
}

void RoomGeometryNode::SelectMaterial(int i)
{
	//
	// Define the reflective properties of the 3D Object faces.
	//
	GLfloat alpha = m_pRoomModelList->m_vMaterials[i].alpha;
	MyMaterial(GL_AMBIENT, m_pRoomModelList->m_vMaterials[i].ambient, alpha);
	MyMaterial(GL_DIFFUSE, m_pRoomModelList->m_vMaterials[i].diffuse, alpha);
	MyMaterial(GL_SPECULAR, m_pRoomModelList->m_vMaterials[i].specular, alpha);
	MyMaterial(GL_EMISSION, m_pRoomModelList->m_vMaterials[i].emission, alpha);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_pRoomModelList->m_vMaterials[i].phExp);
	if (m_pRoomModelList->m_vMaterials[i].texture > -1)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_pRoomModelList->m_vTextureMaps[m_pRoomModelList->m_vMaterials[i].texture].id);
	}
	else
		glDisable(GL_TEXTURE_2D);

}

void RoomGeometryNode::MyMaterial(GLenum mode, GLfloat * f, GLfloat alpha)
{
	GLfloat d[4];
	d[0] = f[0];
	d[1] = f[1];
	d[2] = f[2];
	d[3] = alpha;
	glMaterialfv(GL_FRONT_AND_BACK, mode, d);
}