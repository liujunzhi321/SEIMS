#include "SNO_DD.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>

SNO_DD::SNO_DD(void)
{
	// set default values for member variables	
	this->m_nCells = -1;
	this->m_t0= -99.0f;
	this->m_kblow= 0.0f;
	this->m_crain = -99.0f;
	this->m_csnow = -99.0f;
	this->m_swe = -99.0f;
	this->m_lastSWE = -99.0f;
	this->m_tsnow = -99.0f;
	this->m_swe0 = -99.0f;

	this->m_Pnet = NULL;
	this->m_tMax = NULL;
	this->m_tMin = NULL;
	this->m_SA = NULL;
	this->m_SR = NULL;
	this->m_SE = NULL;

	this->m_SM = NULL;
	m_isInitial = true;
}

SNO_DD::~SNO_DD(void)
{
	//// cleanup
	if(this->m_SM != NULL) delete [] this->m_SM; 
	if (m_SA != NULL)
		delete[] m_SA;
}

bool SNO_DD::CheckInputData(void)
{
	if(this->m_date <=0)			throw ModelException("SNO_DD","CheckInputData","You have not set the time.");
	if(m_nCells <= 0)				throw ModelException("SNO_DD","CheckInputData","The dimension of the input data can not be less than zero.");
	//if(this->m_kblow == -99)		throw ModelException("SNO_DD","CheckInputData","The fraction coefficient of snow blowing into or out of the watershed can not be NULL.");
	if(this->m_Pnet == NULL)		throw ModelException("SNO_DD","CheckInputData","The net precipitation data can not be NULL.");
	if(this->m_tMin == NULL)		throw ModelException("SNO_DD","CheckInputData","The min temperature data can not be NULL.");
	if(this->m_tMax == NULL)		throw ModelException("SNO_DD","CheckInputData","The max temperature data can not be NULL.");
	//if(this->m_SA == NULL)			throw ModelException("SNO_DD","CheckInputData","The snow accumulation data can not be NULL.");
	//if(this->m_SR == NULL)			throw ModelException("SNO_DD","CheckInputData","The snow redistribution data can not be NULL.");
	//if(this->m_SE == NULL)			throw ModelException("SNO_DD","CheckInputData","The snow sublimation data can not be NULL.");
	//if(this->m_swe == -99)			throw ModelException("SNO_DD","CheckInputData","The swe can not be -99.");
	if(this->m_swe0 == -99)			throw ModelException("SNO_DD","CheckInputData","The swe0 can not be -99.");
	if(this->m_csnow == -99)		throw ModelException("SNO_DD","CheckInputData","The temperature impact factor can not be -99.");
	if(this->m_crain == -99)		throw ModelException("SNO_DD","CheckInputData","The rainfall impact factor can not be -99.");
	if(this->m_t0 == -99)			throw ModelException("SNO_DD","CheckInputData","The Snowmelt temperature can not be -99.");
	if(this->m_tsnow == -99)		throw ModelException("SNO_DD","CheckInputData","The snow fall temperature can not be -99.");

	return true;
}

void SNO_DD::initalOutputs()
{
	if(m_nCells <= 0)				throw ModelException("SNO_DD","CheckInputData","The dimension of the input data can not be less than zero.");
	if(m_SM == NULL) 
	{
		m_SM = new float[this->m_nCells];
		m_SA = new float[this->m_nCells];
		for (int i = 0; i < this->m_nCells; i++) 
		{
			m_SM[i] = 0.0f;
			m_SA[i] = 0.f;
		}

	}
}

int SNO_DD::Execute()
{
	this->CheckInputData();

	this->initalOutputs();

	if(m_isInitial)
	{
		int count = 0;

		#pragma omp parallel for
		for(int i=0; i<m_nCells; i++) 
		{
			if((this->m_tMin[i] + this->m_tMax[i]) / 2 < this->m_tsnow)	
			{
				m_SA[i] = this->m_swe0; 
				count++;
			}	//winter
			else
				this->m_SA[i] = 0.0f;						// other seasons
		}
		//m_swe =  this->m_swe0 * count / this->m_nCells;
		m_isInitial = false;
	}

	//if(m_lastSWE == -99.0f) 
	//	m_lastSWE = m_swe; 
	//if(m_swe < 0.01)	//all cells have not snow, so snowmelt is 0.
	//{
	//	if(this->m_lastSWE >= 0.01) 
	//	{
	//		for (int i = 0; i < this->m_nCells; i++) 
	//			m_SM[i] = 0.0f;
	//	}
	//	this->m_lastSWE = this->m_swe;

	//	return true;
	//}

	#pragma omp parallel for
	for (int i = 0; i < m_nCells; i++) 
	{
		float se = 0.f;
		float sr = 0.f;
		if (m_SE != NULL)
			se = m_SE[i];
		if (m_SR != NULL)
			sr = m_SR[i];

		m_SA[i] = m_SA[i] + sr - se;
		
		if (FloatEqual(m_SA[i], 0.f) && FloatEqual(m_Pnet[i], 0.f))
		{
			m_SM[i] = 0.f;
			continue;
		}
		
		float tmean = (this->m_tMin[i] + this->m_tMax[i]) / 2;

		if(tmean <= m_tsnow) 
		{
			m_SM[i] = 0.f;
			//m_SA[i] += (1 + m_kblow) * m_Pnet[i]; 
			m_SA[i] += m_Pnet[i]; 
		}
		else if (tmean > m_tsnow && tmean <= m_t0 && m_SA[i] > m_Pnet[i])
		{
			m_SM[i] = 0.f;
			//m_SA[i] += (1 + m_kblow) * m_Pnet[i]; 
			m_SA[i] += m_Pnet[i]; 
		}
		else
		{
			float dt    = tmean - m_t0;
			if(dt < 0) 
				m_SM[i] = 0.0f;  //if temperature is lower than t0, the snowmelt is 0.
			else
			{
				float sm = m_csnow*dt + m_crain*m_Pnet[i]*dt;
				m_SM[i] = min(m_SA[i], sm);
				m_SA[i] -= m_SM[i];
			}
		}
	}

	//this->m_lastSWE = this->m_swe;
	return 0;
}

bool SNO_DD::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException("SNO_DD","CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(this->m_nCells != n)
	{
		if(this->m_nCells <=0) this->m_nCells = n;
		else
		{
			throw ModelException("SNO_DD","CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}

	return true;
}

void SNO_DD::SetValue(const char* key, float data)
{
	string s(key);
	if(StringMatch(s,"K_blow"))				this->m_kblow = data;
	else if(StringMatch(s,"c_snow"))		this->m_csnow = data;
	else if(StringMatch(s,"c_rain"))		this->m_crain = data;
	else if(StringMatch(s,"T0"))			this->m_t0 = data;
	else if(StringMatch(s,"T_snow"))		this->m_tsnow = data;
	else if(StringMatch(s,"swe0"))			this->m_swe0 = data;
	else if (StringMatch(s, VAR_OMP_THREADNUM))
	{
		omp_set_num_threads((int)data);
	}
	else									throw ModelException("SNO_DD","SetValue","Parameter " + s 
		+ " does not exist in SNO_DD method. Please contact the module developer.");
	
}

void SNO_DD::Set1DData(const char* key, int n, float* data)
{
	//check the input data
	string s(key);

	this->CheckInputSize(key,n);

	if(StringMatch(s,"D_TMIN"))				this->m_tMin = data;
	else if(StringMatch(s,"D_TMAX"))		this->m_tMax = data;
	else if(StringMatch(s,"D_NEPR"))		this->m_Pnet = data;
	//else if(StringMatch(s,"D_SNAC"))		this->m_SA = data;	
	else if(StringMatch(s,"D_SNRD"))		this->m_SR = data;
	else if(StringMatch(s,"D_SNSB"))		this->m_SE = data;
	else									throw ModelException("SNO_DD","SetValue","Parameter " + s + 
		" does not exist in SNO_DD method. Please contact the module developer.");

}

void SNO_DD::Get1DData(const char* key, int* n, float** data)
{
	string s(key);
	if(StringMatch(s,"SNME"))				
	{
		*data = this->m_SM;
	}
	else if (StringMatch(s, "SNAC"))
	{
		*data = m_SA;
	}
	else									throw ModelException("SNO_DD","Get1DData","Result " + s + " does not exist in SNO_DD method. Please contact the module developer.");

	*n = this->m_nCells;
}

