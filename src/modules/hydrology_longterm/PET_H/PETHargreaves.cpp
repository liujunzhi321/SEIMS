/*!
 * \file PETHargreaves.cpp
 *
 * \author Junzhi Liu
 * \date Nov. 2010
 * \revised LiangJun Zhu
 *
 */
#include "PETHargreaves.h"
#include "MetadataInfo.h"
#include <vector>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include "util.h"
#include "ModelException.h"
#include <omp.h>

using namespace std;

PETHargreaves::PETHargreaves(void):m_size(-1), m_petFactor(1.f),m_HCoef_pet(0.0023f), m_tMin(NULL), m_tMax(NULL), m_pet(NULL)
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
	if (StringMatch(sk,VAR_PET_K))
	{
		m_petFactor  = value;
	}
	else if (StringMatch(sk, VAR_PET_HCOEF))
	{
		m_HCoef_pet = value;
	}
	else if (StringMatch(sk, VAR_JULIAN_DAY))
	{
		m_jday = value;
	}
	else if (StringMatch(sk, VAR_OMP_THREADNUM))
	{
		omp_set_num_threads((int)value);
	}
	else
	{
		throw ModelException("PET_H","SetValue","Parameter " + sk + " does not exist in Hargreaves method. Please contact the module developer.");
	}

}

void PETHargreaves::Set1DData(const char* key,int n, float *value)
{
	if(!this->CheckInputSize(key,n)) return;
	string sk(key);
	if (StringMatch( sk,DataType_MeanTemperature))
		this->m_tMean = value;
	else if (StringMatch( sk,DataType_MaximumTemperature))
		this->m_tMax = value;
	else if (StringMatch( sk, DataType_MinimumTemperature))
		this->m_tMin = value;
	else if (StringMatch( sk, Tag_Latitude_Meteorology))
		this->m_latitude = value;
	else if (StringMatch(sk, VAR_SR_MAX))
		this->m_srMax = value;
	else
		throw ModelException(MID_PET_H,"Set1DValue","Parameter " + sk + " does not exist in PETHargreaves method. Please contact the module developer.");
}

// int PETHargreaves::JulianDay(time_t date)
// {
// 	struct tm dateInfo;
// 	LocalTime(date, &dateInfo);
// 	return dateInfo.tm_yday + 1;
// }


// float PETHargreaves::MaxSolarRadiation(int day,float lat)
// {
// 	  lat = lat*3.1415926/180;
// 	  //Calculate Daylength
//       //calculate solar declination: equation 1:1.1.2 in SWAT Theory 2009, p31
//       float sd = asin(0.4f * sin((day - 82.0f) / 58.09f));  /// 365/2pi = 58.09
// 
//       //calculate the relative distance of the earth from the sun
//       //also called the eccentricity correction factor of the orbit, Duffie and Beckman(1980)
//       //equation 1:1.1.1 in SWAT Theory 2009, p30
//       float dd = 1.0f + 0.033f * cos(day / 58.09f);
// 
//       //daylength = 2 * Acos(-Tan(sd) * Tan(lat)) / omega
//       //where the angular velocity of the earth's rotation, omega, is equal
//       //to 15 deg/hr or 0.2618 rad/hr and 2/0.2618 = 7.6374
//       //equation 2.1.6 in SWAT manual
// 
//       float h = 0.0f;
// 	  /// equation 1:1.1.4 in SWAT Theory 2009, p32
//       float ch = -sin(lat) * tan(sd) / cos(lat);
//       if (ch > 1.f) //ch will be >= 1. if latitude exceeds +/- 66.5 deg in winter
//         h = 0.0f;
//       else if (ch >= -1.0f)
//         h = acos(ch);
//       else
//         h = 3.1416f; //latitude exceeds +/- 66.5 deg in summer
//       
//       float dayl = 7.6394f * h; /// useless?
//           
//       //Calculate Potential (maximum) Radiation !!
//       /// equation 1:1.1.3 in SWAT Theory 2009, p31
//       float ys = sin(lat) * sin(sd);
//       float yc = cos(lat) * cos(sd);
//       /// equation 1:1.1.7 in SWAT Theory 2009, p34
// 	  return 30.f * dd * (h * ys + yc * sin(h));
// }

int PETHargreaves::Execute()
{
	if(!this->CheckInputData()) return false;
	if(NULL == m_pet)
		this->m_pet = new float[m_size];
	// int d = JulianDay(this->m_date);
	//cout<<m_tMean[0]<<","<<m_tMax[0]<<","<<m_tMin[0]<<endl;
#pragma omp parallel for
	for (int i = 0; i < m_size; ++i)
	{	
		//float tMean = (m_tMax[i] + m_tMin[i])/2;
		//float srMax = this->MaxSolarRadiation(m_jday, m_latitude[i]);

		///calculate latent heat of vaporization(from swat)
		float latentHeat = 2.501f - 0.002361f * m_tMean[i];
		/// extraterrestrial radiation
		/// equation 1:1.1.6 in SWAT Theory 2009, p33
		float h0 = m_srMax[i] * 37.59f / 30.0f;
		/// calculate potential evapotranspiration, equation 2:2.2.24 in SWAT Theory 2009, p133
		/// Hargreaves et al., 1985. In SWAT Code, 0.0023 is replaced by harg_petco, which range from 0.0019 to 0.0032. by LJ
		float petValue = m_HCoef_pet * h0 * pow(m_tMax[i]-m_tMin[i], 0.5f)
			* (m_tMean[i] + 17.8f) / latentHeat;
		m_pet[i] = m_petFactor * max(0.0f, petValue);
	}
	/// Test code
	/*for (int i = 0; i < m_size; ++i)
		cout<< "m_srMax: " << m_srMax[i] << ", m_pet: " << m_pet[i] << "; ";*/
	//cout<<endl;
	return 0;
}


void PETHargreaves::Get1DData(const char* key, int* n, float **data)
{
	string sk(key);
	if(this->m_pet == NULL) 
		throw ModelException(MID_PET_H,"GetPET","The result is NULL. Please first execute the module.");
	if (StringMatch(sk, VAR_PET_T) || StringMatch(sk, "PET"))
	{
		*data = this->m_pet;
		*n = this->m_size;
	}
	else
		throw ModelException(MID_PET_H, "Get1DData","Parameter " + sk + " does not exist. Please contact the module developer.");
}

bool PETHargreaves::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException(MID_PET_H,"CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
	}
	if(this->m_size != n)
	{
		if(this->m_size <=0) this->m_size = n;
		else
		{
			throw ModelException(MID_PET_H,"CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
		}
	}

	return true;
}

bool PETHargreaves::CheckInputData()
{
	if(this->m_date == -1)
	{
		throw ModelException(MID_PET_H,"CheckInputData","You have not set the time.");
		return false;
	}

	if(m_size <= 0)
	{
		throw ModelException(MID_PET_H,"CheckInputData","The dimension of the input data can not be less than zero.");
	}

	if(this->m_tMax == NULL)
	{
		throw ModelException(MID_PET_H,"CheckInputData","The maximum temperature can not be NULL.");
	}

	if(this->m_tMin == NULL)
	{
		throw ModelException(MID_PET_H,"CheckInputData","The minimum temperature can not be NULL.");
	}
	if(this->m_srMax == NULL)
	{
		throw ModelException(MID_PET_H,"CheckInputData","The maximum solar radiation can not be NULL.");
	}
	if(this->m_tMean == NULL)
	{
		throw ModelException(MID_PET_H,"CheckInputData","The mean temperature can not be NULL.");
	}
	if(this->m_jday <= 0)
	{
		throw ModelException(MID_PET_H,"CheckInputData","The Julian day must be greater than 0.");
	}
	return true;
}