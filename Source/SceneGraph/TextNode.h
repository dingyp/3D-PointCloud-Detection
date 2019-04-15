#pragma once
#include "scenenode.h"
#include "GLFont.h"
#include "Enum.h"

class AFX_EXT_CLASS TextNode : public SceneNode
{
public:
	TextNode(UINT TextListID);
	virtual ~TextNode(void);

	virtual int getType(void){return TEXT_NODE;}
	virtual void Draw();

	void	SetTextColor(float red, float green, float blue);

	void	SetText(const char *str);
	void    SetText(CString str);

protected:
	UINT		 m_TextListID;

	float		 m_TextColorRed;  //red ����
	float		 m_TextColorGreen; //green����
	float		 m_TextColorBlue; //blue ����

	CString      m_strText;      //the text for display
};
