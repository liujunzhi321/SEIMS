#include "PETPenmanMonteith.h"
#include "MetadataInfo.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include "ModelException.h"
#include "util.h"
#include <omp.h>

using namespace std;

PETPenmanMonteith::PETPenmanMonteith(void):m_cht(NULL), m_elev(NULL), m_growCode(NULL), m_lai(NULL), m_rhd(NULL), m_sr(NULL), 
	m_tMin(NULL), m_tMax(NULL), m_ws(NULL), m_pet(NULL), m_petFactor(1.f)
{
	this->m_size = -1;
}

PETPenmanMonteith::~PETPenmanMonteith(void)
{
	if(this->m_pet != NULL)
	{
		delete [] this->m_pet;
	}

	//This code would be removed if the growcode is got from main.
	if(this->m_growCode != NULL)
	{
		delete [] this->m_growCode;
	}

	if(this->m_cht != NULL)
	{
		delete [] this->m_cht;
	}

	if(this->m_lai != NULL)
	{
		delete [] this->m_lai;
	}
}

void PETPenmanMonteith::Get1DData(const char* key, int* n, float **data)
{
	*data = this->m_pet;
	*n = this->m_size;
}

void PETPenmanMonteith::clearInputs()
{
	this->m_date = -1;
}

bool PETPenmanMonteith::CheckInputData()
{
	if(this->m_date == -1)
	{
		throw ModelException("PET_PM","CheckInputData","You have not set the time.");
		return false;
	}

	if(m_size <= 0)
	{
		throw ModelException("PET_PM","CheckInputData","The dimension of the input data can not be less than zero.");
		return false;
	}

	if(this->m_cht == NULL)
	{
		throw ModelException("PET_PM","CheckInputData","The canopy height can not be NULL.");
		return false;
	}

	if(this->m_elev == NULL)
	{
		throw ModelException("PET_PM","CheckInputData","The elevation can not be NULL.");
		return false;
	}

	if(this->m_growCode == NULL)
	{
		throw ModelException("PET_PM","CheckInputData","The land cover status code can not be NULL.");
		return false;
	}

	if(this->m_lai == NULL)
	{
		throw ModelException("PET_PM","CheckInputData","The leaf area index can not be NULL.");
		return false;
	}

	if(this->m_rhd == NULL)
	{
		throw ModelException("PET_PM","CheckInputData","The relative humidity can not be NULL.");
		return false;
	}

	if(this->m_sr == NULL)
	{
		throw ModelException("PET_PM","CheckInputData","The solar radiation can not be NULL.");
		return false;
	}

	if(this->m_tMin == NULL)
	{
		throw ModelException("PET_PM","CheckInputData","The min temperature can not be NULL.");
		return false;
	}

	if(this->m_tMax == NULL)
	{
		throw ModelException("PET_PM","CheckInputData","The max temperature can not be NULL.");
		return false;
	}

	if(this->m_ws == NULL)
	{
		throw ModelException("PET_PM","CheckInputData","The wind speed can not be NULL.");
		return false;
	}

	return true;
}

bool PETPenmanMonteith::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException("PET_PM","CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(this->m_size != n)
	{
		if(this->m_size <=0) this->m_size = n;
		else
		{
			throw ModelException("PET_PM","CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}

	return true;
}

int PETPenmanMonteith::JulianDay(time_t date)
{
	struct tm dateInfo;
	LocalTime(date, &dateInfo);
	return dateInfo.tm_yday + 1;
}

int PETPenmanMonteith::Execute()
{	
	//*****************************************************
	//Now, all the grow codes are constant and equle to 1
	if(this->m_growCode == NULL && this->m_size>0)
	{
		this->m_growCode = new float[this->m_size];
		this->m_cht = new float[this->m_size];
		this->m_lai = new float[this->m_size];
		this->m_pet = new float[this->m_size];

		#pragma omp parallel for
		for(int i = 0; i<m_size ; i++)
		{
			this->m_growCode[i] = 1;
			this->m_cht[i] = 1.5f;
			this->m_lai[i] = 0.95f;
			m_pet[i] = 0.f;
		}
	}

	//check the data
	if(!this->CheckInputData()) return false;	

	int d = JulianDay(this->m_date);
	//do the execute
	#pragma omp parallel for
	for (int j = 0; j < m_size; ++j)
	{
		//////////////////////////////////////////////////////////////////////////
		//compute net radiation
		//this->StatusMsg("compute net radiation ...");

		//net short-wave radiation for PET(from swat)
		float raShortWave = m_sr[j] * (1.0f - 0.23f);
		float tMean = (m_tMax[j] + m_tMin[j]) / 2;

		//*********************************************************
		//if(m_snow[j] > 0.5f) 
		if(tMean < this->m_tSnow)		//if the mean t < snow t, consider the snow depth is larger than 0.5mm.
			raShortWave = m_sr[j] * (1.0f - 0.8f);
		//*********************************************************

		//calculate the max solar radiation
		float srMax = this->MaxSolarRadiation(d,this->m_latitude[j]);

		//calculate net long-wave radiation
		//net emissivity  equation 2.2.20 in SWAT manual
		float satVaporPressure = SaturationVaporPressure(tMean);//kPa
		float actualVaporPressure = m_rhd[j] * satVaporPressure; //why *0.01?
		float rbo = -(0.34f - 0.139f * sqrt(actualVaporPressure)); //P37 1:1.2.22
		//cloud cover factor equation 2.2.19
		float rto = 0.0f;
		if (srMax >= 1.0e-4)
			rto = 0.9f * (m_sr[j] / srMax) + 0.1f;
		//net long-wave radiation equation 2.2.21
		float tk = tMean + 273.15f;
		float raLongWave = rbo * rto * 4.9e-9f * pow(tk, 4);

		float raNet = raShortWave + raLongWave;  //Hnet

		//////////////////////////////////////////////////////////////////////////
		//calculate the slope of the saturation vapor pressure curve
		//this->StatusMsg("calculate the slope of the saturation vapor pressure curve ...");

		float dlt = 4098.f* satVaporPressure / pow((tMean + 237.3f), 2);

		//calculate latent heat of vaporization(MJ/kg, from swat)
		float latentHeat = 2.501f - 0.002361f * tMean;

		//calculate mean barometric pressure(kPa)
		float pb = 101.3f - m_elev[j] * (0.01152f - 0.544e-6f * m_elev[j]); //p53 1:2.3.8 P
		//psychrometric constant(kPa/deg C)
		float gma = 1.013e-3f * pb / (0.622f * latentHeat);//p53 1:2.3.7

		float rho = 1710.f - 6.85f * tMean;//p127 2:2.2.19
		//aerodynamic resistance to sensible heat and vapor transfer(s/m)
		float rv = 114.f / (m_ws[j] * pow(170.f/1000.f, 0.2f)); // P127 2:2.2.20  ??the wind speed need modified?
		//canopy resistance(s/m)
		float rc = 49.f / (1.4f - 0.4f * m_co2 / 330.f);  //P128 2:2.2.22
		//vapor pressure deficit(kPa)
		float vpd = satVaporPressure - actualVaporPressure;
		float petValue = (dlt * raNet + gma * rho * vpd / rv) /
			(latentHeat * (dlt + gma * (1.f + rc / rv)));  //P122 2:2.2.2
		petValue = max(0.0f, petValue);

		//*********************************************************
		//The albedo would be obtained from plant growth module. But now it is assumed to be a constant.
		//After the plant growth module is completed, the following codes shouble be removed.
		float albedo = 0.8f;
		if(tMean > this->m_tSnow) albedo = 0.23f;
		//*********************************************************

		float raShortWavePlant = m_sr[j] * (1.0f -albedo);
		float raNetPlant = raShortWavePlant + raLongWave;

		float epMax = 0.0f;
		if (m_growCode[j] > 0)//land cover growing
		{
			//determine wind speed and height of wind speed measurement
			//adjust to 100 cm (1 m) above canopy if necessary
			float zz = 0.f;  //height at which wind speed is determined(cm)
			if (m_cht[j] <= 1.0f)
				zz = 170.0f;
			else
				zz = m_cht[j] * 100.f + 100.f;
			//wind speed at height zz(m/s)
			float uzz = m_ws[j] * pow(zz/1000.f, 0.2f);

			//calculate canopy height in cm
			float chz = 0.0f;
			if (m_cht[j] < 0.01f)
				chz = 1.0f;
			else
				chz = m_cht[j] * 100.0f;

			//calculate roughness length for momentum transfer
			float zom = 0.0f;
			if (chz <= 200.0f)
				zom = 0.123f * chz;
			else
				zom = 0.058f * pow(chz, 1.19f);

			//calculate roughness length for vapor transfer
			float zov = 0.1f * zom;

			//calculate zero-plane displacement of wind profile
			float d = 0.667f * chz;

			//calculate aerodynamic resistance
			float rv = log((zz - d) / zom) * log((zz - d) / zov);
			rv = rv / (pow(0.41f,2) * uzz);

			//adjust stomatal conductivity for low vapor pressure
			//this adjustment will lower maximum plant ET for plants
			//sensitive to very low vapor pressure
			float xx = vpd - 1.0f;//difference between vpd and vpthreshold
			//amount of vapro pressure deficit over threshold value
			float fvpd = 1.0f;
			if (xx > 0.0f)
				fvpd = max(0.1f, 1.0f - m_vpd2 * xx);
			//fvpd = max(0.1f, 1.0 - vpd2(idplt(nro(j),icr(j),j)) * xx);
			float gsi_adj = m_gsi * fvpd;
			//gsi(idplt(nro(j),icr(j),j)) * fvpd;

			//calculate canopy resistance
			float rc = 1.0f / gsi_adj;	//single leaf resistance
			//rc = rc / (0.5f * (laiday(j) + 0.01) * (1.4 - 0.4 * co2(hru_sub(j)) / 330.));
			rc = rc / (0.5f * (m_lai[j] + 0.01f) * (1.4f - 0.4f * m_co2/330.f));

			//calculate maximum plant ET
			epMax = (dlt * raNetPlant + gma * rho * vpd / rv) / (latentHeat * (dlt + gma * (1.f + rc / rv)));
		}
		epMax = max(0.0f, epMax);

		m_pet[j] = m_petFactor * min(petValue, epMax);
	}
	
	return 0;
}

//SWAT Manual P52 1:2.3.2
float PETPenmanMonteith::SaturationVaporPressure(float t)
{
	if ( abs(t + 237.3f) >= 1.0e-6f)
	{
		float ee = (16.78f * t - 116.9f) / (t + 237.3f);
		return exp(ee);
	}
	return 0.0f;
}

//The source code come from Junzhi's MaxRadiation project.
float PETPenmanMonteith::MaxSolarRadiation(int day,float lat)
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

void PETPenmanMonteith::SetValue(const char* key, float value)
{
	string sk(key);
	if (StringMatch(sk,"Co2"))
	{
		m_co2 = value;
	}
	else if (StringMatch(sk,"Cond_rate"))
	{
		m_vpd2 = value;
	}
	else if (StringMatch(sk,"Cond_max"))
	{
		m_gsi = value;
	}
	else if (StringMatch(sk,"T_snow"))
	{
		this->m_tSnow = value;
	}
	else if (StringMatch(sk,"K_pet"))
	{
		m_petFactor = value;
	}
	else if (StringMatch(sk, VAR_OMP_THREADNUM))
	{
		omp_set_num_threads((int)value);
	}
	else
	{
		throw ModelException("PET_PM","SetValue","Parameter " + sk + " does not exist in PETPenmanMonteith method. Please contact the module developer.");
	}

}

void PETPenmanMonteith::Set1DData(const char* key,int n, float *value)
{
	if(!this->CheckInputSize(key,n)) return;
	string sk(key);
	//if (StringMatch( sk,DataType_MeanTemperature))
	//	this->m_tMean = value;
	if (StringMatch( sk,DataType_MaximumTemperature))
		this->m_tMax = value;
	else if (StringMatch( sk, DataType_MinimumTemperature))
		this->m_tMin = value;
	else if (StringMatch( sk, Tag_Latitude_Meteorology))
		this->m_latitude = value;
	else if (StringMatch(sk, DataType_RelativeAirMoisture))
		this->m_rhd = value;
	else if (StringMatch(sk, DataType_SolarRadiation))
		this->m_sr = value;
	else if (StringMatch(sk, DataType_WindSpeed))
		this->m_ws = value;
	//from hydroclimate stations table
	else if (StringMatch(sk, Tag_Elevation_Meteorology))
		this->m_elev = value;
	else if (StringMatch(sk, Tag_Latitude_Meteorology))
		this->m_latitude = value;
	//from grid file
	else if (StringMatch(sk,"CHT"))
		m_cht = value;
	//from LAI model
	else if (StringMatch(sk,"LAIDAY"))
		m_lai = value;
	else if (StringMatch(sk, "IGRO"))
		this->m_growCode = value;
	else
		throw ModelException("PET_PM","SetValue","Parameter " + sk + " does not exist in PETPenmanMonteith method. Please contact the module developer.");
}



