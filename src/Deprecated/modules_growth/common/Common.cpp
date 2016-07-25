#include "Common.h"
#include "util.h"
#include <math.h>
#include <stdlib.h>
#include <sstream>


Common::Common()
{
    //get coefficients, Zhiqiang, 2011-8-10

    //N uptake shape coefficient, Line 287-298, readplant.f
    getNPShapeParameter(&(m_biomassNFraction1), &(m_biomassNFraction2), &(m_biomassNFraction3),
                        &(m_biomassNShapeCoefficient1), &(m_biomassNShapeCoefficient2));

    //P uptake shape coefficient, Line 303-314, readplant.f
    getNPShapeParameter(&(m_biomassPFraction1), &(m_biomassPFraction2), &(m_biomassPFraction3),
                        &(m_biomassPShapeCoefficient1), &(m_biomassPShapeCoefficient2));

    //CO2 radiation-use efficiency adjust shape coefficient, Line 270-279, readplant.f
    if (m_secondCO2Concentration == 330.0f) m_secondCO2Concentration = 660.0f;
    getScurveShapeParameter(
            m_radiationUseEfficiency * 0.01f,
            m_secondBiomassEnergyRatio * 0.01f,
            330.0f,
            m_secondCO2Concentration,
            &(m_CO2ShapeCoefficient1),
            &(m_CO2ShapeCoefficient2));

    //optimal leaf area development curve shape coefficient, Line 264, readplant.f
    getScurveShapeParameter(
            m_x1, m_x2,
            m_y1, m_y2,
            &(m_LAIShapeCoefficient1),
            &(m_LAIShapeCoefficient2));
}

Common::~Common(void)
{
}

void Common::getNPShapeParameter(float *fr1, float *fr2, float *fr3, float *shape1, float *shape2)
{
    if (*fr1 - *fr2 < 0.0001f) *fr2 = *fr1 - 0.0001f;
    if (*fr2 - *fr3 < 0.0001f) *fr3 *= 0.75f;

    float t = *fr1 - *fr3;
    float xMid = 1.0f - (*fr2 - *fr3) / t;
    float xEnd = 1.0f - 0.00001f / t;

    getScurveShapeParameter(xMid, xEnd, 0.5f, 1.0f, shape1, shape2);
}

void Common::getScurveShapeParameter(float xMid, float xEnd, float yMid, float yEnd, float *shape1, float *shape2)
{
    float xx = log(yMid / xMid - yMid);
    *shape2 = (xx - log(yEnd / xEnd - yEnd)) / (yEnd - yMid);
    *shape1 = xx + (yMid * (*shape2));
}

float Common::getNPFraction(float fr1, float fr3, float shape1, float shape2, float frPHU)
{
    return (fr1 - fr3) *
           (1.0f - frPHU / (frPHU + exp(shape1 - shape2 * frPHU)))
           + fr3;
}

float Common::NBiomassFraction(float frPHU) const
{
    return getNPFraction(
            m_biomassNFraction1,
            m_biomassNFraction3,
            m_biomassNShapeCoefficient1,
            m_biomassNShapeCoefficient2,
            frPHU);
}

float Common::PBiomassFraction(float frPHU) const
{
    return getNPFraction(
            m_biomassPFraction1,
            m_biomassPFraction3,
            m_biomassPShapeCoefficient1,
            m_biomassPShapeCoefficient2,
            frPHU);
}

float Common::RadiationUseEfficiencyAdjustByCO2(float co2) const
{
    float r1 = m_CO2ShapeCoefficient1;
    float r2 = m_CO2ShapeCoefficient2;

    return 100.0f * co2 / (co2 + exp(r1 - r2 * co2));
}

float Common::RadiationUseEfficiencyAdjustByVPD(float vpd, float radiationUseEfficiencyDeclineRateWithVPD) const
{
    float thresholdVPD = 1.0f;
    if (vpd <= thresholdVPD) return 0.0f;
    return radiationUseEfficiencyDeclineRateWithVPD * (vpd - thresholdVPD);
}

float Common::TreeAnnualBiomass(int age) const
{
    return 1000.0f * m_maxBiomass * age / m_treeFullDevelopmentYear;
}

float Common::TreeAnnualHeight(int age) const
{
    return m_maxHeight * age / m_treeFullDevelopmentYear;
}

float Common::LAIFraction(float frPHU) const
{
    float l1 = m_LAIShapeCoefficient1;
    float l2 = m_LAIShapeCoefficient2;
    return frPHU / (frPHU + exp(l1 - l2 * frPHU));
}

float Common::getNormalization(float distribution) const
{
    return 1 - exp(-distribution);
}

float Common::doHeatUnitAccumulation(float potentialHeatUnit, float tMin, float tMax, float tBase)
{
    if (potentialHeatUnit <= 0.1)
        return 0.f;
    float frAccumulatedHeatUnit = 0;
    float tMean = (tMin + tMax) / 2;
    //tbase = m_param->MinTemperature();
    if (tMean <= tBase)
        return 0.f;
    frAccumulatedHeatUnit += (tMean - tBase) / potentialHeatUnit; // TODO: make sure this is += or = , 2015.6.5, ZhuLJ
    return frAccumulatedHeatUnit;
}

float Common::NPBiomassFraction(float x1, float x2, float x3, float frPHU)
{
    float ShapeCoefficient1 = 0.f;
    float ShapeCoefficient2 = 0.f;
    getNPShapeParameter(&x1, &x2, &x3, &(ShapeCoefficient1), &(ShapeCoefficient2));

    return getNPFraction(x1, x3, ShapeCoefficient1, ShapeCoefficient2, frPHU);
}
