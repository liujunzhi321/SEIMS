#include "PETPriestleyTaylor.h"
#include "MetadataInfo.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include "ModelException.h"
#include "util.h"
#include "text.h"
#include <omp.h>

using namespace std;

PETPriestleyTaylor::PETPriestleyTaylor(void):m_tMin(NULL), m_tMax(NULL), m_sr(NULL), m_rhd(NULL), m_elev(NULL), m_pet(NULL), m_petFactor(1.0f)
{
	this->m_size = -1;
}

PETPriestleyTaylor::~PETPriestleyTaylor(void)
{
	if(this->m_pet != NULL)
	{
		delete [] this->m_pet;
	}
}

void PETPriestleyTaylor::Get1DData(const char* key, int* n, float **data)
{
	string sk(key);
	if (StringMatch(sk, "T_PET") || StringMatch(sk, "PET"))   //infiltration
	{
		*data = this->m_pet;
		*n = this->m_size;
	}
	else
	{
		throw ModelException("PET_PT", "Get1DData","Parameter " + sk + " does not exist. Please contact the module developer.");

	}
}

bool PETPriestleyTaylor::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException("PET_PT","CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
	}
	if(this->m_size != n)
	{
		if(this->m_size <=0) this->m_size = n;
		else
		{
			throw ModelException("PET_PT","CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}

	return true;
}

bool PETPriestleyTaylor::CheckInputData()
{
	if(this->m_date == -1)
	{
		throw ModelException("PET_PT","CheckInputData","You have not set the time.");
		return false;
	}

	if(m_size <= 0)
	{
		throw ModelException("PET_PT","CheckInputData","The dimension of the input data can not be less than zero.");
	}

	if(this->m_elev == NULL)
	{
		throw ModelException("PET_PT","CheckInputData","The elevation can not be NULL.");
	}

	if(this->m_latitude == NULL)
	{
		throw ModelException("PET_PT","CheckInputData","The latitude can not be NULL.");
	}

	if(this->m_rhd == NULL)
	{
		throw ModelException("PET_PT","CheckInputData","The relative humidity can not be NULL.");
	}

	if(this->m_sr == NULL)
	{
		throw ModelException("PET_PT","CheckInputData","The solar radiation can not be NULL.");
	}

	if(this->m_tMin == NULL)
	{
		throw ModelException("PET_PT","CheckInputData","The min temeprature can not be NULL.");
	}

	if(this->m_tMax == NULL)
	{
		throw ModelException("PET_PT","CheckInputData","The max temeprature can not be NULL.");
	}

	return true;
}

float PETPriestleyTaylor::SaturationVaporPressure(float t)
{
	if ( abs(t + 237.3f) >= 1.0e-6f)
	{
		float ee = (16.78f * t - 116.9f) / (t + 237.3f);
		return exp(ee);
	}
	return 0.0f;
}

int PETPriestleyTaylor::JulianDay(time_t date)
{
	struct tm dateInfo;
	LocalTime(date, &dateInfo);
	return dateInfo.tm_yday + 1;
}

int PETPriestleyTaylor::Execute()
{
	if(!this->CheckInputData()) return false;
	if(this->m_pet == NULL)
	{
		this->m_pet = new float[this->m_size];
	}
	int d = JulianDay(this->m_date);
	#pragma omp parallel for
	for (int i = 0; i < m_size; ++i)
	{
		//////////////////////////////////////////////////////////////////////////
		//compute net radiation

		//net short-wave radiation for PET(from swat)
		float raShortWave = m_sr[i] * (1.0f - 0.23f);
		float tMean = (m_tMax[i] + m_tMin[i]) / 2;

		//*********************************************************
		//if(m_snow[j] > 0.5f) 
		if(tMean < this->m_tSnow)		//if the mean t < snow t, consider the snow depth is larger than 0.5mm.
			raShortWave = m_sr[i] * (1.0f - 0.8f);
		//*********************************************************

		//calculate the max solar radiation
		float srMax = this->MaxSolarRadiation(d,this->m_latitude[i]);

		//calculate net long-wave radiation
		//net emissivity  equation 2.2.20 in SWAT manual
		float satVaporPressure = SaturationVaporPressure(tMean);
		float actualVaporPressure = m_rhd[i] * satVaporPressure;
		float rbo = -(0.34f - 0.139f * sqrt(actualVaporPressure));
		//cloud cover factor equation 2.2.19
		float rto = 0.0f;
		if (srMax >= 1.0e-4)
			rto = 0.9f * (m_sr[i] / srMax) + 0.1f;
		//net long-wave radiation equation 2.2.21
		float tk = tMean + 273.15f;
		float raLongWave = rbo * rto * 4.9e-9f * pow(tk, 4);

		float raNet = raShortWave + raLongWave;

		//////////////////////////////////////////////////////////////////////////
		//calculate the slope of the saturation vapor pressure curve
		float dlt = 4098.f* satVaporPressure / pow((tMean + 237.3f), 2);

		//calculate latent heat of vaporization(MJ/kg, from swat)
		float latentHeat = 2.501f - 0.002361f * tMean;

		//calculate mean barometric pressure(kPa)
		float pb = 101.3f - m_elev[i] * (0.01152f - 0.544e-6f * m_elev[i]);
		//psychrometric constant(kPa/deg C)
		float gma = 1.013e-3f * pb / (0.622f * latentHeat);
		float pet_alpha = 1.28f;

		float petValue = pet_alpha * (dlt / (dlt + gma)) * raNet / latentHeat;
		m_pet[i] = m_petFactor * max(0.0f, petValue);
        
	}

	return 0;
}

void PETPriestleyTaylor::Set1DData(const char* key,int n, float *value)
{
	if(!this->CheckInputSize(key,n)) return;

	string sk(key);
	if (StringMatch(sk,"TMIN"))
	{
		m_tMin = value;
	}
	else if (StringMatch(sk,"TMAX"))
	{
		m_tMax = value;
	}
	else if (StringMatch(sk,"RM"))
	{
		m_rhd = value;
	}
	else if (StringMatch(sk,"SR"))
	{
		m_sr = value;
	}
	else if (StringMatch(sk, Tag_Elevation_Meteorology))
	{
		m_elev = value;
	}
	else if (StringMatch(sk, Tag_Latitude_Meteorology))
	{
		this->m_latitude = value;
	}
	else
	{
		throw ModelException("PET_PT","SetValue","Parameter " + sk + " does not exist in PETPriestleyTaylor method. Please contact the module developer.");
	}

}

void PETPriestleyTaylor::SetValue(const char* key, float value)
{
	string sk(key);
	if (StringMatch(sk,"T_snow"))
	{
		this->m_tSnow = value;
	}
	else if (StringMatch(sk,"K_pet"))
	{
		m_petFactor = value;
	}
	else if (StringMatch(sk, "ThreadNum"))
	{
		omp_set_num_threads((int)value);
	}
	else
	{
		throw ModelException("PET_PT","SetValue","Parameter " + sk + " does not exist in PETPriestleyTaylor method. Please contact the module developer.");
	}

}

//The source code come from Junzhi's MaxRadiation project.
float PETPriestleyTaylor::MaxSolarRadiation(int day,float lat)
{
	  lat = lat*3.1415926f/180;
	  //Calculate Daylength
      //calculate solar declination: equation 2.1.2 in SWAT manual
      float sd = asin(0.4f * sin((day - 82.0f) / 58.09f));  //365/2pi = 58.09

      //calculate the relative distance of the earth from the sun
      //the eccentricity of the orbit
      //equation 2.1.1 in SWAT manual
      float dd = 1.0f + 0.033f * cos(day / 58.09f);

      //daylength = 2 * Acos(-Tan(sd) * Tan(lat)) / omega
      //where the angular velocity of the earth's rotation, omega, is equal
      //to 15 deg/hr or 0.2618 rad/hr and 2/0.2618 = 7.6374
      //equation 2.1.6 in SWAT manual

      float h = 0.0f;
      float ch = -sin(lat) * tan(sd) / cos(lat);
      if (ch > 1.f) //ch will be >= 1. if latitude exceeds +/- 66.5 deg in winter
        h = 0.0f;
      else if (ch >= -1.0f)
        h = acos(ch);
      else
        h = 3.1416f; //latitude exceeds +/- 66.5 deg in summer
      
      float dayl = 7.6394f * h;
          
      //Calculate Potential (maximum) Radiation !!
      //equation 2.2.7 in SWAT manual
      float ys = sin(lat) * sin(sd);
      float yc = cos(lat) * cos(sd);
      
	  return 30.f * dd * (h * ys + yc * sin(h));
}

