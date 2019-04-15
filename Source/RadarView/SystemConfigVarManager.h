#pragma once
#include "stdafx.h"
#include "IniFile.h"
#include "singleton.h"
#include "FSFileSysHelper.h"

class  SystemConfigVarManager : public fsutility::Singleton<SystemConfigVarManager>	//������в������е�ʵ���������
{
public:
	 SystemConfigVarManager();
	~SystemConfigVarManager();

	BOOL LoadConfig(CString strConfigPath);											//���س�ʼ��������
	int GetMruFile(CString *mruFiles);												//�õ�����򿪵�CString���飬�������ܹ��ĸ���������10��
	void AddMruFile(CString mruFiles);												//���������򿪵��ļ�
	void DelMruFile(CString mruFiles);												//ɾ�������Ѿ������ڵ��ļ�
	BOOL GetMruFile(int fileIdx,CString& mruFile);									//�õ�����ĳһ�±������ļ�
	void GetShortFilePath(CString &longPath,CString &shortPath,int limit_length=80);//�����ļ�·������������ָ����������,Ĭ��80���ַ�
	BOOL GetFileName(const CString &pcapFileName,CString &shortPcapName);			//���ļ�·������õ��ļ���
	BOOL IsFileExist(const CString& csFile);										//�ж��ļ��Ƿ����
	void GetFileSuffixName(CString &suffixName,CString File_path);					//��ȡ�ļ�File_path�ĺ�׺��suffixName
protected:
	fsutility::CIniFile m_ScanConfig;												//��ini�ļ����ж�ȡ��������
private:
	CString mruFilePath[10];														//���10�δ򿪵��ļ�
	int mruFilePathNumber;															//������ļ��ĸ���
};

