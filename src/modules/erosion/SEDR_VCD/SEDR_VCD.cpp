/*!
 * \file SEDR_VCD.cpp
 * \brief Sediment routing using variable channel dimension(VCD) method at daily time scale
 * \author Hui Wu
 * \date Jul. 2012

 * \revised LiangJun Zhu
 * \date May / 2016

 * \revised Junzhi Liu
 * \date August / 2016
 */

#include "SEDR_VCD.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>
#include <cmath>
#include <iostream>
#include <set>
#include <sstream>
#include <algorithm> 
#include <omp.h>

using namespace std;

SEDR_VCD::SEDR_VCD(void) : m_dt(-1), m_nreach(-1), m_sedtoCh(NULL), m_Chs0(NODATA_VALUE), 
                           m_Vdiv(NULL), m_Vpoint(NULL), m_chStorage(NULL), m_SedOut(NULL),
                           m_chOrder(NULL), m_qchOut(NULL), m_sideslopeMain(1.f), m_sideslopeFloodplain(1.f),
                           m_w_ratio(1.f), m_bankfullQ(5.f),
                           m_prf(NODATA_VALUE), m_spcon(NODATA_VALUE), m_spexp(NODATA_VALUE), m_vcrit(NODATA_VALUE), m_coverFactor(0.1f),
                           m_erodibilityFactor(0.2f), 
						   m_sedStorage(NULL), m_sedDep(NULL), m_sedDeg(NULL), m_sedCon(NULL)
{

}


SEDR_VCD::~SEDR_VCD(void)
{
    if (m_SedOut != NULL) Release1DArray(m_SedOut);
    if (m_sedStorage != NULL) Release1DArray(m_sedStorage);
    if (m_sedDeg != NULL) Release1DArray(m_sedDeg);
    if (m_sedDep != NULL) Release1DArray(m_sedDep);
}

bool SEDR_VCD::CheckInputData(void)
{
    if (m_dt < 0)
        throw ModelException(MID_SEDR_VCD, "CheckInputData", "The parameter: m_dt has not been set.");

    if (m_nreach < 0)
        throw ModelException(MID_SEDR_VCD, "CheckInputData", "The parameter: m_nreach has not been set.");

    if (FloatEqual(m_Chs0, NODATA_VALUE))
        throw ModelException(MID_SEDR_VCD, "CheckInputData", "The parameter: Chs0 has not been set.");

    if (FloatEqual(m_prf, NODATA_VALUE))
        throw ModelException(MID_SEDR_VCD, "CheckInputData", "The parameter: m_prf has not been set.");

    if (FloatEqual(m_spcon, NODATA_VALUE))
        throw ModelException(MID_SEDR_VCD, "CheckInputData", "The parameter: m_spcon has not been set.");
    
    if (FloatEqual(m_spexp, NODATA_VALUE))
        throw ModelException(MID_SEDR_VCD, "CheckInputData", "The parameter: m_spexp has not been set.");
    
    if (FloatEqual(m_vcrit, NODATA_VALUE))
        throw ModelException(MID_SEDR_VCD, "CheckInputData", "The parameter: m_vcrit has not been set.");
    
    if (m_sedtoCh == NULL)
        throw ModelException(MID_SEDR_VCD, "CheckInputData", "The parameter: m_sedtoCh has not been set.");
    
    if (m_chWidth == NULL)
        throw ModelException(MID_SEDR_VCD, "CheckInputData", "The parameter: ReachParameter has not been set.");
    
    if (m_chStorage == NULL)
        throw ModelException(MID_SEDR_VCD, "CheckInputData", "The parameter: m_chStorage has not been set.");
    
    if (m_chWTdepth == NULL)
        throw ModelException(MID_SEDR_VCD, "CheckInputData", "The parameter: m_chWTdepth has not been set.");
    
    if (m_qchOut == NULL)
        throw ModelException(MID_SEDR_VCD, "CheckInputData", "The parameter: m_qchOut has not been set.");
    
    return true;
}

void  SEDR_VCD::initialOutputs()
{

    if (m_nreach <= 0)
        throw ModelException(MID_SEDR_VCD, "initialOutputs", "The cell number of the input can not be less than zero.");

    if (m_reachLayers.empty())
    {
        CheckInputData();
        for (int i = 1; i <= m_nreach; i++)
        {
            int order = (int) m_chOrder[i];
            m_reachLayers[order].push_back(i);
            //m_reachLayers[order].push_back(m_reachId[i]);
        }
    }

    //initial channel storage
    if (m_SedOut == NULL)
    {       
		Initialize1DArray(m_nreach+1, m_SedOut, 0.f);
		Initialize1DArray(m_nreach+1, m_sedStorage, 0.f);
		Initialize1DArray(m_nreach+1, m_sedDep, 0.f);
		Initialize1DArray(m_nreach+1, m_sedDeg, 0.f);

//#pragma omp parallel for
        for (int i = 1; i <= m_nreach; i++)
            m_sedStorage[i] = m_Chs0 * m_chLen[i];
    }
}

int SEDR_VCD::Execute()
{
    //check the data
    CheckInputData();
    initialOutputs();

    map<int, vector<int> >::iterator it;
    for (it = m_reachLayers.begin(); it != m_reachLayers.end(); it++)
    {
        // There are not any flow relationship within each routing layer.
        // So parallelization can be done here.
        int nReaches = it->second.size();
        // the size of m_reachLayers (map) is equal to the maximum stream order
#pragma omp parallel for
        for (int i = 0; i < nReaches; ++i)
        {
            int reachIndex = it->second[i]; // index in the array

            SedChannelRouting(reachIndex);
        }
    }

    return 0;
}

bool SEDR_VCD::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
    {
        //this->StatusMsg("Input data for "+string(key) +" is invalid. The size could not be less than zero.");
        return false;
    }
    if (this->m_nreach != n)
    {
        if (this->m_nreach <= 0) this->m_nreach = n;
        else
        {
            //this->StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
            ostringstream oss;
            oss << "Input data for " + string(key) << " is invalid with size: " << n << ". The origin size is " <<
            m_nreach << ".\n";
            throw ModelException(MID_SEDR_VCD, "CheckInputSize", oss.str());
        }
    }

    return true;
}

void SEDR_VCD::GetValue(const char *key, float *value)
{
    string sk(key);
    int iOutlet = m_reachLayers.rbegin()->second[0];
    if (StringMatch(sk, VAR_SED_OUTLET))
    {
        *value = m_SedOut[iOutlet];
    }

}

void SEDR_VCD::SetValue(const char *key, float value)
{
    string sk(key);

    if (StringMatch(sk, VAR_OMP_THREADNUM))
    {
        omp_set_num_threads((int) value);
    }
#ifdef STORM_MODEL
        else if (StringMatch(sk, Tag_ChannelTimeStep))
        {
            m_dt = (int) value;
        }
#else
    else if (StringMatch(sk, Tag_TimeStep))
    {
        m_dt = (int) value;
    }
#endif
    else if (StringMatch(sk, VAR_P_RF))
    {
        m_prf = value;
    }
    else if (StringMatch(sk, VAR_SPCON))
    {
        m_spcon = value;
    }
    else if (StringMatch(sk, VAR_SPEXP))
    {
        m_spexp = value;
    }
    else if (StringMatch(sk, VAR_VCRIT))
    {
        m_vcrit = value;
    }
    else if (StringMatch(sk, VAR_CHS0))
    {
        m_Chs0 = value;
    }
    else
        throw ModelException(MID_SEDR_VCD, "SetSingleData", "Parameter " + sk
                                                            + " does not exist. Please contact the module developer.");

}

void SEDR_VCD::Set1DData(const char *key, int n, float *value)
{
    string sk(key);
    //check the input data
    if (StringMatch(sk, VAR_SED_TO_CH))
    {
        m_sedtoCh = value;   //for longterm model
    }
    else if (StringMatch(sk, VAR_SUB_SEDTOCH))
    {
        m_sedtoCh = value;   //for storm model
    }
    else if (StringMatch(sk, VAR_QRECH))
    {
        m_qchOut = value;
    }
    else if (StringMatch(sk, VAR_CHST))
    {
        m_chStorage = value;
    }
    else if (StringMatch(sk, VAR_CHWTDEPTH))
    {
        m_chWTdepth = value;
    }
        /*else if (StringMatch(sk, "CROSS_AREA"))
        {
        m_CrAreaCh = value;
        }*/
    else
        throw ModelException(MID_SEDR_VCD, "Set1DData", "Parameter " + sk
                                                        + " does not exist. Please contact the module developer.");

}

void SEDR_VCD::Get1DData(const char *key, int *n, float **data)
{
    string sk(key);
    *n = m_nreach + 1;
    int iOutlet = m_reachLayers.rbegin()->second[0];
    if (StringMatch(sk, VAR_SED_RECH))
    {
        //m_SedOut[0] = m_SedOut[iOutlet] * 1000/24/3600;    // ton/day coverts to kg/s
        m_SedOut[0] = m_SedOut[iOutlet];    // ton
        *data = m_SedOut;
    }
    else
        throw ModelException(MID_SEDR_VCD, "Get1DData", "Output " + sk
                                                        +
                                                        " does not exist in current module. Please contact the module developer.");

}

void SEDR_VCD::Get2DData(const char *key, int *nRows, int *nCols, float ***data)
{
    string sk(key);
    /*if (StringMatch(sk,"T_CHSB"))
    {
    *data = this->m_T_CHSB;
    *nRows = this->m_nreach;
    *nCols = SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_COUNT;
    }
    else
        throw ModelException(MID_SEDR_VCD, "Get2DData", "Output " + sk
        + " does not exist in the SEDR_VCD module. Please contact the module developer.");*/

}

void SEDR_VCD::Set2DData(const char *key, int nrows, int ncols, float **data)
{
    string sk(key);

    if (StringMatch(sk, Tag_RchParam))
    {
        m_nreach = ncols - 1;

        m_reachId = data[0];
        m_reachDownStream = data[1];
        m_chOrder = data[2];
        m_chWidth = data[3];
        m_chLen = data[4];
        m_chDepth = data[5];
        m_chVel = data[6];
        //m_area = data[7];
        m_chSlope = data[9];

        m_reachUpStream.resize(m_nreach + 1);
        if (m_nreach > 1)
        {
            for (int i = 1; i <= m_nreach; i++)// index of the reach is the equal to streamlink ID(1 to nReaches)
            {
                int downStreamId = int(m_reachDownStream[i]);
                if (downStreamId <= 0)
                    continue;
                m_reachUpStream[downStreamId].push_back(i);
            }
        }
    }
    else
        throw ModelException(MID_SEDR_VCD, "Set2DData", "Parameter " + sk
                                                        + " does not exist. Please contact the module developer.");
}

void SEDR_VCD::SedChannelRouting(int i)
{
    // 1 .whether is no water in channel
    if (m_qchOut[i] < 0.0001f)
    {
		m_sedDeg[i] = 0.0f;
		m_sedDep[i] = 0.0f;
        m_SedOut[i] = 0.f;
    }
    else
    {   // 2. sediment from upstream reaches
        float sedUp = 0;
        for (size_t j = 0; j < m_reachUpStream[i].size(); ++j)
        {
            int upReachId = m_reachUpStream[i][j];
            sedUp += m_SedOut[upReachId];
        }
        float allSediment = sedUp + m_sedtoCh[i] + m_sedStorage[i];

        //get peak channel velocity (m/s)
        float peakFlowRate = m_qchOut[i] * m_prf;
        float crossarea = m_chStorage[i] / m_chLen[i];            // SWAT, eq. 7:1.2.3
        float peakVelocity = peakFlowRate / crossarea;
        if (peakVelocity > 5) peakVelocity = 5.0f;
        //max concentration
        float maxCon = m_spcon * pow(peakVelocity, m_spexp);
        //initial concentration,mix sufficiently

        float qOutV = m_qchOut[i] * m_dt;
        float allWater = m_chStorage[i] + qOutV;

        if (allWater < 0.001f)
        {
			m_sedDeg[i] = 0.0f;
			m_sedDep[i] = 0.0f;
            m_SedOut[i] = 0.0f;
            return;
        }

		//deposition and degradation
        float initCon = allSediment / allWater;
        float sedDeposition = allWater * (initCon - maxCon);
        //if (abs(sedDeposition) < 1.0e-6f)
        //    sedDeposition = 0.0f;
        if (peakVelocity < m_vcrit)
            sedDeposition = 0.0f;

        float sedDegradation = 0.0f;
        float sedDegradation1 = 0.0f;
        float sedDegradation2 = 0.0f;
		//get tbase
		float tbase = m_chLen[i] / (m_dt * peakVelocity);
		if (tbase > 1) tbase = 1.0f;
        if (sedDeposition < 0.f)    //degradation
        {
            sedDegradation = -sedDeposition * tbase;
			// first the deposited material will be degraded before channel bed
            if (sedDegradation >= m_sedDep[i])
            {
                sedDegradation1 = m_sedDep[i];
                sedDegradation2 = (sedDegradation - sedDegradation1) * m_erodibilityFactor * m_coverFactor;
            }
            else
            {
                sedDegradation1 = sedDegradation;
            }

            sedDeposition = 0.0f;
        }

        //update sed deposition
        m_sedDep[i] += sedDeposition - sedDegradation1;
        m_sedDeg[i] += sedDegradation1 + sedDegradation2;

        //get sediment after deposition and degradation
        allSediment += sedDegradation1 + sedDegradation2 - sedDeposition;

        //get out flow water fraction
        float outFraction = qOutV / allWater;
        m_SedOut[i] = allSediment * outFraction;

        //update sed storage
        m_sedStorage[i] = allSediment - m_SedOut[i];

        //get final sediment in water, cannot large than 0.848t/m3
        float maxSedinWt = 0.848f * qOutV;
        if (m_SedOut[i] > maxSedinWt)
        {
            m_sedDep[i] += m_SedOut[i] - maxSedinWt;
            m_SedOut[i] = maxSedinWt;
        }

        ////channel downcutting and widening
        //bool dodowncutting = false;
        //if(dodowncutting)
        //{
        //	doChannelDowncuttingAndWidening(i);
        //}
        //}

    }

}

void SEDR_VCD::doChannelDowncuttingAndWidening(int id)
{
    //float additionalDepth = m_depthcurrent[id] - m_chDepth[m_idToIndex[id]];
    //if (additionalDepth < m_chSlope[m_idToIndex[id]] * m_chLen[m_idToIndex[id]])
    //{
    //    float storage = m_chStorage[id];
    //    float vout = m_qchOut[id] * m_dt;
    //    if (storage + vout > 1.4e6f)
    //    {
    //        float waterDepth = m_chWTdepth[id];
    //        float addDepth = 358.6f * waterDepth *
    //                         m_slopecurrent[id] * m_erodibilityFactor;
    //        m_depthcurrent[id] += addDepth;

    //        m_widthcurrent[id] = m_depthcurrent[id] *
    //                             (m_chWidth[m_idToIndex[id]] / m_chDepth[m_idToIndex[id]]);

    //        m_slopecurrent[id] -= m_depthcurrent[id] / m_chLen[m_idToIndex[id]];
    //        m_slopecurrent[id] = max(0.0001f, m_slopecurrent[id]);
    //    }
    //}
}
