/** \defgroup ATMDEP
 * \ingroup Nutrient
 * \brief Calculate the atmospheric deposition of nitrogen, include nitrate and ammonia.
 */
/*!
 * \file AtmosphericDeposition.h
 * \ingroup ATMDEP
 * \author Huiran Gao
 * \date May 2016
 */
#pragma once
#ifndef SEIMS_ATMDEP_PARAMS_INCLUDE
#define SEIMS_ATMDEP_PARAMS_INCLUDE
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
	virtual void Set2DData(const char* key, int nrows, int ncols, float** data);
	virtual void GetValue(const char* key, float* value);
	//virtual void Get1DData(const char* key, int* n, float** data);
	virtual void Get2DData(const char* key, int *nRows, int *nCols, float*** data);
	bool CheckInputSize(const char* key, int n);
	bool CheckInputData(void);

private:

	/// size of array 
	int m_nCells;
	/// cell width of grid map (m)
	float m_cellWidth;
	/// soil layers
	float* m_nSoilLayers;
	/// maximum soil layers
	int m_soiLayers;

	/// parameters
	/// concentration of nitrate in the rain (mg N/L) 
	float m_rcn;
	/// concentration of ammonia in the rain (mg N/L)
	float m_rca;
	///atmospheric dry deposition of nitrates (kg/km2)
	float m_drydep_no3;
	///atmospheric dry deposition of ammonia (kg/km2)
	float m_drydep_nh4;

	/// input
	/// precipitation (mm H2O) 
	float *m_preci;
	/// root depth from the soil surface
	float **m_sol_z;
	///amount of ammonium in layer (kg/km2)
	float **m_sol_nh3;

	/// output
	/// nitrate added by rainfall (kg/km2)
	float m_addrnh3;
	/// ammonium added by rainfall (kg/km2)
	float m_addrno3;

	/// input & output
	/// average annual amount of NO3 added to soil by rainfall in watershed (kg/km2)
	float m_wshd_rno3;
	/// amount of nitrate in layer (kg/km2)
	float **m_sol_no3;


};
#endif