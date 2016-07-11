/*!
 * \file SNO_SP.h
 * \brief
 * \author Zhiqiang Yu
 * \date May 2011
 * \revised LiangJun Zhu
 * \date 2016-5-29
 *  1. Remove m_isInitial and add initialOutputs()
 * 
 */
#pragma once

#include <string>
#include <ctime>
#include "api.h"

using namespace std;

#include "SimulationModule.h"
/** \defgroup SNO_SP
 * \ingroup Hydrology_longterm
 * \brief Calculate snow melt by snowpack daily method from SWAT
 *
 */

/*!
 * \class SNO_SP
 * \ingroup SNO_SP
 * \brief Calculate snow melt by snowpack daily method from SWAT
 * 
 */
class SNO_SP : public SimulationModule
{
public:
    //! Constructor
    SNO_SP(void);

    //! Destructor
    ~SNO_SP(void);

    virtual int Execute();

    virtual void SetValue(const char *key, float data);

    virtual void Set1DData(const char *key, int n, float *data);

    virtual void Get1DData(const char *key, int *n, float **data);

    bool CheckInputSize(const char *key, int n);

    bool CheckInputData(void);

private:
    //! Valid cells number
    int m_nCells;
    //! Current date
    ///time_t m_Date; No need to define date again! By LJ

    float m_t0;
    //float m_csnow;
    //float m_crain;
    float m_kblow;
    float m_swe;
    float m_lastSWE;
    float m_tsnow;
    float m_swe0;
    float m_lagSnow;
    float m_csnow6;
    float m_csnow12;

    float *m_tMean;
    float *m_tMax;
    float *m_Pnet;
    float *m_SA;
    float *m_SR;
    float *m_SE;

    //temp fro snow pack temperature
    float *m_packT;

    //result
    float *m_SM;
    /// removed by LJ
    ///bool m_isInitial;

    float CMelt();

    void initialOutputs();
};

