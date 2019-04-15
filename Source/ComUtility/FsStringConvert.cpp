#include "StdAfx.h"
#include "FsStringConvert.h"

FsStringConvert::FsStringConvert(void)
{
}

FsStringConvert::~FsStringConvert(void)
{
}

//utf-8 to wide char
std::wstring FsStringConvert::MyUstringToWString(std::string &source)
{
	return MyUstringToWString(source.c_str());
}

std::wstring FsStringConvert::MyUstringToWString(const char* source)
{
	if(source == NULL)
	{
		return std::wstring(_T(""));
	}

	int needSize = 0;
	wchar_t* ret = 0;

	needSize = MultiByteToWideChar(CP_UTF8, 0, source, -1, NULL, 0);
	ret = (wchar_t*)malloc((needSize + 1) * sizeof(wchar_t));

	memset(ret, 0, (needSize + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, source, -1, ret, needSize);

	std::wstring result(ret);

	free(ret);

	return result;
}


std::wstring FsStringConvert::MyAstringToWString(std::string &source)
{
	return MyAstringToWString(source.c_str());
}

std::wstring FsStringConvert::MyAstringToWString(const char* source)
{
	if(source == NULL)
	{
		return std::wstring(_T(""));
	}
	int needSize = 0;
	wchar_t* ret = 0;

	needSize = MultiByteToWideChar(GetACP(), 0, source, -1, NULL, 0);
	ret = (wchar_t*)malloc((needSize + 1) * sizeof(wchar_t));

	memset(ret, 0, (needSize + 1) * sizeof(wchar_t));
	MultiByteToWideChar(GetACP(), 0, source, -1, ret, needSize);

	std::wstring result(ret);

	free(ret);

	return result;

	//std::wstring result(source, source+strlen(source));
	//return result;
}

std::string  FsStringConvert::MyWstringToAString(LPCWSTR source)
{
	int iTextLen = WideCharToMultiByte(CP_ACP,0,source,-1,NULL,0,NULL,0);

	//初始化空间
	char *lText = new char[iTextLen+1];
	memset(lText,0,(iTextLen+1)*sizeof(char));  

	// 将strText中的字符串全部转换成ASCII，并将其保存在lText开辟的字符空间中
	WideCharToMultiByte(CP_ACP,0,source,-1,lText,iTextLen,NULL,0);  

	std::string result(lText);

	delete []lText;

	return result;
}


std::string FsStringConvert::MyWstringToAString(std::wstring &source)
{
	return MyWstringToAString(source.c_str());
}




/*************************************二维码编解码******************************************/
//定义函数实现文字编码从GBK，GB2313到UTF-8编码的转换
std::string FsStringConvert::GBKToUTF8(const std::string& strGBK)  
{  
	std::string strOutUTF8 = "";  
	WCHAR * str1;  
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);  
	str1 = new WCHAR[n];  
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);  
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);  
	char * str2 = new char[n];  
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);  
	strOutUTF8 = str2;  
	delete[]str1;  
	str1 = NULL;  
	delete[]str2;  
	str2 = NULL;  
	return strOutUTF8;  
}  

//定义函数实现文字编码从UTF-8到GBK编码的转换
std::string FsStringConvert::UTF8ToGBK(const std::string& strUTF8)  
{  
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);  
	//unsigned short * wszGBK = new unsigned short[len + 1];  
	wchar_t *wszGBK = new wchar_t[len+1];
	memset(wszGBK, 0, len * 2 + 2);  
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUTF8.c_str(), -1, wszGBK, len);  

	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);  
	char *szGBK = new char[len + 1];  
	memset(szGBK, 0, len + 1);  
	WideCharToMultiByte(CP_ACP,0, wszGBK, -1, szGBK, len, NULL, NULL);  
	//strUTF8 = szGBK;  
	std::string strTemp(szGBK);  
	delete[]szGBK;  
	delete[]wszGBK;  
	return strTemp;  
}