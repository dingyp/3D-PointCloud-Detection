#pragma once

//�������ʵ��CEdit��ˮƽ���У����ڲ��Ź���������֡������ʾ

class CEditVC : public CEdit
{
	DECLARE_DYNAMIC(CEditVC)
public:
	CEditVC();																		//CEditVC��Ĺ��캯��
	virtual ~CEditVC();																//CEditVC�����������

protected:
	CRect m_rcClient;																//��¼��ɫ�Ŀ�

protected:
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);	//�������ı߿���
	afx_msg void OnNcPaint();														//ʵ�ֻ��ƴ���
	afx_msg UINT OnGetDlgCode();													//���ÿؼ����Ա�ؼ����Դ�������tab��������
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);							//CEditVC����create���������õ�ʱ�����

	DECLARE_MESSAGE_MAP()

	//ʹ�øú��������ѡ������ָ���ַ����ĸ߶ȺͿ��
	BOOL GetTextExtent(HDC hDC, LPCSTR lpString, int nCount, HFONT hFont, LPSIZE lpSize, int iDir = 0);
};

