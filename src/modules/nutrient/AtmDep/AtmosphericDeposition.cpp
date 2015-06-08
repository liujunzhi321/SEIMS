#include "AtmosphericDeposition.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>
#include <cmath>
#include <iostream>

using namespace std;

AtmosphericDeposition::AtmosphericDeposition(void):m_nLayers(3), m_size(-1),m_conRainNitra(-1.0f), m_conRainAmm(-1.0f), m_rootDepth(NULL), m_P(NULL), //input
	 m_Nitrate(NULL), m_Ammon(NULL), m_addRainNitra(NULL), m_addRainAmm(NULL) //output
{
	m_depth[0] = 10.f;
	m_depth[1] = 100.f;
}

AtmosphericDeposition::~AtmosphericDeposition(void)
{
	if(m_Nitrate != NULL)
	{
		for (int i=0; i < m_size; ++i)
			delete [] m_Nitrate[i];
		delete [] m_Nitrate;
	}
	if(m_Ammon != NULL)
	{
		for (int i=0; i < m_size; ++i)
			delete [] m_Ammon[i];
		delete [] m_Ammon;
	}
	if(m_addRainNitra != NULL)
		delete [] m_addRainNitra;
	if(m_addRainAmm != NULL)
		delete [] m_addRainAmm;
}

bool AtmosphericDeposition::CheckInputData(void)
{
	if(m_size <= 0)
	{
		throw ModelException("AtmosphericDeposition","CheckInputData","The cell number of the input can not be less than zero.");
		return false;
	}
	if(m_conRainNitra <= 0)
	{
		m_conRainNitra = 1.f;
	}
	if(m_conRainAmm <= 0)
	{
		m_conRainAmm = 1.f;
	}

	if(m_P == NULL)
		throw ModelException("AtmosphericDeposition","CheckInputData","You have not set the amount of precipitation in a given day.");
	
	return true;
}

void  AtmosphericDeposition::initalOutputs()
{
	if(m_size <= 0) throw ModelException("AtmosphericDeposition","initalOutputs","The cell number of the input can not be less than zero.");

	if(m_Nitrate == NULL)
	{
		m_Nitrate = new float*[m_size];
        m_Ammon = new float*[m_size];
		m_addRainNitra = new float[m_size];
		m_addRainAmm = new float[m_size];

		#pragma omp parallel for
		for (int i = 0; i < m_size; ++i)
		{
			m_addRainNitra[i] = 0.0f;	
			m_addRainAmm[i] = 0.f;

			m_Nitrate[i] = new float[m_nLayers];
			m_Ammon[i] = new float[m_nLayers];
			
			m_Nitrate[i][0] = 7 * exp ( -m_depth[0] / 2 / 1000);
			m_Nitrate[i][1] = 7 * exp ( -(m_depth[0] + m_depth[1]) / 2 / 1000);
			m_Nitrate[i][2] = 7 * exp ( -(m_depth[1] + m_rootDepth[i]) / 2 / 1000);

			for (int j = 0; j < m_nLayers; ++j)
				m_Ammon[i][j] = 0.f;
		}

	}

}

bool AtmosphericDeposition::CheckInputSize(const char* key, int n)
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
			throw ModelException("AtmosphericDeposition","CheckInputSize",oss.str());
		}
	}

	return true;
}

void AtmosphericDeposition::SetValue(const char* key, float data)
{
	string sk(key);
	if (StringMatch(sk, "ConRainNitra"))
		m_conRainNitra = data;
	else if (StringMatch(sk, "ConRainAmm"))
		m_conRainAmm = data;
	else if (StringMatch(sk, "ThreadNum"))
		omp_set_num_threads((int)data);
	else
		throw ModelException("AtmosphericDeposition", "SetSingleData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
	
}

void AtmosphericDeposition::Set1DData(const char* key, int n, float* data)
{
	//check the input data
	CheckInputSize(key,n);
	string sk(key);
	if(StringMatch(sk, "D_P"))
		m_P = data;
	else if(StringMatch(sk, "rootDepth"))
		m_rootDepth = data;
	else
		throw ModelException("AtmosphericDeposition", "Set1DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
	
}

void AtmosphericDeposition::Get1DData(const char* key, int* n, float** data)
{
	initalOutputs();

	string sk(key);
	*n = m_size;
	if (StringMatch(sk, "AddRainNitra"))
		*data = m_addRainNitra; 
	else if (StringMatch(sk, "AddRainAmm"))
		*data = m_addRainAmm;
	else
		throw ModelException("AtmosphericDeposition", "Get1DData", "Output " + sk 
		+ " does not exist in the AtmosphericDeposition module. Please contact the module developer.");
}

void AtmosphericDeposition::Get2DData(const char* key, int *nRows, int *nCols, float*** data)
{
	string sk(key);
	*nRows = m_size;
	*nCols = m_nLayers;
	if (StringMatch(sk, "Nitrate"))
		*data = m_Nitrate;
	else if (StringMatch(sk, "Ammon"))
		*data = m_Ammon;
	else
		throw ModelException("AtmosphericDeposition", "Get2DData", "Output " + sk 
		+ " does not exist in the Atmospheric Deposition module. Please contact the module developer.");

}

int AtmosphericDeposition::Execute()
{
	//check the data
	CheckInputData();	

	initalOutputs();

	#pragma omp parallel for
	for(int i=0; i < m_size; i++)
    {
		m_addRainNitra[i] = 0.01f * m_conRainNitra * m_P[i];
		m_addRainAmm[i] = 0.01f * m_conRainAmm * m_P[i];

		m_Nitrate[i][0] += m_addRainNitra[i];
		m_Ammon[i][0] += m_addRainAmm[i];
	}

	return 0;
}