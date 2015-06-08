/** 
*	@file
*	@version	1.0
*	@author	Junzhi Liu
*	@date	1-November-2010
*
*	@brief	Penman Monteith Method to Compute PET
*
*	Revision:
*   Date:
*/
#ifndef SEIMS_PET_PENMAN_MONTEITH_INCLUDE
#define SEIMS_PET_PENMAN_MONTEITH_INCLUDE

#include <string>
#include "api.h"
#include "SimulationModule.h"

using namespace std;

class PETPenmanMonteith : public SimulationModule
{
public:
	PETPenmanMonteith(void);
	~PETPenmanMonteith(void);

	virtual void Set1DData(const char* key, int n, float *value);
	virtual void SetValue(const char* key, float value);
	virtual int Execute();
	virtual void Get1DData(const char* key, int* n, float **data);

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

	void clearInputs(void);
private:
	/// mean air temperature for a given day(degree)
	//float *m_tMean;
	float *m_tMin;		//now the mean T is calcuated by minT and maxT. They can be obtained from database directly
	float *m_tMax;
	/// maximum solar radiation(MJ/m2/d)
	///The max solar radiation is calculated from Julin day and latitude now.
	///float *m_srMax;
	/// solar radiation(MJ/m2/d)
	float *m_sr;
	/// relative humidity(%)
	float *m_rhd;
	/// wind speed
	float *m_ws;
	/// amount of water in snow(mm)
	///float *m_snow;
	/// elevation(m)
	float *m_elev;
	/**
	* @brief land cover status code
	*
	* 0 no land cover currently growing
	* 1 land cover growing
	*/
	float *m_growCode;
	/// CO2 concentration(ppmv)
	float m_co2;
	/// canopy height for the day(m)
	float *m_cht;
	/// rate of decline in stomatal conductance per unit increase in vapor pressure deficit(m/s/kPa)
	float m_vpd2;
	/// maximum stomatal conductance(m/s)
	float m_gsi;
	/// leaf area index(m2/m2)
	float *m_lai;
	/// albedo in the day
	///float *m_albedo;

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