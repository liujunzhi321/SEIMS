// SSR_DA.cpp : main project file.

#include <string.h>
#include <math.h>
#include "SSR_DA.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>
#include <stdlib.h>

#include <map>
#include <iostream>

SSR_DA::SSR_DA(void):m_nSoilLayers(2), m_dt(-1), m_nCells(-1), m_nSubbasin(-1), m_frozenT(-99.f), m_ki(-99.f), m_ks(NULL), 
	m_porosity(NULL), m_poreIndex(NULL), m_sm(NULL), m_fc(NULL), m_soilT(NULL), m_rootDepth(NULL), m_slope(NULL), m_qi(NULL), m_qiVol(NULL),
	m_chWidth(NULL), m_streamLink(NULL), m_flowInIndex(NULL), m_flowInPercentage(NULL), m_routingLayers(NULL), m_nRoutingLayers(-1),
	m_subbasin(NULL), m_qiSubbasin(NULL), m_upSoilDepth(200.f)
{
	
}

SSR_DA::~SSR_DA(void)
{	
	if(m_qi != NULL)
	{
		for (int i = 0; i < m_nCells; i++)
			delete[] m_qi[i];
		delete[] m_qi;
	}

	if(m_qiVol != NULL)
	{
		for (int i = 0; i < m_nCells; i++)
			delete[] m_qiVol[i];
		delete[] m_qiVol;
	}

	if(m_qiSubbasin != NULL)
		delete[] m_qiSubbasin;
}

void SSR_DA::FlowInSoil(int id)
{
	float s0 =  max(m_slope[id], 0.01f);

	float depth[2];
	depth[0] = m_upSoilDepth;
	depth[1] = m_rootDepth[id] - m_upSoilDepth;
	if(depth[1] < 0)
	{
		ostringstream oss;
		oss << "The root depth at cell(" << id << ") is " << m_rootDepth[id] << ", and is less than the upper soil depth (" << m_upSoilDepth << endl;
		throw ModelException("SSR_DA", "Execute",  oss.str());
	}

	float flowWidth = m_cellWidth;
	// there is no land in this cell
	if (m_streamLink[id] > 0)
		flowWidth -= m_chWidth[id];

	int nUpstream = (int)m_flowInIndex[id][0];
	//cout << "number of layers:" << m_nSoilLayers << endl;
	for (int j = 0; j < m_nSoilLayers; j++)
	{
        float smOld = m_sm[id][j];
		//sum the upstream overland flow
		float qUp = 0.0f;
		for (int upIndex = 1; upIndex <= nUpstream; ++upIndex)
		{
			int flowInID = (int)m_flowInIndex[id][upIndex];
			//cout << id << "\t" << flowInID << "\t" << m_nCells << endl;

			qUp  += m_qi[flowInID][j];// * m_flowInPercentage[id][upIndex];
			//cout << id << "\t" << flowInID << "\t" << m_nCells << "\t" << m_qi[flowInID][j] << endl;
			//error happens in this place
		}

		if (flowWidth <= 0)
		{
			m_qi[id][j] = 0.f;
			m_qiVol[id][j] = 0.f;
			continue;
		}
		
		// add upstream water to the current cell
		float soilVolumn = depth[j]/1000*m_cellWidth*flowWidth; //m3
		m_sm[id][j] += qUp / soilVolumn;

		//TEST
		if(m_sm[id][j] != m_sm[id][j] || m_sm[id][j] < 0.f)
		{
			ostringstream oss;
			oss << "cell id: " << id << "\t layer: " << j << "\nmoisture is less than zero: " << m_sm[id][j] << "\t" 
				<< smOld <<  "\nqUp: " << qUp << "\t depth:" << depth[j] << "\tupper soil depth:" << m_upSoilDepth << endl;
			throw ModelException("SSR_DA", "Execute:FlowInSoil", oss.str());
		}

		m_qi[id][j] = 0.f; //default return value

		// if soil moisture is below the field capacity, no interflow will be generated
		if (m_sm[id][j] > m_fc[id][j])
		{
			// for the upper two layers, soil may be frozen
			// also check if there are upstream inflow
			if(j == 0 && m_soilT[id] <= m_frozenT && qUp <= 0.f) 
				continue;

			float k, maxSoilWater, soilWater, fcSoilWater;
			//the moisture content can exceed the porosity in the way the algorithm is implemented
			if (m_sm[id][j] > m_porosity[id][j])
				k = m_ks[id][j];
			else
			{
				float dcIndex = 2.f/m_poreIndex[id][j] + 3.f; // pore disconnectedness index
				k = m_ks[id][j] * pow(m_sm[id][j]/m_porosity[id][j], dcIndex);
			}
			m_qi[id][j] = m_ki * s0 * k * m_dt/3600.f * depth[j]/1000.f / flowWidth; // the unit is mm
			
			maxSoilWater = depth[j] * m_porosity[id][j];
			soilWater = depth[j] * m_sm[id][j];
			fcSoilWater = depth[j] * m_fc[id][j];

			if(soilWater - m_qi[id][j] < fcSoilWater)
				m_qi[id][j] = soilWater - fcSoilWater;

			soilWater -= m_qi[id][j];
			if(soilWater > maxSoilWater)
				m_qi[id][j] += soilWater - maxSoilWater;

			m_qiVol[id][j] = m_qi[id][j]/1000*m_cellWidth*flowWidth; //m3
			//Adjust the moisture content in the current layer, and the layer immediately below it
			m_sm[id][j] -= m_qi[id][j]/depth[j];
			
			if(m_sm[id][j] != m_sm[id][j] || m_sm[id][j] < 0.f)
			{
				ostringstream oss;
				oss << id << "\t" << j << "\nmoisture is less than zero: " << m_sm[id][j] << "\t" << depth[j];
				throw ModelException("SSR_DA", "Execute", oss.str());
			}
		}
	}
}

//Execute module
int SSR_DA::Execute()
{	
	CheckInputData();
	initalOutputs();

	if(m_qi == NULL)
	{
		m_qi = new float*[m_nCells];
		m_qiVol = new float*[m_nCells];
		
		for (int i = 0; i < m_nCells; i++)
		{
			m_qi[i] = new float[m_nSoilLayers];
			m_qiVol[i] = new float[m_nSoilLayers];
			for (int j = 0; j < m_nSoilLayers; j++)
			{
				m_qi[i][j] = 0.f;
				m_qiVol[i][j] = 0.f;
			}
		}
	}

	for (int iLayer = 0; iLayer < m_nRoutingLayers; ++iLayer)
	{
		// There are not any flow relationship within each routing layer.
		// So parallelization can be done here.
		int nCells = (int)m_routingLayers[iLayer][0];
		
		#pragma omp parallel for
		for (int iCell = 1; iCell <= nCells; ++iCell)
		{
			int id = (int)m_routingLayers[iLayer][iCell];
			FlowInSoil(id);
		}
	}

	//cout << "end flowinsoil" << endl;
	for(int i = 0; i < m_nSubbasin; i++)
		m_qiSubbasin[i] = 0.f;

    float qiAllLayers = 0.f;
	for(int i = 0; i < m_nCells; i++)
	{
	    if(m_streamLink[i] > 0)
		{
			qiAllLayers = 0.f;
			for(int j = 0; j < m_nSoilLayers; j++)
				qiAllLayers += m_qiVol[j][i];
			//cout << m_nSubbasin << "\tsubbasin:" << m_subbasin[i] << "\t" << qiAllLayers << endl;
			if(m_nSubbasin > 1)
				m_qiSubbasin[int(m_subbasin[i])] += qiAllLayers/86400.f;
			else
				m_qiSubbasin[1] += qiAllLayers/86400.f;
        }
	}
	
	for(int i = 1; i < m_nSubbasin; i++)
		m_qiSubbasin[0] += m_qiSubbasin[i];

	return 0;

}

void SSR_DA::SetValue(const char* key, float data)
{
	string s(key);
	if (StringMatch(s, "ThreadNum"))
		omp_set_num_threads((int)data);
	else if(StringMatch(s,"T_Soil"))				
		m_frozenT = data;
	else if(StringMatch(s,"Ki"))			
		m_ki = data;
	else if(StringMatch(s,"CellWidth"))		
		m_cellWidth = data;
	else if(StringMatch(s,"TimeStep"))		
		m_dt = int(data);
	//else if(StringMatch(s,"UpperSoilDepth"))		
	//s	m_upSoilDepth = data;
	else									
		throw ModelException("SSR_DA","SetValue",
		    "Parameter " + s + " does not exist in SSR_DA method. Please contact the module developer.");
}

void SSR_DA::Set1DData(const char* key, int nRows, float* data)
{
	string s(key);
	CheckInputSize(key,nRows);

	if(StringMatch(s, "Rootdepth"))		
		m_rootDepth = data;
	else if(StringMatch(s, "Slope"))			
		m_slope = data;
	else if(StringMatch(s, "CHWIDTH"))			
		m_chWidth = data;
	else if(StringMatch(s, "Stream_Link"))			
		m_streamLink = data;
	else if(StringMatch(s, "D_SOTE"))		
		m_soilT = data;
	else if (StringMatch(s, "subbasin"))
		m_subbasin = data;
	else									
		throw ModelException("SSR_DA", "SetValue",
		    "Parameter " + s + " does not exist in SSR_DA method. Please contact the module developer.");

}

void SSR_DA::Set2DData(const char* key, int nrows, int ncols, float** data)
{
	string sk(key);

	if(StringMatch(sk,"Conductivity_2D"))
	{
		CheckInputSize(key, nrows);
		m_nSoilLayers = ncols;
		m_ks = data;
	}
	else if (StringMatch(sk, "Porosity_2D"))
	{
		CheckInputSize(key, nrows);
		m_nSoilLayers = ncols;
		m_porosity = data;
	}
	else if (StringMatch(sk, "FieldCap_2D"))
	{
		CheckInputSize(key, nrows);
		m_nSoilLayers = ncols;
		m_fc = data;
	}
	else if(StringMatch(sk, "Poreindex_2D"))		
	{
		CheckInputSize(key, nrows);
		m_nSoilLayers = ncols;
		m_poreIndex = data;	
	}
	else if(StringMatch(sk, "D_SOMO_2D"))		
	{
		CheckInputSize(key, nrows);
		m_nSoilLayers = ncols;
		m_sm = data;
	}
	else if(StringMatch(sk, "ROUTING_LAYERS"))
	{
		m_nRoutingLayers = nrows;
		m_routingLayers = data;
	}
	else if (StringMatch(sk, "FLOWIN_INDEX_D8"))
	{
		CheckInputSize(key, nrows);
		m_flowInIndex = data;
	}
	else if (StringMatch(sk, "FLOWIN_PERCENTAGE_D8"))
	{
		CheckInputSize(key, nrows);
		m_flowInPercentage = data;
	}
	else
		throw ModelException("SSR_DA", "Set2DData", 
		    "Parameter " + sk + " does not exist. Please contact the module developer.");
}


void SSR_DA::Get1DData(const char* key, int* n, float** data)
{
	string sk(key);
	if (StringMatch(sk, "SBIF"))  
		*data = m_qiSubbasin;
	else									
		throw ModelException("SSR_DA", "Get1DData", "Result " + sk + " does not exist in Get1DData method.");

	*n = m_nSubbasin + 1;
}


void SSR_DA::Get2DData(const char* key, int *nRows, int *nCols, float*** data)
{
	string sk(key);
	*nRows = m_nCells;
	*nCols = m_nSoilLayers;

	if (StringMatch(sk, "SSRU_2D"))   // excess precipitation
	{
		*data = m_qi;
	}
	else if (StringMatch(sk, "SSRUVOL_2D"))   // excess precipitation
	{
		*data = m_qiVol;
	}
	else
		throw ModelException("SSR_DA", "Get2DData", "Output " + sk 
		                         + " does not exist. Please contact the module developer.");

}

bool SSR_DA::CheckInputData()
{
	if(m_nCells <= 0)					
		throw ModelException("SSR_DA","CheckInputData","The dimension of the input data can not be less than zero.");

	if(m_ki <= 0)						
		throw ModelException("SSR_DA","CheckInputData","You have not set Ki.");
	if(m_frozenT <= -99.0f)					
		throw ModelException("SSR_DA","CheckInputData","You have not set frozen T.");
	if(m_dt <= 0)					
		throw ModelException("SSR_DA","CheckInputData","You have not set time step.");
	if(m_cellWidth <= 0)				
		throw ModelException("SSR_DA","CheckInputData","You have not set cell width.");
	if(m_subbasin == NULL)		
		throw ModelException("SSR_DA","CheckInputData","The parameter: subbasin can not be NULL.");
	if(m_rootDepth == NULL)		
		throw ModelException("SSR_DA","CheckInputData","The root depth can not be NULL.");
	if(m_slope == NULL)			
		throw ModelException("SSR_DA","CheckInputData","The slope can not be NULL.");
	if(m_ks == NULL)	
		throw ModelException("SSR_DA","CheckInputData","The conductivity can not be NULL.");
	if(m_porosity == NULL)		
		throw ModelException("SSR_DA","CheckInputData","The porosity can not be NULL.");
	if(m_poreIndex == NULL)		
		throw ModelException("SSR_DA","CheckInputData","The pore index can not be NULL.");
	if(m_fc == NULL)	
		throw ModelException("SSR_DA","CheckInputData","The field capacity can not be NULL.");
	if(m_sm == NULL)	
		throw ModelException("SSR_DA","CheckInputData","The soil moistrue can not be NULL.");
	if(m_soilT == NULL)			
		throw ModelException("SSR_DA","CheckInputData","The soil temperature can not be NULL.");

	if(m_chWidth == NULL)			
		throw ModelException("SSR_DA","CheckInputData","The channel width can not be NULL.");
	if(m_streamLink == NULL)			
		throw ModelException("SSR_DA","CheckInputData","The stream link can not be NULL.");

	if(m_flowInIndex == NULL)			
		throw ModelException("SSR_DA","CheckInputData","The flow in index can not be NULL.");
	//if(m_flowInPercentage == NULL)			
	//	throw ModelException("SSR_DA","CheckInputData","The flow in percentage can not be NULL.");
	if(m_routingLayers == NULL)			
		throw ModelException("SSR_DA","CheckInputData","The routing layers can not be NULL.");
	if(m_nRoutingLayers < 0)		
		throw ModelException("SSR_DA","CheckInputData","The nubmer of routing layers can not be NULL.");

	return true;
}

void SSR_DA::initalOutputs()
{
	if(m_nSubbasin <= 0)
	{
		map<int,int> subs;
		for(int i=0; i < m_nCells; i++)
		{
			subs[int(m_subbasin[i])] = 1;
		}
		m_nSubbasin = subs.size();
	}

	if(m_qiSubbasin == NULL) 
	{
		m_qiSubbasin = new float[m_nSubbasin+1];
		for (int i = 0; i <= m_nSubbasin; i++)
		{
			m_qiSubbasin[i] = 0.f;
		}
	}
}

bool SSR_DA::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException("SSR_DA","CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(this->m_nCells != n)
	{
		if(this->m_nCells <=0) this->m_nCells = n;
		else
		{
			throw ModelException("SSR_DA","CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}

	return true;
}
