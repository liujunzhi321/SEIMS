/*!
 * \file SEDR_VCD.cpp
 * \brief Sediment routing using variable channel dimension(VCD) method at daily time scale
 * \author Hui Wu
 * \date Jul. 2012

 * \revised LiangJun Zhu
 * \date May / 2016
 * \description: 1. move m_erodibilityFactor, m_coverFactor, to reach collection of MongoDB as inputs, and is DT_Array1D
 *               2. add point source loadings from Scenario database
 *               3. add SEDRECHConc output with the unit g/cm3 (i.e., Mg/m3)
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

SEDR_VCD::SEDR_VCD(void) : m_dt(-1), m_nreach(-1), m_sedtoCh(NULL), m_Chs0(NODATA_VALUE), m_sedChi0(NODATA_VALUE),
                           m_ptSub(NULL), m_chStorage(NULL), m_sedOut(NULL),
                           m_reachDownStream(NULL), m_chOrder(NULL), m_chWidth(NULL), 
						   m_chLen(NULL), m_chDepth(NULL), m_chVel(NULL), m_chCover(NULL), m_chErod(NULL), m_qchOut(NULL),
                           m_prf(NODATA_VALUE), m_spcon(NODATA_VALUE), m_spexp(NODATA_VALUE), m_vcrit(NODATA_VALUE), 
                           //m_coverFactor(0.1f), m_erodibilityFactor(0.2f), 
						   m_sedStorage(NULL), m_sedDep(NULL), m_sedDeg(NULL), m_sedConc(NULL)
{
}

SEDR_VCD::~SEDR_VCD(void)
{
	if (m_reachDownStream != NULL) Release1DArray(m_reachDownStream);
	if (m_chOrder != NULL) Release1DArray(m_chOrder);
	if (m_chWidth != NULL) Release1DArray(m_chWidth);
	if (m_chLen != NULL) Release1DArray(m_chLen);
	if (m_chDepth != NULL) Release1DArray(m_chDepth);
	if (m_chVel != NULL) Release1DArray(m_chVel);
	if (m_chCover != NULL) Release1DArray(m_chCover);
	if (m_chErod != NULL) Release1DArray(m_chErod);
	if (m_ptSub != NULL) Release1DArray(m_ptSub);
    if (m_sedOut != NULL) Release1DArray(m_sedOut);
	if (m_sedConc != NULL) Release1DArray(m_sedConc);
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
        }
    }

    //initial channel storage
    if (m_sedOut == NULL)
    {       
		Initialize1DArray(m_nreach+1, m_sedOut, 0.f);
		Initialize1DArray(m_nreach+1, m_sedConc, 0.f);
		Initialize1DArray(m_nreach+1, m_sedStorage, 0.f);
		Initialize1DArray(m_nreach+1, m_sedDep, 0.f);
		Initialize1DArray(m_nreach+1, m_sedDeg, 0.f);

        for (int i = 1; i <= m_nreach; i++){
            //m_sedStorage[i] = m_Chs0 * m_chLen[i]; // m_Chs0 is initial channel storage per meter, not sediment! By LJ
			m_sedStorage[i] = m_sedChi0 * m_Chs0 * m_chLen[i] * 1000.f; /// ton/m3 * m3/m * m * 1000 = kg
		}
    }
	/// initialize point source loadings
	if (m_ptSub == NULL)
		Initialize1DArray(m_nreach+1,m_ptSub,0.f);
}

void SEDR_VCD::PointSourceLoading()
{
	/// load point source water discharge (m3/s) on current day from Scenario
	for (map<int, BMPPointSrcFactory*>::iterator it = m_ptSrcFactory.begin(); it != m_ptSrcFactory.end(); it++)
	{
		//cout<<"unique Point Source Factory ID: "<<it->first<<endl;
		vector<int> m_ptSrcMgtSeqs = it->second->GetPointSrcMgtSeqs();
		map<int, PointSourceMgtParams *>  m_pointSrcMgtMap = it->second->GetPointSrcMgtMap();
		vector<int> m_ptSrcIDs = it->second->GetPointSrcIDs();
		map<int, PointSourceLocations *> m_pointSrcLocsMap = it->second->GetPointSrcLocsMap();
		// 1. looking for management operations from m_pointSrcMgtMap
		for (vector<int>::iterator seqIter = m_ptSrcMgtSeqs.begin(); seqIter != m_ptSrcMgtSeqs.end(); seqIter++)
		{
			PointSourceMgtParams* curPtMgt = m_pointSrcMgtMap.at(*seqIter);
			// 1.1 If current day is beyond the date range, then continue to next management
			if(curPtMgt->GetStartDate() != 0 && curPtMgt->GetEndDate() != 0)
			{
				if (m_date < curPtMgt->GetStartDate() || m_date > curPtMgt->GetEndDate())
					continue;
			}
			// 1.2 Otherwise, get the water volume
			float per_sed = curPtMgt->GetSedment(); /// kg/'size'/day 
			// 1.3 Sum up all point sources
			for (vector<int>::iterator locIter = m_ptSrcIDs.begin(); locIter != m_ptSrcIDs.end(); locIter++)
			{
				if (m_pointSrcLocsMap.find(*locIter) != m_pointSrcLocsMap.end()){
					PointSourceLocations* curPtLoc = m_pointSrcLocsMap.at(*locIter);
					int curSubID = curPtLoc->GetSubbasinID();
					m_ptSub[curSubID] += per_sed * curPtLoc->GetSize(); /// kg
				}
			}
		}
	}
}

int SEDR_VCD::Execute()
{
    //check the data
    CheckInputData();
    initialOutputs();
	/// load point source water volume from m_ptSrcFactory
	PointSourceLoading();
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
    if (StringMatch(sk, VAR_SED_OUTLET))*value = m_sedOut[iOutlet];
	else
		throw ModelException(MID_SEDR_VCD, "GetValue", "Parameter " + sk + " does not exist.");
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
	else if (StringMatch(sk, VAR_SED_CHI0)) m_sedChi0 = value;
    else
        throw ModelException(MID_SEDR_VCD, "SetSingleData", "Parameter " + sk + " does not exist.");
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
    else
        throw ModelException(MID_SEDR_VCD, "Set1DData", "Parameter " + sk + " does not exist");
}

void SEDR_VCD::Get1DData(const char *key, int *n, float **data)
{
    string sk(key);
    *n = m_nreach + 1;
    int iOutlet = m_reachLayers.rbegin()->second[0];
    if (StringMatch(sk, VAR_SED_RECH))
    {
        m_sedOut[0] = m_sedOut[iOutlet];    // kg
        *data = m_sedOut;
    }
	else if (StringMatch(sk, VAR_SED_RECHConc))
	{
		m_sedConc[0] = m_sedConc[iOutlet];    // kg/m3, i.e., g/L
		*data = m_sedConc;
	}
    else
        throw ModelException(MID_SEDR_VCD, "Get1DData", "Output " + sk + " does not exist.");
}

void SEDR_VCD::Get2DData(const char *key, int *nRows, int *nCols, float ***data)
{
    string sk(key);
}

void SEDR_VCD::SetScenario(Scenario *sce)
{
	if (sce != NULL){
		map <int, BMPFactory* > *tmpBMPFactories = sce->GetBMPFactories();
		for (map<int, BMPFactory *>::iterator it = tmpBMPFactories->begin(); it != tmpBMPFactories->end(); it++)
		{
			/// Key is uniqueBMPID, which is calculated by BMP_ID * 100000 + subScenario;
			if (it->first / 100000 == BMP_TYPE_POINTSOURCE)
			{
				m_ptSrcFactory[it->first] = (BMPPointSrcFactory*)it->second;
			}
		}
	}
	else
		throw ModelException(MID_SEDR_VCD, "SetScenario", "The scenario can not to be NULL.");
}

void SEDR_VCD::SetReaches(clsReaches *reaches)
{
	if(reaches != NULL)
	{
		m_nreach = reaches->GetReachNumber();
		m_reachId = reaches->GetReachIDs();
		Initialize1DArray(m_nreach+1,m_reachDownStream, 0.f);
		Initialize1DArray(m_nreach+1,m_chOrder, 0.f);
		Initialize1DArray(m_nreach+1,m_chWidth, 0.f);
		Initialize1DArray(m_nreach+1,m_chLen, 0.f);
		Initialize1DArray(m_nreach+1,m_chDepth, 0.f);
		Initialize1DArray(m_nreach+1,m_chVel, 0.f);
		Initialize1DArray(m_nreach+1,m_chSlope, 0.f);
		Initialize1DArray(m_nreach+1,m_chCover, 0.f);
		Initialize1DArray(m_nreach+1,m_chErod, 0.f);
		for (vector<int>::iterator it = m_reachId.begin(); it != m_reachId.end(); it++)
		{
			int i = *it;
			clsReach* tmpReach = reaches->GetReachByID(i);
			m_reachDownStream[i] = (float)tmpReach->GetDownStream();
			m_chOrder[i] = (float)tmpReach->GetUpDownOrder();
			m_chWidth[i] = (float)tmpReach->GetWidth();
			m_chLen[i] = (float)tmpReach->GetLength();
			m_chDepth[i] = (float)tmpReach->GetDepth();
			m_chVel[i] = (float)tmpReach->GetV0();
			m_chCover[i] = (float)tmpReach->GetCover();
			m_chErod[i] = (float)tmpReach->GetErod();
		}
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
		throw ModelException(MID_SEDR_VCD, "SetReaches", "The reaches input can not to be NULL.");
}

//void SEDR_VCD::Set2DData(const char *key, int nrows, int ncols, float **data)
//{
//    string sk(key);
//
//    if (StringMatch(sk, Tag_RchParam))
//    {
//        m_nreach = ncols - 1;
//
//        m_reachId = data[0];
//        m_reachDownStream = data[1];
//        m_chOrder = data[2];
//        m_chWidth = data[3];
//        m_chLen = data[4];
//        m_chDepth = data[5];
//        m_chVel = data[6];
//        //m_area = data[7];
//        m_chSlope = data[9];
//
//        m_reachUpStream.resize(m_nreach + 1);
//        if (m_nreach > 1)
//        {
//            for (int i = 1; i <= m_nreach; i++)// index of the reach is the equal to streamlink ID(1 to nReaches)
//            {
//                int downStreamId = int(m_reachDownStream[i]);
//                if (downStreamId <= 0)
//                    continue;
//                m_reachUpStream[downStreamId].push_back(i);
//            }
//        }
//    }
//    else
//        throw ModelException(MID_SEDR_VCD, "Set2DData", "Parameter " + sk
//                                                        + " does not exist. Please contact the module developer.");
//}

void SEDR_VCD::SedChannelRouting(int i)
{
    // 1 .whether is no water in channel
    if (m_qchOut[i] < 0.0001f)
    {
		m_sedDeg[i] = 0.f;
		m_sedDep[i] = 0.f;
        m_sedOut[i] = 0.f;
		m_sedConc[i] = 0.f;
    }
    else
    {   // 2. sediment from upstream reaches
        float sedUp = 0.f;
        for (size_t j = 0; j < m_reachUpStream[i].size(); ++j)
        {
            int upReachId = m_reachUpStream[i][j];
            sedUp += m_sedOut[upReachId];
        }
		float allSediment = sedUp + m_sedtoCh[i] + m_sedStorage[i];
		if (m_ptSub != NULL && m_ptSub[i] > 0.f)
			allSediment += m_ptSub[i];


        //get peak channel velocity (m/s)
        float peakFlowRate = m_qchOut[i] * m_prf;
        float crossarea = m_chStorage[i] / m_chLen[i];            // SWAT, eq. 7:1.2.3
        float peakVelocity = peakFlowRate / crossarea;
        if (peakVelocity > 5.f) peakVelocity = 5.f;
        //max concentration
        float maxCon = m_spcon * pow(peakVelocity, m_spexp);
        //initial concentration,mix sufficiently

        float qOutV = m_qchOut[i] * m_dt;
        float allWater = m_chStorage[i] + qOutV;

        if (allWater < 0.000001f)
        {
			m_sedDeg[i] = 0.f;
			m_sedDep[i] = 0.f;
            m_sedOut[i] = 0.f;
			m_sedConc[i] = 0.f;
            return;
        }

		//deposition and degradation
        float initCon = allSediment / allWater;
        float sedDeposition = allWater * (initCon - maxCon);
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
				sedDegradation2 = (sedDegradation - sedDegradation1) * m_chErod[i] * m_chCover[i];
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
        m_sedOut[i] = allSediment * outFraction;
        //update sed storage
        m_sedStorage[i] = allSediment - m_sedOut[i];

        //get final sediment in water, cannot large than 0.848 ton/m3
        float maxSedinWt = 0.848f * qOutV * 1000.f; /// kg
        if (m_sedOut[i] > maxSedinWt)
        {
            m_sedDep[i] += m_sedOut[i] - maxSedinWt;
            m_sedOut[i] = maxSedinWt;
        }
		/// calculate sediment concentration
		m_sedConc[i] = m_sedOut[i] / qOutV; /// kg/m3, i.e., g/L

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
