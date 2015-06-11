#include "InorganicPSorption.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>
#include <cmath>
#include <iostream>

using namespace std;

InorganicPSorption::InorganicPSorption(void):m_nLayers(3), m_size(-1), m_sloER(0.0006f), m_SoluP(NULL),  // input
	m_pai(NULL), m_actMinP(NULL), m_staMinP(NULL), m_tSolActP(NULL), m_tStaActP(NULL)  // output
{

}

InorganicPSorption::~InorganicPSorption(void)
{
	if(m_pai != NULL)
	{
		for(int i=0; i < m_nLayers; i++)
			delete [] m_pai;
		delete [] m_pai;
	}
	if(m_actMinP != NULL)
	{
		for(int i=0; i < m_nLayers; i++)
			delete [] m_actMinP;
		delete [] m_actMinP;
	}
	if(m_staMinP != NULL)
	{
		for(int i=0; i < m_nLayers; i++)
			delete [] m_staMinP;
		delete [] m_staMinP;
	}
	if(m_tSolActP != NULL)
	{
		for(int i=0; i < m_nLayers; i++)
			delete [] m_tSolActP;
		delete [] m_tSolActP;
	}
	if(m_tStaActP != NULL)
	{
		for(int i=0; i < m_nLayers; i++)
			delete [] m_tStaActP;
		delete [] m_tStaActP;
	}
}

bool InorganicPSorption::CheckInputData(void)
{
	if(m_size <= 0)
	{
		throw ModelException("InorganicPSorption","CheckInputData","The cell number of the input can not be less than zero.");
		return false;
	}
	if(m_sloER <= 0)
	{
		throw ModelException("InorganicPSorption","CheckInputData","The rslow equilibration rate constant of the input can not be less than zero.");
		return false;
	}
	if(m_SoluP == NULL)
		throw ModelException("InorganicPSorption","CheckInputData","You have not set the amount of phosphorus in solution.");
	/*if(m_aftSolP == NULL)
		throw ModelException("InorganicPSorption","CheckInputData","You have not set the amount of phosphorus in solution after fertilization and incubation.");
	if(m_befSolP == NULL)
		throw ModelException("InorganicPSorption","CheckInputData","You have not set the amount of phosphorus in solution before fertilization.");
	if(m_solPFer == NULL)
		throw ModelException("InorganicPSorption","CheckInputData","You have not set the amount of soluble P fertilizer added to the sample.");*/
	return true;
}

void  InorganicPSorption::initalOutputs()
{
	if(m_size <= 0) throw ModelException("InorganicPSorption","initalOutputs","The cell number of the input can not be less than zero.");

	if(m_tSolActP == NULL)
	{
		m_pai = new float*[m_nLayers];
		m_actMinP = new float*[m_nLayers];
		m_staMinP = new float*[m_nLayers];
		m_tSolActP = new float*[m_nLayers];
		m_tStaActP = new float*[m_nLayers];
	
	#pragma omp parallel for
		for (int i = 0; i < m_nLayers; ++i)
		{
			m_pai[i] = new float[m_size];
			m_actMinP[i] = new float[m_size];
			m_staMinP[i] = new float[m_size];
			m_tSolActP[i] = new float[m_size];
			m_tStaActP[i] = new float[m_size];
			for (int j =0; j < m_size; ++j)
			{
				m_pai[i][j] = 0.0f;
				m_actMinP[i][j] = 0.0f;
				m_staMinP[i][j] = 0.0f;
				m_tSolActP[i][j] = 0.0f;
				m_tStaActP[i][j] = 0.0f;
			}

		}
	}

}

bool InorganicPSorption::CheckInputSize(const char* key, int n)
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
			throw ModelException("InorganicPSorption","CheckInputSize",oss.str());
		}
	}

	return true;
}

void InorganicPSorption::SetValue(const char* key, float data)
{
	string sk(key);
	if (StringMatch(sk, "SloER"))
	{
		if(data > 0)
			m_sloER = data;
	}
	else if (StringMatch(sk, "ThreadNum"))
	{
		omp_set_num_threads((int)data);
	}
	else
		throw ModelException("InorganicPSorption", "SetSingleData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
	
}

void InorganicPSorption::Set2DData(const char* key, int nrows, int ncols, float** data)
{
	//check the input data
	CheckInputSize(key, ncols);

	string sk(key);
	if (StringMatch(sk, "D_SoluP"))
		m_SoluP = data;
	else
		throw ModelException("InorganicPSorption", "Set2DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
}

void InorganicPSorption::Get2DData(const char* key, int *nRows, int *nCols, float*** data)
{
	string sk(key);
	*nRows = m_nLayers;
	*nCols = m_size;
    if (StringMatch(sk, "Pai"))
		*data = m_pai;
	else if (StringMatch(sk, "ActMinP"))
		*data = m_actMinP;
	else if (StringMatch(sk, "StaMinP"))
		*data = m_staMinP;
	else if (StringMatch(sk, "TSolActP"))
		*data = m_tSolActP; 
	else if (StringMatch(sk, "TStaActP"))
		*data = m_tStaActP; 
	else
		throw ModelException("InorganicPSorption", "Get2DData", "Output " + sk 
		+ " does not exist in the Inorganic P Sorption module. Please contact the module developer.");
}

int InorganicPSorption::Execute()
{
	//check the data
	CheckInputData();	

	initalOutputs();

	#pragma omp parallel for
	for(int i=0; i < m_nLayers; i++)
    {
		for(int j=0; j < m_size; j++)
		{
			m_pai[i][j] = 0.7f;
		    m_actMinP[i][j] = m_SoluP[i][j] * (1 - m_pai[i][j]) / m_pai[i][j];
			m_staMinP[i][j] = 4 * m_actMinP[i][j];
            float r = m_actMinP[i][j] * m_pai[i][j] / (1 - m_pai[i][j]);
		    if(m_SoluP[i][j] > r)
				m_tSolActP[i][j] = 0.1f * (m_SoluP[i][j] - r);
		    else
			    m_tSolActP[i][j] = 0.6f * (m_SoluP[i][j] - r);
		
		    float r1 = 4 * m_actMinP[i][j] - m_staMinP[i][j];
		    if(r1 > 0)
			    m_tStaActP[i][j] = m_sloER * r1;
		    else
			    m_tStaActP[i][j] = 0.1f * m_sloER * r1;

			m_SoluP[i][j] -= m_tSolActP[i][j];
			m_actMinP[i][j] += m_tSolActP[i][j] - m_tStaActP[i][j];
			m_staMinP[i][j] += m_tStaActP[i][j];
		}
	}

	return 0;
}