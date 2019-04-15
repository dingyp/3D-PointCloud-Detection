#include "stdafx.h"
#include "FileDataReader.h"
#include <fstream>
FileDataReader * FileDataReader::instance = nullptr;
FileDataReader::FileDataReader()
{
}


FileDataReader * FileDataReader::Instance()
{
	if (instance == nullptr) {
		instance = new FileDataReader();
	}
	return instance;
}

FileDataReader::~FileDataReader()
{
}

vector<string> FileDataReader::readFile(string filepath)
{
	ifstream ifile(filepath);
	if (!ifile) {
		return vector<string>();
	}
	string perstr;
	vector<string> strlist;
	while (getline(ifile, perstr)) {
		if (perstr.size() > 0) {
			strlist.push_back(perstr);
		}
		
	}
	return strlist;
}
