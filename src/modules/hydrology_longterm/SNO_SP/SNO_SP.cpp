#include "SNO_SP.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include <math.h>
#include <cmath>
#include "util.h"
#include <omp.h>

#define M_PI       3.14159265358979323846

SNO_SP::SNO_SP(void)
{
	// set default values for member variables	
	this->m_Date = -1;
	this->m_cellSize = -1;
	this->m_t0= -99.0f;
	this->m_kblow= -99.0f;
	//this->m_crain = -99.0f;
	//this->m_csnow = -99.0f;
	this->m_swe = -99.0f;
	this->m_lastSWE = -99.0f;
	this->m_tsnow = -99.0f;
	this->m_swe0 = -99.0f;
	this->m_lagSnow = -99.0f;
	this->m_csnow6 = -99.0f;
	this->m_csnow12 = -99.0f;

	this->m_Pnet = NULL;
	this->m_tMax = NULL;
	this->m_tMin = NULL;
	this->m_SA = NULL;
	this->m_SR = NULL;
	this->m_SE = NULL;

	this->m_SM = NULL;
	m_isInitial = true;

	this->m_packT = NULL;
}

SNO_SP::~SNO_SP(void)
{
	//// cleanup
	if(this->m_SM != NULL) delete [] this->m_SM; 
	if(this->m_packT != NULL) delete [] this->m_packT; 
}

bool SNO_SP::CheckInputData(void)
{
	if(this->m_Date <=0)			throw ModelException("SNO_SP","CheckInputData","You have not set the time.");
	if(m_cellSize <= 0)				throw ModelException("SNO_SP","CheckInputData","The dimension of the input data can not be less than zero.");
	if(this->m_kblow == -99)		throw ModelException("SNO_SP","CheckInputData","The fraction coefficient of snow blowing into or out of the watershed can not be NULL.");
	if(this->m_Pnet == NULL)		throw ModelException("SNO_SP","CheckInputData","The net precipitation data can not be NULL.");
	if(this->m_tMin == NULL)		throw ModelException("SNO_SP","CheckInputData","The min temperature data can not be NULL.");
	if(this->m_tMax == NULL)		throw ModelException("SNO_SP","CheckInputData","The max temperature data can not be NULL.");
	if(this->m_SA == NULL)			throw ModelException("SNO_SP","CheckInputData","The snow accumulation data can not be NULL.");
	if(this->m_SR == NULL)			throw ModelException("SNO_SP","CheckInputData","The snow redistribution data can not be NULL.");
	if(this->m_SE == NULL)			throw ModelException("SNO_SP","CheckInputData","The snow sublimation data can not be NULL.");
	if(this->m_swe == -99)			throw ModelException("SNO_SP","CheckInputData","The swe can not be -99.");
	if(this->m_swe0 == -99)			throw ModelException("SNO_SP","CheckInputData","The swe0 can not be -99.");
	//if(this->m_csnow == -99)		throw ModelException("SNO_SP","CheckInputData","The temperature impact factor can not be -99.");
	//if(this->m_crain == -99)		throw ModelException("SNO_SP","CheckInputData","The rainfall impact factor can not be -99.");
	if(this->m_t0 == -99)			throw ModelException("SNO_SP","CheckInputData","The Snowmelt temperature can not be -99.");
	if(this->m_tsnow == -99)		throw ModelException("SNO_SP","CheckInputData","The snow fall temperature can not be -99.");
	if(this->m_lagSnow == -99)		throw ModelException("SNO_SP","CheckInputData","The lag snow can not be -99.");
	if(this->m_csnow6 == -99)		throw ModelException("SNO_SP","CheckInputData","The csnow6 can not be -99.");
	if(this->m_csnow12 == -99)		throw ModelException("SNO_SP","CheckInputData","The csnow12 can not be -99.");
	return true;
}

void SNO_SP::initalOutputs()
{
	if(m_cellSize <= 0)				throw ModelException("SNO_SP","CheckInputData","The dimension of the input data can not be less than zero.");
	if(m_SM == NULL) 
	{
		m_SM = new float[this->m_cellSize];
		m_packT = new float[this->m_cellSize];
		for (int rw = 0; rw < this->m_cellSize; rw++) 
		{
			m_SM[rw] = 0.0f;
			m_packT[rw] = 0.0f;
		}

	}
}

int SNO_SP::Execute()
{
	this->CheckInputData();

	this->initalOutputs();

	//the first time
	if(m_isInitial)
	{
		int count = 0;
		for(int i=0;i<this->m_cellSize;i++) 
		{
			if((this->m_tMin[i] + this->m_tMax[i]) / 2 < this->m_tsnow)	{this->m_SA[i] = this->m_swe0; count++;}	//winter
			else														this->m_SA[i] = 0.0f;						// other seasons

			m_packT[i] = (this->m_tMin[i] + this->m_tMax[i]) / 2.0f;	//initial pack T as average air temperature
		}

		m_swe =  this->m_swe0 * count / this->m_cellSize;
		m_isInitial = false;
	}
	else	//not the first time, update pack T using eq. 1:2.5.1 SWAT p57
	{
		for(int i=0;i<this->m_cellSize;i++) m_packT[i] = m_packT[i] * (1 - this->m_lagSnow) + (this->m_tMin[i] + this->m_tMax[i]) / 2.0f * this->m_lagSnow;
	}

	//begin
	if(this->m_lastSWE == -99.0f) this->m_lastSWE = this->m_swe; 
	if(m_swe < 0.01)	//all cells have not snow, so snowmelt is 0.
	{
		if(this->m_lastSWE >= 0.01) 
		{
			for (int rw = 0; rw < this->m_cellSize; rw++) 
				m_SM[rw] = 0.0f;
		}
		this->m_lastSWE = this->m_swe;

		return true;
	}

	float cmelt = -1.0f; //cmelt
	for (int rw = 0; rw < this->m_cellSize; rw++) 
	{
		float snow = this->m_SA[rw] + this->m_SR[rw] - this->m_SE[rw];
		float tmean = (this->m_tMin[rw] + this->m_tMax[rw]) / 2;
		if(tmean < this->m_tsnow) snow += (1+this->m_kblow)*this->m_Pnet[rw]; 
		if(snow < 0.01) this->m_SM[rw] = 0.0f;
		else
		{			
			float dt    = tmean - this->m_t0;
			if(dt<0) m_SM[rw] = 0.0f;  //if temperature is lower than t0, the snowmelt is 0.
			else
			{
				if(cmelt == -1.0f) cmelt = CMelt();	//just calculate once
				//calculate using eq. 1:2.5.2 SWAT p58
				float sm = cmelt * (m_packT[rw] / 2 + this->m_tMax[rw] / 2 - this->m_t0);
				this->m_SM[rw] = min(snow,sm);
			}
		}
	}

	this->m_lastSWE = this->m_swe;
	return 0;
}

//calculate cmelt using eq. 1:2.5.2 SWAT p58
float SNO_SP::CMelt()
{
	int d = JulianDay(this->m_Date);
	float sinv = float(sin(2*M_PI/365.0f*(d-81)));
	return (this->m_csnow6 + this->m_csnow12) / 2.0f + (this->m_csnow6 - this->m_csnow12) / 2.0f * sinv;
}

int SNO_SP::JulianDay(time_t date)
{
	struct tm dateInfo;
#ifndef linux
	localtime_s(&dateInfo,&date);
#else
	localtime_r(&date, &dateInfo);
#endif
	return dateInfo.tm_yday + 1;
}

bool SNO_SP::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException("SNO_SP","CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(this->m_cellSize != n)
	{
		if(this->m_cellSize <=0) this->m_cellSize = n;
		else
		{
			throw ModelException("SNO_SP","CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}

	return true;
}

void SNO_SP::SetValue(const char* key, float data)
{
	string s(key);
	if(StringMatch(s,"K_blow"))				this->m_kblow = data;
	//else if(StringMatch(s,"c_snow"))		this->m_csnow = data;
	//else if(StringMatch(s,"c_rain"))		this->m_crain = data;
	else if(StringMatch(s,"T0"))			this->m_t0 = data;
	else if(StringMatch(s,"T_snow"))		this->m_tsnow = data;
	else if(StringMatch(s,"SWE"))			this->m_swe = data;
	else if(StringMatch(s,"swe0"))			this->m_swe0 = data;
	else if(StringMatch(s,"lag_snow"))		this->m_lagSnow = data;
	else if(StringMatch(s,"c_snow6"))		this->m_csnow6 = data;
	else if(StringMatch(s,"c_snow12"))		this->m_csnow12 = data;
	else if (StringMatch(s, "ThreadNum"))
	{
		omp_set_num_threads((int)data);
	}
	else									throw ModelException("SNO_SP","SetValue","Parameter " + s 
		+ " does not exist in SNO_SP method. Please contact the module developer.");

}

void SNO_SP::Set1DData(const char* key, int n, float* data)
{
	//check the input data
	string s(key);

	this->CheckInputSize(key,n);

	if(StringMatch(s,"D_TMIN"))				this->m_tMin = data;
	else if(StringMatch(s,"D_TMAX"))		this->m_tMax = data;
	else if(StringMatch(s,"D_NEPR"))		this->m_Pnet = data;
	else if(StringMatch(s,"D_SNAC"))		this->m_SA = data;	
	else if(StringMatch(s,"D_SNRD"))		this->m_SR = data;
	else if(StringMatch(s,"D_SNSB"))		this->m_SE = data;
	else									throw ModelException("SNO_SP","SetValue","Parameter " + s + 
		" does not exist in SNO_SP method. Please contact the module developer.");

}

void SNO_SP::Get1DData(const char* key, int* n, float** data)
{
	string s(key);
	if(StringMatch(s,"SNME"))				
	{
		*data = this->m_SM;
	}
	else									throw ModelException("SNO_SP","getResult","Result " + s + " does not exist in SNO_SP method. Please contact the module developer.");

	*n = this->m_cellSize;
}

