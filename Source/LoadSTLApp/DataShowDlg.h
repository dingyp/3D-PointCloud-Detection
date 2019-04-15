#pragma once

class DataShowDlg : public CDialogBar
{
	DECLARE_DYNAMIC(DataShowDlg)
public:
	DataShowDlg(CWnd* pParent =NULL);
	~DataShowDlg();

	void ShowInitGrid();																			//չʾ��ʼ�ı��
	void SetShowValue(int PropertyNumber, int SubItemNumber, CString setValue);						//������ʾ����
	void SetShowValue(int PropertyNumber, int SubItemNumber, int setValue);							//���ظ�����ʾ����
	void SetShowValue(int PropertyNumber, int SubItemNumber, double setValue);						//���ظ�����ʾ����
	void AddGridProperty(CMFCPropertyGridProperty *set_pProp, int PropertyNumber, int SubItemNumber);		//��pProp�ӵ���PropertyNumber��SubItemNumber����
	void DelGridProperty(int PropertyNumber, int SubItemNumber);									//ɾ����������

protected:
	BOOL m_bMenuRemoved;																			//�����Ƿ��ƶ�����־λ

	virtual void DoDataExchange(CDataExchange* pDX);												// DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);										//����λ�÷����仯����Ӧ����
	afx_msg void OnSize(UINT nType, int cx, int cy);												//�������ڲ��ؼ��Ĵ�С

	//afx_msg void OnButton();
private:
};

