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
	m_soilDepth(NULL), m_soilLayers(NULL), m_soilThick(NULL), m_Slope(NULL),  
	m_VgroundwaterFromBankStorage(NULL),m_subbasinsInfo(NULL),
	m_dp_co(NODATA_VALUE), m_Kg(NODATA_VALUE), m_Base_ex(NODATA_VALUE), 
	m_perc(NULL), m_D_EI(NULL), m_D_ED(NULL), m_D_ES(NULL), m_D_PET(NULL), m_soilMoisture(NULL),
	/// intermediate
	m_petSubbasin(NULL), m_gwStore(NULL),
	/// outputs
	m_T_Perco(NULL),m_T_PerDep(NULL),m_T_RG(NULL),m_T_QG(NULL),m_D_Revap(NULL),m_T_Revap(NULL),m_firstRun(true), m_T_GWWB(NULL)
                                         
{
	
}

ReservoirMethod::~ReservoirMethod(void)
{
	if(m_T_Perco != NULL) Release1DArray(m_T_Perco);
	if(m_T_PerDep != NULL) Release1DArray(m_T_PerDep);
    if (m_D_Revap != NULL) Release1DArray(m_D_Revap);
	if (m_T_Revap != NULL) Release1DArray(m_T_Revap);
    if (m_T_RG != NULL) Release1DArray(m_T_RG);
    if (m_T_QG != NULL) Release1DArray(m_T_QG);
    if (m_petSubbasin != NULL) Release1DArray(m_petSubbasin);
    if (m_gwStore != NULL) Release1DArray(m_gwStore);
	if(m_T_GWWB != NULL) Release2DArray(m_nSubbasins+1, m_T_GWWB);
}
void ReservoirMethod::initialOutputs()
{
    int nLen = m_nSubbasins + 1;
	if(m_firstRun){
		setSubbasinInfos();
		m_firstRun = false;
	}
	if(m_T_Perco == NULL) Initialize1DArray(nLen,m_T_Perco,0.f);
	if(m_T_Revap == NULL) Initialize1DArray(nLen,m_T_Revap,0.f);
	if(m_T_PerDep == NULL) Initialize1DArray(nLen,m_T_PerDep,0.f);
	if (m_T_RG == NULL) Initialize1DArray(nLen, m_T_RG, 0.f);
	if (m_T_QG == NULL) Initialize1DArray(nLen, m_T_QG, 0.f);
	if (m_petSubbasin == NULL) Initialize1DArray(nLen, m_petSubbasin, 0.f);
	if (m_gwStore == NULL) Initialize1DArray(nLen, m_gwStore, 0.f);
    if (m_D_Revap == NULL) Initialize1DArray(m_nCells, m_D_Revap, 0.f);
	if(m_T_GWWB == NULL) Initialize2DArray(nLen, 6, m_T_GWWB, 0.f);
}
int ReservoirMethod::Execute()
{
    if (!CheckInputData()) return -1;
	initialOutputs();
    float QGConvert = 1.f * m_CellWidth * m_CellWidth / (m_TimeStep) / 1000.f; // mm ==> m3/s
	for (vector<int>::iterator it = m_subbasinIDs.begin(); it!=m_subbasinIDs.end();it++)
    {
		Subbasin *curSub = m_subbasinsInfo->GetSubbasinByID(*it);
        float gwBank = 0.f;
		// at the first time step m_VgroundwaterFromBankStorage is NULL
        if (m_VgroundwaterFromBankStorage != NULL)
            gwBank = m_VgroundwaterFromBankStorage[*it];
		int curCellsNum = curSub->getCellCount();
		int *curCells = curSub->getCells();
		float perco = 0.f;
		int index = 0;
		for (int i = 0; i < curCellsNum; i++)
		{
			index = curCells[i];
			perco += m_perc[index][(int)m_soilLayers[index]-1];
		}
		perco /= curCellsNum;
		curSub->setPerco(perco);
		//calculate EG, i.e. Revap
		//if percolation < 0.01, EG will be 0.
		//if percolation >= 0.01, EG will be calculated by equation
		//because the soil temperature has been considered when percolation is calculated,
		//it do not need to be reconsidered here.
		float revap = 0.0f;
		float fPET = 0.0f;
		float fEI = 0.0f;
		float fED = 0.0f;
		float fES = 0.0f;
		for (int i = 0; i < curCellsNum; i++)
		{
			index = curCells[i];
			fPET += m_D_PET[index];
			fEI += m_D_EI[index];
			fED += m_D_ED[index];
			fES += m_D_ES[index];
		}
		fPET /= curCellsNum;
		fEI /= curCellsNum;
		fED /= curCellsNum;
		fES /= curCellsNum;

		curSub->setPET(fPET);

		if (perco >= 0.01f)
		{
			revap = (fPET - fEI - fED - fES) * m_GW0 / m_GWMAX;    //revap
			revap = max(revap, 0.f);
			revap = min(revap, perco);
		}

		float prevRevap = curSub->getEG();
		if (prevRevap != revap)
		{
			curSub->setEG(revap);
			curSub->setIsRevapChanged(true);
		}
		else
			curSub->setIsRevapChanged(false);

		float percoDeep = perco * m_dp_co; //deep percolation
		curSub->setPerde(percoDeep);
		float slopeCoef = curSub->getSlopeCoef();
		float kg = m_Kg * slopeCoef;
		float groundRunoff = kg * pow(m_GW0, m_Base_ex); // groundwater runoff (mm)
		float groundQ = groundRunoff * curCellsNum * QGConvert; // groundwater discharge (m3/s)

		float gwOld = curSub->getGW();
		float groundStorage = 0.f;
		groundStorage += (perco - revap - percoDeep - groundRunoff);
		//add the ground water from bank storage, 2011-3-14
		groundStorage += gwBank / curSub->getArea() * 1000.f;
		groundStorage = max(groundStorage, 0.f);
		if (groundStorage > m_GWMAX)
		{
			groundQ += (groundStorage - m_GWMAX);
			groundRunoff = groundQ / (curCellsNum * QGConvert);
			groundStorage = m_GWMAX;
		}
		curSub->setRG(groundRunoff);
		curSub->setGW(groundStorage);
		curSub->setQG(groundQ);
		if (groundStorage != groundStorage)
		{
			ostringstream oss;
			oss << perco << "\t" << revap << "\t" << percoDeep << "\t" << groundRunoff << "\t" << gwOld << "\t" << m_Kg << "\t" <<
				m_Base_ex << "\t" << slopeCoef << endl;
			throw ModelException("Subbasin", "setInputs", oss.str());
		}
		m_T_Perco[*it] = curSub->getPerco();
		m_T_Revap[*it] = curSub->getEG();
		m_T_PerDep[*it] = curSub->getPerde();
        m_T_RG[*it] = curSub->getRG();                //get rg of specific subbasin
        m_T_QG[*it] = curSub->getQG();                //get qg of specific subbasin
        m_petSubbasin[*it] = curSub->getPET();
        m_gwStore[*it] = curSub->getGW();
    }

	m_T_Perco[0] = m_subbasinsInfo->subbasin2basin(VAR_PERCO);
	m_T_Revap[0] = m_subbasinsInfo->subbasin2basin(VAR_REVAP);
	m_T_PerDep[0] = m_subbasinsInfo->subbasin2basin(VAR_PERDE);
	m_T_RG[0] = m_subbasinsInfo->subbasin2basin(VAR_RG);  // get rg of entire watershed
	m_gwStore[0] = m_subbasinsInfo->subbasin2basin(VAR_GW_Q);
	m_T_QG[0] = m_subbasinsInfo->subbasin2basin(VAR_QG);  // get qg of entire watershed
	// output to GWWB
	for (int i = 0; i <= m_nSubbasins; i++)
	{
		m_T_GWWB[i][0] = m_T_Perco[i];
		m_T_GWWB[i][1] = m_T_Revap[i];
		m_T_GWWB[i][2] = m_T_PerDep[i];
		m_T_GWWB[i][3] = m_T_RG[i];
		m_T_GWWB[i][4] = m_gwStore[i];
		m_T_GWWB[i][5] = m_T_QG[i];
	}
	// update soil moisture
	for (vector<int>::iterator it = m_subbasinIDs.begin(); it!=m_subbasinIDs.end();it++)
	{
		Subbasin *sub = m_subbasinsInfo->GetSubbasinByID(*it);
		//if the revap is the same with last time step, do not set its value to m_D_Revap.
        if (!(sub->getIsRevapChanged())) 
            continue;
        int *cells = sub->getCells();
        int nCells = sub->getCellCount();
        int index = 0;
        for (int i = 0; i < nCells; i++)
        {
            index = cells[i];
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
    if (m_Slope == NULL)
        throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: Slope has not been set.");
    if (m_soilMoisture == NULL)
		throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: soil moisture has not been set.");
	if (m_soilLayers == NULL)
		throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: soil layers has not been set.");
	if (m_soilThick == NULL)
		throw ModelException(MID_GWA_RE, "CheckInputData", "The parameter: soil thickness has not been set.");

	if (m_nSubbasins <= 0) throw ModelException(MID_GWA_RE, "CheckInputData", "The subbasins number must be greater than 0.");
	if (m_subbasinIDs.empty()) throw ModelException(MID_GWA_RE, "CheckInputData", "The subbasin IDs can not be EMPTY.");
	if (m_subbasinsInfo == NULL) throw ModelException(MID_GWA_RE, "CheckInputData", "The subbasins information can not be NULL.");
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
        throw ModelException(MID_GWA_RE, "SetValue", "Parameter " + sk + " does not exist in current module.");
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
    else if (StringMatch(sk, VAR_SLOPE))
        m_Slope = data;
	else if (StringMatch(sk, VAR_SOILLAYERS))
		m_soilLayers = data;
    else
        throw ModelException(MID_GWA_RE, "Set1DData", "Parameter " + sk + " does not exist in current module.");
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
        throw ModelException(MID_GWA_RE, "Set2DData", "Parameter " + sk + " does not exist.");
}
void ReservoirMethod::SetSubbasins(clsSubbasins *subbasins)
{
	if(m_subbasinsInfo == NULL){
		m_subbasinsInfo = subbasins;
		m_nSubbasins = m_subbasinsInfo->GetSubbasinNumber();
		m_subbasinIDs = m_subbasinsInfo->GetSubbasinIDs();
	}
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
        *nRows = m_nSubbasins + 1;
    }
    else if (StringMatch(sk, VAR_SBQG))
    {
        *data = m_T_QG;
        *nRows = m_nSubbasins + 1;
    }
    else if (StringMatch(sk, VAR_SBGS))
    {
        *data = m_gwStore;
        *nRows = m_nSubbasins + 1;
    }
    else if (StringMatch(sk, VAR_SBPET))
    {
        *data = m_petSubbasin;
        *nRows = m_nSubbasins + 1;
    }
    else
        throw ModelException(MID_GWA_RE, "Get1DData", "Parameter " + sk + " does not exist in current module.");
}

void ReservoirMethod::Get2DData(const char *key, int *nRows, int *nCols, float ***data)
{
	initialOutputs();
    string sk(key);
	if (StringMatch(sk, VAR_GWWB))
	{
		*data = m_T_GWWB;
		*nRows = m_nSubbasins + 1;
		*nCols = 6;
	}
    else
        throw ModelException(MID_GWA_RE, "Get2DData", "Parameter " + sk + " does not exist in current module.");
}

void ReservoirMethod::setSubbasinInfos()
{
	for (vector<int>::iterator it = m_subbasinIDs.begin(); it!=m_subbasinIDs.end();it++)
	{
		Subbasin *curSub = m_subbasinsInfo->GetSubbasinByID(*it);
		if(curSub->getSlope() <= 0.f)
			curSub->setSlope(m_Slope);
	}
	m_subbasinsInfo->SetSlopeCoefficient();
}
