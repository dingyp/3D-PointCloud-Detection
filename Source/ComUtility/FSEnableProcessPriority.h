#pragma once

namespace fsutility
{
class AFX_EXT_CLASS CFSEnableProcessPriority
{
public:
	CFSEnableProcessPriority(void);
	~CFSEnableProcessPriority(void);

	//���õ�ǰ�������ȼ�Ϊ���(ʵʱ)
	//
	BOOL SetRealTimePriority();

	BOOL SetHighPriority();

	//
	//������ǰ����Ȩ�޺���("SeShutdownPrivilege"�ػ�Ȩ��)
	//
	BOOL EnableShutdownPriv();

	//
	//������ǰ����Ȩ�޺���("SeDebugPrivilege"����д����Ȩ��)
	//
	BOOL EnableDebugPriv();

	//
	//������ǰ����Ȩ�޺���("SeBackupPrivilege"ע�����Ȩ��)
	//
	BOOL EnableBackupPriv();

	//
	//������ǰ����Ȩ�޺���("SeRestorePrivilege"�ָ�����Ȩ��)
	//
	BOOL EnableRestorePriv();
};

}
