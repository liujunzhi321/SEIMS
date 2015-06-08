/*----------------------------------------------------------------------
*	Purpose: 	Overland routing using 4-point implicit finite difference method
*
*	Created:	Junzhi Liu
*	Date:		23-Febrary-2011
*
*	Revision:
*   Date:
*---------------------------------------------------------------------*/

//#include "vld.h"
#include "InterFlow.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>
#include <cmath>
#include <iostream>

using namespace std;

InterFlow::InterFlow(void):m_size(-1), m_dt(-1.0f), m_dx(-1.0f), m_chWidth(NULL),
	m_s0(NULL), m_rootDepth(NULL), m_ks(NULL), m_landuseFactor(1.f),
	m_soilMoistrue(NULL), m_porosity(NULL), m_poreIndex(NULL),  m_fieldCapacity(NULL),
	m_flowInIndex(NULL), m_routingLayers(NULL), m_nLayers(-1),
	m_q(NULL), m_h(NULL), m_sr(NULL), m_streamLink(NULL), m_hReturnFlow(NULL)
{
	SQ2 = sqrt(2.f);
}


InterFlow::~InterFlow(void)
{
	if (m_h != NULL)
		delete[] m_h;
	if(m_q != NULL)
		delete [] m_q;
	if(m_hReturnFlow != NULL)
		delete[] m_hReturnFlow;

}

bool InterFlow::CheckInputData(void)
{
	if(this->m_date <= 0)
	{
		throw ModelException("INTERFLOW_IKW","CheckInputData","You have not set the Date variable.");
		return false;
	}

	if(this->m_size <= 0)
	{
		throw ModelException("INTERFLOW_IKW","CheckInputData","The cell number of the input can not be less than zero.");
		return false;
	}

	if(this->m_dt <= 0)
	{
		throw ModelException("INTERFLOW_IKW","CheckInputData","You have not set the TimeStep variable.");
		return false;
	}

	if(this->m_dx <= 0)
	{
		throw ModelException("INTERFLOW_IKW","CheckInputData","You have not set the CellWidth variable.");
		return false;
	}

	if(m_chWidth == NULL)
		throw ModelException("IKW_CH","CheckInputData","The parameter: CHWIDTH has not been set.");

	if(m_flowInIndex == NULL)
		throw ModelException("INTERFLOW_IKW","CheckInputData","The parameter: flow in index has not been set.");
	if(m_routingLayers == NULL)
		throw ModelException("INTERFLOW_IKW","CheckInputData","The parameter: routingLayers has not been set.");

	if(m_s0 == NULL)
		throw ModelException("INTERFLOW_IKW","CheckInputData","The parameter: slope has not been set.");	
	if(m_rootDepth == NULL)
		throw ModelException("INTERFLOW_IKW","CheckInputData","The parameter: soil depth has not been set.");
	if(m_ks == NULL)
		throw ModelException("INTERFLOW_IKW","CheckInputData","The parameter: Conductivity has not been set.");

	if(this->m_porosity == NULL)		
		throw ModelException("INTERFLOW_IKW","CheckInputData","The porosity can not be NULL.");
	if(this->m_poreIndex == NULL)		
		throw ModelException("INTERFLOW_IKW","CheckInputData","The pore index can not be NULL.");
	if(this->m_fieldCapacity == NULL)	
		throw ModelException("INTERFLOW_IKW","CheckInputData","The field capacity can not be NULL.");
	if(this->m_soilMoistrue == NULL)	
		throw ModelException("INTERFLOW_IKW","CheckInputData","The soil moistrue can not be NULL.");
	if(this->m_streamLink == NULL)	
		throw ModelException("INTERFLOW_IKW","CheckInputData","The STREAM_LINK can not be NULL.");

	if(this->m_sr == NULL)	
		throw ModelException("INTERFLOW_IKW","CheckInputData","The parameter D_SURU is not set.");

	return true;
}

void  InterFlow::initalOutputs()
{
	if(this->m_size <= 0) 
		throw ModelException("INTERFLOW_IKW","initalOutputs","The cell number of the input can not be less than zero.");

	if(m_q == NULL)
	{
		CheckInputData();	

		m_q = new float[m_size];
		m_h = new float[m_size];
		m_hReturnFlow = new float[m_size];
		for (int i = 0; i < m_size; ++i)
		{
			m_q[i] = 0.0f;	
			m_h[i] = 0.f;
			m_hReturnFlow[i] = 0.f;
		}
	}
}

void InterFlow::FlowInSoil(int id)
{
	//sum the upstream overland flow
	float qUp = 0.0f;
	for (int k = 1; k <= (int)m_flowInIndex[id][0]; ++k)
	{
		int flowInID = (int)m_flowInIndex[id][k];
		if (m_streamLink[id] > 0)
			qUp  += m_q[flowInID];
	}

	float flowWidth = m_dx;
	// there is no land in this cell
	if (m_streamLink[id] > 0)
	{
		flowWidth -= m_chWidth[id];
		if (flowWidth <= 0)
		{
			m_q[id] = qUp;
			m_h[id] = 0.f;
		}
		return;
	}

	// adjust soil moisture
	float s0 =  m_s0[id];
	float soilVolumn = m_rootDepth[id]/1000*m_dx*flowWidth/cos(atan(s0)); //m3
	m_soilMoistrue[id] += qUp * m_dt / soilVolumn;
	
	// the water exceeds the porosity is added to storage (return flow)
	if(m_soilMoistrue[id] > m_porosity[id]) 
	{
		m_hReturnFlow[id] = (m_soilMoistrue[id] - m_porosity[id]) * m_rootDepth[id];
		m_sr[id] += m_hReturnFlow[id];
		m_soilMoistrue[id] = m_porosity[id];
	}

	// if soil moisture is below the field capacity, no interflow will be generated
	if (m_soilMoistrue[id] < m_fieldCapacity[id])
	{
		m_q[id] = 0.f;
		m_h[id] = 0.f;
		return;
	}

	// calculate effective hydraulic conductivity (mm/h -> m/s)
	//float k = m_ks[id]/1000/3600 * pow((m_soilMoistrue[id] - m_residual[id])/(m_porosity[id] - m_residual[id]), m_poreIndex[id]);
	float k = m_ks[id]/1000/3600 * pow(m_soilMoistrue[id]/m_porosity[id], m_poreIndex[id]);
	// calculate interflow (m3/s)
	m_q[id] = m_landuseFactor * m_rootDepth[id]/1000 * s0 * k * m_dx;
	
	// available water
	float availableWater = (m_soilMoistrue[id] - m_fieldCapacity[id]) * soilVolumn;
	float interFlow = m_q[id] * m_dt; // m3
	if (interFlow > availableWater)
	{
		m_q[id] = availableWater / m_dt;
		interFlow = availableWater;
	}
	m_h[id] = 1000 * interFlow / (m_dx*m_dx);

	// adjust soil moisture
	m_soilMoistrue[id] -= interFlow / soilVolumn;
}

int InterFlow::Execute()
{

	initalOutputs();

	for (int iLayer = 0; iLayer < m_nLayers; ++iLayer)
	{
		// There are not any flow relationship within each routing layer.
		// So parallelization can be done here.
		int nCells = (int)m_routingLayers[iLayer][0];
		//omp_set_num_threads(2);
		#pragma omp parallel for
		for (int iCell = 1; iCell <= nCells; ++iCell)
		{
			int id = (int)m_routingLayers[iLayer][iCell];
			FlowInSoil(id);
		}
	}
	return 0;
}

bool InterFlow::CheckInputSize(const char* key, int n)
{
	if(n <= 0)
	{
		//this->StatusMsg("Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(this->m_size != n)
	{
		if(this->m_size <=0) this->m_size = n;
		else
		{
			//this->StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
			ostringstream oss;
			oss << "Input data for "+string(key) << " is invalid with size: " << n << ". The origin size is " << m_size << ".\n";  
			throw ModelException("INTERFLOW_IKW","CheckInputSize",oss.str());
		}
	}

	return true;
}

void InterFlow::SetValue(const char* key, float data)
{
	string sk(key);
	if (StringMatch(sk, "DT_HS"))
		m_dt = data;
	else if (StringMatch(sk, "CellWidth"))
		m_dx = data;
	else if (StringMatch(sk, "Ki"))
		m_landuseFactor = data;
	else if (StringMatch(sk, "ThreadNum"))
	{
		omp_set_num_threads((int)data);
	}
	else
		throw ModelException("INTERFLOW_IKW", "SetSingleData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
	
}

void InterFlow::Set1DData(const char* key, int n, float* data)
{
	//check the input data
	CheckInputSize(key,n);
	string s(key);
	if(StringMatch(s, "Slope"))
		m_s0 = data;
	else if (StringMatch(s, "SoilDepth"))
		m_rootDepth = data;
	else if(StringMatch(s,"Fieldcap"))			
		this->m_fieldCapacity = data;
	else if(StringMatch(s,"Rootdepth"))		
		this->m_rootDepth = data;
	else if(StringMatch(s,"Conductivity"))	
		this->m_ks = data;
	else if(StringMatch(s,"Porosity"))		
		this->m_porosity = data;	
	else if(StringMatch(s,"Poreindex"))		
		this->m_poreIndex = data;	
	else if(StringMatch(s,"D_SOMO"))		
		this->m_soilMoistrue = data;	
	else if(StringMatch(s, "CHWIDTH"))
		m_chWidth = data;
	else if (StringMatch(s,"D_SURU"))
		m_sr = data;
	else if (StringMatch(s,"STREAM_LINK"))
		m_streamLink = data;
	else
		throw ModelException("INTERFLOW_IKW", "Set1DData", "Parameter " + s 
		+ " does not exist. Please contact the module developer.");
	
}

void InterFlow::Get1DData(const char* key, int* n, float** data)
{
	initalOutputs();

	string sk(key);
	*n = m_size;
	if (StringMatch(sk, "QSoil"))
		*data = m_q;
	else if(StringMatch(sk, "ReturnFlow"))
		*data = m_hReturnFlow;
	else
		throw ModelException("INTERFLOW_IKW", "Get1DData", "Output " + sk 
		+ " does not exist in the INTERFLOW_IKW module. Please contact the module developer.");
}

void InterFlow::Set2DData(const char* key, int nrows, int ncols, float** data)
{
	//check the input data
	
	string sk(key);
	if(StringMatch(sk, "Routing_Layers"))
	{
		m_nLayers = nrows;
		m_routingLayers = data;
	}
	else if (StringMatch(sk, "FlowIn_Index_D8"))
		m_flowInIndex = data;
	else
		throw ModelException("INTERFLOW_IKW", "Set2DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
}