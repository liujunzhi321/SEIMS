#pragma once

#include <string>
#include "api.h"
#include "SimulationModule.h"

using namespace std;

class clsTSD_RD : public SimulationModule
{
private:
	int m_Rows;
	float* m_Data;

public:
	clsTSD_RD(void);
	~clsTSD_RD(void);

	virtual void Set1DData(const char* key, int n, float* data);
	virtual void Get1DData(const char* key, int* n, float** data);
};

