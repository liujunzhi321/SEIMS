/*//
 * \file NutrientRemviaSr.cpp
 * \ingroup NutRemv
 * \author Huiran Gao
 * \date May 2016
 */

#include <iostream>
#include "NutrientRemviaSr.h"
#include "MetadataInfo.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include "ModelException.h"
#include "util.h"
#include <omp.h>

using namespace std;

NutrientRemviaSr::NutrientRemviaSr(void):
	//input 
	m_nCells(-1), m_cellWidth(-1), m_soiLayers(-1),  m_sedimentYield(NULL), m_nperco(-1), m_phoskd(-1), m_pperco(-1), m_qtile(-1), 
	m_nSoilLayers(NULL), m_anion_excl(NULL), m_isep_opt(NULL), m_ldrain(NULL), m_surfr(NULL), m_flat(NULL),
	m_sol_perco(NULL), m_sol_wsatur(NULL), m_sol_crk(NULL), m_sol_bd(NULL), m_sol_z(NULL), m_sol_depth(NULL),
	 m_sol_om(NULL), m_gw_q(NULL),
	//output 
	m_latno3(NULL), m_percn(NULL), m_surqno3(NULL), m_sol_no3(NULL), m_surqsolp(NULL), m_wshd_plch(NULL), m_sol_solp(NULL),
	m_cod(NULL), m_chl_a(NULL) //,m_doxq(), m_soxy()
{

}

NutrientRemviaSr::~NutrientRemviaSr(void)
{
    ///TODO
}

bool NutrientRemviaSr::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
    {
        throw ModelException(MID_NutRemv, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
        return false;
    }
    if (m_nCells != n)
    {
        if (m_nCells <= 0)
        {
            m_nCells = n;
        } else
        {
            //StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
            ostringstream oss;
            oss << "Input data for " + string(key) << " is invalid with size: " << n << ". The origin size is " <<
            m_nCells << ".\n";
            throw ModelException(MID_NutRemv, "CheckInputSize", oss.str());
        }
    }
    return true;
}
bool NutrientRemviaSr::CheckInputData() {
	if(this -> m_nCells <= 0) {throw ModelException(MID_NutRemv, "CheckInputData","The input data can not be less than zero.");return false;}
	if(this ->m_cellWidth < 0) {throw ModelException(MID_NutRemv, "CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_soiLayers < 0) {throw ModelException(MID_NutRemv, "CheckInputData", "The input data can not be NULL.");return false;}
	if(this -> m_nSoilLayers == NULL) {throw ModelException(MID_NutRemv, "CheckInputData", "The data can not be NULL.");return false;}
	if(this -> m_sedimentYield == NULL) {throw ModelException(MID_NutRemv, "CheckInputData", "The input data can not be NULL.");return false;}
	if(this -> m_sol_om == NULL) {throw ModelException(MID_NutRemv, "CheckInputData", "The data can not be NULL.");return false;}
	if(this -> m_anion_excl == NULL) {throw ModelException(MID_NutRemv, "CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_isep_opt == NULL) {throw ModelException(MID_NutRemv, "CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_ldrain == NULL) {throw ModelException(MID_NutRemv, "CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_surfr == NULL) {throw ModelException(MID_NutRemv, "CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_nperco == NULL) {throw ModelException(MID_NutRemv, "CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_flat == NULL) {throw ModelException(MID_NutRemv, "CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_sol_perco == NULL) {throw ModelException(MID_NutRemv, "CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_sol_wsatur == NULL) {throw ModelException(MID_NutRemv, "CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_phoskd == NULL) {throw ModelException(MID_NutRemv, "CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_sol_crk == NULL) {throw ModelException(MID_NutRemv, "CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_pperco == NULL) {throw ModelException(MID_NutRemv, "CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_sol_bd == NULL) {throw ModelException(MID_NutRemv, "CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_sol_z == NULL) {throw ModelException(MID_NutRemv, "CheckInputData","The input data can not be NULL.");return false;}
	if(this -> m_sol_depth == NULL) {throw ModelException(MID_NutRemv, "CheckInputData","The input data can not be NULL.");return false;}
	if(this->m_gw_q == NULL) {throw ModelException("NutGW", "CheckInputData", "The input data can not be NULL.");return false;}
	return true;
}

void NutrientRemviaSr::SetValue(const char *key, float value)
{
    string sk(key);
    if (StringMatch(sk, VAR_OMP_THREADNUM))
    {
        omp_set_num_threads((int) value);
    }
    else if (StringMatch(sk, Tag_CellSize)) { this->m_nCells = value; }
    else if (StringMatch(sk, Tag_CellWidth)) { this->m_cellWidth = value; }
    else if (StringMatch(sk, VAR_QTILE)) { this->m_qtile = value; }
    else if (StringMatch(sk, VAR_NPERCO)) { this->m_nperco = value; }
    else if (StringMatch(sk, VAR_PPERCO)) { this->m_pperco = value; }
    else if (StringMatch(sk, VAR_PHOSKD)) { this->m_phoskd = value; }
    else if (StringMatch(sk, VAR_ISEP_OPT)) { this->m_isep_opt = value; }
    else
    {
        throw ModelException("NutRemv", "SetValue", "Parameter " + sk +
                                                    " does not exist in CLIMATE method. Please contact the module developer.");
    }
}

void NutrientRemviaSr::Set1DData(const char *key, int n, float *data)
{
	if(!this->CheckInputSize(key,n)) return;
	string sk(key);
	if (StringMatch(sk, VAR_SURU)) {this -> m_surfr = data;}
	else if (StringMatch(sk, VAR_ANION_EXCL)) {this -> m_anion_excl = data;}
	else if (StringMatch(sk, VAR_LDRAIN)) {this -> m_ldrain = data;}
	else if (StringMatch(sk, VAR_SOL_CRK)) {this -> m_sol_crk = data;}
	else if (StringMatch(sk, VAR_SOL_SUMSAT)) {this -> m_sol_wsatur = data;}
	else if (StringMatch(sk, VAR_SOER)) {this -> m_sedimentYield = data;}
	else if (StringMatch(sk, VAR_SEDORGN)) {this -> m_sedorgn = data;}
	else if (StringMatch(sk, VAR_SEDORGP)) {this -> m_sedorgp = data;}
	else if (StringMatch(sk, VAR_TMEAN)) {this -> m_tmean = data;}
	else if (StringMatch(sk, VAR_GW_Q)) {this -> m_gw_q = data;}
	else {
		throw ModelException("NutRemv","SetValue","Parameter " + sk + " does not exist in CLIMATE module. Please contact the module developer.");
	}
}

void NutrientRemviaSr::Set2DData(const char *key, int nRows, int nCols, float **data)
{
	if(!this->CheckInputSize(key,nCols)) return;
	string sk(key);
	m_soiLayers = nCols;
	if (StringMatch(sk, VAR_FLAT)) {this -> m_flat = data;}
	else if (StringMatch(sk, VAR_SOL_NO3)) {this -> m_sol_no3 = data;}
	else if (StringMatch(sk, VAR_SOL_BD)) {this -> m_sol_bd = data;}
	else if (StringMatch(sk, VAR_ROOTDEPTH)) {this -> m_sol_z = data;}
	else if (StringMatch(sk, VAR_SOL_SOLP)) {this -> m_sol_solp = data;}
	else if (StringMatch(sk, VAR_SOILDEPTH)) {this -> m_sol_depth = data;}
	else if (StringMatch(sk, VAR_SOL_PERCO)) {this -> m_sol_perco = data;}
	else if (StringMatch(sk, VAR_SOL_OM)) {this -> m_sol_om = data;}
	else {
		throw ModelException("NutRemv","SetValue","Parameter " + sk + " does not exist in CLIMATE module. Please contact the module developer.");
	}
}
void NutrientRemviaSr::initialOutputs() {
	if(this->m_nCells <= 0) {
		throw ModelException("NutRemv", "CheckInputData", "The dimension of the input data can not be less than zero.");
	}
	// allocate the output variables
	if(m_latno3 == NULL) {
		for(int i=0; i < m_nCells; i++) {
			m_latno3[i] = 0.;
			m_percn[i] = 0.;
			m_surqno3[i] = 0.;
			m_surqsolp[i] = 0.;
		}
	}
	if(m_cod == NULL) {
		for(int i = 0; i < m_nCells; i++) {
			m_cod[i] = 0.;
			m_chl_a[i] = 0.;
			//m_doxq[i] = 0.;
			//m_soxy[i] = 0.;
		}
	}
	if(m_wshd_plch < 0) {
		m_wshd_plch = 0.;
	}
	// input variables
	if(m_flat == NULL) {Initialize2DArray(m_nCells, m_soiLayers, m_flat, 0.0001f);}
	if(m_sol_perco == NULL) {Initialize2DArray(m_nCells, m_soiLayers, m_sol_perco, 0.0001f);}
	if(m_ldrain == NULL) {Initialize1DArray(m_nCells, m_ldrain, -1.f);}
	m_qtile = 0.0001;

}

int NutrientRemviaSr::Execute()
{
    if (!this->CheckInputData())
    {
        return false;
    }
    this->initialOutputs();
    // Calculate total no3
    float total_no3 = 0.;
    for (int i = 0; i < m_nCells; i++)
    {
        for (int k = 0; k < m_nSoilLayers[i]; k++)
        {
            total_no3 += m_sol_no3[i][k];
        }
    }
    //Calculate the loss of nitrate via surface runoff, lateral flow, tile flow, and percolation out of the profile.
    Nitrateloss();
    // Calculates the amount of phosphorus lost from the soil.
    Phosphorusloss();
    //return ??
    return 0;
}


void NutrientRemviaSr::Nitrateloss(){
	//percnlyr nitrate moved to next lower layer with percolation (kg/km2)
	float percnlyr = 0.;
	float* tileno3;
	#pragma omp parallel for
	for(int i = 0; i < m_nCells; i++) {
		for(int k = 0; k < m_nSoilLayers[i]; k++) {
			//add nitrate moved from layer above
			m_sol_no3[i][k] = m_sol_no3[i][k] + percnlyr;
			if (m_sol_no3[i][k] < 1e-6) {
				m_sol_no3[i][k] = 0.0;
			}
			// determine concentration of nitrate in mobile water
			// surface runoff generated (sro)
			float sro = 0.;
			// amount of mobile water in the layer (mw)
			float mw = 0.;
			float vno3 = 0.;
			float con = 0.;
			float ww = 0.;


void NutrientRemviaSr::Nitrateloss()
{
    //percnlyr nitrate moved to next lower layer with percolation (kg/km2)
    float percnlyr = 0.;
    float *tileno3;
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++)
    {
        for (int k = 0; k < m_nSoilLayers[i]; k++)
        {
            //add nitrate moved from layer above
            m_sol_no3[i][k] = m_sol_no3[i][k] + percnlyr;
            if (m_sol_no3[i][k] < 1e-6)
            {
                m_sol_no3[i][k] = 0.0;
            }
            // determine concentration of nitrate in mobile water
            // surface runoff generated (sro)
            float sro = 0.;
            // amount of mobile water in the layer (mw)
            float mw = 0.;
            float vno3 = 0.;
            float con = 0.;
            float ww = 0.;

            if (k == 0)
            {
                sro = m_surfr[i];
            } else
            {
                sro = 0.;
            }
            if (m_ldrain[i] == k)
            {
                mw = mw + m_qtile;
            }
            // Calculate the concentration of nitrate in the mobile water (con),
            // equation 4:2.1.2, 4:2.1.3 and 4:2.1.4 in SWAT Theory 2009, p269
            mw = m_sol_perco[i][k] + sro + m_flat[i][k] + 1.e-10;
            ww = -mw / ((1. - m_anion_excl[i]) * m_sol_wsatur[i]);
            vno3 = m_sol_no3[i][k] * (1. - exp(ww));
            if (mw > 1.e-10)
            {
                con = max(vno3 / mw, 0.f);
            }

            // calculate nitrate in surface runoff
            // concentration of nitrate in surface runoff (cosurf)
            float cosurf = 0.;
            if (m_isep_opt == 2)
            {
                cosurf = 1.0 * con; // N percolation does not apply to failing septic place;
            } else
            {
                cosurf = m_nperco * con;
            }
            if (k == 0)
            {
                m_surqno3[i] = m_surfr[i] * cosurf;
                m_surqno3[i] = min(m_surqno3[i], m_sol_no3[i][k]);
                m_sol_no3[i][k] = m_sol_no3[i][k] - m_surqno3[i];
            }

            // calculate nitrate in tile flow
            if (m_ldrain[i] == k)
            {
                // m_alph_e[i] = exp(-1./(m_n_lag[i] + 1.e-6))
                // ww1 = -1./ ((1. - m_anion_excl[i]) * m_sol_wsatur[i])
                // m_vno3_c = m_sol_no3[i][k] * (1. - exp(ww1))
                // if (total_no3 > 1.001) {
                //	 tno3ln = n_lnco[i] * (log(total_no3)) ** m_n_ln[i]
                // else
                //	 tno3ln = 0.
                // }
                // vno3_c = tno3ln * (1. - Exp(ww1))
                // co_p[i] = co_p[i] * (1. - alph_e[i]) + vno3_c * alph_e[i]
                tileno3[i] = con * m_qtile;
                tileno3[i] = min(tileno3[i], m_sol_no3[i][k]);
                m_sol_no3[i][k] = m_sol_no3[i][k] - tileno3[i];
            }
            // calculate nitrate in lateral flow
            // nitrate transported in lateral flow from layer (ssfnlyr)
            float ssfnlyr = 0.;
            if (k == 1)
            {
                ssfnlyr = cosurf * m_flat[i][k];
            } else
            {
                ssfnlyr = con * m_flat[i][k];
            }
            ssfnlyr = min(ssfnlyr, m_sol_no3[i][k]);
            m_latno3[i] = m_latno3[i] + ssfnlyr;
            m_sol_no3[i][k] = m_sol_no3[i][k] - ssfnlyr;

            // calculate nitrate in percolate
            float percnlyr = 0.;
            percnlyr = con * m_sol_perco[i][k];
            percnlyr = min(percnlyr, m_sol_no3[i][k]);
            m_sol_no3[i][k] = m_sol_no3[i][k] - percnlyr;
        }


		// calculate nitrate leaching from soil profile
		m_percn[i] = percnlyr;
		float nloss = 0.;
		// average distance to the stream(m), default is 35m.
		float dis_stream = 35.0;
		nloss = (2.18 * dis_stream - 8.63) / 100.;
		nloss = max(0.,nloss);
		nloss = min(1.,nloss);
		m_latno3[i] = (1. - nloss) * m_latno3[i];

		// calculate CBOD, COD, Chl_a, doxq and soxy
		// calculcate water temperature
		// SWAT manual 2.3.13
		float wtmp = 0.;
		wtmp = 5.0 + 0.75 * m_tmean[i];
		if (wtmp <= 0.1)  {
			wtmp = 0.1;
		}
		wtmp = wtmp + 273.15;    // deg C to deg K
		// water in cell
		float qdr = 0.;
		qdr = m_surfr[i] + m_flat[i][0] + m_qtile + m_gw_q[i];
		if (qdr > 1.e-4) {
			// kilo moles of phosphorus in nutrient loading to main channel (tp)
			float tp = 0.;
			tp = 100. * (m_sedorgn[i] + m_surqno3[i]) / qdr;   //100*kg/ha/mm = ppm 
			// regional adjustment on sub chla_a loading
			float chla_subco = 40.;
			m_chl_a[i] = chla_subco * tp;
			m_chl_a[i] =m_chl_a[i] / 1000;  // um/L to mg/L

			// calculate enrichment ratio
			float enratio;
			if (m_sedimentYield[i] < 1e-4) {
				m_sedimentYield[i] = 0;
			}
			// CREAMS method for calculating enrichment ratio
			float cy = 0;
			// Calculate sediment calculations, equation 4:2.2.3 in SWAT Theory 2009, p272
			cy = 0.1 * m_sedimentYield[i] / (m_cellWidth * m_cellWidth * 0.0001 * m_surfr[i] + 1e-6);
			if (cy > 1e-6) {
				enratio = 0.78 * pow(cy, -0.2468f);
			} else {
				enratio = 0;
			}
			if (enratio > 3.5) {
				enratio = 3.5;
			}

			// calculate organic carbon loading to main channel
			float org_c = 0.;
			org_c = (m_sol_om[i][0] * 0.58  / 100.) * enratio * m_sedimentYield[i] * 1000.;
			// calculate carbonaceous biological oxygen demand (CBOD) and COD(transform from CBOD)
			float cbod = 0.;
			cbod = 2.7 * org_c / (qdr * m_cellWidth * m_cellWidth);
			// calculate COD
			float n = 3.; // Conversion factor 1~6.5  
			float k = 0.15; // Reaction coefficient 0.1~0.2  
			m_cod[i] = n *(cbod * (1. - exp(-5. * k)));

			/*
			// calculate dissolved oxygen saturation concentration (soxy)
			float soxy = 0.;
			float ww = 0.;
			float xx = 0.;
			float yy = 0.;
			float zz = 0.;
			ww = -139.34410 + (1.575701E05 / wtmp);
			xx = 6.642308E07 /pow (wtmp, 2);
			yy = 1.243800E10 /pow (wtmp, 3);
			zz = 8.621949E11 / pow(wtmp, 4);
			m_soxy[i] = exp(ww - xx + yy - zz);
			if (soxy < 0.) {
				soxy = 0.;
			}
			// calculate actual dissolved oxygen concentration
			m_doxq[i] = soxy * exp(-0.1 * cbod);
			if (m_doxq[i] < 0.) {
				m_doxq[i] = 0.;
			}
			if (m_doxq[i] > soxy) {
				m_doxq[i] = soxy;
			}
			*/
		} else {
			m_chl_a[i] = 0.;
			m_cod[i] = 0.;
			//m_doxq[i] = 0.;
		}
	}
}

void NutrientRemviaSr::Phosphorusloss()
{

    for (int i = 0; i < m_nCells; i++)
    {
        // mg/kg => kg/ha
        float *sol_thick = new float(m_nSoilLayers[i]);
        sol_thick[0] = m_sol_depth[i][0];
        for (int k = 1; k < m_nSoilLayers[i]; k++)
        {
            sol_thick[k] = m_sol_depth[k] - m_sol_depth[k - 1];
        }
        float wt1 = m_sol_bd[i][0] * sol_thick[0] / 100.;
        float conv_wt = 1.e6 * wt1;

        // amount of P leached from soil layer (vap)
        float vap = 0.;
        float vap_tile = 0.;
        // compute soluble P lost in surface runoff
        float xx = 0.;  // variable to hold intermediate calculation result
        xx = m_sol_bd[i][0] * m_sol_z[i][0] * m_phoskd;
        m_surqsolp[i] = m_sol_solp[i][0] * m_surfr[i] / xx;
        m_surqsolp[i] = min(m_surqsolp[i], m_sol_solp[i][0]);
        m_surqsolp[i] = max(m_surqsolp[i], 0.f);
        m_sol_solp[i][0] = m_sol_solp[i][0] - m_surqsolp[i];

        // compute soluble P leaching
        vap = m_sol_solp[i][0] * m_sol_perco[i][0] / ((conv_wt / 1000.) * m_pperco);
        vap = min(vap, 0.5f * m_sol_solp[i][0]);
        m_sol_solp[i][0] = m_sol_solp[i][0] - vap;

        // estimate soluble p in tiles due to crack flow
        if (m_ldrain[i] > 0)
        {
            xx = min(1.f, m_sol_crk[i] / 3.f);
            vap_tile = xx * vap;
            vap = vap - vap_tile;
        }
        if (m_nSoilLayers[i] >= 1)
        {
            m_sol_solp[i][1] = m_sol_solp[i][1] + vap;
        }
        for (int k = 1; k < m_nSoilLayers[i]; k++)
        {
            vap = 0.;
            //if (k != m_i_sep[i]) {  // soil layer where biozone exists (m_i_sep)
            vap = m_sol_solp[i][k] * m_sol_perco[i][k] / ((conv_wt / 1000.) * m_pperco);
            vap = min(vap, .2f * m_sol_solp[i][k]);
            m_sol_solp[i][k] = m_sol_solp[i][k] - vap;
            //}
        }
        //m_percp[i] = vap
        // summary calculation
        m_wshd_plch = m_wshd_plch + vap * (1 / m_nCells);
    }
}

void NutrientRemviaSr::GetValue(const char *key, float *value)
{
    string sk(key);
    if (StringMatch(sk, VAR_WSHD_PLCH))
    {
        *value = this->m_wshd_plch;
    }
}

void NutrientRemviaSr::Get1DData(const char* key, int* n, float** data) {
	string sk(key);
	*n = m_nCells;
	if (StringMatch(sk, VAR_LATNO3)) {*data = this -> m_latno3;}
	if (StringMatch(sk, VAR_PERCN)) {*data = this -> m_percn;}
	if (StringMatch(sk, VAR_SURQNO3)) {*data = this -> m_surqno3;}
	if (StringMatch(sk, VAR_SURQSOLP)) {*data = this -> m_surqsolp;}
	if (StringMatch(sk, VAR_COD)) {*data = this -> m_cod;}
	if (StringMatch(sk, VAR_CHL_A)) {*data = this -> m_chl_a;}

	else {
		throw ModelException("NutRemv", "GetValue","Parameter " + sk + " does not exist. Please contact the module developer.");
	}
}

void NutrientRemviaSr::Get2DData(const char *key, int *nRows, int *nCols, float ***data)
{
    string sk(key);
    *nRows = m_nCells;
    *nCols = m_soiLayers;
    if (StringMatch(sk, VAR_SOL_NO3))
    {
        *data = this->m_sol_no3;
    }
    if (StringMatch(sk, VAR_SOL_SOLP))
    {
        *data = this->m_sol_solp;
    }
    else
        throw ModelException("NutRemv", "Get2DData", "Output " + sk
                                                     +
                                                     " does not exist in the NutRemv module. Please contact the module developer.");
}
// int main() {
//	system("pause");
//}