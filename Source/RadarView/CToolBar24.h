#pragma once
// EnBitmap.h: CEnBitmap��Ľӿ�
#if _MSC_VER > 1000
#pragma once
#endif
#include "CEditVC.h"

class CToolBar24 : public CToolBar
{
	DECLARE_DYNAMIC(CToolBar24)
public:
	CToolBar24();
	virtual ~CToolBar24();

	BOOL Init(UINT BITMap_ID, UINT Disabled_ID, UINT Hot_ID);		//��Toolbar��״̬���г�ʼ��
	void UpdateColorImage(UINT BITMap_ID, UINT Disabled_ID, UINT Hot_ID, int idx);
	void UpdateColorImage(UINT BITMap_ID, UINT Disabled_ID, int idx);//�����������е�idx����ť��ͼ��Ϊbitmap�е�idx��ͼ��

protected:
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);		//������λ�÷����仯����Ӧ����
	DECLARE_MESSAGE_MAP()

public:
	RECT rect;														//��������λ��

protected:
	CImageList m_imgToolbar;										//һ��״̬�¹�������ʾͼ�б�
	CImageList m_imgToolbarDis;										//disable״̬�¹�������ʾͼ�б�
	CImageList m_imgToolbarNA;										//�ȼ�״̬�¹�������ʾͼ�б�
	CBitmap m_bmToolbar;											//һ��״̬�¹�����λͼ
	CBitmap m_bmToolbarDis;											//disable״̬�¹�����λͼ
	CBitmap m_bmToolbarNA;											//�ȼ�״̬�¹�����λͼ

	BOOL m_bMenuRemoved;											//�������Ƿ��ƶ�����־λ
};

//���Ź�����
class CPlayToolBar :public CToolBar24
{
public:
	CPlayToolBar();
	~CPlayToolBar();

	BOOL InitSlider(UINT ID_SLIDER, UINT ID_FrameEdit);				//��PlayToolbar��slider���г�ʼ��
	void SetFrameNumber(UINT frameNumber);							//������ʾ��ͻ�������֡�����
	void InitFrameNumber(UINT totalFrameNumber);					//��ʼ����֡�����û��������Խ��л���
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);//���ŵĽ�����������
	DECLARE_MESSAGE_MAP()
private:
	CSliderCtrl playSlider;											//����������
	CEditVC frameEdit;												//֡����ʾ��
};

//��ɫ������
class CColorToolBar :public CToolBar24
{
public:
	CColorToolBar();
	~CColorToolBar();

	BOOL InitColorAttribute(UINT ID_Attribute);						//��ʼ����ɫ����
	void SetAttriComBoxCurSel(int setSel, bool bUpdate=false);		//����AttributeComBox��ֵ,update��ʾ�Ƿ�redraw
	afx_msg void OnSelchangeCombo();								//combobox����Ӧ�¼�
	DECLARE_MESSAGE_MAP()
private:
	CComboBox AttributeComBox;										//����������
};