#include <string.h>
#include <math.h>
#include "SSR_NUTR.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>
#include <stdlib.h>

#include <map>
#include <iostream>

SSR_NUTR::SSR_NUTR(void) : m_nSoilLayers(-1), m_dt(-1), m_nCells(-1), m_CellWidth(-1.f), m_nSubbasin(-1), 
	m_frozenT(NODATA_VALUE), m_ki(NODATA_VALUE), 
	m_soilLayers(NULL), m_soilThick(NULL),m_ks(NULL), m_sat(NULL), m_poreIndex(NULL), m_fc(NULL), m_wp(NULL),
	m_slope(NULL),m_chWidth(NULL), m_streamLink(NULL), m_subbasin(NULL),
	m_flowInIndex(NULL), m_flowInPercentage(NULL),m_routingLayers(NULL), m_nRoutingLayers(-1),
	/// input from other modules
    m_somo(NULL),  m_soilT(NULL),
	/// outputs
	m_qi(NULL), m_qiVol(NULL), m_qiSubbasin(NULL)           
{
}

SSR_NUTR::~SSR_NUTR(void)
{
    if (m_qi != NULL) Release2DArray(m_nCells, m_qi);
    //{
    //    for (int i = 0; i < m_nCells; i++)
    //        delete[] m_qi[i];
    //    delete[] m_qi;
    //}

    if (m_qiVol != NULL) Release2DArray(m_nCells, m_qiVol);
    //{
    //    for (int i = 0; i < m_nCells; i++)
    //        delete[] m_qiVol[i];
    //    delete[] m_qiVol;
    //}

    if (m_qiSubbasin != NULL) Release1DArray(m_qiSubbasin);
        //delete[] m_qiSubbasin;
}

void SSR_NUTR::FlowInSoil(int id)
{
    float s0 = max(m_slope[id], 0.01f);

    //float depth[2];
    //depth[0] = m_upSoilDepth;
    //depth[1] = m_rootDepth[id] - m_upSoilDepth;
    //if (depth[1] < 0)
    //{
    //    ostringstream oss;
    //    oss << "The root depth at cell(" << id << ") is " << m_rootDepth[id] <<
    //    ", and is less than the upper soil depth (" << m_upSoilDepth << endl;
    //    throw ModelException(MID_SSR_NUTR, "Execute", oss.str());
    //}

    float flowWidth = m_CellWidth;
    // there is no land in this cell
    if (m_streamLink[id] > 0)
		flowWidth -= m_chWidth[id];
	// initialize for current cell of current timestep
	for (int j = 0; j < (int)m_soilLayers[id]; j++){
		m_qi[id][j] = 0.f;
		m_qiVol[id][j] = 0.f;
	}
	// return with initial values if flowWidth is less than 0
	if (flowWidth <= 0) return;

	// number of flow in cells
    int nUpstream = (int) m_flowInIndex[id][0];
    for (int j = 0; j < (int)m_soilLayers[id]; j++)
    {
        float smOld = m_somo[id][j];
        //sum the upstream overland flow, m3
        float qUp = 0.f;
        for (int upIndex = 1; upIndex <= nUpstream; ++upIndex)
        {
            int flowInID = (int) m_flowInIndex[id][upIndex];
            //cout << id << "\t" << flowInID << "\t" << m_nCells << endl;
			if(m_qi[flowInID][j] > 0.f)
				qUp += m_qi[flowInID][j];// * m_flowInPercentage[id][upIndex];
            //cout << id << "\t" << flowInID << "\t" << m_nCells << "\t" << m_qi[flowInID][j] << endl;
            //error happens in this place £¿£¿£¿
        }
		// moved up by LJ.
        //if (flowWidth <= 0)
        //{
        //    m_qi[id][j] = 0.f;
        //    m_qiVol[id][j] = 0.f;
        //    continue;
        //}

        // add upstream water to the current cell
        //float soilVolumn = m_soilThick[id][j] / 1000.f * m_CellWidth * flowWidth; //m3
        //m_somo[id][j] += qUp / soilVolumn;
		m_somo[id][j] += qUp; // mm
        //TEST
        if (m_somo[id][j] != m_somo[id][j] || m_somo[id][j] < UTIL_ZERO)
        {
            ostringstream oss;
            oss << "cell id: " << id << "\t layer: " << j << "\nmoisture is less than zero: " << m_somo[id][j] << "\t"
            << smOld << "\nqUp: " << qUp << "\t depth:" << m_soilThick[id][j] << endl;
			cout<<oss.str()<<endl;
            throw ModelException(MID_SSR_NUTR, "Execute:FlowInSoil", oss.str());
        }

		//m_qi[id][j] = 0.f; //reset to default value for current cell
		//m_qiVol[id][j] = 0.f;
        // if soil moisture is below the field capacity, no interflow will be generated

        if (m_somo[id][j] > m_fc[id][j] + m_wp[id][j])
        {
            // for the upper two layers, soil may be frozen
            // also check if there are upstream inflow
            if (j == 0 && m_soilT[id] <= m_frozenT && qUp <= UTIL_ZERO)
                continue;

            float k= 0.f, maxSoilWater= 0.f, soilWater= 0.f, fcSoilWater= 0.f, wpSoilWater= 0.f;
			soilWater = m_somo[id][j];
			maxSoilWater = m_sat[id][j] + m_wp[id][j];
			fcSoilWater = m_fc[id][j] + m_wp[id][j];
			wpSoilWater = m_wp[id][j];
            //the moisture content can exceed the porosity in the way the algorithm is implemented
            if (m_somo[id][j] > maxSoilWater)
                k = m_ks[id][j];
            else
            {
                float dcIndex = 2.f / m_poreIndex[id][j] + 3.f; // pore disconnectedness index
                k = m_ks[id][j] * pow(m_somo[id][j] / maxSoilWater, dcIndex);
				if(k<UTIL_ZERO) k = 0.f;
            }
            m_qi[id][j] = m_ki * s0 * k * m_dt / 3600.f * m_soilThick[id][j] / 1000.f / flowWidth; // the unit is mm

			if (m_qi[id][j] < UTIL_ZERO)
				m_qi[id][j] = 0.f;
			else if (soilWater - m_qi[id][j]> maxSoilWater)
				m_qi[id][j] = soilWater - maxSoilWater;
			else if (soilWater - m_qi[id][j] < UTIL_ZERO)
				m_qi[id][j] = 0.f;
            //else if (soilWater - m_qi[id][j] < fcSoilWater)
            //    m_qi[id][j] = soilWater - fcSoilWater;
			else if (soilWater - m_qi[id][j] < wpSoilWater)
			{
				if(soilWater > wpSoilWater)
					m_qi[id][j] = soilWater - wpSoilWater;
				else
					m_qi[id][j] = 0.f;
			}

            //soilWater -= m_qi[id][j];
            //if (soilWater > maxSoilWater)
            //    m_qi[id][j] += soilWater - maxSoilWater;
			if(m_qi[id][j] < UTIL_ZERO)
				m_qi[id][j] = 0.f;
            m_qiVol[id][j] = m_qi[id][j] / 1000.f * m_CellWidth * flowWidth; //m3
			if(m_qiVol[id][j] < UTIL_ZERO)
				m_qiVol[id][j] = 0.f;
            //Adjust the moisture content in the current layer, and the layer immediately below it
            //m_somo[id][j] -= m_qi[id][j] / depth[j]; 
			m_somo[id][j] -= m_qi[id][j];

            if (m_somo[id][j] != m_somo[id][j] || m_somo[id][j] < UTIL_ZERO)
            {
                ostringstream oss;
                oss << id << "\t" << j << "\nmoisture is less than zero: " << m_somo[id][j] << "\t" << m_soilThick[id][j];
                throw ModelException(MID_SSR_NUTR, "Execute", oss.str());
            }
        }
    }
}

//Execute module
int SSR_NUTR::Execute()
{
    CheckInputData();
    initialOutputs();

    //if (m_qi == NULL)
    //{
    //    m_qi = new float *[m_nCells];
    //    m_qiVol = new float *[m_nCells];

    //    for (int i = 0; i < m_nCells; i++)
    //    {
    //        m_qi[i] = new float[m_nSoilLayers];
    //        m_qiVol[i] = new float[m_nSoilLayers];
    //        for (int j = 0; j < m_nSoilLayers; j++)
    //        {
    //            m_qi[i][j] = 0.f;
    //            m_qiVol[i][j] = 0.f;
    //        }
    //    }
    //}

    for (int iLayer = 0; iLayer < m_nRoutingLayers; ++iLayer)
    {
        // There are not any flow relationship within each routing layer.
        // So parallelization can be done here.
        int nCells = (int) m_routingLayers[iLayer][0];

#pragma omp parallel for
        for (int iCell = 1; iCell <= nCells; ++iCell)
        {
            int id = (int) m_routingLayers[iLayer][iCell];
            FlowInSoil(id);
        }
    }

    //cout << "end flowinsoil" << endl;
    for (int i = 0; i < m_nSubbasin; i++)
        m_qiSubbasin[i] = 0.f;

    
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++)
    {
		float qiAllLayers = 0.f;
        if (m_streamLink[i] > 0)
        {
            qiAllLayers = 0.f;
            for (int j = 0; j < (int)m_soilLayers[i]; j++){
				if (m_qiVol[i][j] > UTIL_ZERO)
					qiAllLayers += m_qiVol[i][j];
			}
            //cout << m_nSubbasin << "\tsubbasin:" << m_subbasin[i] << "\t" << qiAllLayers << endl;
            if (m_nSubbasin > 1)
                m_qiSubbasin[int(m_subbasin[i])] += qiAllLayers;
            else
                m_qiSubbasin[1] += qiAllLayers;
        }
    }
	//for (int i = 0; i <= m_nSubbasin; i++){
	//	if(m_qiSubbasin[i] > 0.)
	//		cout<<m_qiSubbasin[i]<<",";
	//}

    for (int i = 1; i <= m_nSubbasin; i++)
	{
		m_qiSubbasin[i] /= float(m_dt);//86400 secs, m3/s
        m_qiSubbasin[0] += m_qiSubbasin[i];
	}

    return 0;
}

void SSR_NUTR::SetValue(const char *key, float data)
{
    string s(key);
    if (StringMatch(s, VAR_OMP_THREADNUM))
        omp_set_num_threads((int) data);
    else if (StringMatch(s, VAR_T_SOIL))
        m_frozenT = data;
    else if (StringMatch(s, VAR_KI))
        m_ki = data;
    //else if (StringMatch(s, Tag_CellSize))
    //    m_nCells = int(data);
	else if (StringMatch(s, VAR_SUBBSNID_NUM))
		m_nSubbasin = (int)data;
    else if (StringMatch(s, Tag_CellWidth))
        m_CellWidth = data;
    else if (StringMatch(s, Tag_TimeStep))
        m_dt = int(data);
    else
        throw ModelException(MID_SSR_NUTR, "SetValue",
                             "Parameter " + s +
                             " does not exist in current method. Please contact the module developer.");
}

void SSR_NUTR::Set1DData(const char *key, int nRows, float *data)
{
    string s(key);
    CheckInputSize(key, nRows);

    //if (StringMatch(s, VAR_SOILDEPTH))
    //    m_rootDepth = data;
    if (StringMatch(s, VAR_SLOPE))
        m_slope = data;
    else if (StringMatch(s, VAR_CHWIDTH))
        m_chWidth = data;
    else if (StringMatch(s, VAR_STREAM_LINK))
        m_streamLink = data;
    else if (StringMatch(s, VAR_SOTE))
        m_soilT = data;
    else if (StringMatch(s, VAR_SUBBSN))
        m_subbasin = data;
	else if (StringMatch(s, VAR_SOILLAYERS))
		m_soilLayers = data;
    else
        throw ModelException(MID_SSR_NUTR, "SetValue",
                             "Parameter " + s +
                             " does not exist in current method. Please contact the module developer.");

}

void SSR_NUTR::Set2DData(const char *key, int nrows, int ncols, float **data)
{
    string sk(key);
	if (StringMatch(sk, VAR_SOILTHICK))
	{
		CheckInputSize(key, nrows);
		m_nSoilLayers = ncols;
		m_soilThick = data;
	}
    else if (StringMatch(sk, VAR_CONDUCT))
    {
        CheckInputSize(key, nrows);
        m_nSoilLayers = ncols;
        m_ks = data;
    }
    else if (StringMatch(sk, VAR_SOL_UL))
    {
        CheckInputSize(key, nrows);
        m_nSoilLayers = ncols;
        m_sat = data;
    }
    else if (StringMatch(sk, VAR_SOL_AWC))
    {
        CheckInputSize(key, nrows);
        m_nSoilLayers = ncols;
        m_fc = data;
    }
	else if (StringMatch(sk, VAR_SOL_WPMM))
	{
		CheckInputSize(key, nrows);
		m_nSoilLayers = ncols;
		m_wp = data;
	}
    else if (StringMatch(sk, VAR_POREID))
    {
        CheckInputSize(key, nrows);
        m_nSoilLayers = ncols;
        m_poreIndex = data;
    }
    else if (StringMatch(sk, VAR_SOMO))
    {
        CheckInputSize(key, nrows);
        m_nSoilLayers = ncols;
        m_somo = data;
    }
    else if (StringMatch(sk, Tag_ROUTING_LAYERS))
    {
        m_nRoutingLayers = nrows;
        m_routingLayers = data;
    }
    else if (StringMatch(sk, Tag_FLOWIN_INDEX_D8))
    {
        CheckInputSize(key, nrows);
        m_flowInIndex = data;
    }
    //else if (StringMatch(sk, Tag_FLOWIN_PERCENTAGE_DINF))
    //{
    //    CheckInputSize(key, nrows);
    //    m_flowInPercentage = data;
    //}
    else
        throw ModelException(MID_SSR_NUTR, "Set2DData",
                             "Parameter " + sk + " does not exist. Please contact the module developer.");
}


void SSR_NUTR::Get1DData(const char *key, int *n, float **data)
{
    string sk(key);
    if (StringMatch(sk, VAR_SBIF))
        *data = m_qiSubbasin;
    else
        throw ModelException(MID_SSR_NUTR, "Get1DData", "Result " + sk + " does not exist in current module.");
    *n = m_nSubbasin + 1;
}


void SSR_NUTR::Get2DData(const char *key, int *nRows, int *nCols, float ***data)
{
    string sk(key);
    *nRows = m_nCells;
    *nCols = m_nSoilLayers;

    if (StringMatch(sk, VAR_SSRU))
    {
        *data = m_qi;
    }
    else if (StringMatch(sk, VAR_SSRUVOL))
    {
        *data = m_qiVol;
    }
    else
        throw ModelException(MID_SSR_NUTR, "Get2DData", "Output " + sk
                                                    + " does not exist. Please contact the module developer.");

}

bool SSR_NUTR::CheckInputData()
{
    if (m_nCells <= 0)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "The dimension of the input data can not be less than zero.");

    if (m_ki <= 0)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "You have not set Ki.");
    if (FloatEqual(m_frozenT, NODATA_VALUE))
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "You have not set frozen T.");
    if (m_dt <= 0)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "You have not set time step.");
    if (m_CellWidth <= 0)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "You have not set cell width.");
	if (m_nSubbasin < 0)
		throw ModelException(MID_SSR_NUTR, "CheckInputData", "The number of subbasins can not be less than 0.");
    if (m_subbasin == NULL)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "The parameter: subbasin can not be NULL.");
	if (m_soilLayers == NULL)
		throw ModelException(MID_SSR_NUTR, "CheckInputData", "The soil layers number can not be NULL.");
    if (m_soilThick == NULL)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "The soil thickness can not be NULL.");
    if (m_slope == NULL)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "The slope can not be NULL.");
    if (m_ks == NULL)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "The conductivity can not be NULL.");
    if (m_sat == NULL)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "The porosity can not be NULL.");
    if (m_poreIndex == NULL)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "The pore index can not be NULL.");
    if (m_fc == NULL)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "The field capacity can not be NULL.");
	if (m_wp == NULL)
		throw ModelException(MID_SSR_NUTR, "CheckInputData", "The wilting point can not be NULL.");
    if (m_somo == NULL)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "The soil moisture can not be NULL.");
    if (m_soilT == NULL)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "The soil temperature can not be NULL.");
    if (m_chWidth == NULL)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "The channel width can not be NULL.");
    if (m_streamLink == NULL)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "The stream link can not be NULL.");
    if (m_flowInIndex == NULL)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "The flow in index can not be NULL.");
    //if(m_flowInPercentage == NULL)
    //	throw ModelException(MID_SSR_NUTR,"CheckInputData","The flow in percentage can not be NULL.");
    if (m_routingLayers == NULL)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "The routing layers can not be NULL.");
    if (m_nRoutingLayers <= 0)
        throw ModelException(MID_SSR_NUTR, "CheckInputData", "The number of routing layers can not be less than 0.");

    return true;
}

void SSR_NUTR::initialOutputs()
{
	// Read from MongoDB.
    //if (m_nSubbasin <= 0)
    //{
    //    map<int, int> subs;
    //    for (int i = 0; i < m_nCells; i++)
    //    {
    //        subs[int(m_subbasin[i])] = 1;
    //    }
    //    m_nSubbasin = subs.size();
    //}

    if (m_qiSubbasin == NULL) Initialize1DArray(m_nSubbasin + 1, m_qiSubbasin, 0.f);
    //{
    //    m_qiSubbasin = new float[m_nSubbasin + 1];
    //    for (int i = 0; i <= m_nSubbasin; i++)
    //    {
    //        m_qiSubbasin[i] = 0.f;
    //    }
    //}
	if (m_qi == NULL)
	{
		Initialize2DArray(m_nCells, m_nSoilLayers, m_qi, 0.f);
		Initialize2DArray(m_nCells, m_nSoilLayers, m_qiVol, 0.f);
	}
	//{
	//	m_qi = new float *[m_nCells];
	//	m_qiVol = new float *[m_nCells];

	//	for (int i = 0; i < m_nCells; i++)
	//	{
	//		m_qi[i] = new float[m_nSoilLayers];
	//		m_qiVol[i] = new float[m_nSoilLayers];
	//		for (int j = 0; j < m_nSoilLayers; j++)
	//		{
	//			m_qi[i][j] = 0.f;
	//			m_qiVol[i][j] = 0.f;
	//		}
	//	}
	//}
}

bool SSR_NUTR::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
        throw ModelException(MID_SSR_NUTR, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    if (this->m_nCells != n)
    {
        if (this->m_nCells <= 0) this->m_nCells = n;
        else
            throw ModelException(MID_SSR_NUTR, "CheckInputSize", "Input data for " + string(key) +
                                                             " is invalid. All the input data should have same size.");
    }
    return true;
}
