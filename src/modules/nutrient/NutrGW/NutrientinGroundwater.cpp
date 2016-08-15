/*//
 * \file NutrientinGroundwater.cpp
 * \ingroup NutGW
 * \author Huiran Gao
 * \date Jun 2016
 */

#include <iostream>
#include "NutrientinGroundwater.h"
#include "MetadataInfo.h"
#include <cmath>
#include <fstream>
#include "ModelException.h"
#include "util.h"
#include <omp.h>

using namespace std;

NutrientinGroundwater::NutrientinGroundwater(void) :
//input
        m_TimeStep(-1), m_nCells(-1), m_cellWidth(-1), m_gwno3Con(NULL), m_gwminpCon(NULL), m_gw_q(NULL),
		m_nSubbasins(-1), m_subbasin(NULL), m_subbasinsInfo(NULL), m_gwStor(NULL), 
		m_perco_no3_gw(NULL), m_perco_solp_gw(NULL), m_soilLayers(NULL), m_nSoilLayers(-1), m_sol_no3(NULL),
        //output
        m_minpgwToCh(NULL), m_no3gwToCh(NULL)
{

}

NutrientinGroundwater::~NutrientinGroundwater(void)
{
	if (m_no3gwToCh != NULL) Release1DArray(m_no3gwToCh);
    if (m_minpgwToCh != NULL) Release1DArray(m_minpgwToCh);
}

void NutrientinGroundwater::SetSubbasins(clsSubbasins *subbasins)
{
	if(m_subbasinsInfo == NULL)
	{
		m_subbasinsInfo = subbasins;
		m_nSubbasins = m_subbasinsInfo->GetSubbasinNumber();
		m_subbasinIDs = m_subbasinsInfo->GetSubbasinIDs();
	}
}

bool NutrientinGroundwater::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
    {
        throw ModelException(MID_NUTRGW, "CheckInputSize",
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
            m_nCells << ".\n";
            throw ModelException(MID_NUTRGW, "CheckInputSize", oss.str());
        }
    }
    return true;
}

bool NutrientinGroundwater::CheckInputData()
{
    if (m_nCells <= 0)
        throw ModelException(MID_NUTRGW, "CheckInputData", "The cells number can not be less than zero.");
	if (m_TimeStep <= 0)
		throw ModelException(MID_NUTRGW, "CheckInputData", "The parameter: m_TimeStep has not been set.");
    if (m_cellWidth < 0)
        throw ModelException(MID_NUTRGW, "CheckInputData", "The cell width can not be less than zero.");
    if (m_gw_q == NULL)
        throw ModelException(MID_NUTRGW, "CheckInputData", "The groundwater contribution to stream flow data can not be NULL.");
	if (m_gwStor == NULL)
		throw ModelException(MID_NUTRGW, "CheckInputData", "The groundwater storage can not be NULL.");
	if (m_perco_no3_gw == NULL)
		throw ModelException(MID_NUTRGW, "CheckInputData", "The NO3 percolation to groundwater can not be NULL.");
	if (m_perco_solp_gw == NULL)
		throw ModelException(MID_NUTRGW, "CheckInputData", "The solute P percolation to groundwater can not be NULL.");
	if (m_soilLayers == NULL)
		throw ModelException(MID_NUTRGW, "CheckInputData", "The soil layers number can not be NULL.");
	if(m_sol_no3 == NULL)
		throw ModelException(MID_NUTRGW, "CheckInputData", "m_sol_no3 can not be NULL.");
    return true;
}

void NutrientinGroundwater::SetValue(const char *key, float value)
{
    string sk(key);
	if (StringMatch(sk, Tag_TimeStep))
		m_TimeStep = int(value);
    else if (StringMatch(sk, VAR_OMP_THREADNUM))
    {
        omp_set_num_threads((int) value);
    }
    //else if (StringMatch(sk, Tag_CellSize))
    //{
    //    m_nCells = (int)value;
    //}
    else if (StringMatch(sk, Tag_CellWidth))
    {
        m_cellWidth = value;
    }
    else
    {
        throw ModelException(MID_NUTRGW, "SetValue", "Parameter " + sk +
                                                  " does not exist in current method. Please contact the module developer.");
    }
}

void NutrientinGroundwater::Set1DData(const char *key, int n, float *data)
{
    string sk(key);
    if (StringMatch(sk, VAR_SUBBSN))
	{
		if (!CheckInputSize(key, n)) return;
		m_subbasin = data;
	}
	else if (StringMatch(sk, VAR_GWNO3_CON))
        m_gwno3Con = data;
	else if (StringMatch(sk, VAR_GWMINP_CON))
        m_gwminpCon = data;
    else if (StringMatch(sk, VAR_SBQG))
        m_gw_q = data;
	else if (StringMatch(sk, VAR_SBGS))
		m_gwStor = data;
	else if (StringMatch(sk, VAR_PERCO_N_GW))
		m_perco_no3_gw = data;
	else if (StringMatch(sk, VAR_PERCO_P_GW))
		m_perco_solp_gw = data;
	else if (StringMatch(sk, VAR_SOILLAYERS))
		m_soilLayers = data;
    else
        throw ModelException(MID_NUTRGW, "Set1DData", "Parameter " + sk +
                                                  " does not exist in current module. Please contact the module developer.");
}

void NutrientinGroundwater::Set2DData(const char *key, int nRows, int nCols, float **data)
{
	if (!this->CheckInputSize(key, nRows)) return;
	string sk(key);

	m_nSoilLayers = nCols;
	if (StringMatch(sk, VAR_SOL_NO3)) 
		m_sol_no3 = data; 
	else
		throw ModelException(MID_NUTRGW, "Set2DData", "Parameter " + sk +
		    " does not exist. Please contact the module developer.");
}


void NutrientinGroundwater::initialOutputs()
{
    if (this->m_nCells <= 0)
        throw ModelException(MID_NUTRGW, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");

	if (m_gwno3Con == NULL)
	{
		Initialize1DArray(m_nSubbasins+1, m_gwno3Con, 10.f);
		Initialize1DArray(m_nSubbasins+1, m_gwminpCon, 10.f);
	}

    // allocate the output variables
	if (m_no3gwToCh == NULL)
	{
		Initialize1DArray(m_nSubbasins+1, m_no3gwToCh, 0.f);
		Initialize1DArray(m_nSubbasins+1, m_minpgwToCh, 0.f);
	}
}


int NutrientinGroundwater::Execute()
{
    if (!this->CheckInputData())
		return -1;
    initialOutputs();
	for(vector<int>::iterator iter=m_subbasinIDs.begin(); iter != m_subbasinIDs.end(); iter++)
    {
		int id = *iter;
		
		// gw no3 to channel
		float xx = m_gw_q[id] * m_TimeStep;
		m_no3gwToCh[id] = m_gwno3Con[id] * xx; //kg
		m_minpgwToCh[id] = m_gwminpCon[id] * xx;

		// gw no3 loss through revep
		Subbasin *subbasin = m_subbasinsInfo->GetSubbasinByID(id);
		float subArea = subbasin->getCellCount() * m_cellWidth * m_cellWidth; //m2
		float revap = subbasin->getEG();
		float no3ToSoil = revap/1000.f * subArea * m_gwno3Con[id];
		float solpToSoil = revap/1000.f * subArea * m_gwminpCon[id];//kg
		// update no3 in the bottom soil layer due to revap
		int *cells = subbasin->getCells();
		int nCells = subbasin->getCellCount();
		int index = 0;
		for (int i = 0; i < nCells; i++)
		{
			index = cells[i];
			m_sol_no3[index][(int)m_soilLayers[index] - 1] += no3ToSoil/nCells;
		}

		// update concentration
		float gwVol = subArea * m_gwStor[id]/1000.f;//m3
		m_gwno3Con[id] += m_perco_no3_gw[id]/gwVol;
		m_gwminpCon[id] += m_perco_solp_gw[id]/gwVol;
        
    }
    return 0;
}

void NutrientinGroundwater::Get1DData(const char *key, int *n, float **data)
{
    string sk(key);
    *n = m_nCells;
    if (StringMatch(sk, VAR_NO3GW_CH))
    {
        *data = this->m_no3gwToCh;
    }
    else if (StringMatch(sk, VAR_MINPGW_CH))
    {
        *data = this->m_minpgwToCh;
    }
    else
    {
        throw ModelException(MID_NUTRGW, "Get1DData", "Parameter " + sk + " does not exist.");
    }
}
