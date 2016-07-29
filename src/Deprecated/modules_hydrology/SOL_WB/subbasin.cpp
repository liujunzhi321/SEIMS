#include "subbasin.h"
#include "util.h"

subbasin::subbasin(int id)
{
    m_id = id;
    clear();
}

subbasin::~subbasin(void)
{

}

void subbasin::addCell(int cell)
{
    m_cells.push_back(cell);
}

void subbasin::clear()
{
    m_DepressionET = 0.0f;
    m_Infiltration = 0.0f;
    m_Interception = 0.0f;
    m_NetPercolation = 0.0f;
    m_P = 0.0f;
    m_R = 0.0f;
    m_Revap = 0.0f;
    m_RG = 0.0f;
    m_RI = 0.0f;
    m_RS = 0.0f;
    m_S_MOI = 0.0f;
    m_TotalET = 0.0f;
    m_soilET = 0.f;
    m_T = 0.0f;
    m_Soil_T = 0.0f;
    m_moistureDepth = 0.f;
    m_interceptionET = 0.f;
    m_pNet = 0.f;
}

int subbasin::getId()
{
    return m_id;
}

float subbasin::getAverage(string key)
{
    if (StringMatch(key, "P"))
        return m_P / m_cells.size();
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
        return m_moistureDepth / m_cells.size();
    else
        return -1.0f;
}

void subbasin::addP(float p)
{
    m_P += p;
}

void subbasin::addNetP(float pNet)
{
    m_pNet += pNet;
}

void subbasin::addDepressionET(float depressionET)
{
    m_DepressionET += depressionET;
}

void subbasin::addInterceptionET(float et)
{
    m_interceptionET += et;
}

void subbasin::addInfiltration(float infiltration)
{
    m_Infiltration += infiltration;
}

void subbasin::addInterception(float interception)
{
    m_Interception += interception;
}

void subbasin::addNetPercolation(float netPercolation)
{
    m_NetPercolation += netPercolation;
}

void subbasin::addR(float r)
{
    m_R += r;
}

void subbasin::addRevap(float revap)
{
    m_Revap += revap;
}

void subbasin::addRG(float rg)
{
    m_RG += rg;
}

void subbasin::addRI(float ri)
{
    m_RI += ri;
}

void subbasin::addRS(float rs)
{
    m_RS += rs;
}

void subbasin::addSMOI(float smoi)
{
    m_S_MOI += smoi;
}

void subbasin::addMoistureDepth(float depth)
{
    m_moistureDepth += depth;
}

void subbasin::addTotalET(float totalET)
{
    m_TotalET += totalET;
}

void subbasin::addSoilET(float soilET)
{
    m_soilET += soilET;
}

void subbasin::addT(float t)
{
    m_T += t;
}

void subbasin::addSoilT(float soilT)
{
    m_Soil_T += soilT;
}


vector<int> *subbasin::getCells()
{
    return &m_cells;
}
