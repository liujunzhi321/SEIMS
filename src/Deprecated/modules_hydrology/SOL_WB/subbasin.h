#pragma once

#include <vector>
#include <map>
#include <string>

using namespace std;

class subbasin
{
public:
    subbasin(int);

    ~subbasin(void);

private:
    //subbasin id
    int m_id;

    //all the cells
    vector<int> m_cells;

    double m_P;
    double m_Interception;
    double m_DepressionET;
    double m_Infiltration;
    double m_TotalET;
    double m_soilET;
    double m_NetPercolation;
    double m_Revap;
    double m_RS;
    double m_RI;
    double m_RG;
    double m_R;
    double m_S_MOI;
    double m_moistureDepth;
    double m_interceptionET;
    double m_pNet;
    double m_T;
    double m_Soil_T;

public:
    void addCell(int);

    float getAverage(string key);

    vector<int> *getCells();

    void addP(float);

    void addNetP(float);

    void addInterception(float);

    void addInterceptionET(float);

    void addDepressionET(float);

    void addInfiltration(float);

    void addTotalET(float);

    void addNetPercolation(float);

    void addRevap(float);

    void addRS(float);

    void addRI(float);

    void addRG(float);

    void addR(float);

    void addSMOI(float);

    void addT(float);

    void addSoilT(float);

    void addSoilET(float soilET);

    void addMoistureDepth(float depth);

    void clear();

    int getId();
};

