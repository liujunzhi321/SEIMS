#pragma once
#include <string>
#include "api.h"
#include "util.h"
#include "SimulationModule.h"

using namespace std;

class ModulesTest : public SimulationModule
{
private:
	/// data row number
	int m_nCells;
	/// input 1D raster data
	float* m_raster1D;
	/// input 2D array
	//float **m_routingLayers;
	/// layer of this 2D array
	//int m_nLayers;
	/// number of soil layers
	int m_soilLayers;
	/// input 2D raster data
	float** m_raster2D;
	/// output 1D raster data
	float* m_output1Draster;
	/// output 2D raster data
	float** m_output2Draster;
public:
	ModulesTest(void);
	~ModulesTest(void);
	virtual int Execute();
	virtual void Set1DData(const char* key, int n, float* data);
	virtual void Get1DData(const char* key, int* n, float** data);
	virtual void Set2DData(const char* key, int n, int col, float** data);
	virtual void Get2DData(const char* key, int* n, int* col, float*** data);
};

