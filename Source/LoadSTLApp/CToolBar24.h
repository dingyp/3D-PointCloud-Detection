#pragma once
// EnBitmap.h: CEnBitmap��Ľӿ�

class CToolBar24 : public CToolBar
{
	typedef struct
	{
		UINT Width, Height, Colors;
		LPBYTE lpBits;
		DWORD dwNumBytes;
		LPBITMAPINFO lpbi;
		LPBYTE lpXOR;
		LPBYTE lpAND;
	}ICONIMAGE, *LPICONIMAGE;

	DECLARE_DYNAMIC(CToolBar24)
public:
	CToolBar24();
	virtual ~CToolBar24();

	BOOL Init(UINT BITMap_ID, UINT Disabled_ID, UINT Hot_ID);		//��Toolbar��״̬���г�ʼ��
	void UpdateColorImage(UINT Icon_ID, UINT Disabled_ID, UINT Hot_ID, int idx);
	void UpdateColorImage(UINT BITMap_ID, UINT Disabled_ID, int idx);//�����������е�idx����ť��ͼ��Ϊbitmap�е�idx��ͼ��

protected:
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
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