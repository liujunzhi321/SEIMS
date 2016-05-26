#include "SNO_SP.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include <math.h>
#include <cmath>
#include "util.h"
#include "ClimateParams.h"
#include <omp.h>

SNO_SP::SNO_SP(void)
{
	// set default values for member variables	
	this->m_nCells = -1;
	this->m_t0= NODATA;
	this->m_kblow= NODATA;
	//this->m_crain = NODATA;
	//this->m_csnow = NODATA;
	this->m_swe = NODATA;
	this->m_lastSWE = NODATA;
	this->m_tsnow = NODATA;
	this->m_swe0 = NODATA;
	this->m_lagSnow = NODATA;
	this->m_csnow6 = NODATA;
	this->m_csnow12 = NODATA;

	this->m_Pnet = NULL;
	this->m_tMean = NULL;
	this->m_tMax = NULL;
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
	if(this->m_date <=0)			throw ModelException(MID_SNO_SP,"CheckInputData","You have not set the time.");
	if(this->m_nCells <= 0)				throw ModelException(MID_SNO_SP,"CheckInputData","The dimension of the input data can not be less than zero.");
	if(this->m_kblow == NODATA)		throw ModelException(MID_SNO_SP,"CheckInputData","The fraction coefficient of snow blowing into or out of the watershed can not be NULL.");
	if(this->m_Pnet == NULL)		throw ModelException(MID_SNO_SP,"CheckInputData","The net precipitation data can not be NULL.");
	if(this->m_tMean == NULL)		throw ModelException(MID_SNO_SP,"CheckInputData","The mean temperature data can not be NULL.");
	if(this->m_tMax == NULL)		throw ModelException(MID_SNO_SP,"CheckInputData","The max temperature data can not be NULL.");
	/// m_SA should be the output! By LJ
	//if(this->m_SA == NULL)			throw ModelException(MID_SNO_SP,"CheckInputData","The snow accumulation data can not be NULL.");
	if(this->m_swe == NODATA)			throw ModelException(MID_SNO_SP,"CheckInputData","The swe can not be NODATA.");
	if(this->m_swe0 == NODATA)			throw ModelException(MID_SNO_SP,"CheckInputData","The swe0 can not be NODATA.");
	//if(this->m_csnow == NODATA)		throw ModelException(MID_SNO_SP,"CheckInputData","The temperature impact factor can not be NODATA.");
	//if(this->m_crain == NODATA)		throw ModelException(MID_SNO_SP,"CheckInputData","The rainfall impact factor can not be NODATA.");
	if(this->m_t0 == NODATA)			throw ModelException(MID_SNO_SP,"CheckInputData","The Snowmelt temperature can not be NODATA.");
	if(this->m_tsnow == NODATA)		throw ModelException(MID_SNO_SP,"CheckInputData","The snow fall temperature can not be NODATA.");
	if(this->m_lagSnow == NODATA)		throw ModelException(MID_SNO_SP,"CheckInputData","The lag snow can not be NODATA.");
	if(this->m_csnow6 == NODATA)		throw ModelException(MID_SNO_SP,"CheckInputData","The csnow6 can not be NODATA.");
	if(this->m_csnow12 == NODATA)		throw ModelException(MID_SNO_SP,"CheckInputData","The csnow12 can not be NODATA.");

	//if(this->m_SR == NULL)			throw ModelException(MID_SNO_SP,"CheckInputData","The snow redistribution data can not be NULL.");
	//if(this->m_SE == NULL)			throw ModelException(MID_SNO_SP,"CheckInputData","The snow sublimation data can not be NULL.");
	if(this->m_SR == NULL)
	{
		this->m_SR = new float[m_nCells];
		for(int i = 0; i < m_nCells; i++)
			this->m_SR[i] = 0.f;
	}
	if(this->m_SE == NULL)
	{
		this->m_SE = new float[m_nCells];
		for(int i = 0; i < m_nCells; i++)
			this->m_SE[i] = 0.f;
	}
	return true;
}

void SNO_SP::initalOutputs()
{
	if(m_nCells <= 0)				throw ModelException(MID_SNO_SP,"CheckInputData","The dimension of the input data can not be less than zero.");
	if(m_SM == NULL) 
	{
		m_SM = new float[this->m_nCells];
		m_packT = new float[this->m_nCells];
		m_SA = new float[this->m_nCells];   /// m_SA should be output
		for (int rw = 0; rw < this->m_nCells; rw++) 
		{
			m_SM[rw] = 0.0f;
			m_packT[rw] = 0.0f;
			m_SA[rw] = 0.0f;
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
		for(int i=0;i<this->m_nCells;i++) 
		{
			if(this->m_tMean[i] < this->m_tsnow)	{this->m_SA[i] = this->m_swe0; count++;}	//winter
			else		
				this->m_SA[i] = 0.0f;						// other seasons
			m_packT[i] = this->m_tMean[i];	//initial pack T as average air temperature
		}

		m_swe =  this->m_swe0 * count / this->m_nCells;
		m_isInitial = false;
	}
	else	//not the first time, update pack T using eq. 1:2.5.1 SWAT p57
	{
		for(int i=0;i<this->m_nCells;i++) m_packT[i] = m_packT[i] * (1 - this->m_lagSnow) + this->m_tMean[i] * this->m_lagSnow;
	}

	//begin
	if(this->m_lastSWE == NODATA) this->m_lastSWE = this->m_swe; 
	if(m_swe < 0.01)	//all cells have not snow, so snowmelt is 0.
	{
		if(this->m_lastSWE >= 0.01) 
		{
			for (int rw = 0; rw < this->m_nCells; rw++) 
				m_SM[rw] = 0.0f;
		}
		this->m_lastSWE = this->m_swe;

		return true;
	}

	float cmelt = -1.0f; //cmelt
	for (int rw = 0; rw < this->m_nCells; rw++) 
	{
		float snow = this->m_SA[rw] + this->m_SR[rw] - this->m_SE[rw];
		float tmean = this->m_tMean[rw];
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
	int d = JulianDay(this->m_date);
	float sinv = float(sin(2*PI/365.0f*(d-81)));
	return (this->m_csnow6 + this->m_csnow12) / 2.0f + (this->m_csnow6 - this->m_csnow12) / 2.0f * sinv;
}

bool SNO_SP::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException(MID_SNO_SP,"CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(this->m_nCells != n)
	{
		if(this->m_nCells <=0) this->m_nCells = n;
		else
		{
			throw ModelException(MID_SNO_SP,"CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}

	return true;
}
void SNO_SP::SetValue(const char* key, float data)
{
	string s(key);
	if(StringMatch(s, VAR_K_BLOW))				this->m_kblow = data;
	else if(StringMatch(s, VAR_T0))			this->m_t0 = data;
	else if(StringMatch(s, VAR_SNOW_TEMP))		this->m_tsnow = data;
	else if(StringMatch(s, VAR_SWE))			this->m_swe = data;
	else if(StringMatch(s, VAR_SWE0))			this->m_swe0 = data;
	else if(StringMatch(s, VAR_LAG_SNOW))		this->m_lagSnow = data;
	else if(StringMatch(s, VAR_C_SNOW6))		this->m_csnow6 = data;
	else if(StringMatch(s, VAR_C_SNOW12))		this->m_csnow12 = data;
	else if (StringMatch(s, VAR_OMP_THREADNUM)) omp_set_num_threads((int)data);
	else									throw ModelException(MID_SNO_SP,"SetValue","Parameter " + s 
		+ " does not exist in current module. Please contact the module developer.");
}

void SNO_SP::Set1DData(const char* key, int n, float* data)
{
	//check the input data
	string s(key);

	this->CheckInputSize(key,n);
	if(StringMatch(s,VAR_TMEAN))				this->m_tMean = data;
	else if(StringMatch(s, VAR_TMAX))		this->m_tMax = data;
	else if(StringMatch(s, VAR_NEPR))		this->m_Pnet = data;
	/// else if(StringMatch(s, VAR_SNAC))		this->m_SA = data;	 m_SA should be a output. By LJ
	else if(StringMatch(s, VAR_SNRD))		this->m_SR = data;
	else if(StringMatch(s, VAR_SNSB))		this->m_SE = data;
	else									throw ModelException(MID_SNO_SP,"Set1DData","Parameter " + s + 
		" does not exist in current module. Please contact the module developer.");

}

void SNO_SP::Get1DData(const char* key, int* n, float** data)
{
	string s(key);
	if(StringMatch(s, VAR_SNME))				
	{
		*data = this->m_SM;
	}
	else if(StringMatch(s, VAR_SNAC))				
	{
		*data = this->m_SR;
	}
	else									
		throw ModelException(MID_SNO_SP,"Get1DData","Result " + s 
		+ " does not exist in current module. Please contact the module developer.");
	*n = this->m_nCells;
}

