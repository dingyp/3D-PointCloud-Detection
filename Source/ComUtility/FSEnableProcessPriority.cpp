#include "StdAfx.h"
#include "FSEnableProcessPriority.h"

using namespace fsutility;

CFSEnableProcessPriority::CFSEnableProcessPriority(void)
{
}

CFSEnableProcessPriority::~CFSEnableProcessPriority(void)
{
}

//��Ĺ��ܽ���:
//-------------------------------------------------------------
//#define            SE_BACKUP_NAME               TEXT("SeBackupPrivilege")     //��������Ȩ��
//#define            SE_RESTORE_NAME              TEXT("SeRestorePrivilege")    //�ָ�����Ȩ��
//#define            SE_SHUTDOWN_NAME             TEXT("SeShutdownPrivilege")   //�ػ�Ȩ��
//#define            SE_DEBUG_NAME                TEXT("SeDebugPrivilege")      //����д����Ȩ��
//-------------------------------------------------------------

//��Ĺ��ܽ���:
//-------------------------------------------------------------
//ABOVE_NORMAL_PRIORITY_CLASS(0x00008000)   ���ڱ�׼
//BELOW_NORMAL_PRIORITY_CLASS(0x00004000)   ���ڱ�׼
//HIGH_PRIORITY_CLASS(0x00000080)           ��
//IDLE_PRIORITY_CLASS(0x00000040)           ��
//NORMAL_PRIORITY_CLASS(0x00000020)         ��׼
//REALTIME_PRIORITY_CLASS(0x00000100)       ʵʱ
//-------------------------------------------------------------

//
//���õ�ǰ�������ȼ�Ϊ���(ʵʱ)
//
BOOL CFSEnableProcessPriority::SetRealTimePriority()
{
	if( ! SetPriorityClass( GetCurrentProcess(), REALTIME_PRIORITY_CLASS ))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CFSEnableProcessPriority::SetHighPriority()
{
	if( ! SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS ))
	{
		return FALSE;
	}

	return TRUE;
}


//
//������ǰ����Ȩ�޺���("SeShutdownPrivilege"�ػ�Ȩ��)
//
BOOL CFSEnableProcessPriority::EnableShutdownPriv()
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	if ( ! OpenProcessToken( GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) )
		return FALSE;

	if ( ! LookupPrivilegeValue( NULL, SE_SHUTDOWN_NAME, &sedebugnameValue ) )
	{
		CloseHandle( hToken );
		return FALSE;
	}

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if ( ! AdjustTokenPrivileges( hToken, FALSE, &tkp, sizeof tkp, NULL, NULL ) )
		CloseHandle( hToken );

	return TRUE;
}

//
//������ǰ����Ȩ�޺���("SeDebugPrivilege"����д����Ȩ��)
//
BOOL CFSEnableProcessPriority::EnableDebugPriv()
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	if ( ! OpenProcessToken( GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) )
		return FALSE;

	if ( ! LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &sedebugnameValue ) )
	{
		CloseHandle( hToken );
		return FALSE;
	}

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if ( ! AdjustTokenPrivileges( hToken, FALSE, &tkp, sizeof tkp, NULL, NULL ) )
		CloseHandle( hToken );

	return TRUE;
}

//
//������ǰ����Ȩ�޺���("SeBackupPrivilege"��������Ȩ��)
//
BOOL CFSEnableProcessPriority::EnableBackupPriv()
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	if ( ! OpenProcessToken( GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) )
		return FALSE;

	if ( ! LookupPrivilegeValue( NULL, SE_BACKUP_NAME, &sedebugnameValue ) )
	{
		CloseHandle( hToken );
		return FALSE;
	}

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if ( ! AdjustTokenPrivileges( hToken, FALSE, &tkp, sizeof tkp, NULL, NULL ) )
		CloseHandle( hToken );

	return TRUE;
}

//
//������ǰ����Ȩ�޺���("SeRestorePrivilege"�ָ�����Ȩ��)
//
BOOL CFSEnableProcessPriority::EnableRestorePriv()
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	if ( ! OpenProcessToken( GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) )
		return FALSE;

	if ( ! LookupPrivilegeValue( NULL, SE_RESTORE_NAME, &sedebugnameValue ) )
	{
		CloseHandle( hToken );
		return FALSE;
	}

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if ( ! AdjustTokenPrivileges( hToken, FALSE, &tkp, sizeof tkp, NULL, NULL ) )
		CloseHandle( hToken );

	return TRUE;
}