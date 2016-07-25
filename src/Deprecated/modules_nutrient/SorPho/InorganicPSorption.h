#pragma once

#include <string>
#include <ctime>
#include <cmath>
#include <map>
#include "SimulationModule.h"

using namespace std;

class InorganicPSorption : public SimulationModule
{
public:
    InorganicPSorption(void);

    ~InorganicPSorption(void);

    virtual int Execute();

    virtual void SetValue(const char *key, float data);

    //virtual void Set1DData(const char* key, int n, float* data);
    //virtual void Get1DData(const char* key, int* n, float** data);
    virtual void Set2DData(const char *key, int nrows, int ncols, float **data);

    virtual void Get2DData(const char *key, int *nRows, int *nCols, float ***data);

    bool CheckInputSize(const char *key, int n);

    bool CheckInputData(void);

private:

    void initialOutputs();

    // size of array
    int m_size;
    int m_nLayers;

    // input
    // slow equilibration rate constant (0.0006d-1)
    float m_sloER;

    //float* m_aftSolP;
    // amount of phosphorus in solution before fertilization
    //float* m_befSolP;
    // amount of soluble P fertilizer added to the sample
    //float* m_solPFer;

    // amount of phosphorus in solution in layer ly(kg P/ha)
    float **m_SoluP;

    // output
    // phosphorus availability index
    float **m_pai;
    // amount of phosphorus in the active mineral pool (kg P/ha)
    float **m_actMinP;
    // amount of phosphorus in the stable mineral pool (kg P/ha)
    float **m_staMinP;

    // amount of phosphorus transferred between the soluble and active mineral pool (kg P/ha)
    float **m_tSolActP;
    // amount of phosphorus transferred between the active and stable mineral pools (kg P/ha)
    float **m_tStaActP;

};