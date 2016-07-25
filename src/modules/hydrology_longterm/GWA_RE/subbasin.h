#pragma once

#include <vector>
#include <map>

#include <string>
#include <cmath>

using namespace std;

class subbasin
{
public:
    subbasin(int, bool);

    ~subbasin(void);

private:
    //subbasin id
    int m_id;

    //all the cells
    int m_nCells;
    int *m_cells;

    //inputs
    float m_PET;
    //float m_EI;
    //float m_ED;
    //float m_ES;
    float m_PERCO;
    float m_cellArea;    //area of one cell
    float m_Area;        //area of this subbasin

    //parameters
    //float m_GW0;
    float m_GWMAX;
    float m_kg;
    float m_dp_co;
    float m_base_ex;
    float m_QGCoefficient;

    float m_slopeCoefficient;
    float m_slope;

    float m_GW;
    float m_PERDE;
    float m_EG;
    float m_RG;
    float m_QG;

    bool m_isOutput; //If it's listed in file.out, it's true.
    bool m_isRevapChanged; //if the revap (EG) is different with last time step.
public:
    int *getCells();

    void setCellList(int, int *);

    void setParameter(float rv_co, float GW0, float GWMAX, float kg, float base_ex, float cellWidth, int timeStep);

    void setSlope(float *slope);

    void setSlopeBasin(float slopeBasin);

    void setInputs(float *PET, float *EI, float *ED, float *ES, float **PERCO, float groundwaterFromBankStorage);

    int getId();

    int getCellCount();

    bool getIsOutput();

    bool getIsRevapChanged();

    float getPerco();

    float getPerde();

    float getSlope();

    float getEG();

    float getGW();

    float getRG();

    float getQG();

    float getPET();

    static float subbasin2basin(string key, vector<subbasin *> *subbasins);
};

