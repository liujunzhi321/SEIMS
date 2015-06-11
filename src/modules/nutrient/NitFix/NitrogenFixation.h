#pragma once

#include <string>
#include <ctime>
#include <cmath>
#include <map>
#include "SimulationModule.h"
using namespace std;

class NitrogenFixation : public SimulationModule
{
public:
	NitrogenFixation(void);
	~NitrogenFixation(void);

	virtual int Execute();

	//virtual void SetValue(const char* key, float data);
	virtual void Set1DData(const char* key, int n, float* data);
	virtual void Get1DData(const char* key, int* n, float** data);
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

	//fraction of phu accumulated on a given day
	float *m_frPHU;

	//Soil Moisture
	float **m_SOMO;
	//Field capacity
	float **m_FieldCap;
    // amount of nitrate in layer ly (kg N/ha)
	float **m_Nitrate;
	//plant nitrogen demand not met by uptake from the soil(kg N/ha)
	float **m_demNit;
	
	//output
	// growth stage factor
	float *m_fGS;
	// soil water factor
	float **m_fSW;
	// soil nitrate factor
	float **m_fSN;
	//amount of nitrogen added to the plant biomass by fixation(kg N/ha)
	float **m_fixN;

};