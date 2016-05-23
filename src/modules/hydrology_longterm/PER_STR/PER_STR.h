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
 * \brief Calculate the amount of water percolated from the soil water reservoir
 *
 */

/*!
 * \class PER_STR
 * \ingroup PER_STR
 * \brief Calculate water percolated
 * 
 */
class PER_STR : public SimulationModule
{
private:
	/// number of soil layers
	int m_nLayers;
	
	float m_upSoilDepth;

	int m_dt;
	int m_nCells;
	float  m_frozenT;

	float **m_ks;
	float **m_porosity;
	float **m_fc;
	float **m_poreIndex;
	float **m_sm;
	
	float *m_rootDepth;
	float *m_soilT;
	
	float *m_infil;

	float **m_perc;
	
public:
	PER_STR(void);
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
	*	@brief checke the input size. Make sure all the input data have same dimension.
	*	
	*	@param key The key of the input data
	*	@param n The input data dimension
	*	@return bool The validity of the dimension
	*/
	bool CheckInputSize(const char*,int);

	static string toString(float value);	
	
};

