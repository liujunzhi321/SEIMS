#pragma once

#include <string>

using namespace std;

class clsLapseData
{
public:
	clsLapseData(string);
	~clsLapseData(void);

	void getLapseData(int* nRows,float*** lapseData);
	void show(void);
private:
	string m_lapseName;
	float** m_lapseData;

private:
	void readLapseData(string);
};

