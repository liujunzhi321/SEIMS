/*!
 * \file ClimateParams.h
 *
 * \author ZhuLJ
 * \date April 2016
 *
 * 
 */
#pragma once
#ifndef SEIMS_CLIMATE_PARAMS_INCLUDE
#define SEIMS_CLIMATE_PARAMS_INCLUDE
#include <string>
#include "api.h"
#include "SimulationModule.h"

using namespace std;
/*!
 * \class ClimateParameters
 * \ingroup CLIMATE
 *
 * \brief Climate related intermediate parameters, e.g., saturation vapor pressure, max solar radiation.
 *
 */
class ClimateParameters : public SimulationModule
{
public:
	ClimateParameters(void);
	~ClimateParameters(void);

	virtual void Set1DData(const char* key, int n, float *value);
	virtual void SetValue(const char* key, float value);
	virtual int Execute();
	virtual void GetValue(const char* key, float* value);
private:
	
	/// mean air temperature for a given day(degree)
	float *m_tMean;
	/// maximum air temperature for a given day(degree)
	float *m_tMax;
	/// minimum air temperature for a given day(degree)
	float *m_tMin;
	/// solar radiation(MJ/m2/d)
	float *m_sr;
	/// relative humidity(%)
	float *m_rhd;
	/// wind speed
	float *m_ws;
	/// elevation(m)
	float *m_elev;
	int m_size;

	///latitude of the stations
	float *m_latitude;
private:

	/*!
	 * \brief check the input data. Make sure all the input data is available.
	 * \return bool The validity of the input data.
	 */
	bool CheckInputData(void);

	/*!
	 * \brief check the input size. Make sure all the input data have same dimension.
	 *
	 *
	 * \param[in] key The key of the input data
	 * \param[in] n The input data dimension
	 * \return bool The validity of the dimension
	 */
	bool CheckInputSize(const char*,int);

	/*!
	 * \brief Calculate the max solar radiation for a station of one day
	 *
	 *
	 * \param[in] day Julian day.
	 * \param[in] lat Latitude of the station
	 * \return float The max solar radiation.
	 */
	float MaxSolarRadiation(int,float);

	/*!
	 * \brief Get the Julian day of one day
	 * \return int Julian day
	 */
	int JulianDay(time_t);

	/*!
	 * \brief calculates saturation vapor pressure at a given air temperature.
	 * \param[in] float t: mean air temperature(deg C)
	 * \return saturation vapor pressure(kPa)
	 */
	float SaturationVaporPressure(float t);
};

#endif