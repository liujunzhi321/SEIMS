#include "PER_PI.h"
#include "MetadataInfo.h"
#include "util.h"
#include "ModelException.h"
#include <sstream>
#include <cmath>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>

PER_PI::PER_PI(void) : m_nSoilLayers(-1), m_dt(-1), m_nCells(-1), m_frozenT(NODATA_VALUE),
                       m_ks(NULL), m_sat(NULL), m_poreIndex(NULL), m_fc(NULL), 
					   m_wp(NULL), m_soilThick(NULL),
                       m_infil(NULL), m_soilT(NULL), m_somo(NULL),
                       m_perc(NULL)
{
}

PER_PI::~PER_PI(void)
{
    if (m_perc == NULL) Release2DArray(m_nCells, m_perc);
    //{
    //    for (int i = 0; i < m_nCells; i++)
    //        delete[] m_perc[i];
    //    delete[] m_perc;
    //    m_perc = NULL;
    //}
}
void PER_PI::initialOutputs()
{
	if (m_perc == NULL)
		Initialize2DArray(m_nCells, m_nSoilLayers, m_perc, 0.f);
}
int PER_PI::Execute()
{
    CheckInputData();
	initialOutputs();
    
#pragma omp parallel for
    for (int i = 0; i < m_nCells; i++)
    {
        float k = 0.f, maxSoilWater = 0.f, fcSoilWater = 0.f;
		float swater = 0.f, wpSoilWater = 0.f;        
        /// firstly, assume all infiltrated water is added to the first soil layer.
		m_somo[i][0] += m_infil[i];
		/// secondly, model water percolation across layers
        for (int j = 0; j < (int)m_soilLayers[i]; j++)
        {
            // for the upper two layers, soil may be frozen
            // No movement if soil moisture is below field capacity
            if (j == 0 && m_soilT[i] <= m_frozenT) 
                continue;
            if (m_somo[i][j] > m_fc[i][j] + m_wp[i][j])
            {
                swater = m_somo[i][j];
                maxSoilWater = m_sat[i][j] + m_wp[i][j];
                fcSoilWater = m_fc[i][j] + m_wp[i][j];
				wpSoilWater = m_wp[i][j];
                //the moisture content can exceed the porosity in the way the algorithm is implemented
                if (swater > maxSoilWater) //(m_somo[i][j] > m_porosity[i][j])
                    k = m_ks[i][j];
                else
                {
                    float dcIndex = 2.f / m_poreIndex[i][j] + 3.f; // pore disconnectedness index
					k = m_ks[i][j] * pow(swater / maxSoilWater, dcIndex);
                    //k = m_ks[i][j] * pow(m_somo[i][j] / m_porosity[i][j], dcIndex);
                }

                m_perc[i][j] = k * m_dt / 3600.f;  /// mm
				//if (m_perc[i][j] < 0.f)
				//	m_perc[i][j] = 0.f;
                if (swater - m_perc[i][j] > maxSoilWater)
                    m_perc[i][j] = swater - maxSoilWater;
                else if (swater - m_perc[i][j] < fcSoilWater)
                    m_perc[i][j] = swater - fcSoilWater;

                //Adjust the moisture content in the current layer, and the layer immediately below it
                m_somo[i][j] -= m_perc[i][j];// / m_soilThick[i][j];
                if (j < m_soilLayers[i] - 1)
                    m_somo[i][j + 1] += m_perc[i][j];// / m_soilThick[i][j + 1];

				
                //if (m_somo[i][j] != m_somo[i][j] || m_somo[i][j] < 0.f)
                //{
                //    cout << MID_PER_PI << " CELL:" << i << ", Layer: " << j << "\tPerco:" << swater << "\t" <<
                //    fcSoilWater << "\t" << m_perc[i][j] << "\t" << m_soilThick[i][j] << "\tValue:" << m_somo[i][j] <<
                //    endl;
                //    throw ModelException(MID_PER_PI, "Execute", "moisture is less than zero.");
                //}
            }
			else
			{
				for (int j = 0; j < (int)m_soilLayers[i]; j++)
					m_perc[i][j] = 0.f;
			}
			for (int j = (int)m_soilLayers[i]; j < m_nSoilLayers; j++)
				m_perc[i][j] = NODATA_VALUE;
			if(m_perc[i][(int)m_soilLayers[i]-1] > 0.f) /// If there have water percolated down to groundwater
			{
				/// update groundwater variables
			}
        }
    }
    return 0;
}


void PER_PI::Get2DData(const char *key, int *nRows, int *nCols, float ***data)
{
    string sk(key);
    *nRows = m_nCells;
    *nCols = m_nSoilLayers;
    if (StringMatch(sk, VAR_PERCO)) *data = m_perc;
	else if (StringMatch(sk, VAR_SOMO)) *data = m_somo;
    else
        throw ModelException(MID_PER_PI, "Get2DData", "Output " + sk
                                                      + " does not exist. Please contact the module developer.");
}

void PER_PI::Set1DData(const char *key, int nRows, float *data)
{
    string sk(key);

    CheckInputSize(key, nRows);

    if (StringMatch(sk, VAR_SOTE)) m_soilT = data;
	else if (StringMatch(sk, VAR_INFIL)) m_infil = data;
	else if (StringMatch(sk, VAR_SOILLAYERS)) m_soilLayers = data;
    else
        throw ModelException(MID_PER_PI, "Set1DData",
                             "Parameter " + sk +
                             " does not exist in current module. Please contact the module developer.");
}

void PER_PI::Set2DData(const char *key, int nrows, int ncols, float **data)
{
    string sk(key);
    CheckInputSize(key, nrows);
    m_nSoilLayers = ncols;

    if (StringMatch(sk, VAR_CONDUCT)) m_ks = data;
	else if (StringMatch(sk, VAR_SOILTHICK)) m_soilThick = data;
    //else if (StringMatch(sk, VAR_POROST)) m_sat = data;
    //else if (StringMatch(sk, VAR_FIELDCAP)) m_fc = data;
	//else if (StringMatch(sk, VAR_WILTPOINT)) m_wp = data;
	else if (StringMatch(sk, VAR_SOL_UL)) m_sat = data;
	else if (StringMatch(sk, VAR_SOL_AWC)) m_fc = data;
	else if (StringMatch(sk, VAR_SOL_WPMM)) m_wp = data;
    else if (StringMatch(sk, VAR_POREID)) m_poreIndex = data;
    else if (StringMatch(sk, VAR_SOMO)) m_somo = data;
    else
        throw ModelException(MID_PER_PI, "Set2DData",
                             "Parameter " + sk + " does not exist. Please contact the module developer.");
}

void PER_PI::SetValue(const char *key, float data)
{
    string s(key);
    if (StringMatch(s, Tag_TimeStep)) m_dt = int(data);
    else if (StringMatch(s, VAR_T_SOIL)) m_frozenT = data;
    else if (StringMatch(s, VAR_OMP_THREADNUM))omp_set_num_threads((int) data);
    else
        throw ModelException(MID_PER_PI, "SetValue",
                             "Parameter " + s +
                             " does not exist in current module. Please contact the module developer.");
}

bool PER_PI::CheckInputData()
{
    if (m_date <= 0)
        throw ModelException(MID_PER_PI, "CheckInputData", "You have not set the time.");
    if (m_nCells <= 0)
        throw ModelException(MID_PER_PI, "CheckInputData",
                             "The dimension of the input data can not be less than zero.");
    if (m_dt <= 0)
        throw ModelException(MID_PER_PI, "CheckInputData", "The time step can not be less than zero.");

    if (m_ks == NULL)
        throw ModelException(MID_PER_PI, "CheckInputData", "The Conductivity can not be NULL.");
    if (m_sat == NULL)
        throw ModelException(MID_PER_PI, "CheckInputData", "The Porosity can not be NULL.");
    if (m_poreIndex == NULL)
        throw ModelException(MID_PER_PI, "CheckInputData", "The Pore index can not be NULL.");
	if (m_fc == NULL)
		throw ModelException(MID_PER_PI, "CheckInputData", "The field capacity can not be NULL.");
	if (m_wp == NULL)
		throw ModelException(MID_PER_PI, "CheckInputData", "The wilting point can not be NULL.");
    if (m_soilThick == NULL)
        throw ModelException(MID_PER_PI, "CheckInputData", "The soil depth can not be NULL.");
    if (m_soilT == NULL)
        throw ModelException(MID_PER_PI, "CheckInputData", "The soil temperature can not be NULL.");
    if (m_infil == NULL)
        throw ModelException(MID_PER_PI, "CheckInputData", "The infiltration can not be NULL.");
    if (FloatEqual(m_frozenT, NODATA_VALUE))
        throw ModelException(MID_PER_PI, "CheckInputData", "The threshold soil freezing temperature can not be NULL.");
    if (m_somo == NULL)
        throw ModelException(MID_PER_PI, "CheckInputData", "The Moisture can not be NULL.");
    return true;
}

bool PER_PI::CheckInputSize(const char *key, int n)
{
    if (n <= 0)
        throw ModelException(MID_PER_PI, "CheckInputSize",
                             "Input data for " + string(key) + " is invalid. The size could not be less than zero.");
    if (this->m_nCells != n)
    {
        if (this->m_nCells <= 0) this->m_nCells = n;
        else
            throw ModelException(MID_PER_PI, "CheckInputSize", "Input data for " + string(key) +
                                                               " is invalid. All the input data should have same size.");
    }
    return true;
}
