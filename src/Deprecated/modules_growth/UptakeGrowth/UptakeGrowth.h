#pragma once

#include <string>
#include <ctime>
#include "api.h"
#include "Common.h"
//#include "Climate.h"
#include "SimulationModule.h"

using namespace std;

class UptakeGrowth : public SimulationModule
{
public:
    UptakeGrowth(void);

    ~UptakeGrowth(void);

    virtual int Execute();

    virtual void SetValue(const char *key, float data);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Set2DData(const char *key, int nRows, int nCols, float **data);

    virtual void Get1DData(const char *key, int *n, float **data);

    virtual void Get2DData(const char *key, int *nRows, int *nCols, float ***data);

    bool CheckInputSize(const char *key, int n);

    bool CheckInputData(void);

private:

    int m_nCells;
    time_t m_startDate;
    time_t m_endDate;
    // layer num, layer boundary num = layer num +1
    //int m_layers;
    //parameters
    //land cover/plant classification:1-7
    float *m_classification;
    float m_NFixFactor;
    float m_NFixDailyMax;

    float m_waterDis;
    float m_waterCom;
    //nitrogen uptake distribution parameter
    float m_NDis;
    float m_PDis;
    float m_layers; //add on 2013.10.29

    float *m_tBase;
    float *m_tOpt;

    float *m_rootDepthmax;
    //rate must transform to mm h2o
    float **m_layerFieldCapacity;
    //update on 2013.10.29
    float **m_layerWiltingPnt;
    //update on 2013.10.29
    float *m_totalFieldCapacity;//update on 2013.10.29

    float m_initBiomass;
    float *m_maxBiomass;
    float m_curYear;
    float *m_fullDevYear;
    float m_PHU;
    float *m_frDPHU;

    float *m_frN1;
    float *m_frN2;
    float *m_frN3;
    float *m_frP1;
    float *m_frP2;
    float *m_frP3;
    float *m_maxLAI;

    float *m_zRoot;
    float *m_WnormalizationParam;
    float *m_NnormalizationParam;
    float *m_PnormalizationParam;
    float *m_waterLastDepth;    //the water uptake in last depth
    float *m_waterThisDepth;
    float *m_waterTotal;        //total water uptake of all layers
    float **m_layerAvailableWaterCapacity;//update on 2013.10.29

    float *m_potentialNUptake;
    float *m_potentialPUptake;
    float *m_NLastDepth;    //the N uptake in last depth
    float *m_NThisDepth;
    float *m_NTotal;        //total N uptake of all layers
    float *m_PLastDepth;    //the N uptake in last depth
    float *m_PThisDepth;
    float *m_PTotal;        //total N uptake of all layers
    //growth stage factor
    float *m_fGrowthStage;
    //soil water factor(0.0-1.0)
    float *m_fSoilWater;
    //////must be total field capacity
    //soil nitrate factor(0.0-1.0)
    float *m_fSoilNitrate;

    //input
    float **m_layerNO3;
    float **m_layerP;
    float *m_totalNO3;

    float *m_tMin;
    float *m_tMax;
    float *m_PET;

    float **m_layerSoilWater;
    //update on 2013.10.29
    float *m_totalSoilWater;//update on 2013.10.29

    float *m_LAIdelta;
    float *m_preLAI;
    float *m_LAI;

    float *m_frPHU;
    float *m_frN;
    float *m_frP;
    float *m_frRoot;

    float *m_biomassDelta;
    float *m_biomass;
    float *m_biomassNOpt;
    float *m_biomassPOpt;

    //stress factors
    //float* m_aerationStress;
    float *m_waterStress;
    float *m_tempStress;
    float *m_NStress;
    float *m_PStress;
    float *m_totalStress;

    //result
    float *m_transpirationMax; //it is also the water demand
    float *m_depthArray;//3 rows

    float *m_bioTreeAnnual;
    float *m_biomassN;
    float *m_biomassP;
    float *m_biomassRoot;
    float *m_biomassAG;

    float **m_layerWUptake;
    float **m_layerNUptake;
    float **m_layerPUptake;

    float *m_NFixation;
    float *m_totalWUptake;
    float *m_totalNUptake;
    float *m_totalPUptake;


    ////this stress is not descriped in theory documentation but appears in
    //codes of SWAT2009 in Line128 swu.f and Line225 grow.f--zhiqiang




    Common *m_common;
    //Climate* m_climate;
    //accumulate heat unit to determine the growth stage
    //it's the base of plant growth
    //void doHeatUnitAccumulation(Climate* climate);

    void initialOutputs();

    float GrowthStageFactorForNitrogenFixation(float frAccumulatedHeatUnit);

    float WaterFactorForNitrogenFixation(float totalWater, float totalFieldCapacity);

    float NitrateFactorForNitrogenFixation(float totalNO3);

    float AerationStress(float totalWater, float totalFieldCapacity, float totalSaturation);

    float getWaterStress(float maxTranspiration, float actualTranspiration);

    float getTemperatureStress(float tMin, float tMax, float tBase, float tOpt);

    float getNPStress(float actual, float optimal);
};

