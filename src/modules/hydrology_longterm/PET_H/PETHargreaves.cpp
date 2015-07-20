#include "PETHargreaves.h"
#include "MetadataInfo.h"
#include <vector>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include "util.h"
#include "text.h"
#include "ModelException.h"
#include <omp.h>

using namespace std;

PETHargreaves::PETHargreaves(void):m_size(-1), m_petFactor(1.f), m_tMin(NULL), m_tMax(NULL), m_pet(NULL)
{
}

PETHargreaves::~PETHargreaves(void)
{
	if(this->m_pet != NULL)
	{
		delete [] this->m_pet;
	}
}

void PETHargreaves::SetValue(const char* key, float value)
{
	string sk(key);
	if (StringMatch(sk,"K_pet"))
	{
		m_petFactor  = value;
	}
	else if (StringMatch(sk, "ThreadNum"))
	{
		omp_set_num_threads((int)value);
	}
	else
	{
		throw ModelException("PET_PM","SetValue","Parameter " + sk + " does not exist in Hargreaves method. Please contact the module developer.");
	}

}

void PETHargreaves::Set1DData(const char* key,int n, float *value)
{
	if(!this->CheckInputSize(key,n)) return;

	string sk(key);
	if (StringMatch( sk,"TMAX"))
	{
		m_tMax = value;
	}
	else if (StringMatch( sk, "TMIN"))
	{
		m_tMin = value;
	}
	else if (StringMatch( sk, Tag_Latitude_Meteorology))
	{
		this->m_latitude = value;
	}
	else
	{
		throw ModelException("PET_H","SetValue","Parameter " + sk + " does not exist in PETHargreaves method. Please contact the module developer.");
	}

}

int PETHargreaves::JulianDay(time_t date)
{
	struct tm dateInfo;
	LocalTime(date, &dateInfo);
	return dateInfo.tm_yday + 1;
}

///The source code come from Junzhi's MaxRadiation project.
float PETHargreaves::MaxSolarRadiation(int day,float lat)
{
	  lat = lat*3.1415926/180;
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

int PETHargreaves::Execute()
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
		float tMean = (m_tMax[i] + m_tMin[i])/2;
		float srMax = this->MaxSolarRadiation(d, m_latitude[i]);

		//calculate latent heat of vaporization(from swat)
		float latentHeat = 2.501f - 0.002361f * tMean;
		// extraterrestrial radiation(from swat)
		// 37.59 is coefficient in equation 2.2.6 !!extraterrestrial
		// 30.00 is coefficient in equation 2.2.7 !!max at surface
		float h0 = srMax * 37.59f / 30.0f;
		float petValue = 0.0023f * h0 * pow(m_tMax[i]-m_tMin[i], 0.5f)
			* (tMean + 17.8f) / latentHeat;
		m_pet[i] = m_petFactor * max(0.0f, petValue);

		//if (FloatEqual(m_pet[i], 0.f))
		//{
		//	int a = 0.f;
		//}
	}
	return 0;
}


void PETHargreaves::Get1DData(const char* key, int* n, float **data)
{
	if(this->m_pet == NULL) 
		throw ModelException("PET_H","getPET","The result is NULL. Please first execute the module.");
	*data = this->m_pet;
	*n = this->m_size;
}

bool PETHargreaves::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException("PET_H","CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
	}
	if(this->m_size != n)
	{
		if(this->m_size <=0) this->m_size = n;
		else
		{
			throw ModelException("PET_H","CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
		}
	}

	return true;
}

bool PETHargreaves::CheckInputData()
{
	if(this->m_date == -1)
	{
		throw ModelException("PET_H","CheckInputData","You have not set the time.");
		return false;
	}

	if(m_size <= 0)
	{
		throw ModelException("PET_H","CheckInputData","The dimension of the input data can not be less than zero.");
	}

	if(this->m_tMax == NULL)
	{
		throw ModelException("PET_H","CheckInputData","The maximum temeprature can not be NULL.");
	}

	if(this->m_tMin == NULL)
	{
		throw ModelException("PET_H","CheckInputData","The minimum temeprature can not be NULL.");
	}

	return true;
}