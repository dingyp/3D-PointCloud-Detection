#include "stdafx.h"
#include "SystemConfigVarManager.h"
#include "SysPathManager.h"
#include "MainFrm.h"

/**************************************************************************
*  @Function Name :: SystemConfigVarManager
*  @Description   :: SystemConfigVarManager��Ĺ��캯��
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
SystemConfigVarManager::SystemConfigVarManager()
{
	mruFilePathNumber = 0;//��ʼ�������ÿ�
}

/**************************************************************************
*  @Function Name :: ~SystemConfigVarManager
*  @Description   :: SystemConfigVarManager�����������
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
SystemConfigVarManager::~SystemConfigVarManager()
{
}

/**************************************************************************
*  @Function Name :: LoadConfig
*  @Description   :: ����ini�ļ��е����ò���
*  @Author        :: zzw
*  @Parameters    :: strConfigPath:�ļ�·��
*
*  @Return        :: �����Ƿ���سɹ�
**************************************************************************/
BOOL SystemConfigVarManager::LoadConfig(CString strConfigPath)
{
	if (!PathFileExists(strConfigPath))
	{//�����ڽ��д���,open�����д�������˼
		m_ScanConfig.Open(strConfigPath);
		return FALSE;
	}

	if (m_ScanConfig.Open(strConfigPath))
	{
		//��ȡ����������ļ�
		m_ScanConfig.Move2Section(_T("Mru File"));
		for (mruFilePathNumber = 0; mruFilePathNumber < 10; mruFilePathNumber++)
		{
			CString strIndex;
			strIndex.Format(_T("%d"), mruFilePathNumber);

			m_ScanConfig.ReadKey(strIndex, mruFilePath[mruFilePathNumber]);
			if (mruFilePath[mruFilePathNumber].IsEmpty())
			{
				break;
			}
		}

	}
	return TRUE;
}

/**************************************************************************
*  @Function Name :: GetMruFile
*  @Description   :: �õ�����򿪵�CString���飬�������ܹ��ĸ���������10
*  @Author        :: zzw
*  @Parameters    :: mruFiles:������ļ�·������
*
*  @Return        :: ��������ļ�����Ŀ
**************************************************************************/
int SystemConfigVarManager::GetMruFile(CString *mruFiles)
{
	for (int i = 0; i < mruFilePathNumber; i++)
	{
		mruFiles[i] = mruFilePath[i];
	}
	return mruFilePathNumber;
}

/**************************************************************************
*  @Function Name :: DelMruFile
*  @Description   :: ɾ�������Ѿ������ڵ��ļ�
*  @Author        :: zzw
*  @Parameters    :: mruFiles:����ļ�
*
*  @Return        :: None
**************************************************************************/
void SystemConfigVarManager::DelMruFile(CString mruFiles)
{
	//Ѱ���ļ��Ƿ���·������
	int idx;
	for (idx = 0; idx < mruFilePathNumber; idx++)
	{
		if (mruFilePath[idx].Compare(mruFiles) == 0)
		{//���
			break;
		}
	}
	//�ڵĻ�����ɾ�������Ѻ������ǰ�ƶ�
	if (idx >= mruFilePathNumber)
	{//û���ҵ�
		return;
	}
	//�����idxɾ������֮�����ǰ�ƶ�
	for (int i = idx; i < mruFilePathNumber-1; i++)
	{
		mruFilePath[i] = mruFilePath[i + 1];//��ǰ�ƶ�һλ
	}
	mruFilePathNumber--;

	//����ini�ļ�
	m_ScanConfig.Move2Section(_T("Mru File"));
	for (int i = 0; i < mruFilePathNumber; i++)
	{//д��ȥ�Ĳ�����ʵ������������
		CString strIndex;
		strIndex.Format(_T("%d"), i);
		m_ScanConfig.WriteKey(strIndex, mruFilePath[i]);
	}

	//ɾ�����е�
	CString cstmp;
	cstmp.Format(_T("%d"), mruFilePathNumber);
	CString strSysConfigFile = fsutility::CSysPathManager::Instance().GetExePath() + _T("config\\setting.ini");
	WritePrivateProfileString(_T("Mru File"), cstmp, NULL, strSysConfigFile);

	//�����Ҫ�Բ˵���������ļ����и���
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->UpdateMenuMruFile(mruFilePath, mruFilePathNumber);
}

/**************************************************************************
*  @Function Name :: AddMruFile
*  @Description   :: ��������򿪵��ļ�
*  @Author        :: zzw
*  @Parameters    :: mruFiles:������ļ�
*
*  @Return        :: None
**************************************************************************/
void SystemConfigVarManager::AddMruFile(CString mruFiles)
{
	//�����жϼ�����ļ�֮ǰ��û�д򿪹����򿪹��Ļ��Ƿ���Ҫ�ƶ�����
	int idx;
	for (idx = 0; idx < mruFilePathNumber; idx++)
	{
		if (mruFilePath[idx].Compare(mruFiles) == 0)
		{//���
			break;
		}
	}
	//֮ǰû�д򿪹������������ƶ�
	for (int i = (idx == mruFilePathNumber) ? ((mruFilePathNumber < 10) ? (mruFilePathNumber) : (9)) : (idx); i >0; i--)
	{
		mruFilePath[i] = mruFilePath[i - 1];//�����ƶ�һλ
	}
	mruFilePath[0] = mruFiles;
	if ((idx == mruFilePathNumber)&&(mruFilePathNumber < 10))
	{
		mruFilePathNumber++;
	}

	//����ini�ļ�
	m_ScanConfig.Move2Section(_T("Mru File"));
	for (int i = 0; i < mruFilePathNumber; i++)
	{//д��ȥ�Ĳ�����ʵ������������
		CString strIndex;
		strIndex.Format(_T("%d"), i);
		m_ScanConfig.WriteKey(strIndex, mruFilePath[i]);
	}

	//�����Ҫ�Բ˵���������ļ����и���
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->UpdateMenuMruFile(mruFilePath, mruFilePathNumber);
}

/**************************************************************************
*  @Function Name :: GetMruFile
*  @Description   :: �õ�����ĳһ�±������ļ�
*  @Author        :: zzw
*  @Parameters    :: fileIdx:�ļ��±�
*					 mruFile:����ļ�������
*  @Return        :: �����Ƿ���سɹ�
**************************************************************************/
BOOL SystemConfigVarManager::GetMruFile(int fileIdx, CString& mruFile)
{
	if ((fileIdx >= mruFilePathNumber)|| fileIdx<0)
	{
		return FALSE;
	}
	mruFile = mruFilePath[fileIdx];
	return TRUE;
}

/**************************************************************************
*  @Function Name :: GetShortFilePath
*  @Description   :: ���ܵõ���·���Ķ�·��
*  @Author        :: zzw
*  @Parameters    :: longPath:�����ĳ�·��
*					 shortPath:��·�������� limit_length:·�����Ƶĳ���
*  @Return        :: None
**************************************************************************/
void SystemConfigVarManager::GetShortFilePath(CString &longPath, CString &shortPath,int limit_length)
{
	int layer = 0;//�ļ�����/����Ŀ�����ļ��Ĳ���Ŀ
	int total_length = longPath.GetLength();
	if (limit_length <= 10)//̫��û������
	{
		limit_length = 10;
	}

	//������������60���ַ�
	if (total_length <= limit_length)
	{
		shortPath = longPath;
		return;
	}

	total_length = 0;
	CString allfileName=_T("");
	CString fileName;
	CString filePath= longPath;

	while (TRUE)
	{
		fileName = fsutility::FSFileSysHelper::TrimFileNameFromFullName(filePath);//�õ��ļ���
		filePath = fsutility::FSFileSysHelper::TrimFilePathFromFullName(filePath);//�õ��ļ�·��
		total_length++;
		total_length += fileName.GetLength();
		if (total_length >= limit_length)
		{//������һ����ַ��ʹ���limit_length����������Ȼ����ǰֻ�����̵�·��
			if (0 == layer)
			{//ֻ���ļ��ͳ�����limit_length����ô
				shortPath = filePath.Left(3) + _T("...\\") + fileName.Left(4)+_T("...")+fileName.Right(limit_length-9);
				return;
			}
			//�в����Ļ���������һ����
			shortPath = filePath.Left(3) + _T("...\\") + allfileName;
			return;
		}
		else
		{//������һ·�����������һЩ��ҪĿ¼
			if (0 == layer)
			{
				allfileName = fileName;
			}
			else
			{
				allfileName = fileName + _T("\\") + allfileName;
			}
			layer++;
		}
		filePath = filePath.Left(filePath.GetLength() - 1);//��ȥ"//"
	}

	return;
}

/**************************************************************************
*  @Function Name :: GetFileName
*  @Description   :: ���ļ�·������õ��ļ���
*  @Author        :: zzw
*  @Parameters    :: pcapFileName:�ļ�·��
*					 shortPcapName:�ļ���
*  @Return        :: �����Ƿ�õ��ļ���
**************************************************************************/
BOOL SystemConfigVarManager::GetFileName(const CString &pcapFileName, CString &shortPcapName)
{
	CString fileName = fsutility::FSFileSysHelper::TrimFileNameFromFullName(pcapFileName);//�õ��ļ���
	int pos = fileName.ReverseFind('.');
	shortPcapName = fileName.Left(pos);
	return TRUE;
}

/**************************************************************************
*  @Function Name :: IsFileExist
*  @Description   :: �ж��ļ��Ƿ����
*  @Author        :: zzw
*  @Parameters    :: csFile:��Ҫ�жϵ��ļ�
*
*  @Return        :: �����ļ��Ƿ����
**************************************************************************/
BOOL SystemConfigVarManager::IsFileExist(const CString& csFile)
{
	DWORD dwAttrib = GetFileAttributes(csFile);
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

/**************************************************************************
*  @Function Name :: GetFileSuffixName
*  @Description   :: ��ȡ�ļ�File_path�ĺ�׺��suffixName
*  @Author        :: zzw
*  @Parameters    :: suffixName:��ȡ���ļ���׺��������
*					 File_path:�ļ�·��
*  @Return        :: None
**************************************************************************/
void SystemConfigVarManager::GetFileSuffixName(CString &suffixName, CString File_path)
{
	if (File_path == "")		//���·���ǿ��򷵻�
	{
		return;
	}
	int pos = File_path.ReverseFind('.');
	suffixName = File_path.Right(File_path.GetLength() - pos -1);
}