//! Class for the Precipitation Interception module
#include "Interception.h"
#include "MetadataInfo.h"
#include "api.h"

#define _USE_MATH_DEFINES

#include <sstream>
#include <fstream>
#include <math.h>
#include <cmath>
#include <time.h>
#include "util.h"
#include "ModelException.h"
#include "omp.h"
#include <iostream>

//! Constructor
clsPI_MSM::clsPI_MSM(void):m_s0(NULL), m_P(NULL), m_maxInterception(NULL), m_minInterception(NULL),m_interceptionLast(NULL), m_interceptionLoss(NULL)
{
	m_date = -1;
	m_size = -1;
	m_dt = -1;
	this->m_Pi_b = 1.35f;
	//this->m_K_pet = -1.0f;
	this->m_dateLastTimeStep = -1;
	this->m_Init_IS = 0.0f;
}

//! Destructor
clsPI_MSM::~clsPI_MSM(void)
{
	if(this->m_interceptionLoss != NULL) delete [] this->m_interceptionLoss;
	if(this->m_interceptionLast != NULL) delete [] this->m_interceptionLast;
	if(this->m_netPrecipitation != NULL) delete [] this->m_netPrecipitation;
	//if(this->m_evaporation != NULL) delete [] this->m_evaporation;
}

void clsPI_MSM::SetDate(time_t date)
{
	this->m_dateLastTimeStep = this->m_date;
	this->m_date = date;
}

void clsPI_MSM::Set1DData(const char* key, int n, float* data)
{
	this->CheckInputSize(key, n);

	string s(key);
	if(StringMatch(s,"D_P"))				
	{
		this->m_P = data;

		//Output1DArray(m_size, m_P, "f:\\p.txt");
	}
	else if(StringMatch(key, "Slope"))
		m_s0 = data;
	//else if(StringMatch(s,"D_PET"))			this->m_PET = data;
	else if(StringMatch(s,"Interc_max"))	
	{
		this->m_maxInterception = data;
	}
	else if(StringMatch(s,"Interc_min"))	this->m_minInterception = data;	
	else									throw ModelException("PI_STORM","SetValue","Parameter " + s + " does not exist in PI_STORM method. Please contact the module developer.");

}

void clsPI_MSM::SetValue(const char* key, float data)
{
	string s(key);
	if(StringMatch(s,"Pi_b"))				
		this->m_Pi_b = data;
	else if (StringMatch(s, "DT_HS"))	
		m_dt = data;
	//else if(StringMatch(s,"K_pet"))			this->m_K_pet = data;
	else if(StringMatch(s,"Init_IS"))		
		this->m_Init_IS = data;
	else if (StringMatch(s, "ThreadNum"))
	{
		omp_set_num_threads((int)data);
	}
	else									throw ModelException("PI_STORM","SetValue","Parameter " + s + " does not exist in PI_STORM method. Please contact the module developer.");

	//this->StatusMessage("Set "+ s + "=" + clsPI_MSM::toString(data));
}

void clsPI_MSM::Get1DData(const char* key, int* n, float** data)
{
	string s(key);
	if(StringMatch(s,"INLO"))				*data = this->m_interceptionLoss;
	//else if(StringMatch(s,"INET"))			*data = this->m_evaporation;
	else if(StringMatch(s,"NEPR"))			*data = this->m_netPrecipitation;
	else									throw ModelException("PI_STORM","SetValue","Result " + s + " does not exist in PI_STORM method. Please contact the module developer.");

	*n = this->m_size;
}

//! 
int clsPI_MSM::Execute()
{
	#pragma omp parallel for
	for (int i = 0; i < m_size; ++i)
	{
		//m_P[i] = m_P[i] * m_dt / 3600;
		if (m_P[i] > 0.f)
			m_P[i] = m_P[i] * m_dt / 3600 * cos(atan(m_s0[i]));
		else
			m_P[i] = 0.f;
	}

	//initial the state variable
	if(this->m_interceptionLast == NULL)
	{
		CheckInputData();
		
		m_interceptionLast = new float[m_size];
		//this->m_evaporation = new float[m_size];
		this->m_interceptionLoss = new float[m_size];
		this->m_netPrecipitation = new float[m_size];
		#pragma omp parallel for
		for(int i = 0 ;i<this->m_size;i++) 
		{
			m_interceptionLast[i] = this->m_Init_IS;
		}
	}
	int yday = DayOfYear(this->m_date);
#pragma omp parallel for
	for(int i = 0 ; i < this->m_size; i ++)
	{
		//float PE = this->m_PET[i] * this->m_K_pet;
		
		//evaporation
		//if(this->m_interceptionLast[i] <= 0)			this->m_evaporation[i] = 0.0f;
		//else if(this->m_interceptionLast[i] > PE)	this->m_evaporation[i] = PE;
		//else														this->m_evaporation[i] = this->m_interceptionLast[i];

		//interception storage capacity
		//int julian = 100;
		double degree = 2 * M_PI * (yday - 87) /365;
		float min = this->m_minInterception[i];
		float max = this->m_maxInterception[i];
		double capacity = min + (max - min)*pow(0.5 + 0.5*sin(degree),double(this->m_Pi_b));

		//interception
		//double availableSpace = capacity - this->m_interceptionLast[i] + this->m_evaporation[i];
		double availableSpace = capacity - this->m_interceptionLast[i];
		if(availableSpace < this->m_P[i]) 
			this->m_interceptionLoss[i] = float(availableSpace);
		else		
			this->m_interceptionLoss[i] = this->m_P[i];

		//net precipitation
		this->m_netPrecipitation[i] = m_P[i] - m_interceptionLoss[i];

		//override the interception storage of last time step
		//this->m_interceptionLast[i] += this->m_interceptionLoss[i] - this->m_evaporation[i];
		m_interceptionLast[i] += m_interceptionLoss[i];
	}


	return 0;
}

int clsPI_MSM::DayOfYear(time_t date)
{
	struct tm dateInfo;
#ifndef linux
	localtime_s(&dateInfo,&date);
#else
    localtime_r(&date, &dateInfo);
#endif
	return dateInfo.tm_yday + 1;
}

bool clsPI_MSM::CheckInputData()
{
	if(this->m_date == -1)
	{
		throw ModelException("PI_STORM","CheckInputData","You have not set the time.");
		return false;
	}

	if(m_size <= 0)
	{
		throw ModelException("PI_STORM","CheckInputData","The dimension of the input data can not be less than zero.");
		return false;
	}

	if(this->m_P == NULL)
	{
		throw ModelException("PI_STORM","CheckInputData","The precipitation data can not be NULL.");
		return false;
	}

	//if(this->m_PET == NULL)
	//{
	//	throw ModelException("PI_STORM","CheckInputData","The PET data can not be NULL.");
	//	return false;
	//}

	if(this->m_maxInterception == NULL)
	{
		throw ModelException("PI_STORM","CheckInputData","The maximum interception storage capacity can not be NULL.");
		return false;
	}

	if(this->m_minInterception == NULL)
	{
		throw ModelException("PI_STORM","CheckInputData","The minimum interception storage capacity can not be NULL.");
		return false;
	}

	//if(this->m_K_pet > 1.3 || this->m_K_pet < 0.7)
	//{
	//	throw ModelException("PI_STORM","CheckInputData","The correction factor of PET can not be " + clsPI_MSM::toString(this->m_K_pet) + ". It should between 0.7 and 1.3.");
	//	return false;
	//}

	if(this->m_Pi_b > 1.5 || this->m_Pi_b < 0.5)
	{
		throw ModelException("PI_STORM","CheckInputData","The interception storage capacity exponent can not be " + clsPI_MSM::toString(this->m_Pi_b) + ". It should between 0.5 and 1.5.");
		return false;
	}

	if(this->m_Init_IS > 1 || this->m_Init_IS < 0)
	{
		throw ModelException("PI_STORM","CheckInputData","The Initial interception storage can not be " + clsPI_MSM::toString(this->m_Init_IS) + ". It should between 0 and 1.");
		return false;
	}

	return true;
}

string clsPI_MSM::toString(float value)
{
	char s[20];
	strprintf(s,20,"%f",value);
	return string(s);
}

bool clsPI_MSM::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException("PI_STORM","CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(this->m_size != n)
	{
		if(this->m_size <=0) this->m_size = n;
		else
		{
			throw ModelException("PI_STORM","CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			ostringstream oss;
			oss << "Input data for "+string(key) << " is invalid with size: " << n << ". The origin size is " << m_size << ".\n";  
			throw ModelException("PI_STORM","CheckInputSize",oss.str());
		}
	}

	return true;
}



