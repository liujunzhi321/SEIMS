#include "SSM_PE.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"

#include <omp.h>

SSM_PE::SSM_PE(void)
{
	// set default values for member variables	
	this->m_cellSize = -1;

	this->m_t0= -99.0f;
	this->m_ksubli=-99.0f;
	this->m_kblow=-99.0f;
	this->m_swe = -99.0f;
	this->m_swe0 = -99.0f;
	this->m_lastSWE = -99.0f;
	this->m_tsnow = -99.0f;

	this->m_PET = NULL;
	this->m_Pnet = NULL;
	this->m_SA = NULL;
	this->m_SR = NULL;
	this->m_SE = NULL;

	this->m_tMax = NULL;
	this->m_tMin = NULL;

	m_isInitial = true;
}

SSM_PE::~SSM_PE(void)
{
	//// cleanup
	if(this->m_SE != NULL) delete [] this->m_SE;	
}

bool SSM_PE::CheckInputData(void)
{
	if(m_cellSize <= 0)				throw ModelException("SSM_PE","CheckInputData","The dimension of the input data can not be less than zero.");
	if(this->m_kblow == -99)		throw ModelException("SSM_PE","CheckInputData","The fraction coefficient of snow blowing into or out of the watershed can not be NULL.");
	if(this->m_Pnet == NULL)		throw ModelException("SSM_PE","CheckInputData","The net precipitation data can not be NULL.");
	if(this->m_PET == NULL)			throw ModelException("SSM_PE","CheckInputData","The PET data can not be NULL.");
	if(this->m_SA == NULL)			throw ModelException("SSM_PE","CheckInputData","The snow accumulation data can not be NULL.");
	if(this->m_SR == NULL)			throw ModelException("SSM_PE","CheckInputData","The snow redistribution data can not be NULL.");
	if(this->m_swe == -99)			throw ModelException("SSM_PE","CheckInputData","The swe can not be -99.");
	if(this->m_swe0 == -99)			throw ModelException("SSM_PE","CheckInputData","The swe0 can not be -99.");
	if(this->m_ksubli == -99)		throw ModelException("SSM_PE","CheckInputData","The k_subli can not be -99.");
	if(this->m_tMin == NULL)		throw ModelException("SSM_PE","CheckInputData","The min temperature data can not be NULL.");
	if(this->m_tMax == NULL)		throw ModelException("SSM_PE","CheckInputData","The max temperature data can not be NULL.");
	if(this->m_t0 == -99)			throw ModelException("SSM_PE","CheckInputData","The Snowmelt temperature can not be -99.");
	if(this->m_tsnow == -99)		throw ModelException("SSM_PE","CheckInputData","The snow fall temperature can not be -99.");
	return true;
}

void SSM_PE::initalOutputs()
{
	if(m_cellSize <= 0)				throw ModelException("SSM_PE","CheckInputData","The dimension of the input data can not be less than zero.");
	if(m_SE == NULL)
	{
		m_SE = new float[this->m_cellSize];
		for (int rw = 0; rw < this->m_cellSize; rw++) 
		{
			m_SE[rw] = 0.0f;
		}							
	}
}

int SSM_PE::Execute()
{
	this->CheckInputData();

	this->initalOutputs();

	if(m_isInitial)
	{
		int count = 0;
		for(int i=0;i<this->m_cellSize;i++) 
		{
			if((this->m_tMin[i] + this->m_tMax[i]) / 2 < this->m_tsnow)	{this->m_SA[i] = this->m_swe0; count++;}	//winter
			else														this->m_SA[i] = 0.0f;						// other seasons
		}

		m_swe =  this->m_swe0 * count / this->m_cellSize;
		m_isInitial = false;
	}

	if(this->m_lastSWE == -99.0f) this->m_lastSWE = this->m_swe;
	if(m_swe < 0.01)	//all cells have not snow, so snow sublimation is 0.
	{
		if(this->m_lastSWE >= 0.01)
		{
			for (int rw = 0; rw < this->m_cellSize; rw++) 
				m_SE[rw] = 0.0f;
		}

		this->m_lastSWE = this->m_swe;
		return true;
	}

	for (int rw = 0; rw < this->m_cellSize; rw++) 
	{
		float snow = this->m_SA[rw] + this->m_SR[rw];
		float tmean = (this->m_tMin[rw] + this->m_tMax[rw]) / 2;
		if(tmean < this->m_tsnow) snow+=(1+this->m_kblow)*this->m_Pnet[rw];
		if(snow < 0.01) this->m_SE[rw] = 0.0f;
		else
		{			
			if(tmean >= this->m_t0) this->m_SE[rw] = 0.0f;   //if temperature is higher than t0, the sublimation is 0.
			else
			{
				float se = this->m_ksubli * this->m_PET[rw];
				this->m_SE[rw] = min(snow,se);
			}
		}
	}

	this->m_lastSWE = this->m_swe;
	return true;
	return 0;
}

bool SSM_PE::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException("SSM_PE","CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(this->m_cellSize != n)
	{
		if(this->m_cellSize <=0) this->m_cellSize = n;
		else
		{
			throw ModelException("SSM_PE","CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}

	return true;
}

void SSM_PE::SetValue(const char* key, float data)
{
	string s(key);
	if (StringMatch(s, "ThreadNum"))
	{
		omp_set_num_threads((int)data);
	}
	else if(StringMatch(s,"K_blow"))				this->m_kblow = data;
	else if(StringMatch(s,"K_subli"))		this->m_ksubli = data;
	else if(StringMatch(s,"SWE"))			this->m_swe = data;
	else if(StringMatch(s,"swe0"))			this->m_swe0 = data;
	else if(StringMatch(s,"T0"))			this->m_t0 = data;
	else if(StringMatch(s,"T_snow"))		this->m_tsnow = data;
	else									throw ModelException("SSM_PE","SetValue","Parameter " + s 
		+ " does not exist in SSM_PE method. Please contact the module developer.");
	
}

void SSM_PE::Set1DData(const char* key, int n, float* data)
{
	//check the input data
	string s(key);

	this->CheckInputSize(key,n);

	if(StringMatch(s,"D_PET"))				this->m_PET = data;
	else if(StringMatch(s,"D_NEPR"))		this->m_Pnet = data;
	else if(StringMatch(s,"D_SNAC"))		this->m_SA = data;	
	else if(StringMatch(s,"D_SNRD"))		this->m_SR = data;
	else if(StringMatch(s,"D_TMIN"))		this->m_tMin = data;
	else if(StringMatch(s,"D_TMAX"))		this->m_tMax = data;
	else									throw ModelException("SSM_PE","SetValue","Parameter " + s + 
		" does not exist in SSM_PE method. Please contact the module developer.");

}

void SSM_PE::Get1DData(const char* key, int* n, float** data)
{
	string s(key);
	if(StringMatch(s,"SNSB"))				
	{
		*data = this->m_SE;
	}
	else									throw ModelException("SSM_PE","getResult","Result " + s + " does not exist in SSM_PE method. Please contact the module developer.");

	*n = this->m_cellSize;
}

