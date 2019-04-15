#pragma once

#include <gl/gl.h>
#include <gl/glu.h>

#include "Vector.h"
#include "SceneNode.h"
#include "Enum.h"
#include "Material.h"
#include "MaterialFront.h"
#include "MaterialBack.h"
#include "Texture2D.h"
#include "Trackball.h"
#include "MouseSelect.h"

class AFX_EXT_CLASS SceneObjectNode :  public SceneNode
{
public:
	SceneObjectNode(void);
	virtual ~SceneObjectNode(void);

	virtual int getType(void){return SCENEOBJECT_NODE;}
	virtual void Draw();
	// Ϊ Mouse Select �������Ƶ�ǰ�ڵ�
	virtual void DrawForMouseSelect(void);

	virtual void FinalRelease(void);

	virtual void ComputeBoundBox(void) {};
protected:
	virtual  void   DrawObject(void); //����ľ�����ƺ������ڸ�����Ϊ�գ��ڼ̳����о���ʵ��
	virtual  void   RenderStart(void); //����֮ǰ�ĳ�ʼ��
	virtual  void   RenderEnd(void); //����֮��Ĺ���

public:
	Material * getMaterial(void);
	void setMaterial(Material *mat);

	MaterialFront *getFrontMaterial(void);
	void setFrontMaterial(MaterialFront *mat);

	MaterialBack *getBackMaterial(void);
	void setBackMaterial(MaterialBack *mat);

	void setPolygonMode(GLenum Mode = GL_FILL);

	void setTransparent(BOOL transparent); //��������͸������
	void setBlendFactor(GLenum glSRCFactor, GLenum glDSTFactor); //�����ں�����

	void getDrawMartrix(double m[16]);

	BOOL IsEnableAlphaTest() {return m_bAlphaTest;}
	void setAlphaFunc(GLenum func, GLclampf ref);
	void setAlphaTest(BOOL istest);

protected:
	GLenum      m_eBlendSRCFactor;  //͸��������Դ�ں�����
	GLenum      m_eBlendDSTFactor;  //͸��������Ŀ���ں�����

	Material	  * m_pMaterial; //��������ָ��
	MaterialFront * m_pFrontMaterial; //�����������ָ��
	MaterialBack  * m_pBackMaterial;  //�����������ָ��

	double        m_DrawMartirx[16];

	BOOL	   m_bAlphaTest; //�Ƿ�ʹ��alpha���ԣ�04��8��23��add
	GLenum	   m_eAlphaFunc; //alpha���Ժ�����ѡ��GL_NEVER��GL_LESS��GL_EQUAL��GL_LEQUAL
	//GL_GREATER��GL_NOTEQUAL��GL_GEQUAL��GL_ALWAYS�����glAlphaFunc
	//SceneGraph��ȱʡʹ��GL_GEQUAL
    GLclampf   m_fAlphaRef; //alpha�������ֵ��0��1֮��
	//SceneGraph��ȱʡʹ��0����ȱʡ�����alpha���Բ�������

	GLenum      m_ePolygonMode;//����Ļ��Ʒ�ʽ
};
