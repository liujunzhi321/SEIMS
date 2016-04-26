/*!
 * \file ClimateParams.cpp
 * \ingroup CLIM
 * \author ZhuLJ
 * \date April 2016
 *
 * 
 */
#include "ClimateParams.h"
#include "MetadataInfo.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include "ModelException.h"
#include "util.h"
#include <omp.h>

using namespace std;

ClimateParameters::ClimateParameters(void):m_tMean(NULL), m_tMin(NULL), m_tMax(NULL), m_rhd(NULL), m_sr(NULL), m_ws(NULL), m_latitude(NULL)
{
	this->m_size = -1;
}

ClimateParameters::~ClimateParameters(void)
{
	
}


bool ClimateParameters::CheckInputData()
{
	if(this->m_date == -1)
	{
		throw ModelException("CLIMATE","CheckInputData","You have not set the date time.");
		return false;
	}

	if(m_size <= 0)
	{
		throw ModelException("CLIMATE","CheckInputData","The dimension of the input data can not be less than zero.");
		return false;
	}

	if(this->m_rhd == NULL)
	{
		throw ModelException("CLIMATE","CheckInputData","The relative humidity can not be NULL.");
		return false;
	}

	if(this->m_sr == NULL)
	{
		throw ModelException("CLIMATE","CheckInputData","The solar radiation can not be NULL.");
		return false;
	}

	if(this->m_tMin == NULL)
	{
		throw ModelException("CLIMATE","CheckInputData","The min temperature can not be NULL.");
		return false;
	}

	if(this->m_tMax == NULL)
	{
		throw ModelException("CLIMATE","CheckInputData","The max temperature can not be NULL.");
		return false;
	}

	if(this->m_ws == NULL)
	{
		throw ModelException("CLIMATE","CheckInputData","The wind speed can not be NULL.");
		return false;
	}
	if (this->m_latitude == NULL)
	{
		throw ModelException("CLIMATE","CheckInputData","The latitude of meteorology stations can not be NULL.");
	}
	return true;
}

bool ClimateParameters::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException("CLIMATE","CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(this->m_size != n)
	{
		if(this->m_size <=0) this->m_size = n;
		else
		{
			throw ModelException("CLIMATE","CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}

	return true;
}

void ClimateParameters::SetValue(const char* key, float value)
{
	string sk(key);
	if (StringMatch(sk, VAR_OMP_THREADNUM))
	{
		omp_set_num_threads((int)value);
	}
	else
	{
		throw ModelException("CLIMATE","SetValue","Parameter " + sk + " does not exist in CLIMATE method. Please contact the module developer.");
	}
}

void ClimateParameters::Set1DData(const char* key,int n, float *value)
{
	if(!this->CheckInputSize(key,n)) return;

	string sk(key);
	if (StringMatch(sk,DataType_MeanTemperature) || StringMatch(sk,"TMEAN"))
	{
		this->m_tMean = value;
	}
	if (StringMatch(sk,DataType_MinimumTemperature))
	{
		this->m_tMin = value;
	}
	else if (StringMatch(sk,DataType_MaximumTemperature))
	{
		this->m_tMax = value;
	}
	else if (StringMatch(sk,DataType_RelativeAirMoisture))
	{
		this->m_rhd = value;
	}
	else if (StringMatch(sk,DataType_SolarRadiation))
	{
		this->m_sr = value;
	}
	else if (StringMatch(sk, Tag_Latitude_Meteorology))
	{
		this->m_latitude = value;
	}
	else
	{
		throw ModelException("CLIMATE","SetValue","Parameter " + sk + " does not exist in CLIMATE module. Please contact the module developer.");
	}
	/// If TMEAN is not existed, then set TMEAN = (TMAX + TMIN) / 2
	if (this->m_tMean == NULL){
		for (int i = 0; i < m_size; ++i)
			m_tMean[i] = (m_tMax[i] + m_tMin[i]) / 2;
	}
}

int ClimateParameters::Execute()
{	
	if(!this->CheckInputData()) return false;
	if(this->m_srMax == NULL)
	{
		this->m_srMax = new float[this->m_size];
	}
	int d = JulianDay(this->m_date);
#pragma omp parallel for
	/// Calculate climate parameters
	for (int i = 0; i < m_size; ++i)
	{
		/// calculate the max solar radiation
		m_srMax[i] = this->MaxSolarRadiation(d,this->m_latitude[i]);
		/// calculate saturated vapor pressure and actual vapor pressure
		m_svp[i] = SaturationVaporPressure(m_tMean[i]);
		m_avp[i] = m_rhd[i] * m_svp[i];
		//calculate latent heat of vaporization(MJ/kg, from swat)
		float latentHeat = 2.501f - 0.002361f * m_tMean[i];
	}
	return 0;
}

void ClimateParameters::Get1DData(const char* key, int* n, float** data)
{
	string sk(key);
	if (StringMatch(sk, VAR_SR_MAX))
	{
		*data = this->m_srMax;
		*n = this->m_size;
	}
	else if (StringMatch(sk, VAR_VP_SAT))
	{
		*data = this->m_svp;
		*n = this->m_size;
	}
	else if (StringMatch(sk, VAR_VP_ACT))
	{
		*data = this->m_avp;
		*n = this->m_size;
	}
	else
	{
		throw ModelException("CLIMATE", "Get1DData","Parameter " + sk + " does not exist. Please contact the module developer.");
	}
}

//// FUNCTIONS IMPLEMENTATION ////

float ClimateParameters::SaturationVaporPressure(float t)
{
	/// Calculate saturation vapor pressure, equation 1:2.3.2 in SWAT Theory 2009, p54
	/// Tetens (1930) and Murray (1967), ee.f in SWAT src.
	if (!FloatEqual(t + 237.3f, UTIL_ZERO))
	{
		float ee = (16.78f * t - 116.9f) / (t + 237.3f);
		return exp(ee);
	}
	return 0.0f;
}

float ClimateParameters::MaxSolarRadiation(int day,float lat)
{
	lat = lat*3.1415926/180;
	//Calculate Daylength
	//calculate solar declination: equation 1:1.1.2 in SWAT Theory 2009, p31
	float sd = asin(0.4f * sin((day - 82.0f) / 58.09f));  /// 365/2pi = 58.09

	//calculate the relative distance of the earth from the sun
	//also called the eccentricity correction factor of the orbit, Duffie and Beckman(1980)
	//equation 1:1.1.1 in SWAT Theory 2009, p30
	float dd = 1.0f + 0.033f * cos(day / 58.09f);

	//daylength = 2 * Acos(-Tan(sd) * Tan(lat)) / omega
	//where the angular velocity of the earth's rotation, omega, is equal
	//to 15 deg/hr or 0.2618 rad/hr and 2/0.2618 = 7.6374
	//equation 2.1.6 in SWAT manual

	float h = 0.0f;
	/// equation 1:1.1.4 in SWAT Theory 2009, p32
	float ch = -sin(lat) * tan(sd) / cos(lat);
	if (ch > 1.f) //ch will be >= 1. if latitude exceeds +/- 66.5 deg in winter
		h = 0.0f;
	else if (ch >= -1.0f)
		h = acos(ch);
	else
		h = 3.1416f; //latitude exceeds +/- 66.5 deg in summer

	float dayl = 7.6394f * h; /// useless?

	//Calculate Potential (maximum) Radiation !!
	/// equation 1:1.1.3 in SWAT Theory 2009, p31
	float ys = sin(lat) * sin(sd);
	float yc = cos(lat) * cos(sd);
	/// equation 1:1.1.7 in SWAT Theory 2009, p34
	return 30.f * dd * (h * ys + yc * sin(h));
}

int ClimateParameters::JulianDay(time_t date)
{
	struct tm dateInfo;
	LocalTime(date, &dateInfo);
	return dateInfo.tm_yday + 1;
}



