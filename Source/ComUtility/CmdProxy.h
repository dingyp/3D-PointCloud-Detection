#pragma once
namespace fsutility
{
	class AFX_EXT_CLASS CmdProxy
	{
	public:
		CmdProxy(void);
		virtual ~CmdProxy(void);

		//����cmd���������������
		static BOOL RunCmd(CString strCmdText, CString &strResult);
	};
}