#include <math.h>
#include <iostream>
#include "SUR_MR.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>

SUR_MR::SUR_MR(void):m_nCells(-1), m_tSnow(NODATA), m_t0(NODATA), m_tFrozen(NODATA), m_kRunoff(NODATA),m_pMax(NODATA), 
	m_sFrozen(NULL),  m_runoffCo(NULL), m_initSoilMoisture(NULL), m_soilDepth(NULL), m_fieldCap(NULL), m_porosity(NULL), 
	m_pNet(NULL),  m_tMean(NULL), m_sd(NULL), m_soilTemp(NULL), m_snowAccu(NULL), m_snowMelt(NULL),
	m_pe(NULL), m_infil(NULL),m_soilMoisture(NULL),
	m_dt(-1), m_nSoilLayers(-1),m_upSoilDepth(NULL)
{
}

SUR_MR::~SUR_MR(void)
{
	//// cleanup
	if (m_pe != NULL) delete[] m_pe;
	if (m_infil != NULL) delete[] m_infil;
	if (m_soilMoisture != NULL)
	{
		for (int i = 0; i < m_nCells; i++)
			delete[] m_soilMoisture[i];
		delete[] m_soilMoisture;
		m_soilMoisture = NULL;
	}
}

void SUR_MR::CheckInputData(void)
{
	if(m_date < 0)
		throw ModelException(MID_SUR_MR,"CheckInputData","You have not set the time.");
	if (m_dt < 0)
		throw ModelException(MID_SUR_MR,"CheckInputData","You have not set the time step.");
	if(m_nCells <= 0)
		throw ModelException(MID_SUR_MR,"CheckInputData","The cell number of the input can not be less than zero.");
	if (m_initSoilMoisture == NULL)
		throw ModelException(MID_SUR_MR,"CheckInputData","The initial soil moisture can not be NULL.");
	if (m_runoffCo == NULL)
		throw ModelException(MID_SUR_MR,"CheckInputData","The potential runoff coefficient can not be NULL.");
	if (FloatEqual(m_tSnow, NODATA))
		throw ModelException(MID_SUR_MR,"CheckInputData","The snowfall temperature of the input data can not be NULL.");
	if (FloatEqual(m_tFrozen, NODATA))
		throw ModelException(MID_SUR_MR,"CheckInputData","The soil freezing temperature of the input data can not be NULL.");
	if (FloatEqual(m_t0, NODATA))
		throw ModelException(MID_SUR_MR,"CheckInputData","The snowmelt threshold temperature of the input data can not be NULL.");
	if (FloatEqual(m_kRunoff, NODATA))
		throw ModelException(MID_SUR_MR,"CheckInputData","The runoff exponent of the input data can not be NULL.");
	if (FloatEqual(m_pMax, NODATA))
		throw ModelException(MID_SUR_MR,"CheckInputData","The maximum P corresponding to runoffCo of the input data can not be NULL.");
	if (FloatEqual(m_sFrozen, NODATA))
		throw ModelException(MID_SUR_MR,"CheckInputData","The frozen soil moisture of the input data can not be NULL.");
	if (m_fieldCap == NULL)
		throw ModelException(MID_SUR_MR,"CheckInputData","The water content of soil at field capacity of the input data can not be NULL.");
	if (m_soilDepth == NULL)
		throw ModelException(MID_SUR_MR,"CheckInputData","the input data: Soil depth can not be NULL.");
	if (m_porosity == NULL)
		throw ModelException(MID_SUR_MR,"CheckInputData","The soil porosity of the input data can not be NULL.");
	if (m_tMean == NULL)
		throw ModelException(MID_SUR_MR,"CheckInputData","The mean air temperature of the input data can not be NULL.");
	if (m_soilTemp == NULL)
		throw ModelException(MID_SUR_MR,"CheckInputData","The soil temperature of the input data can not be NULL.");
	if (m_pNet == NULL)
		throw ModelException(MID_SUR_MR,"CheckInputData","The net precipitation can not be NULL.");
	if (m_sd == NULL)
		throw ModelException(MID_SUR_MR,"CheckInputData","The depression capacity can not be NULL.");
	if (m_snowAccu == NULL)
		throw ModelException(MID_SUR_MR,"CheckInputData","The snow accumulation can not be NULL.");
	if (m_snowMelt == NULL)
		throw ModelException(MID_SUR_MR,"CheckInputData","The snow melt can not be NULL.");
}

void SUR_MR::initalOutputs()
{
	if(m_nCells <= 0)				
		throw ModelException(MID_SUR_MR,"CheckInputData","The dimension of the input data can not be less than zero.");
	// allocate the output variables
	if (m_pe == NULL)
	{
		//cout<<"SUR_MR, intialOutputs()"<<endl;
		m_pe = new float[m_nCells];
		m_infil = new float[m_nCells];
		m_soilMoisture = new float*[m_nCells];
		for(int i = 0; i < m_nCells; i++)
			m_soilMoisture[i] = new float[m_nSoilLayers];

		#pragma omp parallel for
		for (int i = 0; i < m_nCells; i++)
		{
			m_pe[i] = 0.0f;
			m_infil[i] = 0.0f;
			for (int j = 0; j < m_nSoilLayers; j++)
				m_soilMoisture[i][j] = m_initSoilMoisture[i] * m_fieldCap[i][j];
		}
	}
}

int SUR_MR::Execute()
{
	CheckInputData();
	initalOutputs();
	m_pMax = m_pMax * m_dt / 86400.f;
	if(m_upSoilDepth == NULL)
		m_upSoilDepth = new float[m_nSoilLayers];
#pragma omp parallel for
	for (int i = 0; i < m_nCells; i++)
	{
		//account for the effects of snow melt and soil temperature
		float t = 0.f, snowMelt = 0.f, snowAcc = 0.f, sd = 0.f;
		if (m_tMean != NULL) t = m_tMean[i];
		if (m_snowMelt != NULL)snowMelt = m_snowMelt[i];
		if (m_snowAccu != NULL)snowAcc = m_snowAccu[i];
		if (m_sd != NULL)sd = m_sd[i];
			
		float hWater = 0.f;
		// snow, without snow melt
		if (t <= m_tSnow)
			hWater = 0.0f;
		// rain on snow, no snow melt
		else if (t > m_tSnow && t <= m_t0 && snowAcc > m_pNet[i])
			hWater = 0.0f;
		else
			hWater = m_pNet[i] + snowMelt + sd;

		if(hWater > 0.f)
		{
			/// Update soil layers from solid two layers to multi-layers by m_nSoilLayers. By LJ
			int curSoilLayers = -1, j;
			m_upSoilDepth[0] =	m_soilDepth[i][0];
			for(j = 1; j < m_nSoilLayers; j++)
			{
				if(!FloatEqual(m_soilDepth[i][j],NODATA_VALUE))
					m_upSoilDepth[j] = m_soilDepth[i][j] - m_soilDepth[i][j-1];
				else
					break;
			}
			curSoilLayers = j;
			float sm = 0.f, por = 0.f;
			for (int j = 0; j < curSoilLayers; j++)
			{
				sm += m_soilMoisture[i][j] * m_upSoilDepth[j];
				por += m_porosity[i][j] * m_upSoilDepth[j];
			}
			
            float smFraction = sm / por;
			smFraction = min(smFraction, 1.0f);

			// for frozen soil
			if (m_soilTemp != NULL && m_soilTemp[i] <= m_tFrozen && smFraction >= m_sFrozen)
			{
				m_pe[i] = m_pNet[i] + snowMelt;
				m_infil[i] = 0.0f;
			}
			//else if (m_soilMoisture[i] >= m_porosity[i])//for saturation overland flow
			//{
			//	m_pe[i] = m_pNet[i] + snowMelt;
			//	m_infil[i] = 0.0f;
			//}
			else
			{
				float alpha = m_kRunoff - (m_kRunoff - 1) * hWater / m_pMax;
				if (hWater >= m_pMax)
					alpha = 1.0f;

				//runoff percentage
				float runoffPercentage;
				if (m_runoffCo[i] > 0.99f)
					runoffPercentage = 1.0f;
				else
					runoffPercentage =  m_runoffCo[i] * pow(smFraction, alpha);

				float surfq = hWater * runoffPercentage;

				m_infil[i] = hWater - surfq;		
				m_pe[i] = surfq;

				// check the output data
				if (m_infil[i] != m_infil[i] || m_infil[i] < 0.0f)
				{
					//string datestr = getDate(&m_date);
					ostringstream oss;
					oss << "Cell id:" << i << "\tPrecipitation(mm) = " << m_pNet[i] << "\thwater = " << hWater 
						<< "\tpercentage:" << runoffPercentage << "\tmoisture = " << sm 
						<< "\tInfiltration(mm) = " << m_infil[i] << "\n";
					throw ModelException(MID_SUR_MR,"Execute","Output data error: infiltration is less than zero. Where:\n"
						+ oss.str() + "Please contact the module developer. ");
				}
			}
		}
		else
		{
			m_pe[i] = 0.0f;
			m_infil[i] = 0.0f;
		}
	}
	return 0;
}

bool SUR_MR::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException(MID_SUR_MR,"CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(m_nCells != n)
	{
		if(m_nCells <=0) m_nCells = n;
		else
		{
			throw ModelException(MID_SUR_MR,"CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}
	return true;
}

void SUR_MR::SetValue(const char* key, float value)
{
	string sk(key);
	if (StringMatch(key, VAR_OMP_THREADNUM))omp_set_num_threads((int)value);
	else if (StringMatch(sk, Tag_HillSlopeTimeStep))	m_dt = value;
	else if (StringMatch(sk, VAR_SNOW_TEMP))m_tSnow = value;
	else if (StringMatch(sk, VAR_T_SOIL))m_tFrozen = value;
	else if (StringMatch(sk, VAR_T0))m_t0 = value;
	else if (StringMatch(sk, VAR_K_RUN))m_kRunoff = value;
	else if (StringMatch(sk, VAR_P_MAX))m_pMax = value;
	else if (StringMatch(sk, VAR_S_FROZEN))m_sFrozen = value;
	else									
		throw ModelException(MID_SUR_MR,"SetValue","Parameter " + sk 
		+ " does not exist in current module. Please contact the module developer.");
}
void SUR_MR::Set1DData(const char* key, int n, float* data)
{
	CheckInputSize(key,n);
	//set the value
	string sk(key);
	if (StringMatch(sk, VAR_RUNOFF_CO))m_runoffCo = data;
	else if (StringMatch(sk,VAR_NEPR))m_pNet = data;
	else if (StringMatch(sk,VAR_TMEAN))m_tMean = data;
	else if (StringMatch(sk, VAR_MOIST_IN))m_initSoilMoisture = data;
	else if (StringMatch(sk, VAR_DPST))m_sd = data;
	else if (StringMatch(sk, VAR_SOTE))m_soilTemp = data;
	else if (StringMatch(sk, VAR_SNAC))m_snowAccu = data;
	else if (StringMatch(sk, VAR_SNME))m_snowMelt = data;
	else									
		throw ModelException(MID_SUR_MR,"Set1DData","Parameter " + sk + 
		" does not exist in current module. Please contact the module developer.");
}

void SUR_MR::Set2DData(const char* key, int nrows, int ncols, float** data)
{
	string sk(key);
	CheckInputSize(key, nrows);
	m_nSoilLayers = ncols;
	if (StringMatch(sk, VAR_POROST))m_porosity = data;
	else if (StringMatch(sk, VAR_FIELDCAP))m_fieldCap = data;
	else if (StringMatch(sk, VAR_SOILDEPTH))m_soilDepth = data;
	else
		throw ModelException(MID_SUR_MR, "Set1DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
}

void SUR_MR::Get1DData(const char* key, int* n, float** data)
{
	initalOutputs();
	string sk(key);
	if (StringMatch(sk, VAR_INFIL))  *data = m_infil;	 //infiltration
	else if (StringMatch(sk, VAR_EXCP))  *data = m_pe; // excess precipitation
	else
		throw ModelException(MID_SUR_MR,"Get1DData","Result " + sk + 
		" does not exist in current module. Please contact the module developer.");
	*n = m_nCells;
}

void SUR_MR::Get2DData(const char* key, int *nRows, int *nCols, float*** data)
{
	initalOutputs();
	string sk(key);
	*nRows = m_nCells;
	*nCols = m_nSoilLayers;

	if (StringMatch(sk, VAR_SOMO)) 
		*data = m_soilMoisture;
	else
		throw ModelException(MID_SUR_MR, "Get2DData", "Output " + sk 
		+ " does not exist. Please contact the module developer.");
}

