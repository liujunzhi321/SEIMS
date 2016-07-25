#include <iostream>
#include "PotentialBiomass.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "utils.h"
#include "util.h"
#include "Common.h"
#include "Climate.h"
#include <omp.h>
#include <cmath>

using namespace std;

PotentialBiomass::PotentialBiomass(void) : m_prefrLAImx(NULL), m_frLAImx(NULL), m_initBiomass(10.f), m_initLAI(0.1f),
                                           m_curYear(10.f), m_PHU(6000.f), m_nCells(-1), m_classification(NULL),
                                           m_co2(NODATA),
                                           m_lightextinctioncoefficient(NULL), m_maxLAI(NULL), m_fullDevYear(NULL),
                                           m_tBase(NULL),
                                           m_frPHU1(NULL), m_frPHU2(NULL), m_ee(NULL), m_tMean(NULL),
                                           m_frLAI1(NULL), m_frLAI2(NULL), m_frDPHU(NULL), m_rueAmb(NULL),
                                           m_co2Hi(NULL), m_rueHi(NULL), m_rueDcl(NULL), m_rue(NULL),
                                           m_activeRadiation(NULL),
                                           m_tMax(NULL), m_tMin(NULL), m_RM(NULL), m_LAIShapeCoefficient1(NULL),
                                           m_LAIShapeCoefficient2(NULL), m_CO2ShapeCoefficient1(NULL),
                                           m_CO2ShapeCoefficient2(NULL),
                                           m_SR(NULL), m_frN1(NULL), m_frN2(NULL), m_frN3(NULL), m_frP1(NULL),
                                           m_frP2(NULL), m_frP3(NULL), m_biomassDelta(NULL), m_biomass(NULL),
                                           m_biomassNOpt(NULL), m_biomassPOpt(NULL), m_VPD(NULL), m_frPHU(NULL),
                                           m_preLAI(NULL), m_LAIdelta(NULL), m_LAI(NULL),
                                           m_frN(NULL), m_frP(NULL), m_frRoot(NULL)
{
    utils utl;
    m_startDate = utl.ConvertToTime2("1970-04-20 00:00:00", "%d-%d-%d %d:%d:%d", true);
    m_endDate = utl.ConvertToTime2("1970-10-02 00:00:00", "%d-%d-%d %d:%d:%d", true);
}

PotentialBiomass::~PotentialBiomass(void)
{
    //// cleanup
    if (m_LAIShapeCoefficient1 != NULL)
        delete[] m_LAIShapeCoefficient1;
    if (m_LAIShapeCoefficient2 != NULL)
        delete[] m_LAIShapeCoefficient2;

    if (m_CO2ShapeCoefficient1 != NULL)
        delete[] m_CO2ShapeCoefficient1;
    if (m_CO2ShapeCoefficient2 != NULL)
        delete[] m_CO2ShapeCoefficient2;

    if (m_rue != NULL)
        delete[] m_rue;
    if (m_activeRadiation != NULL)
        delete[] m_activeRadiation;

    if (m_ee != NULL)
        delete[] m_ee;
    if (m_tMean != NULL)
        delete[] m_tMean;

    if (m_prefrLAImx != NULL)
        delete[] m_prefrLAImx;
    if (m_frLAImx != NULL)
        delete[] m_frLAImx;

    if (m_biomassDelta != NULL)
        delete[] m_biomassDelta;
    if (m_biomass != NULL)
        delete[] m_biomass;
    if (m_biomassNOpt != NULL)
        delete[] m_biomassNOpt;
    if (m_biomassPOpt != NULL)
        delete[] m_biomassPOpt;

    if (m_VPD != NULL)
        delete[] m_VPD;
    if (m_frPHU != NULL)
        delete[] m_frPHU;
    if (m_LAIdelta != NULL)
        delete[] m_LAIdelta;
    if (m_LAI != NULL)
        delete[] m_LAI;
    if (m_frN != NULL)
        delete[] m_frN;
    if (m_frP != NULL)
        delete[] m_frP;
    if (m_frRoot != NULL)
        delete[] m_frRoot;
}

bool PotentialBiomass::CheckInputData(void)
{
    if (m_date <= 0)
        throw ModelException("PotentialBiomass", "CheckInputData", "You have not set the time.");

    if (m_nCells <= 0)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The dimension of the input data can not be less than zero.");

    if (m_classification == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The  land cover/plant classification can not be NULL.");

    if (m_lightextinctioncoefficient == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The  light extinction coefficient data can not be NULL.");

    if (m_maxLAI == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData", "The max LAI data can not be NULL.");
    //if(m_startDate == NULL)
    //	throw ModelException("UptakeGrowth","CheckInputData","The date begin to plant data can not be NULL.");
    //if(m_endDate == NULL)
    //	throw ModelException("UptakeGrowth","CheckInputData","The date of harvest data can not be NULL.");


    if (m_fullDevYear == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData", "The full development year data can not be NULL.");

    if (m_tBase == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData", "The base temperature data can not be NULL.");

    if (FloatEqual(m_co2, NODATA))
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The ambient atmospheric CO2 concentration  can not be -99.");

    if (m_frPHU1 == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The PHU fraction for the 1st point data can not be NULL.");
    if (m_frPHU2 == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The PHU fraction for the 2nd point data can not be NULL.");
    if (m_frLAI1 == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The LAI fraction for the 1st point data can not be NULL.");
    if (m_frLAI2 == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "TheLAI fraction for the 2nd point  data can not be NULL.");
    if (m_frDPHU == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The PHU fraction when senescence becomes dominant data can not be NULL.");

    if (m_rueAmb == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The RUE at ambient atmospheric CO2 concentration data can not be NULL.");
    if (m_co2Hi == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The elevated atmospheric CO2 concentration data can not be NULL.");
    if (m_rueHi == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The  RUE at elevated atmospheric CO2 concentration data can not be NULL.");
    if (m_rueDcl == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The rate of decline in rue per unit increase in VPD data can not be NULL.");

    if (m_frN1 == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The normal fraction of nitrogen in the plant biomass at emergence data can not be NULL.");
    if (m_frN2 == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The normal fraction of nitrogen in the plant biomass at 50% maturity data can not be NULL.");
    if (m_frN3 == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The normal fraction of nitrogen in the plant biomass at maturity data can not be NULL.");
    if (m_frP1 == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The normal fraction of phosphorus in the plant biomass at emergence data can not be NULL.");
    if (m_frP2 == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The normal fraction of phosphorus in the plant biomass at 50% maturity data can not be NULL.");
    if (m_frP3 == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The normal fraction of phosphorus in the plant biomass at maturity data can not be NULL.");

    if (m_tMin == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData", "The min temperature data can not be NULL.");
    if (m_tMax == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData", "The max temperature data can not be NULL.");

    if (m_RM == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData", "The relative humidity data can not be NULL.");
    if (m_SR == NULL)
        throw ModelException("PotentialBiomass", "CheckInputData", "The solar radiation data can not be NULL.");
    return true;
}

bool PotentialBiomass::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
    {
        throw ModelException("PotentialBiomass", "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
        return false;
    }
    if (m_nCells != n)
    {
        if (m_nCells <= 0)
            m_nCells = n;
        else
        {
            throw ModelException("PotentialBiomass", "CheckInputSize", "Input data for " + string(key) +
                                                                       " is invalid. All the input data should have same size.");
            return false;
        }
    }

    return true;
}

void PotentialBiomass::initialOutputs()
{
    if (m_nCells <= 0)
        throw ModelException("PotentialBiomass", "CheckInputData",
                             "The dimension of the input data can not be less than zero.");

    //initialize  the fraction of the plant��s maximum leaf area index for day i-1
    if (m_LAIShapeCoefficient1 == NULL)
    {
        m_LAIShapeCoefficient1 = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
            m_LAIShapeCoefficient1[i] = 0.0f;

        for (int i = 0; i < m_nCells; i++)
        {
            //m_common->m_classification = (int)(m_classification[i]);
            if (IsTree((int) (m_classification[i])))
                m_maxLAI[i] *= m_curYear / m_fullDevYear[i];
        }
    }
    if (m_LAIShapeCoefficient2 == NULL)
    {
        m_LAIShapeCoefficient2 = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_LAIShapeCoefficient2[i] = 0.0f;
        }
    }

    if (m_CO2ShapeCoefficient1 == NULL)
    {
        m_CO2ShapeCoefficient1 = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_CO2ShapeCoefficient1[i] = 0.0f;
        }
    }
    if (m_CO2ShapeCoefficient2 == NULL)
    {
        m_CO2ShapeCoefficient2 = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
            m_CO2ShapeCoefficient2[i] = 0.0f;
    }

    if (m_rue == NULL)
    {
        m_rue = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
            m_rue[i] = 0.0f;
    }
    if (m_activeRadiation == NULL)
    {
        m_activeRadiation = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
            m_activeRadiation[i] = 0.0f;
    }

    if (m_ee == NULL)
    {
        m_ee = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
            m_ee[i] = 0.0f;
    }
    if (m_tMean == NULL)
    {
        m_tMean = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
            m_tMean[i] = 0.0f;
    }

    if (m_prefrLAImx == NULL)
    {
        m_prefrLAImx = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_prefrLAImx[i] = 0.0f;
        }
    }
    //initialize  the fraction of the plant��s maximum leaf area index for day i
    if (m_frLAImx == NULL)
    {
        m_frLAImx = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_frLAImx[i] = 0.0f;
        }
    }

    if (m_biomassDelta == NULL)
    {
        m_biomassDelta = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_biomassDelta[i] = 0.0f;
        }
    }
    if (m_biomass == NULL)
    {
        m_biomass = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_biomass[i] = m_initBiomass;
        }
    }

    if (m_biomassNOpt == NULL)
    {
        m_biomassNOpt = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_biomassNOpt[i] = 0.0f;
        }
    }
    if (m_biomassPOpt == NULL)
    {
        m_biomassPOpt = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_biomassPOpt[i] = 0.0f;
        }
    }

    if (m_VPD == NULL)
    {
        m_VPD = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_VPD[i] = 0.0f;
        }
    }
    if (m_frPHU == NULL)
    {
        m_frPHU = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
            m_frPHU[i] = 0.0f;
    }
    if (m_preLAI == NULL)
    {
        m_preLAI = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_preLAI[i] = m_initLAI;
        }
    }
    if (m_LAIdelta == NULL)
    {
        m_LAIdelta = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_LAIdelta[i] = 0.0f;
        }
    }
    if (m_LAI == NULL)
    {
        m_LAI = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_LAI[i] = 0.0f;
        }
    }
    if (m_frN == NULL)
    {
        m_frN = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_frN[i] = 0.0f;
        }
    }
    if (m_frP == NULL)
    {
        m_frP = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_frP[i] = 0.0f;
        }
    }
    if (m_frRoot == NULL)
    {
        m_frRoot = new float[m_nCells];
        for (int i = 0; i < m_nCells; i++)
        {
            m_frRoot[i] = 0.0f;
        }
    }
}

void PotentialBiomass::SetValue(const char *key, float data)
{
    string s(key);
    if (StringMatch(s, "CO2"))
        m_co2 = data;

    else if (StringMatch(s, VAR_OMP_THREADNUM))
    {
        omp_set_num_threads((int) data);
    }
    else
        throw ModelException("PotentialBiomass", "SetValue", "Parameter " + s
                                                             +
                                                             " does not exist in PotentialBiomass method. Please contact the module developer.");

}

void PotentialBiomass::Set1DData(const char *key, int n, time_t *data)
{
    //check the input data
    string s(key);
    //n = m_nCells;
    CheckInputSize(key, n);
    //if(StringMatch(s,"startDate"))
    //	m_startDate = data;
    //else if(StringMatch(s,"endDate"))
    //	m_endDate = data;
    //else
    //	throw ModelException("UptakeGrowth","Set1DData","Parameter " + s +
    //	" does not exist in UptakeGrowth method. Please contact the module developer.");

}

void PotentialBiomass::Set1DData(const char *key, int n, float *data)
{

    string s(key);
    //check the input data
    CheckInputSize(key, n);
    //set the value
    if (StringMatch(s, "EXT_COEF"))
        m_lightextinctioncoefficient = data;
    else if (StringMatch(s, "IDC"))
        m_classification = data;

    else if (StringMatch(s, "MAT_YRS"))
    {
        m_fullDevYear = data;
    }
    else if (StringMatch(s, "BLAI"))
    {
        m_maxLAI = data;
    }
        //else if(StringMatch(s,"YR_INIT"))		m_curYear = data;

    else if (StringMatch(s, "T_BASE"))
        m_tBase = data;

    else if (StringMatch(s, "FRGRW1"))
        m_frPHU1 = data;
    else if (StringMatch(s, "FRGRW2"))
        m_frPHU2 = data;
    else if (StringMatch(s, "LAIMX1"))
        m_frLAI1 = data;
    else if (StringMatch(s, "LAIMX2"))
        m_frLAI2 = data;
    else if (StringMatch(s, "DLAI"))
        m_frDPHU = data;
    else if (StringMatch(s, "BIO_E"))
        m_rueAmb = data;
    else if (StringMatch(s, "BIOEHI"))
        m_rueHi = data;
    else if (StringMatch(s, "CO2HI"))
        m_co2Hi = data;
    else if (StringMatch(s, "WAVP"))
        m_rueDcl = data;

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

    else if (StringMatch(s, "D_TMIN"))
        m_tMin = data;
    else if (StringMatch(s, "D_TMAX"))
        m_tMax = data;

    else if (StringMatch(s, "D_SR"))
        m_SR = data;
    else if (StringMatch(s, "D_RM"))
        m_RM = data;

    else
        throw ModelException("PotentialBiomass", "Set1DData", "Parameter " + s +
                                                              " does not exist in PotentialBiomass method. Please contact the module developer00.");

}

void PotentialBiomass::Get1DData(const char *key, int *n, float **data)
{
    //initialOutputs();
    string s(key);
    if (StringMatch(s, "BIOMASS_Delta"))
    {
        *data = m_biomassDelta;
    }
    else if (StringMatch(s, "BIOMASS"))
    {
        *data = m_biomass;
    }
    else if (StringMatch(s, "BIOMASS_NOpt"))
    {
        *data = m_biomassNOpt;
    }
    else if (StringMatch(s, "BIOMASS_POpt"))
    {
        *data = m_biomassPOpt;
    }

    else if (StringMatch(s, "VPD"))
    {
        *data = m_VPD;
    }

    else if (StringMatch(s, "Fr_PHU"))
    {
        if (m_frPHU == NULL)
        {
            m_frPHU = new float[m_nCells];
            for (int i = 0; i < m_nCells; i++)
                m_frPHU[i] = 0.0f;
        }
        *data = m_frPHU;
    }
    else if (StringMatch(s, "LAI_Delta"))
    {
        *data = m_LAIdelta;

    }
    else if (StringMatch(s, "LAI_Pre"))
    {
        *data = m_preLAI;

    }
    else if (StringMatch(s, "LAI"))
    {
        *data = m_LAI;

    }
    else if (StringMatch(s, "Fr_N"))
    {
        *data = m_frN;

    }
    else if (StringMatch(s, "Fr_P"))
    {
        *data = m_frP;

    }
    else if (StringMatch(s, "Fr_Root"))
    {
        *data = m_frRoot;

    }
    else
        throw ModelException("PotentialBiomass", "Get1DData", "Result " + s +
                                                              " does not exist in PotentialBiomass method. Please contact the module developer.");

    *n = m_nCells;
}

float PotentialBiomass::RadiationUseEfficiencyAdjustByVPD(float vpd, float radiationUseEfficiencyDeclineRateWithVPD)
{
    float thresholdVPD = 1.0f;
    if (vpd <= thresholdVPD)
        return 0.0f;
    return radiationUseEfficiencyDeclineRateWithVPD * (vpd - thresholdVPD);
}

float PotentialBiomass::doHeatUnitAccumulation(float potentialHeatUnit, float tMin, float tMax, float tBase)
{
    if (potentialHeatUnit <= 0.1)
    {
        return 0.f;
    }
    float frAccumulatedHeatUnit = 0;
    float tMean = (tMin + tMax) / 2;
    //tbase = m_param->MinTemperature();
    if (tMean <= tBase)
    {
        return 0.f;
    }
    frAccumulatedHeatUnit = (tMean - tBase) / potentialHeatUnit;
    return frAccumulatedHeatUnit;
}

void PotentialBiomass::getScurveShapeParameter(float xMid, float xEnd, float yMid, float yEnd, float *shape1,
                                               float *shape2)
{
    float xx = log(yMid / xMid - yMid);
    *shape2 = (xx - log(yEnd / xEnd - yEnd)) / (yEnd - yMid);
    *shape1 = xx + (yMid * (*shape2));
}

void PotentialBiomass::getNPShapeParameter(float *fr1, float *fr2, float *fr3, float *shape1, float *shape2)
{
    if (*fr1 - *fr2 < 0.0001f)
        *fr2 = *fr1 - 0.0001f;
    if (*fr2 - *fr3 < 0.0001f)
        *fr3 *= 0.75f;

    float t = *fr1 - *fr3;
    float xMid = 1.0f - (*fr2 - *fr3) / t;
    float xEnd = 1.0f - 0.00001f / t;

    getScurveShapeParameter(xMid, xEnd, 0.5f, 1.0f, shape1, shape2);
}

float PotentialBiomass::getNPFraction(float fr1, float fr3, float shape1, float shape2, float frPHU)
{
    return (fr1 - fr3) *
           (1.0f - frPHU / (frPHU + exp(shape1 - shape2 * frPHU)))
           + fr3;
}

float PotentialBiomass::NPBiomassFraction(float x1, float x2, float x3, float frPHU)
{
    float ShapeCoefficient1 = 0.f;
    float ShapeCoefficient2 = 0.f;
    getNPShapeParameter(&x1, &x2, &x3, &(ShapeCoefficient1), &(ShapeCoefficient2));

    return getNPFraction(x1, x3, ShapeCoefficient1, ShapeCoefficient2, frPHU);
}

int PotentialBiomass::Execute()
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

#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++)
    {
        if (m_date >= startDate && m_date <= endDate)
        {
            m_frPHU[i] += doHeatUnitAccumulation(m_PHU, m_tMin[i], m_tMax[i], m_tBase[i]);

            if (m_frLAI1 != NULL && m_frLAI2 != NULL && m_frPHU1 != NULL && m_frPHU2 != NULL)
            {
                getScurveShapeParameter(m_frLAI1[i], m_frLAI2[i], m_frPHU1[i], m_frPHU2[i],
                                        &(m_LAIShapeCoefficient1[i]), &(m_LAIShapeCoefficient2[i]));
            }

            //the fraction of the plant's maximum leaf area index for day i
            m_frLAImx[i] =
                    m_frPHU[i] / (m_frPHU[i] + exp(m_LAIShapeCoefficient1[i] - m_LAIShapeCoefficient2[i] * m_frPHU[i]));
            if (m_frPHU[i] <= m_frDPHU[i])//grow
            {

                if (m_preLAI[i] > m_maxLAI[i])
                {
                    m_preLAI[i] = m_maxLAI[i];
                }

                m_LAIdelta[i] = (m_frLAImx[i] - m_prefrLAImx[i]) * m_maxLAI[i] *
                                (1.0f - exp(5.0f * (m_preLAI[i] - m_maxLAI[i])));

                m_prefrLAImx[i] = m_frLAImx[i];
            }

            //m_biomassDelta
            //active radiation
            m_activeRadiation[i] = 0.5f * m_SR[i] * (1.0f - exp(-m_lightextinctioncoefficient[i] * (m_LAI[i] + 0.05f)));
            //rue:Radiation Use Efficiency Adjust By CO2
            m_rue[i] = 0.f;

            if (m_rueAmb != NULL && m_rueHi != NULL && m_co2 != 0 && m_co2Hi != NULL)
            {
                if (m_co2Hi[i] == 330.0f)
                {
                    m_co2Hi[i] = 660.f;
                }

                getScurveShapeParameter(m_rueAmb[i] * 0.01f, m_rueHi[i] * 0.01f, m_co2, m_co2Hi[i],
                                        &(m_CO2ShapeCoefficient1[i]), &(m_CO2ShapeCoefficient2[i]));
            }
            if (m_co2 < 330.0f)
            {
                m_rue[i] = m_rueAmb[i];
            }
            if (m_co2 = 330)
            {
                m_rue[i] =
                        100.0f * m_co2 / (m_co2 + exp(m_CO2ShapeCoefficient1[i] - m_CO2ShapeCoefficient2[i] * m_co2));
            }
            //rue:Radiation Use Efficiency Adjust By VPD
            //m_VPD
            m_ee[i] = 0.0f;
            m_tMean[i] = (m_tMin[i] + m_tMax[i]) / 2;
            if (abs(m_tMean[i] + 237.3f) >= 1.0e-6f)
                m_ee[i] = exp((16.78f * m_tMean[i] - 116.9f) / (m_tMean[i] + 237.3f));
            float rm = 0.4f;
            if (m_RM != NULL)
                rm = m_RM[i];
            m_VPD[i] = m_ee[i] * (1.0f - rm);
            //1KPa is the threshold vapor pressure deficit above which a plant will exhibit reduced rue
            if (m_VPD[i] > 1)
            {
                //omp_set_lock(&lock);
                m_rue[i] -= RadiationUseEfficiencyAdjustByVPD(m_VPD[i], m_rueDcl[i]);
                //omp_unset_lock(&lock);
                m_rue[i] = max(m_rue[i],
                               0.27f * m_rueAmb[i]);      //rue is never allowed to fall below 27% of rue ambient
            }
            m_biomassDelta[i] = max(0.0f, m_rue[i] * m_activeRadiation[i]);

            //omp_set_lock(&lock);
            if (m_frN1 != NULL && m_frN2 != NULL && m_frN3 != NULL && m_frP1 != NULL && m_frP2 != NULL &&
                m_frP3 != NULL)
            {
                m_frN[i] = NPBiomassFraction(m_frN1[i], m_frN2[i], m_frN3[i], m_frPHU[i]);
                m_frP[i] = NPBiomassFraction(m_frP1[i], m_frP2[i], m_frP3[i], m_frPHU[i]);
            }

            //omp_unset_lock(&lock);
            m_biomassNOpt[i] = m_frN[i] * m_biomass[i];
            m_biomassPOpt[i] = m_frP[i] * m_biomass[i];
            //N/P Stress

            m_frRoot[i] = 0.4f - 0.2f * m_frPHU[i];
        }

    }

    return 0;
}


bool PotentialBiomass::IsTree(int classification)
{
    return classification == 7;
}

bool PotentialBiomass::IsAnnual(int classification)
{
    return
            classification == 1 ||
            classification == 2 ||
            classification == 4 ||
            classification == 5;
}

bool PotentialBiomass::IsLegume(int classification)
{
    return classification <= 3;
}

bool PotentialBiomass::IsPerennial(int classification)
{
    return classification == 3 || classification == 6;
}

bool PotentialBiomass::IsCoolSeasonAnnual(int classification)
{
    return classification == 2 || classification == 5;
}
