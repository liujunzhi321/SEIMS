// SOL_WB.cpp : main project file.

#include "SOL_WB.h"
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

SOL_WB::SOL_WB(void):m_nSoilLayers(2), m_upSoilDepth(200.f)
{
	m_Rootdepth = NULL;
	m_Infil=NULL;
	m_ES=NULL;
	m_RI=NULL;
	m_Percolation=NULL;
	m_Revap=NULL;
	m_Precipitation=NULL;
	m_Interception=NULL;
	m_Depression=NULL;
	m_EI=NULL;
	m_ED=NULL;
	m_RS=NULL;
	m_RG=NULL;
	m_SE=NULL;
	m_tMax=NULL;
	m_tMin=NULL;
	m_SoilT=NULL;
	m_pNet = NULL;

	m_sm = NULL;
	m_soilWaterBalance = NULL;
	m_subbasinList = NULL;
	m_subbasin = NULL;
	m_subbasinSelected = NULL;

	m_subbasinSelectedCount = -1;
	m_nCells = -1;

}

SOL_WB::~SOL_WB(void)
{	
	if(m_subbasinList != NULL)
	{
		map<int,subbasin*>::iterator it;
		for(it=m_subbasinList->begin();it!=m_subbasinList->end();it++)
		{
			if(it->second != NULL) delete it->second;
		}
		delete m_subbasinList;
	}

	if(m_soilWaterBalance!=NULL)
	{
		for(int i=0;i<m_subbasinSelectedCount;i++)
		{
			if(m_soilWaterBalance[i] != NULL) delete [] m_soilWaterBalance[i];
		}
		delete [] m_soilWaterBalance;
	}
		 
}


//Execute module
int SOL_WB::Execute()
{	
	CheckInputData();

	if(m_subbasinList == NULL)
	{
		m_subbasinSelectedCount = 1;
		m_subbasinSelected = new float[1];
		m_subbasinSelected[0] = 0;
		getSubbasinList(m_nCells, m_subbasin, m_subbasinSelectedCount, m_subbasinSelected);
		delete m_subbasinSelected;
	}

	return 0;
}

void SOL_WB::SetValue(const char* key, float data)
{
	string s(key);
	if (StringMatch(s, "ThreadNum"))
	{
		omp_set_num_threads((int)data);
	}
	else									
		throw ModelException("SOL_WB","SetValue","Parameter " + s 
		+ " does not exist in SOL_WB method. Please contact the module developer.");

}
void SOL_WB::setValueToSubbasin()
{
	
	if(m_subbasinList != NULL)
	{
		if(m_soilWaterBalance == NULL)
		{
			m_soilWaterBalance = new float*[m_subbasinSelectedCount];
			for(int i=0;i<m_subbasinSelectedCount;i++)
			{
				m_soilWaterBalance[i] = new float[18];
			}
		}

		int index = 0;
		float ri, sm;
		map<int,subbasin*>::iterator it;
		for(it=m_subbasinList->begin();it!=m_subbasinList->end();it++)
		{
			it->second->clear();
			vector<int>* cells = it->second->getCells();
			vector<int>::iterator itCells;
			for(itCells=cells->begin(); itCells<cells->end(); itCells++)
			{
				int cell = *itCells;
				float depth[2];
				depth[0] = m_upSoilDepth;
				depth[1] = m_Rootdepth[cell] - m_upSoilDepth;

				ri = 0.f;
				sm = 0.f;
				for(int i = 0; i < m_nSoilLayers; i++)
				{
					if(m_RI != NULL)
						ri += m_RI[cell][i];
					sm += m_sm[cell][i] * depth[i];
				}
				sm /= m_Rootdepth[cell];
				
				it->second->addP(m_Precipitation[cell]);
				it->second->addNetP(m_pNet[cell]);
				it->second->addDepressionET(m_ED[cell]);
				it->second->addInfiltration(m_Infil[cell]);
				it->second->addInterception(m_Interception[cell]);
				it->second->addInterceptionET(m_EI[cell]);
				it->second->addNetPercolation(m_Percolation[cell][m_nSoilLayers-1] - m_Revap[cell]);
				//it->second->addR(m_RS[cell] + m_RI[cell] + m_RG[cell]);
				it->second->addRevap(m_Revap[cell]);
				//it->second->addRG(m_RG[cell]);

				it->second->addRI(ri);
				it->second->addRS(m_RS[cell]);
				it->second->addSMOI(sm);
				//it->second->addTotalET(m_EI[cell] + m_ED[cell] + m_ES[cell] + m_SE[cell]);
				it->second->addTotalET(m_EI[cell] + m_ED[cell] + m_ES[cell]);
				it->second->addSoilET(m_ES[cell]);
				it->second->addT(m_tMax[cell] / 2 + m_tMin[cell] / 2);
				it->second->addSoilT(m_SoilT[cell]);
				it->second->addMoistureDepth(m_Rootdepth[cell]*sm);
			}

			m_soilWaterBalance[index][0] = it->second->getAverage("P");
			m_soilWaterBalance[index][1] = it->second->getAverage("T");
			m_soilWaterBalance[index][2] = it->second->getAverage("Soil_T");
			m_soilWaterBalance[index][3] = it->second->getAverage("NetP");
			m_soilWaterBalance[index][4] = it->second->getAverage("InterceptionET");
			m_soilWaterBalance[index][5] = it->second->getAverage("DepressionET");
			m_soilWaterBalance[index][6] = it->second->getAverage("Infiltration");
			m_soilWaterBalance[index][7] = it->second->getAverage("Total_ET");
			m_soilWaterBalance[index][8] = it->second->getAverage("Soil_ET");
			m_soilWaterBalance[index][9] = it->second->getAverage("Net_Percolation");
			m_soilWaterBalance[index][10] = it->second->getAverage("Revap");
			m_soilWaterBalance[index][11] = it->second->getAverage("RS");
			m_soilWaterBalance[index][12] = it->second->getAverage("RI");

			int id = it->second->getId();
			if(id >= m_subbasinTotalCount)  
				throw ModelException("SOL_WB","setValueToSubbasin","The groundwater runoff for subbasin " + SOL_WB::toString(float(id)) + " does not exist. Please check the input variable named T_RG from groundwater module.");
			m_soilWaterBalance[index][13] = m_RG[id];
			m_soilWaterBalance[index][14] = m_soilWaterBalance[index][11] + m_soilWaterBalance[index][12] + m_soilWaterBalance[index][13];
			m_soilWaterBalance[index][15] = it->second->getAverage("S_MOI");
			m_soilWaterBalance[index][16] = it->second->getAverage("MoistureDepth");

			index++;
		}
	}
}

void SOL_WB::Set1DData(const char* key, int nRows, float* data)
{
	string s(key);
	if(StringMatch(s,"subbasinSelected"))
	{
		m_subbasinSelected = data;
		m_subbasinSelectedCount = nRows;
		return;
	}

	if(StringMatch(s,"T_RG"))
	{
		m_RG = data;
		m_subbasinTotalCount = nRows;		

		return;
	}

	CheckInputSize(key,nRows);

	if(StringMatch(s,"D_INLO"))				
		m_Interception = data;
	else if(StringMatch(s,"D_P"))			
		m_Precipitation = data;
	else if(StringMatch(s,"D_INET"))		
		m_EI = data;
	else if(StringMatch(s,"D_DPST"))		
		m_Depression = data;
	else if(StringMatch(s,"D_DEET"))		
		m_ED = data;	
	else if(StringMatch(s,"D_INFIL"))
		m_Infil = data;
	else if(StringMatch(s,"D_SOET"))	
		m_ES = data;
	else if(StringMatch(s,"D_Revap"))		
		m_Revap = data;	
	else if(StringMatch(s,"D_SURU"))		
		m_RS = data;
	else if(StringMatch(s,"T_RG"))			
		m_RG = data;
	else if(StringMatch(s,"D_SNSB"))		
		m_SE = data;
	else if(StringMatch(s,"D_TMIN"))		
		m_tMin = data;	
	else if(StringMatch(s,"D_TMAX"))		
		m_tMax = data;	
	else if(StringMatch(s,"D_SOTE"))		
		m_SoilT = data;
	else if(StringMatch(s,"subbasin"))		
		m_subbasin = data;
	else if(StringMatch(s,"Rootdepth"))		
		m_Rootdepth = data;	
	else if(StringMatch(s, "D_NEPR"))	
		m_pNet = data;
	else
		throw ModelException("SOL_WB", "Set1DData", "Parameter " + s + " does not exist in the SOL_WB module.");

}

void SOL_WB::Set2DData(const char* key, int nrows, int ncols, float** data)
{
	string s(key);
	CheckInputSize(key, nrows);
	m_nSoilLayers = ncols;

	if(StringMatch(s,"D_Percolation_2D"))		
		m_Percolation = data;	
	else if(StringMatch(s,"D_SSRU_2D"))		
		m_RI = data;
	else if(StringMatch(s,"D_SOMO_2D"))		
		m_sm = data;	
	else
		throw ModelException("SOL_WB", "Set2DData", "Parameter " + s
								+ " does not exist. Please contact the module developer.");

}

void SOL_WB::Get2DData(const char* key, int* nRows, int* nCols, float*** data)
{
	string s(key);
	if(StringMatch(s,"SOWB"))
	{
		setValueToSubbasin();
		*nRows = m_subbasinSelectedCount;
		*nCols = 17;
		*data = m_soilWaterBalance;
	}
	else throw ModelException("SOL_WB","getResult","Result " + s + " does not exist in SOL_WB method. Please contact the module developer.");
}


string SOL_WB::toString(float value)
{
	char s[20];
	strprintf(s,20,"%f",value);
	return string(s);
}

void SOL_WB::CheckInputData()
{
	//if(m_Date <=0)				throw ModelException("SOL_WB","CheckInputData","You have not set the time.");
	if(m_nCells <= 0)					throw ModelException("SOL_WB","CheckInputData","The dimension of the input data can not be less than zero.");
	if(m_Precipitation == NULL)	throw ModelException("SOL_WB","CheckInputData","The precipitation data can not be NULL.");
	if(m_Depression  == NULL)		throw ModelException("SOL_WB","CheckInputData","The depression data can not be NULL.");
	if(m_ED == NULL)				throw ModelException("SOL_WB","CheckInputData","The evaporation of depression can not be NULL.");
	if(m_EI  == NULL)				throw ModelException("SOL_WB","CheckInputData","The evaporation of interception can not be NULL.");
	if(m_ES  == NULL)				throw ModelException("SOL_WB","CheckInputData","The evaporation of soil can not be NULL.");
	if(m_Infil  == NULL)			throw ModelException("SOL_WB","CheckInputData","The infiltration can not be NULL.");
	if(m_Interception  == NULL)	throw ModelException("SOL_WB","CheckInputData","The interception can not be NULL.");
	if(m_Percolation  == NULL)	throw ModelException("SOL_WB","CheckInputData","The percolation data can not be NULL.");
	if(m_Revap  == NULL)			throw ModelException("SOL_WB","CheckInputData","The revap can not be NULL.");
	if(m_RG  == NULL)				throw ModelException("SOL_WB","CheckInputData","The runoff of groundwater can not be NULL.");
	//if(m_RI  == NULL)				throw ModelException("SOL_WB","CheckInputData","The runoff of subsurface can not be NULL.");
	if(m_Rootdepth == NULL)		throw ModelException("SOL_WB","CheckInputData","The root depth can not be NULL.");
	if(m_RS   == NULL)			throw ModelException("SOL_WB","CheckInputData","The runoff of surface can not be NULL.");
	if(m_subbasin   == NULL)		throw ModelException("SOL_WB","CheckInputData","The subbasion can not be NULL.");
	//if(m_SE  == NULL)				throw ModelException("SOL_WB","CheckInputData","The snow sublimation can not be NULL.");
	if(m_tMin == NULL)			throw ModelException("SOL_WB","CheckInputData","The min temperature can not be NULL.");
	if(m_tMax == NULL)			throw ModelException("SOL_WB","CheckInputData","The max temperature can not be NULL.");
	if(m_SoilT == NULL)			throw ModelException("SOL_WB","CheckInputData","The soil temperature can not be NULL.");
}

bool SOL_WB::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException("SOL_WB","CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(m_nCells != n)
	{
		if(m_nCells <=0) m_nCells = n;
		else
		{
			throw ModelException("SOL_WB","CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}

	return true;
}

void SOL_WB::getSubbasinList(int cellCount, float* subbasinGrid, int subbasinSelectedCount, float* subbasinSelected)
{
	if(m_subbasinList != NULL) return;
	if(subbasinSelected == NULL) return;

	map<int,bool> selected;
	bool isAllNeedStatistc = false;
	for(int i = 0;i < subbasinSelectedCount; i++)
	{
		int subid = int(subbasinSelected[i]);
		selected[subid] = true;
		if(subid == 0) 
			isAllNeedStatistc = true;		
	}
	
	//map<int,subbasin*> list;	
	m_subbasinList = new map<int,subbasin*>();
	if(isAllNeedStatistc) (*m_subbasinList)[0] = new subbasin(0); 
	for(int i = 0;i < cellCount; i++)
	{
		if(isAllNeedStatistc) 
			(*m_subbasinList)[0]->addCell(i);

		int subid = int(subbasinGrid[i]);
		if(!selected[subid]) continue;		

		map<int,subbasin*>::iterator it = (*m_subbasinList).find(subid);
		if(it == (*m_subbasinList).end()) (*m_subbasinList)[subid] = new subbasin(subid);
		(*m_subbasinList)[subid]->addCell(i);
	}
}
