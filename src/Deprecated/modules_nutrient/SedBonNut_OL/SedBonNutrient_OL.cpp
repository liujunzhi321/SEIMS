/*----------------------------------------------------------------------
*	Purpose: 	Overland routing of nutrients using Knisel equation
*
*	Created:	Wang Lin
*	Date:		1-Aug-2012
*
*	Revision:
*   Date:
*---------------------------------------------------------------------*/

//#include "vld.h"
#include "SedBonNutrient_OL.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>
#include <cmath>
#include <iostream>

using namespace std;

//#define MINI_SLOPE 0.0001f

SedBonNutrient_OL::SedBonNutrient_OL(void) : m_nCells(-1), m_dt(-1.0f), m_CellWidth(-1.0f), m_initConcP(-1.0f),
                                             m_initConcNO3(-0.f), m_initConcNH4(-0.f),
                                             m_clayFrac(NULL), m_Porosity(NULL), m_Density(NULL), m_soilMoisture(NULL),
                                             m_flowInIndex(NULL), m_routingLayers(NULL), m_nLayers(-1), m_CELLQ(NULL),
                                             m_CELLH(NULL), m_infil(NULL), m_Slope(NULL), m_sd(NULL), m_SedBonP(NULL),
                                             m_SedBonAmmonium(NULL), m_SedBonPToCh(NULL), m_SedBonAmmoniumToCh(NULL),
                                             m_flowWidth(NULL), m_streamLink(NULL), m_CP_Sed(NULL), m_CNH4_Sed(NULL),
                                             m_ChV(NULL), m_whtoCh(NULL), m_PreConcP(NULL), m_PreConcNH4(NULL),
                                             m_PreConcNO3(NULL), m_SedKG(NULL)
{
    omp_set_num_threads(1);
}

SedBonNutrient_OL::~SedBonNutrient_OL(void)
{
    if (m_SedBonP != NULL)
        delete[] m_SedBonP;
    if (m_SedBonAmmonium != NULL)
        delete[] m_SedBonAmmonium;
    if (m_SedBonPToCh != NULL)
        delete[] m_SedBonPToCh;
    if (m_SedBonAmmoniumToCh != NULL)
        delete[] m_SedBonAmmoniumToCh;
    if (m_CP_Sed != NULL)
        delete[] m_CP_Sed;
    if (m_CNH4_Sed != NULL)
        delete[] m_CNH4_Sed;
    if (m_PreConcP != NULL)
        delete[] m_PreConcP;
    if (m_PreConcNH4 != NULL)
        delete[] m_PreConcNH4;
    if (m_PreConcNO3 != NULL)
        delete[] m_PreConcNO3;

    //test
    if (m_ChV != NULL)
        delete[] m_ChV;
}

bool SedBonNutrient_OL::CheckInputData(void)
{
    if (this->m_date <= 0)
    {
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "You have not set the Date variable.");
        return false;
    }

    if (this->m_nCells <= 0)
    {
        throw ModelException("SedBonNutrient_OL", "CheckInputData",
                             "The cell number of the input can not be less than zero.");
        return false;
    }

    if (this->m_dt <= 0)
    {
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "You have not set the TimeStep variable.");
        return false;
    }

    if (this->m_CellWidth <= 0)
    {
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "You have not set the CellWidth variable.");
        return false;
    }
    if (this->m_Slope <= 0)
    {
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "You have not set the slope.");
        return false;
    }
    //if(this->m_initConcP <= 0)
    //{
    //	throw ModelException("SedBonNutrient_OL","CheckInputData","You have not set the initial average concentration of nutrient-P variable.");
    //	return false;
    //}
    //if(this->m_initConcNH4 <= 0)
    //{
    //	throw ModelException("SedBonNutrient_OL","CheckInputData","You have not set the initial average concentration of nutrient-NH4 variable.");
    //	return false;
    //}
    //if(this->m_initConcNO3 <= 0)
    //{
    //	throw ModelException("SedBonNutrient_OL","CheckInputData","You have not set the initial average concentration of nutrient-NO3 variable.");
    //	return false;
    //}
    if (m_clayFrac == NULL)
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "The parameter: clay fraction has not been set.");
    if (m_Porosity == NULL)
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "The parameter: soil porosity has not been set.");
    if (m_soilMoisture == NULL)
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "The parameter: soil moisture has not been set.");
    if (m_Density == NULL)
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "The parameter: soil density has not been set.");
    if (m_flowInIndex == NULL)
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "The parameter: flow in index has not been set.");
    if (m_routingLayers == NULL)
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "The parameter: routingLayers has not been set.");
    if (this->m_chWidth == NULL)
    {
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "Channel width can not be NULL.");
        return false;
    }
    if (this->m_flowWidth == NULL)
    {
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "Flow width can not be NULL.");
        return false;
    }
    if (this->m_whtoCh == NULL)
    {
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "Flow to channel can not be NULL.");
        return false;
    }
    if (this->m_streamLink == NULL)
    {
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "Flow length can not be NULL.");
        return false;
    }

    if (m_CELLQ == NULL)
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "The parameter: D_CELLQ has not been set.");
    if (m_CELLH == NULL)
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "The parameter: D_CELLH has not been set.");
    if (m_infil == NULL)
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "The parameter: D_INFIL has not been set.");
    if (m_sd == NULL)
        throw ModelException("SedBonNutrient_OL", "CheckInputData",
                             "The parameter: D_DPST(depression storage) has not been set.");
    if (m_SedKG == NULL)
        throw ModelException("SedBonNutrient_OL", "CheckInputData", "The parameter: D_SedKG has not been set.");
    return true;
}

void  SedBonNutrient_OL::initialOutputs()
{
    if (this->m_nCells <= 0)
        throw ModelException("SedBonNutrient_OL", "initialOutputs",
                             "The cell number of the input can not be less than zero.");

    if (m_SedBonP == NULL)
    {
        m_initConcP = 0.001;
        m_initConcNH4 = 0.001;
        m_initConcNO3 = 0.001;

        m_SedBonP = new float[m_nCells];
        m_SedBonAmmonium = new float[m_nCells];
        m_SedBonPToCh = new float[m_nCells];
        m_SedBonAmmoniumToCh = new float[m_nCells];

        m_CP_Sed = new float[m_nCells];
        m_CNH4_Sed = new float[m_nCells];

        m_PreConcP = new float[m_nCells];
        m_PreConcNH4 = new float[m_nCells];
        m_PreConcNO3 = new float[m_nCells];

        m_PPT_P = new float[m_nCells];
        m_PPT_NH4 = new float[m_nCells];

        m_ChV = new float[m_nCells];
        m_cellA = new float[m_nCells];
#pragma omp parallel for
        for (int i = 0; i < m_nCells; ++i)
        {
            m_SedBonP[i] = 0.0f;
            m_SedBonAmmonium[i] = 0.0f;
            m_SedBonPToCh[i] = 0.0f;
            m_SedBonAmmoniumToCh[i] = 0.f;
            m_CP_Sed[i] = 0.0f;
            m_CNH4_Sed[i] = 0.0f;
            m_PreConcP[i] = m_initConcP;
            m_PreConcNH4[i] = m_initConcNH4;

            m_PPT_P[i] = 0.0f;
            m_PPT_NH4[i] = 0.0f;
            m_ChV[i] = 0.0f;
            //cell area
            float s = max(0.001f, m_Slope[i]);
            float cellareas = (m_CellWidth / cos(atan(s))) * m_CellWidth;
            m_cellA[i] = cellareas;
        }
    }

    this->CalcuVelocityChannelFlow();
}

void SedBonNutrient_OL::concentration(int id)
{
    float Fcl = m_clayFrac[id];
    float K_NH4 = 1.34f + 0.038f * Fcl;    // partition coefficient
    float K_P = 100 + 2.5f * Fcl;     // partition coefficient
    // sendiment bound nutrient concentration
    //concentration at the end of the time step
    float Q = m_CELLQ[id];   //m3/s
    float h = m_CELLH[id] * m_cellA[id] / 1000;  //m3
    float f = m_infil[id] * m_cellA[id] / 1000;  //m3
    float S = m_sd[id] * m_cellA[id] / 1000;     //m3
    float p = m_Porosity[id];
    if (S != 0)
    {
        if (h == 0)
        {
            h = m_whtoCh[id];
        }
        m_X2 = (h + f) / S;
        m_X3 = h / S;
    }
    else
    {
        m_X2 = 0;
        m_X3 = 0;
    }
    float temp1 = Q * m_dt + f + S;    //m3
    float temp2 = 2.65f * (1 - p);
    float X_NH4 = temp1 / (p + temp2 * K_NH4);
    float Cfin_NH4 = m_PreConcNH4[id] * exp(-X_NH4);   //kg/kg-soil

    float X_P = temp1 / (p + temp2 * K_P);
    float Cfin_P = m_PreConcP[id] * exp(-X_P);    //kg/kg-soil

    float V = m_soilMoisture[id] * 1000;    //kg/m3
    float M = m_Density[id];    //kg/m3
    float P0_NH4 = K_NH4 * M * Cfin_NH4 / V;    //kg/kg-soil
    float P0_P = K_NH4 * M * Cfin_NH4 / V;    //kg/kg-soil
    // newly generated sediment-bound nutrient (kg/s)
    float Sed = m_SedKG[id] / m_dt;   //newly generated sediment (kg/s)
    m_CP_Sed[id] = P0_P * Sed;
    m_CNH4_Sed[id] = P0_NH4 * Sed;
}


void SedBonNutrient_OL::CalcuVelocityChannelFlow()
{
    const float beta = 0.6f;
    const float _23 = 2.0f / 3;
    float Perim, R, S, n;

    for (int i = 0; i < m_nCells; i++)
    {
        if (m_whtoCh[i] > 0.0001f)
        {
            Perim = 2 * m_whtoCh[i] / 1000 + m_chWidth[i];    // mm to m -> /1000
            if (Perim > 0)
                R = m_whtoCh[i] / 1000 * m_chWidth[i] / Perim;
            else
                R = 0.0f;
            S = sin(atan(m_Slope[i]));   //sine of the slope
            S = max(0.001f, S);
            n = m_ManningN[i];  //manning n
            m_ChV[i] = pow(R, _23) * sqrt(S) / n;
        }
        else
            m_ChV[i] = 0;
    }
}

void SedBonNutrient_OL::OverlandFlow(int id)
{
    concentration(id);
    //sum the upstream overland flow
    float flowwidth = m_flowWidth[id];
    float PUp = 0.0f;
    float NH4Up = 0.0f;
    for (int k = 1; k <= (int) m_flowInIndex[id][0]; ++k)
    {
        int flowInID = (int) m_flowInIndex[id][k];
        PUp += m_CP_Sed[flowInID];
        NH4Up += m_CNH4_Sed[flowInID];
    }
    // if the channel width is greater than the cell width
    if (m_streamLink[id] >= 0 && flowwidth <= 0)
    {
        m_SedBonPToCh[id] = PUp * m_dt;  // convert to kg
        m_SedBonAmmoniumToCh[id] = NH4Up * m_dt;
        return;
    }

    //An initial test is done on the discharge at the end of the time interval.  When the discharge is equal to zero
    if (m_CP_Sed[id] == 0)
    {
        float Pinflow = PUp;
        float NH4inflow = NH4Up;
        //The amount of sediment bound nutrient in transit
        float Ptransit = (m_PPT_P[id] + Pinflow) / 2;
        float NH4transit = (m_PPT_NH4[id] + NH4inflow) / 2;
        //The outflow of sediment bound nutrient  kg/s
        m_SedBonP[id] = 0;
        m_SedBonAmmonium[id] = 0;
        //The new value of PPT
        m_PPT_P[id] = Pinflow;
        m_PPT_NH4[id] = NH4inflow;
    }
        //When the discharge is positive
    else
    {
        float Pinflow = PUp + m_CP_Sed[id];
        float NH4inflow = NH4Up + m_CNH4_Sed[id];
        //The amount of sediment bound nutrient in transit
        float Ptransit = (m_PPT_P[id] + Pinflow) / (1 + m_X1);
        float NH4transit = (m_PPT_NH4[id] + NH4inflow) / (1 + m_X1);
        //The outflow of sediment bound nutrient  kg/s
        float Q = m_CELLQ[id];   //m3/s
        float S = m_sd[id] * m_cellA[id] / 1000;     //m3
        m_SedBonP[id] = (Ptransit * Q) / S;
        m_SedBonAmmonium[id] = (NH4transit * Q) / S;
        //The new value of PPT
        float ratio = max(0.0f, 1 - m_X1);
        m_PPT_P[id] = Pinflow + m_SedBonP[id] * ratio;
        m_PPT_NH4[id] = NH4inflow + m_SedBonAmmonium[id] * ratio;
    }
    // compute nutrients to channel
    float fractiontochannel = 0.0f;
    if (m_chWidth[id] > 0)
    {
        float tem = m_ChV[id] * m_dt;
        fractiontochannel = 2 * tem / (m_CellWidth - m_chWidth[id]);
        fractiontochannel = min(fractiontochannel, 1.0f);
        // convert to kg
        m_SedBonPToCh[id] = m_SedBonP[id] * m_dt * fractiontochannel;
        m_SedBonAmmoniumToCh[id] = m_SedBonAmmonium[id] * m_dt * fractiontochannel;
    }
}

int SedBonNutrient_OL::Execute()
{
    //check the data
    CheckInputData();

    initialOutputs();

    for (int iLayer = 0; iLayer < m_nLayers; ++iLayer)
    {
        // There are not any flow relationship within each routing layer.
        // So parallelization can be done here.
        int nCells = (int) m_routingLayers[iLayer][0];
        //omp_set_num_threads(2);
#pragma omp parallel for
        for (int iCell = 1; iCell <= nCells; ++iCell)
        {
            int id = (int) m_routingLayers[iLayer][iCell];
            OverlandFlow(id);
        }
    }
    return 0;
}

bool SedBonNutrient_OL::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
    {
        //this->StatusMsg("Input data for "+string(key) +" is invalid. The size could not be less than zero.");
        return false;
    }
    if (this->m_nCells != n)
    {
        if (this->m_nCells <= 0) this->m_nCells = n;
        else
        {
            //this->StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
            ostringstream oss;
            oss << "Input data for " + string(key) << " is invalid with size: " << n << ". The origin size is " <<
            m_nCells << ".\n";
            throw ModelException("SedBonNutrient_OL", "CheckInputSize", oss.str());
        }
    }

    return true;
}

void SedBonNutrient_OL::SetValue(const char *key, float data)
{
    string sk(key);
    if (StringMatch(sk, "STORM_DT"))
        m_dt = data;
    else if (StringMatch(sk, Tag_CellWidth))
        m_CellWidth = data;
    else if (StringMatch(sk, Tag_CellSize))
        m_nCells = (int) data;
        /*else if (StringMatch(sk, "InitConc_P"))
            m_initConcP = data;
        else if (StringMatch(sk, "InitConc_NH4"))
            m_initConcNH4 = data;
        else if (StringMatch(sk, "InitConc_NO3"))
            m_initConcNO3 = data;*/
    else if (StringMatch(sk, VAR_OMP_THREADNUM))
    {
        omp_set_num_threads((int) data);
    }
    else
        throw ModelException("SedBonNutrient_OL", "SetSingleData", "Parameter " + sk
                                                                   +
                                                                   " does not exist. Please contact the module developer.");

}

void SedBonNutrient_OL::Set1DData(const char *key, int n, float *data)
{
    //check the input data
    CheckInputSize(key, n);
    string sk(key);
    if (StringMatch(sk, "Slope"))
        m_Slope = data;
    else if (StringMatch(sk, "Manning"))
        m_ManningN = data;
    else if (StringMatch(sk, "Clay"))
        m_clayFrac = data;
    else if (StringMatch(sk, "Porosity"))
        m_Porosity = data;
    else if (StringMatch(sk, "Density"))
        m_Density = data;
    else if (StringMatch(sk, "D_FlowWidth"))
        m_flowWidth = data;
    else if (StringMatch(sk, "D_HTOCH"))
        m_whtoCh = data;
    else if (StringMatch(sk, "STREAM_LINK"))
        m_streamLink = data;
    else if (StringMatch(sk, "D_CELLQ"))
        m_CELLQ = data;
    else if (StringMatch(sk, "D_CELLH"))
        m_CELLH = data;
    else if (StringMatch(sk, "D_INFIL"))
        m_infil = data;
    else if (StringMatch(sk, "D_DPST"))
        m_sd = data;
    else if (StringMatch(sk, "CHWIDTH"))
        m_chWidth = data;
    else
        throw ModelException("SedBonNutrient_OL", "Set1DData", "Parameter " + sk
                                                               +
                                                               " does not exist. Please contact the module developer.");

}

//void SedBonNutrient_OL::GetValue(const char* key, float* data)
//{
//	string sk(key);
//	if(StringMatch(sk, "ID_OUTLET"))
//		*data = m_idOutlet;
//	else
//		throw ModelException("SedBonNutrient_OL", "GetValue", "Output " + sk 
//		+ " does not exist in the SedBonNutrient_OL module. Please contact the module developer.");
//	
//}

void SedBonNutrient_OL::Get1DData(const char *key, int *n, float **data)
{
    initialOutputs();

    string sk(key);
    *n = m_nCells;
    if (StringMatch(sk, "DissovP"))
        *data = m_SedBonP;
    else if (StringMatch(sk, "Ammonium"))
        *data = m_SedBonAmmonium;
    else if (StringMatch(sk, "DissovPToCh"))
        *data = m_SedBonPToCh;
    else if (StringMatch(sk, "AmmoniumToCh"))
        *data = m_SedBonAmmoniumToCh;
    else if (StringMatch(sk, "TESTDPST"))
        *data = m_CELLH;
    else
        throw ModelException("SedBonNutrient_OL", "Get1DData", "Output " + sk
                                                               +
                                                               " does not exist in the SedBonNutrient_OL module. Please contact the module developer.");
}

void SedBonNutrient_OL::Set2DData(const char *key, int nrows, int ncols, float **data)
{
    //check the input data
    //m_nLayers = nrows;
    string sk(key);
    if (StringMatch(sk, "Routing_Layers"))
    {
        m_routingLayers = data;
        m_nLayers = nrows;
    }
    else if (StringMatch(sk, "FlowIn_Index_D8"))
        m_flowInIndex = data;
    else
        throw ModelException("SedBonNutrient_OL", "Set2DData", "Parameter " + sk
                                                               +
                                                               " does not exist. Please contact the module developer.");
}

