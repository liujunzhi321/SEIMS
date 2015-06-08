/*----------------------------------------------------------------------
*	Purpose: 	A simple fill and spill method method to calculate depression storage
*
*	Created:	Junzhi Liu
*	Date:		14-Febrary-2011
*
*	Revision:
*   Date:
*---------------------------------------------------------------------*/

#include "DepressionFS.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include "omp.h"
#include "math.h"

DepressionFS::DepressionFS(void):m_size(-1),
	m_depCo(NODATA), m_depCap(NULL), m_pet(NULL), m_ei(NULL),
	m_sd(NULL),	m_ed(NULL),	m_sr(NULL), m_checkInput(true)
{
	
}

DepressionFS::~DepressionFS(void)
{
	if(m_sd != NULL)
		delete[] m_sd;
	//if(m_ed != NULL)
	//	delete[] m_ed;
	if(m_sr != NULL)
		delete[] m_sr;
	if (m_storageCapSurplus != NULL)
		delete m_storageCapSurplus;

}

bool DepressionFS::CheckInputData(void)
{
	if(m_date == -1)
		throw ModelException("DEP_FS","CheckInputData","You have not set the time.");

	if(m_size <= 0)
		throw ModelException("DEP_FS","CheckInputData","The cell number of the input can not be less than zero.");

	if(m_depCo == NODATA)
		throw ModelException("DEP_FS","CheckInputData","The parameter: initial depression storage coefficient has not been set.");
	if(m_depCap == NULL)
		throw ModelException("DEP_FS","CheckInputData","The parameter: depression storage capacity has not been set.");
#ifndef STORM_MODE	
	if(m_pet == NULL)
		throw ModelException("DEP_FS","CheckInputData","The parameter: PET has not been set.");
	if(m_ei == NULL)
		throw ModelException("DEP_FS","CheckInputData","The parameter: evaporation from the interception storage has not been set.");
#endif

	return true;
}

void  DepressionFS::initalOutputs()
{
	if(m_size <= 0) 
		throw ModelException("DEP_FS","initalOutputs","The cell number of the input can not be less than zero.");

	if(m_sd == NULL)
	{
		m_sd = new float[m_size];
		//m_ed = new float[m_size];
		m_sr = new float[m_size];
		m_storageCapSurplus = new float[m_size];
		#pragma omp parallel for
		for (int i = 0; i < m_size; ++i)
		{
			m_sd[i] = m_depCo* m_depCap[i];
			//m_ed[i] = 0.0f;
			m_sr[i] = 0.0f;
		}			
	}
}

int DepressionFS::Execute()
{
	initalOutputs();

	if(m_checkInput)
	{
		CheckInputData();	
		m_checkInput = false;
	}

	#pragma omp parallel for
	for (int i = 0; i < m_size; ++i)
	{
		// sr is temporarily used to stored the water depth including the depression storage
		float hWater = m_sr[i];
		if (hWater <= m_depCap[i])
		{
			m_sd[i] = hWater;
			m_sr[i] = 0.f;
		}
		else
		{
			m_sd[i] = m_depCap[i];
			m_sr[i] = hWater - m_depCap[i];
		}
		
		m_storageCapSurplus[i] = m_depCap[i] - m_sd[i];

	}

	return 0;
}


bool DepressionFS::CheckInputSize(const char* key, int n)
{
	if(n <= 0)
	{
		//StatusMsg("Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(m_size != n)
	{
		if(m_size <=0) m_size = n;
		else
		{
			//StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
			ostringstream oss;
			oss << "Input data for "+string(key) << " is invalid with size: " << n << ". The origin size is " << m_size << ".\n";  
			throw ModelException("DEP_FS","CheckInputSize",oss.str());
		}
	}

	return true;
}

void DepressionFS::SetValue(const char* key, float data)
{
	string sk(key);
	if (StringMatch(sk, "Depre_in"))
		m_depCo = data;
	else if (StringMatch(sk, "ThreadNum"))
	{
		omp_set_num_threads((int)data);
	}
	else
		throw ModelException("DEP_FS", "SetSingleData", "Parameter " + sk 
		+ " does not exist in the DEP_FS module. Please contact the module developer.");
	
}

void DepressionFS::Set1DData(const char* key, int n, float* data)
{
	//check the input data
	CheckInputSize(key,n);
	string sk(key);
	if(StringMatch(sk, "Depression"))
		m_depCap = data;
	else if(StringMatch(sk, "D_PET"))
		m_pet = data;
	else
		throw ModelException("DEP_FS", "Set1DData", "Parameter " + sk 
		+ " does not exist in the DEP_FS module. Please contact the module developer.");
	
}

void DepressionFS::Get1DData(const char* key, int* n, float** data)
{
	initalOutputs();

	string sk(key);
	*n = m_size;
	if (StringMatch(sk, "DPST"))
		*data = m_sd;
	//else if (StringMatch(sk, "DEET"))
	//	*data = m_ed;
	else if (StringMatch(sk, "SURU"))
		*data = m_sr;
	else if (StringMatch(sk, "STCAPSURPLUS"))
		*data = m_storageCapSurplus;
	else
		throw ModelException("DEP_FS", "Get1DData", "Output " + sk 
		+ " does not exist in the DEP_FS module. Please contact the module developer.");

}

