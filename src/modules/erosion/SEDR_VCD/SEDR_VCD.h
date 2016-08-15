/*!
 * \file SEDR_VCD.h
 * \brief Sediment routing using variable channel dimension(VCD) method at daily time scale
 * \author Hui Wu
 * \date Jul. 2012
 * \revised LiangJun Zhu
 * \date May/ 2016
 */
#pragma once
#define DEPTH_INTERVAL 0.001f



#include <string>
#include <ctime>
#include <cmath>
#include <map>
#include <vector>
#include "SimulationModule.h"

using namespace std;
/** \defgroup SEDR_VCD
 * \ingroup Erosion
 * \brief Sediment routing using variable channel dimension(VCD) method at daily time scale
 */
/*!
 * \class SEDR_VCD
 * \ingroup SEDR_VCD
 *
 * \brief Sediment routing using variable channel dimension(VCD) method at daily time scale
 *
 */
class SEDR_VCD : public SimulationModule
{
public:
    //! Constructor
    SEDR_VCD(void);

    //! Destructor
    ~SEDR_VCD(void);

    virtual int Execute();

    virtual void SetValue(const char *key, float data);

    virtual void GetValue(const char *key, float *value);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Get1DData(const char *key, int *n, float **data);

    virtual void Set2DData(const char *key, int nrows, int ncols, float **data);

    virtual void Get2DData(const char *key, int *nRows, int *nCols, float ***data);

    bool CheckInputSize(const char *key, int n);

    bool CheckInputSizeChannel(const char *key, int n);

    bool CheckInputData(void);

    virtual TimeStepType GetTimeStepType() { return TIMESTEP_CHANNEL; };
private:
    /// time step (sec)
    int m_dt;
    /// reach number (= subbasin number)
    int m_nreach;

    /// diversion loss (Vdiv) of the river reach, m_Vid[id], id is the reach id
    float *m_Vdiv;
    /// The point source discharge, m_Vpoint[id], id is the reach id
    float *m_Vpoint;

    /// the peak rate adjustment factor
    float m_prf;
    ///  Coefficient in sediment transport equation
    float m_spcon;
    /// Exponent in sediment transport equation
    float m_spexp;
    /// critical velocity for sediment deposition
    float m_vcrit;
    /// reach cover factor
    float m_coverFactor;
    /// channel erodibility factor (cm/hr/Pa)  TODO: this should be an input parameter from database, LJ
    float m_erodibilityFactor;

    /// inverse of flood plain side slope of channel, is a fixed number:  1/slope
    float m_sideslopeFloodplain;
    /// inverse of side slope of main channel, is a fixed number:  1/slope
    float m_sideslopeMain;
    /// width ratio
    float m_w_ratio;
    /// bank full water storage
    float m_bankfullQ;
    /// sediment from subbasin
    float *m_sedtoCh;
    /// the subbasin area (m2)  //add to the reach parameters file
    //float* m_area;
    /// cross-sectional area of flow in the channel (m2)
    float *m_CrAreaCh;
    /// initial channel storage per meter of reach length (m3/m)
    float m_Chs0;

    //temporary at routing time
    /// reach storage (m3) at time t
    //float* m_chStorage;
    /// channel outflow
    float *m_qchOut;

    float *m_chOrder;
    float *m_chWidth;
    float *m_chDepth;
    float *m_chLen;      //length of reach (m)
    float *m_chVel;
    float *m_chSlope;
    float *m_chManning;

    /// downstream id (The value is 0 if there if no downstream reach)
    float *m_reachDownStream;
    /// upstream id (The value is -1 if there if no upstream reach)
    vector<vector<int> > m_reachUpStream;

    // id the reaches
    float *m_reachId;
    /// map from subbasin id to index of the array
    map<int, int> m_idToIndex;

    /// reach storage (m3) at time t
    float *m_chStorage;
    /// channel water depth m
    float *m_chWTdepth;

    // OUTPUT
    /// initial reach sediment out (metrics ton) at time t
    float *m_SedOut;
    /// Channel sediment balance in a text format for each reach and at each time step
    //float** m_T_CHSB;
    /// channel sediment storage (ton)
    float *m_sedStorage;
    /// sediment of deposition
    float *m_sedDep;
    /// sediment of degradation
    float *m_sedDeg;
	/// sediment concentration
	float *m_sedCon;

    map<int, vector<int> > m_reachLayers;

    void initialOutputs();

    void SedChannelRouting(int i);

    void doChannelDowncuttingAndWidening(int id);
};

