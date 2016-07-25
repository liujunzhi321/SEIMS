#pragma once

#include <string>
#include <ctime>
#include "api.h"
#include "Common.h"
//#include "Climate.h"
using namespace std;

#include "SimulationModule.h"

class YLD : public SimulationModule
{
public:
    YLD(void);

    ~YLD(void);

    virtual int Execute();

    virtual void SetValue(const char *key, float data);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Set1DData(const char *key, int n, time_t *data);

    virtual void Get1DData(const char *key, int *n, float **data);

    bool CheckInputSize(const char *key, int n);

    bool CheckInputData(void);

private:

    int m_nCells;
    time_t m_harvestDate;
    bool m_harvested;

    //land cover/plant classification:1-7
    float *m_classification;

    float *m_HiMin;
    float *m_HiOpt;
    float *m_frNyld;
    float *m_frPyld;
    float *m_frDeclineLAI;
    //global variation
    //potential harvest index for a given day
    float *m_HI;
    //actual ET simulated during life of plant
    float *m_totalPlantET;
    //potential ET simulated during life of plant
    float *m_totalPlantPET;
    float *m_wur;
    float *m_Hiact;
    float *m_harvestEfficiency;
    float *m_clip;
    float *m_clipN;
    float *m_clipP;
    float *m_removeFraction;
    float *m_rootFraction;
    float *m_rootLeft;
    float *m_rootRemove;
    float *m_rootRemoveFraction;
    float *m_rootRemoveN;
    float *m_rootRemoveP;


    //input
    float *m_PET;
    float *m_soilET;
    float *m_biomass;
    float *m_biomassN;
    float *m_biomassP;
    float *m_biomassAG;
    float *m_biomassRoot;
    float *m_frPHU;
    float *m_LAI;
    float *m_totalWaterUptake;

    //result
    float *m_yield;
    float *m_yieldN;
    float *m_yieldP;

    Common *m_common;
    //Climate* m_climate;*/
    //accumulate heat unit to determine the growth stage
    //it's the base of plant growth

    void initialOutputs();
};

