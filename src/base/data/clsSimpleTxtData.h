#pragma once

#include <string>
#include <ostream>

using namespace std;

class clsSimpleTxtData
{
public:
	clsSimpleTxtData(string fileName);
	~clsSimpleTxtData();

	void getData(int *nRow, float **data);
	void dump(ostream *fs);
private:
	int		m_row;
	float*	m_data;
};

