/*!
 * \file AtmosphericDeposition.h
 * \brief Calculate the atmospheric deposition of nitrogen, include nitrate and ammonia
 *
 *
 *
 * \author Lin Wang
 * \revised by Liangjun Zhu
 * \version 
 * \date July 2015
 *
 * 
 */
#pragma once

#include <string>
#include <ctime>
#include <cmath>
#include <map>
#include "SimulationModule.h"
using namespace std;

class AtmosphericDeposition : public SimulationModule
{
public:
	AtmosphericDeposition(void);
	~AtmosphericDeposition(void);

	virtual int Execute();

	virtual void SetValue(const char* key, float data);
	virtual void Set1DData(const char* key, int n, float* data);
	virtual void Get1DData(const char* key, int* n, float** data);
	//virtual void Set2DData(const char* key, int nrows, int ncols, float** data);
	virtual void Get2DData(const char* key, int *nRows, int *nCols, float*** data);

	bool CheckInputSize(const char* key, int n);
	bool CheckInputData(void);

private:

	void initalOutputs();

	int m_nLayers;

	/// parameters
	/// root depth from the soil surface
	float *m_rootDepth;
	/// size of array 
	int m_size;
	/// concentration of nitrate in the rain (mg N/L) 
	float m_conRainNitra;
	/// concentration of ammonia in the rain (mg N/L)
	float m_conRainAmm;
	/// input
	/// amount of precipitation in a given day (mm H2O) 
	float* m_P;
	
	/// output
	/// depth of the layer (mm)
	float **m_Depth;
	/// amount of nitrate in layer ly (kg N/ha)
	float **m_Nitrate;
	///amount of ammonium in layer ly (kg N/ha)
	float **m_Ammon;
	/// nitrate added by rainfall (kg N/ha)
	float* m_addRainNitra;
    /// ammonium added by rainfall (kg N/ha)
	float* m_addRainAmm;

};