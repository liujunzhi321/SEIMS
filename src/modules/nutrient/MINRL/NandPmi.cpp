/*!
 * \ingroup MINRL
 * \author Huiran Gao
 * \date April 2016
 */

#include <iostream>
#include "NandPmi.h"
#include "MetadataInfo.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include "ModelException.h"
#include "util.h"
#include <omp.h>

using namespace std;

NandPim::NandPim(void) :
		//input
        m_nCells(-1), m_cellWidth(-1), m_soilLayers(-1), m_cmn(-1), m_cdn(-1), m_sdnco(-1),m_nactfr(-1), m_psp(-1),
        m_nSoilLayers(NULL), m_sol_z(NULL), m_sol_thick(NULL), m_sol_clay(NULL), m_sol_bd(NULL),
        m_landcover(NULL), m_rsdco_pl(NULL), m_sol_cbn(NULL), 
        m_sol_wpmm(NULL),  m_sol_awc(NULL),
        m_sol_solp(NULL), m_sol_orgp(NULL),m_sol_actp(NULL), m_sol_stap(NULL),  m_sol_fop(NULL), 
        m_sol_no3(NULL),m_sol_nh3(NULL),m_sol_orgn(NULL), m_sol_aorgn(NULL), m_sol_fon(NULL),
		/// from other modules
		m_sol_cov(NULL),m_sol_rsd(NULL), m_somo(NULL),m_sote(NULL),
		/// cell scale output
		m_hmntl(NULL), m_hmptl(NULL), m_rmn2tl(NULL),
		m_rmptl(NULL), m_rwntl(NULL), m_wdntl(NULL), m_rmp1tl(NULL), m_roctl(NULL),
		/// watershed scale statistics
		m_wshd_dnit(-1), m_wshd_hmn(-1), m_wshd_hmp(-1), m_wshd_rmn(-1), m_wshd_rmp(-1), m_wshd_rwn(-1),
        m_wshd_nitn(-1), m_wshd_voln(-1), m_wshd_pal(-1), m_wshd_pas(-1)
		
{
}

NandPim::~NandPim(void)
{
	if(m_hmntl != NULL) Release1DArray(m_hmntl);
	if(m_hmptl != NULL) Release1DArray(m_hmptl);
	if(m_rmn2tl != NULL) Release1DArray(m_rmn2tl);
	if(m_rmptl != NULL) Release1DArray(m_rmptl);
	if(m_rwntl != NULL) Release1DArray(m_rwntl);
	if(m_wdntl != NULL) Release1DArray(m_wdntl);
	if(m_rmp1tl != NULL) Release1DArray(m_rmp1tl);
	if(m_roctl != NULL) Release1DArray(m_roctl);
}

bool NandPim::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
        throw ModelException(MID_MINRL, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    if (m_nCells != n)
    {
        if (m_nCells <= 0)
            m_nCells = n;
        else
        {
            //StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
            ostringstream oss;
            oss << "Input data for " + string(key) << " is invalid with size: " << n << ". The origin size is " <<
            m_nCells << ".\n";
            throw ModelException(MID_MINRL, "CheckInputSize", oss.str());
        }
    }
    return true;
}

bool NandPim::CheckInputData()
{
    if (this->m_nCells <= 0)
        throw ModelException(MID_MINRL, "CheckInputData", "The input data can not be less than zero.");
    if (this->m_soilLayers < 0)
        throw ModelException(MID_MINRL, "CheckInputData", "The maximum soil layers number can not be NULL.");
    if (this->m_cellWidth < 0)
        throw ModelException(MID_MINRL, "CheckInputData", "The cell width can not be less than 0.");
    if (this->m_nSoilLayers == NULL)
        throw ModelException(MID_MINRL, "CheckInputData", "The soil layers can not be NULL.");
    if (this->m_cmn <0)
        throw ModelException(MID_MINRL, "CheckInputData", "The m_cmn can not be less than 0.");
    if (this->m_cdn <0)
        throw ModelException(MID_MINRL, "CheckInputData", "The m_cdn can not be less than 0.");
    if (this->m_landcover == NULL)
        throw ModelException(MID_MINRL, "CheckInputData", "The m_landcover can not be NULL.");
    if (this->m_nactfr <0)
        throw ModelException(MID_MINRL, "CheckInputData", "The m_nactfr can not be less than 0.");
	if (this->m_sdnco <0)
		throw ModelException(MID_MINRL, "CheckInputData", "The m_sdnco can not be less than 0.");
    if (this->m_psp <0)
        throw ModelException(MID_MINRL, "CheckInputData", "The m_psp can not be less than 0.");
	if (this->m_sol_clay == NULL)
		throw ModelException(MID_MINRL, "CheckInputData", "The m_sol_clay can not be NULL.");
    if (this->m_sol_z == NULL)
        throw ModelException(MID_MINRL, "CheckInputData", "The m_sol_z can not be NULL.");
	if (this->m_sol_rsdin == NULL)
		throw ModelException(MID_MINRL, "CheckInputData", "The m_sol_rsdin can not be NULL.");
	//if (m_sol_cov == NULL)
	//	throw ModelException(MID_MINRL, "CheckInputData", "The residue on soil surface can not be NULL.");
	//if (m_sol_rsd == NULL)
	//	throw ModelException(MID_MINRL, "CheckInputData", "The organic matter in soil classified as residue can not be NULL.");
	if (this->m_sol_thick == NULL)
		throw ModelException(MID_MINRL, "CheckInputData", "The m_sol_thick can not be NULL.");
	if (this->m_sol_bd == NULL)
		throw ModelException(MID_MINRL, "CheckInputData", "The m_sol_bd can not be NULL.");
    if (this->m_rsdco_pl == NULL)
        throw ModelException(MID_MINRL, "CheckInputData", "The m_rsdco_pl can not be NULL.");
    if (this->m_sol_cbn == NULL)
        throw ModelException(MID_MINRL, "CheckInputData", "The m_sol_cbn can not be NULL.");
    if (this->m_sol_awc == NULL)
        throw ModelException(MID_MINRL, "CheckInputData", "The m_sol_awc can not be NULL.");
    if (this->m_sol_wpmm == NULL)
        throw ModelException(MID_MINRL, "CheckInputData", "The m_sol_wpmm can not be NULL.");
    if (this->m_sol_no3 == NULL)
        throw ModelException(MID_MINRL, "CheckInputData", "The m_sol_no3 can not be NULL.");
    if (this->m_sol_nh3 == NULL)
        throw ModelException(MID_MINRL, "CheckInputData", "The m_sol_nh3 can not be NULL.");
    if (this->m_sol_orgn == NULL)
        throw ModelException(MID_MINRL, "CheckInputData", "The m_sol_orgn can not be NULL.");
    if (this->m_sol_orgp == NULL)
        throw ModelException(MID_MINRL, "CheckInputData", "The m_sol_orgp can not be NULL.");
    if (this->m_sol_solp == NULL)
        throw ModelException(MID_MINRL, "CheckInputData", "The m_sol_solp can not be NULL.");
    if (this->m_somo == NULL)
        throw ModelException(MID_MINRL, "CheckInputData", "The m_somo can not be NULL.");
    if (this->m_sote == NULL)
        throw ModelException(MID_MINRL, "CheckInputData", "The m_sote can not be NULL.");
    return true;
}

void NandPim::SetValue(const char *key, float value)
{
    string sk(key);
    if (StringMatch(sk, VAR_OMP_THREADNUM)) omp_set_num_threads((int) value); 
    else if (StringMatch(sk, Tag_CellWidth)) { this->m_cellWidth = value; }
	else if (StringMatch(sk, VAR_NACTFR)) { this->m_nactfr = value; }
	else if (StringMatch(sk, VAR_SDNCO)) { this->m_sdnco = value; }
    else if (StringMatch(sk, VAR_CMN)) { this->m_cmn = value; }
    else if (StringMatch(sk, VAR_CDN)) { this->m_cdn = value; }
    else if (StringMatch(sk, VAR_PSP)) { this->m_psp = value; }
    else
        throw ModelException(MID_MINRL, "SetValue", "Parameter " + sk +
                                                    " does not exist in CLIMATE method. Please contact the module developer.");
}

void NandPim::Set1DData(const char *key, int n, float *data)
{
    if (!this->CheckInputSize(key, n)) return;
    string sk(key);
    if (StringMatch(sk, VAR_LCC)) { this->m_landcover = data; }
	else if (StringMatch(sk, VAR_PL_RSDCO)) { this->m_rsdco_pl = data; }
	else if (StringMatch(sk, VAR_SOL_RSDIN)) { this->m_sol_rsdin = data; }
	//else if (StringMatch(sk, VAR_SOL_COV)) { this->m_sol_cov = data; }
	else if (StringMatch(sk, VAR_SOILLAYERS)) { this->m_nSoilLayers = data; }
	else if (StringMatch(sk, VAR_SOTE)) { this->m_sote = data; }
    else
        throw ModelException(MID_MINRL, "Set1DData", "Parameter " + sk +
                                                    " does not exist in current module. Please contact the module developer.");
}

void NandPim::Set2DData(const char *key, int nRows, int nCols, float **data)
{
    if (!this->CheckInputSize(key, nRows)) return;
    string sk(key);
    m_soilLayers = nCols;
	if (StringMatch(sk, VAR_SOL_CBN)) { this->m_sol_cbn = data; }
	else if (StringMatch(sk, VAR_SOL_BD)) { this->m_sol_bd = data; }
	else if (StringMatch(sk, VAR_CLAY)) { this->m_sol_clay = data; }
    else if (StringMatch(sk, VAR_SOL_ST)) { this->m_somo = data; }
    else if (StringMatch(sk, VAR_SOL_AWC)) { this->m_sol_awc = data; }
	else if (StringMatch(sk, VAR_SOL_NO3)) { this->m_sol_no3 = data; }
	else if (StringMatch(sk, VAR_SOL_NH3)) { this->m_sol_nh3 = data; }
    else if (StringMatch(sk, VAR_SOL_SORGN)) { this->m_sol_orgn = data; }
    else if (StringMatch(sk, VAR_SOL_HORGP)) { this->m_sol_orgp = data; }
    else if (StringMatch(sk, VAR_SOL_SOLP)) { this->m_sol_solp = data; }
    else if (StringMatch(sk, VAR_SOL_WPMM)) { this->m_sol_wpmm = data; }
	else if (StringMatch(sk, VAR_SOILDEPTH)) { this->m_sol_z = data; }
	else if (StringMatch(sk, VAR_SOILTHICK)) { this->m_sol_thick = data; }
	//else if (StringMatch(sk, VAR_SOL_RSD)) this->m_sol_rsd = data;
    else
        throw ModelException(MID_MINRL, "Set2DData", "Parameter " + sk +
                                                    " does not exist in current module. Please contact the module developer.");
}

void NandPim::initialOutputs()
{
	if(m_hmntl == NULL) Initialize1DArray(m_nCells, m_hmntl, 0.f);
	if(m_hmptl == NULL) Initialize1DArray(m_nCells, m_hmptl, 0.f);
	if(m_rmn2tl == NULL) Initialize1DArray(m_nCells, m_rmn2tl, 0.f);
	if(m_rmptl == NULL) Initialize1DArray(m_nCells, m_rmptl, 0.f);
	if(m_rwntl == NULL) Initialize1DArray(m_nCells, m_rwntl, 0.f);
	if(m_wdntl == NULL) Initialize1DArray(m_nCells, m_wdntl, 0.f);
	if(m_rmp1tl == NULL) Initialize1DArray(m_nCells, m_rmp1tl, 0.f);
	if(m_roctl == NULL) Initialize1DArray(m_nCells, m_roctl, 0.f);
	if(m_sol_cov == NULL || m_sol_rsd == NULL)
	{
		Initialize1DArray(m_nCells, m_sol_cov, m_sol_rsdin);
		Initialize2DArray(m_nCells, m_soilLayers, m_sol_rsd, 0.f);
#pragma omp parallel for
		for (int i = 0; i < m_nCells; i++)
			m_sol_rsd[i][0] = m_sol_cov[i];
	}
	// initial input soil chemical in first run
	if(m_sol_fon == NULL || m_sol_fop == NULL || m_sol_aorgn == NULL || 
		m_sol_actp == NULL || m_sol_stap == NULL) 
	{
		Initialize2DArray(m_nCells, m_soilLayers, m_sol_fon, 0.f);
		Initialize2DArray(m_nCells, m_soilLayers, m_sol_fop, 0.f);
		Initialize2DArray(m_nCells, m_soilLayers, m_sol_aorgn, 0.f);
		Initialize2DArray(m_nCells, m_soilLayers, m_sol_actp, 0.f);
		Initialize2DArray(m_nCells, m_soilLayers, m_sol_stap, 0.f);
				
//#pragma omp parallel for
	// calculate sol_cbn for lower layers if only have upper layer's data
	 for (int i = 0; i < m_nCells; i++)
	 {
		 /// NO NEED. this is already done in preprocess code. By LJ
		 // calculate sol_cbn for lower layers if only have upper layer's data
		 //if(m_nSoilLayers[i] >= 3 && (m_sol_cbn[i][2]) <= 0)
		 //{
			// for (int k = 2; k < (int)m_nSoilLayers[i]; k++)
			//	 m_sol_cbn[i][k] = m_sol_cbn[i - 1][k] * exp(-.001f * m_sol_thick[i][k]);
		 //}
		 
		 // fresh organic P / N
		 m_sol_fop[i][0] = m_sol_cov[i] * .0010f;
		 m_sol_fon[i][0] = m_sol_cov[i] * .0055f;

		 for (int k = 0; k < (int)m_nSoilLayers[i]; k++)
		 {
			float wt1 = 0.f;
			float conv_wt = 0.f;
			// mg/kg => kg/ha
			wt1 = m_sol_bd[i][k] * m_sol_thick[i][k] / 100.f;
			// kg/kg => kg/ha
			conv_wt = 1.e6f * wt1;

			/// if m_sol_no3 is not provided, then initialize it.
			if (m_sol_no3[i][k] <= 0.f) 
			{
				m_sol_no3[i][k] = 0.f;
				float zdst = 0.f;
				zdst = exp(-m_sol_z[i][k] );
				m_sol_no3[i][k] = 10.f * zdst * 0.7f;
				m_sol_no3[i][k] *= wt1;// mg/kg => kg/ha
			}
			/// if m_sol_orgn is not provided, then initialize it.
			if (m_sol_orgn[i][k] <=0.f)
			{
				// CN ratio changed back to 14
				m_sol_orgn[i][k] = 10000.f * (m_sol_cbn[i][k] / 14.f) * wt1;
			}
			// assume C:N ratio of 10:1
			// nitrogen active pool fraction (nactfr)
			float nactfr = .02f;
			m_sol_aorgn[i][k] = m_sol_orgn[i][k] * nactfr;
			m_sol_orgn[i][k] *= (1.f - nactfr);

			// currently not used
			//sumorgn = sumorgn + m_sol_aorgn[i][k] + m_sol_orgn[i][k] + m_sol_fon[i][k];

			// NO NEED. this is already done in preprocess code. By LJ
			//m_sol_orgp[i][k] = m_sol_orgp[i][k] * wt1;
			//m_sol_solp[i][k] = m_sol_solp[i][k] * wt1;

			if (m_sol_orgp[i][k] <= 0.f)
			{
				// assume N:P ratio of 8:1
				m_sol_orgp[i][k] = 0.125f * m_sol_orgn[i][k];
			}
			
			if (m_sol_solp[i][k] <= 0.f) 
			{
				// assume initial concentration of 5 mg/kg
				m_sol_solp[i][k] = 5.f * wt1;
			}

			float solp = 0.f;
			float actp = 0.f;
			// Set active pool based on dynamic PSP MJW
			if (conv_wt != 0)
			{
				solp = (m_sol_solp[i][k] / conv_wt) * 1000000.f;
			}
			
			float psp = 0.f;
			if (m_sol_clay[i][k] > 0.f)
			{
				psp = -0.045f * log(m_sol_clay[i][k]) + (0.001f * solp);
				psp = psp - (0.035f * m_sol_cbn[i][k]) + 0.43f;
			} else
			{
				psp = 0.4f;
			}

			// Limit PSP range
			if (psp < .05f) psp = 0.05f; 
			else if(psp > 0.9f) psp = 0.9f;
			m_sol_actp[i][k] = m_sol_solp[i][k] * (1.f - psp) / psp;

			// Set Stable pool based on dynamic coefficient
			if (conv_wt != 0)
			{
				// convert to concentration for ssp calculation
				actp = m_sol_actp[i][k] / conv_wt * 1000000.f;
				solp = m_sol_solp[i][k] / conv_wt * 1000000.f;
				// estimate Total Mineral P in this soil
				float ssp = 0.;
				ssp = 25.044f * pow((actp + solp), -0.3833f);
				//limit SSP Range
				if (ssp > 7.f) ssp = 7.;
				if (ssp < 1.f) ssp = 1.f;
				// define stableP
				m_sol_stap[i][k] = ssp * (m_sol_actp[i][k] + m_sol_solp[i][k]);
			} else
			{
				// The original code
				m_sol_stap[i][k] = 4.f * m_sol_actp[i][k];
			}
			//m_sol_hum[i][k] = m_sol_cbn[i][k] * wt1 * 17200.f;
			//summinp = summinp + m_sol_solp[i][k] + m_sol_actp[i][k] + m_sol_stap[i][k];
			//sumorgp = sumorgp + m_sol_orgp[i][k] + m_sol_fop[i][k];

			/// NO NEED since m_sol_rsd is initialized as 0.f
			//if(k > 0) m_sol_rsd[i][k] = 0.f;
		 }
	 }
	}
    // allocate the output variables
	if(!FloatEqual(m_wshd_dnit, 0.f))
    {
        m_wshd_dnit = 0.f;
        m_wshd_hmn = 0.f;
        m_wshd_hmp = 0.f;
        m_wshd_rmn = 0.f;
        m_wshd_rmp = 0.f;
        m_wshd_rwn = 0.f;
        m_wshd_nitn = 0.f;
        m_wshd_voln = 0.f;
        m_wshd_pal = 0.f;
        m_wshd_pas = 0.f;
        //m_hmntl = 0.f;
        //m_hmptl = 0.f;
        //m_rmn2tl = 0.f;
        //m_rmptl = 0.f;
        //m_rwntl = 0.f;
        //m_wdntl = 0.f;
        //m_rmp1tl = 0.f;
        //m_roctl = 0.f;
    }
}

int NandPim::Execute()
{
    CheckInputData();
    initialOutputs();
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++)
    {
        //Calculate daily nitrogen and phosphorus mineralization and immobilization
        CalculateMinerandImmobi(i);
        //Calculate daily mineralization (NH3 to NO3) and volatilization of NH3
        CalculateMinerandVolati(i);
        //Calculate P flux between the labile, active mineral and stable mineral P pools
        CalculatePflux(i);
    }
    return 0;
}

void NandPim::CalculateMinerandImmobi(int i)
{
    //soil layer (k)
    for (int k = 0; k < (int)m_nSoilLayers[i]; k++)
    {
        //soil layer used to compute soil water and soil temperature factors
        int kk = 0;
        if (k == 0)
        {
            kk = 1;
        } else
        {
            kk = k;
        }
        float sut = 0.f;
        // mineralization can occur only if temp above 0 deg
        if (m_sote[i] > 0)
        {
            // compute soil water factor (sut)
            sut = 0.f;
            if (m_somo[i][kk] < 0)
            {
                m_somo[i][kk] = 0.0000001f;
            }
            sut = 0.1f + 0.9f * sqrt(m_somo[i][kk] / m_sol_awc[i][kk]);
            sut = max(0.05f, sut);

            //compute soil temperature factor
            //variable to hold intermediate calculation result (xx)
            float xx = 0.f;
            //soil temperature factor (cdg)
            float cdg = 0.f;
            xx = m_sote[i];
            cdg = 0.9f * xx / (xx + exp(9.93f - 0.312f * xx)) + 0.1f;
            cdg = max(0.1f, cdg);

            // compute combined factor
            xx = 0.f;
            //combined temperature/soil water factor (csf)
            float csf = 0.f;
            xx = cdg * sut;
            if (xx < 0)
            {
                xx = 0.f;
            }
            if (xx > 1000000)
            {
                xx = 1000000.f;
            }
            csf = sqrt(xx);

            // compute flow from active to stable pools
            //amount of nitrogen moving from active organic to stable organic pool in layer (rwn)
            float rwn = 0.f;
            rwn = 0.1e-4f * (m_sol_aorgn[i][k] * (1 / m_nactfr - 1.f) - m_sol_orgn[i][k]);
            if (rwn > 0.f)
            {
                rwn = min(rwn, m_sol_aorgn[i][k]);
            } else
            {
                rwn = -(min(abs(rwn), m_sol_orgn[i][k]));
            }
            m_sol_orgn[i][k] = max(1.e-6f, m_sol_orgn[i][k] + rwn);
            m_sol_aorgn[i][k] = max(1.e-6f, m_sol_aorgn[i][k] - rwn);

            // compute humus mineralization on active organic n
            //amount of nitrogen moving from active organic nitrogen pool to nitrate pool in layer (hmn)
            float hmn = 0.f;
            hmn = m_cmn * csf * m_sol_aorgn[i][k];
            hmn = min(hmn, m_sol_aorgn[i][k]);
            // compute humus mineralization on active organic p
            xx = 0.f;
            //amount of phosphorus moving from the organic pool to the labile pool in layer (hmp)
            float hmp = 0.f;
            xx = m_sol_orgn[i][k] + m_sol_aorgn[i][k];
            if (xx > 1e-6f)
            {
                hmp = 1.4f * hmn * m_sol_orgp[i][k] / xx;
            } else
            {
                hmp = 0.f;
            }
            hmp = min(hmp, m_sol_orgp[i][k]);

            // move mineralized nutrients between pools;
            m_sol_aorgn[i][k] = max(1e-6f, m_sol_aorgn[i][k] - hmn);
            m_sol_no3[i][k] = m_sol_no3[i][k] + hmn;
            m_sol_orgp[i][k] = m_sol_orgp[i][k] - hmp;
            m_sol_solp[i][k] = m_sol_solp[i][k] + hmp;

            // compute residue decomp and mineralization of
            // fresh organic n and p (upper two layers only)
            //amount of nitrogen moving from fresh organic to nitrate(80%) and active organic(20%) pools in layer (rmn1)
            float rmn1 = 0.f;
            //amount of phosphorus moving from fresh organic to labile(80%) and organic(20%) pools in layer (rmp)
            float rmp = 0.f;
            if (k <= 2)
            {
                //the C:N ratio (cnr)
                float cnr = 0.f;
                //the C:P ratio (cnr)
                float cpr = 0.f;
                //the nutrient cycling water factor for layer (ca)
                float ca = 0.f;
                float cnrf = 0.f;
                float cprf = 0.f;
                if (m_sol_fon[i][k] + m_sol_no3[i][k] > 1e-4f)
                {
                    //Calculate cnr, equation 3:1.2.5 in SWAT Theory 2009, p189
                    cnr = 0.58f * m_sol_rsd[i][k] / (m_sol_fon[i][k] + m_sol_no3[i][k]);
                    if (cnr > 500)
                    {
                        cnr = 500.f;
                    }
                    cnrf = exp(-0.693f * (cnr - 25.f) / 25.f);
                } else
                {
                    cnrf = 1.f;
                }
                if (m_sol_fop[i][k] + m_sol_solp[i][k] > 1e-4f)
                {
                    cpr = 0.58f * m_sol_rsd[i][k] / (m_sol_fop[i][k] + m_sol_solp[i][k]);
                    if (cpr > 5000)
                    {
                        cpr = 5000.f;
                    }
                    cprf = 0.f;
                    cprf = exp(-0.693f * (cpr - 200.f) / 200.f);
                } else
                {
                    cprf = 1.f;
                }
                //decay rate constant (decr)
                float decr = 0.f;

                float rdc = 0.f;
                //Calculate ca, equation 3:1.2.8 in SWAT Theory 2009, p190
                ca = min(min(cnrf, cprf), 1.f);
                //if (m_landcover[i] > 0)
                //{
                //    decr = m_rsdco_pl[(int) m_landcover[i]] * ca * csf;
                //} else
                //{
                //    decr = 0.05f;
                //}
                if (m_rsdco_pl[i] < 0.f)
					decr = 0.05f;
				else
					decr = m_rsdco_pl[i] * ca * csf;
                decr = min(decr, 1.f);
                m_sol_rsd[i][k] = max(1.e-6f, m_sol_rsd[i][k]);
                rdc = decr * m_sol_rsd[i][k];
                m_sol_rsd[i][k] = m_sol_rsd[i][k] - rdc;
                if (m_sol_rsd[i][k] < 0)m_sol_rsd[i][k] = 0.f;
                rmn1 = decr * m_sol_fon[i][k];
                m_sol_fop[i][k] = max(1e-6f, m_sol_fop[i][k]);
                rmp = decr * m_sol_fop[i][k];

                m_sol_fop[i][k] = m_sol_fop[i][k] - rmp;
                m_sol_fon[i][k] = max(1e-6f, m_sol_fon[i][k]) - rmn1;;
                //Calculate no3, aorgn, solp, orgp, equation 3:1.2.9 in SWAT Theory 2009, p190
                m_sol_no3[i][k] = m_sol_no3[i][k] + 0.8f * rmn1;
                m_sol_aorgn[i][k] = m_sol_aorgn[i][k] + 0.2f * rmn1;
                m_sol_solp[i][k] = m_sol_solp[i][k] + 0.8f * rmp;
                m_sol_orgp[i][k] = m_sol_orgp[i][k] + 0.2f * rmp;
            }
            //  compute denitrification
            //amount of nitrogen lost from nitrate pool in layer due to denitrification
            float wdn = 0.f;
            //Calculate wdn, equation 3:1.4.1 and 3:1.4.2 in SWAT Theory 2009, p194
            if (sut >= m_sdnco)
            {
                wdn = m_sol_no3[i][k] * (1.f - exp(-m_cdn * cdg * m_sol_cbn[i][k]));
            } else
            {
                wdn = 0.f;
            }
            m_sol_no3[i][k] = m_sol_no3[i][k] - wdn;

            // Summary calculations
            m_wshd_hmn = m_wshd_hmn + hmn * (1.f / m_nCells);
            m_wshd_rwn = m_wshd_rwn + rwn * (1.f / m_nCells);
            m_wshd_hmp = m_wshd_hmp + hmp * (1.f / m_nCells);
            m_wshd_rmn = m_wshd_rmn + rmn1 * (1.f / m_nCells);
            m_wshd_rmp = m_wshd_rmp + rmp * (1.f / m_nCells);
            m_wshd_dnit = m_wshd_dnit + wdn * (1.f / m_nCells);
            //m_hmntl = m_hmntl + hmn;
            //m_rwntl = m_rwntl + rwn;
            //m_hmptl = m_hmptl + hmp;
            //m_rmn2tl = m_rmn2tl + rmn1;
            //m_rmptl = m_rmptl + rmp;
            //m_wdntl = m_wdntl + wdn;
			m_hmntl[i] = hmn;
			m_rwntl[i] = rwn;
			m_hmptl[i] = hmp;
			m_rmn2tl[i] = rmn1;
			m_rmptl[i] = rmp;
			m_wdntl[i] = wdn;
        }
    }
}

void NandPim::CalculateMinerandVolati(int i)
{
    //soil layer (k)
    float kk = 0.f;
    for (int k = 0; k < (int)m_nSoilLayers[i]; k++)
    {
        //nitrification/volatilization temperature factor (nvtf)
        float nvtf = 0.f;
        //Calculate nvtf, equation 3:1.3.1 in SWAT Theory 2009, p192
        nvtf = 0.41f * (m_sote[i] - 5.f) / 10.f;
        if (m_sol_nh3[i][k] > 0.f && nvtf >= 0.001f)
        {
            float sw25 = 0.f;
            float swwp = 0.f;
            //nitrification soil water factor (swf)
            float swf = 0.f;
            //Calculate nvtf, equation 3:1.3.2 and 3:1.3.3 in SWAT Theory 2009, p192
            sw25 = m_sol_wpmm[i][k] + 0.25f * m_sol_awc[i][k];
            swwp = m_sol_wpmm[i][k] + m_somo[i][k];
            if (swwp < sw25)
            {
                swf = (swwp - m_sol_wpmm[i][k]) / (sw25 - m_sol_wpmm[i][k]);
            } else
            {
                swf = 1.f;
            }

            if (k == 0)
            {
                kk = 0.f;
            } else
            {
                kk = m_sol_z[k - 1][i];
            }
            //depth from the soil surface to the middle of the layer (dmidl/mm)
            float dmidl = 0.f;
            //volatilization depth factor (dpf)
            float dpf = 0.f;
            //nitrification regulator (akn)
            float akn = 0.f;
            //volatilization regulator (akn)
            float akv = 0.f;
            //amount of ammonium converted via nitrification and volatilization in layer (rnv)
            float rnv = 0.f;
            //amount of nitrogen moving from the NH3 to the NO3 pool (nitrification) in the layer (rnit)
            float rnit = 0.f;
            //amount of nitrogen lost from the NH3 pool due to volatilization (rvol)
            float rvol = 0.f;
            //volatilization CEC factor (cecf)
            float cecf = 0.15f;
            //Calculate nvtf, equation 3:1.3.2 and 3:1.3.3 in SWAT Theory 2009, p192
            dmidl = (m_sol_z[i][k] + kk) / 2.f;
            dpf = 1.f - dmidl / (dmidl + exp(4.706f - 0.0305f * dmidl));
            //Calculate rnv, equation 3:1.3.6, 3:1.3.7 and 3:1.3.8 in SWAT Theory 2009, p193
            akn = nvtf * swf;
            akv = nvtf * dpf * cecf;
            rnv = m_sol_nh3[i][k] * (1.f - exp(-akn - akv));
            //Calculate rnit, equation 3:1.3.9 in SWAT Theory 2009, p193
            rnit = 1.f - exp(-akn);
            //Calculate rvol, equation 3:1.3.10 in SWAT Theory 2009, p193
            rvol = 1.f - exp(-akv);

            //calculate nitrification (NH3 => NO3)
            if ((rvol + rnit) > 1e-6f)
            {
                //Calculate the amount of nitrogen removed from the ammonium pool by nitrification,
                //equation 3:1.3.11 in SWAT Theory 2009, p193
                rvol = rnv * rvol / (rvol + rnit);
                //Calculate the amount of nitrogen removed from the ammonium pool by volatilization,
                //equation 3:1.3.12 in SWAT Theory 2009, p194
                rnit = rnv - rvol;
                if (rnit < 0)rnit = 0.f;
                m_sol_nh3[i][k] = max(1e-6f, m_sol_nh3[i][k] - rnit);
            }
            if (m_sol_nh3[i][k] < 0)
            {
                rnit = rnit + m_sol_nh3[i][k];
                m_sol_nh3[i][k] = 0.f;
            }
            m_sol_no3[i][k] = m_sol_no3[i][k] + rnit;
            //calculate ammonia volatilization
            m_sol_nh3[i][k] = max(1e-6f, m_sol_nh3[i][k] - rvol);
            if (m_sol_nh3[i][k] < 0)
            {
                rvol = rvol + m_sol_nh3[i][k];
                m_sol_nh3[i][k] = 0.f;
            }
            //summary calculations
            m_wshd_voln = m_wshd_voln + rvol * (1.f / m_nCells);
            m_wshd_nitn = m_wshd_nitn + rnit * (1.f / m_nCells);
        }
    }
}

void NandPim::CalculatePflux(int i)
{
    //slow equilibration rate constant (bk)
    float bk = 0.0006f;
    float rto = m_psp / (1 - m_psp);
    for (int k = 0; k < (int)m_nSoilLayers[i]; k++)
    {
        float rmn1 = 0.f;
        rmn1 = (m_sol_solp[i][k] - m_sol_actp[i][k] * rto);
        if (rmn1 > 0.f) rmn1 *= 0.1f;
        if (rmn1 < 0.f)rmn1 *= 0.6f;
        rmn1 = min(rmn1, m_sol_solp[i][k]);
        //amount of phosphorus moving from the active mineral to the stable mineral pool in the soil layer (roc)
        float roc = 0;
        //Calculate roc, equation 3:2.3.4 and 3:2.3.5 in SWAT Theory 2009, p215
        roc = bk * (4.f * m_sol_actp[i][k] - m_sol_stap[i][k]);
        if (roc < 0)roc *= 0.1f;
        roc = min(roc, m_sol_actp[i][k]);
        m_sol_stap[i][k] = m_sol_stap[i][k] + roc;
        if (m_sol_stap[i][k] < 0)m_sol_stap[i][k] = 0.f;
        m_sol_actp[i][k] = m_sol_actp[i][k] - roc + rmn1;
        if (m_sol_actp[i][k] < 0)m_sol_actp[i][k] = 0.f;
        m_sol_solp[i][k] = m_sol_solp[i][k] - rmn1;
        if (m_sol_solp[i][k] < 0)m_sol_solp[i][k] = 0.f;
        m_wshd_pas += roc * (1.f / m_nCells);
        m_wshd_pal += rmn1 * (1.f / m_nCells);
        m_roctl[i] += roc;
        m_rmp1tl[i] += rmn1;
    }
}

void NandPim::GetValue(const char *key, float *value)
{
    string sk(key);
    //if (StringMatch(sk, VAR_HMNTL)) { *value = this->m_hmntl; }
    //else if (StringMatch(sk, VAR_HMPTL)) { *value = this->m_hmptl; }
    //else if (StringMatch(sk, VAR_RMN2TL)) { *value = this->m_rmn2tl; }
    //else if (StringMatch(sk, VAR_RMPTL)) { *value = this->m_rmptl; }
    //else if (StringMatch(sk, VAR_RWNTL)) { *value = this->m_rwntl; }
    //else if (StringMatch(sk, VAR_WDNTL)) { *value = this->m_wdntl; }
    //else if (StringMatch(sk, VAR_RMP1TL)) { *value = this->m_rmp1tl; }
    //else if (StringMatch(sk, VAR_ROCTL)) { *value = this->m_roctl; }
    if (StringMatch(sk, VAR_WSHD_DNIT)) { *value = this->m_wshd_dnit; }
    else if (StringMatch(sk, VAR_WSHD_HMN)) { *value = this->m_wshd_hmn; }
    else if (StringMatch(sk, VAR_WSHD_HMP)) { *value = this->m_wshd_hmp; }
    else if (StringMatch(sk, VAR_WSHD_RMN)) { *value = this->m_wshd_rmn; }
    else if (StringMatch(sk, VAR_WSHD_RMP)) { *value = this->m_wshd_rmp; }
    else if (StringMatch(sk, VAR_WSHD_RWN)) { *value = this->m_wshd_rwn; }
    else if (StringMatch(sk, VAR_WSHD_NITN)) { *value = this->m_wshd_nitn; }
    else if (StringMatch(sk, VAR_WSHD_VOLN)) { *value = this->m_wshd_voln; }
    else if (StringMatch(sk, VAR_WSHD_PAL)) { *value = this->m_wshd_pal; }
    else if (StringMatch(sk, VAR_WSHD_PAS)) { *value = this->m_wshd_pas; }
    else
        throw ModelException(MID_MINRL, "GetValue",
                             "Parameter " + sk + " does not exist. Please contact the module developer.");
}

void NandPim::Get1DData(const char *key, int *n, float **data)
{
	initialOutputs();
	string sk(key);
	if (StringMatch(sk, VAR_HMNTL)) { *data = this->m_hmntl; }
	else if (StringMatch(sk, VAR_HMPTL)) { *data = this->m_hmptl; }
	else if (StringMatch(sk, VAR_RMN2TL)) { *data = this->m_rmn2tl; }
	else if (StringMatch(sk, VAR_RMPTL)) { *data = this->m_rmptl; }
	else if (StringMatch(sk, VAR_RWNTL)) { *data = this->m_rwntl; }
	else if (StringMatch(sk, VAR_WDNTL)) { *data = this->m_wdntl; }
	else if (StringMatch(sk, VAR_RMP1TL)) { *data = this->m_rmp1tl; }
	else if (StringMatch(sk, VAR_ROCTL)) { *data = this->m_roctl; }
	else if (StringMatch(sk, VAR_SOL_COV)){*data = this->m_sol_cov;}
	else
		throw ModelException(MID_MINRL, "Get1DData", "Output " + sk +
		" does not exist in current module. Please contact the module developer.");
	*n = this->m_nCells;
}

void NandPim::Get2DData(const char *key, int *nRows, int *nCols, float ***data)
{
	initialOutputs();
    string sk(key);
    *nRows = m_nCells;
    *nCols = m_soilLayers;
    if (StringMatch(sk, VAR_SOL_AORGN)) { *data = this->m_sol_aorgn; }
    else if (StringMatch(sk, VAR_SOL_FON)) { *data = this->m_sol_fon; }
    else if (StringMatch(sk, VAR_SOL_FOP)) { *data = this->m_sol_fop; }
    else if (StringMatch(sk, VAR_SOL_NO3)) { *data = this->m_sol_no3; }
    else if (StringMatch(sk, VAR_SOL_NH3)) { *data = this->m_sol_nh3; }
    else if (StringMatch(sk, VAR_SOL_SORGN)) { *data = this->m_sol_orgn; }
    else if (StringMatch(sk, VAR_SOL_HORGP)) { *data = this->m_sol_orgp; }
    else if (StringMatch(sk, VAR_SOL_SOLP)) { *data = this->m_sol_solp; }
    else if (StringMatch(sk, VAR_SOL_ACTP)) { *data = this->m_sol_actp; }
    else if (StringMatch(sk, VAR_SOL_STAP)) { *data = this->m_sol_stap; }
	else if (StringMatch(sk, VAR_SOL_RSD)) {*data = this->m_sol_rsd;}
    else
        throw ModelException(MID_MINRL, "Get2DData", "Output " + sk +
                                                     " does not exist in the current module. Please contact the module developer.");
}
