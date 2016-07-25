/*----------------------------------------------------------------------
*	Purpose: 	Overland routing using 4-point implicit finite difference method
*
*	Created:	Wang Lin
*	Date:		23-July-2012
*
*	Revision:
*   Date:
*---------------------------------------------------------------------*/

#pragma once

#include <string>
#include <ctime>
#include <cmath>
#include <map>
#include "SimulationModule.h"

using namespace std;

class SedBonNutrient_OL : public SimulationModule
{
public:
    SedBonNutrient_OL(void);

    ~SedBonNutrient_OL(void);

    virtual int Execute();

    virtual void SetValue(const char *key, float data);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Get1DData(const char *key, int *n, float **data);

    virtual void Set2DData(const char *key, int nrows, int ncols, float **data);

    bool CheckInputSize(const char *key, int n);

    bool CheckInputData(void);

private:
    /// deal with positive and negative float numbers
    float Power(float a, float n)
    {
        if (a >= 0)
            return pow(a, n);
        else
            return -pow(-a, n);
    }

    void OverlandFlow(int id);

    void initialOutputs();

    void concentration(int id);

    // calculate the velocity of channel flow
    /**
    *	@brief calculate the velocity of overland flow.
    */
    void CalcuVelocityChannelFlow();

    /// size of array
    int m_nCells;

    /// cell width of the grid (m)
    float m_CellWidth;
    /// time step (second)
    float m_dt;
    /// Slope
    float *m_Slope;
    ///  initial average concentration of nutrient-P in the soil(kg/kg-soil);
    float m_initConcP;
    ///  initial average concentration of nutrient-NH4 in the soil(kg/kg-soil);
    float m_initConcNH4;
    ///  initial average concentration of nutrient-NO3 in the soil(kg/kg-soil);
    float m_initConcNO3;
    /// concP at previous time step
    float *m_PreConcP;
    /// concNH4 at previous time step
    float *m_PreConcNH4;
    /// concNO3 at previous time step
    float *m_PreConcNO3;

    /// clay fraction
    float *m_clayFrac;
    /// soil porosity
    float *m_Porosity;
    // soil moisture
    float *m_soilMoisture;
    /// Soil density
    float *m_Density;
    /// channel width (zero for non-channel cells)
    float *m_chWidth;
    /**
    *	@brief 2d array of flow in cells
    *
    *	The first element in each sub-array is the number of flow in cells in this sub-array
    */
    float **m_flowInIndex;
    /**
    *	@brief Routing layers according to the flow direction
    *
    *	There are not flow relationships within each layer.
    *	The first element in each layer is the number of cells in the layer
    */
    float **m_routingLayers;
    int m_nLayers;

    /// runoff rate(m3/s)
    float *m_CELLQ;
    /// water depth (mm)
    float *m_CELLH;
    /// infiltration map of watershed (mm) of the total nCells
    float *m_infil;
    /// depression storage
    float *m_sd;
    ///distribution of sediment content in flow
    float *m_SedKG;

    /// distribution of sediment bound P (kg/s)
    float *m_SedBonP;
    /// distribution of sediment bound ammonium (kg/s)
    float *m_SedBonAmmonium;
    /// sediment bound P flow to channel (kg)
    float *m_SedBonPToCh;
    /// sediment bound ammonium flow to channel (kg)
    float *m_SedBonAmmoniumToCh;
    /// Water depth added to channel water depth
    float *m_whtoCh;

    /// lateral inflow (from other cells)
    //float *m_qLat;
    /// the sum of the initial values of the nutrient specie (kg/s)
    float *m_PPT_P;
    float *m_PPT_NH4;
    /// concentration of sediment bound P
    float *m_CP_Sed;
    /// concentration of sediment bound NH4
    float *m_CNH4_Sed;
    /// flow width of each cell
    float *m_flowWidth;
    /*/// flow length of each cell
    float *m_flowLen;*/
    /// stream link
    float *m_streamLink;
    ///  the ratio of runoff and storage volume
    float m_X1;
    ///  the ratio of runoff plus infiltration and storage volume
    float m_X2;
    /// the ratio of runoff and storage volume
    float m_X3;

    /// flow velocity
    float *m_ChV;
    /// Manning's roughness [-]
    float *m_ManningN;
    /// cell area m2
    float *m_cellA;
};

