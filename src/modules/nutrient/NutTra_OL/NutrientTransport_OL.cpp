#include "NutrientTransport_OL.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>
#include <cmath>
#include <iostream>
#include <map>

using namespace std;

NutrientTransport_OL::NutrientTransport_OL(void):m_nLayers(3), m_size(-1), m_aniExcF(0.2f), m_nitPC(0.26f), m_Nitrate(NULL), m_porosity(NULL),
	 m_sr(NULL), m_ssr(NULL), m_perc(NULL),                                             // nitrate input
	 m_clayFrac(NULL), m_Ammon(NULL),    // Ammonium input
	 m_parCoeP(165.f), m_phoPC(15.f), m_SoluP(NULL),                  // soluble P input
	 m_cellWidth(-1), m_Depth(NULL), m_sedimentYield(NULL), m_Density(NULL), m_ActOrgN(NULL), m_StaOrgN(NULL), m_FreOrgN(NULL),   // organic N input
	 m_actMinP(NULL), m_staMinP(NULL), m_humOrgP(NULL), m_FreOrgP(NULL),    // Attached P input
	 m_chWidth(NULL), m_streamLink(NULL), m_flowInIndex(NULL), m_routingLayers(NULL), m_nRoutingLayers(-1),   // cell to cell
	 m_nsub(-1), m_subbasin(NULL),  // to channel input
	 m_satW(NULL), m_mobQ(NULL), m_conNit(NULL), m_surNit(NULL), m_latNit(NULL), m_perNit(NULL),  // nitrate output
	 m_surAmm(NULL), m_conAmm(NULL),      // Ammonium output
	 m_conSed(NULL), m_enrN(NULL), m_conOrgN(NULL), m_traOrgN(NULL),   // organic N output
	 m_surSolP(NULL), m_perPho(NULL),   // soluble P output
	 m_enrP(NULL), m_conAttP(NULL), m_traAttP(NULL),    // Attached P output
	 m_sNittoCh(NULL), m_sNittoCh_T(0.f), m_sAmmtoCh(NULL), m_sAmmtoCh_T(0.f), m_sSolPtoCh(NULL), m_sSolPtoCh_T(0.f), m_orgNtoCh(NULL), m_orgNtoCh_T(0.f), 
	 m_attPtoCh(NULL), m_attPtoCh_T(0.f), m_laNittoCh(NULL), m_laNittoCh_T(0.f)   // to channel output
{

}

NutrientTransport_OL::~NutrientTransport_OL(void)
{
	if(m_satW != NULL)
	{
		for(int i=0; i < m_nLayers; i++)
			delete [] m_satW;
		delete [] m_satW;
	}
	if(m_mobQ != NULL)
	{
		for(int i=0; i < m_nLayers; i++)
			delete [] m_mobQ;
		delete [] m_mobQ;
	}
	if(m_conNit != NULL)
	{
		for(int i=0; i < m_nLayers; i++)
			delete [] m_conNit;
		delete [] m_conNit;
	}
	if(m_surNit != NULL)
	{
		delete [] m_surNit;
	}
	if(m_latNit != NULL)
	{
		for(int i=0; i < m_nLayers; i++)
			delete [] m_latNit;
		delete [] m_latNit;
	}
	if(m_perNit != NULL)
	{
		for(int i=0; i < m_nLayers; i++)
			delete [] m_perNit;
		delete [] m_perNit;
	}
	if(m_conAmm != NULL)
	{
		for(int i=0; i < m_nLayers; i++)
			delete [] m_conAmm;
		delete [] m_conAmm;
	}
	if(m_surAmm != NULL)
	{
		delete [] m_surAmm;
	}
	if(m_conSed != NULL)
	{
		delete [] m_conSed;
	}
	if(m_enrN != NULL)
	{
		delete [] m_enrN;
	}
	if(m_conOrgN != NULL)
	{
		delete [] m_conOrgN;
	}
	if(m_traOrgN != NULL)
	{
		delete [] m_traOrgN;
	}
	if(m_surSolP != NULL)
	{
		delete [] m_surSolP;
	}
	if(m_perPho != NULL)
	{
		delete [] m_perPho;
	}
	if(m_enrP != NULL)
	{
		delete [] m_enrP;
	}
	if(m_conAttP != NULL)
	{
		delete [] m_conAttP;
	}
	if(m_traAttP != NULL)
	{
		delete [] m_traAttP;
	}
	if(m_sNittoCh != NULL)
	{
		delete [] m_sNittoCh;
	}
	if(m_sAmmtoCh != NULL)
	{
		delete [] m_sAmmtoCh;
	}
	if(m_sSolPtoCh != NULL)
	{
		delete [] m_sSolPtoCh;
	}
	if(m_orgNtoCh != NULL)
	{
		delete [] m_orgNtoCh;
	}
	if(m_attPtoCh != NULL)
	{
		delete [] m_attPtoCh;
	}
	if(m_laNittoCh != NULL)
	{
		delete [] m_laNittoCh;
	}
	
}

bool NutrientTransport_OL::CheckInputData(void)
{
	if(m_size <= 0)
	{
		throw ModelException("NutrientTransport_OL","CheckInputData","The cell number of the input can not be less than zero.");
		return false;
	}
	if(m_parCoeP <= 0)
	{
		throw ModelException("NutrientTransport_OL","CheckInputData","The phosphorus percolation coefficient can not be less than zero.");
		return false;
	}
	if(m_phoPC <= 0)
	{
		throw ModelException("NutrientTransport_OL","CheckInputData","The phosphorus soil partitioning coefficient can not be less than zero.");
		return false;
	}
	if(m_Nitrate == NULL)
	{
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the amount of nitrate in layer ly.");
		return false;
	}
	if(m_Ammon == NULL)
	{
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the amount of ammonium in layer ly.");
		return false;
	}
	if(m_porosity == NULL)
	{
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the soil porosity of the soil layer.");
		return false;
	}
	if(m_clayFrac == NULL)
	{
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the clay fraction of the soil layer.");
		return false;
	}
	if(m_sr == NULL)
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the surface runoff generated on a given day.");
	if(m_ssr == NULL)
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the water discharged from the layer by lateral flow.");
	if(m_perc == NULL)
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the amount of water percolating to the underlying soil layer on a given day.");
	
	if(m_cellWidth <= 0)
	{
		throw ModelException("NutrientTransport_OL","CheckInputData","The width (length) of cell can not be less than zero.");
		return false;
	}
	if(m_Depth == NULL)
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the depth of the Layer.");
	if(m_sedimentYield == NULL)
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the sediment yield on a given day.");
	if(m_Density == NULL)
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the soil density.");
	if(m_ActOrgN == NULL)
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the amount of nitrogen in the active organic pool in layer ly.");
	if(m_StaOrgN == NULL)
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the amount of nitrogen in the stable organic pool.");
	if(m_FreOrgN == NULL)
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the amount of nitrogen in the fresh organic pool in layer ly.");
    if(m_SoluP == NULL)
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the amount of soluble phosphorus lost in surface runoff.");
	if(m_actMinP == NULL)
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the amount of phosphorus in the active mineral pool.");
	if(m_staMinP == NULL)
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the amount of phosphorus in the stable mineral pool.");
	if(m_humOrgP == NULL)
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the concentration of humic organic phosphorus in the layer.");
	if(m_FreOrgP == NULL)
		throw ModelException("NutrientTransport_OL","CheckInputData","You have not set the phosphorus in the fresh organic pool in layer ly.");
	if (m_subbasin == NULL)
	{
		throw ModelException("NutrientTransport_OL","CheckInputData","The parameter: m_subbasin has not been set.");
		return false;
	}

	return true;
}

void  NutrientTransport_OL::initalOutputs()
{
	if(m_size <= 0) throw ModelException("NutrientTransport_OL","initalOutputs","The cell number of the input can not be less than zero.");
	if(m_nsub <= 0)
	{
		map<int,int> subs;
		for(int i=0;i<this->m_size;i++)
		{
			subs[int(this->m_subbasin[i])] += 1;
		}
		this->m_nsub = subs.size();
	}

	m_sNittoCh_T = 0.0f;  // for every time step
	m_sAmmtoCh_T = 0.0f;
	m_sSolPtoCh_T = 0.0f;
	m_orgNtoCh_T = 0.0f;  // for every time step
	m_attPtoCh_T = 0.0f;
	m_laNittoCh_T = 0.0f;
	if(this->m_sNittoCh == NULL) 
	{
		m_sNittoCh = new float[m_nsub+1];
		m_sAmmtoCh = new float[m_nsub+1];
		m_sSolPtoCh = new float[m_nsub+1];
		m_orgNtoCh = new float[m_nsub+1];
		m_attPtoCh = new float[m_nsub+1];
		m_laNittoCh = new float[m_nsub+1];
		for (int i = 0; i <= m_nsub; i++)
		{
			m_sNittoCh[i] = 0.f;
			m_sAmmtoCh[i] = 0.f;
			m_sSolPtoCh[i] = 0.f;
			m_orgNtoCh[i] = 0.f;
			m_attPtoCh[i] = 0.f;
			m_laNittoCh[i] = 0.f;			
		}
	}

	if(m_mobQ == NULL)
	{
		m_satW = new float*[m_nLayers];
		m_mobQ = new float*[m_nLayers];
		m_conNit = new float*[m_nLayers];		
		m_latNit = new float*[m_nLayers];
		m_perNit = new float*[m_nLayers];
		m_conAmm = new float*[m_nLayers];

		m_surNit = new float[m_size];
		m_surAmm = new float[m_size];
		m_conSed = new float[m_size];
		m_enrN = new float[m_size];
		m_conOrgN = new float[m_size];
		m_traOrgN = new float[m_size];

		m_surSolP = new float[m_size];
		m_perPho = new float[m_size];

		m_enrP = new float[m_size];
		m_conAttP = new float[m_size];
		m_traAttP = new float[m_size];

	    #pragma omp parallel for
	    	for (int i = 0; i < m_size; ++i)
		    {
			    m_surNit[i] = 0.0f;
				m_surAmm[i] = 0.0f;
				m_conSed[i] = 0.0f;
			    m_enrN[i] = 0.0f;
			    m_conOrgN[i] = 0.0f;
				m_traOrgN[i] = 0.0f;
				m_surSolP[i] = 0.0f;
				m_perPho[i] = 0.0f;
			    m_enrP[i] = 0.0f;
				m_conAttP[i] = 0.0f;
				m_traAttP[i] = 0.0f;
		    }
	
	    #pragma omp parallel for
		    for (int i = 0; i < m_nLayers; ++i)
		    {
			    m_mobQ[i] = new float[m_size];
				m_satW[i] = new float[m_size];
			    m_conNit[i] = new float[m_size];
				m_conAmm[i] = new float[m_size];
			    m_latNit[i] = new float[m_size];
			    m_perNit[i] = new float[m_size];

			    for (int j = 0; j < m_size; ++j)
			    {
			    	m_mobQ[i][j] = 0.0f;
					m_satW[i][j] = 0.0f;
			        m_conNit[i][j] = 0.0f;	
					m_conAmm[i][j] = 0.0f;	
			        m_latNit[i][j] = 0.0f;
			        m_perNit[i][j] = 0.0f;

			    }
		    }
	}
}

bool NutrientTransport_OL::CheckInputSize(const char* key, int n)
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
			throw ModelException("NutrientTransport_OL","CheckInputSize",oss.str());
		}
	}

	return true;
}

void NutrientTransport_OL::SetValue(const char* key, float data)
{
	string sk(key);
	if (StringMatch(sk, "AniExcF"))
		m_aniExcF = data;	
	else if (StringMatch(sk, "NitPC"))
		m_nitPC = data;
	else if (StringMatch(sk, "CellWidth"))
		m_cellWidth = data;
	else if (StringMatch(sk, "ParCoeP"))
		m_parCoeP = data;
	else if (StringMatch(sk, "PhoPC"))
		m_phoPC = data;
	else if (StringMatch(sk, "ThreadNum"))
	{
		omp_set_num_threads((int)data);
	}
	else
		throw ModelException("NutrientTransport_OL", "SetSingleData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
	
}

void NutrientTransport_OL::GetValue(const char* key, float* value)
{
	string s(key);								
	if(StringMatch(s,"SNITTOCH_T"))				
	{
		*value = m_sNittoCh_T; 
	}
	else if(StringMatch(s,"SAMMTOCH_T"))				
	{
		*value = m_sAmmtoCh_T; 
	}
	else if(StringMatch(s,"SSOLPTOCH_T"))				
	{
		*value = m_sSolPtoCh_T; 
	}
	else if(StringMatch(s,"ORGNTOCH_T"))				
	{
		*value = m_orgNtoCh_T; 
	}
	else if(StringMatch(s,"ATTPTOCH_T"))				
	{
		*value = m_attPtoCh_T; 
	}
	else if(StringMatch(s,"LANITTOCH_T"))				
	{
		*value = m_laNittoCh_T; 
	}
	else			
		throw ModelException("NutrientTransport_OL","getResult","Result " + s + " does not exist in NutrientTransport_OL method. Please contact the module developer.");
}


void NutrientTransport_OL::Set1DData(const char* key, int n, float* data)
{
	//check the input data
	CheckInputSize(key,n);
	string sk(key);
	if (StringMatch(sk, "D_SURU"))
		m_sr = data;
	//else if (StringMatch(sk, "Clay"))
	//	m_clayFrac = data;
	else if (StringMatch(sk, "D_SOER"))
		m_sedimentYield = data;
	else if(StringMatch(sk, "CHWIDTH"))			
		m_chWidth = data;
	else if(StringMatch(sk, "STREAM_LINK"))			
		m_streamLink = data;
	else if(StringMatch(sk, "subbasin"))			
		m_subbasin = data;
	else
		throw ModelException("NutrientTransport_OL", "Set1DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
}

void NutrientTransport_OL::Get1DData(const char* key, int* n, float** data)
{
	initalOutputs();

	string sk(key);
	*n = m_size;
	if (StringMatch(sk, "SurNit"))
		*data = m_surNit; 
	else if (StringMatch(sk, "SurAmm"))
		*data = m_surAmm; 
	else if (StringMatch(sk, "ConSed"))
		*data = m_conSed; 
	else if (StringMatch(sk, "EnrN"))
		*data = m_enrN;
	else if (StringMatch(sk, "ConOrgN"))
		*data = m_conOrgN;
	else if (StringMatch(sk, "TraOrgN"))
		*data = m_traOrgN;
	else if (StringMatch(sk, "SurSolP"))
		*data = m_surSolP;
	else if (StringMatch(sk, "PerPho"))
		*data = m_perPho;
	else if (StringMatch(sk, "EnrP"))
		*data = m_enrP;
	else if (StringMatch(sk, "ConAttP"))
		*data = m_conAttP;
	else if (StringMatch(sk, "TraAttP"))
		*data = m_traAttP;
	else if (StringMatch(sk, "SNITTOCH"))
	{	*data = m_sNittoCh;
	    *n = m_nsub+1;
	}
	else if (StringMatch(sk, "SAMMTOCH"))
	{	*data = m_sNittoCh;
	    *n = m_nsub+1;
	}	
	else if (StringMatch(sk, "SSOLPTOCH"))
	{	*data = m_sSolPtoCh;
	    *n = m_nsub+1;
	}	
	else if (StringMatch(sk, "ORGNTOCH"))
	{	*data = m_orgNtoCh;
	    *n = m_nsub+1;
	}	
	else if (StringMatch(sk, "ATTPTOCH"))
	{	*data = m_attPtoCh;
	    *n = m_nsub+1;
	}
	else if (StringMatch(sk, "LANITTOCH"))
	{	*data = m_laNittoCh;
	    *n = m_nsub+1;
	}	
	else
		throw ModelException("NutrientTransport_OL", "Get1DData", "Output " + sk 
		+ " does not exist in the NutrientTransport_OL module. Please contact the module developer.");
}

void NutrientTransport_OL::Set2DData(const char* key, int nrows, int ncols, float** data)
{
	//check the input data
	//CheckInputSize(key, ncols);
	//int nsoilLayers = max(0, m_nLayers -1);

	string sk(key); 

	if(StringMatch(sk, "D_Nitrate"))
	{
		CheckInputSize(key, ncols);
		m_nLayers = nrows;
		m_Nitrate = data;
	}
	else if(StringMatch(sk,"D_Ammon"))		
	{
		CheckInputSize(key, ncols);
		m_nLayers = nrows;
		m_Ammon = data;
	}	
	else if(StringMatch(sk,"Porosity_2D"))		
	{
		CheckInputSize(key, ncols);
		m_nLayers = nrows;
		m_porosity = data;
	}
	else if(StringMatch(sk,"Clay_2D"))
	{
		CheckInputSize(key,ncols);
		m_nLayers = nrows;
		m_clayFrac = data;
	}
	else if(StringMatch(sk, "D_SSRU_2D"))
	{
		CheckInputSize(key, ncols);
		m_nLayers = nrows;
		m_ssr = data;
	}	
	else if(StringMatch(sk, "D_Percolation_2D"))
	{
		CheckInputSize(key, ncols);
		m_nLayers = nrows;
		m_perc = data;
	}
	else if(StringMatch(sk, "Density_2D"))
	{
		CheckInputSize(key, ncols);
		m_nLayers = nrows;
		m_Density = data;
	}	
	else if(StringMatch(sk, "D_Depth"))
	{
		CheckInputSize(key, ncols);
		m_nLayers = nrows;
		m_Depth = data;
	}
	else if (StringMatch(sk, "D_ActOrgN"))
	{
		CheckInputSize(key, ncols);
		m_nLayers = nrows;
		m_ActOrgN = data;
	}	
	else if (StringMatch(sk, "D_StaOrgN"))
	{
		CheckInputSize(key, ncols);
		m_nLayers = nrows;
		m_StaOrgN = data;
	}	
	else if (StringMatch(sk, "D_FreOrgN"))
	{
		CheckInputSize(key, ncols);
		m_nLayers = nrows;
		m_FreOrgN = data;
	}	
	else if (StringMatch(sk, "D_SoluP"))
	{
		CheckInputSize(key, ncols);
		m_nLayers = nrows;
		m_SoluP = data;
	}
	else if (StringMatch(sk, "D_ActMinP"))
	{
		CheckInputSize(key, ncols);
		m_nLayers = nrows;
		m_actMinP = data;
	}	
	else if (StringMatch(sk, "D_StaMinP"))
	{
		CheckInputSize(key, ncols);
		m_nLayers = nrows;
		m_staMinP = data;
	}
	else if (StringMatch(sk, "D_HumOrgP"))
	{
		CheckInputSize(key, ncols);
		m_nLayers = nrows;
		m_humOrgP = data;
	}	
	else if (StringMatch(sk, "D_FreOrgP"))
	{
		CheckInputSize(key, ncols);
		m_nLayers = nrows;
		m_FreOrgP = data;
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
	else
		throw ModelException("NutrientTransport_OL", "Set2DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
}

void NutrientTransport_OL::Get2DData(const char* key, int *nRows, int *nCols, float*** data)
{
	string sk(key);
	*nRows = m_nLayers;
	*nCols = m_size;
    if (StringMatch(sk, "MobQ"))
		*data = m_mobQ; 
	else if (StringMatch(sk, "SatW"))
		*data = m_satW;
	else if (StringMatch(sk, "ConNit"))
		*data = m_conNit; 
	else if (StringMatch(sk, "LatNit"))
		*data = m_latNit; 
	else if (StringMatch(sk, "PerNit"))
		*data = m_perNit; 
	else if (StringMatch(sk, "ConAmm"))
		*data = m_conAmm; 
	else
		throw ModelException("NutrientTransport_OL", "Get2DData", "Output " + sk 
		+ " does not exist in the NutrientTransport_OL module. Please contact the module developer.");
}

void NutrientTransport_OL::FlowInSoil(int id)
{
	float flowWidth = m_cellWidth;
	// there is no land in this cell
	if (m_streamLink[id] > 0)
		flowWidth -= m_chWidth[id];

	int nUpstream = (int)m_flowInIndex[id][0];
	//cout << "number of layers:" << m_nSoilLayers << endl;
	for (int i = 0; i < m_nLayers; i++)
	{
        float nitOld = m_Nitrate[i][id];
		//sum the upstream overland flow
		float lnUp = 0.0f;
		for (int upIndex = 1; upIndex <= nUpstream; ++upIndex)
		{
			int flowInID = (int)m_flowInIndex[id][upIndex];
			//cout << id << "\t" << flowInID << "\t" << m_nCells << endl;

			lnUp  += m_latNit[i][flowInID];// * m_flowInPercentage[id][upIndex];
			//cout << id << "\t" << flowInID << "\t" << m_nCells << "\t" << m_qi[flowInID][j] << endl;
			//error happens in this place
		}

		if (flowWidth <= 0)
		{
			m_latNit[i][id] = 0.f;
			continue;
		}
		
		// add upstream nutrient to the current cell
	    m_Nitrate[i][id] += lnUp ;
	}
}

int NutrientTransport_OL::Execute()
{
	//check the data
	CheckInputData();	

	initalOutputs();

	int area = m_cellWidth * m_cellWidth / 10000;

	// calculate amount of nutrient for transpotation in each cell 
	#pragma omp parallel for
	for(int i=0; i < m_nLayers; i++)
	{
	    for(int j=0; j < m_size; j++)
        { 
			int hydroIndex = max(0, i-1);
			float sr = 0.f;
			if (i == 0)
				sr = m_sr[j];
			else
				sr = 0.f;
			
            m_mobQ[i][j] = sr + m_ssr[hydroIndex][j] + m_perc[hydroIndex][j] + 1.e-10;

		    m_satW[i][j] = m_porosity[hydroIndex][j] * m_Depth[i][j];
			float r = (-m_mobQ[i][j]) / ((1-m_aniExcF) * m_satW[i][j]);
			float conNit = 0.f;
			conNit = m_Nitrate[i][j] * (1- exp(r)) / m_mobQ[i][j];
			m_conNit[i][j] = max(conNit,0.f); 
			float surNit = 0.f;
			surNit = m_nitPC * m_conNit[0][j] * m_sr[j];
			m_surNit[j] = min(surNit, m_Nitrate[0][j]);
			m_Nitrate[0][j] -= m_surNit[j];

			float nitPC = 0.f;
			if (i == 0)
				nitPC = m_nitPC;
			else
				nitPC = 1.f;
			float latNit = m_nitPC * m_conNit[i][j] * m_ssr[hydroIndex][j];
			m_latNit[i][j] = min(latNit, m_Nitrate[i][j]);
			m_Nitrate[i][j] -= m_latNit[0][j];

			float perNit = m_conNit[i][j] * m_perc[hydroIndex][j];
			m_perNit[i][j] = min(perNit,m_Nitrate[i][j]);
			m_Nitrate[i][j] -= m_perNit[i][j];
			if(i < m_nLayers-1)
			    m_Nitrate[i+1][j] += m_perNit[i][j];

			float K_NH4 = 0.083 * m_clayFrac[0][j] + 1.34f;   //! Knisel et al. 1994
			float pk = m_porosity[hydroIndex][j] + 2.65f * (1 - m_porosity[hydroIndex][j]) * K_NH4;
			float c_Amm = m_Ammon[i][j] * exp( - m_mobQ[i][j] * area * 0.001 / pk );

			float coe_extra = 0.f;   //extraction coefficient
	        if(K_NH4 <=1.f) 
				coe_extra = 0.5f;
			else if(K_NH4>1.f && K_NH4 <=10.f)
				coe_extra = 0.598f * exp(-0.179f * K_NH4);
			else
				coe_extra = 0.1f;
			float conAmm =  c_Amm * coe_extra / ((1 + K_NH4 * coe_extra) * m_mobQ[i][j]);
			m_conAmm[i][j] = max(conAmm, 0.f);
			float surAmm = m_conAmm[0][j] * m_sr[j];
			m_surAmm[j] = min(surAmm, m_Ammon[0][j]);
			m_Ammon[0][j] -= m_surAmm[j];

		    if ( m_sr[j] > 0.f)
				m_conSed[j] = m_sedimentYield[j] / (10.f * area * m_sr[j]);
			else
				m_conSed[j] = 0.f;
		    m_enrN[j] = m_enrP[j] = 0.78f * pow (m_conSed[j], -0.2468f);

			float orgN = m_FreOrgN[0][j] + m_ActOrgN[0][j] + m_StaOrgN[0][j];
			m_conOrgN[j] = 100.f * orgN / (m_Density[0][j] * m_Depth[0][j]);
			m_traOrgN[j] = 0.001f * m_conOrgN[j] * m_enrN[j] * m_sedimentYield[j] / area;

			m_surSolP[j] = m_SoluP[0][j] * m_sr[j] / (m_Density[0][j] * m_Depth[0][j] * m_parCoeP);
			m_perPho[j] = m_SoluP[0][j] * m_perc[0][j] / (10.f * m_Density[0][j] * m_Depth[0][j] * m_phoPC);

			m_SoluP[0][j] -= m_surSolP[j] + m_perPho[j];
			m_SoluP[1][j] += m_perPho[j];

			float p = m_actMinP[0][j] + m_staMinP[0][j] + m_humOrgP[0][j] + m_FreOrgP[0][j];
			m_conAttP[j] = 100.f * p / (m_Density[0][j] * m_Depth[0][j]);
			m_traAttP[j] = 0.001f * m_conAttP[j] * m_enrP[j] * m_sedimentYield[j] / area;

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

	for(int i = 0; i < m_nsub; i++)
		m_laNittoCh[i] = 0.f;

    float laNitAllLayers = 0.f;
	for(int i = 0; i < m_size; i++)
	{
	    if(m_streamLink[i] > 0)
		{
			laNitAllLayers = 0.f;
			for(int j = 0; j < m_nLayers; j++)
				laNitAllLayers += m_latNit[j][i];
			//cout << m_nSubbasin << "\tsubbasin:" << m_subbasin[i] << "\t" << qiAllLayers << endl;
			if(m_nsub > 1)
				m_laNittoCh[int(m_subbasin[i])] += laNitAllLayers * area;
			else
				m_laNittoCh[1] += laNitAllLayers * area;
        }
		//m_laNittoCh_T += m_laNittoCh[i];
	}

	for(int i = 0; i < m_nsub; i++)
		m_laNittoCh_T += m_laNittoCh[i];

	for(int i=0;i<m_size;i++)
	{
		// add each subbasin's sediment yield to channel
		if(m_nsub > 1)
		{
			m_sNittoCh[int(m_subbasin[i])] += m_surNit[i] * area;
			m_sAmmtoCh[int(m_subbasin[i])] += m_surAmm[i] * area;
		    m_sSolPtoCh[int(m_subbasin[i])] += m_surSolP[i] * area;
			m_orgNtoCh[int(m_subbasin[i])] += m_traOrgN[i] * area;
		    m_attPtoCh[int(m_subbasin[i])] += m_traAttP[i] * area;
		}
		else
		{	
			m_sNittoCh[1] += m_surNit[i] * area;
			m_sAmmtoCh[1] += m_surAmm[i] * area;
		    m_sSolPtoCh[1] += m_surSolP[i] * area;
			m_orgNtoCh[1] += m_traOrgN[i] * area;
		    m_attPtoCh[1] += m_traAttP[i] * area;
		}	
		m_sNittoCh_T += m_surNit[i] * area;
		m_sAmmtoCh_T += m_surAmm[i] * area;
		m_sSolPtoCh_T += m_surSolP[i] * area;
		m_orgNtoCh_T += m_traOrgN[i] * area;
		m_attPtoCh_T += m_traAttP[i] * area;
	}

	return 0;
}