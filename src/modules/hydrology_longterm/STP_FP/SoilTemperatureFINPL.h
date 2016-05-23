/** 
*	@file
*	@version	1.0
*	@author	Junzhi Liu
*	@date	5-January-2011
*
*	@brief	Finn Plauborg Method to Compute Soil Temperature
*
*	Revision:
*   Date:
*/
#ifndef SEIMS_SOIL_TEMPEPRATURE_FIN_PL_INCLUDE
#define SEIMS_SOIL_TEMPEPRATURE_FIN_PL_INCLUDE

#include <string>
#include "api.h"
#include "SimulationModule.h"

using namespace std;
/** \defgroup STP_FP
 * \ingroup Hydrology_longterm
 * \brief Finn Plauborg Method to Compute Soil Temperature
 *
 */

/*!
 * \class SoilTemperatureFINPL
 * \ingroup STP_FP
 * \brief Soil temperature
 * 
 */
class SoilTemperatureFINPL : public SimulationModule
{
public:
	SoilTemperatureFINPL(void);
	~SoilTemperatureFINPL(void);

	virtual void SetValue(const char* key, float value);
	virtual void Set1DData(const char* key, int n, float* data);
	virtual void Get1DData(const char* key, int* n, float** data);
	virtual void Set2DData(const char *key, int nRows, int nCols, float **data);
	virtual int Execute();

	virtual void SetDate(time_t t);
private:

	/// time
	time_t m_date;

	/// from parameter database
	/// coefficients in the Equation
	float m_a0, m_a1, m_a2, m_a3, m_b1, m_b2, m_d1, m_d2;
	/// ratio between soil temperature at 10 cm and the mean
	float m_kSoil10;

	/// from GIS interface Project/model subdirectory
	/// Julian day
	int m_julianDay;
	/// count of cells
	int m_size;
	/// factor of soil temperature relative to short grass (degree) 
	float *m_relativeFactor;

	/// from interpolation module
	/// air temperature of the current day
	float *m_tMin, *m_tMax;
	///// air temperature of the day(d-1)
	//float *m_tMin1, *m_tMax1;
	///// air temperature of the day(d-2)
	//float *m_tMin2, *m_tMax2;
	/// array containing the row and column numbers for valid cells
	//float **m_mask;
	float *m_t1;
	float *m_t2;

	/// output soil temperature
	float *m_soilTemp;

	/// w=2*pi/365
	float w;


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
	bool CheckInputSize(const char*, int);
};

#endif