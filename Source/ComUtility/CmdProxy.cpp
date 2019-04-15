#include "StdAfx.h"
#include "CmdProxy.h"
#include "FsStringConvert.h"

using namespace fsutility;

CmdProxy::CmdProxy(void)
{
}

CmdProxy::~CmdProxy(void)
{
}


BOOL CmdProxy::RunCmd(CString strCmdText, CString &strResult)
{
	//������������
	CString strCmd;
	strCmd.Format(_T("cmd /c %s"),strCmdText);

	/*ͨ�������ܵ���ʽ��ȡcmd��������������*/
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead,hWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
	//ʹ��ϵͳĬ�ϵİ�ȫ������ 
	sa.lpSecurityDescriptor = NULL;  
	//�����Ľ��̼̳о��
	sa.bInheritHandle = TRUE;        

	if (!CreatePipe(&hRead,&hWrite,&sa,0)) 
	{  
		return FALSE;
	}

	STARTUPINFO si; 
	PROCESS_INFORMATION pi;

	ZeroMemory(&si,sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO); 

	GetStartupInfo(&si); 

	 //�´������̵ı�׼�������д�ܵ�һ��
	si.hStdError = hWrite; 
	si.hStdOutput = hWrite; 
	si.wShowWindow = SW_HIDE; 

	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	if (!CreateProcess(NULL,strCmd.GetBuffer(0),NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))  //�����ӽ���
	{
		CloseHandle(hWrite); 
		CloseHandle(hRead); 
		return FALSE;
	}

	//�ȴ�cmd����������ɣ���ȴ�60s
	WaitForSingleObject(pi.hProcess, 30000);
	CloseHandle(hWrite); 

	TCHAR buffer[4096] = {0};
	DWORD bytesRead=0;
	while (true) 
	{
		if (ReadFile(hRead,buffer,4095,&bytesRead,NULL) == NULL)  //��ȡ�ܵ�
			break;
		buffer[bytesRead] = _T('\0');
		strResult += buffer;
	}

	CloseHandle(hRead);
	CloseHandle(pi.hThread);   
	CloseHandle(pi.hProcess);

	return TRUE;
}