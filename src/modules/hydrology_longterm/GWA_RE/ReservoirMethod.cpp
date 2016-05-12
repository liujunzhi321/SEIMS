#include "ReservoirMethod.h"
#include "MetadataInfo.h"
#include <set>
#include <cmath>
#include <iostream>
#include <fstream>
#include "ModelException.h"
#include "util.h"
#include <omp.h>

ReservoirMethod::ReservoirMethod(void): m_TimeStep(-1), m_nCells(-1), m_nSoilLayers(2), m_dp_co(NODATA),
	m_Kg(NODATA), m_Base_ex(NODATA), m_perc(NULL), m_D_EI(NULL), m_D_ED(NULL), m_D_ES(NULL),
	m_D_PET(NULL), m_petSubbasin(NULL), m_gwStore(NULL), m_soilMoisture(NULL), m_rootDepth(NULL), m_nSubbasins(0), m_upSoilDepth(200.f)
{
	m_Slope = NULL;
	m_T_GWWB = NULL;
	m_T_RG = NULL;
	m_T_QG = NULL;
	m_D_Revap = NULL;
	m_subbasin = NULL;
	m_subbasinSelected = NULL;
	m_subbasinSelectedCount = -1;
	m_VgroundwaterFromBankStorage = NULL;
}

ReservoirMethod::~ReservoirMethod(void)
{
	if(m_D_Revap != NULL) 
		delete[] m_D_Revap;
	if(m_T_RG != NULL) 
		delete[] m_T_RG;
	if(m_T_QG != NULL) 
		delete[] m_T_QG;

	if(m_T_GWWB != NULL)
	{
		for(int i=0;i<m_subbasinSelectedCount;i++)
		{
			if(m_T_GWWB[i] != NULL) 
				delete[] m_T_GWWB[i];
		}
		delete[] m_T_GWWB;
	}

	for(size_t i = 0; i < m_subbasinList.size(); i++)
	{
		if(m_subbasinList[i] != NULL) 
			delete m_subbasinList[i];
	}

	if (m_petSubbasin != NULL)
		delete m_petSubbasin;
	
	if (m_gwStore != NULL)
		delete m_gwStore;

}

int ReservoirMethod::Execute()
{
	// check the data
	if (!CheckInputData()) 
		return -1;
	
	//read subbasin information
	if(m_subbasinList.size() == 0) 
		getSubbasinList();

	//initial results
	if(m_D_Revap == NULL) 
		m_D_Revap = new float[m_nCells];

	int nLen = m_subbasinList.size() + 1;

	if(m_T_RG == NULL) 
	{
		m_T_RG = new float[nLen];
		m_T_QG = new float[nLen];
		m_petSubbasin = new float[nLen];
		m_gwStore = new float[nLen];
	}

	//if(m_T_GWWB == NULL && m_subbasinSelectedCount > 0 && m_subbasinSelected != NULL) 
	//{
	//	m_T_GWWB = new float*[m_subbasinSelectedCount];
	//	for(int i=0; i<m_subbasinSelectedCount; i++) 
	//		m_T_GWWB[i] = new float[6];
	//}
	//cout << "nSubbasins:\t" << m_nSubbasins << endl;
	for(int i = 0; i < m_nSubbasins; i++)
	{
		int subbasinID = i+1;
		subbasin *sub = m_subbasinList[i];
		//cout << "subbasin ID: " << subbasinID << "\t" << sub << endl;

		float gwBank = 0.0f;
		if(m_VgroundwaterFromBankStorage != NULL)//at the first time step m_VgroundwaterFromBankStorage is null 
			gwBank = m_VgroundwaterFromBankStorage[subbasinID];

		sub->setInputs(m_D_PET, m_D_EI, m_D_ED, m_D_ES, m_perc, gwBank);	//caculate
		m_T_RG[subbasinID] = sub->getRG();				//get rg of specific subbasin
		m_T_QG[subbasinID] = sub->getQG();				//get qg of specific subbasin
		m_petSubbasin[subbasinID] = sub->getPET();
		m_gwStore[subbasinID] = sub->getGW();
	}
	
	m_T_RG[0] = subbasin::subbasin2basin("RG", &m_subbasinList);  // get rg of entire watershed
	m_T_QG[0] = subbasin::subbasin2basin("QG", &m_subbasinList);  // get qg of entire watershed
	m_petSubbasin[0] = subbasin::subbasin2basin("PET", &m_subbasinList);
	m_gwStore[0] = subbasin::subbasin2basin("GW", &m_subbasinList);

	//get D_Revap
	for(int i = 0; i < m_nSubbasins; i++)
	{
		subbasin* sub = m_subbasinList[i];
		if(!(sub->getIsRevapChanged())) //if the revap is the same with last time step, do not set its value to m_D_Revap.
			continue; 

		int* cells = sub->getCells();
		int nCells = sub->getCellCount();
		int index = 0;
		for(int i = 0; i < nCells; i++)
		{
			index = cells[i];
			float depth2 = m_rootDepth[index] - m_upSoilDepth;
			m_soilMoisture[index][m_nSoilLayers-1] += sub->getEG()/depth2;
		}
	}

	return 0;
}

void ReservoirMethod::clearInputs()
{
	m_TimeStep = -1;
	m_nCells = -1;
	m_dp_co = NODATA;
	m_Kg = NODATA;
	m_Base_ex = NODATA;
}

bool ReservoirMethod::CheckInputData()
{
	if (m_nCells < 0)
		throw ModelException("GWA_RE","CheckInputData","The parameter: m_nCells has not been set.");
	if (m_CellWidth < 0)
		throw ModelException("GWA_RE","CheckInputData","The parameter: m_NumSub has not been set.");
	if (m_TimeStep <= 0)
		throw ModelException("GWA_RE","CheckInputData","The parameter: m_TimeStep has not been set.");
	if (m_nCells <= 0)
		throw ModelException("GWA_RE","CheckInputData","The parameter: m_nCells has not been set.");
	if (FloatEqual(m_dp_co,NODATA))
		throw ModelException("GWA_RE","CheckInputData","The parameter: df_coef has not been set.");
	if (FloatEqual(m_Kg,NODATA))
		throw ModelException("GWA_RE","CheckInputData","The parameter: m_Kg has not been set.");
	if (FloatEqual(m_Base_ex,NODATA))
		throw ModelException("GWA_RE","CheckInputData","The parameter: m_Base_ex has not been set.");
	if (m_perc == NULL)
		throw ModelException("GWA_RE","CheckInputData","The parameter: Percolation_2D has not been set.");
	if (m_D_EI == NULL)
		throw ModelException("GWA_RE","CheckInputData","The parameter: m_D_EI has not been set.");
	if (m_D_ED == NULL)
		throw ModelException("GWA_RE","CheckInputData","The parameter: m_D_ED has not been set.");
	if (m_D_ES == NULL)
		throw ModelException("GWA_RE","CheckInputData","The parameter: m_D_ES has not been set.");
	if (m_D_PET == NULL)
		throw ModelException("GWA_RE","CheckInputData","The parameter: m_D_PET has not been set.");
	if (m_subbasin == NULL)
		throw ModelException("GWA_RE","CheckInputData","The parameter: m_Subbasin has not been set.");
	if (m_Slope == NULL)
		throw ModelException("GWA_RE","CheckInputData","The parameter: Slope has not been set.");
	if (m_soilMoisture == NULL)
		throw ModelException("GWA_RE","CheckInputData","The parameter: soil moisture has not been set.");
	if (m_rootDepth == NULL)
		throw ModelException("GWA_RE","CheckInputData","The parameter: RootDepth has not been set.");
	return true;
}

bool ReservoirMethod::CheckInputSize(const char* key, int n)
{
	if(n<=0)
		throw ModelException("GWA_RE","CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");

	if(m_nCells != n)
	{
		if(m_nCells <= 0) 
			m_nCells = n;
		else
		{
			printf("m_nCells: %d\n",m_nCells);
			throw ModelException("GWA_RE","CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
		}
	}

	return true;
}

// set value
void ReservoirMethod::SetValue(const char* key, float value)
{
	string sk(key);
	if (StringMatch(sk,"TimeStep"))
		m_TimeStep = int(value);
	else if (StringMatch(sk, VAR_OMP_THREADNUM))
		omp_set_num_threads((int)value);
	else if (StringMatch(sk,Tag_CellSize))
		m_nCells = int(value);
	else if (StringMatch(sk,Tag_CellWidth))
		m_CellWidth = value;
	else if (StringMatch(sk,"Kg"))
		m_Kg = value;
	else if (StringMatch(sk,"Base_ex"))
		m_Base_ex = value;
	else if (StringMatch(sk,"df_coef"))
		m_dp_co = value;
	else if (StringMatch(sk,"GW0"))
		m_GW0 = value;
	else if (StringMatch(sk,"GWMAX"))
		m_GWMAX = value;
	//else if(StringMatch(sk, "UpperSoilDepth"))		
	//	m_upSoilDepth = value;
	else
		throw ModelException("GWA_RE","SetValue","Parameter " + sk + " does not exist in GWA_RE module. Please contact the module developer.");
	
}

void ReservoirMethod::Set1DData(const char* key, int n, float *data)
{
	string sk(key);

	//if (StringMatch(sk,"subbasinSelected"))
	//{
	//	m_subbasinSelected = data;
	//	m_subbasinSelectedCount = n;
	//}

	if (StringMatch(sk,"T_GWNEW"))
	{
		m_VgroundwaterFromBankStorage = data;
	}

	//check the input data
	if (!CheckInputSize(key,n)) return;

	//set the value	
	if (StringMatch(sk,"D_INET"))
	{
		m_D_EI = data;
	}
	else if (StringMatch(sk, "D_DEET"))
	{
		m_D_ED = data;
	}
	else if (StringMatch(sk, "D_SOET"))
	{
		m_D_ES = data;
	}
	else if (StringMatch(sk, "D_PET"))
	{
		m_D_PET = data;
	}
	else if (StringMatch(sk,"Subbasin"))
	{
		m_subbasin = data;
	}
	else if (StringMatch(sk,"Slope"))
	{
		m_Slope = data;
	}	
	else if (StringMatch(sk,"Rootdepth"))
	{
		m_rootDepth = data;
	}	
	else
	{
		throw ModelException("GWA_RE","Set1DData","Parameter " + sk + " does not exist in GWA_RE module. Please contact the module developer.");
	}
}


void ReservoirMethod::Set2DData(const char* key, int nrows, int ncols, float** data)
{
	string sk(key);
	CheckInputSize(key, nrows);
	m_nSoilLayers = ncols;

	if (StringMatch(sk,"D_Percolation_2D"))
		m_perc = data;
	else if(StringMatch(sk, "D_SOMO_2D"))		
		m_soilMoisture = data;
	else
		throw ModelException("PER_PI", "Set2DData", 
		    "Parameter " + sk + " does not exist. Please contact the module developer.");
}

void ReservoirMethod::Get1DData(const char* key, int* nRows, float **data)
{
	string sk(key);
	if (StringMatch(sk,"Revap"))
	{
		*data = m_D_Revap;
		*nRows = m_nCells;
	}
	else if (StringMatch(sk,"RG"))
	{
		*data = m_T_RG;
		*nRows = m_subbasinList.size()+1;
	}
	else if (StringMatch(sk,"SBQG"))
	{
		*data = m_T_QG;
		*nRows = m_subbasinList.size()+1;
	}
	else if (StringMatch(sk,"SBGS"))
	{
		*data = m_gwStore;
		*nRows = m_subbasinList.size()+1;
	}
	else if (StringMatch(sk,"SBPET"))
	{
		*data = m_petSubbasin;
		*nRows = m_subbasinList.size()+1;
	}
	else
	{
		throw ModelException("GWA_RE","getResults","Parameter " + sk + " does not exist in GWA_RE module. Please contact the module developer.");
	}
}

void ReservoirMethod::Get2DData(const char* key, int* nRows, int* nCols, float ***data)
{
	string sk(key);
	if (StringMatch(sk,"GWWB"))
	{
		*data = m_T_GWWB;
		*nRows = m_subbasinSelectedCount;
		*nCols = 6;
	}
	else
	{
		throw ModelException("GWA_RE","getResults","Parameter " + sk + " does not exist in GWA_RE module. Please contact the module developer.");
	}
}

void ReservoirMethod::getSubbasinList()
{
	if(m_subbasinList.size() > 0) 
		return;

	if(m_subbasin == NULL) 
		return;

	//map<int,bool> selected;
	//for(int i = 0;i < m_subbasinSelectedCount; i++)
	//{
	//	int subid = int(m_subbasinSelected[i]);
	//	selected[subid] = true;
	//}
	
	m_nSubbasins = 0;
	map<int, vector<int>* > cellListMap;
	map<int, vector<int>* >::iterator it;
	int subID = 0;
	for(int i = 0; i < m_nCells; i++)
	{
		subID = int(m_subbasin[i]);
		it = cellListMap.find(subID);
		if(it == cellListMap.end())
			cellListMap[subID] = new vector<int>;
		cellListMap[subID]->push_back(i);
	}
	m_nSubbasins = cellListMap.size();

	for(it = cellListMap.begin(); it != cellListMap.end(); it++)
	{
		subID = it->first;
		subbasin* newSub = new subbasin(subID, false);
		newSub->setParameter(m_dp_co, m_GW0, m_GWMAX, m_Kg, m_Base_ex, m_CellWidth, m_TimeStep);

		int nCells = it->second->size();
		int *tmp = new int[nCells];
		for(int j = 0; j < nCells; j++)
			tmp[j] = it->second->at(j);
		newSub->setCellList(nCells, tmp);

		m_subbasinList.push_back(newSub);
	}

	for(int i = 0; i < m_nSubbasins; i++)
		m_subbasinList[i]->setSlope(m_Slope);

	float slope_basin = subbasin::subbasin2basin("Slope", &m_subbasinList);
	
	for(int i = 0; i < m_nSubbasins; i++)
		m_subbasinList[i]->setSlopeBasin(slope_basin);
	
	for(it = cellListMap.begin(); it != cellListMap.end(); it++)
	{
		if(it->second != NULL)
			delete it->second;
	}
	
}
