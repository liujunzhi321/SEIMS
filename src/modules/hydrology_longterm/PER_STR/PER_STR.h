/*!
 * \file PER_STR.h
 * \brief Percolation calculated by storage routing method.
 * \author Junzhi Liu
 * \date May 2011
 * \revised LiangJun Zhu
 * \date 2016-5-29
 */
#pragma once
#include "api.h"
#include "SimulationModule.h"
#include <string>
#include <vector>
#include <string>
#include <sstream>
using namespace std;
/** \defgroup PER_STR
 * \ingroup Hydrology_longterm
 * \brief Percolation calculated by storage routing method.
 *
 */

/*!
 * \class PER_STR
 * \ingroup PER_STR
 * \brief Percolation calculated by storage routing method.
 * 
 */
class PER_STR : public SimulationModule
{
private:
	/// number of soil layers
	int m_nSoilLayers;
	/// soil depth
	float** m_soilDepth;
	/// depth of the up soil layer
	float* m_upSoilDepth;
	/// time step
	int m_dt;
	/// valid cells number
	int m_nCells;
	/// threshold soil freezing temperature
	float  m_frozenT;
	/// saturated conductivity
	float **m_ks;
	/// soil porosity
	float **m_porosity;
	/// field capacity
	float **m_fc;
	/// pore index
	float **m_poreIndex;
	/// soil moisture
	float **m_somo;
	/// soil temperature
	float *m_soilT;
	/// infiltration
	float *m_infil;

	/// Output: percolation
	float **m_perc;
	
public:
	/// Constructor
	PER_STR(void);
	/// Destructor
	~PER_STR(void);

	virtual int Execute();

	virtual void SetValue(const char* key, float data);
	virtual void Set1DData(const char* key, int nRows, float* data);
	virtual void Set2DData(const char* key, int nrows, int ncols, float** data);
	virtual void Get2DData(const char* key, int *nRows, int *nCols, float*** data);

private:
	/**
	*	@brief check the input data. Make sure all the input data is available.
	*
	*	@return bool The validity of the input data.
	*/
	bool CheckInputData(void);

	/**
	*	@brief check the input size. Make sure all the input data have same dimension.
	*	
	*	@param key The key of the input data
	*	@param n The input data dimension
	*	@return bool The validity of the dimension
	*/
	bool CheckInputSize(const char*,int);
};

