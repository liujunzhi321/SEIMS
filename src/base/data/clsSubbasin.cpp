#include "clsSubbasin.h"
#include "util.h"
#include <iostream>
#include <sstream>
#include "ModelException.h"

using namespace std;

Subbasin::Subbasin(int id, bool isOutput) : m_id(id), m_isOutput(isOutput),m_isRevapChanged(true), 
	                                        m_nCells(-1), m_cells(NULL), m_cellArea(-1.f),m_Area(-1.f), 
											m_nSoilLayers(NULL),m_slope(-1.f), m_slopeCoefficient(1.f), 
											m_GWMAX(-1.f),m_kg(-1.f),m_dp_co(-1.f),m_base_ex(-1.f),m_QGCoefficient(-1.f),
											m_EG(-1.f), m_GW(-1.f), m_PERCO(-1.f),m_PERDE(-1.f), 
											m_PET(-1.f),m_QG(-1.f), m_RG(-1.f)        
{
}
Subbasin::Subbasin(int id) : m_id(id), m_isRevapChanged(true), 
	m_nCells(-1), m_cells(NULL), m_cellArea(-1.f),m_Area(-1.f), 
	m_nSoilLayers(NULL),m_slope(-1.f), m_slopeCoefficient(1.f), 
	m_GWMAX(-1.f),m_kg(-1.f),m_dp_co(-1.f),m_base_ex(-1.f),m_QGCoefficient(-1.f),
	m_EG(-1.f), m_GW(-1.f), m_PERCO(-1.f),m_PERDE(-1.f), 
	m_PET(-1.f),m_QG(-1.f), m_RG(-1.f)        
{
}
Subbasin::~Subbasin(void)
{
	// There seems no variables need to be released! By LJ
    //if (m_cells != NULL) Release1DArray(m_cells);
    //    //delete m_cells;
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
    //m_nCells = nCells;
    m_Area = m_nCells * m_cellArea;
}
void Subbasin::setSoilLayers(int nCells, int *soilLayers)
{
	CheckInputSize(nCells);
	m_nSoilLayers = soilLayers;
}
void Subbasin::setParameter(float rv_co, float GW0, float GWMAX, float kg, float base_ex, float cellWidth, int timeStep)
{
    m_dp_co = rv_co;
    m_GW = GW0;
    m_GWMAX = GWMAX;
    m_kg = kg;
    m_base_ex = base_ex;
    m_cellArea = cellWidth * cellWidth;
    m_QGCoefficient = 1.f * cellWidth * cellWidth / (timeStep) / 1000.f;
}
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

void Subbasin::setSlopeCoefofBasin(float slopeBasin)
{
    if (slopeBasin <= 0.f)
        m_slopeCoefficient = 1.f;
    else
        m_slopeCoefficient = m_slope / slopeBasin;
}

void Subbasin::setInputs(float *PET, float *EI, float *ED, float *ES, float **PERCO, float groundwaterFromBankStorage)
{
    m_PERCO = 0.f;
    int index = 0;
    //cout << "start setInputs\t" << m_nCells << endl;
    for (int i = 0; i < m_nCells; i++)
    {
        index = m_cells[i];
        //cout << i << "\t" << index << endl;
		// percolation amount of current subbasin is accumulated by
		// the percolation water of the bottom soil layer of each valid cell
        //m_PERCO += PERCO[index][1]; // the previous version has a fixed soil layers of 2
		if(m_nSoilLayers == NULL)
			throw ModelException("Subbasin Class", "setInputs", "Soil layers number can not be NULL, please setSoilLayers first!");
		m_PERCO += PERCO[index][m_nSoilLayers[i]-1];
    }
    m_PERCO /= m_nCells;
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
    for (int i = 0; i < m_nCells; i++)
    {
        index = m_cells[i];
        fPET += PET[index];
        fEI += EI[index];
        fED += ED[index];
        fES += ES[index];
    }
    fPET /= m_nCells;
    fEI /= m_nCells;
    fED /= m_nCells;
    fES /= m_nCells;

    m_PET = fPET;
    //cout << "PET:\t" << m_PET << endl;

    if (m_PERCO >= 0.01f)
    {
        revap = (fPET - fEI - fED - fES) * m_GW / m_GWMAX;    //revap
        revap = max(revap, 0.f);
        revap = min(revap, m_PERCO);
    }


    if (m_EG != revap)
    {
        m_EG = revap;
        m_isRevapChanged = true;
    }
    else
    {
        m_isRevapChanged = false;
    }
    //cout << m_PERCO << "\t" << m_GW << "\t" << endl;

    m_PERDE = m_PERCO * m_dp_co; //deep percolation

    float kg = m_kg * m_slopeCoefficient;
    m_RG = kg * pow(m_GW, m_base_ex);        //base flow
    m_QG = m_RG * m_nCells * m_QGCoefficient;
    //cout << "QG:\t" << m_QG << endl;

    float gwOld = m_GW;
    m_GW = m_GW + m_PERCO - m_EG - m_PERDE - m_RG;
	//add the ground water from bank storage, 2011-3-14
    m_GW += groundwaterFromBankStorage / m_Area * 1000.f;
    m_GW = max(m_GW, 0.f);
    if (m_GW > m_GWMAX)
    {
        m_QG += (m_GW - m_GWMAX);
        m_RG = m_QG / (m_nCells * m_QGCoefficient);
        m_GW = m_GWMAX;
    }

    if (m_GW != m_GW)
    {
        ostringstream oss;
        oss << m_PERCO << "\t" << m_EG << "\t" << m_PERDE << "\t" << m_RG << "\t" << gwOld << "\t" << m_kg << "\t" <<
        m_base_ex << "\t" << m_slopeCoefficient << endl;
        throw ModelException("Subbasin", "setInputs", oss.str());
    }
}



float Subbasin::subbasin2basin(string key, vector<Subbasin *> *subbasins)
{
    float temp = 0.f;
    int totalCount = 0;
    int nCount;
    Subbasin *sub = NULL;
    for (size_t i = 0; i < subbasins->size(); i++)
    {
        sub = subbasins->at(i);
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

        totalCount += nCount;
    }
    if (StringMatch(key, VAR_QG))
        return temp;    // basin sum
	if(StringMatch(key, VAR_SLOPE))
		return tan(temp / totalCount);
    return temp / totalCount;  // basin average 
}
