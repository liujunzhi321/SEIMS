#include <iostream>
#include "SurrunoffTransfer.h"
#include "MetadataInfo.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include "ModelException.h"
#include "util.h"
#include <omp.h>

using namespace std;

SurrunoffTransfer::SurrunoffTransfer(void) :
//input
        m_nCells(-1), m_cellWidth(-1), m_soiLayers(-1), m_nSoilLayers(NULL), m_sedimentYield(NULL), m_surfr(NULL),
        m_sol_bd(NULL), m_soilDepth(NULL), m_enratio(NULL),
        m_sol_actp(NULL), m_sol_orgn(NULL), m_sol_orgp(NULL), m_sol_stap(NULL), m_sol_aorgn(NULL), m_sol_fon(NULL),
        m_sol_fop(NULL), m_nSubbasins(-1), m_subbasin(NULL), m_subbasinsInfo(NULL),
        //outputs
        m_sedorgn(NULL), m_sedorgp(NULL), m_sedminpa(NULL), m_sedminps(NULL)
{
}

SurrunoffTransfer::~SurrunoffTransfer(void)
{
	if (m_enratio != NULL) Release1DArray(m_enratio);

	if (m_sedorgp != NULL) Release1DArray(m_sedorgp);
	if (m_sedorgn != NULL) Release1DArray(m_sedorgn);
	if (m_sedminpa != NULL) Release1DArray(m_sedminpa);
	if (m_sedminps != NULL) Release1DArray(m_sedminps);

	if (m_sedorgnToCh != NULL) Release1DArray(m_sedorgnToCh);
	if (m_sedorgpToCh != NULL) Release1DArray(m_sedorgpToCh);
	if (m_sedminpaToCh != NULL) Release1DArray(m_sedminpaToCh);
	if (m_sedminpsToCh != NULL) Release1DArray(m_sedminpsToCh);
}

bool SurrunoffTransfer::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
		throw ModelException(MID_NUTRSED,"CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.\n");
    if (m_nCells != n)
    {
        if (m_nCells <= 0)
            m_nCells = n;
        else
			throw ModelException(MID_NUTRSED, "CheckInputSize", "Input data for " + string(key) + " is invalid with size: " +ValueToString(n)+ 
								". The origin size is " + ValueToString(m_nCells)+".\n");
    }
    return true;
}

bool SurrunoffTransfer::CheckInputData()
{
    if (this->m_nCells <= 0)
    {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The input data can not be less than zero.");
    }
    if (this->m_cellWidth <= 0)
    {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The cell width can not be less than zero.");
    }
    if (this->m_soiLayers <= 0)
    {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The layer number of the input 2D raster data can not be less than zero.");
    }
    if (this->m_nSoilLayers == NULL)
    {
        throw ModelException(MID_NUTRSED, "CheckInputData", "Soil layers number must not be NULL.");
    }
    if (this->m_sedimentYield == NULL)
    {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The distribution of soil loss caused by water erosion can not be NULL.");
    }
    if (this->m_surfr == NULL)
    {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The distribution of surface runoff generated data can not be NULL.");
    }
    if (this->m_sol_bd == NULL)
    {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The bulk density of the soil data can not be NULL.");
    }
    if (this->m_soilDepth == NULL)
    {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The depth to bottom of soil layer can not be NULL.");
    }
    if (this->m_sol_actp == NULL)
    {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The amount of phosphorus stored in the active mineral phosphorus pool can not be NULL.");
    }
    if (this->m_sol_orgn == NULL)
    {
		throw ModelException(MID_NUTRSED, "CheckInputData", "The amount of nitrogen stored in the stable organic N pool can not be NULL.");
    }
    if (this->m_sol_orgp == NULL)
    {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The amount of phosphorus stored in the organic P pool can not be NULL.");
    }
    if (this->m_sol_stap == NULL)
    {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The amount of phosphorus in the soil layer stored in the stable mineral phosphorus pool can not be NULL.");
    }
    if (this->m_sol_aorgn == NULL)
    {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The amount of nitrogen stored in the active organic nitrogen pool data can not be NULL.");
    }
    if (this->m_sol_fon == NULL)
    {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The amount of nitrogen stored in the fresh organic pool can not be NULL.");
    }
    if (this->m_sol_fop == NULL)
    {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The amount of phosphorus stored in the fresh organic pool can not be NULL.");
    }

	if (m_subbasin == NULL)
		throw ModelException(MID_NUTRSED, "CheckInputData", "The parameter: m_subbasin has not been set.");

	if (m_nSubbasins <= 0) throw ModelException(MID_NUTRSED, "CheckInputData", "The subbasins number must be greater than 0.");
	if (m_subbasinIDs.empty()) throw ModelException(MID_NUTRSED, "CheckInputData", "The subbasin IDs can not be EMPTY.");
	if (m_subbasinsInfo == NULL)
		throw ModelException(MID_NUTRSED, "CheckInputData", "The parameter: m_subbasinsInfo has not been set.");

    return true;
}

void SurrunoffTransfer::SetValue(const char *key, float value)
{
    string sk(key);
    if (StringMatch(sk, VAR_OMP_THREADNUM)) omp_set_num_threads((int) value);
    else if (StringMatch(sk, Tag_CellWidth))m_cellWidth = value;
    else
        throw ModelException(MID_NUTRSED, "SetValue", "Parameter " + sk +" does not exist.");
}

void SurrunoffTransfer::Set1DData(const char *key, int n, float *data)
{
    if (!this->CheckInputSize(key, n)) return;

    string sk(key);
	if (StringMatch(sk, VAR_SUBBSN))
		m_subbasin = data;
    else if (StringMatch(sk, VAR_SED_OL))
    {
        this->m_sedimentYield = data;
		// convert kg to ton
		for (int i = 0; i < n; i++)
			m_sedimentYield[i] /= 1000.f;
    }
	else if (StringMatch(sk, VAR_SOILLAYERS))
	{
		m_nSoilLayers = data;
	}
    else if (StringMatch(sk, VAR_FLOW_OL))
    {
        this->m_surfr = data;
    }
    else
    {
        throw ModelException(MID_NUTRSED, "Set1DData", "Parameter " + sk +" does not exist.");
    }
}

void SurrunoffTransfer::Set2DData(const char *key, int nRows, int nCols, float **data)
{
    if (!this->CheckInputSize(key, nRows)) return;
    string sk(key);
    m_soiLayers = nCols;
    if (StringMatch(sk, VAR_SOILDEPTH)) { this->m_soilDepth = data; }
    else if (StringMatch(sk, VAR_SOL_BD)) { this->m_sol_bd = data; }
    else if (StringMatch(sk, VAR_SOL_AORGN)) { this->m_sol_aorgn = data; }
    else if (StringMatch(sk, VAR_SOL_SORGN)) { this->m_sol_orgn = data; }
    else if (StringMatch(sk, VAR_SOL_HORGP)) { this->m_sol_orgp = data; }
    else if (StringMatch(sk, VAR_SOL_FOP)) { this->m_sol_fop = data; }
    else if (StringMatch(sk, VAR_SOL_FON)) { this->m_sol_fon = data; }
    else if (StringMatch(sk, VAR_SOL_ACTP)) { this->m_sol_actp = data; }
    else if (StringMatch(sk, VAR_SOL_STAP)) { this->m_sol_stap = data; }
    //else if (StringMatch(sk, VAR_SOL_MP)) {this -> m_sol_mp = data;}
    else
        throw ModelException(MID_NUTRSED, "Set2DData", "Parameter " + sk + " does not exist.");
}

void SurrunoffTransfer::initialOutputs()
{
    if (this->m_nCells <= 0)
    {
        throw ModelException(MID_NUTRSED, "CheckInputData", "The dimension of the input data can not be less than zero.");
    }
	// initial enrichment ratio
	if (this->m_enratio == NULL)
	{
		Initialize1DArray(m_nCells, m_enratio, 0.f);
		for (int i = 0; i < m_nCells; i++)
		{
			if (m_sedimentYield[i] < 1e-4f)
			{
				m_sedimentYield[i] = 0.f;
			}
			// CREAMS method for calculating enrichment ratio
			float cy = 0.f;
			// Calculate sediment calculations, equation 4:2.2.3 in SWAT Theory 2009, p272
			cy = 0.1f * m_sedimentYield[i] / (m_cellWidth * m_cellWidth * 0.0001f * m_surfr[i] + 1e-6f);
			if (cy > 1e-6f)
			{
				m_enratio[i] = 0.78f * pow(cy, -0.2468f);
			} else
			{
				m_enratio[i] = 0.f;
			}
			if (m_enratio[i] > 3.5f)
			{
				m_enratio[i] = 3.5f;
			}
		}
	}

    // allocate the output variables
    if (m_sedorgn == NULL)
    {
		Initialize1DArray(m_nCells, m_sedorgn, 0.f);
		Initialize1DArray(m_nCells, m_sedorgp, 0.f);
		Initialize1DArray(m_nCells, m_sedminpa, 0.f);
		Initialize1DArray(m_nCells, m_sedminps, 0.f);

		Initialize1DArray(m_nSubbasins+1, m_sedorgnToCh, 0.f);
		Initialize1DArray(m_nSubbasins+1, m_sedorgpToCh, 0.f);
		Initialize1DArray(m_nSubbasins+1, m_sedminpaToCh, 0.f);
		Initialize1DArray(m_nSubbasins+1, m_sedminpsToCh, 0.f);
    }
    //if(m_sol_mp == NULL) {Initialize2DArray(m_nCells, m_soiLayers, m_sol_mp, 0.f);}
}

void SurrunoffTransfer::SetSubbasins(clsSubbasins *subbasins)
{
	if(m_subbasinsInfo == NULL){
		m_subbasinsInfo = subbasins;
		m_nSubbasins = m_subbasinsInfo->GetSubbasinNumber();
		m_subbasinIDs = m_subbasinsInfo->GetSubbasinIDs();
	}
}

int SurrunoffTransfer::Execute()
{
    if (!CheckInputData())return false;
    this->initialOutputs();

	#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++)
    {
        //Calculates the amount of organic nitrogen removed in surface runoff. orgn.f of SWAT
        OrgnRemoveinSr(i);
        //Calculates the amount of organic and mineral phosphorus attached to sediment in surface runoff. psed.f of SWAT
        OrgpAttachedtoSed(i);
    }
    
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
			throw ModelException(MID_NUTRSED, "Execute", "The subbasin " + ValueToString(subi) + " is invalid.");
		}
		m_sedorgnToCh[subi] += m_sedorgn[i];
		m_sedorgpToCh[subi] += m_sedorgp[i];
		m_sedminpaToCh[subi] = m_sedminpa[i];
		m_sedminpsToCh[subi] = m_sedminps[i];
	}
	// sum all the subbasins and put the sum value in the zero-index of the array
	float cellArea = m_cellWidth * m_cellWidth * 0.0001f; //ha
	//for (int i = 1; i < m_nSubbasins + 1; i++)
	for (vector<int>::iterator it = m_subbasinIDs.begin(); it != m_subbasinIDs.end(); it++)
	{
		m_sedorgnToCh[0] += m_sedorgnToCh[*it] * cellArea;
		m_sedorgpToCh[0] += m_sedorgpToCh[*it] * cellArea;
		m_sedminpaToCh[0] = m_sedminpaToCh[*it] * cellArea;
		m_sedminpsToCh[0] = m_sedminpsToCh[*it] * cellArea;
	}
    return 0;
}

void SurrunoffTransfer::OrgnRemoveinSr(int i)
{
    for (int k = 0; k < (int)m_nSoilLayers[i]; k++)
    {
        //amount of organic N in first soil layer (orgninfl)
        float orgninfl = 0.f;
        //conversion factor (wt)
        float wt = 0.f;
        orgninfl = m_sol_orgn[i][0] + m_sol_aorgn[i][0] + m_sol_fon[i][0];
        wt = m_sol_bd[i][0] * m_soilDepth[i][0] / 100.f;
        //concentration of organic N in soil (concn)
        float concn = 0.f;
        concn = orgninfl * m_enratio[i] / wt;
        //Calculate the amount of organic nitrogen transported with sediment to the stream, equation 4:2.2.1 in SWAT Theory 2009, p271
        m_sedorgn[i] = 0.001f * concn * m_sedimentYield[i] / (m_cellWidth * m_cellWidth * m_nCells);
        //update soil nitrogen pools
        if (orgninfl > 1e-6f)
        {
            m_sol_aorgn[i][0] = m_sol_aorgn[i][0] - m_sedorgn[i] * (m_sol_aorgn[i][0] / orgninfl);
            m_sol_orgn[i][0] = m_sol_orgn[i][0] - m_sedorgn[i] * (m_sol_orgn[i][0] / orgninfl);
            m_sol_fon[i][0] = m_sol_fon[i][0] - m_sedorgn[i] * (m_sol_fon[i][0] / orgninfl);
            if (m_sol_aorgn[i][0] < 0.f)
            {
                m_sedorgn[i] = m_sedorgn[i] + m_sol_aorgn[i][0];
                m_sol_aorgn[i][0] = 0.f;
            }
            if (m_sol_orgn[i][0] < 0.f)
            {
                m_sedorgn[i] = m_sedorgn[i] + m_sol_orgn[i][0];
                m_sol_orgn[i][0] = 0.f;
            }
            if (m_sol_fon[i][0] < 0.f)
            {
                m_sedorgn[i] = m_sedorgn[i] + m_sol_fon[i][0];
                m_sol_fon[i][0] = 0.f;
            }
        }
    }
}

void SurrunoffTransfer::OrgpAttachedtoSed(int i)
{
    for (int k = 0; k < (int)m_nSoilLayers[i]; k++)
    {
        //amount of phosphorus attached to sediment in soil (sol_attp)
        float sol_attp = 0.f;
        //fraction of active mineral/organic/stable mineral phosphorus in soil (sol_attp_o, sol_attp_a, sol_attp_s)
        float sol_attp_o = 0.f;
        float sol_attp_a = 0.f;
        float sol_attp_s = 0.f;
        //Calculate sediment
        sol_attp = m_sol_orgp[i][0] + m_sol_fop[i][0] + m_sol_actp[i][0] + m_sol_stap[i][0];
        if (sol_attp > 1e-3f)
        {
            sol_attp_o = (m_sol_orgp[i][0] + m_sol_fop[i][0]) / sol_attp;
            sol_attp_a = m_sol_actp[i][0] / sol_attp;
            sol_attp_s = m_sol_stap[i][0] / sol_attp;
        }
        //conversion factor (mg/kg => kg/ha) (wt)
        float wt = m_sol_bd[i][0] * m_soilDepth[i][0] / 100.f;
        //concentration of organic P in soil (concp)
        float concp = 0.f;
        concp = sol_attp * m_enratio[i] / wt;
        //total amount of P removed in sediment erosion (sedp)
        float sedp = 0.001f * concp * m_sedimentYield[i] / (m_cellWidth * m_cellWidth) / 10000.f;
        m_sedorgp[i] = sedp * sol_attp_o;
        m_sedminpa[i] = sedp * sol_attp_a;
        m_sedminps[i] = sedp * sol_attp_s;
        //modify phosphorus pools
        //total amount of P in mineral sediment pools prior to sediment removal (psedd)
        float psedd = 0.f;
        //total amount of P in organic pools prior to sediment removal (porgg)
        float porgg = 0.f;
        psedd = m_sol_actp[i][0] + m_sol_stap[i][0];
        porgg = m_sol_orgp[i][0] + m_sol_fop[i][0];
        if (porgg > 1e-3f)
        {
            m_sol_orgp[i][0] = m_sol_orgp[i][0] - m_sedorgp[i] * (m_sol_orgp[i][0] / porgg);
            m_sol_fop[i][0] = m_sol_fop[i][0] - m_sedorgp[i] * (m_sol_fop[i][0] / porgg);
        }
        m_sol_actp[i][0] = m_sol_actp[i][0] - m_sedminpa[i];
        m_sol_stap[i][0] = m_sol_stap[i][0] - m_sedminps[i];
        if (m_sol_orgp[i][0] < 0.f)
        {
            m_sedorgp[i] = m_sedorgp[i] + m_sol_orgp[i][0];
            m_sol_orgp[i][0] = 0.f;
        }
        if (m_sol_fop[i][0] < 0.f)
        {
            m_sedorgp[i] = m_sedorgp[i] + m_sol_fop[i][0];
            m_sol_fop[i][0] = 0.f;
        }
        if (m_sol_actp[i][0] < 0.f)
        {
            m_sedminpa[i] = m_sedminpa[i] + m_sol_actp[i][0];
            m_sol_actp[i][0] = 0.f;
        }
        if (m_sol_stap[i][0] < 0.f)
        {
            m_sedminps[i] = m_sedminps[i] + m_sol_stap[i][0];
            m_sol_stap[i][0] = 0.f;
        }
    }
}

void SurrunoffTransfer::Get1DData(const char *key, int *n, float **data)
{
    string sk(key);
    if (StringMatch(sk, VAR_SEDORGN)) 
	{ 
		*data = this->m_sedorgn; 
		*n = m_nCells;
	}
    else if (StringMatch(sk, VAR_SEDORGP)) 
	{ 
		*data = this->m_sedorgp; 
		*n = m_nCells;
	}
    else if (StringMatch(sk, VAR_SEDMINPA)) 
	{ 
		*data = this->m_sedminpa; 
		*n = m_nCells;
	}
    else if (StringMatch(sk, VAR_SEDMINPS)) 
	{ 
		*data = this->m_sedminps; 
		*n = m_nCells;
	}
	else if(StringMatch(sk, VAR_SEDORGN_TOCH))
	{
		*data = m_sedorgnToCh; 
		*n = m_nSubbasins + 1;
	}
	else if(StringMatch(sk, VAR_SEDORGP_TOCH))
	{
		*data = m_sedorgpToCh; 
		*n = m_nSubbasins + 1;
	}
	else if(StringMatch(sk, VAR_SEDMINPA_TOCH))
	{
		*data = m_sedminpaToCh; 
		*n = m_nSubbasins + 1;
	}
	else if(StringMatch(sk, VAR_SEDMINPS_TOCH))
	{
		*data = m_sedminpsToCh; 
		*n = m_nSubbasins + 1;
	}
    else
    {
        throw ModelException(MID_NUTRSED, "Get1DData","Parameter " + sk + " does not exist");
    }
}

void SurrunoffTransfer::Get2DData(const char *key, int *nRows, int *nCols, float ***data)
{
    string sk(key);
    *nRows = m_nCells;
    *nCols = m_soiLayers;
    if (StringMatch(sk, VAR_SOL_AORGN)) { *data = this->m_sol_aorgn; }
    else if (StringMatch(sk, VAR_SOL_FON)) { *data = this->m_sol_fon; }
    else if (StringMatch(sk, VAR_SOL_SORGN)) { *data = this->m_sol_orgn; }
    else if (StringMatch(sk, VAR_SOL_HORGP)) { *data = this->m_sol_orgp; }
    else if (StringMatch(sk, VAR_SOL_FOP)) { *data = this->m_sol_fop; }
    else if (StringMatch(sk, VAR_SOL_STAP)) { *data = this->m_sol_stap; }
    else if (StringMatch(sk, VAR_SOL_ACTP)) { *data = this->m_sol_actp; }
    else
        throw ModelException(MID_NUTRSED, "Get2DData", "Output " + sk +" does not exist.");
}
