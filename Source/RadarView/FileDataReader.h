#pragma once
#include <vector>

using namespace std;
class FileDataReader
{
public:
	static FileDataReader * Instance();
	
	~FileDataReader();

	vector<string> readFile(string filepath);

	//bool writeFile(string filename);
	
private:
	FileDataReader();
	static FileDataReader  * instance;
};

