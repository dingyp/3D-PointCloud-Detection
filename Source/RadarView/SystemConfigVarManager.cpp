#include "stdafx.h"
#include "SystemConfigVarManager.h"
#include "SysPathManager.h"
#include "MainFrm.h"

/**************************************************************************
*  @Function Name :: SystemConfigVarManager
*  @Description   :: SystemConfigVarManager类的构造函数
*  @Author        :: zzw
*  @Parameters    :: None
*
*  @Return        :: None
**************************************************************************/
SystemConfigVarManager::SystemConfigVarManager()
{
	mruFilePathNumber = 0;//初始化个数置空
}

/**************************************************************************
*  @Function Name :: ~SystemConfigVarManager
*  @Description   :: SystemConfigVarManager类的析构函数
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
*  @Description   :: 加载ini文件中的设置参数
*  @Author        :: zzw
*  @Parameters    :: strConfigPath:文件路径
*
*  @Return        :: 返回是否加载成功
**************************************************************************/
BOOL SystemConfigVarManager::LoadConfig(CString strConfigPath)
{
	if (!PathFileExists(strConfigPath))
	{//不存在进行创建,open里面有创建的意思
		m_ScanConfig.Open(strConfigPath);
		return FALSE;
	}

	if (m_ScanConfig.Open(strConfigPath))
	{
		//读取所有最近打开文件
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
*  @Description   :: 得到最近打开的CString数组，并返回总共的个数，上限10
*  @Author        :: zzw
*  @Parameters    :: mruFiles:最近的文件路径数组
*
*  @Return        :: 返回最近文件的数目
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
*  @Description   :: 删除检验已经不存在的文件
*  @Author        :: zzw
*  @Parameters    :: mruFiles:最近文件
*
*  @Return        :: None
**************************************************************************/
void SystemConfigVarManager::DelMruFile(CString mruFiles)
{
	//寻找文件是否在路径里面
	int idx;
	for (idx = 0; idx < mruFilePathNumber; idx++)
	{
		if (mruFilePath[idx].Compare(mruFiles) == 0)
		{//相等
			break;
		}
	}
	//在的话进行删除，并把后面的往前移动
	if (idx >= mruFilePathNumber)
	{//没有找到
		return;
	}
	//将这个idx删掉，把之后的往前移动
	for (int i = idx; i < mruFilePathNumber-1; i++)
	{
		mruFilePath[i] = mruFilePath[i + 1];//往前移动一位
	}
	mruFilePathNumber--;

	//更新ini文件
	m_ScanConfig.Move2Section(_T("Mru File"));
	for (int i = 0; i < mruFilePathNumber; i++)
	{//写进去的操作其实可以在最后进行
		CString strIndex;
		strIndex.Format(_T("%d"), i);
		m_ScanConfig.WriteKey(strIndex, mruFilePath[i]);
	}

	//删除所有的
	CString cstmp;
	cstmp.Format(_T("%d"), mruFilePathNumber);
	CString strSysConfigFile = fsutility::CSysPathManager::Instance().GetExePath() + _T("config\\setting.ini");
	WritePrivateProfileString(_T("Mru File"), cstmp, NULL, strSysConfigFile);

	//这边需要对菜单栏的最近文件进行更新
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->UpdateMenuMruFile(mruFilePath, mruFilePathNumber);
}

/**************************************************************************
*  @Function Name :: AddMruFile
*  @Description   :: 加入最近打开的文件
*  @Author        :: zzw
*  @Parameters    :: mruFiles:加入的文件
*
*  @Return        :: None
**************************************************************************/
void SystemConfigVarManager::AddMruFile(CString mruFiles)
{
	//首先判断加入的文件之前有没有打开过，打开过的话是否需要移动排名
	int idx;
	for (idx = 0; idx < mruFilePathNumber; idx++)
	{
		if (mruFilePath[idx].Compare(mruFiles) == 0)
		{//相等
			break;
		}
	}
	//之前没有打开过，依次往后移动
	for (int i = (idx == mruFilePathNumber) ? ((mruFilePathNumber < 10) ? (mruFilePathNumber) : (9)) : (idx); i >0; i--)
	{
		mruFilePath[i] = mruFilePath[i - 1];//往后移动一位
	}
	mruFilePath[0] = mruFiles;
	if ((idx == mruFilePathNumber)&&(mruFilePathNumber < 10))
	{
		mruFilePathNumber++;
	}

	//更新ini文件
	m_ScanConfig.Move2Section(_T("Mru File"));
	for (int i = 0; i < mruFilePathNumber; i++)
	{//写进去的操作其实可以在最后进行
		CString strIndex;
		strIndex.Format(_T("%d"), i);
		m_ScanConfig.WriteKey(strIndex, mruFilePath[i]);
	}

	//这边需要对菜单栏的最近文件进行更新
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->UpdateMenuMruFile(mruFilePath, mruFilePathNumber);
}

/**************************************************************************
*  @Function Name :: GetMruFile
*  @Description   :: 得到具体某一下标的最近文件
*  @Author        :: zzw
*  @Parameters    :: fileIdx:文件下标
*					 mruFile:最近文件的引用
*  @Return        :: 返回是否加载成功
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
*  @Description   :: 智能得到长路径的短路径
*  @Author        :: zzw
*  @Parameters    :: longPath:完整的长路径
*					 shortPath:短路径的引用 limit_length:路径限制的长度
*  @Return        :: None
**************************************************************************/
void SystemConfigVarManager::GetShortFilePath(CString &longPath, CString &shortPath,int limit_length)
{
	int layer = 0;//文件中有/的数目，即文件的层数目
	int total_length = longPath.GetLength();
	if (limit_length <= 10)//太短没有意义
	{
		limit_length = 10;
	}

	//将长度缩减到60个字符
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
		fileName = fsutility::FSFileSysHelper::TrimFileNameFromFullName(filePath);//得到文件名
		filePath = fsutility::FSFileSysHelper::TrimFilePathFromFullName(filePath);//得到文件路径
		total_length++;
		total_length += fileName.GetLength();
		if (total_length >= limit_length)
		{//加上这一层的字符就大于limit_length，先缩减，然后往前只留下盘的路径
			if (0 == layer)
			{//只是文件就超过了limit_length，那么
				shortPath = filePath.Left(3) + _T("...\\") + fileName.Left(4)+_T("...")+fileName.Right(limit_length-9);
				return;
			}
			//有层数的话就舍弃这一层了
			shortPath = filePath.Left(3) + _T("...\\") + allfileName;
			return;
		}
		else
		{//加上这一路径，视情况加一些主要目录
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
		filePath = filePath.Left(filePath.GetLength() - 1);//除去"//"
	}

	return;
}

/**************************************************************************
*  @Function Name :: GetFileName
*  @Description   :: 从文件路径里面得到文件名
*  @Author        :: zzw
*  @Parameters    :: pcapFileName:文件路径
*					 shortPcapName:文件名
*  @Return        :: 返回是否得到文件名
**************************************************************************/
BOOL SystemConfigVarManager::GetFileName(const CString &pcapFileName, CString &shortPcapName)
{
	CString fileName = fsutility::FSFileSysHelper::TrimFileNameFromFullName(pcapFileName);//得到文件名
	int pos = fileName.ReverseFind('.');
	shortPcapName = fileName.Left(pos);
	return TRUE;
}

/**************************************************************************
*  @Function Name :: IsFileExist
*  @Description   :: 判断文件是否存在
*  @Author        :: zzw
*  @Parameters    :: csFile:需要判断的文件
*
*  @Return        :: 返回文件是否存在
**************************************************************************/
BOOL SystemConfigVarManager::IsFileExist(const CString& csFile)
{
	DWORD dwAttrib = GetFileAttributes(csFile);
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

/**************************************************************************
*  @Function Name :: GetFileSuffixName
*  @Description   :: 获取文件File_path的后缀名suffixName
*  @Author        :: zzw
*  @Parameters    :: suffixName:获取的文件后缀名的引用
*					 File_path:文件路径
*  @Return        :: None
**************************************************************************/
void SystemConfigVarManager::GetFileSuffixName(CString &suffixName, CString File_path)
{
	if (File_path == "")		//如果路径是空则返回
	{
		return;
	}
	int pos = File_path.ReverseFind('.');
	suffixName = File_path.Right(File_path.GetLength() - pos -1);
}