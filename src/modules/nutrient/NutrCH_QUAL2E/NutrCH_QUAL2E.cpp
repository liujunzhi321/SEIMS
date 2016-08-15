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

NutrCH_QUAL2E::NutrCH_QUAL2E(void) :
//input
        m_dt(-1), m_aBank(-1.f), m_qUpReach(-1.f), m_rnum1(-1.f), igropt(-1),
        m_ai0(-1.f), m_ai1(-1.f), m_ai2(-1.f), m_ai3(-1.f), m_ai4(-1.f), m_ai5(-1.f), 
		m_ai6(-1.f), m_lambda0(-1.f), m_lambda1(-1.f), m_lambda2(-1.f),
        m_k_l(-1.f), m_k_n(-1.f), m_k_p(-1.f), m_p_n(-1.f), tfact(-1.f), m_mumax(-1.f), m_rhoq(-1.f),
        m_daylen(NULL), m_sra(NULL), m_bankStorage(NULL), m_qsSub(NULL), m_qiSub(NULL), m_qgSub(NULL),
        m_qsCh(NULL), m_qiCh(NULL), m_qgCh(NULL), m_chStorage(NULL), m_chWTdepth(NULL), m_wattemp(NULL),
        m_latno3ToCh(NULL), m_surqno3ToCh(NULL), m_surqsolpToCh(NULL), m_no3gwToCh(NULL),
        m_minpgwToCh(NULL), m_sedorgnToCh(NULL), m_sedorgpToCh(NULL), m_sedminpaToCh(NULL),
        m_sedminpsToCh(NULL), m_ammoToCh(NULL), m_nitriteToCh(NULL),
        //output
        m_algae(NULL), m_organicn(NULL), m_organicp(NULL), m_ammonian(NULL), m_nitriten(NULL), m_nitraten(NULL),
        m_disolvp(NULL), m_rch_cod(NULL), m_rch_dox(NULL), m_chlora(NULL), m_soxy(NULL)
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

    if (m_algae != NULL) Release1DArray(m_algae);
    if (m_organicn != NULL) Release1DArray(m_organicn);
    if (m_organicp != NULL) Release1DArray(m_organicp);
    if (m_ammonian != NULL) Release1DArray(m_ammonian);
    if (m_nitriten != NULL) Release1DArray(m_nitriten);
    if (m_nitraten != NULL) Release1DArray(m_nitraten);
    if (m_disolvp != NULL) Release1DArray(m_disolvp);
    if (m_rch_cod != NULL) Release1DArray(m_rch_cod);
    if (m_rch_dox != NULL) Release1DArray(m_rch_dox);
    if (m_chlora != NULL) Release1DArray(m_chlora);
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
            throw ModelException("NutCHRout", "CheckInputSize", oss.str());
        }
    }
    return true;
}

bool NutrCH_QUAL2E::CheckInputData()
{
    if (this->m_dt < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The parameter: m_dt has not been set.");
    }
    if (this->m_nReaches < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The parameter: m_nReaches has not been set.");
    }
    if (this->m_aBank < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_qUpReach < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_rnum1 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->igropt < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_ai0 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_ai1 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_ai2 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_ai3 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_ai4 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_ai5 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_ai6 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_lambda0 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_lambda1 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_lambda2 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_k_l < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_k_n < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_k_p < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_p_n < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->tfact < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_mumax < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_rhoq < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_daylen == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_sra == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_bankStorage == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_qsSub == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_qiSub == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_qgSub == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_qsCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_qiCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_qgCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_chStorage == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_chWTdepth == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_wattemp == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_latno3ToCh == NULL)
    {
		throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_surqno3ToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_surqsolpToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_no3gwToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_minpgwToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_sedorgnToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_sedorgpToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_sedminpaToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_sedminpsToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_ammoToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    if (this->m_nitriteToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
    }
    return true;
}

void NutrCH_QUAL2E::SetValue(const char *key, float value)
{
    string sk(key);
    if (StringMatch(sk, VAR_OMP_THREADNUM))
    {
        omp_set_num_threads((int) value);
    }
    else if (StringMatch(sk, Tag_ChannelTimeStep)) { this->m_dt = (int) value; }
    else if (StringMatch(sk, VAR_A_BNK)) { this->m_aBank = value; }
    else if (StringMatch(sk, VAR_QUPREACH)) { this->m_qUpReach = value; }
    else if (StringMatch(sk, VAR_RNUM1)) { this->m_rnum1 = value; }
    else if (StringMatch(sk, VAR_IGROPT)) { this->igropt = (int) value; }
    else if (StringMatch(sk, VAR_AI0)) { this->m_ai0 = value; }
    else if (StringMatch(sk, VAR_AI1)) { this->m_ai1 = value; }
    else if (StringMatch(sk, VAR_AI2)) { this->m_ai2 = value; }
    else if (StringMatch(sk, VAR_AI3)) { this->m_ai3 = value; }
    else if (StringMatch(sk, VAR_AI4)) { this->m_ai4 = value; }
    else if (StringMatch(sk, VAR_AI5)) { this->m_ai5 = value; }
    else if (StringMatch(sk, VAR_AI6)) { this->m_ai6 = value; }
    else if (StringMatch(sk, VAR_LAMBDA0)) { this->m_lambda0 = value; }
    else if (StringMatch(sk, VAR_LAMBDA1)) { this->m_lambda1 = value; }
    else if (StringMatch(sk, VAR_LAMBDA2)) { this->m_lambda2 = value; }
    else if (StringMatch(sk, VAR_K_L)) { this->m_k_l = value; }
    else if (StringMatch(sk, VAR_K_N)) { this->m_k_n = value; }
    else if (StringMatch(sk, VAR_K_P)) { this->m_k_p = value; }
    else if (StringMatch(sk, VAR_P_N)) { this->m_p_n = value; }
    else if (StringMatch(sk, VAR_TFACT)) { this->tfact = value; }
    else if (StringMatch(sk, VAR_MUMAX)) { this->m_mumax = value; }
    else if (StringMatch(sk, VAR_RHOQ)) { this->m_rhoq = value; }
        //else if(StringMatch(sk, VAR_VSF)) {m_vScalingFactor = value;}
        //else if (StringMatch(sk, VAR_MSK_X)) {m_x = value;}
        //else if (StringMatch(sk, VAR_MSK_CO1)) {m_co1 = value;}
    else
    {
        throw ModelException("NutCHRout", "SetValue", "Parameter " + sk +
                                                      " does not exist in CLIMATE method. Please contact the module developer.");
    }
}

void NutrCH_QUAL2E::Set1DData(const char *key, int n, float *data)
{
    if (!this->CheckInputSize(key, n))
    {
        return;
    }
    string sk(key);
    if (StringMatch(sk, VAR_DAYLEN)) { this->m_daylen = data; }
    else if (StringMatch(sk, VAR_SRA)) { this->m_sra = data; }
    else if (StringMatch(sk, VAR_BKST)) { this->m_bankStorage = data; }
    else if (StringMatch(sk, VAR_SBOF)) { this->m_qsSub = data; }
    else if (StringMatch(sk, VAR_SBIF)) { this->m_qiSub = data; }
    else if (StringMatch(sk, VAR_SBQG)) { this->m_qgSub = data; }
    else if (StringMatch(sk, VAR_QS)) { this->m_qsCh = data; }
    else if (StringMatch(sk, VAR_QI)) { this->m_qiCh = data; }
    else if (StringMatch(sk, VAR_QG)) { this->m_qgCh = data; }
    else if (StringMatch(sk, VAR_CHST)) { this->m_chStorage = data; }
    else if (StringMatch(sk, VAR_CHWTDEPTH)) { this->m_chWTdepth = data; }
    else if (StringMatch(sk, VAR_WATTEMP)) { this->m_wattemp = data; }
    else if (StringMatch(sk, VAR_LATNO3_TOCH)) { this->m_latno3ToCh = data; }
    else if (StringMatch(sk, VAR_SUR_NO3_TOCH)) { this->m_surqno3ToCh = data; }
    else if (StringMatch(sk, VAR_SUR_SOLP_TOCH)) { this->m_surqsolpToCh = data; }
    else if (StringMatch(sk, VAR_NO3GW_TOCH)) { this->m_no3gwToCh = data; }
    else if (StringMatch(sk, VAR_MINPGW_TOCH)) { this->m_minpgwToCh = data; }
    else if (StringMatch(sk, VAR_SEDORGN_TOCH)) { this->m_sedorgnToCh = data; }
    else if (StringMatch(sk, VAR_SEDORGP_TOCH)) { this->m_sedorgpToCh = data; }
    else if (StringMatch(sk, VAR_SEDMINPA_TOCH)) { this->m_sedminpaToCh = data; }
    else if (StringMatch(sk, VAR_SEDMINPS_TOCH)) { this->m_sedminpsToCh = data; }
    else if (StringMatch(sk, VAR_AMMO_CH)) { this->m_ammoToCh = data; }
    else if (StringMatch(sk, VAR_NITRITE_CH)) { this->m_nitriteToCh = data; }
    else
    {
        throw ModelException("NutCHRout", "SetValue", "Parameter " + sk + " does not exist.");
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
        throw ModelException("NutCHPout", "initialOutputs", "The cell number of the input can not be less than zero.");

    if (m_reachLayers.empty())
    {
        CheckInputData();
        for (int i = 1; i <= m_nReaches; i++)
		{
			if (m_chOrder == NULL)
				throw ModelException("NutCHPout", "initialOutputs", "Stream order is not loaded successful from Reach table.");
            int order = (int) m_chOrder[i];
            m_reachLayers[order].push_back(i);
        }
    }
    if (m_organicn == NULL)
    {
		Initialize1DArray(m_nReaches+1,m_algae,0.f);
		Initialize1DArray(m_nReaches+1,m_organicn,0.f);
		Initialize1DArray(m_nReaches+1,m_organicp,0.f);
		Initialize1DArray(m_nReaches+1,m_ammonian,0.f);
		Initialize1DArray(m_nReaches+1,m_nitriten,0.f);
		Initialize1DArray(m_nReaches+1,m_nitraten,0.f);
		Initialize1DArray(m_nReaches+1,m_disolvp,0.f);
		Initialize1DArray(m_nReaches+1,m_rch_cod,0.f);
		Initialize1DArray(m_nReaches+1,m_rch_dox,0.f);
		Initialize1DArray(m_nReaches+1,m_chlora,0.f);
        m_soxy = 0.f;
    }
}

int NutrCH_QUAL2E::Execute()
{
    if (!CheckInputData())return false;
    initialOutputs();

    map<int, vector<int> > ::iterator it;
    for (it = m_reachLayers.begin(); it != m_reachLayers.end(); it++)
    {
        // There are not any flow relationship within each routing layer.
        // So parallelization can be done here.
        m_nReaches = it->second.size();
        // the size of m_reachLayers (map) is equal to the maximum stream order
		#pragma omp parallel for
        for (int i = 1; i <= m_nReaches; ++i)
        {
            // index in the array
            int reachIndex = it->second[i];
            NutrientinChannel(reachIndex);
        }
    }
    return 0;
}

void NutrCH_QUAL2E::NutrientinChannel(int i)
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

    // initialize inflow concentrations
    float chlin = 0.f;
    float algin = 0.f;
    float orgnin = 0.f;
    float ammoin = 0.f;
    float nitritin = 0.f;
    float nitratin = 0.f;
    float orgpin = 0.f;
    float dispin = 0.f;
    float codin = 0.f;
    float disoxin = 0.f;
    float cinn = 0.f;
    float heatin = 0.f;
    float cod = 0.f;

    float dalgae = 0.f;
    float dchla = 0.f;
    float dbod = 0.f;
    float ddisox = 0.f;
    float dorgn = 0.f;
    float dnh4 = 0.f;
    float dno2 = 0.f;
    float dno3 = 0.f;
    float dorgp = 0.f;
    float dsolp = 0.f;
    // initialize water flowing into reach
    float wtrIn = 0.f;
    float qiSub = 0.f;
    if (m_qiSub != NULL)
    {
        qiSub = m_qiSub[i];
    }
    float qgSub = 0.f;
    if (m_qgSub != NULL)
    {
        qgSub = m_qgSub[i];
    }
    // 1. water from this subbasin
    wtrIn = m_qsSub[i] + qiSub + qgSub;
    // 2. water from upstream reaches
    float qsUp = 0.f;
    float qiUp = 0.f;
    float qgUp = 0.f;
    for (size_t j = 0; j < m_reachUpStream[i].size(); ++j)
    {
        int upReachId = m_reachUpStream[i][j];
        qsUp += m_qsCh[upReachId];
        qiUp += m_qiCh[upReachId];
        qgUp += m_qgCh[upReachId];
    }
    wtrIn += qsUp + qiUp + qgUp;
    // m_qUpReach is zero for not-parallel program and qsUp, qiUp and qgUp are zero for parallel computing
    wtrIn += m_qUpReach;
    // 3. water from bank storage
    float bankOut = 0.f;
    bankOut = m_bankStorage[i] * (1.f - exp(-m_aBank));
    m_bankStorage[i] -= bankOut;
    wtrIn += bankOut / m_dt;

    //wtrIn = varoute[][inum2)] * (1. - m_rnum1);
    // Calculate flow out of reach
    float wtrOut = 0.f;
    wtrOut = m_qsCh[i] + m_qiCh[i] + m_qgCh[i];

    // Concentrations
    if ((wtrOut / 86400.f) > 0.01f && wtrIn > 0.001f)
    {
        // initialize concentration of nutrient in reach
        // inflow + storage water (wtrTotal m3)
        float wtrTotal = 0.f;
        // initial algal biomass concentration in reach (algcon mg/L)
        float algcon = 0.f;
        // initial organic N concentration in reach (orgncon mg/L)
        float orgncon = 0.f;
        // initial ammonia concentration in reach (nh3con mg/L)
        float nh3con = 0.f;
        // initial nitrite concentration in reach (no2con mg/L)
        float no2con = 0.f;
        // initial nitrate concentration in reach (no3con mg/L)
        float no3con = 0.f;
        // initial organic P concentration in reach (orgpcon  mg/L)
        float orgpcon = 0.f;
        // initial soluble P concentration in reach (solpcon mg/L)
        float solpcon = 0.f;
        // initial carbonaceous biological oxygen demand (cbodcon mg/L)
        float cbodcon = 0.f;
        // initial dissolved oxygen concentration in reach (o2con mg/L)
        float o2con = 0.f;
        wtrTotal = wtrIn + m_chStorage[i];

        algcon = m_algae[i];
        orgncon = m_organicn[i];
        nh3con = m_ammonian[i];
        no2con = m_nitriten[i];
        no3con = m_nitraten[i];
        orgpcon = m_organicp[i];
        solpcon = m_disolvp[i];
        cbodcon = m_rch_cod[i];
        o2con = m_rch_dox[i];
        // temperature of water in reach (wtmp deg C)
        float wtmp = 0.f;
        wtmp = m_wattemp[i];
        // calculate temperature in stream
        if (wtmp <= 0.f)
        {
            wtmp = 0.1f;
        }

        // calculate effective concentration of available nitrogen (cinn)
        float cinn = 0.f;
        cinn = nh3con + no3con;

        // calculate saturation concentration for dissolved oxygen
        float ww = 0.f;    // variable to hold intermediate calculation result
        float xx = 0.f;     // variable to hold intermediate calculation result
        float yy = 0.f;     // variable to hold intermediate calculation result
        float zz = 0.f;     // variable to hold intermediate calculation result
        ww = -139.34410f + (1.575701e05f / (wtmp + 273.15f));
        xx = 6.642308e+07f / pow((wtmp + 273.15f), 2);
        yy = 1.243800e+10f / pow((wtmp + 273.15f), 3);
        zz = 8.621949e+11f / pow((wtmp + 273.15f), 4);
        m_soxy = exp(ww - xx + yy - zz);
        if (m_soxy < 1.e-6f)
        {
            m_soxy = 0.f;
        }

        // O2 impact calculations
        // calculate nitrification rate correction factor for low (cordo)
        float cordo = 0.f;
        float o2con2 = 0.f;
        o2con2 = o2con;
        if (o2con2 <= 0.1f)
        {
            o2con2 = 0.1f;
        }
        if (o2con2 > 30.f)
        {
            o2con2 = 30.f;
        }
        cordo = 1.f - exp(-0.6f * o2con2);
        if (o2con <= 0.001f)
        {
            o2con = 0.001f;
        }
        if (o2con > 30.f)
        {
            o2con = 30.f;
        }
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
        {
            lambda = m_lambda0 + (m_lambda1 * m_ai0 * algcon) + m_lambda2 * pow((m_ai0 * algcon), 0.6666667f);
        }
        else
        {
            lambda = m_lambda0;
        }

        // calculate algal growth limitation factors for nitrogen and phosphorus
        float fnn = 0.f;
        float fpp = 0.f;
        fnn = cinn / (cinn + m_k_n);
        fpp = solpcon / (solpcon + m_k_p);

        // calculate daylight average, photo synthetically active (algi)
        float algi = 0.f;
        dchla = 0.f;
        dbod = 0.f;
        ddisox = 0.f;
        dorgn = 0.f;
        dnh4 = 0.f;
        dno2 = 0.f;
        dno3 = 0.f;
        dorgp = 0.f;
        dsolp = 0.f;
        if (m_daylen[i] > 0.f)
        {
            algi = m_sra[i] * tfact / m_daylen[i];
        } else
        {
            algi = 0.00001f;
        }

        // calculate growth attenuation factor for light, based on daylight average light intensity
        float fl_1 = 0.f;
        float fll = 0.f;
        fl_1 = (1.f / (lambda * m_chWTdepth[i])) *
               log((m_k_l * 24.f + algi) / (m_k_l * 24.f + algi * exp(-lambda * m_chWTdepth[i])));
        fll = 0.92f * (m_daylen[i] / 24.f) * fl_1;

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
                {
                    gra = m_mumax * fll * 2.f / ((1.f / fnn) + (1.f / fpp));
                } else
                {
                    gra = 0.f;
                }
        }

        // calculate algal biomass concentration at end of day (phytoplanktonic algae)
        dalgae = 0.f;
        float setl = min(1.f, corTempc(m_rs1[i], thrs1, wtmp) / m_chWTdepth[i]);
        dalgae = algcon +
                 (corTempc(gra, thgra, wtmp) * algcon - corTempc(m_rhoq, thrho, wtmp) * algcon - setl * algcon) * tday;
        if (dalgae < 0.00001f)
        {
            m_algae[i] = 0.00001f;
        }
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
        {
            dbod = 0.00001f;
        }

        // calculate dissolved oxygen concentration if reach at end of day (ddisox)
        float uu = 0.f;     // variable to hold intermediate calculation result
        float vv = 0.f;      // variable to hold intermediate calculation result
        ww = 0.f;    // variable to hold intermediate calculation result
        xx = 0.f;     // variable to hold intermediate calculation result
        yy = 0.f;     // variable to hold intermediate calculation result
        zz = 0.f;     // variable to hold intermediate calculation result
        float hh = corTempc(m_rk2[i], thm_rk2, wtmp);
        uu = corTempc(m_rk2[i], thm_rk2, wtmp) * (m_soxy - o2con);
        if (algcon > 0.001f)
        {
            vv = (m_ai3 * corTempc(gra, thgra, wtmp) - m_ai4 * corTempc(m_rhoq, thrho, wtmp)) * algcon;
        }
        else
        {
            algcon = 0.001f;
        }
        ww = corTempc(m_rk1[i], thm_rk1, wtmp) * cbodcon;
        if (m_chWTdepth[i] > 0.001f)
        {
            xx = corTempc(m_rk4[i], thm_rk4, wtmp) / (m_chWTdepth[i] * 1000.f);
        }
        if (nh3con > 0.001f)
        {
            yy = m_ai5 * corTempc(bc1mod, thbc1, wtmp) * nh3con;
        }
        else
        {
            nh3con = 0.001f;
        }
        if (no2con > 0.001f)
        {
            zz = m_ai6 * corTempc(bc2mod, thbc2, wtmp) * no2con;
        }
        else
        {
            no2con = 0.001f;
        }
        ddisox = 0.f;
        ddisox = o2con + (uu + vv - ww - xx - yy - zz) * tday;
        //o2proc = o2con - ddisox;   // not found variable "o2proc"
        if (ddisox < 0.00001f)
        {
            ddisox = 0.00001f;
        }

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
        {
            dorgn = 0.00001f;
        }

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
        {
            dnh4 = 0.f;
        }

        // calculate concentration of nitrite at end of day (dno2)
        yy = 0.f;
        zz = 0.f;
        yy = corTempc(bc1mod, thbc1, wtmp) * nh3con;
        zz = corTempc(bc2mod, thbc2, wtmp) * no2con;
        dno2 = 0.f;
        dno2 = no2con + (yy - zz) * tday;
        if (dno2 < 1.e-6f)
        {
            dno2 = 0.f;
        }

        // calculate nitrate concentration at end of day (dno3)
        yy = 0.f;
        zz = 0.f;
        yy = corTempc(bc2mod, thbc2, wtmp) * no2con;
        zz = (1.f - f1) * m_ai1 * algcon * corTempc(gra, thgra, wtmp);
        dno3 = 0.f;
        dno3 = no3con + (yy - zz) * tday;
        if (dno3 < 1.e-6)
        {
            dno3 = 0.f;
        }

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
        {
            dorgp = 0.f;
        }

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
        {
            dsolp = 0.f;
        }

        wtrTotal = wtrIn + m_chStorage[i];

        if (wtrIn > 0.001f)
        {
            //chlin = 1000. * m_chloraToCh[i] * (1. - m_rnum1) / wtrIn;	//chlorophyll-a
            algin = 1000.f * chlin / m_ai0;
            orgnin = 1000.f * m_sedorgnToCh[i] * (1.f - m_rnum1) / wtrIn;
            ammoin = 1000.f * m_ammoToCh[i] * (1.f - m_rnum1) / wtrIn;
            nitritin = 1000.f * m_nitriteToCh[i] * (1.f - m_rnum1) / wtrIn;
            nitratin = 1000.f * (m_latno3ToCh[i] + m_surqno3ToCh[i] + m_no3gwToCh[i]) * (1.f - m_rnum1) / wtrIn;
            orgpin = 1000.f * (m_sedorgpToCh[i] + m_sedminpsToCh[i] + m_sedminpaToCh[i]) * (1.f - m_rnum1) / wtrIn;
            dispin = 1000.f * (m_surqsolpToCh[i] + m_minpgwToCh[i]) * (1.f - m_rnum1) / wtrIn;
            codin = 1000.f * m_codToCh[i] * (1.f - m_rnum1) / wtrIn;
            //disoxin= 1000. * m_disoxToCh[i] * (1. - m_rnum1) / wtrIn;
            heatin = m_wattemp[i] * (1.f - m_rnum1);
        }

        m_wattemp[i] = (heatin * wtrIn + wtmp * m_chStorage[i]) / wtrTotal;
        m_algae[i] = (algin * wtrIn + dalgae * m_chStorage[i]) / wtrTotal;
        m_organicn[i] = (orgnin * wtrIn + dorgn * m_chStorage[i]) / wtrTotal;
        m_ammonian[i] = (ammoin * wtrIn + dnh4 * m_chStorage[i]) / wtrTotal;
        m_nitriten[i] = (nitritin * wtrIn + dno2 * m_chStorage[i]) / wtrTotal;
        m_nitraten[i] = (nitratin * wtrIn + dno3 * m_chStorage[i]) / wtrTotal;
        m_organicp[i] = (orgpin * wtrIn + dorgp * m_chStorage[i]) / wtrTotal;
        m_disolvp[i] = (dispin * wtrIn + dsolp * m_chStorage[i]) / wtrTotal;
        m_rch_cod[i] = (codin * wtrIn + dbod * m_chStorage[i]) / wtrTotal;
        //m_rch_dox[i] = (disoxin * wtrIn +  ddisox * m_chStorage[i]) / wtrTotal;

        // calculate chlorophyll-a concentration at end of day
        m_chlora[i] = 0.f;
        m_chlora[i] = m_algae[i] * m_ai0 / 1000.f;
    }
    else
    {
        // all water quality variables set to zero when no flow
        algin = 0.f;
        chlin = 0.f;
        orgnin = 0.f;
        ammoin = 0.f;
        nitritin = 0.f;
        nitratin = 0.f;
        orgpin = 0.f;
        dispin = 0.f;
        codin = 0.f;
        disoxin = 0.f;
        m_algae[i] = 0.f;
        m_chlora[i] = 0.f;
        m_organicn[i] = 0.f;
        m_ammonian[i] = 0.f;
        m_nitriten[i] = 0.f;
        m_nitraten[i] = 0.f;
        m_organicp[i] = 0.f;
        m_disolvp[i] = 0.f;
        m_rch_cod[i] = 0.f;
        m_rch_dox[i] = 0.f;
        dalgae = 0.f;
        dchla = 0.f;
        dorgn = 0.f;
        dnh4 = 0.f;
        dno2 = 0.f;
        dno3 = 0.f;
        dorgp = 0.f;
        dsolp = 0.f;
        dbod = 0.f;
        ddisox = 0.f;
        m_soxy = 0.f;
    }
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
        *value = m_soxy;
    }

}

void NutrCH_QUAL2E::Get1DData(const char *key, int *n, float **data)
{
    string sk(key);
    *n = m_nReaches + 1;
    if (StringMatch(sk, VAR_CH_ALGAE)) { *data = this->m_algae; }
    else if (StringMatch(sk, VAR_CH_NO2)) { *data = this->m_nitriten; }
    else if (StringMatch(sk, VAR_CH_COD)) { *data = this->m_rch_cod; }
    //else if (StringMatch(sk, VAR_RCH_DOX)) {*data = this -> m_rch_dox;}
    else if (StringMatch(sk, VAR_CH_CHLORA)) { *data = this->m_chlora; }
    else
    {
        throw ModelException("NutCH_QUAL2E", "GetValue",
                             "Parameter " + sk + " does not exist. Please contact the module developer.");
    }
}