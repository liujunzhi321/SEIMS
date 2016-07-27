#include <set>
#include <cmath>
#include <iostream>
#include <fstream>
#include <omp.h>
#include "ReservoirMethod.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"


ReservoirMethod::ReservoirMethod(void) : m_TimeStep(-1), m_nCells(-1), m_nSubbasins(-1), m_nSoilLayers(-1), 
	m_soilDepth(NULL), m_soilLayers(NULL), m_soilThick(NULL), m_Slope(NULL), m_subbasin(NULL), 
	m_VgroundwaterFromBankStorage(NULL),
	m_dp_co(NODATA_VALUE), m_Kg(NODATA_VALUE), m_Base_ex(NODATA_VALUE), 
	m_perc(NULL), m_D_EI(NULL), m_D_ED(NULL), m_D_ES(NULL), m_D_PET(NULL), m_soilMoisture(NULL),
	/// intermediate
	m_petSubbasin(NULL), m_gwStore(NULL),
	/// outputs
	m_T_RG(NULL),m_T_QG(NULL),m_D_Revap(NULL)
                                         
{
	//m_T_GWWB(NULL) // currently no longer used. By LJ
}

ReservoirMethod::~ReservoirMethod(void)
{
    if (m_D_Revap != NULL) Release1DArray(m_D_Revap);
        //delete[] m_D_Revap;
    if (m_T_RG != NULL) Release1DArray(m_T_RG);
        //delete[] m_T_RG;
    if (m_T_QG != NULL) Release1DArray(m_T_QG);
        //delete[] m_T_QG;
    //if (m_T_GWWB != NULL) Release2DArray(m_nSubbasins+1, m_T_GWWB);
    //{
    //    for (int i = 0; i < m_subbasinSelectedCount; i++)
    //    {
    //        if (m_T_GWWB[i] != NULL)
    //            delete[] m_T_GWWB[i];
    //    }
    //    delete[] m_T_GWWB;
    //}

    //for (size_t i = 0; i < m_subbasinList.size(); i++)
	for (vector<Subbasin*>::iterator it = m_subbasinList.begin(); it != m_subbasinList.end();)
    {
        if (*it != NULL)
            delete *it;
		it = m_subbasinList.erase(it);
    }
	m_subbasinList.clear();

    if (m_petSubbasin != NULL) Release1DArray(m_petSubbasin);
        //delete m_petSubbasin;

    if (m_gwStore != NULL) Release1DArray(m_gwStore);
        //delete m_gwStore;
}
void ReservoirMethod::initialOutputs()
{
	//read subbasin information
    if (m_subbasinList.size() == 0)
        getSubbasinList();
	m_nSubbasins = m_subbasinList.size();
    int nLen = m_nSubbasins + 1;

	if (m_T_RG == NULL) Initialize1DArray(nLen, m_T_RG, 0.f);
	if (m_T_QG == NULL) Initialize1DArray(nLen, m_T_QG, 0.f);
	if (m_petSubbasin == NULL) Initialize1DArray(nLen, m_petSubbasin, 0.f);
	if (m_gwStore == NULL) Initialize1DArray(nLen, m_gwStore, 0.f);
    
    if (m_D_Revap == NULL) Initialize1DArray(m_nCells, m_D_Revap, 0.f);
}
int ReservoirMethod::Execute()
{
    // check the data
    if (!CheckInputData()) return -1;
	initialOutputs();
    //if(m_T_GWWB == NULL && m_subbasinSelectedCount > 0 && m_subbasinSelected != NULL)
    //{
    //	m_T_GWWB = new float*[m_subbasinSelectedCount];
    //	for(int i=0; i<m_subbasinSelectedCount; i++)
    //		m_T_GWWB[i] = new float[6];
    //}
    //cout << "nSubbasins:\t" << m_nSubbasins << endl;
    for (int i = 0; i < m_nSubbasins; i++)
    {
        int subbasinID = i + 1;
        Subbasin *sub = m_subbasinList[i];
        float gwBank = 0.f;
		// at the first time step m_VgroundwaterFromBankStorage is NULL
        if (m_VgroundwaterFromBankStorage != NULL)
            gwBank = m_VgroundwaterFromBankStorage[subbasinID];

        sub->setInputs(m_D_PET, m_D_EI, m_D_ED, m_D_ES, m_perc, gwBank);    //calculate
        m_T_RG[subbasinID] = sub->getRG();                //get rg of specific subbasin
        m_T_QG[subbasinID] = sub->getQG();                //get qg of specific subbasin
        m_petSubbasin[subbasinID] = sub->getPET();
        m_gwStore[subbasinID] = sub->getGW();
    }

    m_T_RG[0] = Subbasin::subbasin2basin(VAR_RG, &m_subbasinList);  // get rg of entire watershed
    m_T_QG[0] = Subbasin::subbasin2basin(VAR_QG, &m_subbasinList);  // get qg of entire watershed
    m_petSubbasin[0] = Subbasin::subbasin2basin(VAR_PET, &m_subbasinList);
    m_gwStore[0] = Subbasin::subbasin2basin(VAR_GW_Q, &m_subbasinList);

    //get D_Revap
    for (int i = 0; i < m_nSubbasins; i++)
    {
        Subbasin *sub = m_subbasinList[i];
		//if the revap is the same with last time step, do not set its value to m_D_Revap.
        if (!(sub->getIsRevapChanged())) 
            continue;

        int *cells = sub->getCells();
        int nCells = sub->getCellCount();
        int index = 0;
        for (int i = 0; i < nCells; i++)
        {
            index = cells[i];
            //float depth2 = m_soilDepth[index] - m_upSoilDepth;
            //m_soilMoisture[index][m_nSoilLayers - 1] += sub->getEG() / depth2;
            m_soilMoisture[index][(int)m_soilLayers[index] - 1] += sub->getEG();
			// TODO: Is it need to allocate revap to each soil layers??? By LJ
        }
    }
    return 0;
}

/// deprecated in next update version. By LJ
void ReservoirMethod::clearInputs()
{
    m_TimeStep = -1;
    m_nCells = -1;
    m_dp_co = NODATA_VALUE;
    m_Kg = NODATA_VALUE;
    m_Base_ex = NODATA_VALUE;
}

bool ReservoirMethod::CheckInputData()
{
    if (m_nCells <= 0)
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: m_nCells has not been set.");
    if (m_CellWidth < 0)
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: m_NumSub has not been set.");
    if (m_TimeStep <= 0)
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: m_TimeStep has not been set.");
    if (m_nSoilLayers <= 0)
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: m_nSoilLayers has not been set.");
    if (FloatEqual(m_dp_co, NODATA_VALUE))
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: df_coef has not been set.");
    if (FloatEqual(m_Kg, NODATA_VALUE))
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: m_Kg has not been set.");
    if (FloatEqual(m_Base_ex, NODATA_VALUE))
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: m_Base_ex has not been set.");
    if (m_perc == NULL)
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: Percolation_2D has not been set.");
    if (m_D_EI == NULL)
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: m_D_EI has not been set.");
    if (m_D_ED == NULL)
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: m_D_ED has not been set.");
    if (m_D_ES == NULL)
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: m_D_ES has not been set.");
    if (m_D_PET == NULL)
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: m_D_PET has not been set.");
    if (m_subbasin == NULL)
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: m_Subbasin has not been set.");
    if (m_Slope == NULL)
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: Slope has not been set.");
    if (m_soilMoisture == NULL)
		throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: soil moisture has not been set.");
	if (m_soilLayers == NULL)
		throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: soil layers has not been set.");
	if (m_soilThick == NULL)
		throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: soil thickness has not been set.");
    //if (m_soilDepth == NULL)
    //    throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: RootDepth has not been set.");
    return true;
}

bool ReservoirMethod::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
        throw ModelException(MID_GWA_RE, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    if (m_nCells != n)
    {
        if (m_nCells <= 0)
            m_nCells = n;
        else
            throw ModelException(MID_GWA_RE, "CheckInputSize", "Input data for " + string(key) +
                                                             " is invalid. All the input data should have same size.");
    }
    return true;
}

// set value
void ReservoirMethod::SetValue(const char *key, float value)
{
    string sk(key);
    if (StringMatch(sk, Tag_TimeStep))
        m_TimeStep = int(value);
    else if (StringMatch(sk, VAR_OMP_THREADNUM))
        omp_set_num_threads((int) value);
    //else if (StringMatch(sk, Tag_CellSize))
    //    m_nCells = int(value);
    else if (StringMatch(sk, Tag_CellWidth))
        m_CellWidth = value;
    else if (StringMatch(sk, VAR_KG))
        m_Kg = value;
    else if (StringMatch(sk, VAR_Base_ex))
        m_Base_ex = value;
    else if (StringMatch(sk, VAR_DF_COEF))
        m_dp_co = value;
    else if (StringMatch(sk, VAR_GW0))
        m_GW0 = value;
    else if (StringMatch(sk, VAR_GWMAX))
        m_GWMAX = value;
    else
        throw ModelException(MID_GWA_RE, "SetValue", "Parameter " + sk +
                                                   " does not exist in current module. Please contact the module developer.");
}

void ReservoirMethod::Set1DData(const char *key, int n, float *data)
{
    string sk(key);
    if (StringMatch(sk, VAR_GWNEW))
    {
        m_VgroundwaterFromBankStorage = data;
		return;
    }

    //check the input data
    if (!CheckInputSize(key, n)) return;

    //set the value
    if (StringMatch(sk, VAR_INET))
        m_D_EI = data;
    else if (StringMatch(sk, VAR_DEET))
        m_D_ED = data;
    else if (StringMatch(sk, VAR_SOET))
        m_D_ES = data;
    else if (StringMatch(sk, VAR_PET))
        m_D_PET = data;
    else if (StringMatch(sk, VAR_SUBBSN))
        m_subbasin = data;
    else if (StringMatch(sk, VAR_SLOPE))
        m_Slope = data;
	else if (StringMatch(sk, VAR_SOILLAYERS))
		m_soilLayers = data;
    else
        throw ModelException(MID_GWA_RE, "Set1DData", "Parameter " + sk +
                                                    " does not exist in current module. Please contact the module developer.");
}


void ReservoirMethod::Set2DData(const char *key, int nrows, int ncols, float **data)
{
    string sk(key);
    CheckInputSize(key, nrows);
    m_nSoilLayers = ncols;

    if (StringMatch(sk, VAR_PERCO))
        m_perc = data;
    else if (StringMatch(sk, VAR_SOMO))
        m_soilMoisture = data;
	else if (StringMatch(sk, VAR_SOILDEPTH))
		m_soilDepth = data;
	else if (StringMatch(sk, VAR_SOILTHICK))
		m_soilThick = data;
    else
        throw ModelException(MID_GWA_RE, "Set2DData",
                             "Parameter " + sk + " does not exist. Please contact the module developer.");
}

void ReservoirMethod::Get1DData(const char *key, int *nRows, float **data)
{
	initialOutputs();
    string sk(key);
    if (StringMatch(sk, VAR_REVAP))
    {
        *data = m_D_Revap;
        *nRows = m_nCells;
    }
    else if (StringMatch(sk, VAR_RG))
    {
        *data = m_T_RG;
        *nRows = m_subbasinList.size() + 1;
    }
    else if (StringMatch(sk, VAR_SBQG))
    {
        *data = m_T_QG;
        *nRows = m_subbasinList.size() + 1;
    }
    else if (StringMatch(sk, VAR_SBGS))
    {
        *data = m_gwStore;
        *nRows = m_subbasinList.size() + 1;
    }
    else if (StringMatch(sk, VAR_SBPET))
    {
        *data = m_petSubbasin;
        *nRows = m_subbasinList.size() + 1;
    }
    else
        throw ModelException(MID_GWA_RE, "Get1DData", "Parameter " + sk +
                                                     " does not exist in current module. Please contact the module developer.");
}

void ReservoirMethod::Get2DData(const char *key, int *nRows, int *nCols, float ***data)
{
	initialOutputs();
    string sk(key);
	if (StringMatch(sk, VAR_GWWB))
	{
		//*data = m_T_GWWB;
		//*nRows = m_nSubbasins + 1;
		//*nCols = 6;
	}
    else
        throw ModelException(MID_GWA_RE, "getResults", "Parameter " + sk +
                                                     " does not exist in current module. Please contact the module developer.");
}

void ReservoirMethod::getSubbasinList()
{
    if (m_subbasinList.size() > 0)
        return;

    if (m_subbasin == NULL)
        return;

    m_nSubbasins = 0;
	// valid cell indexes of each subbasin, key is subbasin ID, value is vector of cell's index
    map<int, vector<int> *> cellListMap;
    map<int, vector<int> *>::iterator it;
    int subID = 0;
    for (int i = 0; i < m_nCells; i++)
    {
        subID = int(m_subbasin[i]);
        it = cellListMap.find(subID);
        if (it == cellListMap.end())
            cellListMap[subID] = new vector<int>;
        cellListMap[subID]->push_back(i);
    }
    m_nSubbasins = cellListMap.size();

    for (it = cellListMap.begin(); it != cellListMap.end(); it++)
    {
		// swap for saving memory, added by LJ
		vector<int>(*it->second).swap(*it->second);
        subID = it->first;
        Subbasin *newSub = new Subbasin(subID);
        newSub->setParameter(m_dp_co, m_GW0, m_GWMAX, m_Kg, m_Base_ex, m_CellWidth, m_TimeStep);

        int nCells = it->second->size();
        int *tmp = new int[nCells];
		int *tmpSoilLayers = new int[nCells];
        for (int j = 0; j < nCells; j++)
		{
            tmp[j] = it->second->at(j);
			tmpSoilLayers[j] = (int)m_soilLayers[tmp[j]];
		}
        newSub->setCellList(nCells, tmp);
		newSub->setSoilLayers(nCells, tmpSoilLayers);
        m_subbasinList.push_back(newSub);
    }
	vector<Subbasin *>(m_subbasinList).swap(m_subbasinList);

    for (int i = 0; i < m_nSubbasins; i++)
        m_subbasinList[i]->setSlope(m_Slope);
    float slope_basin = Subbasin::subbasin2basin(VAR_SLOPE, &m_subbasinList);
    for (int i = 0; i < m_nSubbasins; i++)
        m_subbasinList[i]->setSlopeCoefofBasin(slope_basin);
	// release cellListMap to save memory
    for (it = cellListMap.begin(); it != cellListMap.end();)
    {
        if (it->second != NULL)
            delete it->second;
		it = cellListMap.erase(it);
    }
	cellListMap.clear();
}
