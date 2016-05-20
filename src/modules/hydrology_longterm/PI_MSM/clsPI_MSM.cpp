//! Class for the Precipitation Interception module
#include "clsPI_MSM.h"
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
#include <omp.h>

//! Constructor
clsPI_MSM::clsPI_MSM(void):m_st(NULL)
{
	m_cellSize = -1;
	this->m_Pi_b = -1.0f;
	this->m_dateLastTimeStep = -1;
	this->m_Init_IS = 0.0f;
}

//! Destructor
clsPI_MSM::~clsPI_MSM(void)
{
	if(this->m_interceptionLoss != NULL) delete [] this->m_interceptionLoss;
	if(this->m_st != NULL) delete [] this->m_st;
	if(this->m_netPrecipitation != NULL) delete [] this->m_netPrecipitation;
	if(this->m_evaporation != NULL) delete [] this->m_evaporation;
}

void clsPI_MSM::Set1DData(const char* key, int nRows, float* data)
{
	this->CheckInputSize(key,nRows);

	string s(key);
	if(StringMatch(s, VAR_PRECI) || StringMatch(s,"P"))				
		m_P = data;
	else if(StringMatch(s,VAR_PET) || StringMatch(s,"PET"))			
		m_PET = data;
	else if(StringMatch(s, VAR_INTERC_MAX))	
		m_maxSt = data;
	else if(StringMatch(s,VAR_INTERC_MIN))	
		m_minSt = data;	
	else									
		throw ModelException("PI_MSM","SetValue","Parameter " + s + " does not exist in PI_MSM method. Please contact the module developer.");

}

void clsPI_MSM::SetValue(const char* key, float data)
{
	string s(key);
	if(StringMatch(s,VAR_PI_B))				this->m_Pi_b = data;
	else if(StringMatch(s, VAR_INIT_IS))		this->m_Init_IS = data;
	else if (StringMatch(s, VAR_OMP_THREADNUM))
	{
		omp_set_num_threads((int)data);
	}
	else									throw ModelException("PI_MSM","SetValue","Parameter " + s + " does not exist in PI_MSM method. Please contact the module developer.");
}

void clsPI_MSM::Get1DData(const char* key, int* nRows, float** data)
{
	string s(key);
	if(StringMatch(s, VAR_INLO))				
		*data = m_interceptionLoss;
	else if(StringMatch(s, VAR_INET))			
		*data = m_evaporation;
	else if(StringMatch(s, VAR_NEPR))			
		*data = m_netPrecipitation;
	else									
		throw ModelException("PI_MSM","SetValue","Result " + s + " does not exist in PI_MSM method. Please contact the module developer.");

	*nRows = this->m_cellSize;
}

//! 
int clsPI_MSM::Execute()
{
	//check input data
	this->CheckInputData();

	//initial the state variable
	if(this->m_st == NULL)
	{
		m_st = new float[this->m_cellSize];
		this->m_evaporation = new float[this->m_cellSize];
		this->m_interceptionLoss = new float[this->m_cellSize];
		this->m_netPrecipitation = new float[this->m_cellSize];
		
		#pragma omp parallel for
		for(int i = 0 ;i<this->m_cellSize;i++) 
		{
			m_st[i] = this->m_Init_IS;
			m_evaporation[i] = 0.f;
			m_interceptionLoss[i] = 0.f;
			m_netPrecipitation[i] = 0.f;
		}
	}

	int julian = JulianDay(m_date);
	#pragma omp parallel for
	for(int i = 0 ; i < this->m_cellSize; i ++)
	{
		if (m_P[i] > 0)
		{
			//interception storage capacity
			float degree = 2 * PI * (julian - 87)/365;
			float min = m_minSt[i];
			float max = m_maxSt[i];
			float capacity = min + (max - min)*pow(0.5 + 0.5*sin(degree),double(m_Pi_b));

			//interception
			float availableSpace = capacity - m_st[i];
			if (availableSpace < 0)
				availableSpace = 0.f;

			if(availableSpace < m_P[i]) 
				m_interceptionLoss[i] = availableSpace;
			else
				m_interceptionLoss[i] = m_P[i];

			//net precipitation
			m_netPrecipitation[i] = m_P[i] - m_interceptionLoss[i];
			m_st[i] += m_interceptionLoss[i];
		}
		else
		{
			m_interceptionLoss[i] = 0.f;
			m_netPrecipitation[i] = 0.f;
		}

		//evaporation
		if(m_st[i] > m_PET[i])	
			m_evaporation[i] = m_PET[i];
		else														
			m_evaporation[i] = m_st[i];

		m_st[i] -= m_evaporation[i];
	}

	return 0;
}

int clsPI_MSM::JulianDay(time_t date)
{
	struct tm dateInfo;
	LocalTime(date,&dateInfo);
	return dateInfo.tm_yday + 1;
}

bool clsPI_MSM::CheckInputData()
{
	if(this->m_date == -1)
	{
		throw ModelException("PI_MSM","CheckInputData","You have not set the time.");
		return false;
	}

	if(m_cellSize <= 0)
	{
		throw ModelException("PI_MSM","CheckInputData","The dimension of the input data can not be less than zero.");
		return false;
	}

	if(this->m_P == NULL)
	{
		throw ModelException("PI_MSM","CheckInputData","The precipitation data can not be NULL.");
		return false;
	}

	if(this->m_PET == NULL)
	{
		throw ModelException("PI_MSM","CheckInputData","The PET data can not be NULL.");
		return false;
	}

	if(this->m_maxSt == NULL)
	{
		throw ModelException("PI_MSM","CheckInputData","The maximum interception storage capacity can not be NULL.");
		return false;
	}

	if(this->m_minSt == NULL)
	{
		throw ModelException("PI_MSM","CheckInputData","The minimum interception storage capacity can not be NULL.");
		return false;
	}

	if(this->m_Pi_b > 1.5 || this->m_Pi_b < 0.5)
	{
		throw ModelException("PI_MSM","CheckInputData","The interception storage capacity exponent can not be " + clsPI_MSM::toString(this->m_Pi_b) + ". It should between 0.5 and 1.5.");
		return false;
	}

	if(this->m_Init_IS > 1 || this->m_Init_IS < 0)
	{
		throw ModelException("PI_MSM","CheckInputData","The Initial interception storage can not be " + clsPI_MSM::toString(this->m_Init_IS) + ". It should between 0 and 1.");
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
		throw ModelException("PI_MSM","CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(this->m_cellSize != n)
	{
		if(this->m_cellSize <=0) this->m_cellSize = n;
		else
		{
			throw ModelException("PI_MSM","CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}

	return true;
}



