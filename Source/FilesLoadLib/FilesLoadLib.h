#pragma once
// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� FILESLOADLIB_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// FILESLOADLIB_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef FILESLOADLIB_EXPORTS
#define FILESLOADLIB_API __declspec(dllexport)
#else
#define FILESLOADLIB_API __declspec(dllimport)
#endif


// �����Ǵ� FilesLoadLib.dll ������
class FILESLOADLIB_API CFilesLoadLib {
public:
	CFilesLoadLib(void);
	// TODO: �ڴ�������ķ�����
};

extern FILESLOADLIB_API int nFilesLoadLib;

FILESLOADLIB_API int fnFilesLoadLib(void);

#include <gl/gl.h>