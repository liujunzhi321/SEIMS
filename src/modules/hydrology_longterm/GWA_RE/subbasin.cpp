#include "subbasin.h"
#include "util.h"
#include <iostream>
#include <sstream>
#include "ModelException.h"

using namespace std;

subbasin::subbasin(int id, bool isOutput) : m_id(id), m_isOutput(isOutput),
                                            m_isRevapChanged(true), m_EG(-1.0f), m_Area(0.f),
                                            m_cells(NULL), m_QG(0.f), m_RG(0.f), m_GW(0.f),
                                            m_PERDE(0.f), m_PERCO(0.f)
{
}

subbasin::~subbasin(void)
{
    if (m_cells != NULL)
        delete m_cells;
}

void subbasin::setCellList(int nCells, int *cells)
{
    m_cells = cells;
    m_nCells = nCells;
    m_Area = nCells * m_cellArea;
}

int subbasin::getId()
{
    return m_id;
}

void subbasin::setParameter(float rv_co, float GW0, float GWMAX, float kg, float base_ex, int cellWidth, int timeStep)
{
    m_dp_co = rv_co;
    m_GW = GW0;
    m_GWMAX = GWMAX;
    m_kg = kg;
    m_base_ex = base_ex;
    m_cellArea = cellWidth * cellWidth;
    m_QGCoefficient = 1.0f * cellWidth * cellWidth / (timeStep) / 1000.0f;
}

void subbasin::setSlope(float *slope)
{
    m_slope = 0.0f;
    int index = 0;
    for (int i = 0; i < m_nCells; i++)
    {
        index = m_cells[i];
        m_slope += slope[index];
    }
    m_slope /= m_nCells;
}

void subbasin::setSlopeBasin(float slopeBasin)
{
    if (slopeBasin <= 0.f)
        m_slopeCoefficient = 1.f;
    else
        m_slopeCoefficient = m_slope / slopeBasin;
}

void subbasin::setInputs(float *PET, float *EI, float *ED, float *ES, float **PERCO, float groundwaterFromBankStorage)
{
    m_PERCO = 0.0f;
    int index = 0;
    //cout << "start setInputs\t" << m_nCells << endl;
    for (int i = 0; i < m_nCells; i++)
    {
        index = m_cells[i];
        //cout << i << "\t" << index << endl;
        m_PERCO += PERCO[index][1];
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

    if (m_PERCO >= 0.01)
    {
        revap = (fPET - fEI - fED - fES) * m_GW / m_GWMAX;    //revap
        revap = max(revap, 0.0f);
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
    m_GW += groundwaterFromBankStorage / m_Area * 1000.0f;//add the ground water from bank storage, 2011-3-14
    m_GW = max(m_GW, 0.0f);
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
        throw ModelException("subbasin", "setInputs", oss.str());
    }
}

float  subbasin::getQG()
{
    return m_QG;
}

float subbasin::getEG()
{
    return m_EG;
}

float subbasin::getRG()
{
    return m_RG;
}

float subbasin::getGW()
{
    return m_GW;
}

float  subbasin::getSlope()
{
    return m_slope;
}

int    subbasin::getCellCount()
{
    return m_nCells;
}

float subbasin::getPerco()
{
    return m_PERCO;
}

float subbasin::getPET()
{
    return m_PET;
}

float subbasin::getPerde()
{
    return m_PERDE;
}

bool subbasin::getIsOutput()
{
    return m_isOutput;
}

bool subbasin::getIsRevapChanged()
{
    return m_isRevapChanged;
}

float subbasin::subbasin2basin(string key, vector<subbasin *> *subbasins)
{
    float temp = 0.0f;
    int totalCount = 0;
    int nCount;
    subbasin *sub = NULL;
    for (int i = 0; i < subbasins->size(); i++)
    {
        sub = subbasins->at(i);
        nCount = sub->getCellCount();
        if (StringMatch(key, "Slope"))
            temp += sub->getSlope() * nCount;
        else if (StringMatch(key, "PET"))
            temp += sub->getPET() * nCount;
        else if (StringMatch(key, "PERCO"))
            temp += sub->getPerco() * nCount;
        else if (StringMatch(key, "REVAP"))
            temp += sub->getEG() * nCount;
        else if (StringMatch(key, "PERDE"))
            temp += sub->getPerde() * nCount;
        else if (StringMatch(key, "RG"))
            temp += sub->getRG() * nCount;
        else if (StringMatch(key, "QG"))
            temp += sub->getQG();
        else if (StringMatch(key, "GW"))
            temp += sub->getGW() * nCount;

        totalCount += nCount;
    }
    if (StringMatch(key, "QG"))
        return temp;    //sum
    return temp / totalCount;                    //average
}

int *subbasin::getCells()
{
    return m_cells;
}
