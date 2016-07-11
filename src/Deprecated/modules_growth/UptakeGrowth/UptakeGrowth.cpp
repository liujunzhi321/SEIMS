#include <iostream>
#include "UptakeGrowth.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "utils.h"
#include "util.h"
#include "Common.h"
//#include "Climate.h"
#include <omp.h>
#include <cmath>
#include <cstdlib>

using namespace std;

UptakeGrowth::UptakeGrowth(void) : m_nCells(-1), m_curYear(10.f),//parameter
                                   m_initBiomass(10.f), m_PHU(1500.f), m_tBase(NULL), m_tOpt(NULL),
                                   m_frN1(NULL), m_frN2(NULL), m_frN3(NULL), m_frP1(NULL), m_frP2(NULL), m_frP3(NULL),
                                   m_maxLAI(NULL),
                                   m_classification(NULL), m_NFixFactor(NODATA), m_NFixDailyMax(NODATA),
                                   m_waterDis(NODATA), m_waterCom(NODATA), m_NDis(NODATA),
                                   m_PDis(NODATA), m_layers(NODATA), m_rootDepthmax(NULL), m_layerFieldCapacity(NULL),
                                   m_layerWiltingPnt(NULL), m_frDPHU(NULL), m_maxBiomass(NULL), m_fullDevYear(NULL),
                                   m_zRoot(NULL), m_WnormalizationParam(NULL), m_NnormalizationParam(NULL),
                                   m_PnormalizationParam(NULL), m_waterLastDepth(NULL),
                                   m_waterThisDepth(NULL), m_waterTotal(NULL), m_layerAvailableWaterCapacity(NULL),
                                   m_potentialNUptake(NULL), m_potentialPUptake(NULL),
                                   m_NLastDepth(NULL), m_NThisDepth(NULL), m_NTotal(NULL), m_PLastDepth(NULL),
                                   m_PThisDepth(NULL), m_PTotal(NULL),
                                   m_fGrowthStage(NULL), m_fSoilWater(NULL), m_fSoilNitrate(NULL),
                                   m_totalSoilWater(NULL), m_totalNO3(NULL), m_totalFieldCapacity(NULL),
        //input
                                   m_LAIdelta(NULL), m_LAI(NULL), m_preLAI(NULL), m_frPHU(NULL), m_frN(NULL),
                                   m_frP(NULL), m_frRoot(NULL),
                                   m_biomassDelta(NULL), m_biomass(NULL), m_biomassNOpt(NULL), m_biomassPOpt(NULL),
                                   m_layerNO3(NULL), m_layerP(NULL), m_tMax(NULL), m_tMin(NULL), m_PET(NULL),
                                   m_layerSoilWater(NULL),

        //result
                                   m_bioTreeAnnual(NULL), m_biomassN(NULL), m_biomassP(NULL), m_biomassRoot(NULL),
                                   m_biomassAG(NULL), m_transpirationMax(NULL), m_depthArray(NULL),
                                   m_layerWUptake(NULL), m_layerNUptake(NULL), m_layerPUptake(NULL), m_NFixation(NULL),
                                   m_totalWUptake(NULL), m_totalNUptake(NULL),
                                   m_totalPUptake(NULL),//m_aerationStress(NULL),
                                   m_waterStress(NULL), m_tempStress(NULL), m_NStress(NULL), m_PStress(NULL),
                                   m_totalStress(NULL)
{
    // set default values for member variables
    m_common = new Common();
    utils utl;
    m_startDate = utl.ConvertToTime2("1970-04-20 00:00:00", "%d-%d-%d %d:%d:%d", true);
    m_endDate = utl.ConvertToTime2("1970-10-02 00:00:00", "%d-%d-%d %d:%d:%d", true);
}

UptakeGrowth::~UptakeGrowth(void)
{
    if (m_zRoot != NULL)
        delete[] m_zRoot;
    if (m_WnormalizationParam != NULL)
        delete[] m_WnormalizationParam;
    if (m_NnormalizationParam != NULL)
        delete[] m_NnormalizationParam;
    if (m_PnormalizationParam != NULL)
        delete[] m_PnormalizationParam;

    if (m_waterLastDepth != NULL)
        delete[] m_waterLastDepth;
    if (m_waterThisDepth != NULL)
        delete[] m_waterThisDepth;
    if (m_NLastDepth != NULL)
        delete[] m_NLastDepth;
    if (m_NThisDepth != NULL)
        delete[] m_NThisDepth;
    if (m_PLastDepth != NULL)
        delete[] m_PLastDepth;
    if (m_PThisDepth != NULL)
        delete[] m_PThisDepth;


    if (m_potentialNUptake != NULL)
        delete[] m_potentialNUptake;
    if (m_potentialPUptake != NULL)
        delete[] m_potentialPUptake;

    if (m_waterTotal != NULL)
        delete[] m_waterTotal;
    if (m_NTotal != NULL)
        delete[] m_NTotal;
    if (m_PTotal != NULL)
        delete[] m_PTotal;

    if (m_fGrowthStage != NULL)
        delete[] m_fGrowthStage;
    if (m_fSoilWater != NULL)
        delete[] m_fSoilWater;
    if (m_fSoilNitrate != NULL)
        delete[] m_fSoilNitrate;


    if (m_biomassN != NULL)
        delete[] m_biomassN;
    if (m_biomassP != NULL)
        delete[] m_biomassP;
    if (m_biomassRoot != NULL)
        delete[] m_biomassRoot;
    if (m_biomassAG != NULL)
        delete[] m_biomassAG;
    if (m_bioTreeAnnual != NULL)
        delete[] m_bioTreeAnnual;

    if (m_transpirationMax != NULL)
        delete[] m_transpirationMax;
    if (m_depthArray != NULL)
        delete[] m_depthArray;

    if (m_totalWUptake != NULL)
        delete[] m_totalWUptake;
    if (m_totalNUptake != NULL)
        delete[] m_totalNUptake;
    if (m_totalPUptake != NULL)
        delete[] m_totalPUptake;
    if (m_NFixation != NULL)
        delete[] m_NFixation;

    if (m_totalFieldCapacity != NULL)
        delete[] m_totalFieldCapacity;
    if (m_totalNO3 != NULL)
        delete[] m_totalNO3;
    if (m_totalSoilWater != NULL)
        delete[] m_totalSoilWater;

    // cleanup float ** 
    if (m_layerWUptake != NULL)
    {
        for (int i = 0; i < m_layers; i++)
        {
            if (m_layerWUptake[i] != NULL)
                delete[] m_layerWUptake[i];
        }
        delete[] m_layerWUptake;
    }
    if (m_layerNUptake != NULL)
    {
        for (int i = 0; i < m_layers; i++)
        {
            if (m_layerNUptake[i] != NULL)
                delete[] m_layerNUptake[i];
        }
        delete[] m_layerNUptake;
    }
    if (m_layerPUptake != NULL)
    {
        for (int i = 0; i < m_layers; i++)
        {
            if (m_layerPUptake[i] != NULL)
                delete[] m_layerPUptake[i];
        }
        delete[] m_layerPUptake;
    }


    if (m_waterStress != NULL)
        delete[] m_waterStress;
    if (m_tempStress != NULL)
        delete[] m_tempStress;
    if (m_NStress != NULL)
        delete[] m_NStress;
    if (m_PStress != NULL)
        delete[] m_PStress;
    if (m_totalStress != NULL)
        delete[] m_totalStress;

}

bool UptakeGrowth::CheckInputData(void)
{
    if (m_date <= 0)
        throw ModelException("UptakeGrowth", "CheckInputData", "You have not set the time.");
    if (m_nCells <= 0)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    if (m_classification == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData", "The  land cover/plant classification can not be NULL.");
    if (FloatEqual(m_waterDis, NODATA))
        throw ModelException("UptakeGrowth", "CheckInputData", "The water-use distribution parameter  can not be -99.");
    if (FloatEqual(m_waterCom, NODATA))
        throw ModelException("UptakeGrowth", "CheckInputData", "The plant uptake compensation factor  can not be -99.");
    if (FloatEqual(m_NFixFactor, NODATA))
        throw ModelException("UptakeGrowth", "CheckInputData", "The nitrogen fixation factor can not be -99.");
    if (FloatEqual(m_NFixDailyMax, NODATA))
        throw ModelException("UptakeGrowth", "CheckInputData", "The maximum daily-n fixation can not be -99.");
    if (FloatEqual(m_NDis, NODATA))
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The nitrogen uptake distribution parameter can not be -99.");
    if (FloatEqual(m_PDis, NODATA))
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The phosphorus uptake distribution parameter can not be -99.");
    if (FloatEqual(m_layers, NODATA))
        throw ModelException("UptakeGrowth", "CheckInputData", "The num of soil layers can not be -99.");

    if (m_rootDepthmax == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The maximum depth for root development in the soil data can not be NULL.");
    if (m_layerFieldCapacity == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData", "The soil field capacity data can not be NULL.");
    if (m_layerWiltingPnt == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The Plant wilting point moisture data can not be NULL.");
    if (m_layerSoilWater == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData", "The soil moisture data can not be NULL.");
    if (m_layerNO3 == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData", "The amount of nitrate data can not be NULL.");
    if (m_layerP == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The amount of phosphorus in solution in layer ly can not be NULL.");

    if (m_maxBiomass == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData", "The max biomass data can not be NULL.");
    if (m_fullDevYear == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData", "The full development year data can not be NULL.");
    if (m_frDPHU == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The PHU fraction when senescence becomes dominant data can not be NULL.");

    if (m_tBase == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData", "The base temperature data can not be NULL.");
    if (m_tOpt == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData", "The optimal temperature data can not be NULL.");
    //if(m_startDate == NULL)
    //	throw ModelException("UptakeGrowth","CheckInputData","The date begin to plant data can not be NULL.");
    //if(m_endDate == NULL)
    //	throw ModelException("UptakeGrowth","CheckInputData","The date of harvest data can not be NULL.");

    if (m_frN1 == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The normal fraction of nitrogen in the plant biomass at emergence data can not be NULL.");
    if (m_frN2 == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The normal fraction of nitrogen in the plant biomass at 50% maturity data can not be NULL.");
    if (m_frN3 == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The normal fraction of nitrogen in the plant biomass at maturity data can not be NULL.");
    if (m_frP1 == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The normal fraction of phosphorus in the plant biomass at emergence data can not be NULL.");
    if (m_frP2 == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The normal fraction of phosphorus in the plant biomass at 50% maturity data can not be NULL.");
    if (m_frP3 == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The normal fraction of phosphorus in the plant biomass at maturity data can not be NULL.");
    if (m_maxLAI == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData", "The maximum leaf area index data can not be NULL.");

    if (m_tMin == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData", "The min temperature data can not be NULL.");
    if (m_tMax == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData", "The max temperature data can not be NULL.");
    if (m_PET == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The Potential evapotranspiration data can not be NULL.");

    if (m_LAIdelta == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The potential leaf area added on day data can not be NULL.");
    if (m_LAI == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The leaf area index for a given day data can not be NULL.");
    if (m_preLAI == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The leaf area index for previous day data can not be NULL.");


    if (m_biomass == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The total biomass on a given day data can not be NULL.");
    if (m_biomassDelta == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The biomass increase on a given day data can not be NULL.");
    if (m_biomassNOpt == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData", "The potential biomass N data can not be NULL.");
    if (m_biomassPOpt == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData", "The potential biomass P data can not be NULL.");

    if (m_frPHU == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The fraction of phu accumulated on a given day data can not be NULL.");
    if (m_frN == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The optimal fraction of nitrogen in the plant biomass data can not be NULL.");
    if (m_frP == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The optimal fraction of phosphorus in the plant biomass data can not be NULL.");
    if (m_frRoot == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The fraction of total biomass partitioned to roots on a given day data can not be NULL.");

    return true;
}

bool UptakeGrowth::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
    {
        throw ModelException("UptakeGrowth", "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero00.");
        return false;
    }
    if (m_nCells != n)
    {
        if (m_nCells <= 0) m_nCells = n;
        else
        {
            throw ModelException("UptakeGrowth", "CheckInputSize", "Input data for " + string(key) +
                                                                   " is invalid. All the input data should have same size.");
            return false;
        }
    }
    return true;
}

void UptakeGrowth::initialOutputs()
{
    if (m_nCells <= 0)
        throw ModelException("UptakeGrowth", "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    if (m_zRoot == NULL)
    {
        m_zRoot = new float[m_nCells];
        m_WnormalizationParam = new float[m_nCells];
        m_NnormalizationParam = new float[m_nCells];
        m_PnormalizationParam = new float[m_nCells];
        m_waterLastDepth = new float[m_nCells];
        m_NLastDepth = new float[m_nCells];
        m_PLastDepth = new float[m_nCells];
        m_waterThisDepth = new float[m_nCells];
        m_NThisDepth = new float[m_nCells];
        m_PThisDepth = new float[m_nCells];
        m_layerAvailableWaterCapacity = new float *[m_nCells];
        m_potentialNUptake = new float[m_nCells];
        m_potentialPUptake = new float[m_nCells];
        m_waterTotal = new float[m_nCells];
        m_NTotal = new float[m_nCells];
        m_PTotal = new float[m_nCells];
        m_fGrowthStage = new float[m_nCells];
        m_fSoilWater = new float[m_nCells];
        m_fSoilNitrate = new float[m_nCells];
        m_biomassN = new float[m_nCells];
        m_biomassP = new float[m_nCells];
        m_biomassRoot = new float[m_nCells];
        m_biomassAG = new float[m_nCells];
        m_bioTreeAnnual = new float[m_nCells];
        m_transpirationMax = new float[m_nCells];
        m_depthArray = new float[int(m_layers) + 1];
        m_depthArray[0] = 0.0f;
        m_depthArray[1] = 10.f;
        m_depthArray[2] = 1000.f;
        m_totalWUptake = new float[m_nCells];
        m_totalNUptake = new float[m_nCells];
        m_totalPUptake = new float[m_nCells];
        m_NFixation = new float[m_nCells];
        m_layerWUptake = new float *[m_nCells];
        m_layerNUptake = new float *[m_nCells];
        m_layerPUptake = new float *[m_nCells];
        m_waterStress = new float[m_nCells];
        m_tempStress = new float[m_nCells];
        m_NStress = new float[m_nCells];
        m_PStress = new float[m_nCells];
        m_totalStress = new float[m_nCells];
        m_totalFieldCapacity = new float[m_nCells];
        m_totalNO3 = new float[m_nCells];
        m_totalSoilWater = new float[m_nCells];

#pragma omp parallel for
        for (int i = 0; i < m_nCells; i++)
        {
            m_zRoot[i] = 0.0f;
            m_WnormalizationParam[i] = 0.0f;
            m_NnormalizationParam[i] = 0.0f;
            m_PnormalizationParam[i] = 0.0f;
            m_waterLastDepth[i] = 0.0f;
            m_NLastDepth[i] = 0.0f;
            m_PLastDepth[i] = 0.0f;
            m_waterThisDepth[i] = 0.0f;
            m_NThisDepth[i] = 0.0f;
            m_PThisDepth[i] = 0.0f;
            m_potentialNUptake[i] = 0.0f;
            m_potentialPUptake[i] = 0.0f;
            m_waterTotal[i] = 0.0f;
            m_NTotal[i] = 0.0f;
            m_PTotal[i] = 0.0f;
            m_fGrowthStage[i] = 0.0f;
            m_fSoilWater[i] = 0.0f;
            m_fSoilNitrate[i] = 0.0f;
            m_biomassP[i] = 0.0f;
            m_biomassRoot[i] = 0.0f;
            m_biomassAG[i] = 0.0f;
            m_bioTreeAnnual[i] = 0.0f;
            m_transpirationMax[i] = 0.0f;
            m_totalWUptake[i] = 0.0f;
            m_totalNUptake[i] = 0.0f;
            m_totalPUptake[i] = 0.0f;
            m_NFixation[i] = 0.0f;
            m_waterStress[i] = 0.0f;
            m_tempStress[i] = 0.0f;
            m_NStress[i] = 0.0f;
            m_PStress[i] = 0.0f;
            m_totalStress[i] = 0.0f;
            m_totalFieldCapacity[i] = 0.0f;
            m_totalNO3[i] = 0.0f;
            m_totalSoilWater[i] = 0.0f;

            m_layerAvailableWaterCapacity[i] = new float[int(m_layers)];
            for (int j = 0; j < m_layers; j++)
                m_layerAvailableWaterCapacity[i][j] = 0.0f;

            m_layerWUptake[i] = new float[int(m_layers)];
            for (int j = 0; j < m_layers; j++)
                m_layerWUptake[i][j] = 0.0f;

            m_layerNUptake[i] = new float[int(m_layers)];
            for (int j = 0; j < m_layers; j++)
                m_layerNUptake[i][j] = 0.0f;

            m_layerPUptake[i] = new float[int(m_layers)];
            for (int j = 0; j < m_layers; j++)
                m_layerPUptake[i][j] = 0.0f;

        }
    }
}

void UptakeGrowth::SetValue(const char *key, float data)
{
    string s(key);
    if (StringMatch(s, "wDis"))
        m_waterDis = data;
    else if (StringMatch(s, "wCom"))
        m_waterCom = data;
    else if (StringMatch(s, "nFixFactor"))
        m_NFixFactor = data;
    else if (StringMatch(s, "nFixMax"))
        m_NFixDailyMax = data;
    else if (StringMatch(s, "nDis"))
        m_NDis = data;
    else if (StringMatch(s, "pDis"))
        m_PDis = data;
    else if (StringMatch(s, "soilLayer"))
        m_layers = data;
        //else if(StringMatch(s, "startDate"))
        //	m_startDate = data;
        //else if(StringMatch(s, "endDate"))
        //	m_endDate = data;

    else if (StringMatch(s, VAR_OMP_THREADNUM))
    {
        omp_set_num_threads((int) data);
    }
    else
        throw ModelException("UptakeGrowth", "SetValue", "Parameter " + s
                                                         +
                                                         " does not exist in UptakeGrowth method. Please contact the module developer.");

}

void UptakeGrowth::Set1DData(const char *key, int n, float *data)
{
    //check the input data
    string s(key);
    //n = m_nCells;
    CheckInputSize(key, n);
    if (StringMatch(s, "IDC"))
        m_classification = data;

    else if (StringMatch(s, "T_BASE"))
        m_tBase = data;
    else if (StringMatch(s, "T_OPT"))
        m_tOpt = data;

    else if (StringMatch(s, "RDMX"))
    {
        m_rootDepthmax = data;
        for (int i = 0; i < m_nCells; i++)
        {
            m_rootDepthmax[i] *= 1000.f;
        }
    }

    else if (StringMatch(s, "BMX_TREES"))
        m_maxBiomass = data;
    else if (StringMatch(s, "MAT_YRS"))
        m_fullDevYear = data;
    else if (StringMatch(s, "DLAI"))
        m_frDPHU = data;

    else if (StringMatch(s, "BN1"))
        m_frN1 = data;
    else if (StringMatch(s, "BN2"))
        m_frN2 = data;
    else if (StringMatch(s, "BN3"))
        m_frN3 = data;
    else if (StringMatch(s, "BP1"))
        m_frP1 = data;
    else if (StringMatch(s, "BP2"))
        m_frP2 = data;
    else if (StringMatch(s, "BP3"))
        m_frP3 = data;
    else if (StringMatch(s, "BLAI"))
        m_maxLAI = data;

    else if (StringMatch(s, "D_TMIN"))
        m_tMin = data;
    else if (StringMatch(s, "D_TMAX"))
        m_tMax = data;
    else if (StringMatch(s, "D_PET"))
        m_PET = data;

    else if (StringMatch(s, "LAI_Delta"))
        m_LAIdelta = data;
    else if (StringMatch(s, "LAI"))
        m_LAI = data;
    else if (StringMatch(s, "LAI_Pre"))
        m_preLAI = data;

    else if (StringMatch(s, "BIOMASS"))
        m_biomass = data;
    else if (StringMatch(s, "BIOMASS_Delta"))
        m_biomassDelta = data;
    else if (StringMatch(s, "BIOMASS_NOpt"))
        m_biomassNOpt = data;
    else if (StringMatch(s, "BIOMASS_POpt"))
        m_biomassPOpt = data;

    else if (StringMatch(s, "Fr_PHU"))
        m_frPHU = data;
    else if (StringMatch(s, "Fr_N"))
        m_frN = data;
    else if (StringMatch(s, "Fr_P"))
        m_frP = data;
    else if (StringMatch(s, "Fr_Root"))
        m_frRoot = data;
    else
        throw ModelException("UptakeGrowth", "Set1DData", "Parameter " + s +
                                                          " does not exist in UptakeGrowth method. Please contact the module developer.");

}

void UptakeGrowth::Set2DData(const char *key, int nRows, int nCols, float **data)
{
    string s(key);
    if (StringMatch(s, "Fieldcap_2D"))
        m_layerFieldCapacity = data;
    else if (StringMatch(s, "Wiltingpoint_2D"))
        m_layerWiltingPnt = data;
    else if (StringMatch(s, "D_SOMO_2D"))
        m_layerSoilWater = data;
    else if (StringMatch(s, "D_Nitrate"))
        m_layerNO3 = data;
    else if (StringMatch(s, "D_SoluP"))
        m_layerP = data;
}

void UptakeGrowth::Get1DData(const char *key, int *n, float **data)
{
    //initialOutputs();
    string s(key);

    if (StringMatch(s, "BIOMASS"))
    {
        *data = m_biomass;
        //cout<<"BIOMASS:"<<m_biomass[300]<<endl;
    }
    else if (StringMatch(s, "BIOMASS_N"))
    {
        *data = m_biomassN;

    }
    else if (StringMatch(s, "BIOMASS_P"))
    {
        *data = m_biomassP;

    }
    else if (StringMatch(s, "BIOMASS_ROOT"))
    {
        *data = m_biomassRoot;

    }
    else if (StringMatch(s, "BIOMASS_AG"))
    {
        *data = m_biomassAG;

    }

    else if (StringMatch(s, "Transpiration_Max"))
    {
        *data = m_transpirationMax;
    }

    else if (StringMatch(s, "Total_WaterUptake"))
    {
        for (int i = 0; i < *n; i++)
        {
            m_totalWUptake[i] *= 10000;
        }
        *data = m_totalWUptake;

    }
    else if (StringMatch(s, "Total_NUptake"))
    {
        for (int i = 0; i < *n; i++)
        {
            m_totalNUptake[i] *= 10000;
        }
        *data = m_totalNUptake;

    }
    else if (StringMatch(s, "Total_PUptake"))
    {
        for (int i = 0; i < *n; i++)
        {
            m_totalPUptake[i] *= 10000;
        }
        *data = m_totalPUptake;

    }
    else if (StringMatch(s, "N_Fixation"))
    {
        *data = m_NFixation;

    }
    else
        throw ModelException("UptakeGrowth", "Get1DData", "Result " + s +
                                                          " does not exist in UptakeGrowth method. Please contact the module developer.");

    *n = m_nCells;
}

void UptakeGrowth::Get2DData(const char *key, int *nRows, int *nCols, float ***data)
{
    //initialOutputs();
    string s(key);
    if (StringMatch(s, "Layer_WaterUptake"))
    {
        *data = m_layerWUptake;
    }
    else if (StringMatch(s, "Layer_NUptake"))
    {
        *data = m_layerNUptake;
    }
    else if (StringMatch(s, "Layer_PUptake"))
    {
        *data = m_layerPUptake;
    }
    else
        throw ModelException("UptakeGrowth", "Get2DData", "Result " + s +
                                                          " does not exist in UptakeGrowth method. Please contact the module developer.");

    *nRows = m_nCells;
    *nCols = 2;
}

float UptakeGrowth::GrowthStageFactorForNitrogenFixation(float frAccumulatedHeatUnit)
{
    if (frAccumulatedHeatUnit <= 0.15)
        return 0.0f;
    else if (frAccumulatedHeatUnit <= 0.30)
        return 6.67f * frAccumulatedHeatUnit - 1.0f;
    else if (frAccumulatedHeatUnit <= 0.55)
        return 1.0f;
    else if (frAccumulatedHeatUnit <= 0.75)
        return 3.75f - 5.0f * frAccumulatedHeatUnit;
    else
        return 0.0f;
}

float UptakeGrowth::WaterFactorForNitrogenFixation(float totalWater, float totalFieldCapacity)
{
    return totalWater / 0.85f / totalFieldCapacity;
}

float UptakeGrowth::NitrateFactorForNitrogenFixation(float totalNO3)
{
    if (totalNO3 <= 100)
        return 1.0f;
    else if (totalNO3 <= 300)
        return 1.5f - 0.0005f * totalNO3;
    else
        return 0.0f;
}

float UptakeGrowth::AerationStress(float totalWater, float totalFieldCapacity, float totalSaturation)
{

    if (totalWater > totalFieldCapacity)
    {
        float satco = (totalWater - totalFieldCapacity) / (totalSaturation - totalFieldCapacity);
        return satco / (satco + exp(0.176f - 4.544f * satco));
    }
    else
        return 0.0f;
}

float UptakeGrowth::getWaterStress(float actualTranspiration, float maxTranspiration)
{
    if (actualTranspiration > maxTranspiration)
    {
        actualTranspiration = maxTranspiration;
    }
    return 1 - actualTranspiration / maxTranspiration;
}

float UptakeGrowth::getTemperatureStress(float tMin, float tMax, float tBase, float tOpt)
{
    float tMean = (tMin + tMax) / 2;
    float Tmax = 2.0f * tOpt - tBase;

    if (tMean <= tBase || tMean > Tmax)
        return 1.0f;
    else
    {
        float temp = -0.1054f * pow(tOpt - tMean, 2.0f);
        if (tMean <= tOpt)
            return 1.0f - exp(temp / pow(tMean - tBase, 2.0f));
        else
            return 1.0f - exp(temp / pow(Tmax - tMean, 2.0f));
    }
}

float UptakeGrowth::getNPStress(float actual, float optimal)
{
    if (optimal <= 1.0e-6)
        return 0.0f;

    float scalingFactor = 200.0f * (actual / optimal - 0.5f);
    if (scalingFactor < 0)
        return 1.0f;
    else if (scalingFactor >= 99)
        return 0.0f;
    else
        return 1.0f - scalingFactor / (scalingFactor + exp(3.535f - 0.02597f * scalingFactor));
}

int UptakeGrowth::Execute()
{
    CheckInputData();
    initialOutputs();

    struct tm timeinfo;
    LocalTime(m_date, &timeinfo);
    timeinfo.tm_mon = 0;
    timeinfo.tm_mday = 0;
    timeinfo.tm_hour = 0;
    timeinfo.tm_min = 0;
    timeinfo.tm_sec = 0;
    timeinfo.tm_isdst = false;
    time_t tYear = mktime(&timeinfo);

    time_t startDate = tYear + m_startDate;
    time_t endDate = tYear + m_endDate;

    if (m_date >= startDate && m_date <= endDate)
    {
#pragma omp parallel for
        for (int i = 0; i < m_nCells; i++)
        {
            m_common->m_classification = (int) (m_classification[i]);
            //If m_PET is not calculated with Penman-Monteith method
            //--->tp_max: the maximum transpiration
            //m_PET is the potential evapotranspiration adjusted for evaporation of free water in the canopy
            m_LAI[i] = m_preLAI[i];
            if (m_PET[i] > 1.0e-6)
            {
                if (m_LAI[i] <= 3.0f)
                    m_transpirationMax[i] = m_LAI[i] * m_PET[i] / 3.0f;
                else
                    m_transpirationMax[i] = m_PET[i];
            }
            if (m_transpirationMax[i] < 0)
                m_transpirationMax[i] = 0.0f;
            //zroot
            m_zRoot[i] = 0.0f;
            if (m_common->IsAnnual())
            {
                m_zRoot[i] = 2.5f * m_frPHU[i] * m_rootDepthmax[i];
                m_zRoot[i] = min(m_zRoot[i], m_rootDepthmax[i]);
                m_zRoot[i] = max(m_zRoot[i], 10.0f);
            }
            else
            {
                m_zRoot[i] = m_rootDepthmax[i];
            }
            if (m_zRoot[i] <= 0.0001)
            {
                for (int j = 0; j < m_layers; j++)
                {
                    m_layerWUptake[i][j] = 0.f;
                    m_layerNUptake[i][j] = 0.f;
                    m_layerPUptake[i][j] = 0.f;
                }
                continue;

            }

            //m_layerWUptake, m_totalWUptake,
            if (m_waterDis < 0) m_waterDis = 10.0f;  //water-use distribution parameter
            m_WnormalizationParam[i] = m_common->getNormalization(m_waterDis);
            if (m_waterCom < 0) m_waterCom = 0.0f;
            if (m_waterCom > 1) m_waterCom = 1.0f;
            m_waterLastDepth[i] = 0.0f;    //the water uptake in last depth
            m_waterTotal[i] = 0.0f;        //total water uptake of all layers
            m_waterThisDepth[i] = 0.0f;
            //divide soil to 2 layers
            //the first layer is 10mm
            for (int j = 0; j < m_layers; j++)
            {
                m_layerAvailableWaterCapacity[i][j] = m_layerFieldCapacity[i][j] - m_layerWiltingPnt[i][j];
                if (m_zRoot[i] < m_depthArray[j])
                    m_depthArray[j] = m_zRoot[i];
                //waterLastDepth = tp_max * (1.0f - exp(-m_waterDis * m_depthArray[j] / zroot)) / WnormalizationParam;
                m_waterThisDepth[i] =
                        m_transpirationMax[i] * (1.0f - exp(-m_waterDis * m_depthArray[j + 1] / m_zRoot[i])) /
                        m_WnormalizationParam[i];
                if (m_waterThisDepth[i] < 0)
                    m_waterThisDepth[i] = 0.0f;
                m_layerWUptake[i][j] = m_waterThisDepth[i] - m_waterLastDepth[i];
                m_layerWUptake[i][j] += (m_waterLastDepth[i] - m_waterTotal[i]) * m_waterCom;
                if (m_layerSoilWater[i][j] >= 0.25f * m_layerAvailableWaterCapacity[i][j])
                {
                    m_layerWUptake[i][j] *= 1.0f;
                }
                else
                {
                    m_layerWUptake[i][j] *= exp(
                            5.0f * (4.0f * m_layerSoilWater[i][j] / m_layerAvailableWaterCapacity[i][j] - 1.0f));
                }
                m_layerWUptake[i][j] = min(m_layerWUptake[i][j], (m_layerSoilWater[i][j] - m_layerWiltingPnt[i][j]));
                m_layerWUptake[i][j] = max(0.0f, m_layerWUptake[i][j]);
                m_waterLastDepth[i] = m_waterThisDepth[i];

                m_waterTotal[i] += m_layerWUptake[i][j];
                if (m_zRoot[i] <= m_depthArray[j + 1])
                    break;

            }
            m_totalWUptake[i] = 0;//m_waterTotal[i];
            //******************************************************************//
            //m_layerNUptake, m_layerPUptake, m_totalNUptake, m_totalPUptake

            m_potentialNUptake[i] = 0.f;
            m_potentialPUptake[i] = 0.f;
            m_potentialNUptake[i] = min(4.0f * m_frN3[i] * m_biomassDelta[i], m_biomassNOpt[i] - m_biomassN[i]);
            m_potentialPUptake[i] = min(4.0f * m_frP3[i] * m_biomassDelta[i], m_biomassPOpt[i] - m_biomassP[i]);
            if (m_NDis < 0) m_NDis = 20.0f;
            if (m_PDis < 0) m_PDis = 20.0f;
            m_NnormalizationParam[i] = m_common->getNormalization(m_NDis);
            m_PnormalizationParam[i] = m_common->getNormalization(m_PDis);
            m_NLastDepth[i] = 0.0f;    //the N uptake in last depth
            m_NTotal[i] = 0.0f;        //total N uptake of all layers
            m_PLastDepth[i] = 0.0f;    //the N uptake in last depth
            m_PTotal[i] = 0.0f;        //total N uptake of all layers
            m_NThisDepth[i] = 0.f;
            m_PThisDepth[i] = 0.f;
            for (int j = 0; j < m_layers; j++)
            {
                m_NThisDepth[i] = m_potentialNUptake[i] * (1.0f - exp(-m_NDis * m_depthArray[j + 1] / m_zRoot[i])) /
                                  m_NnormalizationParam[i];
                m_PThisDepth[i] = m_potentialPUptake[i] * (1.0f - exp(-m_PDis * m_depthArray[j + 1] / m_zRoot[i])) /
                                  m_PnormalizationParam[i];
                if (m_NThisDepth[i] < 0) m_NThisDepth[i] = 0.0f;
                if (m_PThisDepth[i] < 0) m_PThisDepth[i] = 0.0f;
                m_layerNUptake[i][j] = m_NThisDepth[i] - m_NLastDepth[i];
                m_layerPUptake[i][j] = m_PThisDepth[i] - m_PLastDepth[i];
                m_layerNUptake[i][j] += m_NLastDepth[i] - m_NTotal[i];
                m_layerPUptake[i][j] += m_PLastDepth[i] - m_PTotal[i];
                m_layerNUptake[i][j] = min(m_layerNUptake[i][j], m_layerNO3[i][j]);
                m_layerPUptake[i][j] = min(m_layerPUptake[i][j], m_layerP[i][j]);
                m_layerNUptake[i][j] = max(0.0f, m_layerNUptake[i][j]);
                m_layerPUptake[i][j] = max(0.0f, m_layerPUptake[i][j]);
                m_NLastDepth[i] = m_NThisDepth[i];
                m_PLastDepth[i] = m_PThisDepth[i];
                //omp_set_lock(&lock);
                m_NTotal[i] += m_layerNUptake[i][j];
                m_PTotal[i] += m_layerPUptake[i][j];
                //omp_unset_lock(&lock);
                if (m_zRoot[i] <= m_depthArray[j + 1])
                    break;
            }
            //total N/P uptake on a given day
            m_totalNUptake[i] = m_NTotal[i];
            m_totalPUptake[i] = m_PTotal[i];
            //update m_biomassN:  accumulate everyday's uptake
            m_biomassN[i] += m_totalNUptake[i];
            m_biomassP[i] += m_totalPUptake[i];
            //stress
            m_tempStress[i] = getTemperatureStress(m_tMin[i], m_tMax[i], m_tBase[i], m_tOpt[i]);
            m_waterStress[i] = getWaterStress(m_totalWUptake[i], m_transpirationMax[i]);


            //******************************************************************//
            //the deficit of N.
            float Ndeficit = m_potentialNUptake[i] - m_totalNUptake[i];
            //growth stage factor
            m_fGrowthStage[i] = GrowthStageFactorForNitrogenFixation(m_frPHU[i]);
            for (int j = 0; j < m_layers; j++)
            {
                m_totalSoilWater[i] += m_layerSoilWater[i][j];
                m_totalFieldCapacity[i] += m_layerFieldCapacity[i][j];
                m_totalNO3[i] += m_layerNO3[i][j];
            }
            //soil water factor(0.0-1.0)
            m_fSoilWater[i] = WaterFactorForNitrogenFixation(m_totalSoilWater[i],
                                                             m_totalFieldCapacity[i]);//////must be total field capacity
            //soil nitrate factor(0.0-1.0)
            m_fSoilNitrate[i] = NitrateFactorForNitrogenFixation(m_totalNO3[i]);/////////must be total NO3


            //m_NFixation
            if (m_common->IsLegume())
            {
                float fixn = Ndeficit * m_fGrowthStage[i] * min(min(m_fSoilWater[i], m_fSoilNitrate[i]), 1.0f);
                fixn = min(6.0f, fixn);
                //from SWAT
                if (m_NFixFactor < 0)
                    m_NFixFactor = 0.5f;
                if (m_NFixFactor > 1)
                    m_NFixFactor = 1.0f;
                fixn = m_NFixFactor * fixn + (1.0f - m_NFixFactor) * Ndeficit;
                if (m_NFixDailyMax < 0)
                    m_NFixDailyMax = 20.0f;
                fixn = min(fixn, Ndeficit);                        //nitrogen fixation can't surpass the demand
                fixn = min(fixn,
                           m_NFixDailyMax);                    //nitrogen fixation can't surpass maximum limitation
                m_NFixation[i] = fixn;
                m_biomassN[i] += m_NFixation[i];
                m_NStress[i] = 0.0f;
            }
            else
                m_NStress[i] = getNPStress(m_biomassN[i], m_biomassNOpt[i]);
            m_PStress[i] = getNPStress(m_biomassP[i], m_biomassPOpt[i]);
            //uptake
            //m_aerationStress[i] =
            //m_totalStress[i] = 1.0f - max(m_waterStress[i],max(m_tempStress[i], max(m_NStress[i], max(m_PStress[i],m_aerationStress[i]))));
            m_totalStress[i] = 1.0f - max(m_waterStress[i], max(m_tempStress[i], max(m_NStress[i], m_PStress[i])));
            m_totalStress[i] = min(1.0f, m_totalStress[i]);
            m_totalStress[i] = max(0.0f, m_totalStress[i]);
            if (m_frPHU[i] <= m_frDPHU[i])//grow
            {
                m_LAIdelta[i] *= sqrt(m_totalStress[i]);
                m_LAI[i] += m_LAIdelta[i];
                if (m_LAI[i] > m_maxLAI[i])
                    m_LAI[i] = m_maxLAI[i];
                m_preLAI[i] = m_LAI[i];
            }
            else//leaf begins senescence
            {
                m_LAI[i] = m_maxLAI[i] * (1.0f - m_frPHU[i]) / (1.0f - m_frDPHU[i]);
            }
            m_LAI[i] = max(0.0f, m_LAI[i]);
            m_biomassDelta[i] *= m_totalStress[i];
            m_biomass[i] += m_biomassDelta[i];
            if (m_common->IsTree())
            {
                m_bioTreeAnnual[i] = 1000.0f * m_maxBiomass[i] * m_curYear / m_fullDevYear[i];
                //accumulated by year, the result should be less than annual biomass
                m_biomass[i] = min(m_biomass[i], m_bioTreeAnnual[i]);
            }
            m_biomassRoot[i] = m_frRoot[i] * m_biomass[i];
            m_biomassAG[i] = m_biomass[i] - m_biomassRoot[i];
        }
    }

    return 0;
}

