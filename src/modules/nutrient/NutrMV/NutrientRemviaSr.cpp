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

NutrientRemviaSr::NutrientRemviaSr(void) :
//input
        m_nCells(-1), m_cellWidth(-1), m_soiLayers(-1), m_sedimentYield(NULL), m_nperco(-1), m_phoskd(-1), m_pperco(-1),
        m_qtile(-1), m_nSoilLayers(NULL), m_anion_excl(NULL), m_isep_opt(-1), m_ldrain(NULL), m_surfr(NULL), m_flat(NULL),
        m_sol_perco(NULL), m_sol_wsatur(NULL), m_sol_crk(NULL), m_sol_bd(NULL), m_sol_z(NULL), m_sol_thick(NULL),
        m_sol_om(NULL), m_flowOutIndex(NULL), m_nSubbasins(-1), m_subbasin(NULL), m_subbasinsInfo(NULL), m_streamLink(NULL),
        //output
        m_latno3(NULL), m_perco_n(NULL),m_perco_p(NULL), m_surqno3(NULL), m_sol_no3(NULL), m_surqsolp(NULL), m_wshd_plch(-1),
		m_latno3ToCh(NULL), m_sur_no3ToCh(NULL), m_sur_solpToCh(NULL), m_perco_n_gw(NULL), m_perco_p_gw(NULL),
        m_sol_solp(NULL), m_cod(NULL), m_chl_a(NULL) //,m_doxq(), m_soxy()
{

}

NutrientRemviaSr::~NutrientRemviaSr(void)
{
	if (m_latno3 != NULL) Release1DArray(m_latno3);
	if (m_perco_n != NULL) Release1DArray(m_perco_n);
	if (m_perco_p != NULL) Release1DArray(m_perco_p);
	if (m_surqno3 != NULL) Release1DArray(m_surqno3);
	if (m_surqsolp != NULL) Release1DArray(m_surqsolp);
	if (m_cod != NULL) Release1DArray(m_cod);
	if (m_chl_a != NULL) Release1DArray(m_chl_a);

	if(m_latno3ToCh != NULL) Release1DArray(m_latno3ToCh);
	if(m_sur_no3ToCh != NULL) Release1DArray(m_sur_no3ToCh);
	if(m_sur_solpToCh != NULL) Release1DArray(m_sur_solpToCh);
	if(m_perco_n_gw != NULL) Release1DArray(m_perco_n_gw);
	if(m_perco_p_gw != NULL) Release1DArray(m_perco_p_gw);
}

void NutrientRemviaSr::SumBySubbasin()
{
	float cellArea = m_cellWidth * m_cellWidth * 0.0001f; //ha
	// sum by subbasin
	for (int i = 0; i < m_nCells; i++)
	{
		//add today's flow
		int subi = (int) m_subbasin[i];
		if (m_nSubbasins == 1)
		{
			subi = 1;
		}
		else if (subi >= m_nSubbasins + 1)
		{
			ostringstream oss;
			oss << subi;
			throw ModelException(MID_NUTRSED, "Execute", "The subbasin " + oss.str() + " is invalid.");
		}


		m_sur_no3ToCh[subi] += m_surqno3[i] * cellArea;
		m_sur_solpToCh[subi] += m_surqsolp[i] * cellArea;
		m_perco_n_gw[subi] += m_perco_n[i] * cellArea;
		m_perco_p_gw[subi] += m_perco_p[i] * cellArea;

		if(m_streamLink[i] > 0)
			m_latno3ToCh[subi] += m_latno3[i];
	}


	// sum all the subbasins and put the sum value in the zero-index of the array
	for (int i = 1; i < m_nSubbasins + 1; i++)
	{
		m_sur_no3ToCh[0] += m_sur_no3ToCh[i];
		m_sur_solpToCh[0] += m_sur_solpToCh[i];
		m_latno3ToCh[0] += m_latno3ToCh[i];
		m_perco_n_gw[0] += m_perco_n_gw[i];
		m_perco_p_gw[0] += m_perco_p_gw[i];
	}
}

bool NutrientRemviaSr::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
    {
        throw ModelException(MID_NUTRMV, "CheckInputSize",
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
            throw ModelException(MID_NUTRMV, "CheckInputSize", oss.str());
        }
    }
    return true;
}

bool NutrientRemviaSr::CheckInputData()
{
    if (this->m_nCells <= 0)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The cells number can not be less than zero.");
    }
    if (this->m_cellWidth <= 0)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The cell width can not be less than zero.");
    }
    if (this->m_soiLayers <= 0)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The layer number of the input 2D raster data can not be less than zero.");
    }
    if (this->m_nSoilLayers == NULL)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "Soil layers number must not be NULL");
    }
    if (this->m_sedimentYield == NULL)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The distribution of soil loss caused by water erosion can not be NULL.");
    }
    if (this->m_sol_om == NULL)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The percent organic matter in soil layer can not be NULL.");
    }
    if (this->m_anion_excl == NULL)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The fraction of porosity from which anions are excluded can not be NULL.");
    }
    if (this->m_isep_opt <= 0)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The septic operational condition code can not be NULL.");
    }
    //if (this->m_ldrain == NULL)
    //{
    //    throw ModelException(MID_NUTRMV, "CheckInputData", "The soil layer where drainage tile is located can not be NULL.");
    //}
    if (this->m_surfr == NULL)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The distribution of surface runoff generated data can not be NULL.");
    }
    if (this->m_nperco <= 0)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The nitrate percolation coefficient can not be less than zero.");
    }
    if (this->m_flat == NULL)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The lateral flow in soil layer data can not be NULL.");
    }
    if (this->m_sol_perco == NULL)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "percolation from soil layer can not be NULL.");
    }
    if (this->m_sol_wsatur == NULL)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The amount of water held in the soil layer at saturation data can not be NULL.");
    }
    if (this->m_phoskd <= 0)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "Phosphorus soil partitioning coefficient can not be less than zero.");
    }
	if (this->m_sol_thick == NULL)
	{
		throw ModelException(MID_NUTRMV, "CheckInputData", "The m_sol_thick can not be NULL.");
	}
    if (this->m_sol_crk == NULL)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The crack volume potential of soil data can not be NULL.");
    }
    if (this->m_pperco <= 0)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "Phosphorus percolation coefficient can not be less than zero.");
    }
    if (this->m_sol_bd == NULL)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The bulk density of the soil data can not be NULL.");
    }
    if (this->m_sol_z == NULL)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The depth to bottom of soil layer can not be NULL.");
    }
	if (m_flowOutIndex == NULL)
		throw ModelException(MID_NUTRMV, "CheckInputData", "The parameter: flow out index has not been set.");
	if (m_nSubbasins <= 0) 
		throw ModelException(MID_NUTRMV, "CheckInputData", "The subbasins number must be greater than 0.");
	if (m_subbasinIDs.empty()) 
		throw ModelException(MID_NUTRMV, "CheckInputData", "The subbasin IDs can not be EMPTY.");
	if (m_subbasinsInfo == NULL)
		throw ModelException(MID_NUTRMV, "CheckInputData", "The parameter: m_subbasinsInfo has not been set.");

    return true;
}

void NutrientRemviaSr::SetSubbasins(clsSubbasins *subbasins)
{
	if(m_subbasinsInfo == NULL){
		m_subbasinsInfo = subbasins;
		m_nSubbasins = m_subbasinsInfo->GetSubbasinNumber();
		m_subbasinIDs = m_subbasinsInfo->GetSubbasinIDs();
	}
}

void NutrientRemviaSr::SetValue(const char *key, float value)
{
    string sk(key);
    if (StringMatch(sk, VAR_OMP_THREADNUM))
    {
        omp_set_num_threads((int) value);
    }
    //else if (StringMatch(sk, Tag_CellSize)) { this->m_nCells = value; }
    else if (StringMatch(sk, Tag_CellWidth)) { this->m_cellWidth = value; }
    else if (StringMatch(sk, VAR_QTILE)) { this->m_qtile = value; }
    else if (StringMatch(sk, VAR_NPERCO)) { this->m_nperco = value; }
    else if (StringMatch(sk, VAR_PPERCO)) { this->m_pperco = value; }
    else if (StringMatch(sk, VAR_PHOSKD)) { this->m_phoskd = value; }
    else if (StringMatch(sk, VAR_ISEP_OPT)) { this->m_isep_opt = value; }
	else if (StringMatch(sk, VAR_WSHD_PLCH)) m_wshd_plch = value;
    else
    {
        throw ModelException(MID_NUTRMV, "SetValue", "Parameter " + sk +
                                                    " does not exist. Please contact the module developer.");
    }
}

void NutrientRemviaSr::Set1DData(const char *key, int n, float *data)
{
    if (!this->CheckInputSize(key, n)) return;
    string sk(key);
    if (StringMatch(sk, VAR_FLOW_OL)) 
		m_surfr = data; 
	else if (StringMatch(sk, VAR_SUBBSN))
		m_subbasin = data;
	else if(StringMatch(sk, VAR_STREAM_LINK))
		m_streamLink = data;
    else if (StringMatch(sk, VAR_ANION_EXCL)) 
		m_anion_excl = data; 
    else if (StringMatch(sk, VAR_LDRAIN)) 
		m_ldrain = data; 
    else if (StringMatch(sk, VAR_SOL_CRK)) 
		m_sol_crk = data; 
	else if (StringMatch(sk, VAR_SOILLAYERS)) 
		m_nSoilLayers = data; 
    else if (StringMatch(sk, VAR_SED_OL)) 
	{ 
		this->m_sedimentYield = data; 
		// convert kg to ton
		for (int i = 0; i < n; i++)
			m_sedimentYield[i] /= 1000.f;
	}
	else if (StringMatch(sk, Tag_FLOWOUT_INDEX_D8))
		m_flowOutIndex = data;
    else if (StringMatch(sk, VAR_SEDORGN)) 
		m_sedorgn = data; 
    else if (StringMatch(sk, VAR_TMEAN)) 
		m_tmean = data;
    else
        throw ModelException("NutRemv", "SetValue", "Parameter " + sk +
                                                    " does not exist. Please contact the module developer.");
}

void NutrientRemviaSr::Set2DData(const char *key, int nRows, int nCols, float **data)
{
    if (!this->CheckInputSize(key, nRows)) return;
    string sk(key);

	m_soiLayers = nCols;
    if (StringMatch(sk, VAR_SSRU)) { m_flat = data; }
    else if (StringMatch(sk, VAR_SOL_NO3)) { m_sol_no3 = data; }
    else if (StringMatch(sk, VAR_SOL_BD)) { m_sol_bd = data; }
    else if (StringMatch(sk, VAR_SOL_SOLP)) { m_sol_solp = data; }
    else if (StringMatch(sk, VAR_SOILDEPTH)) { m_sol_z = data; }
    else if (StringMatch(sk, VAR_PERCO)) { m_sol_perco = data; }
	else if (StringMatch(sk, VAR_SOL_OM)) { m_sol_om = data; }
	else if (StringMatch(sk, VAR_SOILTHICK)) { m_sol_thick = data; }
	else if (StringMatch(sk, VAR_SOL_UL)) { m_sol_wsatur = data; }
    else
        throw ModelException("NutRemv", "SetValue", "Parameter " + sk +
                                                    " does not exist. Please contact the module developer.");
}

void NutrientRemviaSr::initialOutputs()
{
    if (this->m_nCells <= 0)
    {
        throw ModelException(MID_NUTRMV, "CheckInputData", "The dimension of the input data can not be less than zero.");
    }
    // allocate the output variables
    if (m_latno3 == NULL)
    {
		Initialize1DArray(m_nCells, m_latno3, 0.f);
		Initialize1DArray(m_nCells, m_perco_n, 0.f);
		Initialize1DArray(m_nCells, m_perco_p, 0.f);
		Initialize1DArray(m_nCells, m_surqno3, 0.f);
		Initialize1DArray(m_nCells, m_surqsolp, 0.f);

		Initialize1DArray(m_nSubbasins+1, m_latno3ToCh, 0.f);
		Initialize1DArray(m_nSubbasins+1, m_sur_no3ToCh, 0.f);
		Initialize1DArray(m_nSubbasins+1, m_sur_solpToCh, 0.f);
		Initialize1DArray(m_nSubbasins+1, m_perco_n_gw, 0.f);
		Initialize1DArray(m_nSubbasins+1, m_perco_p_gw, 0.f);

    }
    if (m_cod == NULL)
    {
		Initialize1DArray(m_nCells, m_cod, 0.f);
		Initialize1DArray(m_nCells, m_chl_a, 0.f);
    }
    if (m_wshd_plch < 0)
    {
        m_wshd_plch = 0.f;
    }
    // input variables
    if (m_flat == NULL) { Initialize2DArray(m_nCells, m_soiLayers, m_flat, 0.0001f); }
    if (m_sol_perco == NULL) { Initialize2DArray(m_nCells, m_soiLayers, m_sol_perco, 0.0001f); }
    if (m_ldrain == NULL) { Initialize1DArray(m_nCells, m_ldrain, -1.f); }
    m_qtile = 0.0001f;
}

int NutrientRemviaSr::Execute()
{
    if (!CheckInputData())
    {
        return false;
    }
    initialOutputs();

    //Calculate the loss of nitrate via surface runoff, lateral flow, tile flow, and percolation out of the profile.
    NitrateLoss();
    // Calculates the amount of phosphorus lost from the soil.
    PhosphorusLoss();
	// sum by sub-basin
	SumBySubbasin();

    return 0;
}

void NutrientRemviaSr::NitrateLoss()
            {
                //percnlyr nitrate moved to next lower layer with percolation (kg/ha)
                float percnlyr = 0.f;
				float ssfnlyr = 0.f;
                //float *tileno3;
				//#pragma omp parallel for
				//did not use parallel computing to avoid several cells flow into the same downstream cell

                for (int i = 0; i < m_nCells; i++)
                {
                    for (int k = 0; k < m_nSoilLayers[i]; k++)
                    {
                        //add nitrate moved from layer above
                        m_sol_no3[i][k] = m_sol_no3[i][k] + percnlyr;
                        if (m_sol_no3[i][k] < 1e-6f)
                            m_sol_no3[i][k] = 0.f;

                        // determine concentration of nitrate in mobile water
                        // surface runoff generated (sro)
                        float sro = 0.f;
                        // amount of mobile water in the layer (mw)
                        float mw = 0.f;
                        float vno3 = 0.f;
                        float con = 0.f;
                        float ww = 0.f;

                        if (k == 0)
                            sro = m_surfr[i];
                        else
                            sro = 0.f;
                        if (m_ldrain[i] == k)
                            mw = mw + m_qtile;

                        // Calculate the concentration of nitrate in the mobile water (con),
                        // equation 4:2.1.2, 4:2.1.3 and 4:2.1.4 in SWAT Theory 2009, p269
                        mw = m_sol_perco[i][k] + sro + m_flat[i][k] + 1.e-10f;
                        ww = -mw / ((1.f - m_anion_excl[i]) * m_sol_wsatur[i][k]);
                        vno3 = m_sol_no3[i][k] * (1.f - exp(ww));
                        if (mw > 1.e-10f)
                            con = max(vno3 / mw, 0.f);

                        // calculate nitrate in surface runoff
                        // concentration of nitrate in surface runoff (cosurf)
                        float cosurf = 0.f;
                        if (m_isep_opt == 2)
                            cosurf = 1.f * con; // N percolation does not apply to failing septic place;
                        else
                            cosurf = m_nperco * con;

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
                            //tileno3[i] = con * m_qtile;
                            //tileno3[i] = min(tileno3[i], m_sol_no3[i][k]);
                            //m_sol_no3[i][k] = m_sol_no3[i][k] - tileno3[i];
                        }
                        // calculate nitrate in lateral flow
                        // nitrate transported in lateral flow from layer (ssfnlyr)
                        
                        if (k == 1)
                            ssfnlyr = cosurf * m_flat[i][k];
                        else
                            ssfnlyr = con * m_flat[i][k];
                        ssfnlyr = min(ssfnlyr, m_sol_no3[i][k]);
                        m_latno3[i] = m_latno3[i] + ssfnlyr;
						//move the lateral no3 flow to the downslope cell
                        m_sol_no3[i][k] = m_sol_no3[i][k] - ssfnlyr;
						int idDownSlope = (int)m_flowOutIndex[i];
						if (idDownSlope >= 0)
							m_sol_no3[idDownSlope][k] += ssfnlyr;

                        // calculate nitrate in percolate
                        percnlyr = 0.f;
                        percnlyr = con * m_sol_perco[i][k];
                        percnlyr = min(percnlyr, m_sol_no3[i][k]);
                        m_sol_no3[i][k] = m_sol_no3[i][k] - percnlyr;
                    }

                    // calculate nitrate leaching from soil profile
                    m_perco_n[i] = percnlyr;
                    float nloss = 0.f;
                    // average distance to the stream(m), default is 35m.
                    float dis_stream = 35.f;
                    nloss = (2.18f * dis_stream - 8.63f) / 100.f;
                    nloss = min(1.f, max(0.f, nloss));
                    m_latno3[i] = (1.f - nloss) * m_latno3[i];

                    // calculate CBOD, COD, Chl_a, doxq and soxy
                    // calculcate water temperature
                    // SWAT manual 2.3.13
                    float wtmp = 0.f;
                    wtmp = 5.f + 0.75f * m_tmean[i];
                    if (wtmp <= 0.1)
                    {
                        wtmp = 0.1f;
                    }
                    wtmp = wtmp + 273.15f;    // deg C to deg K
                    // water in cell
                    float qdr = 0.f;
                    //qdr = m_surfr[i] + m_flat[i][0] + m_qtile + m_gw_q[i];
					qdr = m_surfr[i] + m_flat[i][0] + m_qtile;
                    if (qdr > 1.e-4f)
                    {
                        // kilo moles of phosphorus in nutrient loading to main channel (tp)
                        float tp = 0.f;
                        tp = 100.f * (m_sedorgn[i] + m_surqno3[i]) / qdr;   //100*kg/ha/mm = ppm
                        // regional adjustment on sub chla_a loading
                        float chla_subco = 40.f;
                        m_chl_a[i] = chla_subco * tp;
                        m_chl_a[i] = m_chl_a[i] / 1000.f;  // um/L to mg/L

                        // calculate enrichment ratio
                        float enratio;
                        if (m_sedimentYield[i] < 1e-4)
                        {
                            m_sedimentYield[i] = 0.f;
                        }
                        // CREAMS method for calculating enrichment ratio
                        float cy = 0.f;
                        // Calculate sediment, equation 4:2.2.3 in SWAT Theory 2009, p272
                        cy = 0.1f * m_sedimentYield[i] / (m_cellWidth * m_cellWidth * 0.0001f * m_surfr[i] + 1e-6f);
                        if (cy > 1e-6f)
                        {
                            enratio = 0.78f * pow(cy, -0.2468f);
                        } else
                        {
                            enratio = 0.f;
                        }
                        if (enratio > 3.5)
                        {
                            enratio = 3.5f;
                        }

                        // calculate organic carbon loading to main channel
                        float org_c = (m_sol_om[i][0] * 0.58f / 100.f) * enratio * m_sedimentYield[i] * 1000.f;
                        // calculate carbonaceous biological oxygen demand (CBOD) and COD(transform from CBOD)
                        float cbod  = 2.7f * org_c / (qdr * m_cellWidth * m_cellWidth);
                        // calculate COD
                        float n = 3.f; // Conversion factor 1~6.5
                        float k = 0.15f; // Reaction coefficient 0.1~0.2
                        m_cod[i] = n * (cbod * (1.f - exp(-5.f * k)));
						m_cod[i] = m_cod[i] * m_surfr[i] * 100.f;	// mg/L converted to kg/ha

                        /*
                        // calculate dissolved oxygen saturation concentration (soxy)
                        float soxy = 0.f;
                        float ww = 0.f;
                        float xx = 0.f;
                        float yy = 0.f;
                        float zz = 0.f;
                        ww = -139.34410 + (1.575701E05 / wtmp);
                        xx = 6.642308E07 /pow (wtmp, 2);
                        yy = 1.243800E10 /pow (wtmp, 3);
                        zz = 8.621949E11 / pow(wtmp, 4);
                        m_soxy[i] = exp(ww - xx + yy - zz);
                        if (soxy < 0.) {
                            soxy = 0.f;
                        }
                        // calculate actual dissolved oxygen concentration
                        m_doxq[i] = soxy * exp(-0.1 * cbod);
                        if (m_doxq[i] < 0.) {
                            m_doxq[i] = 0.f;
                        }
                        if (m_doxq[i] > soxy) {
                            m_doxq[i] = soxy;
                        }
                        */
                    } else
                    {
                        m_chl_a[i] = 0.f;
                        m_cod[i] = 0.f;
                        //m_doxq[i] = 0.f;
                    }
                }
            }

            void NutrientRemviaSr::PhosphorusLoss()
            {

                for (int i = 0; i < m_nCells; i++)
                {
                    // mg/kg => kg/ha
//                     float *sol_thick = new float(m_nSoilLayers[i]);
//                     sol_thick[0] = m_sol_z[i][0];
//                     for (int k = 1; k < m_nSoilLayers[i]; k++)
//                     {
//                         sol_thick[k] = m_sol_z[k] - m_sol_z[k - 1];
//                     }
                    float wt1 = m_sol_bd[i][0] * m_sol_thick[i][0] / 100.f;
                    float conv_wt = 1.e6f * wt1;

                    // amount of P leached from soil layer (vap)
                    float vap = 0.f;
                    float vap_tile = 0.f;
                    // compute soluble P lost in surface runoff
                    float xx = 0.f;  // variable to hold intermediate calculation result
                    xx = m_sol_bd[i][0] * m_sol_z[i][0] * m_phoskd;
                    m_surqsolp[i] = m_sol_solp[i][0] * m_surfr[i] / xx;
                    m_surqsolp[i] = min(m_surqsolp[i], m_sol_solp[i][0]);
                    m_surqsolp[i] = max(m_surqsolp[i], 0.f);
                    m_sol_solp[i][0] = m_sol_solp[i][0] - m_surqsolp[i];

                    // compute soluble P leaching
                    vap = m_sol_solp[i][0] * m_sol_perco[i][0] / ((conv_wt / 1000.f) * m_pperco);
                    vap = min(vap, 0.5f * m_sol_solp[i][0]);
                    m_sol_solp[i][0] = m_sol_solp[i][0] - vap;

                    // estimate soluble p in tiles due to crack flow
                    if (m_ldrain[i] > 0)
                    {
                        xx = min(1.f, m_sol_crk[i] / 3.f);
                        vap_tile = xx * vap;
                        vap = vap - vap_tile;
                    }
                    if (m_nSoilLayers[i] >= 2)
                    {
                        m_sol_solp[i][1] = m_sol_solp[i][1] + vap;
                    }
                    for (int k = 1; k < m_nSoilLayers[i]; k++)
                    {
                        vap = 0.f;
                        //if (k != m_i_sep[i]) {  // soil layer where biozone exists (m_i_sep)
                        vap = m_sol_solp[i][k] * m_sol_perco[i][k] / ((conv_wt / 1000.f) * m_pperco);
                        vap = min(vap, 0.2f * m_sol_solp[i][k]);
                        m_sol_solp[i][k] = m_sol_solp[i][k] - vap;

						if(k < m_nSoilLayers[i] - 1)
							m_sol_solp[i][k+1] += vap;//leach to next layer
						else
							m_perco_p[i] = vap;//leach to groundwater
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

            void NutrientRemviaSr::Get1DData(const char *key, int *n, float **data)
            {
                string sk(key);
                
                if (StringMatch(sk, VAR_LATNO3)) 
				{
					*data = this->m_latno3;
					*n = m_nCells;
				}
                else if (StringMatch(sk, VAR_PERCO_N_GW)) 
				{
					*data = m_perco_n_gw; 
					*n = m_nSubbasins + 1;
				}
				else if (StringMatch(sk, VAR_PERCO_P_GW)) 
				{
					*data = m_perco_p_gw; 
					*n = m_nSubbasins + 1;
				}
                else if (StringMatch(sk, VAR_SUR_NO3)) 
				{
					*data = this->m_surqno3; 
					*n = m_nCells;
				}
                else if (StringMatch(sk, VAR_SUR_SOLP)) 
				{
					*data = this->m_surqsolp; 
					*n = m_nCells;
				}
                else if (StringMatch(sk, VAR_COD)) 
				{
					*data = this->m_cod; 
					*n = m_nCells;
				}
                else if (StringMatch(sk, VAR_CHL_A)) 
				{
					*data = this->m_chl_a; 
					*n = m_nCells;
				}
				else if(StringMatch(sk, VAR_LATNO3_CH))
				{
					*data = m_latno3ToCh;
					*n = m_nSubbasins + 1;
				}
				else if(StringMatch(sk, VAR_SUR_NO3_CH))
				{
					*data = m_sur_no3ToCh;
					*n = m_nSubbasins + 1;
				}
				else if(StringMatch(sk, VAR_SUR_SOLP_CH))
				{
					*data = m_sur_solpToCh;
					*n = m_nSubbasins + 1;
				}
                else
                    throw ModelException("NutRemv", "GetValue",
                                         "Parameter " + sk + " does not exist. Please contact the module developer.");
                
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
                else if (StringMatch(sk, VAR_SOL_SOLP))
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