// PER_PI.cpp : main project file.
// Revised Time: 2012-03-19
// Authour: Junzhi Liu

#include "Percolation.h"
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


Percolation::Percolation(void)
{
	// set default values for member variables	
	this->m_timestep = -1;
	m_size = -1;

	m_Conductivity = NULL;
	m_Porosity= NULL;
	//m_Residual= NULL;
	m_Poreindex= NULL;
	m_Moisture = NULL;
	//m_ES = NULL;
	m_FieldCapacity = NULL;
	//m_SoilT = NULL;
	//m_ForzenT = -99.0f;

	m_recharge= NULL;
	m_rootDepth = NULL;
	m_dx = -1.f;
}

Percolation::~Percolation(void)
{	
	if(m_recharge == NULL) delete [] m_recharge;		 
}

//Execute module
int Percolation::Execute()
{

	if(m_recharge == NULL) 
	{
		CheckInputData();
		m_recharge = new float[this->m_size];	
	}
	
#pragma omp parallel for
	for(int i = 0; i < m_size; i++)
	{
		//if(this->m_SoilT[i] <= this->m_ForzenT)	//if the soil temperature is lower than tFrozen, then PERC = 0.
		//{
		//	m_recharge[i] = 0.0f;
		//	continue;
		//}

		float moisture = m_Moisture[i] ; 

		//float temp = m_Porosity[i] - m_Residual[i];
		//if(temp <= 0.0f) 
		//	temp = 0.001f;

		m_recharge[i] = 0.f;
		if(moisture > m_FieldCapacity[i])
		{
			// the water exceeds the porosity is added to percolation directly
			if(moisture > m_Porosity[i]) 
			{
				m_recharge[i] += (moisture - m_Porosity[i]) * m_rootDepth[i];
				m_Moisture[i] = m_Porosity[i];
			}

			// recharge capacity (mm)
			float dcIndex = 3.f + 2.f/m_Poreindex[i]; // pore disconnectedness index
			//float rechargeCap = m_Conductivity[i] / 3600.f * m_timestep * pow((moisture - m_Residual[i])/temp, dcIndex); 
			float rechargeCap = m_Conductivity[i] / 3600.f * m_timestep * pow(moisture/m_Porosity[i], dcIndex); //Campbell, 1974
			float availableWater = (m_Moisture[i] - m_FieldCapacity[i])*m_rootDepth[i];
			if (rechargeCap >= availableWater)
				rechargeCap = availableWater;
			
			m_recharge[i] += rechargeCap;
			m_Moisture[i] -= m_recharge[i]/m_rootDepth[i];
		}
	}

	return true;

}

void Percolation::Get1DData(const char* key, int* nRows, float** data)
{
	string s(key);
	if(StringMatch(s,"PERCOLATION"))				
	{
		*data = this->m_recharge;
	}
	else									throw ModelException("Percolation","getResult","Result " + s + " does not exist in Percolation method. Please contact the module developer.");

	*nRows = this->m_size;
}

void Percolation::Set1DData(const char* key, int nRows, float* data)
{
	string s(key);

	this->CheckInputSize(key,nRows);

	if(StringMatch(s,"Conductivity"))		this->m_Conductivity = data;
	else if(StringMatch(s,"Porosity"))		this->m_Porosity = data;
	else if(StringMatch(s,"Poreindex"))		this->m_Poreindex = data;	
	else if(StringMatch(s,"D_SOMO"))		this->m_Moisture = data;
	else if(StringMatch(s,"FieldCap"))		this->m_FieldCapacity = data;	
	else if(StringMatch(s,"RootDepth"))		this->m_rootDepth = data;	
	//else if(StringMatch(s,"D_ES"))			this->m_ES = data;
	//else if(StringMatch(s,"D_SOTE"))		this->m_SoilT = data;
	else									throw ModelException("Percolation","SetValue","Parameter " + s + " does not exist in Percolation method. Please contact the module developer.");

}

void Percolation::SetValue(const char* key, float data)
{
	string s(key);
	if(StringMatch(s,"DT_HS"))			
		this->m_timestep = int(data);
	else if(StringMatch(s,"CellWidth"))			
		m_dx = data;
	//else if(StringMatch(s,"t_soil"))		this->m_ForzenT = data;
	else									
		throw ModelException("Percolation","SetValue","Parameter " + s + " does not exist in Percolation method. Please contact the module developer.");
}


bool Percolation::CheckInputData()
{
	if(this->m_date <=0)				
		throw ModelException("Percolation","CheckInputData","You have not set the time.");
	if(m_size <= 0)					
		throw ModelException("Percolation","CheckInputData","The dimension of the input data can not be less than zero.");
	if(this->m_timestep <=0)			
		throw ModelException("Percolation","CheckInputData","The time step can not be less than zero.");
	if (m_dx < 0)
		throw ModelException("Percolation", "CheckInputData","The parameter CellWidth is not set.");
	if(this->m_Conductivity == NULL)	
		throw ModelException("Percolation","CheckInputData","The Conductivity can not be NULL.");
	if(this->m_Porosity == NULL)		
		throw ModelException("Percolation","CheckInputData","The Porosity can not be NULL.");

	if(this->m_Poreindex == NULL)		
		throw ModelException("Percolation","CheckInputData","The Poreindex can not be NULL.");
	if(this->m_Moisture == NULL)		
		throw ModelException("Percolation","CheckInputData","The Moisture can not be NULL.");
	//if(this->m_SoilT == NULL)			throw ModelException("Percolation","CheckInputData","The soil temerature can not be NULL.");
	//if(this->m_ForzenT == -99.0f)		throw ModelException("Percolation","CheckInputData","The threshold soil freezing temerature can not be NULL.");

	return true;
}

bool Percolation::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException("Percolation","CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(this->m_size != n)
	{
		if(this->m_size <=0) this->m_size = n;
		else
		{
			throw ModelException("Percolation","CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}

	return true;
}
