/** 
*	@file
*	@version	1.0
*	@author	Junzhi Liu
*	@date	1-November-2010
*
*	@brief	Hargreaves Method to Compute PET
*
*	Revision:
*   Date:
*/
#ifndef SEIMS_PET_HARGREAVES_INCLUDE
#define SEIMS_PET_HARGREAVES_INCLUDE

#include <string>
#include "api.h"
#include "SimulationModule.h"

using namespace std;
/*!
 * \class PETHargreaves
 *
 * \brief 
 *
 * 
 *
 */
class PETHargreaves : public SimulationModule
{
public:
	PETHargreaves(void);
	~PETHargreaves(void);

	virtual void SetValue(const char* key, float value);
	virtual void Set1DData(const char* key, int n, float *value);
	virtual void Get1DData(const char* key, int* n, float **data);
	virtual int Execute();

private:
	/// mean air temperature for a given day(degree)
	///float *m_tMean;
	/// maximum air temperature for a given day(degree)
	float *m_tMax;
	/// minimum air temperature for a given day(degree)
	float *m_tMin;
	/// maximum solar radiation(MJ/m2/d)
	///float *m_srMax;
	/// size of the input array
	int m_size;

	/// output pet array
	float *m_pet;
	
	/// Correction Factor for PET
	float m_petFactor;

	///latitude of the stations
	float *m_latitude;
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

	/**
	*	@brief Calculate the max solar radiation for a station of one day
	*
	*	@param day Julian day.
	*	@param lat Latitude of the station
	*	
	*	@return float The max solar radiation.
	*/
	float MaxSolarRadiation(int,float);

	/**
	*	@brief Get the Julian day of one day
	*/
	int JulianDay(time_t);
};

#endif