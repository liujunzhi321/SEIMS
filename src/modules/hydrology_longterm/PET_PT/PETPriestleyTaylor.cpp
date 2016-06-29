/*!
 * \file PETPriestleyTaylor.cpp
 *
 * \author Junzhi Liu
 * \date Nov. 2010
 *
 */
#include "PETPriestleyTaylor.h"
#include "MetadataInfo.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include "ModelException.h"
#include "util.h"
#include <omp.h>
#include "ClimateParams.h" /// added by LJ, 2016

using namespace std;

PETPriestleyTaylor::PETPriestleyTaylor(void):m_tMin(NULL), m_tMax(NULL), m_sr(NULL), m_rhd(NULL), m_elev(NULL), m_phutot(NULL),
	m_dayLen(NULL),m_phuBase(NULL), m_pet(NULL),m_vpd(NULL) ,m_petFactor(1.0f),m_nCells(-1)
{
}

PETPriestleyTaylor::~PETPriestleyTaylor(void)
{
	if(this->m_dayLen != NULL) delete [] this->m_dayLen;
	if(this->m_phuBase != NULL) delete[] this->m_phuBase;
	if(this->m_pet != NULL)		delete [] this->m_pet;
	if(this->m_vpd != NULL)	delete [] this->m_vpd;
}

void PETPriestleyTaylor::Get1DData(const char* key, int* n, float **data)
{
	CheckInputData();
	initialOutputs();
	string sk(key);
	if (StringMatch(sk, VAR_DAYLEN))
	{
		*data = this->m_dayLen;
		*n = this->m_nCells;
	}
	else if (StringMatch(sk, VAR_PET))
	{
		*data = this->m_pet;
		*n = this->m_nCells;
	}
	else if (StringMatch(sk, VAR_VPD))
	{
		*data = this->m_vpd;
		*n = this->m_nCells;
	}
	else if (StringMatch(sk, VAR_PHUBASE))
	{
		*data = this->m_phuBase;
		*n = this->m_nCells;
	}
	else
		throw ModelException(MID_PET_PT, "Get1DData","Parameter " + sk + " does not exist. Please contact the module developer.");
}

bool PETPriestleyTaylor::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException(MID_PET_PT,"CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
	}
	if(this->m_nCells != n)
	{
		if(this->m_nCells <=0) this->m_nCells = n;
		else
		{
			throw ModelException(MID_PET_PT,"CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}
	return true;
}

bool PETPriestleyTaylor::CheckInputData()
{
	if(this->m_date == -1)
	{
		throw ModelException(MID_PET_PT,"CheckInputData","You have not set the time.");
		return false;
	}
	if(m_nCells <= 0)
		throw ModelException(MID_PET_PT,"CheckInputData","The dimension of the input data can not be less than zero.");
	if(this->m_elev == NULL)
		throw ModelException(MID_PET_PT,"CheckInputData","The elevation can not be NULL.");
	if(this->m_cellLat == NULL)
		throw ModelException(MID_PET_PT,"CheckInputData","The latitude can not be NULL.");
	if(this->m_rhd == NULL)
		throw ModelException(MID_PET_PT,"CheckInputData","The relative humidity can not be NULL.");
	if(this->m_sr == NULL)
		throw ModelException(MID_PET_PT,"CheckInputData","The solar radiation can not be NULL.");
	if(this->m_tMin == NULL)
		throw ModelException(MID_PET_PT,"CheckInputData","The min temperature can not be NULL.");
	if(this->m_tMax == NULL)
		throw ModelException(MID_PET_PT,"CheckInputData","The max temperature can not be NULL.");
	if(this->m_tMean == NULL)
		throw ModelException(MID_PET_PT,"CheckInputData","The mean temperature can not be NULL.");
	if(this->m_phutot == NULL)
		throw ModelException(MID_PET_PT,"CheckInputData","The PHU0 can not be NULL.");
	return true;
}

void PETPriestleyTaylor::initialOutputs()
{
	if(this->m_pet == NULL)
	{
		this->m_pet = new float[this->m_nCells];
		this->m_vpd = new float[m_nCells];
		this->m_dayLen = new float[m_nCells];
		this->m_phuBase = new float[m_nCells];
#pragma omp parallel for
		for (int i = 0; i < m_nCells; ++i)
		{
			this->m_pet[i] = 0.f;
			this->m_vpd[i] = 0.f;
			this->m_dayLen[i] = 0.f;
			this->m_phuBase[i] = 0.f;
		}
	}
}

int PETPriestleyTaylor::Execute()
{
	if(!this->CheckInputData()) return false;
	initialOutputs();
	int d = JulianDay(this->m_date);
#pragma omp parallel for
	for (int i = 0; i < m_nCells; ++i)
	{
		/// update phubase of the simulation year.
		/* update base zero total heat units, src code from SWAT, subbasin.f
		if (tmpav(j) > 0. .and. phutot(hru_sub(j)) > 0.01) then
			phubase(j) = phubase(j) + tmpav(j) / phutot(hru_sub(j))
		end if*/
		if(m_jday == 1) m_phuBase[i] = 0.;
		if(m_tMean[i] > 0. && m_phutot[i] > 0.01)
			m_phuBase[i] += m_tMean[i] / m_phutot[i];

		/// compute net radiation
		/// net short-wave radiation for PET, etpot.f in SWAT src
		float raShortWave = m_sr[i] * (1.0f - 0.23f);
		//if the mean T < T_snow, consider the snow depth is larger than 0.5mm.
		if(m_tMean[i] < this->m_tSnow)		
			raShortWave = m_sr[i] * (1.0f - 0.8f);

		/// calculate the max solar radiation
		MaxSolarRadiation(d, this->m_cellLat[i],this->m_dayLen[i],m_srMax);

		/// calculate net long-wave radiation
		/// net emissivity  equation 2.2.20 in SWAT manual
		float satVaporPressure = SaturationVaporPressure(m_tMean[i]);
		float actualVaporPressure = 0.f;
		if(m_rhd[i] > 1)   /// IF percent unit.
			actualVaporPressure	 = m_rhd[i] * satVaporPressure * 0.01;
		else
			actualVaporPressure = m_rhd[i] * satVaporPressure;
		m_vpd[i] = satVaporPressure - actualVaporPressure;
		float rbo = -(0.34f - 0.139f * sqrt(actualVaporPressure));
		//cloud cover factor
		float rto = 0.0f;
		if (m_srMax >= 1.0e-4)
			rto = 0.9f * (m_sr[i] / m_srMax) + 0.1f;
		//net long-wave radiation
		float tk = m_tMean[i] + 273.15f;
		float raLongWave = rbo * rto * 4.9e-9f * pow(tk, 4);

		float raNet = raShortWave + raLongWave;

		//////////////////////////////////////////////////////////////////////////
		//calculate the slope of the saturation vapor pressure curve
		float dlt = 4098.f* satVaporPressure / pow((m_tMean[i] + 237.3f), 2);

		//calculate latent heat of vaporization(MJ/kg, from swat)
		float latentHeat = LatentHeatVapor(m_tMean[i]);

		//calculate mean barometric pressure(kPa)
		float pb = MeanBarometricPressure(m_elev[i]);
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
	if (StringMatch( sk,DataType_MeanTemperature)) this->m_tMean = value;
	else if (StringMatch(sk,DataType_MinimumTemperature)) this->m_tMin = value;
	else if (StringMatch(sk,DataType_MaximumTemperature)) this->m_tMax = value;
	else if (StringMatch(sk,DataType_RelativeAirMoisture)) this->m_rhd = value;
	else if (StringMatch(sk,DataType_SolarRadiation)) this->m_sr = value;
	else if (StringMatch(sk, VAR_DEM)) this->m_elev = value;
	else if (StringMatch(sk, VAR_CELL_LAT)) this->m_cellLat = value;
	else if (StringMatch(sk, VAR_PHUTOT)) this->m_phutot = value;
	else
		throw ModelException(MID_PET_PT,"Set1DData","Parameter " + sk + " does not exist in current module. Please contact the module developer.");
}

void PETPriestleyTaylor::SetValue(const char* key, float value)
{
	string sk(key);
	if (StringMatch(sk,VAR_T_SNOW)) this->m_tSnow = value;
	else if (StringMatch(sk,VAR_K_PET)) m_petFactor = value;
	else if (StringMatch(sk, VAR_OMP_THREADNUM)) omp_set_num_threads((int)value);
	else
		throw ModelException(MID_PET_PT,"SetValue","Parameter " + sk + " does not exist in current module. Please contact the module developer.");
}


//float PETPriestleyTaylor::MaxSolarRadiation(int day,float lat)
//{
//	lat = lat*3.1415926/180;
//	//Calculate Daylength
//	//calculate solar declination: equation 1:1.1.2 in SWAT Theory 2009, p31
//	float sd = asin(0.4f * sin((day - 82.0f) / 58.09f));  /// 365/2pi = 58.09
//
//	//calculate the relative distance of the earth from the sun
//	//also called the eccentricity correction factor of the orbit, Duffie and Beckman(1980)
//	//equation 1:1.1.1 in SWAT Theory 2009, p30
//	float dd = 1.0f + 0.033f * cos(day / 58.09f);
//
//	//daylength = 2 * Acos(-Tan(sd) * Tan(lat)) / omega
//	//where the angular velocity of the earth's rotation, omega, is equal
//	//to 15 deg/hr or 0.2618 rad/hr and 2/0.2618 = 7.6374
//	//equation 2.1.6 in SWAT manual
//
//	float h = 0.0f;
//	/// equation 1:1.1.4 in SWAT Theory 2009, p32
//	float ch = -sin(lat) * tan(sd) / cos(lat);
//	if (ch > 1.f) //ch will be >= 1. if latitude exceeds +/- 66.5 deg in winter
//		h = 0.0f;
//	else if (ch >= -1.0f)
//		h = acos(ch);
//	else
//		h = 3.1416f; //latitude exceeds +/- 66.5 deg in summer
//
//	float dayl = 7.6394f * h; /// useless?
//
//	//Calculate Potential (maximum) Radiation !!
//	/// equation 1:1.1.3 in SWAT Theory 2009, p31
//	float ys = sin(lat) * sin(sd);
//	float yc = cos(lat) * cos(sd);
//	/// equation 1:1.1.7 in SWAT Theory 2009, p34
//	return 30.f * dd * (h * ys + yc * sin(h));
//}
