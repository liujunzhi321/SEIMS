#include "PER_PI.h"
#include "MetadataInfo.h"
#include "util.h"
#include "ModelException.h"
#include <sstream>
#include <cmath>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>

PER_PI::PER_PI(void):m_nSoilLayers(-1), m_dt(-1), m_nCells(-1), m_frozenT(NODATA), 
	m_ks(NULL), m_porosity(NULL),m_poreIndex(NULL),m_fc(NULL),m_soilDepth(NULL),
	m_infil(NULL),  m_soilT(NULL), m_somo(NULL),
	m_perc(NULL), 
	m_upSoilDepth(NULL)
{
}

PER_PI::~PER_PI(void)
{	
	if(m_perc == NULL) 
	{
		for (int i = 0; i < m_nCells; i++)
			delete[] m_perc[i];
		delete[] m_perc;
		m_perc = NULL;
	}
}

int PER_PI::Execute()
{
	CheckInputData();

	if(m_perc == NULL) 
	{
		m_perc = new float*[m_nCells];

		#pragma omp parallel for
		for (int i = 0; i < m_nCells; i++)
		{
			m_perc[i] = new float[m_nSoilLayers];
			for (int j = 0; j < m_nSoilLayers; j++)
				m_perc[i][j] = 0.f;
		}
	}
	if(m_upSoilDepth == NULL)
		m_upSoilDepth = new float[m_nSoilLayers];
	#pragma omp parallel for
	for (int i = 0; i < m_nCells; i++)
	{
		float k, maxSoilWater, fcSoilWater, swater;
		// Update soil layers from solid two layers to multi-layers by m_nSoilLayers. By LJ
		int curSoilLayers = -1, j;
		m_upSoilDepth[0] =	m_soilDepth[i][0];
		for(j = 1; j < m_nSoilLayers; j++)
		{
			if(!FloatEqual(m_soilDepth[i][j],NODATA_VALUE))
				m_upSoilDepth[j] = m_soilDepth[i][j] - m_soilDepth[i][j-1];
			else
				break;
		}
		curSoilLayers = j;

		m_somo[i][0] += m_infil[i] / m_upSoilDepth[0];
		for (int j = 0; j < curSoilLayers; j++)
		{
			//No movement if soil moisture is below field capacity
			m_perc[i][j] = 0.f;
			// for the upper two layers, soil may be frozen
			if(j == 0 && m_soilT[i] <= m_frozenT) 
				continue;
			if (m_somo[i][j] > m_fc[i][j])
			{
				maxSoilWater = m_upSoilDepth[j] * m_porosity[i][j];
				swater = m_upSoilDepth[j] * m_somo[i][j];

				fcSoilWater = m_upSoilDepth[j] * m_fc[i][j];
				
				//the moisture content can exceed the porosity in the way the algorithm is implemented
				if (m_somo[i][j] > m_porosity[i][j])
					k = m_ks[i][j];
				else
				{
					float dcIndex = 2.f/m_poreIndex[i][j] + 3.f; // pore disconnectedness index
					k = m_ks[i][j] * pow(m_somo[i][j]/m_porosity[i][j], dcIndex);
				}

				m_perc[i][j] = k * m_dt/3600.f;

				if(swater - m_perc[i][j] < fcSoilWater)
					m_perc[i][j] = swater - fcSoilWater;
				else if(swater - m_perc[i][j] > maxSoilWater)
					m_perc[i][j] = swater - maxSoilWater;

				//Adjust the moisture content in the current layer, and the layer immediately below it
				m_somo[i][j] -= m_perc[i][j]/m_upSoilDepth[j];

				if(j < curSoilLayers-1)
					m_somo[i][j+1] += m_perc[i][j]/m_upSoilDepth[j+1];

				if(m_somo[i][j] != m_somo[i][j] || m_somo[i][j] < 0.f)
				{
					cout << MID_PER_PI<<" CELL:" << i <<", Layer: "<<j<< "\tPerco:" << swater << "\t" << 
						fcSoilWater << "\t" << m_perc[i][j] << "\t" << m_upSoilDepth[j] << "\tValue:" << m_somo[i][j] << endl;
					throw ModelException(MID_PER_PI, "Execute", "moisture is less than zero.");
				}
			}
		}
	}
	return 0;
}


void PER_PI::Get2DData(const char* key, int *nRows, int *nCols, float*** data)
{
	string sk(key);
	*nRows = m_nCells;
	*nCols = m_nSoilLayers;
	if (StringMatch(sk, VAR_PERCO))   *data = m_perc;
	else
		throw ModelException(MID_PER_PI, "Get2DData", "Output " + sk 
		+ " does not exist. Please contact the module developer.");
}

void PER_PI::Set1DData(const char* key, int nRows, float* data)
{
	string s(key);

	CheckInputSize(key,nRows);

	if(StringMatch(s,VAR_SOTE))	m_soilT = data;		
	else if(StringMatch(s,VAR_INFIL))	m_infil = data;	
	else									
		throw ModelException(MID_PER_PI,"Set1DData",
		    "Parameter " + s + " does not exist in current module. Please contact the module developer.");
}

void PER_PI::Set2DData(const char* key, int nrows, int ncols, float** data)
{
	string sk(key);
	CheckInputSize(key, nrows);
	m_nSoilLayers = ncols;

	if(StringMatch(sk, VAR_CONDUCT))		m_ks = data;
	else if(StringMatch(sk, VAR_SOILDEPTH)) m_soilDepth = data;
	else if (StringMatch(sk, VAR_POROST))	m_porosity = data;
	else if (StringMatch(sk, VAR_FIELDCAP))  m_fc = data;
	else if(StringMatch(sk, VAR_POREID))		m_poreIndex = data;	
	else if(StringMatch(sk, VAR_SOMO))		m_somo = data;
	else
		throw ModelException(MID_PER_PI, "Set2DData", 
		    "Parameter " + sk + " does not exist. Please contact the module developer.");
}

void PER_PI::SetValue(const char* key, float data)
{
	string s(key);
	if(StringMatch(s, Tag_TimeStep))		m_dt = int(data);	
	else if(StringMatch(s, VAR_T_SOIL))		m_frozenT = data;
	else if (StringMatch(s, VAR_OMP_THREADNUM))omp_set_num_threads((int)data);
	else									
		throw ModelException(MID_PER_PI,"SetValue",
		    "Parameter " + s + " does not exist in current module. Please contact the module developer.");
}

bool PER_PI::CheckInputData()
{
	if(m_date <=0)				
		throw ModelException(MID_PER_PI,"CheckInputData","You have not set the time.");
	if(m_nCells <= 0)					
		throw ModelException(MID_PER_PI,"CheckInputData","The dimension of the input data can not be less than zero.");
	if(m_dt <=0)			
		throw ModelException(MID_PER_PI,"CheckInputData","The time step can not be less than zero.");

	if(m_ks == NULL)	
		throw ModelException(MID_PER_PI,"CheckInputData","The Conductivity can not be NULL.");
	if(m_porosity == NULL)		
		throw ModelException(MID_PER_PI,"CheckInputData","The Porosity can not be NULL.");
	if(m_poreIndex == NULL)		
		throw ModelException(MID_PER_PI,"CheckInputData","The Pore index can not be NULL.");
	if(m_somo == NULL)		
		throw ModelException(MID_PER_PI,"CheckInputData","The Moisture can not be NULL.");
	if(m_soilDepth == NULL)		
		throw ModelException(MID_PER_PI,"CheckInputData","The soil depth can not be NULL.");
	if(m_soilT == NULL)			
		throw ModelException(MID_PER_PI,"CheckInputData","The soil temperature can not be NULL.");
	if(m_infil == NULL)			
		throw ModelException(MID_PER_PI,"CheckInputData","The infiltration can not be NULL.");
	if(FloatEqual(m_frozenT, NODATA))		
		throw ModelException(MID_PER_PI,"CheckInputData","The threshold soil freezing temperature can not be NULL.");
	return true;
}

bool PER_PI::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException(MID_PER_PI,"CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(this->m_nCells != n)
	{
		if(this->m_nCells <=0) this->m_nCells = n;
		else
		{
			throw ModelException(MID_PER_PI,"CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}
	return true;
}
