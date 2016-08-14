#include "clsSubbasin.h"
#include "util.h"
#include <iostream>
#include <sstream>
#include "ModelException.h"

using namespace std;

Subbasin::Subbasin(int id) : m_id(id), m_isRevapChanged(true), 
	m_nCells(-1), m_cells(NULL), m_cellArea(-1.f),m_Area(-1.f), 
	m_slope(-1.f), m_slopeCoefficient(1.f), 
	m_GWMAX(-1.f),m_kg(-1.f),m_dp_co(-1.f),m_base_ex(-1.f),m_QGConvert(-1.f),
	m_Revap(-1.f), m_GW(-1.f), m_PERCO(-1.f),m_PERDE(-1.f), 
	m_PET(-1.f),m_QG(-1.f), m_RG(-1.f) ,
	m_DepressionET(-1.f),m_Infiltration(-1.f),m_Interception(-1.f),m_NetPercolation(-1.f),
	m_Pcp(-1.f),m_pNet(-1.f),m_R(-1.f),m_RI(-1.f),m_RS(-1.f),m_S_MOI(-1.f),m_TotalET(-1.f),
	m_soilET(-1.f),m_TMean(-1.f),m_SoilT(-1.f),m_interceptionET(-1.f)
{
}
Subbasin::~Subbasin(void)
{
	// There seems no variables need to be released! By LJ
    //if (m_cells != NULL) Release1DArray(m_cells);
}
bool Subbasin::CheckInputSize(int n)
{
	if (n <= 0)
		throw ModelException("Subbasin Class", "CheckInputSize",
		"Input data for Subbasin is invalid. The size could not be less than zero.");

	if (m_nCells != n)
	{
		if (m_nCells <= 0)
			m_nCells = n;
		else
			throw ModelException("Subbasin Class", "CheckInputSize", "All input data should have same size.");
	}
	return true;
}
void Subbasin::setCellList(int nCells, int *cells)
{
	CheckInputSize(nCells);
    m_cells = cells;
}
//void Subbasin::setSoilLayers(int nCells, int *soilLayers)
//{
//	CheckInputSize(nCells);
//	m_nSoilLayers = soilLayers;
//}
//void Subbasin::setParameter4Groundwater(float rv_co, float GW0, float GWMAX, float kg, float base_ex, float cellWidth, int timeStep)
//{
//    m_dp_co = rv_co;
//    m_GW = GW0;
//    m_GWMAX = GWMAX;
//    m_kg = kg;
//    m_base_ex = base_ex;
//    m_cellArea = cellWidth * cellWidth;
//	m_Area = m_nCells * m_cellArea;
//    m_QGConvert = 1.f * cellWidth * cellWidth / (timeStep) / 1000.f; // mm ==> m3/s
//}
// Note: Since slope is calculated by drop divided by distance in TauDEM,
//		 the average should be calculated after doing atan().
//		 By LJ, 2016-7-27
void Subbasin::setSlope(float *slope)
{
    m_slope = 0.f;
    int index = 0;
    for (int i = 0; i < m_nCells; i++)
    {
        index = m_cells[i];
        //m_slope += slope[index];  // percent
		m_slope += atan(slope[index]); // radian
    }
    m_slope /= m_nCells;
	m_slope = tan(m_slope); // to keep consistent with the slope unit in the whole model
}

//void Subbasin::setSlopeCoefofBasin(float slopeBasin)
//{
//    if (slopeBasin <= 0.f)
//        m_slopeCoefficient = 1.f;
//    else
//        m_slopeCoefficient = m_slope / slopeBasin;
//}

//void Subbasin::setInputs4Groundwater(float *PET, float *EI, float *ED, float *ES, float **PERCO, float groundwaterFromBankStorage)
//{
//    m_PERCO = 0.f;
//    int index = 0;
//    //cout << "start setInputs\t" << m_nCells << endl;
//    for (int i = 0; i < m_nCells; i++)
//    {
//        index = m_cells[i];
//        //cout << i << "\t" << index << endl;
//		// percolation amount of current subbasin is accumulated by
//		// the percolation water of the bottom soil layer of each valid cell
//        //m_PERCO += PERCO[index][1]; // the previous version has a fixed soil layers of 2
//		if(m_nSoilLayers == NULL)
//			throw ModelException("Subbasin Class", "setInputs", "Soil layers number can not be NULL, please setSoilLayers first!");
//		m_PERCO += PERCO[index][m_nSoilLayers[i]-1];
//    }
//    m_PERCO /= m_nCells;
//    //calculate EG, i.e. Revap
//    //if percolation < 0.01, EG will be 0.
//    //if percolation >= 0.01, EG will be calculated by equation
//    //because the soil temperature has been considered when percolation is calculated,
//    //it do not need to be reconsidered here.
//    float revap = 0.0f;
//    float fPET = 0.0f;
//    float fEI = 0.0f;
//    float fED = 0.0f;
//    float fES = 0.0f;
//    for (int i = 0; i < m_nCells; i++)
//    {
//        index = m_cells[i];
//        fPET += PET[index];
//        fEI += EI[index];
//        fED += ED[index];
//        fES += ES[index];
//    }
//    fPET /= m_nCells;
//    fEI /= m_nCells;
//    fED /= m_nCells;
//    fES /= m_nCells;
//
//    m_PET = fPET;
//    //cout << "PET:\t" << m_PET << endl;
//
//    if (m_PERCO >= 0.01f)
//    {
//        revap = (fPET - fEI - fED - fES) * m_GW / m_GWMAX;    //revap
//        revap = max(revap, 0.f);
//        revap = min(revap, m_PERCO);
//    }
//
//
//    if (m_Revap != revap)
//    {
//        m_Revap = revap;
//        m_isRevapChanged = true;
//    }
//    else
//    {
//        m_isRevapChanged = false;
//    }
//    //cout << m_PERCO << "\t" << m_GW << "\t" << endl;
//
//    m_PERDE = m_PERCO * m_dp_co; //deep percolation
//
//    float kg = m_kg * m_slopeCoefficient;
//    m_RG = kg * pow(m_GW, m_base_ex);        //base flow
//    m_QG = m_RG * m_nCells * m_QGConvert;
//    //cout << "QG:\t" << m_QG << endl;
//
//    float gwOld = m_GW;
//    m_GW = m_GW + m_PERCO - m_Revap - m_PERDE - m_RG;
//	//add the ground water from bank storage, 2011-3-14
//    m_GW += groundwaterFromBankStorage / m_Area * 1000.f;
//    m_GW = max(m_GW, 0.f);
//    if (m_GW > m_GWMAX)
//    {
//        m_QG += (m_GW - m_GWMAX);
//        m_RG = m_QG / (m_nCells * m_QGConvert);
//        m_GW = m_GWMAX;
//    }
//
//    if (m_GW != m_GW)
//    {
//        ostringstream oss;
//        oss << m_PERCO << "\t" << m_Revap << "\t" << m_PERDE << "\t" << m_RG << "\t" << gwOld << "\t" << m_kg << "\t" <<
//        m_base_ex << "\t" << m_slopeCoefficient << endl;
//        throw ModelException("Subbasin", "setInputs", oss.str());
//    }
//}

//float Subbasin::subbasin2basin(string key, vector<Subbasin *> *subbasins)
//{
//    float temp = 0.f;
//    int totalCount = 0;
//    int nCount;
//    Subbasin *sub = NULL;
//    for (size_t i = 0; i < subbasins->size(); i++)
//    {
//        sub = subbasins->at(i);
//        nCount = sub->getCellCount();
//        if (StringMatch(key, VAR_SLOPE))
//            temp += atan(sub->getSlope()) * nCount;
//        else if (StringMatch(key, VAR_PET))
//            temp += sub->getPET() * nCount;
//        else if (StringMatch(key, VAR_PERCO))
//            temp += sub->getPerco() * nCount;
//        else if (StringMatch(key, VAR_REVAP))
//            temp += sub->getEG() * nCount;
//        else if (StringMatch(key, VAR_PERDE))
//            temp += sub->getPerde() * nCount;
//        else if (StringMatch(key, VAR_RG))
//            temp += sub->getRG() * nCount;
//        else if (StringMatch(key, VAR_QG))
//            temp += sub->getQG();
//        else if (StringMatch(key, VAR_GW_Q))
//            temp += sub->getGW() * nCount;
//
//        totalCount += nCount;
//    }
//    if (StringMatch(key, VAR_QG))
//        return temp;    // basin sum
//	if(StringMatch(key, VAR_SLOPE))
//		return tan(temp / totalCount);
//    return temp / totalCount;  // basin average 
//}


//////////////////////////////////////////////////////////////////////////
//////////  clsSubbasins                           ///////////////////////
////////////////////////////////////////////////////////////////////////// 
clsSubbasins::clsSubbasins(mongoc_gridfs_t *spatialData, map<string, clsRasterData *> &rsMap,int prefixID):
m_nSubbasins(-1)
{
	m_subbasinIDs.clear();
	m_subbasinsInfo.clear();
	// read subbasin data
	int nCells = -1;
	float *subbasinData = NULL;
	float cellWidth = NODATA_VALUE;
	ostringstream oss;
	oss << prefixID << "_" << NAME_MASK;
	string maskFileName = GetUpper(oss.str());
	oss.str("");
	oss << prefixID << "_" << VAR_SUBBSN;
	string subbasinFileName = GetUpper(oss.str());
	oss.str("");

	if (rsMap.find(maskFileName) == rsMap.end()) // if mask not loaded yet
		throw ModelException("clsSubbasins", "Constructor", "MASK data has not been loaded yet!");

	if (rsMap.find(subbasinFileName) == rsMap.end()) // if subbasin not loaded yet
	{
		clsRasterData *subbasinRaster = NULL;
		try
		{			
			subbasinRaster = new clsRasterData(spatialData, subbasinFileName.c_str(), rsMap[maskFileName]);			
			subbasinRaster->getRasterData(&nCells, &subbasinData);
		}
		catch (ModelException e)
		{
			cout << e.toString() << endl;
			return;
		}
		rsMap[subbasinFileName] = subbasinRaster;
		cellWidth = subbasinRaster->getCellWidth();
	}
	else
		rsMap[subbasinFileName]->getRasterData(&nCells, &subbasinData);

	m_nSubbasins = 0;
	// valid cell indexes of each subbasin, key is subbasin ID, value is vector of cell's index
	map<int, vector<int> *> cellListMap;
	map<int, vector<int> *>::iterator it;
	int subID = 0;
#pragma omp parallel for
	for (int i = 0; i < nCells; i++)
	{
		subID = int(subbasinData[i]);
		it = cellListMap.find(subID);
		if (it == cellListMap.end())
			cellListMap[subID] = new vector<int>;
		cellListMap[subID]->push_back(i);
	}
	m_nSubbasins = cellListMap.size();
	for (it = cellListMap.begin(); it != cellListMap.end(); it++)
	{
		// swap for saving memory
		vector<int>(*it->second).swap(*it->second);
		subID = it->first;
		m_subbasinIDs.push_back(subID);
		Subbasin *newSub = new Subbasin(subID);
		int nCellsTmp = it->second->size();
		int *tmp = new int[nCellsTmp];
		for (int j = 0; j < nCellsTmp; j++)
			tmp[j] = it->second->at(j);
		newSub->setCellList(nCellsTmp, tmp);
		newSub->setArea(cellWidth*cellWidth*nCellsTmp);
		m_subbasinsInfo[subID] = newSub;
	}
	vector<int>(m_subbasinIDs).swap(m_subbasinIDs);

	// release cellListMap to save memory
	for (it = cellListMap.begin(); it != cellListMap.end();)
	{
		if (it->second != NULL)
			delete it->second;
		it = cellListMap.erase(it);
	}
	cellListMap.clear();
}

clsSubbasins::~clsSubbasins()
{
	if (!m_subbasinsInfo.empty())
	{
		for (map<int, Subbasin *>::iterator iter = m_subbasinsInfo.begin(); iter != m_subbasinsInfo.end();)
		{
			if (iter->second != NULL)
				delete iter->second;
			iter = m_subbasinsInfo.erase(iter);
		}
		m_subbasinsInfo.clear();
	}
}

float clsSubbasins::subbasin2basin(string key)
{
	float temp = 0.f;
	int totalCount = 0;
	int nCount;
	Subbasin *sub = NULL;
	for (vector<int>::iterator it = m_subbasinIDs.begin(); it!=m_subbasinIDs.end();it++)
	{
		sub = m_subbasinsInfo[*it];
		nCount = sub->getCellCount();
		if (StringMatch(key, VAR_SLOPE))
			temp += atan(sub->getSlope()) * nCount;
		else if (StringMatch(key, VAR_PET))
			temp += sub->getPET() * nCount;
		else if (StringMatch(key, VAR_PERCO))
			temp += sub->getPerco() * nCount;
		else if (StringMatch(key, VAR_REVAP))
			temp += sub->getEG() * nCount;
		else if (StringMatch(key, VAR_PERDE))
			temp += sub->getPerde() * nCount;
		else if (StringMatch(key, VAR_RG))
			temp += sub->getRG() * nCount;
		else if (StringMatch(key, VAR_QG))
			temp += sub->getQG();
		else if (StringMatch(key, VAR_GW_Q))
			temp += sub->getGW() * nCount;
/*
		else if (StringMatch(key, "P"))
			temp += sub->getPCP() * nCount;
		else if (StringMatch(key, "NetP"))
			return m_pNet / m_cells.size();
		else if (StringMatch(key, "Interception"))
			return m_Interception / m_cells.size();
		else if (StringMatch(key, "InterceptionET"))
			return m_interceptionET / m_cells.size();
		else if (StringMatch(key, "DepressionET"))
			return m_DepressionET / m_cells.size();
		else if (StringMatch(key, "Infiltration"))
			return m_Infiltration / m_cells.size();
		else if (StringMatch(key, "Total_ET"))
			return m_TotalET / m_cells.size();
		else if (StringMatch(key, "Soil_ET"))
			return m_soilET / m_cells.size();
		else if (StringMatch(key, "Net_Percolation"))
			return m_NetPercolation / m_cells.size();
		else if (StringMatch(key, "Revap"))
			return m_Revap / m_cells.size();
		else if (StringMatch(key, "RS"))
			return m_RS / m_cells.size();
		else if (StringMatch(key, "RI"))
			return m_RI / m_cells.size();
		else if (StringMatch(key, "RG"))
			return m_RG / m_cells.size();
		else if (StringMatch(key, "R"))
			return m_R / m_cells.size();
		else if (StringMatch(key, "S_MOI"))
			return m_S_MOI / m_cells.size();
		else if (StringMatch(key, "T"))
			return m_T / m_cells.size();
		else if (StringMatch(key, "Soil_T"))
			return m_Soil_T / m_cells.size();
		else if (StringMatch(key, "MoistureDepth"))
			return m_moistureDepth / m_cells.size();*/
		
		totalCount += nCount;
	}
	if (StringMatch(key, VAR_QG))
		return temp;    // basin sum
	if(StringMatch(key, VAR_SLOPE))
		return tan(temp / totalCount);
	return temp / totalCount;  // basin average 
}
void clsSubbasins::SetSlopeCoefficient()
{
	float basinSlope = subbasin2basin(VAR_SLOPE);
	for (vector<int>::iterator it = m_subbasinIDs.begin(); it!=m_subbasinIDs.end();it++)
	{
		Subbasin *curSub = m_subbasinsInfo[*it];
		if(basinSlope <= 0.f)
			curSub->setSlopeCoefofBasin(1.f);
		else
		{
			float slpCoef = curSub->getSlope()/basinSlope;
			curSub->setSlopeCoefofBasin(slpCoef);
		}
	}
}