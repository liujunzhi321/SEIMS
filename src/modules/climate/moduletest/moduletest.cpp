/*!
 * \file ModulesTest.cpp
 * \author Zhiqiang Yu
 * \date Apr. 2010
 *
 * 
 */
#include "api.h"
#include <iostream>
#include "moduletest.h"
#include "MetadataInfo.h"
#include "ModelException.h"
using namespace std;
ModulesTest::ModulesTest(void)
{
	this->m_nCells = -1;
	this->m_raster1D = NULL;
	this->m_output1Draster = NULL;
	this->m_raster2D = NULL;
	this->m_output2Draster = NULL;
	//this->m_routingLayers = NULL;
}


ModulesTest::~ModulesTest(void)
{
	if (m_output1Draster != NULL)
		delete[] m_output1Draster;

	if (m_output2Draster != NULL)
	{
		for(int i = 0; i < m_soilLayers; i++)
			delete[] m_output2Draster[i];
		delete[] m_output2Draster;
		m_output2Draster = NULL;
	}
}

void ModulesTest::Set1DData(const char* key, int n, float* data)
{
	string sk(key);
	if (m_nCells < 0 || m_nCells != n)
	{
		m_nCells = n;
	}
	if(StringMatch(sk, VAR_CN2)) this->m_raster1D = data;	
}

void ModulesTest::Set2DData(const char* key, int n, int col, float** data)
{
	string sk(key);
	if (m_nCells < 0 || m_nCells != n)
	{
		m_nCells = n;
	}
	if(StringMatch(sk, VAR_CONDUCT)) 
	{
		this->m_raster2D = data;
		this->m_soilLayers = col;
	}
	//else if(StringMatch(sk, Tag_ROUTING_LAYERS))
	//{
	//	m_nLayers = n;
	//	m_routingLayers = data;
	//}
}
int ModulesTest::Execute()
{
	/// Initialize output variables
	if (m_output1Draster == NULL)
	{
		m_output1Draster = new float[m_nCells];
#pragma omp parallel for
		for (int i = 0; i < m_nCells; ++i)
			m_output1Draster[i] = 0.f;
	}
	if (m_output2Draster == NULL)
	{
		m_output2Draster = new float*[m_nCells];
#pragma omp parallel for
		for (int i = 0; i < m_nCells; ++i)
		{
			m_output2Draster[i] = new float[m_soilLayers];
			for(int j = 0; j < m_soilLayers; j++)
				m_output2Draster[i][j] = 0.f;
		}
	}
	/// Execute function
#pragma omp parallel for
	for (int i = 0; i < m_nCells; ++i)
	{
		m_output1Draster[i] = m_raster1D[i] * 0.5f;
		for(int j = 0; j < m_soilLayers; j++)
			m_output2Draster[i][j] = m_raster2D[i][j] + 2.f;
	}
	return true;
}

void ModulesTest::Get1DData(const char* key, int* n, float** data)
{
	string sk(key);
	if(StringMatch(sk, "CN2_M"))
	{
		*data = this->m_output1Draster;
		*n = this->m_nCells;
	}
}
void ModulesTest::Get2DData(const char* key, int* n, int* col, float*** data)
{
	string sk(key);
	if(StringMatch(sk, "K_M"))
	{
		*data = this->m_output2Draster;
		*n = this->m_nCells;
		*col = this->m_soilLayers;
	}
	//else if(StringMatch(sk, "LAYERS_M"))
	//{
	//	*data = this->m_routingLayers;
	//	*n = this->m_nLayers;
	//	*col = 0;
	//}
}