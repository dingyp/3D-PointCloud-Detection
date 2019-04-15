#pragma once
#include "GeometryNode.h"

class AFX_EXT_CLASS TexGeometryNode :public GeometryNode
{
public:
	virtual int getType(void){return TEX_GEOMETRY_NODE;}
	virtual void FinalRelease(void);

	virtual ~TexGeometryNode(void);
	TexGeometryNode(void);

protected:
	void	ComputeTexCoords(void);//�����û���ʵ����Ҫȷ����������

	virtual void  RenderStart(void); //����֮ǰ�ĳ�ʼ��

public:
	Texture2D * getTexture(void);
	void setTexture( Texture2D *texture);
	void setTexCoord(unsigned int index, float t0, float t1);
	void setTexGenPara(GLuint genPara = GL_OBJECT_LINEAR);

protected:
	BOOL		m_bTexCoordGen;   //�Ƿ�ʹ���Զ����������־��ȱʡΪTRUE��������������ʱ��ΪFALSE

	Texture2D   * m_pTexture;  //����ָ��
	float       * m_pTexCoord;	// ��������ָ��
	GLuint		m_TexGenPara; //�Զ������������ɷ�ʽ
};
