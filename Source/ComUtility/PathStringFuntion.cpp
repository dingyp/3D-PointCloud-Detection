#include "StdAfx.h"
#include "PathStringFuntion.h"

using namespace fsutility;

CPathStringFunction::CPathStringFunction(void)
{
}

CPathStringFunction::~CPathStringFunction(void)
{
}


CString CPathStringFunction::TrimFileNameFromFullPath(CString strFullPath)
{
	int nFindPos = strFullPath.ReverseFind(_T('\\'));
	if(nFindPos != -1)
	{
		return strFullPath.Left(nFindPos+1);
	}
	return strFullPath;
}

//���·���Ƿ���ڣ�����������򴴽�
//TRUE   ����Ŀ¼
//FALSE  ������Ŀ¼���Ҵ�����ʧ��
BOOL  CPathStringFunction::CheckAndCreatePath(CString strDirectory)
{
	DWORD   dwAttr  = GetFileAttributes(strDirectory);
	if(dwAttr==-1||(dwAttr&FILE_ATTRIBUTE_DIRECTORY)==0)
	{
		if(!CreateDirectory(strDirectory,   NULL)   )
		{
			return FALSE;
		}
	} 
	return TRUE;
}


//ɾ���ļ���
//BOOL DeleteFolder(CString strFilePath)
//{
//	if (strFilePath.Right(1) == _T("\\"))
//	{
//		strFilePath.TrimRight(_T("\\"));
//	}
//	strFilePath += _T("\0");
//	SHFILEOPSTRUCT fos ;
//	ZeroMemory( &fos, sizeof( fos)) ;
//	fos.hwnd   = HWND_DESKTOP;
//	fos.wFunc  = FO_DELETE ;
//	fos.fFlags = FOF_SILENT | FOF_NOCONFIRMATION ;
//	fos.pFrom  = strFilePath ;
//	// ɾ���ļ��м�������
//	if( 0 != SHFileOperation( &fos))
//		return FALSE ;
//	return TRUE;
//}

BOOL CPathStringFunction::DeleteDirectory(CString strDir)
{
	if(strDir.IsEmpty())   
	{ 
		return FALSE;
	} 
	
	if (strDir.Right(1) == _T("\\"))
	{
		strDir.TrimRight(_T("\\"));
	}

	//   ����ɾ���ļ������ļ��� 
	CFileFind   ff; 
	BOOL   bFound  =  ff.FindFile(strDir+ _T("\\*"), 0); 
	while(bFound) 
	{ 
		bFound = ff.FindNextFile(); 
		if(ff.GetFileName()== _T(".")||ff.GetFileName()== _T("..")) 
			continue; 

		//ȥ���ļ�(��)ֻ�������� 
		SetFileAttributes(ff.GetFilePath(),   FILE_ATTRIBUTE_NORMAL); 
		if(ff.IsDirectory()) 
		{   
			DeleteDirectory(ff.GetFilePath()); 
			RemoveDirectory(ff.GetFilePath()); 
		} 
		else
		{ 
			DeleteFile(ff.GetFilePath());   //   ɾ���ļ� 
		} 
	} 
	ff.Close(); 
	return RemoveDirectory(strDir); 
}
