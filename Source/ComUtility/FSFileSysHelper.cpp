#include "StdAfx.h"
#include "FSFileSysHelper.h"
#include <string>

using namespace std;
using namespace fsutility;
FSFileSysHelper::FSFileSysHelper(void)
{
}

FSFileSysHelper::~FSFileSysHelper(void)
{
}

CString FSFileSysHelper::GetModuName(HMODULE hModu /* = NULL */)
{
    CString str;
    DWORD dwSize = 0;
    dwSize = ::GetModuleFileName(hModu, 
        str.GetBufferSetLength(MAX_PATH), MAX_PATH);

    return str;
}

CString FSFileSysHelper::GetModuPath(HMODULE hModu /* = NULL */)
{
    CString strModuName = GetModuName(hModu);
    int pos = strModuName.ReverseFind(_T('\\'));
    if (pos != -1)
    {
        return strModuName.Left(pos);
    }
    
    return strModuName;
}

void FSFileSysHelper::FindAllSubDir(const CString& strPath, 
                                    CList<CString>& listDir,
                                    BOOL bIncludeParentPath /*= TRUE*/)
{
    CFileFind ff;
    BOOL bOK = ff.FindFile(strPath + _T("\\*.*"));

    while (bOK)
    {
        bOK = ff.FindNextFile();
        if (ff.IsDots())
            continue;

        if (ff.IsDirectory())
        {
            bIncludeParentPath 
                ? listDir.AddTail(ff.GetFilePath())
                : listDir.AddTail(ff.GetFileName());
        }
    }
}

void FSFileSysHelper::FindAllFileinPath(const CString& strPath, 
                                        CList<CString>& listFileName,
                                        BOOL bIncludePathName /*= TRUE*/)
{
    CFileFind ff;
    BOOL bOK = ff.FindFile(strPath + _T("\\*.*"));

    while (bOK)
    {
        bOK = ff.FindNextFile();
        if (!ff.IsDots() && !ff.IsDirectory())
        {
            bIncludePathName 
                ? listFileName.AddTail(ff.GetFilePath())
                : listFileName.AddTail(ff.GetFileName());
        }
    }
}

BOOL FSFileSysHelper::IsFileinPath(const CString& strFile2Find,
                                   const CString& strPath)
{
    CFileFind ff;
    BOOL bOK = ff.FindFile(strPath + _T("\\*.*"));

    while (bOK)
    {
        bOK = ff.FindNextFile();
        if (!ff.IsDots())
        {
            if (ff.GetFileName() == strFile2Find)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL FSFileSysHelper::RecurRMDir(const CString& strDir)
{
	//����ɾ���������̣�c:\\ d:\\ e:\\ �ȣ�
    if (strDir.IsEmpty() || strDir.GetLength()<=3)
    {
        return TRUE;
    }

    if (!IsDir(strDir))
    {
        return FALSE;
    }


    CList<CString> listSubDir;
    FindAllSubDir(strDir, listSubDir);
    {
        POSITION pos = listSubDir.GetHeadPosition();
        while (pos != NULL)
        {
            const CString& strSubDir = listSubDir.GetNext(pos);
            if (!RecurRMDir(strSubDir))
            {
                return FALSE;
            }
        }
    }

    CList<CString> listFiles;
    FindAllFileinPath(strDir, listFiles);
    {
        POSITION pos = listFiles.GetHeadPosition();
        while (pos != NULL)
        {
            const CString& strFilePath = listFiles.GetNext(pos);
            CFile::Remove(strFilePath);
        }
    }
    RemoveDirectory(strDir);

    return TRUE;
}

BOOL FSFileSysHelper::RecurMakeDir(const CString& strFullPath)
{
    if (PathFileExists(strFullPath))
        return TRUE;

    if (strFullPath.IsEmpty())
        return FALSE;

    CString strTemp = strFullPath;

    if (strTemp.ReverseFind(_T('\\')) != strTemp.GetLength()-1)
    {
        strTemp.Append(_T("\\"));
    }
    int nPos = 0;
    while (TRUE)
    {
        nPos = strTemp.Find(_T('\\'), nPos+1);   
        if (nPos == -1)
            break;
        if (nPos <= 2)
            continue;

        ::CreateDirectory(strTemp.Left(nPos+1), NULL);
    }

    return PathFileExists(strFullPath);
}

BOOL FSFileSysHelper::RecurCopyDir(const CString& strFromDir, 
                                   const CString& strToDir)
{
    if (!PathFileExists(strFromDir))
        return FALSE;

    if (!RecurMakeDir(strToDir))
        return FALSE;

    CList<CString> listSubDir;
    FindAllSubDir(strFromDir, listSubDir, FALSE);
    {
        POSITION pos = listSubDir.GetHeadPosition();
        while (pos != NULL)
        {
            const CString& strSubDir = listSubDir.GetNext(pos);
            RecurCopyDir(strFromDir + _T("\\") + strSubDir, strToDir + _T("\\") + strSubDir);
        }
    }

    CList<CString> listFiles;
    FindAllFileinPath(strFromDir, listFiles, FALSE);
    {
        POSITION pos = listFiles.GetHeadPosition();
        while (pos != NULL)
        {
            const CString& strFileName= listFiles.GetNext(pos);
            ::CopyFile(
                strFromDir + _T("\\") + strFileName, 
                strToDir + _T("\\") + strFileName,
                FALSE);
        }
    }

    return TRUE;
}

//�ж�һ���ļ����Ƿ����
BOOL FSFileSysHelper::IsDir(const CString& strPath)
{
	DWORD dwAttr = ::GetFileAttributes(strPath);
	return dwAttr != INVALID_FILE_ATTRIBUTES 
		?  dwAttr & FILE_ATTRIBUTE_DIRECTORY
		:  FALSE;
}

BOOL FSFileSysHelper::PathFileExists(const CString& strPath)
{
	return ::PathFileExists(strPath.GetString());
}


INT CALLBACK _BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM pData) 
{
	TCHAR szDir[MAX_PATH];
	switch(uMsg) 
	{
	case BFFM_INITIALIZED: 
		// WParam is TRUE since you are passing a path.
		// It would be FALSE if you were passing a pidl.
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)pData);
		break;
	case BFFM_SELCHANGED: 
		// Set the status window to the currently selected path.
		if (SHGetPathFromIDList((LPITEMIDLIST)lParam ,szDir))
		{
			SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);
		}
		break;
	}
	return 0;
}

CString FSFileSysHelper::SelectPathBySystemDlg(CString strTitle, CString strDefalutPath)
{
	TCHAR buffDisplayName[MAX_PATH];
	TCHAR fullpath[MAX_PATH];
	BROWSEINFO  browseinfo;
	LPITEMIDLIST lpitemidlist;

	ZeroMemory(&browseinfo, sizeof( BROWSEINFO ));
	browseinfo.pszDisplayName = buffDisplayName ;
	browseinfo.lpszTitle = strTitle;
	browseinfo.ulFlags = BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE;
	browseinfo.lParam = (LPARAM)strDefalutPath.GetBuffer(0);
	browseinfo.lpfn = _BrowseCallbackProc;

	if(!(lpitemidlist = SHBrowseForFolder(&browseinfo)))
	{
		//AfxMessageBox(_T("û��ѡ��Ŀ¼"));
		return CString(_T(""));
	}
	else
	{
		SHGetPathFromIDList(lpitemidlist, fullpath);       
		CoTaskMemFree(lpitemidlist); 
		return CString(fullpath);
	}
}

//��ȫ���еõ��ļ���
CString FSFileSysHelper::TrimFileNameFromFullName(CString strFullPath)
{
	int nFindPos = strFullPath.ReverseFind(_T('\\'));
	if(nFindPos != -1)
	{
		return strFullPath.Mid(nFindPos+1);
	}
	return strFullPath;
}

//��ȫ���еõ��ļ�·��
CString FSFileSysHelper::TrimFilePathFromFullName(CString strFullPath)
{
	int nFindPos = strFullPath.ReverseFind(_T('\\'));
	if(nFindPos != -1)
	{
		return strFullPath.Left(nFindPos+1);
	}
	return strFullPath;
}