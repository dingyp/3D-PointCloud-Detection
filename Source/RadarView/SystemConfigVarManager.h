#pragma once
#include "stdafx.h"
#include "IniFile.h"
#include "singleton.h"
#include "FSFileSysHelper.h"

class  SystemConfigVarManager : public fsutility::Singleton<SystemConfigVarManager>	//对软件中参数进行单实例管理变量
{
public:
	 SystemConfigVarManager();
	~SystemConfigVarManager();

	BOOL LoadConfig(CString strConfigPath);											//加载初始化的设置
	int GetMruFile(CString *mruFiles);												//得到最近打开的CString数组，并返回总共的个数，上限10个
	void AddMruFile(CString mruFiles);												//往里面加入打开的文件
	void DelMruFile(CString mruFiles);												//删除检验已经不存在的文件
	BOOL GetMruFile(int fileIdx,CString& mruFile);									//得到具体某一下标的最近文件
	void GetShortFilePath(CString &longPath,CString &shortPath,int limit_length=80);//将长文件路径进行缩减到指定长度以内,默认80个字符
	BOOL GetFileName(const CString &pcapFileName,CString &shortPcapName);			//从文件路径里面得到文件名
	BOOL IsFileExist(const CString& csFile);										//判断文件是否存在
	void GetFileSuffixName(CString &suffixName,CString File_path);					//获取文件File_path的后缀名suffixName
protected:
	fsutility::CIniFile m_ScanConfig;												//对ini文件进行读取操作的类
private:
	CString mruFilePath[10];														//最近10次打开的文件
	int mruFilePathNumber;															//最近打开文件的个数
};

