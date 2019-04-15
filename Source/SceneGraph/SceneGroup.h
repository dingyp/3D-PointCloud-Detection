// SceneGroup.h: interface for the SceneGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEGROUP_H__E75E9D4E_626C_4740_A149_6EA7C1F0598F__INCLUDED_)
#define AFX_SCENEGROUP_H__E75E9D4E_626C_4740_A149_6EA7C1F0598F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneNode.h"
#include "TextNode.h"

//// GroupNode �е��������ݽṹ
class GroupLink
{
public:
	GroupLink(void);
	~GroupLink(void);
	void SetSceneNode(SceneNode * node);

public:
	GroupLink *m_pNext;
	SceneNode *m_pSceneNode;
};

class AFX_EXT_CLASS SceneGroup : public SceneNode  
{
public:
	SceneGroup();
	virtual ~SceneGroup();

	//��õ�ǰ�ڵ�����
    virtual int getType(){return GROUP_NODE;}

	// ������Ϊ name �Ľڵ㣨���� Group��
	virtual SceneNode*	Search(char *name);	

	//����
	virtual void Draw(void);
	//���ư�Χ��
	//virtual void DrawBoundBox(void);

	// Ϊ Mouse Select �������Ƶ�ǰ�ڵ�
	virtual void DrawForMouseSelect(void);

	void	AddChild(SceneNode *child);			// �����ӽڵ�
    void	setChild(int i, SceneNode *child);	// ���õ� i ���ӽڵ�

    int		getCount();					// ��ȡ�ӽڵ�����
    int		getChildIndex(char *name);	// ��ȡ��Ϊ name �ӽڵ��������
	SceneNode*	getChild(int i);		// ��ȡ�� i ���ӽڵ�
	SceneNode*	getChild(char *name);	// ��ȡ��Ϊ name �ӽڵ�

    void	DeleteChild(int i);			// ɾ���� i ���ӽڵ�
    void	DeleteChild(char *name);	// ɾ����Ϊ name �ӽڵ�

protected:
	//  �����ͷ������ݵĶ���
	virtual void FinalRelease(void);

public:
	GroupLink * m_pListHead;
};


#endif // !defined(AFX_SCENEGROUP_H__E75E9D4E_626C_4740_A149_6EA7C1F0598F__INCLUDED_)
