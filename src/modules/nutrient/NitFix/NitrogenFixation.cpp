#include "NitrogenFixation.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>
#include <cmath>
#include <iostream>

using namespace std;

NitrogenFixation::NitrogenFixation(void):m_nLayers(3), m_size(-1), m_frPHU(NULL), m_Nitrate(NULL),m_SOMO(NULL), m_FieldCap(NULL), m_demNit(NULL),  //input
	 m_fGS(NULL), m_fSW(NULL), m_fSN(NULL), m_fixN(NULL)  //output
{

}

NitrogenFixation::~NitrogenFixation(void)
{
	if(m_fGS != NULL)
	{
		delete [] m_fGS;
	}
	if(m_fSW != NULL)
	{
		for(int i=0; i < m_nLayers; i++)
			delete [] m_fSW;
		delete [] m_fSW;
	}
	if(m_fSN != NULL)
	{
		for(int i=0; i < m_nLayers; i++)
			delete [] m_fSN;
		delete [] m_fSN;
	}
	if(m_fixN != NULL)
	{
		for(int i=0; i < m_nLayers; i++)
			delete [] m_fixN;
		delete [] m_fixN;
	}

}

bool NitrogenFixation::CheckInputData(void)
{
	if(m_size <= 0)
	{
		throw ModelException("NitrogenFixation","CheckInputData","The cell number of the input can not be less than zero.");
		return false;
	}
	if(m_frPHU == NULL)
	{
		throw ModelException("NitrogenFixation","CheckInputData","You have not set the fraction of phu accumulated on a given day.");
		return false;
	}
	if(m_Nitrate == NULL)
	{
		throw ModelException("NitrogenFixation","CheckInputData","You have not set the amount of nitrate in layer ly.");
		return false;
	}
	
	if(m_SOMO == NULL)
		throw ModelException("NitrogenFixation","CheckInputData","You have not set the soil moisture of the layer.");
	if(m_FieldCap == NULL)
		throw ModelException("NitrogenFixation","CheckInputData","You have not set the field capacity.");
	if(m_demNit == NULL)
	{
		m_demNit = new float*[m_nLayers];
	
		for (int i = 0; i < m_nLayers; ++i)
		{
			m_demNit[i] = new float[m_size];
			for (int j = 0; j < m_size; ++j)
				m_demNit[i][j] = 10.0f;
		}	
	}
	
	return true;
}

void  NitrogenFixation::initalOutputs()
{
	if(m_size <= 0) throw ModelException("NitrogenFixation","initalOutputs","The cell number of the input can not be less than zero.");

	if(m_fixN == NULL)
	{
		m_fGS = new float[m_size];
		#pragma omp parallel for
		for (int i = 0; i < m_size; ++i)
		{
			m_fGS[i] = 0.0f;
		}

		m_fSW = new float*[m_nLayers];
		m_fSN = new float*[m_nLayers];
		m_fixN = new float*[m_nLayers];
	
	#pragma omp parallel for
		for (int i = 0; i < m_nLayers; ++i)
		{
			m_fSW[i] = new float[m_size];
			m_fSN[i] = new float[m_size];
			m_fixN[i] = new float[m_size];
			
			for (int j = 0; j < m_size; ++j)
			{
				m_fSW[i][j] = 0.0f;
				m_fSN[i][j] = 0.0f;
				m_fixN[i][j] = 0.0f;
			}
		}
	}

}

bool NitrogenFixation::CheckInputSize(const char* key, int n)
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
			throw ModelException("NitrogenFixation","CheckInputSize",oss.str());
		}
	}

	return true;
}

void NitrogenFixation::Set1DData(const char* key, int n, float* data)
{
    //check the input data
	string s(key);
	//n = m_nCells;
	CheckInputSize(key, n);
	if(StringMatch(s,"Fr_PHU"))				
		m_frPHU = data;
	else									
		throw ModelException("NitrogenFixation","Set1DData","Parameter " + s + 
		" does not exist in UptakeGrowth method. Please contact the module developer.");
}

void NitrogenFixation::Get1DData(const char* key, int* n, float** data)
{
	initalOutputs();

	string sk(key);
	*n = m_size;
	if (StringMatch(sk, "F_GS"))
		*data = m_fGS; 
	else
		throw ModelException("NitrogenFixation", "Get1DData", "Output " + sk 
		+ " does not exist in the NitrogenFixation module. Please contact the module developer.");
}

void NitrogenFixation::Set2DData(const char* key, int nrows, int ncols, float** data)
{
	//check the input data
	CheckInputSize(key, ncols);

	string sk(key);
	if(StringMatch(sk, "D_Nitrate"))
		m_Nitrate = data;
	else if(StringMatch(sk,"SOMO_2D"))		
		m_SOMO = data;
	else if (StringMatch(sk, "FieldCap_2D"))
		m_FieldCap = data;
	else if(StringMatch(sk, "D_DemNit"))
		m_demNit = data;
	else
		throw ModelException("NitrogenFixation", "Set2DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
}

void NitrogenFixation::Get2DData(const char* key, int *nRows, int *nCols, float*** data)
{
	string sk(key);
	*nRows = m_nLayers;
	*nCols = m_size;
    if (StringMatch(sk, "FixN"))
		*data = m_fixN; 
	else if (StringMatch(sk, "F_SW"))
		*data = m_fSW;
	else if (StringMatch(sk, "F_SN"))
		*data = m_fSN;
	else
		throw ModelException("NitrogenFixation", "Get2DData", "Output " + sk 
		+ " does not exist in the NitrogenFixation module. Please contact the module developer.");
}

int NitrogenFixation::Execute()
{
	//check the data
	CheckInputData();	

	initalOutputs();

	#pragma omp parallel for
	for(int i=0; i < m_size; i++)
    { 
		if (m_frPHU[i] <= 0.15f)
				m_fGS[i] = 0.f;
		else if ( 0.15f < m_frPHU[i] <= 0.30f)
				m_fGS[i] = 6.67f * m_frPHU[i] - 1.f;
		else if ( 0.30f < m_frPHU[i] <= 0.55f)
				m_fGS[i] = 1.f;
		else if ( 0.55f < m_frPHU[i] <= 0.75f)
				m_fGS[i] = 3.75f - 5.f * m_frPHU[i] - 1.f;
		else
				m_fGS[i] = 0.f;
	}
	#pragma omp parallel for
	for(int i=0; i < m_nLayers; i++)
    { 
		for(int j=0; j < m_size; j++)
		{
			if (m_Nitrate[i][j] <= 100)
				m_fSN[i][j] = 1.f;
			else if ( 100 < m_Nitrate[i][j] <= 300)
				m_fSN[i][j] = 1.5f - 0.0005f * m_Nitrate[i][j];
			else
				m_fSN[i][j] = 0.f;

			int hydroIndex = max(0, i-1);
			m_fSW[i][j] = m_SOMO[hydroIndex][j] / ( m_FieldCap[hydroIndex][j] * 0.85f);
			float r1 = min (m_fSN[i][j], m_fSW[i][j]);
			float r = min ( r1, 1.f);
			
			m_fixN[i][j] = m_demNit[i][j] * m_fGS[i] * r;
		}
	}

	return 0;
}