#pragma once
using namespace std;
#define MAX_CROP_LAND_USE_ID 97
#define BARE_SOIL_LAND_USE     98
#define WATER_LAND_USE_ID     18

class Common
{
public:
    Common(void);

    ~Common(void);

    int m_classification;
private:
    //Land cover/plant classification, IDC

    //Radiation-use efficicency or biomass-energy ratio ((kg/ha)/(MJ/m**2)),BIO_E
    float m_radiationUseEfficiency;
    //Number of years required for tree species to reach full development (years), MAT_YRS
    int m_treeFullDevelopmentYear;
    //Maximum canopy height (m), CHTMX
    float m_maxHeight;
    //Maximum biomass for a forest (metric tons/ha), BMX_TREES
    float m_maxBiomass;
    //Nitrogen uptake parameter #1: normal fraction of nitrogen in plant biomass at emergence (kg N/kg biomass), BN1
    float m_biomassNFraction1;

    //Nitrogen uptake parameter #2: normal fraction of nitrogen in plant biomass at 50% maturity (kg N/kg biomass), BN2
    float m_biomassNFraction2;

    //Nitrogen uptake parameter #3: normal fraction of nitrogen in plant biomass at maturity (kg N/kg biomass), BN3
    float m_biomassNFraction3;

    //Nitrogen uptake parameter #1: normal fraction of phosphorus in plant biomass at emergence (kg P/kg biomass), BP1
    float m_biomassPFraction1;

    //Nitrogen uptake parameter #2: normal fraction of phosphorus in plant biomass at 50% maturity (kg P/kg biomass), BP2
    float m_biomassPFraction2;

    //Nitrogen uptake parameter #3: normal fraction of phosphorus in plant biomass at maturity (kg P/kg biomass), BP3
    float m_biomassPFraction3;
    //Fraction of the plant growing season or fraction of total potential head units corresponding to the 1st point on the optimal leaf area development curve, FRGRW1
    float m_x1;

    //Fraction of the maximum leaf area index corresponding to the 1st point on the optimal leaf area development curve, LAIMX1
    float m_y1;

    //Fraction of the plant growing season or fraction of total potential head units corresponding to the 2nd point on the optimal leaf area development curve, FRGRW2
    float m_x2;

    //Fraction of the maximum leaf area index corresponding to the 2nd point on the optimal leaf area development curve, LAIMX2
    float m_y2;
    //Elevated CO2 atmospheric concentration (uL CO2/L air) corresponding the 2nd on the radition use efficiency curve, CO2HI
    float m_secondCO2Concentration;
    //Biomass-energy ratio corresponding to the 2nd point on the radiation use efficiency curve, BIOEHI
    float m_secondBiomassEnergyRatio;
    //Rate of decline in radiation use efficiency per unit increase in vapor pressure deficit, WAVP
    float radiationUseEfficiencyDeclineRateWithVPD;
    /****** The following parameters are shape coefficients used by equations, Zhiqiang, 2011-8-10 ***********/
private:
    // Plant nitrogen equation (P300, e.q 5:2.3.1) shape coefficient #1, n1
    float m_biomassNShapeCoefficient1;

    // Plant nitrogen equation (P300, e.q 5:2.3.1) shape coefficient #2, n2
    float m_biomassNShapeCoefficient2;

    // Plant phosphorus equation (P305, e.q 5:2.3.19) shape coefficient #1, p1
    float m_biomassPShapeCoefficient1;

    // Plant phosphorus equation (P305, e.q 5:2.3.19) shape coefficient #2, p2
    float m_biomassPShapeCoefficient2;

    // CO2 radiation-use efficiency adjust equation (p288, e.q. 5:2.1.4) shape coefficient #1, r1
    float m_CO2ShapeCoefficient1;

    // CO2 radiation-use efficiency adjust equation (p288, e.q. 5:2.1.4) shape coefficient #2, r2
    float m_CO2ShapeCoefficient2;

    // optimal leaf area development curve shape coefficient 1 (p292 5:2.1.10)
    float m_LAIShapeCoefficient1;

    // optimal leaf area development curve shape coefficient 2 (p292 5:2.1.10)
    float m_LAIShapeCoefficient2;
public:
    //get N/P shape parameter
    void getNPShapeParameter(float *fr1, float *fr2, float *fr3, float *shape1, float *shape2);

    //computes shape parameters shape1 and shape2 for the S curve
    //equation x = y/(y + exp(shape1 + shape2*y)) given 2 (x,y) points along the curve.
    //See ascrv.f of SWAT
    static void getScurveShapeParameter(float xMid, float xEnd, float yMid, float yEnd, float *shape1, float *shape2);

    //plant nitrogen/phosphorus equation, p300 5:2.3.1/p305 5:2.3.19
    //calculate the fraction of nitrogen/phosphorus in the plant biomass
    static float getNPFraction(float fr1, float fr3, float shape1, float shape2, float frPHU);

public:
    float doHeatUnitAccumulation(float potentialHeatUnit, float tMin, float tMax, float tBase);

    float NPBiomassFraction(float x1, float x2, float x3, float frPHU);

public:
    //fraction of nitrogen in the plant biomass
    float NBiomassFraction(float frPHU) const;

    //fraction of phosphorus in the plant biomass
    float PBiomassFraction(float frPHU) const;

    //the adjusted radiation-use efficiency by CO2 concentration
    float RadiationUseEfficiencyAdjustByCO2(float co2) const;

    //the adjusted radiation-use efficiency by vapor pressure deficit
    float RadiationUseEfficiencyAdjustByVPD(float vpd, float radiationUseEfficiencyDeclineRateWithVPD) const;

    //amount of biomass a tree can accumulate in a single year, kg/ha
    float TreeAnnualBiomass(int age) const;

    //tree height in a single year, m
    float TreeAnnualHeight(int age) const;

    //fraction of the plant's maximum leaf area index corresponding to a given fraction of potential heat units for the plant
    float LAIFraction(float frPHU) const;

    float getNormalization(float distribution) const;

    bool IsTree(void) const { return m_classification == 7; }

    bool IsAnnual(void) const
    {
        return
                m_classification == 1 ||
                m_classification == 2 ||
                m_classification == 4 ||
                m_classification == 5;
    }

    bool IsLegume(void) const { return m_classification <= 3; }

    bool IsPerennial(void) const
    {
        return
                m_classification == 3 ||
                m_classification == 6;
    }

    bool IsCoolSeasonAnnual() const
    {
        return
                m_classification == 2 ||
                m_classification == 5;
    }

    bool IsGrain() const { return m_classification == 4; }
    /*bool IsPlant(void) const {return m_id <= MAX_CROP_LAND_USE_ID &&
        m_id != WATER_LAND_USE_ID;}*/
};


