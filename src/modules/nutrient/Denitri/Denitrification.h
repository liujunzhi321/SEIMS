
#pragma once

#include <string>
#include <ctime>
#include <cmath>
#include <map>
#include "SimulationModule.h"
using namespace std;

class Denitrification : public SimulationModule
{
public:
	Denitrification(void);
	~Denitrification(void);

	virtual int Execute();

	virtual void SetValue(const char* key, float data);
	//virtual void Set1DData(const char* key, int n, float* data);
	//virtual void Get1DData(const char* key, int* n, float** data);
	virtual void Set2DData(const char* key, int nrows, int ncols, float** data);
	virtual void Get2DData(const char* key, int *nRows, int *nCols, float*** data);

	bool CheckInputSize(const char* key, int n);
	bool CheckInputData(void);

private:

	void initalOutputs();
	
	//input
	// size of array 
	int m_size;
	int m_nLayers;

	// rate coefficient for denitrification 
	float m_denRC;

	// amount of organic carbon in the layer(%)
	float **m_orgCar;
	// amount of nitrate in layer ly (kg N/ha) 
	float **m_Nitrate;
    // nutrient cycling temperature factor for layer ly 
	float **m_TF;
	// nutrient cycling water factor for layer ly
	float **m_WF;
	// threshold value of nutrient cycling water factor for denitrification to occur
	float **m_denWF;
	
	//output
	// amount of nitrogen lost to denitrication (kg N/ha)
	float **m_denLostN;

};