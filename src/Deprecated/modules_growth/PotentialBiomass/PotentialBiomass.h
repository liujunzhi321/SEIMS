#pragma once

#include <string>
#include <ctime>
#include "api.h"
#include "Common.h"
#include "Climate.h"
#include "SimulationModule.h"

using namespace std;

class PotentialBiomass : public SimulationModule
{
public:
    PotentialBiomass(void);

    ~PotentialBiomass(void);

    virtual int Execute();

    virtual void SetValue(const char *key, float data);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Set1DData(const char *key, int n, time_t *data);

    virtual void Get1DData(const char *key, int *n, float **data);

    bool CheckInputSize(const char *key, int n);

    bool CheckInputData(void);

private:
    int m_nCells;
    time_t m_startDate;
    time_t m_endDate;
    //land cover/plant  classification:1-7
    float *m_classification;
    //light extinction coefficient
    float *m_lightextinctioncoefficient;
    //initial PotentialBiomass
    float m_initBiomass;
    //float* m_maxBiomass;
    float m_initLAI;
    float *m_maxLAI;
    float m_curYear;
    float *m_fullDevYear;
    float *m_tBase;
    //float* m_tOpt;
    float m_PHU;
    float *m_frPHU1;
    float *m_frPHU2;
    float *m_frLAI1;
    float *m_frLAI2;
    float *m_frDPHU;
    float m_co2;
    float *m_rueAmb;
    float *m_co2Hi;
    float *m_rueHi;
    float *m_rueDcl;
    float *m_frN1;
    float *m_frN2;
    float *m_frN3;
    float *m_frP1;
    float *m_frP2;
    float *m_frP3;
    float *m_frLAImx;
    //float* m_frTreeDormentResidue;
    float *m_LAIShapeCoefficient1;
    float *m_LAIShapeCoefficient2;
    float *m_CO2ShapeCoefficient1;
    float *m_CO2ShapeCoefficient2;
    float *m_activeRadiation;
    float *m_rue;
    float *m_ee;
    float *m_tMean;
    float *m_tMin;
    float *m_tMax;
    float *m_SR;
    float *m_RM;
    //float* m_latitude;
    //the fraction of the plant��s maximum leaf area index for day i-1
    float *m_prefrLAImx;
    //the fraction of the plant��s maximum leaf area index for day i

    // total plant water uptake, it is also the actual amount of transpiration
    //float* m_totalWUptake;

    //float* m_transpirationMax;

    //float m_LAIBeforeDecline;

    //result
    // the amount of biomass a tree can accumulate in a single year
    //float* m_bioTreeAnnual;
    float *m_biomassDelta;
    //float* m_prebiomass;
    float *m_biomass;
    float *m_biomassNOpt;
    float *m_biomassPOpt;
    //float* m_biomassN;
    //float* m_biomassP;
    //float* m_biomassRoot;
    //float* m_biomassAG;
    float *m_VPD;
    float *m_frPHU;
    // the leaf area indices for day i-1
    float *m_preLAI;
    // the leaf area indices for day i
    float *m_LAI;
    float *m_LAIdelta;
    float *m_frN;
    float *m_frP;
    float *m_frRoot;

    Common *m_common;
    //Climate* m_climate;


    //accumulate heat unit to determine the growth stage
    //it's the base of plant growth
    float doHeatUnitAccumulation(float potentialHeatUnit, float tMin, float tMax, float tBase);

    void getScurveShapeParameter(float xMid, float xEnd, float yMid, float yEnd, float *shape1, float *shape2);

    float RadiationUseEfficiencyAdjustByVPD(float vpd, float radiationUseEfficiencyDeclineRateWithVPD);

    void getNPShapeParameter(float *fr1, float *fr2, float *fr3, float *shape1, float *shape2);

    float getNPFraction(float fr1, float fr3, float shape1, float shape2, float frPHU);

    float NPBiomassFraction(float x1, float x2, float x3, float frPHU);

    void initialOutputs();

    bool IsTree(int classification);

    bool IsAnnual(int classification);

    bool IsLegume(int classification);

    bool IsPerennial(int classification);

    bool IsCoolSeasonAnnual(int classification);

};

