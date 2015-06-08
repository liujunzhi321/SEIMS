/** 
*	@file
*	@version	1.0
*	@author	Junzhi Liu
*	@date	1-November-2010
*
*	@brief	Priestley Taylor Method to Compute PET
*
*	Revision:
*   Date:
*/
#ifndef NEW_WETSPA_PET_PRIESTTAYLOR_INCLUDE
#define NEW_WETSPA_PET_PRIESTTAYLOR_INCLUDE

#include <string>
#include "api.h"
#include "SimulationModule.h"

using namespace std;

class PETPriestleyTaylor : public SimulationModule
{
public:
	PETPriestleyTaylor(void);
	~PETPriestleyTaylor(void);

	virtual void SetValue(const char* key, float value);
	virtual void Set1DData(const char* key, int n, float *value);
	virtual void Get1DData(const char* key, int* n, float **data);
	virtual int Execute();
	
private:
	/**
	*	@brief calculates saturation vapor pressure at a given air temperature.
	*
	*	@param float t: mean air temperature(deg C)
	*	@return saturation vapor pressure(kPa)
	*/
	float SaturationVaporPressure(float t);

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
private:
	/// mean air temperature for a given day(degree)
	///float *m_tMean;
	float *m_tMin;
	float *m_tMax;
	/// maximum solar radiation(MJ/m2/d)
	///float *m_srMax;
	/// solar radiation(MJ/m2/d)
	float *m_sr;
	/// relative humidity(%)
	float *m_rhd;
	/// elevation(m)
	float *m_elev;
	/// amount of water in snow(mm)
	///float *m_snow;

	/// size of the input array
	int m_size;

	/// Correction Factor for PET
	float m_petFactor;

	/// output pet array
	float *m_pet;

	///latitude of the stations
	float *m_latitude;

	///The temperature of snow melt
	float m_tSnow;
};

#endif