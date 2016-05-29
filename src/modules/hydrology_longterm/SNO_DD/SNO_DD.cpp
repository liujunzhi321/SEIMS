#include "SNO_DD.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>

SNO_DD::SNO_DD(void)
{
	// set default values for member variables	
	this->m_nCells = -1;
	this->m_t0= NODATA;
	this->m_kblow= 0.0f;
	this->m_crain = NODATA;
	this->m_csnow = NODATA;
	this->m_swe = NODATA;
	this->m_lastSWE = NODATA;
	this->m_tsnow = NODATA;
	this->m_swe0 = NODATA;

	this->m_Pnet = NULL;
	this->m_tMean = NULL;
	this->m_SA = NULL;
	this->m_SR = NULL;
	this->m_SE = NULL;
	this->m_SM = NULL;
	///m_isInitial = true;
}

SNO_DD::~SNO_DD(void)
{
	//// cleanup
	if(this->m_SM != NULL) delete [] this->m_SM; 
	if (this->m_SA != NULL) 	delete[] this->m_SA;
}

bool SNO_DD::CheckInputData(void)
{
	if(this->m_date <=0)			throw ModelException(MID_SNO_DD,"CheckInputData","You have not set the time.");
	if(this->m_nCells <= 0)				throw ModelException(MID_SNO_DD,"CheckInputData","The dimension of the input data can not be less than zero.");
	if(this->m_Pnet == NULL)		throw ModelException(MID_SNO_DD,"CheckInputData","The net precipitation data can not be NULL.");
	if(this->m_tMean == NULL)		throw ModelException(MID_SNO_DD,"CheckInputData","The mean air temperature data can not be NULL.");
	if(this->m_swe0 == NODATA)			throw ModelException(MID_SNO_DD,"CheckInputData","The swe0 can not be -99.");
	if(this->m_csnow == NODATA)		throw ModelException(MID_SNO_DD,"CheckInputData","The temperature impact factor can not be -99.");
	if(this->m_crain == NODATA)		throw ModelException(MID_SNO_DD,"CheckInputData","The rainfall impact factor can not be -99.");
	if(this->m_t0 == NODATA)			throw ModelException(MID_SNO_DD,"CheckInputData","The Snowmelt temperature can not be -99.");
	if(this->m_tsnow == NODATA)		throw ModelException(MID_SNO_DD,"CheckInputData","The snow fall temperature can not be -99.");
	return true;
}

void SNO_DD::initalOutputs()
{
	if(m_nCells <= 0)				throw ModelException(MID_SNO_DD,"CheckInputData","The dimension of the input data can not be less than zero.");
	if(m_SM == NULL) 
	{
		m_SM = new float[this->m_nCells];
		m_SA = new float[this->m_nCells];
#pragma omp parallel for
		for(int i=0; i<m_nCells; i++) 
		{
			m_SM[i] = 0.f;
			m_SA[i] = 0.f;
			/// Since m_tMean is output of other modules, m_tMean is NULL when the first run invoked by the modules 
			/// ahead of SNO_DD. LJ
			//if(this->m_tMean[i]  < this->m_tsnow)	
			//	m_SA[i] = this->m_swe0; //winter
			//else
			//	this->m_SA[i] = 0.0f;		   // other seasons
		}
	}
}

int SNO_DD::Execute()
{
	this->CheckInputData();

	this->initalOutputs();

	//if(m_isInitial)
	//{
	//	int count = 0;

	//	#pragma omp parallel for
	//	for(int i=0; i<m_nCells; i++) 
	//	{
	//		if(this->m_tMean[i]  < this->m_tsnow)	
	//		{
	//			m_SA[i] = this->m_swe0; 
	//			count++;
	//		}	//winter
	//		else
	//			this->m_SA[i] = 0.0f;						// other seasons
	//	}
	//	m_isInitial = false;
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

		if(m_tMean[i] <= m_tsnow) 
		{
			m_SM[i] = 0.f;
			//m_SA[i] += (1 + m_kblow) * m_Pnet[i]; 
			m_SA[i] += m_Pnet[i]; 
		}
		else if (m_tMean[i] > m_tsnow && m_tMean[i] <= m_t0 && m_SA[i] > m_Pnet[i])
		{
			m_SM[i] = 0.f;
			//m_SA[i] += (1 + m_kblow) * m_Pnet[i]; 
			m_SA[i] += m_Pnet[i]; 
		}
		else
		{
			float dt    = m_tMean[i] - m_t0;
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
		throw ModelException(MID_SNO_DD,"CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(this->m_nCells != n)
	{
		if(this->m_nCells <=0) this->m_nCells = n;
		else
		{
			throw ModelException(MID_SNO_DD,"CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}
	return true;
}

void SNO_DD::SetValue(const char* key, float data)
{
	string s(key);
	if(StringMatch(s, VAR_K_BLOW))				this->m_kblow = data;
	else if(StringMatch(s, VAR_C_SNOW))		this->m_csnow = data;
	else if(StringMatch(s, VAR_C_RAIN))		this->m_crain = data;
	else if(StringMatch(s, VAR_T0))			this->m_t0 = data;
	else if(StringMatch(s,VAR_SNOW_TEMP))		this->m_tsnow = data;
	else if(StringMatch(s, VAR_SWE0))			this->m_swe0 = data;
	else if (StringMatch(s, VAR_OMP_THREADNUM)) omp_set_num_threads((int)data);
	else if (StringMatch(s, Tag_CellSize)) this->m_nCells = (int)data;
	else									
		throw ModelException(MID_SNO_DD,"SetValue","Parameter " + s 
		+ " does not exist in current module. Please contact the module developer.");
}

void SNO_DD::Set1DData(const char* key, int n, float* data)
{
	//check the input data
	string s(key);
	this->CheckInputSize(key,n);
	if(StringMatch(s,VAR_TMEAN))				this->m_tMean = data;
	else if(StringMatch(s, VAR_NEPR))		this->m_Pnet = data;
	else if(StringMatch(s, VAR_SNRD))		this->m_SR = data;
	else if(StringMatch(s, VAR_SNSB))		this->m_SE = data;
	else									
		throw ModelException(MID_SNO_DD,"Set1DData","Parameter " + s + 
		" does not exist in current module. Please contact the module developer.");
}

void SNO_DD::Get1DData(const char* key, int* n, float** data)
{
	initalOutputs();
	string s(key);
	if(StringMatch(s, VAR_SNME))		*data = this->m_SM;		
	else if (StringMatch(s, VAR_SNAC)) *data = this->m_SA;
	else
		throw ModelException(MID_SNO_DD,"Get1DData","Result " + s + " does not exist in current module. Please contact the module developer.");
	*n = this->m_nCells;
}

