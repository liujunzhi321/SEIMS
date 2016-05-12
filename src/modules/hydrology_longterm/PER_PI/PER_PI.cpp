// PER_PI.cpp : main project file.

#include "PER_PI.h"
#include "MetadataInfo.h"
#include "util.h"
#include "ModelException.h"
#include <sstream>
#include <math.h>
#include <cmath>
#include <time.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>

PER_PI::PER_PI(void):m_nSoilLayers(2), m_dt(-1), m_nCells(-1), m_frozenT(-99.f), m_ks(NULL), m_porosity(NULL),
	m_infil(NULL), m_poreIndex(NULL), m_sm(NULL), m_fc(NULL), m_soilT(NULL), m_rootDepth(NULL), m_perc(NULL), m_upSoilDepth(200.f)
{

}

PER_PI::~PER_PI(void)
{	
	if(m_perc == NULL) 
	{
		for (int i = 0; i < m_nCells; i++)
			delete[] m_perc[i];
		delete[] m_perc;
	}
}

//Execute module
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
	
	#pragma omp parallel for
	for (int i = 0; i < m_nCells; i++)
	{
		float k, maxSoilWater, fcSoilWater, swater;
		float depth[2];
		depth[0] = m_upSoilDepth;
		depth[1] = m_rootDepth[i] - m_upSoilDepth;
		if(depth[1] < 0)
		{
			ostringstream oss;
			oss << "The root depth at cell(" << i << ") is " << m_rootDepth[i] << ", and is less than the upper soil depth (" << m_upSoilDepth << endl;
			throw ModelException("SSR_DA", "Execute",  oss.str());
		}

		m_sm[i][0] += m_infil[i] / depth[0];
		for (int j = 0; j < m_nSoilLayers; j++)
		{
			//No movement if soil moisture is below field capacity
			m_perc[i][j] = 0.f;

			// for the upper two layers, soil may be frozen
			if(j == 0 && m_soilT[i] <= m_frozenT) 
				continue;

			if (m_sm[i][j] > m_fc[i][j])
			{
				maxSoilWater = depth[j] * m_porosity[i][j];
				swater = depth[j] * m_sm[i][j];

				fcSoilWater = depth[j] * m_fc[i][j];
				
				//the moisture content can exceed the porosity in the way the algorithm is implemented
				if (m_sm[i][j] > m_porosity[i][j])
					k = m_ks[i][j];
				else
				{
					float dcIndex = 2.f/m_poreIndex[i][j] + 3.f; // pore disconnectedness index
					k = m_ks[i][j] * pow(m_sm[i][j]/m_porosity[i][j], dcIndex);
				}

				m_perc[i][j] = k * m_dt/3600.f;

				if(swater - m_perc[i][j] < fcSoilWater)
					m_perc[i][j] = swater - fcSoilWater;
				else if(swater - m_perc[i][j] > maxSoilWater)
					m_perc[i][j] = swater - maxSoilWater;

				//Adjust the moisture content in the current layer, and the layer immediately below it
				m_sm[i][j] -= m_perc[i][j]/depth[j];

				if(j < m_nSoilLayers-1)
					m_sm[i][j+1] += m_perc[i][j]/depth[j+1];

				if(m_sm[i][j] != m_sm[i][j] || m_sm[i][j] < 0.f)
				{
					m_sm[i][j] = 0.f;
					//cout << "PER_PI CELL:" << i << "\tPerco:" << swater << "\t" << 
					//	fcSoilWater << "\t" << m_perc[i][j] << "\t" << depth[j] << "\tValue:" << m_sm[i][j] << endl;
					//throw ModelException("PER_PI", "Execute", "moisture is less than zero.");
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

	if (StringMatch(sk, "Percolation_2D"))   // excess precipitation
	{
		*data = m_perc;
	}
	else
		throw ModelException("PER_PI", "Get2DData", "Output " + sk 
		+ " does not exist. Please contact the module developer.");

}

void PER_PI::Set1DData(const char* key, int nRows, float* data)
{
	string s(key);

	CheckInputSize(key,nRows);

	if(StringMatch(s,"Rootdepth"))		
		m_rootDepth = data;
	else if(StringMatch(s,"D_SOTE"))		
		m_soilT = data;
	else if(StringMatch(s,"D_INFIL"))		
		m_infil = data;
	else									
		throw ModelException("PER_PI","Set1DData",
		    "Parameter " + s + " does not exist in PER_PI method. Please contact the module developer.");

}

void PER_PI::Set2DData(const char* key, int nrows, int ncols, float** data)
{
	string sk(key);
	CheckInputSize(key, nrows);
	m_nSoilLayers = ncols;

	if(StringMatch(sk,"Conductivity_2D"))		
		m_ks = data;
	else if (StringMatch(sk, "porosity_2D"))
		m_porosity = data;
	else if (StringMatch(sk, "fieldCap_2D"))
		m_fc = data;
	else if(StringMatch(sk, "Poreindex_2D"))		
		m_poreIndex = data;	
	else if(StringMatch(sk, "D_SOMO_2D"))		
		m_sm = data;
	else
		throw ModelException("PER_PI", "Set2DData", 
		    "Parameter " + sk + " does not exist. Please contact the module developer.");
}

void PER_PI::SetValue(const char* key, float data)
{
	string s(key);
	if(StringMatch(s,"TimeStep"))			
		m_dt = int(data);
	else if(StringMatch(s,"t_soil"))		
		m_frozenT = data;
	else if (StringMatch(s, VAR_OMP_THREADNUM))
		omp_set_num_threads((int)data);
	//else if(StringMatch(s,"UpperSoilDepth"))		
	//	m_upSoilDepth = data;
	else									
		throw ModelException("PER_PI","SetValue",
		    "Parameter " + s + " does not exist in PER_PI method. Please contact the module developer.");
}

string PER_PI::toString(float value)
{
	char s[20];
	strprintf(s,20,"%f",value);
	return string(s);
}

bool PER_PI::CheckInputData()
{
	if(m_date <=0)				
		throw ModelException("PER_PI","CheckInputData","You have not set the time.");
	if(m_nCells <= 0)					
		throw ModelException("PER_PI","CheckInputData","The dimension of the input data can not be less than zero.");
	if(m_dt <=0)			
		throw ModelException("PER_PI","CheckInputData","The time step can not be less than zero.");

	if(m_ks == NULL)	
		throw ModelException("PER_PI","CheckInputData","The Conductivity can not be NULL.");
	if(m_porosity == NULL)		
		throw ModelException("PER_PI","CheckInputData","The Porosity can not be NULL.");
	if(m_poreIndex == NULL)		
		throw ModelException("PER_PI","CheckInputData","The Poreindex can not be NULL.");
	if(m_sm == NULL)		
		throw ModelException("PER_PI","CheckInputData","The Moisture can not be NULL.");
	if(m_rootDepth == NULL)		
		throw ModelException("PER_PI","CheckInputData","The rootdepth can not be NULL.");
	if(m_soilT == NULL)			
		throw ModelException("PER_PI","CheckInputData","The soil temerature can not be NULL.");
	if(m_infil == NULL)			
		throw ModelException("PER_PI","CheckInputData","The infiltration can not be NULL.");

	if(FloatEqual(m_frozenT, -99.0f))		
		throw ModelException("PER_PI","CheckInputData","The threshold soil freezing temerature can not be NULL.");

	return true;
}

bool PER_PI::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException("PER_PI","CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(this->m_nCells != n)
	{
		if(this->m_nCells <=0) this->m_nCells = n;
		else
		{
			throw ModelException("PER_PI","CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}

	return true;
}
