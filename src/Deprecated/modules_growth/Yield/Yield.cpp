#include <iostream>
#include "Yield.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include "utils.h"
//#include "Common.h"
//#include "Climate.h"
#include <math.h>
#include <stdlib.h>
#include <omp.h>

YLD::YLD(void) : m_nCells(-1),
                 m_HiOpt(NULL), m_HiMin(NULL), m_frNyld(NULL), m_frPyld(NULL), m_frDeclineLAI(NULL),
                 m_harvestDate(NULL), m_PET(NULL), m_soilET(NULL), m_biomass(NULL), m_biomassN(NULL),
                 m_biomassP(NULL), m_biomassAG(NULL), m_frPHU(NULL), m_totalWaterUptake(NULL), m_yield(NULL),
                 m_yieldN(NULL), m_yieldP(NULL), m_classification(NULL),
                 m_biomassRoot(NULL), m_LAI(NULL), m_HI(NULL), m_totalPlantET(NULL), m_totalPlantPET(NULL), m_wur(NULL),
                 m_Hiact(NULL), m_harvestEfficiency(NULL),
                 m_clip(NULL), m_clipN(NULL), m_clipP(NULL), m_removeFraction(NULL), m_rootFraction(NULL),
                 m_rootLeft(NULL), m_rootRemove(NULL),
                 m_rootRemoveFraction(NULL), m_rootRemoveN(NULL), m_rootRemoveP(NULL), m_harvested(false)
{
    m_common = new Common();
    utils utl;
    m_harvestDate = utl.ConvertToTime2("1970-10-02 00:00:00", "%d-%d-%d %d:%d:%d", true);
}

YLD::~YLD(void)
{
    //// cleanup
    if (m_yield != NULL)
        delete[] m_yield;
    if (m_yieldN != NULL)
        delete[] m_yieldN;
    if (m_yieldP != NULL)
        delete[] m_yieldP;

    if (m_HI != NULL)
        delete[] m_HI;
    if (m_totalPlantET != NULL)
        delete[] m_totalPlantET;
    if (m_totalPlantPET != NULL)
        delete[] m_totalPlantPET;
    if (m_wur != NULL)
        delete[] m_wur;
    if (m_Hiact != NULL)
        delete[] m_Hiact;
    if (m_harvestEfficiency != NULL)
        delete[] m_harvestEfficiency;
    if (m_clip != NULL)
        delete[] m_clip;
    if (m_clipN != NULL)
        delete[] m_clipN;
    if (m_clipP != NULL)
        delete[] m_clipP;
    if (m_removeFraction != NULL)
        delete[] m_removeFraction;
    if (m_rootFraction != NULL)
        delete[] m_rootFraction;
    if (m_rootLeft != NULL)
        delete[] m_rootLeft;
    if (m_rootRemove != NULL)
        delete[] m_rootRemove;
    if (m_rootRemoveFraction != NULL)
        delete[] m_rootRemoveFraction;
    if (m_rootRemoveN != NULL)
        delete[] m_rootRemoveN;
    if (m_rootRemoveP != NULL)
        delete[] m_rootRemoveP;
}

bool YLD::CheckInputData(void)
{
    if (m_date <= 0)
        throw ModelException("Yield", "CheckInputData", "You have not set the time.");
    if (m_nCells <= 0)
        throw ModelException("Yield", "CheckInputData", "The dimension of the input data can not be less than zero.");

    if (m_classification == NULL)
        throw ModelException("UptakeGrowth", "CheckInputData", "The  land cover/plant classification can not be NULL.");
    if (m_HiOpt == NULL)
        throw ModelException("Yield", "CheckInputData",
                             "The potential harvest index for the plant at maturity given ideal growing conditions data can not be NULL.");
    if (m_HiMin == NULL)
        throw ModelException("Yield", "CheckInputData",
                             "The harvest index for the plant in drought conditions and represents the minimum harvest index allowed for the plant data can not be NULL.");
    if (m_frNyld == NULL)
        throw ModelException("Yield", "CheckInputData", "The fraction of nitrogen in the yield data can not be NULL.");
    if (m_frPyld == NULL)
        throw ModelException("Yield", "CheckInputData",
                             "The fraction of phosphorus in the yield data can not be NULL.");
    if (m_frDeclineLAI == NULL)
        throw ModelException("Yield", "CheckInputData", "The Decline LAI Fraction data can not be NULL.");

    if (m_frPHU == NULL)
        throw ModelException("Yield", "CheckInputData",
                             "The fraction of phu accumulated on a given day data can not be NULL.");
    if (m_PET == NULL)
        throw ModelException("Yield", "CheckInputData", "The Potential evapotranspiration data can not be NULL.");
    if (m_biomass == NULL)
        throw ModelException("Yield", "CheckInputData", "The total biomass data can not be NULL.");
    if (m_biomassN == NULL)
        throw ModelException("Yield", "CheckInputData", "The actual biomass N data can not be NULL.");
    if (m_biomassP == NULL)
        throw ModelException("Yield", "CheckInputData", "The actual biomass P data can not be NULL.");
    if (m_biomassAG == NULL)
        throw ModelException("Yield", "CheckInputData",
                             "The aboveground biomass on the day of harvest data can not be NULL.");

    if (m_biomassRoot == NULL)
        throw ModelException("Yield", "CheckInputData", "The biomass root data can not be NULL.");
    if (m_LAI == NULL)
        throw ModelException("Yield", "CheckInputData", "The leaf area index for a given day data can not be NULL.");

    if (m_soilET == NULL)
        throw ModelException("Yield", "CheckInputData", "The soil evapotranpiration data can not be NULL.");
    if (m_totalWaterUptake == NULL)
        throw ModelException("Yield", "CheckInputData", "The total water uptake data can not be NULL.");
    return true;
}

void YLD::initialOutputs()
{
    if (m_nCells <= 0)
        throw ModelException("Yield", "CheckInputData", "The dimension of the input data can not be less than zero.");
    if (m_yield == NULL)////? ????
    {
        m_yield = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_yield[i] = 0.0f;
        }
    }
    if (m_yieldN == NULL)////
    {
        m_yieldN = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_yieldN[i] = 0.0f;
        }
    }
    if (m_yieldP == NULL)////
    {
        m_yieldP = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_yieldP[i] = 0.0f;
        }
    }

    if (m_HI == NULL)////? ????
    {
        m_HI = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_HI[i] = 0.0f;
        }
    }
    if (m_totalPlantET == NULL)////
    {
        m_totalPlantET = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_totalPlantET[i] = 0.0f;
        }
    }
    if (m_totalPlantPET == NULL)////
    {
        m_totalPlantPET = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_totalPlantPET[i] = 0.0f;
        }
    }
    if (m_wur == NULL)////? ????
    {
        m_wur = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_wur[i] = 0.0f;
        }
    }
    if (m_Hiact == NULL)////
    {
        m_Hiact = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_Hiact[i] = 0.0f;
        }
    }
    if (m_harvestEfficiency == NULL)////
    {
        m_harvestEfficiency = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_harvestEfficiency[i] = 0.0f;
        }
    }
    if (m_clip == NULL)////? ????
    {
        m_clip = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_clip[i] = 0.0f;
        }
    }
    if (m_clipN == NULL)////
    {
        m_clipN = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_clipN[i] = 0.0f;
        }
    }
    if (m_clipP == NULL)////
    {
        m_clipP = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_clipP[i] = 0.0f;
        }
    }
    if (m_removeFraction == NULL)////? ????
    {
        m_removeFraction = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_removeFraction[i] = 0.0f;
        }
    }
    if (m_rootFraction == NULL)////
    {
        m_rootFraction = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_rootFraction[i] = 0.0f;
        }
    }
    if (m_rootLeft == NULL)////
    {
        m_rootLeft = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_rootLeft[i] = 0.0f;
        }
    }
    if (m_rootRemove == NULL)////? ????
    {
        m_rootRemove = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_rootRemove[i] = 0.0f;
        }
    }
    if (m_rootRemoveFraction == NULL)////
    {
        m_rootRemoveFraction = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_rootRemoveFraction[i] = 0.0f;
        }
    }
    if (m_rootRemoveN == NULL)////
    {
        m_rootRemoveN = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_rootRemoveN[i] = 0.0f;
        }
    }
    if (m_rootRemoveP == NULL)////
    {
        m_rootRemoveP = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_rootRemoveP[i] = 0.0f;
        }
    }

}

bool YLD::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
    {
        throw ModelException("Yield", "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
        return false;
    }
    if (m_nCells != n)
    {
        if (m_nCells <= 0) m_nCells = n;
        else
        {
            throw ModelException("Yield", "CheckInputSize", "Input data for " + string(key) +
                                                            " is invalid. All the input data should have same size.");
            return false;
        }
    }
    return true;
}

void YLD::SetValue(const char *key, float data)
{
    string s(key);

    if (StringMatch(s, VAR_OMP_THREADNUM))
    {
        omp_set_num_threads((int) data);
    }
    else
        throw ModelException("Yield", "SetValue", "Parameter " + s
                                                  +
                                                  " does not exist in Yield method. Please contact the module developer.");
}

void YLD::Set1DData(const char *key, int n, time_t *data)
{
    //string s(key);
    //CheckInputSize(key, n);

    //if(StringMatch(s,"harvestDate"))
    //	m_harvestDate = data;
    //else
    //	throw ModelException("UptakeGrowth","Set1DData","Parameter " + s +
    //	" does not exist in UptakeGrowth method. Please contact the module developer.");

}

void YLD::Set1DData(const char *key, int n, float *data)
{
    //check the input data
    string s(key);

    CheckInputSize(key, n);
    if (StringMatch(s, "CNYLD"))
        m_frNyld = data;
    else if (StringMatch(s, "CPYLD"))
        m_frPyld = data;
    else if (StringMatch(s, "IDC"))
        m_classification = data;
    else if (StringMatch(s, "DLAI"))
        m_frDeclineLAI = data;
    else if (StringMatch(s, "HVSTI"))
        m_HiOpt = data;
    else if (StringMatch(s, "WSYF"))
        m_HiMin = data;
    else if (StringMatch(s, "D_PET"))
        m_PET = data;
    else if (StringMatch(s, "D_SOET"))
        m_soilET = data;
    else if (StringMatch(s, "Fr_PHU"))
        m_frPHU = data;
    else if (StringMatch(s, "BIOMASS"))
        m_biomass = data;
    else if (StringMatch(s, "BIOMASS_N"))
        m_biomassN = data;
    else if (StringMatch(s, "BIOMASS_P"))
        m_biomassP = data;
    else if (StringMatch(s, "BIOMASS_AG"))
        m_biomassAG = data;
    else if (StringMatch(s, "BIOMASS_ROOT"))
        m_biomassRoot = data;
    else if (StringMatch(s, "LAI"))
        m_LAI = data;
    else if (StringMatch(s, "Total_WaterUptake"))
        m_totalWaterUptake = data;
    else
        throw ModelException("Yield", "Set1DData", "Parameter " + s +
                                                   " does not exist in Yield method. Please contact the module developer.");
}

void YLD::Get1DData(const char *key, int *n, float **data)
{
    string s(key);
    if (StringMatch(s, "Yield_N"))
    {
        *data = m_yieldN;

    }
    else if (StringMatch(s, "Yield_P"))
    {
        *data = m_yieldP;

    }
    else if (StringMatch(s, "Yield"))
    {
        *data = m_yield;

    }
    else
        throw ModelException("Yield", "Get1DData",
                             "Result " + s + " does not exist in Yield method. Please contact the module developer.");

    *n = m_nCells;
}

int YLD::Execute()
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

    time_t harvestDate = tYear + m_harvestDate;

#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++)
    {
        if (m_date > harvestDate && !m_harvested)
        {
            m_harvested = true;
            m_common->m_classification = (int) (m_classification[i]);
            //Harvest index
            //float harvestIndex = 0.0f;// potential harvest index for a given day
            //float totalPlantET = 0.0f;//actual ET simulated during life of plant
            //float totalPlantPET = 0.0f;//potential ET simulated during life of plant
            ////get total ET and PET(see grow.f Line 289-292) from zhiqiang
            if (m_frPHU[i] > 0.5 && m_frPHU[i] < m_frDeclineLAI[i])
            {
                m_totalPlantET[i] += m_totalWaterUptake[i] + m_soilET[i];
                m_totalPlantPET[i] += m_PET[i];
            }
            // water deficiency factor
            m_wur[i] = 100.0f;
            if (m_totalPlantPET[i] > 10)
                m_wur[i] *= m_totalPlantET[i] / m_totalPlantPET[i];

            //get optimal harvest index, p309 5:2.4.1
            m_HI[i] = m_HiOpt[i] * 100.0f * m_frPHU[i] / (100.0f * m_frPHU[i] + exp(11.1f - 10.0f * m_frPHU[i]));
            m_Hiact[i] = m_HI[i] - m_HiMin[i] * (m_wur[i] / (m_wur[i] + exp(6.13f - 0.0883f * m_wur[i]))) + m_HiMin[i];

            m_HI[i] = min(m_HI[i], m_Hiact[i]);
            //float yield = 0.0f;
            if (m_HiOpt[i] > 1.001)
                m_yield[i] = m_biomass[i] * (1.0f - 1.0f / (1.0f + m_HI[i]));
            else
                m_yield[i] = m_biomassAG[i] * m_HI[i];
            m_yield[i] = max(0.0f, m_yield[i]);
            m_harvestEfficiency[i] = 0.5f; //harvestEfficiency read from database
            //IsGrain should read from database
            if (m_common->IsGrain())//grain harvest, no residue, see harvgrainop.f
            {
                m_yield[i] *= m_harvestEfficiency[i];
                m_yieldN[i] = max(0.0f, min(m_yield[i] * m_frNyld[i], 0.85f * m_biomassN[i]));
                m_yieldP[i] = max(0.0f, min(m_yield[i] * m_frPyld[i], 0.85f * m_biomassP[i]));

                m_biomass[i] -= m_yield[i];
            }
            else //biomass harvest, has residue, residue redistribute to soil layer, see harvestop.f
            {
                //divide yield to two parts:clip and yield
                m_clip[i] = m_yield[i] * (1 - m_harvestEfficiency[i]);
                m_yield[i] -= m_clip[i];
                m_clip[i] = max(0.0f, m_clip[i]);
                m_yield[i] = max(0.0f, m_yield[i]);

                //harvest index override  read from database
                //get N&P in clip and yield[i]
                m_yieldN[i] = max(0.0f, min(m_yield[i] * m_frNyld[i], 0.8f * (m_biomassN[i])));
                m_yieldP[i] = max(0.0f, min(m_yield[i] * m_frPyld[i], 0.8f * (m_biomassP[i])));
                m_clipN[i] = max(0.0f, min(m_clip[i] * m_frNyld[i], m_biomassN[i] -
                                                                    m_yieldN[i])); //N in clip residual,needed by nitrient cycling module
                m_clipP[i] = max(0.0f, min(m_clip[i] * m_frPyld[i], m_biomassP[i] -
                                                                    m_yieldP[i])); //P in clip residual,needed by nitrient cycling module
                //reset LAI, frPHU and root fraction
                m_removeFraction[i] = 1.0f; //the fraction of remove part to aboveground part
                if (m_biomass[i] - m_biomassRoot[i] > 1.0e-6)
                    m_removeFraction[i] = (m_yield[i] + m_clip[i]) / (m_biomass[i] - m_biomassRoot[i]);
                m_removeFraction[i] = min(1.0f, m_removeFraction[i]);

                //root part
                m_rootFraction[i] = m_biomassRoot[i] / m_biomass[i];
                m_rootLeft[i] = (m_biomass[i] - m_biomassRoot[i]) * (1 - m_removeFraction[i]) * m_rootFraction[i] /
                                (1.0f - m_rootFraction[i]);
                m_rootRemove[i] = m_biomassRoot[i] - m_rootLeft[i];  //removed root as residual
                m_rootRemoveFraction[i] = 0.0f;
                if (m_biomassRoot[i] > 1.0e-6) m_rootRemoveFraction[i] = m_rootRemove[i] / (m_biomassRoot[i]);
                m_rootRemoveN[i] = m_rootRemoveFraction[i] * (m_biomassN[i]);
                m_rootRemoveP[i] = m_rootRemoveFraction[i] * (m_biomassP[i]);

                //remove aboveground and root biomass from total biomass
                //change the LAI and growth step
                if (m_biomass[i] > 0.001)
                {
                    m_LAI[i] *= 1.0f - m_removeFraction[i];
                    if (m_frPHU[i] < 0.999)
                        m_frPHU[i] *= 1.0f - m_removeFraction[i];

                    m_biomass[i] -= m_yield[i] + m_clip[i] + m_rootRemove[i];
                    m_biomassN[i] -= m_yieldN[i] + m_clipN[i] + m_rootRemoveN[i];
                    m_biomassP[i] -= m_yieldP[i] + m_clipP[i] + m_rootRemoveP[i];

                    m_biomass[i] = min(0.0f, m_biomass[i]);
                    m_biomassN[i] = min(0.0f, m_biomassP[i]);
                    m_biomassP[i] = min(0.0f, m_biomassP[i]);

                    m_biomassRoot[i] = m_biomass[i] * (0.4f - 0.2f * m_frPHU[i]);
                }
                else
                {
                    m_biomass[i] = 0.0f;
                    m_biomassN[i] = 0.0f;
                    m_biomassP[i] = 0.0f;
                    m_biomassRoot[i] = 0.0f;

                    m_LAI[i] = 0.0f;
                    m_frPHU[i] = 0.0f;
                }
            }
        }
    }

    //m_lastSWE = m_swe;
    return 0;
}

