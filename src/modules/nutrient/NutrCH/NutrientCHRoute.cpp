/*//
 * \file NutrientCHRoute.cpp
 * \ingroup NutCHRout
 * \author Huiran Gao
 * \date Jun 2016
 */

#include <iostream>
#include "NutrientCHRoute.h"
#include "MetadataInfo.h"
#include <cmath>
#include <fstream>
#include "ModelException.h"
#include "util.h"
#include <omp.h>

using namespace std;

NutrientCHRoute::NutrientCHRoute(void) :
//input
        m_dt(-1), m_aBank(-1), m_qUpReach(-1), m_rnum1(-1), igropt(-1),
        m_ai0(-1), m_ai1(-1), m_ai2(-1), m_ai3(-1), m_ai4(-1), m_ai5(-1), m_ai6(-1), m_lambda0(-1), m_lambda1(-1),
        m_lambda2(-1),
        m_k_l(-1), m_k_n(-1), m_k_p(-1), m_p_n(-1), tfact(-1), m_mumax(-1), m_rhoq(-1),
        m_daylen(NULL), m_sra(NULL), m_bankStorage(NULL), m_qsSub(NULL), m_qiSub(NULL), m_qgSub(NULL),
        m_qsCh(NULL), m_qiCh(NULL), m_qgCh(NULL), m_chStorage(NULL), m_chWTdepth(NULL), m_wattemp(NULL),
        m_latno3ToCh(NULL), m_surqno3ToCh(NULL), m_surqsolpToCh(NULL), m_no3gwToCh(NULL),
        m_minpgwToCh(NULL), m_sedorgnToCh(NULL), m_sedorgpToCh(NULL), m_sedminpaToCh(NULL),
        m_sedminpsToCh(NULL), m_ammoToCh(NULL), m_nitriteToCh(NULL),
        //output
        m_algae(NULL), m_organicn(NULL), m_organicp(NULL), m_ammonian(NULL), m_nitriten(NULL), m_nitraten(NULL),
        m_disolvp(NULL), m_rch_cbod(NULL), m_rch_dox(NULL), m_chlora(NULL), m_soxy(NULL)
{

}

NutrientCHRoute::~NutrientCHRoute(void)
{
    if (m_algae != NULL)
    {
        delete[] m_algae;
    }
    if (m_organicn != NULL)
    {
        delete[] m_organicn;
    }
    if (m_organicp != NULL)
    {
        delete[] m_organicp;
    }
    if (m_ammonian != NULL)
    {
        delete[] m_ammonian;
    }
    if (m_nitriten != NULL)
    {
        delete[] m_nitriten;
    }
    if (m_nitraten != NULL)
    {
        delete[] m_nitraten;
    }
    if (m_disolvp != NULL)
    {
        delete[] m_disolvp;
    }
    if (m_rch_cbod != NULL)
    {
        delete[] m_rch_cbod;
    }
    if (m_rch_dox != NULL)
    {
        delete[] m_rch_dox;
    }
    if (m_chlora != NULL)
    {
        delete[] m_chlora;
    }

}

bool NutrientCHRoute::CheckInputSize(const char *key, int n)
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

bool NutrientCHRoute::CheckInputData()
{
    if (this->m_dt < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The parameter: m_dt has not been set.");
        return false;
    }
    if (this->m_nReaches < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The parameter: m_nReaches has not been set.");
        return false;
    }
    if (this->m_aBank < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_qUpReach < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_rnum1 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->igropt < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_ai0 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_ai1 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_ai2 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_ai3 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_ai4 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_ai5 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_ai6 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_lambda0 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_lambda1 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_lambda2 < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_k_l < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_k_n < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_k_p < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_p_n < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->tfact < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_mumax < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_rhoq < 0)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_daylen == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_sra == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_bankStorage == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_qsSub == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_qiSub == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_qgSub == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_qsCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_qiCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_qgCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_chStorage == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_chWTdepth == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_wattemp == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_latno3ToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_surqno3ToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_surqsolpToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_no3gwToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_minpgwToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_sedorgnToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_sedorgpToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_sedminpaToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_sedminpsToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_ammoToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    if (this->m_nitriteToCh == NULL)
    {
        throw ModelException("NutCHRout", "CheckInputData", "The input data can not be NULL.");
        return false;
    }
    return true;
}

void NutrientCHRoute::SetValue(const char *key, float value)
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
    else if (StringMatch(sk, VAR_IGROPT)) { this->igropt = value; }
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

void NutrientCHRoute::Set1DData(const char *key, int n, float *data)
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
    else if (StringMatch(sk, VAR_LATNO3_CH)) { this->m_latno3ToCh = data; }
    else if (StringMatch(sk, VAR_SURQNO3_CH)) { this->m_surqno3ToCh = data; }
    else if (StringMatch(sk, VAR_SURQSOLP_CH)) { this->m_surqsolpToCh = data; }
    else if (StringMatch(sk, VAR_NO3GW_CH)) { this->m_no3gwToCh = data; }
    else if (StringMatch(sk, VAR_MINPGW_CH)) { this->m_minpgwToCh = data; }
    else if (StringMatch(sk, VAR_SEDORGN_CH)) { this->m_sedorgnToCh = data; }
    else if (StringMatch(sk, VAR_SEDORGP_CH)) { this->m_sedorgpToCh = data; }
    else if (StringMatch(sk, VAR_SEDMINPA_CH)) { this->m_sedminpaToCh = data; }
    else if (StringMatch(sk, VAR_SEDMINPS_CH)) { this->m_sedminpsToCh = data; }
    else if (StringMatch(sk, VAR_AMMO_CH)) { this->m_ammoToCh = data; }
    else if (StringMatch(sk, VAR_NITRITE_CH)) { this->m_nitriteToCh = data; }
    else
    {
        throw ModelException("NutCHRout", "SetValue", "Parameter " + sk +
                                                      " does not exist in Nutrient module. Please contact the module developer.");
    }
}

void NutrientCHRoute::Set2DData(const char *key, int nrows, int ncols, float **data)
{
    string sk(key);

    if (StringMatch(sk, VAR_REACH_PARAM))
    {
        m_nReaches = ncols - 1;
        m_reachId = data[0];
        m_reachDownStream = data[1];
        m_chOrder = data[2];
        //m_chWidth = data[3];
        //m_chLen = data[4];
        //m_chDepth = data[5];
        //m_chVel = data[6];
        //m_area = data[7];
        m_bc1 = data[8];
        m_bc2 = data[9];
        m_bc3 = data[10];
        m_bc4 = data[11];
        m_rk1 = data[12];
        m_rk2 = data[13];
        m_rk3 = data[14];
        m_rk4 = data[15];
        m_rs1 = data[16];
        m_rs2 = data[17];
        m_rs3 = data[18];
        m_rs4 = data[19];
        m_rs5 = data[20];
        m_reachUpStream.resize(m_nReaches + 1);
        if (m_nReaches > 1)
        {
            for (int i = 1; i <= m_nReaches; i++)// index of the reach is the equal to stream link ID(1 to nReaches)
            {
                int downStreamId = int(m_reachDownStream[i]);
                if (downStreamId <= 0)
                    continue;
                m_reachUpStream[downStreamId].push_back(i);
            }
        }
    }
    else
        throw ModelException("NutCHPout", "Set2DData",
                             "Parameter " + sk + " does not exist. Please contact the module developer.");
}

void  NutrientCHRoute::initialOutputs()
{
    if (m_nReaches <= 0)
        throw ModelException("NutCHPout", "initialOutputs", "The cell number of the input can not be less than zero.");

    if (m_reachLayers.empty())
    {
        CheckInputData();
        for (int i = 1; i <= m_nReaches; i++)
        {
            int order = (int) m_chOrder[i];
            m_reachLayers[order].push_back(i);
        }
    }
    if (m_organicn == NULL)
    {
        m_algae = new float[m_nReaches + 1];
        m_organicn = new float[m_nReaches + 1];
        m_organicp = new float[m_nReaches + 1];
        m_ammonian = new float[m_nReaches + 1];
        m_nitriten = new float[m_nReaches + 1];
        m_nitraten = new float[m_nReaches + 1];
        m_disolvp = new float[m_nReaches + 1];
        m_rch_cbod = new float[m_nReaches + 1];
        m_rch_dox = new float[m_nReaches + 1];
        m_chlora = new float[m_nReaches + 1];
        m_soxy = 0.;

#pragma omp parallel for
        for (int i = 1; i <= m_nReaches; i++)
        {
            m_algae[i] = 0.;
            m_organicn[i] = 0.;
            m_organicp[i] = 0.;
            m_ammonian[i] = 0.;
            m_nitriten[i] = 0.;
            m_nitraten[i] = 0.;
            m_disolvp[i] = 0.;
            m_rch_cbod[i] = 0.;
            m_rch_dox[i] = 0.;
            m_chlora[i] = 0.;

        }
    }
}

int NutrientCHRoute::Execute()
{
    if (!this->CheckInputData())
    {
        return false;
    }
    this->initialOutputs();
    map<int, vector<int> >::iterator it;
    for (it = m_reachLayers.begin(); it != m_reachLayers.end(); it++)
    {
        // There are not any flow relationship within each routing layer.
        // So parallelization can be done here.
        m_nReaches = it->second.size();
        // the size of m_reachLayers (map) is equal to the maximum stream order
#pragma omp parallel for
        for (int i = 0; i < m_nReaches; ++i)
        {
            // index in the array
            int reachIndex = it->second[i];
            NutrientinChannel(reachIndex);
        }
    }
    //return ??
    return 0;
}

/*
//! Get coefficients
void NutrientCHRoute::GetCoefficients(float reachLength, float v0, MuskWeights& weights)
{
	v0 = m_vScalingFactor * v0;
	float K = (4.64f - 3.64f * m_co1) * reachLength / (5.f * v0 / 3.f);

	float min = 2.0f * K * m_x;
	float max = 2.0f * K * (1 - m_x);
	float dt;
	int n;
	weights.dt = dt;

	//get coefficient
	float temp = max + dt;
	weights.c1 = (dt - min)/temp;
	weights.c2 = (dt + min)/temp;
	weights.c3 = (max - dt)/temp;
	weights.c4 = 2*dt/temp;
	weights.n = n;

	//make sure any coefficient is positive
	if(weights.c1 < 0) 
	{
		weights.c2 += weights.c1;
		weights.c1 = 0.0f;
	}
	if(weights.c3 < 0)
	{
		weights.c2 += weights.c1;
		weights.c3 = 0.0f;
	}
}
*/
void NutrientCHRoute::NutrientinChannel(int i)
{

    float thbc1 = 1.083;    ///temperature adjustment factor for local biological oxidation of NH3 to NO2
    float thbc2 = 1.047;    ///temperature adjustment factor for local biological oxidation of NO2 to NO3
    float thbc3 = 1.04;    ///temperature adjustment factor for local hydrolysis of organic N to ammonia N
    float thbc4 = 1.047;    ///temperature adjustment factor for local decay of organic P to dissolved P

    float thgra = 1.047;    ///temperature adjustment factor for local algal growth rate
    float thrho = 1.047;    ///temperature adjustment factor for local algal respiration rate

    float thm_rk1 = 1.047;    ///temperature adjustment factor for local CBOD deoxygenation
    float thm_rk2 = 1.024;    ///temperature adjustment factor for local oxygen reaeration rate
    float thm_rk3 = 1.024;    ///temperature adjustment factor for loss of CBOD due to settling
    float thm_rk4 = 1.060;    ///temperature adjustment factor for local sediment oxygen demand

    float thrs1 = 1.024;    ///temperature adjustment factor for local algal settling rate
    float thrs2 = 1.074;    ///temperature adjustment factor for local benthos source rate for dissolved phosphorus
    float thrs3 = 1.074;    ///temperature adjustment factor for local benthos source rate for ammonia nitrogen
    float thrs4 = 1.024;    ///temperature adjustment factor for local organic N settling rate
    float thrs5 = 1.024;    ///temperature adjustment factor for local organic P settling rate

    // initialize inflow concentrations
    float chlin = 0.;
    float algin = 0.;
    float orgnin = 0.;
    float ammoin = 0.;
    float nitritin = 0.;
    float nitratin = 0.;
    float orgpin = 0.;
    float dispin = 0.;
    float cbodin = 0.;
    float disoxin = 0.;
    float cinn = 0.;
    float heatin = 0.;

    float dalgae = 0.;
    float dchla = 0.;
    float dbod = 0.;
    float ddisox = 0.;
    float dorgn = 0.;
    float dnh4 = 0.;
    float dno2 = 0.;
    float dno3 = 0.;
    float dorgp = 0.;
    float dsolp = 0.;
    // initialize water flowing into reach
    float wtrIn = 0.;
    float qiSub = 0.f;
    if (m_qiSub != NULL)
        qiSub = m_qiSub[i];
    float qgSub = 0.f;
    if (m_qgSub != NULL)
        qgSub = m_qgSub[i];
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
    float bankOut = 0.;
    bankOut = m_bankStorage[i] * (1 - exp(-m_aBank));
    m_bankStorage[i] -= bankOut;
    wtrIn += bankOut / m_dt;

    //wtrIn = varoute[][inum2)] * (1. - m_rnum1);
    // Calculate flow out of reach
    float wtrOut = 0.;
    wtrOut = m_qsCh[i] + m_qiCh[i] + m_qgCh[i];

    // Concentrations
    if ((wtrOut / 86400.) > 0.01 && wtrIn > 0.001)
    {
        // initialize concentration of nutrient in reach
        // inflow + storage water (wtrTotal m3)
        float wtrTotal = 0.;
        // initial algal biomass concentration in reach (algcon mg/L)
        float algcon = 0.;
        // initial organic N concentration in reach (orgncon mg/L)
        float orgncon = 0.;
        // initial ammonia concentration in reach (nh3con mg/L)
        float nh3con = 0.;
        // initial nitrite concentration in reach (no2con mg/L)
        float no2con = 0.;
        // initial nitrate concentration in reach (no3con mg/L)
        float no3con = 0.;
        // initial organic P concentration in reach (orgpcon  mg/L)
        float orgpcon = 0.;
        // initial soluble P concentration in reach (solpcon mg/L)
        float solpcon = 0.;
        // initial carbonaceous biological oxygen demand (cbodcon mg/L)
        float cbodcon = 0.;
        // initial dissolved oxygen concentration in reach (o2con mg/L)
        float o2con = 0.;
        wtrTotal = wtrIn + m_chStorage[i];

        algcon = m_algae[i];
        orgncon = m_organicn[i];
        nh3con = m_ammonian[i];
        no2con = m_nitriten[i];
        no3con = m_nitraten[i];
        orgpcon = m_organicp[i];
        solpcon = m_disolvp[i];
        cbodcon = m_rch_cbod[i];
        o2con = m_rch_dox[i];
        // temperature of water in reach (wtmp deg C)
        float wtmp = 0.;
        wtmp = m_wattemp[i];
        // calculate temperature in stream
        if (wtmp <= 0.)
        {
            wtmp = 0.1;
        }

        // calculate effective concentration of available nitrogen (cinn)
        float cinn = 0.;
        cinn = nh3con + no3con;

        // calculate saturation concentration for dissolved oxygen
        float ww = 0.;    // variable to hold intermediate calculation result
        float xx = 0.;     // variable to hold intermediate calculation result
        float yy = 0.;     // variable to hold intermediate calculation result
        float zz = 0.;     // variable to hold intermediate calculation result
        ww = -139.34410 + (1.575701e05 / (wtmp + 273.15));
        xx = 6.642308e+07 / pow((wtmp + 273.15), 2);
        yy = 1.243800e+10 / pow((wtmp + 273.15), 3);
        zz = 8.621949e+11 / pow((wtmp + 273.15), 4);
        m_soxy = exp(ww - xx + yy - zz);
        if (m_soxy < 1.e-6)
        {
            m_soxy = 0.;
        }

        // O2 impact calculations
        // calculate nitrification rate correction factor for low (cordo)
        float cordo = 0.;
        float o2con2 = 0.;
        o2con2 = o2con;
        if (o2con2 <= 0.1)
        {
            o2con2 = 0.1;
        }
        if (o2con2 > 30.)
        {
            o2con2 = 30.;
        }
        cordo = 1.0 - exp(-0.6 * o2con2);
        if (o2con <= 0.001)
        {
            o2con = 0.001;
        }
        if (o2con > 30.)
        {
            o2con = 30.;
        }
        cordo = 1.0 - exp(-0.6 * o2con);

        // modify ammonia and nitrite oxidation rates to account for low oxygen
        // rate constant for biological oxidation of NH3 to NO2 modified to reflect impact of low oxygen concentration (bc1mod)
        float bc1mod = 0.;
        // rate constant for biological oxidation of NO2 to NO3 modified to reflect impact of low oxygen concentration (bc1mod)
        float bc2mod = 0.;
        bc1mod = m_bc1[i] * cordo;
        bc2mod = m_bc2[i] * cordo;

        //	tday is the calculation time step = 1 day
        float tday = 1.0;

        // algal growth
        // calculate light extinction coefficient (lambda)
        float lambda = 0;
        if (m_ai0 * algcon > 1.e-6)
        {
            lambda = m_lambda0 + (m_lambda1 * m_ai0 * algcon) + m_lambda2 * pow((m_ai0 * algcon), (2 / 3));
        } else
        {
            lambda = m_lambda0;
        }

        // calculate algal growth limitation factors for nitrogen and phosphorus
        float fnn = 0.;
        float fpp = 0.;
        fnn = cinn / (cinn + m_k_n);
        fpp = solpcon / (solpcon + m_k_p);

        // calculate daylight average, photo synthetically active (algi)
        float algi = 0.;
        dchla = 0.;
        dbod = 0.;
        ddisox = 0.;
        dorgn = 0.;
        dnh4 = 0.;
        dno2 = 0.;
        dno3 = 0.;
        dorgp = 0.;
        dsolp = 0.;
        if (m_daylen[i] > 0.)
        {
            algi = m_sra[i] * tfact / m_daylen[i];
        } else
        {
            algi = 0.00001;
        }

        // calculate growth attenuation factor for light, based on daylight average light intensity
        float fl_1 = 0.;
        float fll = 0.;
        fl_1 = (1. / (lambda * m_chWTdepth[i])) *
               log((m_k_l * 24 + algi) / (m_k_l * 24 + algi * exp(-lambda * m_chWTdepth[i])));
        fll = 0.92 * (m_daylen[i] / 24.) * fl_1;

        // calculate local algal growth rate
        float gra = 0.;
        float dchla = 0.;
        float dbod = 0.;
        float ddisox = 0.;
        float dorgn = 0.;
        float dnh4 = 0.;
        float dno2 = 0.;
        float dno3 = 0.;
        float dorgp = 0.;
        float dsolp = 0.;
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
                if (fnn > 1.e-6 && fpp > 1.e-6)
                {
                    gra = m_mumax * fll * 2. / ((1. / fnn) + (1. / fpp));
                } else
                {
                    gra = 0.;
                }
        }

        // calculate algal biomass concentration at end of day (phytoplanktonic algae)
        dalgae = 0.;
        float setl = min(1.f, corTempc(m_rs1[i], thrs1, wtmp) / m_chWTdepth[i]);
        dalgae = algcon +
                 (corTempc(gra, thgra, wtmp) * algcon - corTempc(m_rhoq, thrho, wtmp) * algcon - setl * algcon) * tday;
        if (dalgae < 0.00001)
        {
            m_algae[i] = 0.00001;
        }
        // calculate chlorophyll-a concentration at end of day
        dchla = 0.;
        dchla = dalgae * m_ai0 / 1000.;

        // oxygen calculations
        // calculate carbonaceous biological oxygen demand at end of day (dbod)
        float yyy = 0.;     // variable to hold intermediate calculation result
        float zzz = 0.;     // variable to hold intermediate calculation result
        yyy = corTempc(m_rk4[i], thm_rk1, wtmp) * cbodcon;
        zzz = corTempc(m_rk3[i], thm_rk3, wtmp) * cbodcon;
        dbod = 0.;
        dbod = cbodcon - (yyy + zzz) * tday;
        if (dbod < 0.00001)
        {
            dbod = 0.00001;
        }

        // calculate dissolved oxygen concentration if reach at end of day (ddisox)
        float uu = 0.;     // variable to hold intermediate calculation result
        float vv = 0.;      // variable to hold intermediate calculation result
        ww = 0.;    // variable to hold intermediate calculation result
        xx = 0.;     // variable to hold intermediate calculation result
        yy = 0.;     // variable to hold intermediate calculation result
        zz = 0.;     // variable to hold intermediate calculation result
        float hh = corTempc(m_rk2[i], thm_rk2, wtmp);
        uu = corTempc(m_rk2[i], thm_rk2, wtmp) * (m_soxy - o2con);
        if (algcon > 0.001)
        {
            vv = (m_ai3 * corTempc(gra, thgra, wtmp) - m_ai4 * corTempc(m_rhoq, thrho, wtmp)) * algcon;
        } else
        {
            algcon = 0.001;
        }
        ww = corTempc(m_rk1[i], thm_rk1, wtmp) * cbodcon;
        if (m_chWTdepth[i] > 0.001)
        {
            xx = corTempc(m_rk4[i], thm_rk4, wtmp) / (m_chWTdepth[i] * 1000.);
        }
        if (nh3con > 0.001)
        {
            yy = m_ai5 * corTempc(bc1mod, thbc1, wtmp) * nh3con;
        } else
        {
            nh3con = 0.001;
        }
        if (no2con > 0.001)
        {
            zz = m_ai6 * corTempc(bc2mod, thbc2, wtmp) * no2con;
        } else
        {
            no2con = 0.001;
        }
        ddisox = 0.;
        ddisox = o2con + (uu + vv - ww - xx - yy - zz) * tday;
        //o2proc = o2con - ddisox;   // not found variable "o2proc"
        if (ddisox < 0.00001)
        {
            ddisox = 0.00001;
        }

        // nitrogen calculations
        // calculate organic N concentration at end of day (dorgn)
        xx = 0.;
        yy = 0.;
        zz = 0.;
        xx = m_ai1 * corTempc(m_rhoq, thrho, wtmp) * algcon;
        yy = corTempc(m_bc3[i], thbc3, wtmp) * orgncon;
        zz = corTempc(m_rs4[i], thrs4, wtmp) * orgncon;
        dorgn = 0.;
        dorgn = orgncon + (xx - yy - zz) * tday;
        if (dorgn < 0.00001)
        {
            dorgn = 0.00001;
        }

        // calculate fraction of algal nitrogen uptake from ammonia pool
        float f1 = 0.;
        f1 = m_p_n * nh3con / (m_p_n * nh3con + (1. - m_p_n) * no3con + 1.e-6);

        // calculate ammonia nitrogen concentration at end of day (dnh4)
        ww = 0.;
        xx = 0.;
        yy = 0.;
        zz = 0.;
        ww = corTempc(m_bc3[i], thbc3, wtmp) * orgncon;
        xx = corTempc(bc1mod, thbc1, wtmp) * nh3con;
        yy = corTempc(m_rs3[i], thrs3, wtmp) / (m_chWTdepth[i] * 1000.);
        zz = f1 * m_ai1 * algcon * corTempc(gra, thgra, wtmp);
        dnh4 = 0.;
        dnh4 = nh3con + (ww - xx + yy - zz) * tday;
        if (dnh4 < 1.e-6)
        {
            dnh4 = 0.;
        }

        // calculate concentration of nitrite at end of day (dno2)
        yy = 0.;
        zz = 0.;
        yy = corTempc(bc1mod, thbc1, wtmp) * nh3con;
        zz = corTempc(bc2mod, thbc2, wtmp) * no2con;
        dno2 = 0.;
        dno2 = no2con + (yy - zz) * tday;
        if (dno2 < 1.e-6)
        {
            dno2 = 0.;
        }

        // calculate nitrate concentration at end of day (dno3)
        yy = 0.;
        zz = 0.;
        yy = corTempc(bc2mod, thbc2, wtmp) * no2con;
        zz = (1. - f1) * m_ai1 * algcon * corTempc(gra, thgra, wtmp);
        dno3 = 0.;
        dno3 = no3con + (yy - zz) * tday;
        if (dno3 < 1.e-6)
        {
            dno3 = 0.;
        }

        // phosphorus calculations
        // calculate organic phosphorus concentration at end of day
        xx = 0.;
        yy = 0.;
        zz = 0.;
        xx = m_ai2 * corTempc(m_rhoq, thrho, wtmp) * algcon;
        yy = corTempc(m_bc4[i], thbc4, wtmp) * orgpcon;
        zz = corTempc(m_rs5[i], thrs5, wtmp) * orgpcon;
        dorgp = 0.;
        dorgp = orgpcon + (xx - yy - zz) * tday;
        if (dorgp < 1.e-6)
        {
            dorgp = 0.;
        }

        // calculate dissolved phosphorus concentration at end of day (dsolp)
        xx = 0.;
        yy = 0.;
        zz = 0.;
        xx = corTempc(m_bc4[i], thbc4, wtmp) * orgpcon;
        yy = corTempc(m_rs2[i], thrs2, wtmp) / (m_chWTdepth[i] * 1000.);
        zz = m_ai2 * corTempc(gra, thgra, wtmp) * algcon;
        dsolp = 0.;
        dsolp = solpcon + (xx + yy - zz) * tday;
        if (dsolp < 1.e-6) dsolp = 0.;

        wtrTotal = wtrIn + m_chStorage[i];

        if (wtrIn > 0.001)
        {
            //chlin = 1000. * m_chloraToCh[i] * (1. - m_rnum1) / wtrIn;	//chlorophyll-a
            algin = 1000. * chlin / m_ai0;
            orgnin = 1000. * m_sedorgnToCh[i] * (1. - m_rnum1) / wtrIn;
            ammoin = 1000. * m_ammoToCh[i] * (1. - m_rnum1) / wtrIn;
            nitritin = 1000. * m_nitriteToCh[i] * (1. - m_rnum1) / wtrIn;
            nitratin = 1000. * (m_latno3ToCh[i] + m_surqno3ToCh[i] + m_no3gwToCh[i]) * (1. - m_rnum1) / wtrIn;
            orgpin = 1000. * (m_sedorgpToCh[i] + m_sedminpsToCh[i] + m_sedminpaToCh[i]) * (1. - m_rnum1) / wtrIn;
            dispin = 1000. * (m_surqsolpToCh[i] + m_minpgwToCh[i]) * (1. - m_rnum1) / wtrIn;
            //cbodin = 1000. * m_cbodToCh[i] * (1. - m_rnum1) / wtrIn;
            //disoxin= 1000. * m_disoxToCh[i] * (1. - m_rnum1) / wtrIn;
            heatin = m_wattemp[i] * (1. - m_rnum1);
        }

        m_wattemp[i] = (heatin * wtrIn + wtmp * m_chStorage[i]) / wtrTotal;
        m_algae[i] = (algin * wtrIn + dalgae * m_chStorage[i]) / wtrTotal;
        m_organicn[i] = (orgnin * wtrIn + dorgn * m_chStorage[i]) / wtrTotal;
        m_ammonian[i] = (ammoin * wtrIn + dnh4 * m_chStorage[i]) / wtrTotal;
        m_nitriten[i] = (nitritin * wtrIn + dno2 * m_chStorage[i]) / wtrTotal;
        m_nitraten[i] = (nitratin * wtrIn + dno3 * m_chStorage[i]) / wtrTotal;
        m_organicp[i] = (orgpin * wtrIn + dorgp * m_chStorage[i]) / wtrTotal;
        m_disolvp[i] = (dispin * wtrIn + dsolp * m_chStorage[i]) / wtrTotal;
        //m_rch_cbod[i] = (cbodin * wtrIn + dbod * m_chStorage[i]) / wtrTotal;
        //m_rch_dox[i] = (disoxin * wtrIn +  ddisox * m_chStorage[i]) / wtrTotal;

        // calculate chlorophyll-a concentration at end of day
        m_chlora[i] = 0.;
        m_chlora[i] = m_algae[i] * m_ai0 / 1000.;
    } else
    {
        // all water quality variables set to zero when no flow
        algin = 0.0;
        chlin = 0.0;
        orgnin = 0.0;
        ammoin = 0.0;
        nitritin = 0.0;
        nitratin = 0.0;
        orgpin = 0.0;
        dispin = 0.0;
        cbodin = 0.0;
        disoxin = 0.0;
        m_algae[i] = 0.0;
        m_chlora[i] = 0.0;
        m_organicn[i] = 0.0;
        m_ammonian[i] = 0.0;
        m_nitriten[i] = 0.0;
        m_nitraten[i] = 0.0;
        m_organicp[i] = 0.0;
        m_disolvp[i] = 0.0;
        m_rch_cbod[i] = 0.0;
        m_rch_dox[i] = 0.0;
        dalgae = 0.0;
        dchla = 0.0;
        dorgn = 0.0;
        dnh4 = 0.0;
        dno2 = 0.0;
        dno3 = 0.0;
        dorgp = 0.0;
        dsolp = 0.0;
        dbod = 0.0;
        ddisox = 0.0;
        m_soxy = 0.0;
    }

}

float NutrientCHRoute::corTempc(float r20, float thk, float tmp)
{
    float theta = 0.;
    theta = r20 * pow(thk, (tmp - 20.f));
    return theta;
}

void NutrientCHRoute::GetValue(const char *key, float *value)
{
    string sk(key);
    if (StringMatch(sk, VAR_SOXY))
    {
        *value = m_soxy;
    }

}

void NutrientCHRoute::Get1DData(const char *key, int *n, float **data)
{
    string sk(key);
    *n = m_nReaches + 1;
    if (StringMatch(sk, VAR_ALGAE)) { *data = this->m_algae; }
    else if (StringMatch(sk, VAR_ORGANICN)) { *data = this->m_organicn; }
    else if (StringMatch(sk, VAR_ORGANICP)) { *data = this->m_organicp; }
    else if (StringMatch(sk, VAR_AMMONIAN)) { *data = this->m_ammonian; }
    else if (StringMatch(sk, VAR_NITRITEN)) { *data = this->m_nitriten; }
    else if (StringMatch(sk, VAR_NITRATEN)) { *data = this->m_nitraten; }
    else if (StringMatch(sk, VAR_DISOLVP)) { *data = this->m_disolvp; }
    else if (StringMatch(sk, VAR_RCH_CBOD)) { *data = this->m_rch_cbod; }
    else if (StringMatch(sk, VAR_RCH_DOX)) { *data = this->m_rch_dox; }
    else if (StringMatch(sk, VAR_CHLORA)) { *data = this->m_chlora; }
    else
    {
        throw ModelException("NutCHRout", "GetValue",
                             "Parameter " + sk + " does not exist. Please contact the module developer.");
    }
}