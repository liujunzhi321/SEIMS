#pragma once

#include <string>
#include <vector>

using namespace std;

enum WgnType
{
	WGN_PRECIPITATION,
	WGN_SNOW
};

class clsWgnData
{
public:
	clsWgnData(string databasePath,WgnType type);
	~clsWgnData(void);
	void getWgnData(int* nRows, int* nCols,float*** wgnData);
	void show(void);
private:
	string m_wtgTableName;
	vector<string> m_columns;
	float** m_wgnData;

private:
	void readWgnData(string,WgnType type);
};

