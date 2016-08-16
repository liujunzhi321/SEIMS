/*//
 * \file NutrCH_QUAL2E.cpp
 * \ingroup NutCHRout
 * \author Huiran Gao
 * \date Jun 2016
 */

#include <iostream>
#include "NutrCH_QUAL2E.h"
#include "MetadataInfo.h"
#include <cmath>
#include <fstream>
#include "ModelException.h"
#include "util.h"
#include <omp.h>

using namespace std;

#define CHECK_POINTER(moduleName, varName, descStr) if (varName == NULL) throw ModelException(moduleName, "CheckInputData", string("parameter has not been set: ") + string(descStr));
#define CHECK_POSITIVE(moduleName, varName, descStr) if (varName < 0) 	throw ModelException(moduleName, "CheckInputData", string("parameter has not been set: ") + string(descStr));


NutrCH_QUAL2E::NutrCH_QUAL2E(void) :
//input
        m_nCells(-1), m_dt(-1), m_qUpReach(0.f), m_rnum1(0.f), igropt(-1),
        m_ai0(-1.f), m_ai1(-1.f), m_ai2(-1.f), m_ai3(-1.f), m_ai4(-1.f), m_ai5(-1.f), 
		m_ai6(-1.f), m_lambda0(-1.f), m_lambda1(-1.f), m_lambda2(-1.f),
        m_k_l(-1.f), m_k_n(-1.f), m_k_p(-1.f), m_p_n(-1.f), tfact(-1.f), m_mumax(-1.f), m_rhoq(-1.f), m_streamLink(NULL),
        m_daylen(NULL), m_sra(NULL), m_bankStorage(NULL), m_qOutCh(NULL), m_chStorage(NULL), m_chWTdepth(NULL), m_chTemp(NULL),
        m_latNO3ToCh(NULL), m_surNO3ToCh(NULL), m_surSolPToCh(NULL), m_gwNO3ToCh(NULL),
        m_gwSolPToCh(NULL), m_sedOrgNToCh(NULL), m_sedOrgPToCh(NULL), m_sedMinPAToCh(NULL),
        m_sedMinPSToCh(NULL), m_nh4ToCh(NULL), m_no2ToCh(NULL), m_codToCh(NULL), 
		m_chSr(NULL), m_chDaylen(NULL), m_chCellCount(NULL), m_soilTemp(NULL),
        //output
        m_chAlgae(NULL), m_chOrgN(NULL), m_chOrgP(NULL), m_chNH4(NULL), m_chNO2(NULL), m_chNO3(NULL),
        m_chSolP(NULL), m_chCOD(NULL), m_chDO(NULL), m_chChlora(NULL), m_chSatDO(NULL),
		m_chOutAlgae(NULL), m_chOutOrgN(NULL), m_chOutOrgP(NULL), m_chOutNH4(NULL), m_chOutNO2(NULL),
		m_chOutNO3(NULL), m_chOutSolP(NULL), m_chOutCOD(NULL), m_chOutDO(NULL)
{
}

NutrCH_QUAL2E::~NutrCH_QUAL2E(void)
{
	/// reach parameters
	if (m_reachDownStream != NULL) Release1DArray(m_reachDownStream);
	if (m_chOrder != NULL) Release1DArray(m_chOrder);
	if (m_bc1 != NULL) Release1DArray(m_bc1);
	if (m_bc2 != NULL) Release1DArray(m_bc2);
	if (m_bc3 != NULL) Release1DArray(m_bc3);
	if (m_bc4 != NULL) Release1DArray(m_bc4);
	if (m_rk1 != NULL) Release1DArray(m_rk1);
	if (m_rk2 != NULL) Release1DArray(m_rk2);
	if (m_rk3 != NULL) Release1DArray(m_rk3);
	if (m_rk4 != NULL) Release1DArray(m_rk4);
	if (m_rs1 != NULL) Release1DArray(m_rs1);
	if (m_rs2 != NULL) Release1DArray(m_rs2);
	if (m_rs3 != NULL) Release1DArray(m_rs3);
	if (m_rs4 != NULL) Release1DArray(m_rs4);
	if (m_rs5 != NULL) Release1DArray(m_rs5);

    if (m_chAlgae != NULL) Release1DArray(m_chAlgae);
    if (m_chOrgN != NULL) Release1DArray(m_chOrgN);
    if (m_chOrgP != NULL) Release1DArray(m_chOrgP);
    if (m_chNH4 != NULL) Release1DArray(m_chNH4);
    if (m_chNO2 != NULL) Release1DArray(m_chNO2);
    if (m_chNO3 != NULL) Release1DArray(m_chNO3);
    if (m_chSolP != NULL) Release1DArray(m_chSolP);
    if (m_chCOD != NULL) Release1DArray(m_chCOD);
    if (m_chDO != NULL) Release1DArray(m_chDO);
    if (m_chChlora != NULL) Release1DArray(m_chChlora);


	if(m_chOutAlgae != NULL) Release1DArray(m_chOutAlgae);
	if(m_chOutOrgN != NULL) Release1DArray(m_chOutOrgN);
	if(m_chOutOrgP != NULL) Release1DArray(m_chOutOrgP);
	if(m_chOutNH4 != NULL) Release1DArray(m_chOutNH4);
	if(m_chOutNO2 != NULL) Release1DArray(m_chOutNO2);
	if(m_chOutNO3 != NULL) Release1DArray(m_chOutNO3);
	if(m_chOutSolP != NULL) Release1DArray(m_chOutSolP);
	if(m_chOutDO != NULL) Release1DArray(m_chOutDO);
	if(m_chOutCOD != NULL) Release1DArray(m_chOutCOD);

    if(m_chCellCount != NULL) Release1DArray(m_chCellCount);
}

void NutrCH_QUAL2E::rasterToSubbasin()
{
	for (int i = 1; i <= m_nReaches; i++)
	{
		m_chDaylen[i] = 0.f;
		m_chSr[i] = 0.f;
		m_chTemp[i] = 0.f;
	}

	for (int i = 0; i < m_nCells; i++)
	{
		if (m_streamLink[i] <= 0.f)
			continue;

		//add today's flow
		int subi = (int) m_streamLink[i];
		if (m_nReaches == 1)
		{
			subi = 1;
		}
		else if (subi >= m_nReaches + 1)
		{
			ostringstream oss;
			oss << subi;
			throw ModelException(MID_NutCHRout, "Execute", "The subbasin " + oss.str() + " is invalid.");
		}


		m_chDaylen[subi] += m_daylen[i];
		m_chSr[subi] += m_sra[i];
		m_chTemp[subi] += m_soilTemp[i];
		m_chCellCount[subi] += 1;
	}

	for (int i = 1; i <= m_nReaches; i++)
	{
		m_chDaylen[i] /= m_chCellCount[i];
		m_chSr[i] /= m_chCellCount[i];
		m_chTemp[i] /= m_chCellCount[i];
	}

}

bool NutrCH_QUAL2E::CheckInputCellSize(const char *key, int n)
{
	if (n <= 0)
	{
		throw ModelException(MID_NutCHRout, "CheckInputSize",
			"Input data for " + string(key) + " is invalid. The size could not be less than zero.");
		return false;
	}
	if (m_nCells != n)
	{
		if (m_nCells <= 0)
		{
			m_nCells = n;
		}
		else
		{
			//StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
			ostringstream oss;
			oss << "Input data for " + string(key) << " is invalid with size: " << n << ". The origin size is " <<
				m_nReaches << ".\n";
			throw ModelException(MID_NutCHRout, "CheckInputCellSize", oss.str());
		}
	}
	return true;
}


bool NutrCH_QUAL2E::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
    {
        throw ModelException(MID_NutCHRout, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
        return false;
    }
    if (m_nReaches != n - 1)
    {
        if (m_nReaches <= 0)
        {
            m_nReaches = n - 1;
        } else
        {
            //StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
            ostringstream oss;
            oss << "Input data for " + string(key) << " is invalid with size: " << n << ". The origin size is " <<
            m_nReaches << ".\n";
            throw ModelException(MID_NutCHRout, "CheckInputSize", oss.str());
        }
    }
    return true;
}

bool NutrCH_QUAL2E::CheckInputData()
{
	CHECK_POSITIVE(MID_NutCHRout, m_dt, "m_dt")
	CHECK_POSITIVE(MID_NutCHRout, m_nReaches, "m_nReaches")
	CHECK_POSITIVE(MID_NutCHRout, m_qUpReach, "m_qUpReach")
	CHECK_POSITIVE(MID_NutCHRout, m_rnum1, "m_rnum1")
	CHECK_POSITIVE(MID_NutCHRout, igropt, "igropt")
	CHECK_POSITIVE(MID_NutCHRout, m_ai0, "m_ai0")
	CHECK_POSITIVE(MID_NutCHRout, m_ai1, "m_ai1")
	CHECK_POSITIVE(MID_NutCHRout, m_ai2, "m_ai2")
	CHECK_POSITIVE(MID_NutCHRout, m_ai3, "m_ai3")
	CHECK_POSITIVE(MID_NutCHRout, m_ai4, "m_ai4")
	CHECK_POSITIVE(MID_NutCHRout, m_ai5, "m_ai5")
	CHECK_POSITIVE(MID_NutCHRout, m_ai6, "m_ai6")
	CHECK_POSITIVE(MID_NutCHRout, m_lambda0, "m_lambda0")
	CHECK_POSITIVE(MID_NutCHRout, m_lambda1, "m_lambda1")
	CHECK_POSITIVE(MID_NutCHRout, m_lambda2, "m_lambda2")
	CHECK_POSITIVE(MID_NutCHRout, m_k_l, "m_k_l")
	CHECK_POSITIVE(MID_NutCHRout, m_k_n, "m_k_n")
	CHECK_POSITIVE(MID_NutCHRout, m_k_p, "m_k_p")
	CHECK_POSITIVE(MID_NutCHRout, m_p_n, "m_p_n")
	CHECK_POSITIVE(MID_NutCHRout, tfact, "tfact")
	CHECK_POSITIVE(MID_NutCHRout, m_mumax, "m_mumax")
	//CHECK_POSITIVE(MID_NutCHRout, m_rhoqv, "m_rhoqv")

    CHECK_POINTER(MID_NutCHRout, m_daylen, "m_daylen")
    CHECK_POINTER(MID_NutCHRout, m_sra, "m_sra")
    CHECK_POINTER(MID_NutCHRout, m_qOutCh, "m_qOutCh")
    CHECK_POINTER(MID_NutCHRout, m_chStorage, "m_chStorage")
    CHECK_POINTER(MID_NutCHRout, m_chWTdepth, "m_chWTdepth")
    CHECK_POINTER(MID_NutCHRout, m_latNO3ToCh, "m_latNO3ToCh")
    CHECK_POINTER(MID_NutCHRout, m_surNO3ToCh, "m_surNO3ToCh")
    CHECK_POINTER(MID_NutCHRout, m_surSolPToCh, "m_surSolPToCh")
    CHECK_POINTER(MID_NutCHRout, m_gwNO3ToCh, "m_gwNO3ToCh")
	CHECK_POINTER(MID_NutCHRout, m_gwSolPToCh, "m_gwSolPToCh")
	CHECK_POINTER(MID_NutCHRout, m_sedOrgNToCh, "m_sedOrgNToCh")
	CHECK_POINTER(MID_NutCHRout, m_sedOrgPToCh, "m_sedOrgPToCh")
	CHECK_POINTER(MID_NutCHRout, m_sedMinPAToCh, "m_sedMinPAToCh")
	CHECK_POINTER(MID_NutCHRout, m_sedMinPSToCh, "m_sedMinPSToCh")
	CHECK_POINTER(MID_NutCHRout, m_streamLink, "m_streamLink")
	CHECK_POINTER(MID_NutCHRout, m_soilTemp, "m_soilTemp")
    
    return true;
}

void NutrCH_QUAL2E::SetValue(const char *key, float value)
{
    string sk(key);
    if (StringMatch(sk, VAR_OMP_THREADNUM))
    {
        omp_set_num_threads((int) value);
    }
	else if (StringMatch(sk, Tag_ChannelTimeStep)) { m_dt = (int) value; }
    else if (StringMatch(sk, VAR_QUPREACH)) { m_qUpReach = value; }
    else if (StringMatch(sk, VAR_RNUM1)) { m_rnum1 = value; }
    else if (StringMatch(sk, VAR_IGROPT)) { igropt = (int) value; }
    else if (StringMatch(sk, VAR_AI0)) { m_ai0 = value; }
    else if (StringMatch(sk, VAR_AI1)) { m_ai1 = value; }
    else if (StringMatch(sk, VAR_AI2)) { m_ai2 = value; }
    else if (StringMatch(sk, VAR_AI3)) { m_ai3 = value; }
    else if (StringMatch(sk, VAR_AI4)) { m_ai4 = value; }
    else if (StringMatch(sk, VAR_AI5)) { m_ai5 = value; }
    else if (StringMatch(sk, VAR_AI6)) { m_ai6 = value; }
    else if (StringMatch(sk, VAR_LAMBDA0)) { m_lambda0 = value; }
    else if (StringMatch(sk, VAR_LAMBDA1)) { m_lambda1 = value; }
    else if (StringMatch(sk, VAR_LAMBDA2)) { m_lambda2 = value; }
    else if (StringMatch(sk, VAR_K_L)) { m_k_l = value; }
    else if (StringMatch(sk, VAR_K_N)) { m_k_n = value; }
    else if (StringMatch(sk, VAR_K_P)) { m_k_p = value; }
    else if (StringMatch(sk, VAR_P_N)) { m_p_n = value; }
    else if (StringMatch(sk, VAR_TFACT)) { tfact = value; }
    else if (StringMatch(sk, VAR_MUMAX)) { m_mumax = value; }
    else if (StringMatch(sk, VAR_RHOQ)) { m_rhoq = value; }
        //else if(StringMatch(sk, VAR_VSF)) {m_vScalingFactor = value;}
        //else if (StringMatch(sk, VAR_MSK_X)) {m_x = value;}
        //else if (StringMatch(sk, VAR_MSK_CO1)) {m_co1 = value;}
    else
    {
        throw ModelException(MID_NutCHRout, "SetValue", "Parameter " + sk +" does not exist.");
    }
}

void NutrCH_QUAL2E::Set1DData(const char *key, int n, float *data)
{
    string sk(key);
    if (StringMatch(sk, VAR_DAYLEN))   
	{ 
		if (!CheckInputCellSize(key, n))
			return;
		m_daylen = data; 
		return;
	}
    else if (StringMatch(sk, DataType_SolarRadiation)) 
	{
		if (!CheckInputCellSize(key, n))
			return;
		m_sra = data; 
		return;
	}
	else if (StringMatch(sk, VAR_STREAM_LINK)) 
	{
		if (!CheckInputCellSize(key, n))
			return;
		m_streamLink = data; 
		return;
	}
	else if (StringMatch(sk, VAR_SOTE)) 
	{
		if (!CheckInputCellSize(key, n))
			return;
		m_soilTemp = data; 
		return;
	}

	CheckInputSize(key, n);
    if (StringMatch(sk, VAR_BKST)) { m_bankStorage = data; }
    else if (StringMatch(sk, VAR_QRECH)) { m_qOutCh = data; }
    else if (StringMatch(sk, VAR_CHST)) { m_chStorage = data; }
    else if (StringMatch(sk, VAR_CHWTDEPTH)) { m_chWTdepth = data; }
    else if (StringMatch(sk, VAR_WATTEMP)) { m_chTemp = data; }

    else if (StringMatch(sk, VAR_LATNO3_TOCH))   { m_latNO3ToCh = data; }
    else if (StringMatch(sk, VAR_SUR_NO3_TOCH))  { m_surNO3ToCh = data; }
    else if (StringMatch(sk, VAR_SUR_SOLP_TOCH)) { m_surSolPToCh = data; }
    else if (StringMatch(sk, VAR_NO3GW_TOCH))    { m_gwNO3ToCh = data; }
    else if (StringMatch(sk, VAR_MINPGW_TOCH))   { m_gwSolPToCh = data; }
    else if (StringMatch(sk, VAR_SEDORGN_TOCH))  { m_sedOrgNToCh = data; }
    else if (StringMatch(sk, VAR_SEDORGP_TOCH))  { m_sedOrgPToCh = data; }
    else if (StringMatch(sk, VAR_SEDMINPA_TOCH)) { m_sedMinPAToCh = data; }
    else if (StringMatch(sk, VAR_SEDMINPS_TOCH)) { m_sedMinPSToCh = data; }
    else if (StringMatch(sk, VAR_SUR_NH4_TOCH))  { m_nh4ToCh = data; }
    else if (StringMatch(sk, VAR_NO2_TOCH))      { m_no2ToCh = data; }
    else
    {
        throw ModelException(MID_NutCHRout, "Set1DData", "Parameter " + sk + " does not exist.");
    }
}

void NutrCH_QUAL2E::SetReaches(clsReaches *reaches)
{
	if(reaches != NULL)
	{
		m_nReaches = reaches->GetReachNumber();
		m_reachId = reaches->GetReachIDs();
		Initialize1DArray(m_nReaches+1,m_reachDownStream, 0.f);
		Initialize1DArray(m_nReaches+1,m_chOrder, 0.f);
		/// initialize reach related parameters
		Initialize1DArray(m_nReaches+1,m_bc1, 0.f);
		Initialize1DArray(m_nReaches+1,m_bc2, 0.f);
		Initialize1DArray(m_nReaches+1,m_bc3, 0.f);
		Initialize1DArray(m_nReaches+1,m_bc4, 0.f);
		Initialize1DArray(m_nReaches+1,m_rk1, 0.f);
		Initialize1DArray(m_nReaches+1,m_rk2, 0.f);
		Initialize1DArray(m_nReaches+1,m_rk3, 0.f);
		Initialize1DArray(m_nReaches+1,m_rk4, 0.f);
		Initialize1DArray(m_nReaches+1,m_rs1, 0.f);
		Initialize1DArray(m_nReaches+1,m_rs2, 0.f);
		Initialize1DArray(m_nReaches+1,m_rs3, 0.f);
		Initialize1DArray(m_nReaches+1,m_rs4, 0.f);
		Initialize1DArray(m_nReaches+1,m_rs5, 0.f);

		Initialize1DArray(m_nReaches+1, m_chDaylen, 0.f);
		Initialize1DArray(m_nReaches+1, m_chSr, 0.f);
		Initialize1DArray(m_nReaches+1, m_chTemp, 0.f);

		for (vector<int>::iterator it = m_reachId.begin(); it != m_reachId.end(); it++)
		{
			int i = *it;
			clsReach* tmpReach = reaches->GetReachByID(i);
			m_reachDownStream[i] = (float)tmpReach->GetDownStream();
			m_chOrder[i] = (float)tmpReach->GetUpDownOrder();
			m_bc1[*it] = tmpReach->GetBc1();
			m_bc2[*it] = tmpReach->GetBc2();
			m_bc3[*it] = tmpReach->GetBc3();
			m_bc4[*it] = tmpReach->GetBc4();
			m_rk1[*it] = tmpReach->GetRk1();
			m_rk2[*it] = tmpReach->GetRk2();
			m_rk3[*it] = tmpReach->GetRk3();
			m_rk4[*it] = tmpReach->GetRk4();
			m_rs1[*it] = tmpReach->GetRs1();
			m_rs2[*it] = tmpReach->GetRs2();
			m_rs3[*it] = tmpReach->GetRs3();
			m_rs4[*it] = tmpReach->GetRs4();
			m_rs5[*it] = tmpReach->GetRs5();
		}

		m_reachUpStream.resize(m_nReaches + 1);
		if (m_nReaches > 1)
		{
			for (int i = 1; i <= m_nReaches; i++)// index of the reach is the equal to streamlink ID(1 to nReaches)
			{
				int downStreamId = int(m_reachDownStream[i]);
				if (downStreamId <= 0)
					continue;
				m_reachUpStream[downStreamId].push_back(i);
			}
		}
	}
	else
		throw ModelException("NutCHPout", "SetReaches", "The reaches input can not to be NULL.");
}

void  NutrCH_QUAL2E::initialOutputs()
{
    if (m_nReaches <= 0)
        throw ModelException(MID_NutCHRout, "initialOutputs", "The cell number of the input can not be less than zero.");

    if (m_reachLayers.empty())
    {
        CheckInputData();
        for (int i = 1; i <= m_nReaches; i++)
		{
			if (m_chOrder == NULL)
				throw ModelException(MID_NutCHRout, "initialOutputs", "Stream order is not loaded successful from Reach table.");
            int order = (int) m_chOrder[i];
            m_reachLayers[order].push_back(i);
        }
    }
    if (m_chOrgN == NULL)
    {
		Initialize1DArray(m_nReaches+1,m_chAlgae,0.f);
		Initialize1DArray(m_nReaches+1,m_chOrgN,0.f);
		Initialize1DArray(m_nReaches+1,m_chOrgP,0.f);
		Initialize1DArray(m_nReaches+1,m_chNH4,0.f);
		Initialize1DArray(m_nReaches+1,m_chNO2,0.f);
		Initialize1DArray(m_nReaches+1,m_chNO3,0.f);
		Initialize1DArray(m_nReaches+1,m_chSolP,0.f);
		Initialize1DArray(m_nReaches+1,m_chCOD,0.f);
		Initialize1DArray(m_nReaches+1,m_chDO,0.f);
		Initialize1DArray(m_nReaches+1,m_chChlora,0.f);
        m_chSatDO = 0.f;

		Initialize1DArray(m_nReaches+1, m_chOutAlgae, 0.f);
		Initialize1DArray(m_nReaches+1, m_chOutOrgN, 0.f);
		Initialize1DArray(m_nReaches+1, m_chOutOrgP, 0.f);
		Initialize1DArray(m_nReaches+1, m_chOutNH4, 0.f);
		Initialize1DArray(m_nReaches+1, m_chOutNO2, 0.f);
		Initialize1DArray(m_nReaches+1, m_chOutNO3, 0.f);
		Initialize1DArray(m_nReaches+1, m_chOutSolP, 0.f);
		Initialize1DArray(m_nReaches+1, m_chOutDO, 0.f);
		Initialize1DArray(m_nReaches+1, m_chOutCOD, 0.f);

		m_chCellCount = new int(m_nReaches+1);
		for (int i = 0; i <= m_nReaches; i++)
		    m_chCellCount[i] = 0;
    }
}

int NutrCH_QUAL2E::Execute()
{
    if (!CheckInputData())return false;
    initialOutputs();

	//sum daylen and sr to channel scale
	rasterToSubbasin();

    map<int, vector<int> > ::iterator it;
    for (it = m_reachLayers.begin(); it != m_reachLayers.end(); it++)
    {
        // There are not any flow relationship within each routing layer.
        // So parallelization can be done here.
		int nReaches = it->second.size();
		// the size of m_reachLayers (map) is equal to the maximum stream order
//#pragma omp parallel for
		for (int i = 0; i < nReaches; ++i)
		{
            // index in the array
            int reachIndex = it->second[i];
            AddInputNutrient(reachIndex);
			NutrientTransform(reachIndex);
			RouteOut(reachIndex);
        }
    }
    return 0;
}

void NutrCH_QUAL2E::AddInputNutrient(int i)
{
	for (size_t j = 0; j < m_reachUpStream[i].size(); ++j)
	{
		int upReachId = m_reachUpStream[i][j];

		m_chOrgN[i]   += m_chOutOrgN[upReachId];
        m_chOrgP[i]   += m_chOutOrgP[upReachId];
		m_chNO3[i]    += m_chOutNO3[upReachId];
        m_chSolP[i]   += m_chOutSolP[upReachId];

		m_chNO2[i]    += m_chOutNO2[upReachId];
		m_chNH4[i]    += m_chOutNH4[upReachId];
        m_chCOD[i]    += m_chOutCOD[upReachId];

		m_chDO[i]     += m_chOutDO[upReachId];
		m_chChlora[i] += m_chOutCOD[upReachId];
	}

	m_chOrgN[i] += m_sedOrgNToCh[i];
	m_chOrgP[i] += m_sedOrgPToCh[i];
	m_chNO3[i]  += m_surNO3ToCh[i] + m_latNO3ToCh[i] + m_gwNO3ToCh[i];
	m_chSolP[i] += m_surSolPToCh[i] + m_gwSolPToCh[i];

	if(m_nh4ToCh != NULL) m_chNH4[i] += m_nh4ToCh[i];
	if(m_no2ToCh != NULL) m_chNO2[i] += m_no2ToCh[i];
	if(m_codToCh != NULL) m_chCOD[i] += m_codToCh[i];
}

void NutrCH_QUAL2E::RouteOut(int i)
{
	//get out flow water fraction
	float wtrOut = m_qOutCh[i]*m_dt; 
	float wtrTotal = wtrOut + m_chStorage[i];
	float outFraction = wtrOut / wtrTotal;

	m_chOutOrgN[i] = m_chOrgN[i] * outFraction;
	m_chOutOrgP[i] = m_chOrgP[i] * outFraction;
	m_chOutNO3[i]  = m_chNO3[i] * outFraction;
	m_chOutSolP[i] = m_chSolP[i] * outFraction;
	m_chOutNO2[i]  = m_chNO2[i] * outFraction;
	m_chOutNH4[i]  = m_chNH4[i] * outFraction;
	m_chOutCOD[i]  = m_chCOD[i] * outFraction;
	m_chOutDO[i]   = m_chDO[i] * outFraction;
	m_chOutCOD[i]  = m_chCOD[i] * outFraction;

	m_chOrgN[i] -= m_chOutOrgN[i];
	m_chOrgP[i] -= m_chOutOrgP[i];
	m_chNO3[i]  -= m_chOutNO3[i];
	m_chSolP[i] -= m_chOutSolP[i];
	m_chNO2[i]  -= m_chOutNO2[i];
	m_chNH4[i]  -= m_chOutNH4[i];
	m_chCOD[i]  -= m_chOutCOD[i];
	m_chDO[i]   -= m_chOutDO[i];
	m_chCOD[i]  -= m_chOutCOD[i];
}

void NutrCH_QUAL2E::NutrientTransform(int i)
{
    float thbc1 = 1.083f;    ///temperature adjustment factor for local biological oxidation of NH3 to NO2
    float thbc2 = 1.047f;    ///temperature adjustment factor for local biological oxidation of NO2 to NO3
    float thbc3 = 1.04f;    ///temperature adjustment factor for local hydrolysis of organic N to ammonia N
    float thbc4 = 1.047f;    ///temperature adjustment factor for local decay of organic P to dissolved P

    float thgra = 1.047f;    ///temperature adjustment factor for local algal growth rate
    float thrho = 1.047f;    ///temperature adjustment factor for local algal respiration rate

    float thm_rk1 = 1.047f;    ///temperature adjustment factor for local CBOD deoxygenation
    float thm_rk2 = 1.024f;    ///temperature adjustment factor for local oxygen reaeration rate
    float thm_rk3 = 1.024f;    ///temperature adjustment factor for loss of CBOD due to settling
    float thm_rk4 = 1.060f;    ///temperature adjustment factor for local sediment oxygen demand

    float thrs1 = 1.024f;    ///temperature adjustment factor for local algal settling rate
    float thrs2 = 1.074f;    ///temperature adjustment factor for local benthos source rate for dissolved phosphorus
    float thrs3 = 1.074f;    ///temperature adjustment factor for local benthos source rate for ammonia nitrogen
    float thrs4 = 1.024f;    ///temperature adjustment factor for local organic N settling rate
    float thrs5 = 1.024f;    ///temperature adjustment factor for local organic P settling rate

	// assume the water volume used to contain nutrients at current time step equals to :
	// flowout plus the storage at the end of day (did not consider the nutrients from stream to groundwater through seepage and bankstorage)
	float wtrOut = m_qOutCh[i]*m_dt; 
	float wtrTotal = wtrOut + m_chStorage[i];

	// initial algal biomass concentration in reach (algcon mg/L, i.e. g/m3)
	float algcon = m_chAlgae[i]*1000.f / wtrTotal;
	// initial organic N concentration in reach (orgncon mg/L)
	float orgncon = m_chOrgN[i]*1000.f / wtrTotal;
	// initial ammonia concentration in reach (nh3con mg/L)
	float nh3con = m_chNH4[i]*1000.f / wtrTotal;
	// initial nitrite concentration in reach (no2con mg/L)
	float no2con = m_chNO2[i]*1000.f / wtrTotal;
	// initial nitrate concentration in reach (no3con mg/L)
	float no3con = m_chNO3[i]*1000.f / wtrTotal;
	// initial organic P concentration in reach (orgpcon  mg/L)
	float orgpcon = m_chOrgP[i]*1000.f / wtrTotal;
	// initial soluble P concentration in reach (solpcon mg/L)
	float solpcon = m_chSolP[i]*1000.f / wtrTotal;
	// initial carbonaceous biological oxygen demand (cbodcon mg/L)
	float cbodcon = m_chCOD[i]*1000.f / wtrTotal;
	// initial dissolved oxygen concentration in reach (o2con mg/L)
	float o2con = m_chDO[i]*1000.f / wtrTotal;

	// temperature of water in reach (wtmp deg C)
	float wtmp = max(m_chTemp[i], 0.1f);
	// calculate effective concentration of available nitrogen (cinn)
	float cinn = nh3con + no3con;

	// calculate saturation concentration for dissolved oxygen
	float ww = -139.34410f + (1.575701e05f / (wtmp + 273.15f));    // variable to hold intermediate calculation result
	float xx = 6.642308e+07f / pow((wtmp + 273.15f), 2);           // variable to hold intermediate calculation result
	float yy = 1.243800e+10f / pow((wtmp + 273.15f), 3);           // variable to hold intermediate calculation result
	float zz = 8.621949e+11f / pow((wtmp + 273.15f), 4);           // variable to hold intermediate calculation result
	m_chSatDO = exp(ww - xx + yy - zz);
	if (m_chSatDO < 1.e-6f)
		m_chSatDO = 0.f;

	// O2 impact calculations
	// calculate nitrification rate correction factor for low (cordo)
	float cordo = 0.f;
	float o2con2 = 0.f;
	o2con2 = o2con;
	if (o2con2 <= 0.1f)
		o2con2 = 0.1f;
	else if (o2con2 > 30.f)
		o2con2 = 30.f;

	cordo = 1.f - exp(-0.6f * o2con2);
	if (o2con <= 0.001f)
		o2con = 0.001f;
	else if (o2con > 30.f)
		o2con = 30.f;

	cordo = 1.f - exp(-0.6f * o2con);

	// modify ammonia and nitrite oxidation rates to account for low oxygen
	// rate constant for biological oxidation of NH3 to NO2 modified to reflect impact of low oxygen concentration (bc1mod)
	float bc1mod = 0.f;
	// rate constant for biological oxidation of NO2 to NO3 modified to reflect impact of low oxygen concentration (bc1mod)
	float bc2mod = 0.f;
	bc1mod = m_bc1[i] * cordo;
	bc2mod = m_bc2[i] * cordo;

	//	tday is the calculation time step = 1 day
	float tday = 1.f;

	// algal growth
	// calculate light extinction coefficient (lambda)
	float lambda = 0.f;
	if (m_ai0 * algcon > 1.e-6f)
		lambda = m_lambda0 + (m_lambda1 * m_ai0 * algcon) + m_lambda2 * pow((m_ai0 * algcon), 0.6666667f);
	else
		lambda = m_lambda0;

	// calculate algal growth limitation factors for nitrogen and phosphorus
	float fnn = 0.f;
	float fpp = 0.f;
	fnn = cinn / (cinn + m_k_n);
	fpp = solpcon / (solpcon + m_k_p);

	// calculate daylight average, photo synthetically active (algi)
	float algi = 0.f;
	if (m_chDaylen[i] > 0.f)
		algi = m_chSr[i] * tfact / m_chDaylen[i];
	else
		algi = 0.00001f;

	// calculate growth attenuation factor for light, based on daylight average light intensity
	float fl_1 = 0.f;
	float fll = 0.f;
	fl_1 = (1.f / (lambda * m_chWTdepth[i])) *
		log((m_k_l * 24.f + algi) / (m_k_l * 24.f + algi * exp(-lambda * m_chWTdepth[i])));
	fll = 0.92f * (m_chDaylen[i] / 24.f) * fl_1;

	// calculate local algal growth rate
	float gra = 0.f;
	float dchla = 0.f;
	float dbod = 0.f;
	float ddisox = 0.f;
	float dorgn = 0.f;
	float dnh4 = 0.f;
	float dno2 = 0.f;
	float dno3 = 0.f;
	float dorgp = 0.f;
	float dsolp = 0.f;
	switch (igropt)
	{
	case 1:
		// multiplicative
		gra = m_mumax * fll * fnn * fpp;
	case 2:
		// limiting nutrient
		gra = m_mumax * fll * min(fnn, fpp);
	case 3:
		// harmonic mean
		if (fnn > 1.e-6f && fpp > 1.e-6f)
			gra = m_mumax * fll * 2.f / ((1.f / fnn) + (1.f / fpp));
		else
			gra = 0.f;
	}

	// calculate algal biomass concentration at end of day (phytoplanktonic algae)
	float dalgae = 0.f;
	float setl = min(1.f, corTempc(m_rs1[i], thrs1, wtmp) / m_chWTdepth[i]);
	dalgae = algcon +
		(corTempc(gra, thgra, wtmp) * algcon - corTempc(m_rhoq, thrho, wtmp) * algcon - setl * algcon) * tday;
	if (dalgae < 0.00001f)
		m_chAlgae[i] = 0.00001f;
	// calculate chlorophyll-a concentration at end of day
	dchla = 0.f;
	dchla = dalgae * m_ai0 / 1000.f;

	// oxygen calculations
	// calculate carbonaceous biological oxygen demand at end of day (dbod)
	float yyy = 0.f;     // variable to hold intermediate calculation result
	float zzz = 0.f;     // variable to hold intermediate calculation result
	yyy = corTempc(m_rk4[i], thm_rk1, wtmp) * cbodcon;
	zzz = corTempc(m_rk3[i], thm_rk3, wtmp) * cbodcon;
	dbod = 0.f;
	dbod = cbodcon - (yyy + zzz) * tday;
	if (dbod < 0.00001f)
		dbod = 0.00001f;

	// calculate dissolved oxygen concentration if reach at end of day (ddisox)
	float uu = 0.f;     // variable to hold intermediate calculation result
	float vv = 0.f;      // variable to hold intermediate calculation result
	ww = 0.f;    // variable to hold intermediate calculation result
	xx = 0.f;     // variable to hold intermediate calculation result
	yy = 0.f;     // variable to hold intermediate calculation result
	zz = 0.f;     // variable to hold intermediate calculation result
	float hh = corTempc(m_rk2[i], thm_rk2, wtmp);
	uu = corTempc(m_rk2[i], thm_rk2, wtmp) * (m_chSatDO - o2con);
	if (algcon > 0.001f)
		vv = (m_ai3 * corTempc(gra, thgra, wtmp) - m_ai4 * corTempc(m_rhoq, thrho, wtmp)) * algcon;
	else
		algcon = 0.001f;

	ww = corTempc(m_rk1[i], thm_rk1, wtmp) * cbodcon;
	if (m_chWTdepth[i] > 0.001f)
		xx = corTempc(m_rk4[i], thm_rk4, wtmp) / (m_chWTdepth[i] * 1000.f);

	if (nh3con > 0.001f)
		yy = m_ai5 * corTempc(bc1mod, thbc1, wtmp) * nh3con;
	else
		nh3con = 0.001f;

	if (no2con > 0.001f)
		zz = m_ai6 * corTempc(bc2mod, thbc2, wtmp) * no2con;
	else
		no2con = 0.001f;

	ddisox = 0.f;
	ddisox = o2con + (uu + vv - ww - xx - yy - zz) * tday;
	//o2proc = o2con - ddisox;   // not found variable "o2proc"
	if (ddisox < 0.00001f)
		ddisox = 0.00001f;

	// nitrogen calculations
	// calculate organic N concentration at end of day (dorgn)
	xx = 0.f;
	yy = 0.f;
	zz = 0.f;
	xx = m_ai1 * corTempc(m_rhoq, thrho, wtmp) * algcon;
	yy = corTempc(m_bc3[i], thbc3, wtmp) * orgncon;
	zz = corTempc(m_rs4[i], thrs4, wtmp) * orgncon;
	dorgn = 0.f;
	dorgn = orgncon + (xx - yy - zz) * tday;
	if (dorgn < 0.00001f)
		dorgn = 0.00001f;

	// calculate fraction of algal nitrogen uptake from ammonia pool
	float f1 = 0.f;
	f1 = m_p_n * nh3con / (m_p_n * nh3con + (1.f - m_p_n) * no3con + 1.e-6f);

	// calculate ammonia nitrogen concentration at end of day (dnh4)
	ww = 0.f;
	xx = 0.f;
	yy = 0.f;
	zz = 0.f;
	ww = corTempc(m_bc3[i], thbc3, wtmp) * orgncon;
	xx = corTempc(bc1mod, thbc1, wtmp) * nh3con;
	yy = corTempc(m_rs3[i], thrs3, wtmp) / (m_chWTdepth[i] * 1000.f);
	zz = f1 * m_ai1 * algcon * corTempc(gra, thgra, wtmp);
	dnh4 = 0.f;
	dnh4 = nh3con + (ww - xx + yy - zz) * tday;
	if (dnh4 < 1.e-6f)
		dnh4 = 0.f;

	// calculate concentration of nitrite at end of day (dno2)
	yy = 0.f;
	zz = 0.f;
	yy = corTempc(bc1mod, thbc1, wtmp) * nh3con;
	zz = corTempc(bc2mod, thbc2, wtmp) * no2con;
	dno2 = 0.f;
	dno2 = no2con + (yy - zz) * tday;
	if (dno2 < 1.e-6f)
		dno2 = 0.f;

	// calculate nitrate concentration at end of day (dno3)
	yy = 0.f;
	zz = 0.f;
	yy = corTempc(bc2mod, thbc2, wtmp) * no2con;
	zz = (1.f - f1) * m_ai1 * algcon * corTempc(gra, thgra, wtmp);
	dno3 = 0.f;
	dno3 = no3con + (yy - zz) * tday;
	if (dno3 < 1.e-6)
		dno3 = 0.f;

	// phosphorus calculations
	// calculate organic phosphorus concentration at end of day
	xx = 0.f;
	yy = 0.f;
	zz = 0.f;
	xx = m_ai2 * corTempc(m_rhoq, thrho, wtmp) * algcon;
	yy = corTempc(m_bc4[i], thbc4, wtmp) * orgpcon;
	zz = corTempc(m_rs5[i], thrs5, wtmp) * orgpcon;
	dorgp = 0.f;
	dorgp = orgpcon + (xx - yy - zz) * tday;
	if (dorgp < 1.e-6f)
		dorgp = 0.f;

	// calculate dissolved phosphorus concentration at end of day (dsolp)
	xx = 0.f;
	yy = 0.f;
	zz = 0.f;
	xx = corTempc(m_bc4[i], thbc4, wtmp) * orgpcon;
	yy = corTempc(m_rs2[i], thrs2, wtmp) / (m_chWTdepth[i] * 1000.f);
	zz = m_ai2 * corTempc(gra, thgra, wtmp) * algcon;
	dsolp = 0.f;
	dsolp = solpcon + (xx + yy - zz) * tday;
	if (dsolp < 1.e-6)
		dsolp = 0.f;

	// concentration at end of day
	m_chAlgae[i] = dalgae * wtrTotal / 1000.f; //kg
	m_chOrgN[i]  = dorgn * wtrTotal / 1000.f;
	m_chNH4[i]   = dnh4 * wtrTotal / 1000.f;
	m_chNO2[i]   = dno2 * wtrTotal / 1000.f;
	m_chNO3[i]   = dno3 * wtrTotal / 1000.f;
	m_chOrgP[i]  = dorgp * wtrTotal / 1000.f;
	m_chSolP[i]  = dsolp * wtrTotal / 1000.f;
	m_chCOD[i]   = dbod * wtrTotal / 1000.f;
	m_chDO[i]    = ddisox * wtrTotal / 1000.f;

	m_chChlora[i] = m_chAlgae[i] * m_ai0;
}

float NutrCH_QUAL2E::corTempc(float r20, float thk, float tmp)
{
    float theta = 0.f;
    theta = r20 * pow(thk, (tmp - 20.f));
    return theta;
}

void NutrCH_QUAL2E::GetValue(const char *key, float *value)
{
    string sk(key);
    if (StringMatch(sk, VAR_SOXY))
    {
        *value = m_chSatDO;
    }

}

void NutrCH_QUAL2E::Get1DData(const char *key, int *n, float **data)
{
    string sk(key);
    *n = m_nReaches + 1;
    if (StringMatch(sk, VAR_CH_ALGAE)) { *data = m_chAlgae; }
    else if (StringMatch(sk, VAR_CH_NO2)) { *data = m_chNO2; }
    else if (StringMatch(sk, VAR_CH_COD)) { *data = m_chCOD; }
    //else if (StringMatch(sk, VAR_RCH_DOX)) {*data = this -> m_rch_dox;}
    else if (StringMatch(sk, VAR_CH_CHLORA)) { *data = m_chChlora; }
    else
    {
        throw ModelException(MID_NutCHRout, "Get1DData", "Parameter " + sk + " does not exist.");
    }
}