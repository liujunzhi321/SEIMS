#include "api.h"
#include <iostream>
#include "AET_PriestleyTaylorHargreaves.h"
#include "MetadataInfo.h"
#include "ModelException.h"

using namespace std;

AET_PT_H::AET_PT_H(void) : m_nCells(-1), m_soilLayers(-1), m_esco(NULL), m_nSoilLayers(NULL), m_soilDepth(NULL), 
	                       m_soilThick(NULL), m_solAWC(NULL),
						   /// input from other modules
                           m_tMean(NULL), m_lai(NULL), m_pet(NULL), m_snowAcc(NULL), m_snowSB(NULL),
                           m_solCov(NULL), m_solNo3(NULL), m_somo(NULL),
						   /// output
                            m_ppt(NULL), m_soilESDay(NULL), m_no3Up(NODATA_VALUE), m_totSOMO(NULL)
{
}

AET_PT_H::~AET_PT_H(void)
{
    /// clean up output variables
	if(m_snowSB != NULL) Release1DArray(m_snowSB);
    if (m_ppt != NULL) Release1DArray(m_ppt);
	if (m_soilESDay != NULL) Release1DArray(m_soilESDay);
	if (m_snowAcc != NULL) Release1DArray(m_snowAcc);
    if (m_somo != NULL) Release2DArray(m_nCells, m_somo);
    if (m_totSOMO != NULL) Release1DArray(m_totSOMO);
}

void AET_PT_H::Set1DData(const char *key, int n, float *data)
{
    string sk(key);
    CheckInputSize(key, n);
    if (StringMatch(sk, VAR_ESCO)) m_esco = data;
    else if (StringMatch(sk, VAR_SOILLAYERS)) m_nSoilLayers = data;
    else if (StringMatch(sk, DataType_MeanTemperature)) m_tMean = data;
    else if (StringMatch(sk, VAR_LAIDAY)) m_lai = data;
    else if (StringMatch(sk, VAR_PET)) m_pet = data;
    else if (StringMatch(sk, VAR_SNAC)) m_snowAcc = data;
    else if (StringMatch(sk, VAR_SNSB)) m_snowSB = data;
    else if (StringMatch(sk, VAR_SOL_COV)) m_solCov = data;
    else
        throw ModelException(MID_AET_PTH, "Set1DData", "Parameter " + sk +
                                                       " does not exist in current module. Please contact the module developer.");
}

void AET_PT_H::Set2DData(const char *key, int n, int col, float **data)
{
    string sk(key);
    CheckInputSize(key, n);
    m_soilLayers = col;
    if (StringMatch(sk, VAR_SOILDEPTH)) m_soilDepth = data;
	else if(StringMatch(sk, VAR_SOILTHICK)) m_soilThick = data;
    else if (StringMatch(sk, VAR_SOL_AWC)) m_solAWC = data;
    else if (StringMatch(sk, VAR_SOL_NO3)) m_solNo3 = data;
    else if (StringMatch(sk, VAR_SOMO)) m_somo = data;
    else
        throw ModelException(MID_AET_PTH, "Set2DData", "Parameter " + sk +
                                                       " does not exist in current module. Please contact the module developer.");
}

bool AET_PT_H::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
        throw ModelException(MID_AET_PTH, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    if (this->m_nCells != n)
    {
        if (this->m_nCells <= 0) this->m_nCells = n;
        else
            throw ModelException(MID_AET_PTH, "CheckInputSize", "Input data for " + string(key) +
                                                                " is invalid. All the input data should have same size.");
    }
    return true;
}

bool AET_PT_H::CheckInputData(void)
{
    if (this->m_date <= 0) throw ModelException(MID_AET_PTH, "CheckInputData", "You have not set the time.");
    if (this->m_nCells <= 0)
        throw ModelException(MID_AET_PTH, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    if (this->m_esco == NULL)
        throw ModelException(MID_AET_PTH, "CheckInputData",
                             "The soil evaporation compensation factor can not be NULL.");
    if (this->m_nSoilLayers == NULL)
        throw ModelException(MID_AET_PTH, "CheckInputData",
                             "The soil layers can not be NULL.");
    if (this->m_tMean == NULL)
        throw ModelException(MID_AET_PTH, "CheckInputData",
                             "The mean temperature can not be NULL.");
    if (this->m_lai == NULL)
        throw ModelException(MID_AET_PTH, "CheckInputData",
                             "The leaf area index can not be NULL.");
    if (this->m_pet == NULL)
        throw ModelException(MID_AET_PTH, "CheckInputData",
                             "The potential evaportranspiration can not be NULL.");
    if (this->m_snowAcc == NULL)
        throw ModelException(MID_AET_PTH, "CheckInputData",
                             "The snow accumulation can not be NULL.");
    //if (this->m_snowSB == NULL)
    //    throw ModelException(MID_AET_PTH, "CheckInputData",
    //                         "The snow sublimation can not be NULL.");
    if (this->m_solCov == NULL)
        throw ModelException(MID_AET_PTH, "CheckInputData",
                             "The residue on soil surface can not be NULL.");
    if (this->m_soilDepth == NULL)
        throw ModelException(MID_AET_PTH, "CheckInputData",
                             "The soil depth can not be NULL.");
	if (this->m_soilThick == NULL)
		throw ModelException(MID_AET_PTH, "CheckInputData",
		"The soil thickness can not be NULL.");
    if (this->m_solAWC == NULL)
        throw ModelException(MID_AET_PTH, "CheckInputData",
                             "The available water capacity at field capacity can not be NULL.");
    if (this->m_solNo3 == NULL)
        throw ModelException(MID_AET_PTH, "CheckInputData",
                             "Nitrogen stored in the nitrate pool can not be NULL.");
    if (this->m_somo == NULL)
        throw ModelException(MID_AET_PTH, "CheckInputData",
                             "The soil moisture can not be NULL.");
    return true;
}
void AET_PT_H::initialOutputs()
{
	/// initialize output variables
	if(this->m_totSOMO == NULL) Initialize1DArray(m_nCells, m_totSOMO, 0.f);
	if(this->m_ppt == NULL) Initialize1DArray(m_nCells, m_ppt, 0.f);
	if(this->m_soilESDay == NULL) Initialize1DArray(m_nCells, m_soilESDay, 0.f);
    if (m_no3Up == NODATA_VALUE) m_no3Up = 0.f;
	if(this->m_snowSB == NULL) Initialize1DArray(m_nCells, m_snowSB, 0.f);
}
int AET_PT_H::Execute()
{
    CheckInputData();
	initialOutputs();
    
    /// define intermediate variables
    float esd = 0.f, etco = 0.f, effnup = 0.f;
    float no3up = 0.f, es_max = 0.f, eos1 = 0.f, xx = 0.f;
	float cej = 0.f, eaj = 0.f, pet = 0.f, esleft = 0.f;
    float evzp = 0.f, eosl = 0.f, dep = 0.f, evz = 0.f, sev = 0.f;

    for (int i = 0; i < m_nCells; i++)
    {
        pet = m_pet[i];
        esd = 500.f;
        etco = 0.8f;
        effnup = 0.1f;
        if (pet > UTIL_ZERO)
        {
            /// compute potential plant evapotranspiration (PPT) other than Penman-Monteith method
            if (m_lai[i] <= 3.f)
                m_ppt[i] = m_lai[i] * m_pet[i] / 3.f;
            else
                m_ppt[i] = m_pet[i];
            if (m_ppt[i] < 0.f)
                m_ppt[i] = 0.f;
            /// compute potential soil evaporation
            cej = -5.e-5f;
            eaj = 0.f;
            es_max = 0.f;  ///maximum amount of evaporation (soil et)
            eos1 = 0.f;
            if (m_snowAcc[i] >= 0.5f)
                eaj = 0.5f;
            else
                eaj = exp(cej * (m_solCov[i] + 0.1f));
            es_max = m_pet[i] * eaj;
            eos1 = m_pet[i] / (es_max + m_ppt[i] + 1.e-10f);
            eos1 = es_max * eos1;
            es_max = min(es_max, eos1);
            es_max = max(es_max, 0.f);
            /// make sure maximum plant and soil ET doesn't exceed potential ET
            if (m_pet[i] < es_max + m_ppt[i])
            {
                es_max = m_pet[i] * es_max / (es_max + m_ppt[i]);
                m_ppt[i] = m_pet[i] * m_ppt[i] / (es_max + m_ppt[i]);
            }
            if (m_pet[i] < es_max + m_ppt[i])
                es_max = m_pet[i] - m_ppt[i] - UTIL_ZERO;

            /// initialize soil evaporation variables
            esleft = es_max;
            /// compute sublimation, using the input m_snowSB from snow sublimation module, if not provided, initialized as 0
            if (m_tMean[i] > 0.f)
            {
                if (m_snowAcc[i] >= esleft)
                {
                    /// take all soil evap from snow cover
                    m_snowAcc[i] -= esleft;
                    m_snowSB[i] += esleft;
                    esleft = 0.f;
                }
                else
                {
                    /// take all soil evap from snow cover then start taking from soil
                    esleft -= m_snowAcc[i];
                    m_snowSB[i] += m_snowAcc[i];
                    m_snowAcc[i] = 0.f;
                }
            }
            evzp = 0.f;
            eosl = esleft;
            for (int ly = 0; ly < (int)m_nSoilLayers[i]; ly++)
            {
                /// depth exceeds max depth for soil evap (esd, by default 500 mm)
                if (ly == 0)
                    dep = m_soilDepth[i][ly];
                else
                    dep = m_soilDepth[i][ly - 1];
                if (dep < esd)
                {
                    /// calculate evaporation from soil layer
                    evz = 0.f;
                    sev = 0.f;
                    xx = 0.f;
                    evz = eosl * m_soilDepth[i][ly] / (m_soilDepth[i][ly] + exp(2.374f - 0.00713f * m_soilDepth[i][ly]));
                    sev = evz - evzp * m_esco[i];
                    evzp = evz;
                    if (m_somo[i][ly] < m_solAWC[i][ly])
                    {
                        xx = 2.5f * (m_somo[i][ly] - m_solAWC[i][ly]) / m_solAWC[i][ly]; /// non dimension
                        sev *= Expo(xx);
                    }
                    sev = min(sev, m_somo[i][ly] * etco);
                    if (sev < 0.f) sev = 0.f;
                    if (sev > esleft) sev = esleft;
                    /// adjust soil storage, potential evap
                    if (m_somo[i][ly] > sev)
                    {
                        esleft -= sev;
                        m_somo[i][ly] = max(UTIL_ZERO, m_somo[i][ly] - sev);
                    }
                    else
                    {
                        esleft -= m_somo[i][ly];
                        m_somo[i][ly] = 0.f;
                    }
                }
                /// compute no3 flux from layer 2 to 1 by soil evaporation
                if (ly == 1)  /// index of layer 2 is 1 (soil surface, 10mm)
                {
                    no3up = 0.f;
                    no3up = effnup * sev * m_solNo3[i][ly] / (m_somo[i][ly] + UTIL_ZERO);
                    no3up = min(no3up, m_solNo3[i][ly]);
                    m_no3Up += no3up / m_nCells;
                    m_solNo3[i][ly] -= no3up;
                    m_solNo3[i][0] += no3up;
                }
            }
            /// update total soil water content
            m_totSOMO[i] = 0.f;
            for (int ly = 0; ly < (int)m_nSoilLayers[i]; ly++)
                m_totSOMO[i] += m_somo[i][ly];
            /// calculate actual amount of evaporation from soil
            m_soilESDay[i] = es_max - esleft;
            if (m_soilESDay[i] < 0.f) m_soilESDay[i] = 0.f;
        }
    }
    return true;
}

//m_ppt(NULL), m_solAET(NULL), m_no3Up(NODATA), m_totSOMO(NULL)
void AET_PT_H::GetValue(const char *key, float *value)
{
	initialOutputs();
    string sk(key);
    if (StringMatch(sk, VAR_SNO3UP)) *value = this->m_no3Up;
    else
        throw ModelException(MID_AET_PTH, "GetValue", "Result " + sk
                                                      +
                                                      " does not exist in current module. Please contact the module developer.");
}

void AET_PT_H::Get1DData(const char *key, int *n, float **data)
{
	initialOutputs();
    string sk(key);
    if (StringMatch(sk, VAR_PPT)) *data = this->m_ppt;
    else if (StringMatch(sk, VAR_SOET)) *data = this->m_soilESDay;
    else if (StringMatch(sk, VAR_SNAC)) *data = this->m_snowAcc;
	else if (StringMatch(sk, VAR_SNSB)) *data = m_snowSB;
    else if (StringMatch(sk, VAR_SOMO_TOT)) *data = this->m_totSOMO;
    else
        throw ModelException(MID_AET_PTH, "Get1DData", "Result " + sk
                                                       +
                                                       " does not exist in current module. Please contact the module developer.");
    *n = this->m_nCells;
}

void AET_PT_H::Get2DData(const char *key, int *n, int *col, float ***data)
{
	initialOutputs();
    string sk(key);
    if (StringMatch(sk, VAR_SOMO)) *data = this->m_somo;
    else
        throw ModelException(MID_AET_PTH, "Get2DData", "Result " + sk
                                                       +
                                                       " does not exist in current module. Please contact the module developer.");
    *n = this->m_nCells;
    *col = this->m_soilLayers;
}