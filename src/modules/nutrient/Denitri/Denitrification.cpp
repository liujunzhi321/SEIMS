#include "Denitrification.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>
#include <cmath>
#include <iostream>

using namespace std;

Denitrification::Denitrification(void):m_nLayers(3), m_size(-1), m_denRC(1.4f), m_Nitrate(NULL), m_TF(NULL), m_WF(NULL), m_orgCar(NULL), m_denWF(NULL),  //input
	 m_denLostN(NULL)  //output
{

}

Denitrification::~Denitrification(void)
{
	if(m_denLostN != NULL)
	{
		for(int i=0; i < m_nLayers; i++)
			delete [] m_denLostN;
		delete [] m_denLostN;
	}

}

bool Denitrification::CheckInputData(void)
{
	if(m_size <= 0)
	{
		throw ModelException("Denitrification","CheckInputData","The cell number of the input can not be less than zero.");
		return false;
	}
	if(m_nLayers <= 0)
	{
		throw ModelException("Denitrification","CheckInputData","The layers number of the input can not be less than zero.");
		return false;
	}
	if(m_denRC <= 0)
	{
		throw ModelException("Denitrification","CheckInputData","The rate coefficient for denitrification of the input can not be less than zero.");
		return false;
	}
	if(m_Nitrate == NULL)
	{
		throw ModelException("Denitrification","CheckInputData","You have not set the amount of nitrate in layer ly.");
		return false;
	}
	if(m_orgCar == NULL)
	{
	}
	if(m_TF == NULL)
		throw ModelException("Denitrification","CheckInputData","You have not set the nutrient cycling temperature factor.");
	if(m_WF == NULL)
		throw ModelException("Denitrification","CheckInputData","You have not set the nutrient cycling water factor.");
/*	if(m_denWF == NULL)
	{
		m_denWF = new float*[m_size];
	
		for (int i = 0; i < m_size; ++i)
		{
			m_denWF[i] = new float[m_nLayers];
			for (int j = 0; j < m_nLayers; ++j)
				m_denWF[i][j] = 1.1f;
		}	
	}*/	
	
	return true;
}

void  Denitrification::initalOutputs()
{
	if(m_size <= 0) throw ModelException("Denitrification","initalOutputs","The cell number of the input can not be less than zero.");

	if(m_denLostN == NULL)
	{
		m_denLostN = new float*[m_nLayers];
	
	#pragma omp parallel for
		for (int i = 0; i < m_nLayers; ++i)
		{
			m_denLostN[i] = new float[m_size];
			
			for (int j = 0; j < m_size; ++j)
				m_denLostN[i][j] = 0.0f;
		}
	}

}

bool Denitrification::CheckInputSize(const char* key, int n)
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
			throw ModelException("Denitrification","CheckInputSize",oss.str());
		}
	}

	return true;
}

void Denitrification::SetValue(const char* key, float data)
{
	string sk(key);
	if (StringMatch(sk, "DenRC"))
	{
		if (data > 0)
			m_denRC = data;
	}
	else if (StringMatch(sk, "ThreadNum"))
	{
		omp_set_num_threads((int)data);
	}
	else
		throw ModelException("Denitrification", "SetSingleData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
	
}

void Denitrification::Set2DData(const char* key, int nrows, int ncols, float** data)
{
	//check the input data
	CheckInputSize(key, ncols);

	string sk(key);
	if(StringMatch(sk, "D_Nitrate"))
		m_Nitrate = data;
	else if(StringMatch(sk,"D_TF"))		
		m_TF = data;
	else if (StringMatch(sk, "D_WF"))
		m_WF = data;
	else if(StringMatch(sk, "DenWF"))
		m_denWF = data;
	else
		throw ModelException("Denitrification", "Set2DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
}

void Denitrification::Get2DData(const char* key, int *nRows, int *nCols, float*** data)
{
	string sk(key);
	*nRows = m_nLayers;
	*nCols = m_size;
    if (StringMatch(sk, "DenLostN"))
		*data = m_denLostN; 
	else
		throw ModelException("Denitrification", "Get2DData", "Output " + sk 
		+ " does not exist in the Denitrification module. Please contact the module developer.");
}

int Denitrification::Execute()
{
	//check the data
	CheckInputData();	

	initalOutputs();

	#pragma omp parallel for
	for(int i=0; i < m_nLayers; i++)
    { 
		for(int j=0; j < m_size; j++)
		{
			float denWF = 1.1f;
			if (m_denWF != NULL)
				denWF = m_denWF[i][j];
			float orgCar = 1.12f;
			if (m_orgCar != NULL)
				orgCar = m_orgCar[i][j];

			if(m_WF[i][j] < denWF)
				m_denLostN[i][j] = 0.0f;
			else
				m_denLostN[i][j] = m_Nitrate[i][j] * (1 - exp(- m_denRC * m_TF[i][j] * orgCar));

	        m_Nitrate[i][j] -= m_denLostN[i][j];
		}
	}

	return 0;
}