#pragma once
#include <iostream>
#include "Biomass_EPIC.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "utils.h"
#include "util.h"
#include "PlantGrowthCommon.h"
#include "ClimateParams.h"
#include <omp.h>
#include <cmath>

using namespace std;

Biomass_EPIC::Biomass_EPIC(void):m_nCells(-1), m_nClimDataYrs(-1), m_co2(NODATA), m_tMean(NULL), m_tMin(NULL), m_SR(NULL),
	m_soilLayers(-1), m_NUpDis(NODATA), m_PUpDis(NODATA), m_NFixCoef(NODATA), m_NFixMax(NODATA), m_soilRD(NODATA), m_tMeanAnn(NODATA),
	m_nSoilLayers(NULL), m_soilZMX(NULL), m_soilALB(NULL),m_soilDepth(NULL), m_soilRsd(NULL), m_soilAWC(NULL), m_totSoilAWC(NULL), m_totSoilSat(NULL), m_somo(NULL), m_totSOMO(NULL),m_soilCov(NULL),
	m_igro(NULL), m_landCoverCls(NULL), m_aLAIMin(NULL), m_BIOE(NULL), m_BIOEHI(NULL), m_maxLAI(NULL), m_maxBiomass(NULL),
	m_frPlantN1(NULL), m_frPlantN2(NULL), m_frPlantN3(NULL), m_frPlantP1(NULL), m_frPlantP2(NULL), m_frPlantP3(NULL), 
	m_chtMax(NULL), m_co2Hi(NULL), m_frDLAI(NULL), m_epco(NULL), m_lightExtCoef(NULL), m_frGrowOptLAI1(NULL), m_frGrowOptLAI2(NULL), 
	m_hvstIdx(NULL), m_frMaxLAI1(NULL), m_frMaxLAI2(NULL), m_matYrs(NULL), m_tBase(NULL), m_tOpt(NULL), m_wavp(NULL), 
	m_cht(NULL), m_initTreeMatYr(NULL), m_initBiomass(NULL), m_initLAI(NULL), m_PHUPlt(NULL),  m_dormIdx(NULL),m_pltET(NULL), m_pltPET(NULL),
	m_VPD(NULL),m_PET(NULL),  m_ppt(NULL), m_soilESDay(NULL), m_soilNO3(NULL), m_soilPsol(NULL), m_snowAcc(NULL),
	m_LAIDay(NULL), m_frPHUacc(NULL), m_LAIYrMax(NULL), m_hvstIdxAdj(NULL), m_LAIMaxFr(NULL), m_oLAI(NULL), m_soilStRD(NULL), 
	m_plantEPDay(NULL), m_frRoot(NULL), m_fixN(NULL),  m_plantUpTkN(NULL),  m_plantUpTkP(NULL),  m_plantN(NULL),  m_plantP(NULL),  m_frPlantP(NULL), 
	m_NO3Defic(NULL),  m_frStrsAe(NULL),  m_frStrsN(NULL),  m_frStrsP(NULL),  m_frStrsTmp(NULL),  m_frStrsWa(NULL),
	m_biomassDelta(NULL),  m_biomass(NULL)
{
	uobw = 0.0;
	ubw = 10.0; /// the uptake distribution for water is hardwired, users are not allowed to modify
	uobw = PGCommon::getNormalization(ubw);
}

Biomass_EPIC::~Biomass_EPIC(void)
{
	//// cleanup output variables
	if(m_somo != NULL)
	{
		for(int i = 0; i < m_soilLayers; i++)
			delete [] m_somo[i];
		m_somo = NULL;
	}
	if(m_soilNO3 != NULL)
	{
		for(int i = 0; i < m_soilLayers; i++)
			delete [] m_soilNO3[i];
		m_soilNO3 = NULL;
	}
	if(m_soilPsol != NULL)
	{
		for(int i = 0; i < m_soilLayers; i++)
			delete [] m_soilPsol[i];
		m_soilPsol = NULL;
	}
	if(m_soilRsd != NULL)
	{
		for(int i = 0; i < m_soilLayers; i++)
			delete [] m_soilRsd[i];
		m_soilRsd = NULL;
	}
	if(m_totSOMO != NULL) delete[] m_totSOMO;
	if(m_soilCov != NULL) delete[] m_soilCov;
	if(m_LAIDay != NULL) delete [] m_LAIDay;
	if(m_cht != NULL) delete[] m_cht;
	if(m_dormIdx != NULL) delete[] m_dormIdx;
	if(m_LAIYrMax != NULL) delete[] m_LAIYrMax;
	if(m_frPHUacc != NULL) delete[] m_frPHUacc;
	if(m_pltET != NULL) delete[] m_pltET;
	if(m_pltPET != NULL) delete [] m_pltPET;
	if(m_plantN != NULL) delete [] m_plantN;
	if(m_plantP != NULL) delete [] m_plantP;
	if(m_plantEPDay != NULL) delete [] m_plantEPDay;
    if(m_biomass != NULL) delete [] m_biomass;
}

void Biomass_EPIC::SetValue(const char* key, float data)
{
	string sk(key);
	if(StringMatch(sk,VAR_CO2))	m_co2 = data;
	else if (StringMatch(sk, VAR_OMP_THREADNUM)) omp_set_num_threads((int)data);
	else if(StringMatch(sk,VAR_NUPDIS))	m_NUpDis = data;
	else if(StringMatch(sk,VAR_PUPDIS))	m_PUpDis = data;
	else if(StringMatch(sk,VAR_NFIXCO))	m_NFixCoef = data;
	else if(StringMatch(sk,VAR_NFIXMX))	m_NFixMax = data;
	else if(StringMatch(sk, VAR_TMEAN_ANN))m_tMeanAnn = data;
	else	
		throw ModelException(MID_BIO_EPIC,"SetValue","Parameter " + sk 
		+ " does not exist in current module. Please contact the module developer.");
}

bool Biomass_EPIC::CheckInputSize(const char* key, int n)
{
	if(n <= 0) throw ModelException(MID_BIO_EPIC,"CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
	if(m_nCells != n)
	{
		if(m_nCells <= 0) 
			m_nCells = n;
		else
		{
			throw ModelException(MID_BIO_EPIC,"CheckInputSize","Input data for "+string(key) +" is invalid. All the input raster data should have same size.");
			return false;
		}
	}
	return true;
}

void Biomass_EPIC::Set1DData(const char* key, int n, float* data)
{
	string sk(key);
	CheckInputSize(key,n);
	//// climate
	if(StringMatch(sk, DataType_MeanTemperature)) m_tMean = data;
	else if(StringMatch(sk, DataType_MinimumTemperature)) m_tMin = data;
	else if(StringMatch(sk, DataType_SolarRadiation)) m_SR = data;
	//// soil properties and water related
	else if(StringMatch(sk, VAR_SOILLAYERS)) m_nSoilLayers = data;
	else if(StringMatch(sk, VAR_SOL_ZMX)) m_soilZMX = data;
	else if(StringMatch(sk, VAR_SOL_ALB)) m_soilALB = data;
	else if(StringMatch(sk, VAR_SOMO_TOT)) m_totSOMO = data;
	else if(StringMatch(sk, VAR_SOL_SUMAWC)) m_totSoilAWC = data;
	else if(StringMatch(sk, VAR_SOL_SUMSAT)) m_totSoilSat = data;
	else if(StringMatch(sk,VAR_PET))	m_PET = data;
	else if(StringMatch(sk,VAR_VPD))	m_VPD = data;
	else if(StringMatch(sk,VAR_PPT))	 m_ppt = data;
	else if(StringMatch(sk,VAR_SOET)) m_soilESDay = data;
	else if(StringMatch(sk, VAR_SOL_COV)) m_soilCov = data;
	else if(StringMatch(sk, VAR_SNAC)) m_snowAcc = data;
	//// land cover
	else if(StringMatch(sk, VAR_IGRO)) m_igro = data;
	else if(StringMatch(sk, VAR_IDC))	m_landCoverCls = data;
	else if(StringMatch(sk, VAR_ALAIMIN)) m_aLAIMin = data;
	else if(StringMatch(sk,VAR_BIO_E))	m_BIOE = data;
	else if(StringMatch(sk,VAR_BIOEHI))	 m_BIOEHI = data;
	else if(StringMatch(sk, VAR_BLAI)) m_maxLAI = data;
	else if(StringMatch(sk, VAR_BMX_TREES)) m_maxBiomass = data;
	else if(StringMatch(sk,VAR_BN1))	m_frPlantN1 = data;
	else if(StringMatch(sk,VAR_BN2))	m_frPlantN2 = data;
	else if(StringMatch(sk,VAR_BN3))	m_frPlantN3 = data;
	else if(StringMatch(sk,VAR_BP1))	m_frPlantP1 = data;
	else if(StringMatch(sk,VAR_BP2))	m_frPlantP2 = data;
	else if(StringMatch(sk,VAR_BP3))	m_frPlantP3 = data;
	else if(StringMatch(sk,VAR_CHTMX))	m_chtMax = data;
	else if(StringMatch(sk,VAR_CO2HI))	 m_co2Hi = data;
	else if(StringMatch(sk,VAR_DLAI)) m_frDLAI = data;
	else if(StringMatch(sk,VAR_EXT_COEF))	m_lightExtCoef = data;
	else if(StringMatch(sk,VAR_FRGRW1))	 m_frGrowOptLAI1 = data;
	else if(StringMatch(sk,VAR_FRGRW2))	 m_frGrowOptLAI2 = data;
	else if(StringMatch(sk,VAR_HVSTI))	m_hvstIdx = data;
	else if(StringMatch(sk,VAR_LAIMX1)) m_frMaxLAI1 = data;
	else if(StringMatch(sk,VAR_LAIMX2)) m_frMaxLAI2 = data;
	else if(StringMatch(sk,VAR_MAT_YRS)) m_matYrs = data;
	else if(StringMatch(sk,VAR_T_BASE))	m_tBase = data;
	else if(StringMatch(sk,VAR_T_OPT))	m_tOpt = data;
	else if(StringMatch(sk,VAR_WAVP))	 m_wavp = data;
	//// management 
	else if(StringMatch(sk,VAR_EPCO))	m_epco = data;
	else if(StringMatch(sk,VAR_TREEYRS)) m_initTreeMatYr = data;
	else if(StringMatch(sk,VAR_LAIINIT)) m_initLAI = data;
	else if(StringMatch(sk,VAR_BIOINIT))	m_initBiomass = data;
	else if(StringMatch(sk,VAR_PHUPLT))	m_PHUPlt = data;
	else if(StringMatch(sk,VAR_CHT))	 m_cht = data;
	else if(StringMatch(sk,VAR_DORMI))	 m_dormIdx = data;
	
	//// need to be initialized in this module
	else if(StringMatch(sk,VAR_LAIDAY))	m_LAIDay = data;
	else if(StringMatch(sk,VAR_FR_PHU_ACC))	m_frPHUacc = data;
	else if(StringMatch(sk,VAR_PLTET_TOT))	 m_pltET = data;
	else if(StringMatch(sk,VAR_PLTPET_TOT))	 m_pltPET = data;
	else									
		throw ModelException(MID_BIO_EPIC, "Set1DData", "Parameter " + sk + 
		" does not exist in current module. Please contact the module developer.");
}

bool Biomass_EPIC::CheckInputSize2D(const char* key, int n, int col)
{
	CheckInputSize(key,n);
	if(col <= 0) throw ModelException(MID_BIO_EPIC,"CheckInputSize2D","Input data for "+string(key) +" is invalid. The layer number could not be less than zero.");
	if(m_soilLayers != col)
	{
		if(m_soilLayers <= 0) 
			m_soilLayers = col;
		else
		{
			throw ModelException(MID_BIO_EPIC,"CheckInputSize2D","Input data for "+string(key) +" is invalid. All the layers of input 2D raster data should have same size.");
			return false;
		}
	}
	return true;
}

void Biomass_EPIC::Set2DData(const char* key, int nRows, int nCols, float** data)
{
	string sk(key);
	CheckInputSize2D(key,nRows,nCols);
	if(StringMatch(sk, VAR_SOILDEPTH)) m_soilDepth = data;
	else if(StringMatch(sk, VAR_SOL_RSD)) m_soilRsd = data;
	else if(StringMatch(sk, VAR_SOL_AWC)) m_soilAWC = data;
	else if(StringMatch(sk, VAR_SOMO)) m_somo = data;
	else if(StringMatch(sk, VAR_SOL_NO3)) m_soilNO3 = data;
	else if(StringMatch(sk, VAR_SOL_SOLP)) m_soilPsol = data;
	else									
		throw ModelException(MID_BIO_EPIC, "Set2DData", "Parameter " + sk + 
		" does not exist in current module. Please contact the module developer.");
}

bool Biomass_EPIC::CheckInputData(void)
{
	/// DT_Single
	if(m_date <= 0) throw ModelException(MID_BIO_EPIC,"CheckInputData","You have not set the time.");
	if(m_nCells <= 0) throw ModelException(MID_BIO_EPIC,"CheckInputData","The dimension of the input data can not be less than zero.");
	if(m_soilLayers <= 0)  throw ModelException(MID_BIO_EPIC,"CheckInputData","The layer number of the input 2D raster data can not be less than zero.");
	if(FloatEqual(m_co2, NODATA))	throw ModelException(MID_BIO_EPIC,"CheckInputData","The ambient atmospheric CO2 concentration must be provided.");
	if(FloatEqual(m_NUpDis, NODATA))	throw ModelException(MID_BIO_EPIC,"CheckInputData","The nitrogen uptake distribution parameter must be provided.");
	if(FloatEqual(m_PUpDis, NODATA))	throw ModelException(MID_BIO_EPIC,"CheckInputData","The phosphorus uptake distribution parameter must be provided.");
	if(FloatEqual(m_NFixCoef, NODATA))	throw ModelException(MID_BIO_EPIC,"CheckInputData","The nitrogen fixation coefficient must be provided.");
	if(FloatEqual(m_NFixMax, NODATA))	throw ModelException(MID_BIO_EPIC,"CheckInputData","The maximum daily-N fixation must be provided.");
	if(FloatEqual(m_tMeanAnn, NODATA))	throw ModelException(MID_BIO_EPIC,"CheckInputData","The annual mean air temperature must be provided.");
	/// DT_Raster1D
	if(m_tMin == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The min temperature data can not be NULL.");
	if(m_tMean == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The mean temperature data can not be NULL.");
	if(m_SR == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The solar radiation data can not be NULL.");

	if(m_nSoilLayers == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The soil layers data can not be NULL.");
	if(m_soilZMX == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The maximum rooting depth in soil profile can not be NULL.");
	if(m_soilALB == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The albedo when soil is moist can not be NULL.");
	if(m_totSOMO == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The amount of water stored in the soil profile can not be NULL.");
	if(m_totSoilAWC == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The amount of water held in soil profile at field capacity can not be NULL.");
	if(m_totSoilSat == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The amount of water held in soil profile at saturation can not be NULL.");
	if(m_soilCov == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The amount of residue in soil surface can not be NULL.");
	if(m_PET == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The PET can not be NULL.");
	if(m_VPD == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The Vapor pressure deficit can not be NULL.");
	if(m_ppt == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The potential plant et can not be NULL.");
	if(m_soilESDay == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The actual soil et can not be NULL.");

	if(m_igro == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The land cover status code can not be NULL.");
	if(m_landCoverCls == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The land cover/plant classification can not be NULL.");
	if(m_aLAIMin == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The minimum LAI during winter dormant period can not be NULL.");
	if(m_BIOE == NULL)	throw ModelException(MID_BIO_EPIC,"CheckInputData","The RUE at ambient atmospheric CO2 concentration data can not be NULL.");
	if(m_BIOEHI == NULL)throw ModelException(MID_BIO_EPIC,"CheckInputData","The  RUE at elevated atmospheric CO2 concentration data can not be NULL.");
	if(m_maxLAI == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The max LAI data can not be NULL.");
	if(m_maxBiomass == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The maximum biomass for a forest can not be NULL.");
	if(m_frPlantN1 == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The normal fraction of nitrogen in the plant biomass at emergence data can not be NULL.");
	if(m_frPlantN2 == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The normal fraction of nitrogen in the plant biomass at 50% maturity data can not be NULL.");
	if(m_frPlantN3 == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The normal fraction of nitrogen in the plant biomass at maturity data can not be NULL.");
	if(m_frPlantP1 == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The normal fraction of phosphorus in the plant biomass at emergence data can not be NULL.");
	if(m_frPlantP2 == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The normal fraction of phosphorus in the plant biomass at 50% maturity data can not be NULL.");
	if(m_frPlantP3 == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The normal fraction of phosphorus in the plant biomass at maturity data can not be NULL.");
	if(m_chtMax == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The maximum canopy height can not be NULL.");
	if(m_co2Hi == NULL)throw ModelException(MID_BIO_EPIC,"CheckInputData","The elevated atmospheric CO2 concentration data can not be NULL.");
	if(m_frDLAI == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The PHU fraction when senescence becomes dominant data can not be NULL.");
	if(m_lightExtCoef == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The  light extinction coefficient data can not be NULL.");
	if(m_frGrowOptLAI1 == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The PHU fraction for the 1st point data can not be NULL.");
	if(m_frGrowOptLAI2 == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The PHU fraction for the 2nd point data can not be NULL.");
	if(m_hvstIdx == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The harvest index can not be NULL.");
	if(m_frMaxLAI1 == NULL)	throw ModelException(MID_BIO_EPIC,"CheckInputData","The LAI fraction for the 1st point data can not be NULL.");
	if(m_frMaxLAI2 == NULL)	throw ModelException(MID_BIO_EPIC,"CheckInputData","TheLAI fraction for the 2nd point  data can not be NULL.");
	if(m_matYrs == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The full development year data can not be NULL.");
	if(m_tBase == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The base temperature data can not be NULL.");
	if(m_tOpt == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The optimal temperature for plant growth can not be NULL.");
	if(m_wavp == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The rate of decline in rue per unit increase in VPD data can not be NULL.");
	
	if(m_epco == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The plant water uptake compensation factor can not be NULL.");
	if(m_initTreeMatYr == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The initial age of tress can not be NULL.");
	if(m_initLAI == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The initial leaf area index of transplants can not be NULL.");
	if(m_initBiomass == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The initial dry weight biomass can not be NULL.");
	if(m_PHUPlt == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The total number of heat unites (hours) needed to bring plant to maturity can not be NULL.");
	if(m_cht == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The canopy height for the day can not be NULL.");
	if(m_dormIdx == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The dormancy status code can not be NULL.");

	/// DT_Raster2D
	if(m_soilDepth == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The soil depth data can not be NULL.");
	if(m_soilRsd == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The organic matter in the soil classified as residue can not be NULL.");
	if(m_soilAWC == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The water available to plants in soil layer at field capacity can not be NULL.");
	if(m_somo == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The soil moisture in soil layers can not be NULL.");
	if(m_soilNO3 == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The nitrogen stored in the nitrate pool in soil layer can not be NULL.");
	if(m_soilPsol == NULL) throw ModelException(MID_BIO_EPIC,"CheckInputData","The phosphorus stored in solution can not be NULL.");

	return true;
}

void Biomass_EPIC::initialOutputs()
{
	if (m_LAIDay == NULL && m_initLAI != NULL)
	{
		m_LAIDay = new float[m_nCells];
		for(int i = 0; i < m_nCells; i++)
			m_LAIDay[i] = m_initLAI[i];
	}
	if (m_LAIYrMax ==NULL)		
	{
		m_LAIYrMax = new float[m_nCells];
		for (int i = 0; i < m_nCells; i++)
		{
			if (PGCommon::IsTree((int)(m_landCoverCls[i])))
				m_LAIYrMax[i] *= m_initTreeMatYr[i] / m_matYrs[i];
			else
				m_LAIYrMax[i] = m_LAIDay[i];
		}
	}
	if(m_frPHUacc == NULL)
	{
		m_frPHUacc = new float[m_nCells];
		for (int i = 0; i < m_nCells; i++) 
			m_frPHUacc[i]=0.0f;
	}
	if(m_pltET == NULL || m_pltPET == NULL)
	{
		m_pltET = new float[m_nCells];
		m_pltPET = new float[m_nCells];
		for(int i = 0; i < m_nCells; i++)
		{
			m_pltET[i] = 0.f;
			m_pltPET[i] = 0.f;
		}
	}
	if (m_hvstIdxAdj ==NULL)		
	{
		m_hvstIdxAdj = new float[m_nCells];
		for (int i = 0; i < m_nCells; i++) 
			m_hvstIdxAdj[i]=0.0f;
	}
	if (m_LAIMaxFr == NULL || m_oLAI ==NULL)		
	{
		m_LAIMaxFr = new float[m_nCells];
		m_oLAI = new float[m_nCells];
		for(int i = 0; i < m_nCells; i++)
		{
			m_LAIMaxFr[i] = 0.f;
			m_oLAI[i] = 0.f;
		}
	}
	if (m_soilStRD ==NULL)		
	{
		m_soilStRD = new float[m_nCells];
		for (int i = 0; i < m_nCells; i++) 
			m_soilStRD[i]=0.0f;
	}
	if (m_plantEPDay ==NULL)		
	{
		m_plantEPDay = new float[m_nCells];
		for (int i = 0; i < m_nCells; i++) 
			m_plantEPDay[i]=0.0f;
	}
	if (m_frRoot ==NULL)		
	{
		m_frRoot = new float[m_nCells];
		for (int i = 0; i < m_nCells; i++) 
			m_frRoot[i]=0.0f;
	}
	if (m_fixN ==NULL)		
	{
		m_fixN = new float[m_nCells];
		for (int i = 0; i < m_nCells; i++) 
			m_fixN[i]=0.0f;
	}
	if (m_plantUpTkN ==NULL || m_plantUpTkP == NULL)		
	{
		m_plantUpTkN = new float[m_nCells];
		m_plantUpTkP = new float[m_nCells];
		for(int i = 0; i < m_nCells; i++)
		{
			m_plantUpTkN[i] = 0.f;
			m_plantUpTkP[i] = 0.f;
		}
	}
	if (m_plantN ==NULL || m_plantP == NULL)		
	{
		m_plantN = new float[m_nCells];
		m_plantP = new float[m_nCells];
		for(int i = 0; i < m_nCells; i++)
		{
			m_plantN[i] = 0.f;
			m_plantP[i] = 0.f;
		}
	}
	if (m_frPlantN ==NULL || m_frPlantP == NULL)		
	{
		m_frPlantN = new float[m_nCells];
		m_frPlantP = new float[m_nCells];
		for(int i = 0; i < m_nCells; i++)
		{
			m_frPlantN[i] = 0.f;
			m_frPlantP[i] = 0.f;
		}
	}
	if (m_NO3Defic == NULL || m_frStrsAe == NULL || m_frStrsN == NULL || m_frStrsP == NULL  || m_frStrsTmp == NULL || m_frStrsWa == NULL)		
	{
		m_NO3Defic = new float[m_nCells];
		m_frStrsAe = new float[m_nCells];
		m_frStrsN = new float[m_nCells];
		m_frStrsP = new float[m_nCells];
		m_frStrsTmp = new float[m_nCells];
		m_frStrsWa = new float[m_nCells];
		for(int i = 0; i < m_nCells; i++)
		{
			m_NO3Defic[i] = 0.f;
			m_frStrsAe[i] = 0.f;
			m_frStrsN[i] = 0.f;
			m_frStrsP[i] = 0.f;
			m_frStrsTmp[i] = 0.f;
			m_frStrsWa[i] = 0.f;
		}
	}

	if(m_biomassDelta == NULL || m_biomass == NULL)
	{
		m_biomassDelta = new float[m_nCells];
		m_biomass = new float[m_nCells];
		for (int i = 0; i < m_nCells; i++) 
		{
			m_biomassDelta[i]=0.0f;
			m_biomass[i] = m_initBiomass[i];
		}
	}
}

void Biomass_EPIC::DistributePlantET(int i)
{
	float sum, sump, gx;
	/// fraction of water uptake by plants achieved
	/// where the reduction is caused by low water content
	float reduc;
	/// water uptake by plants in each soil layer
	float* wuse = new float[(int)m_nSoilLayers[i]];
	/// water uptake by plants from all layers
	float xx;
	int j,k,ir;
	int idc = int(m_landCoverCls[i]);
	if(idc == 1 || idc == 2 || idc == 4 || idc == 5)
	{
		m_soilRD = 2.5 * m_frPHUacc[i] * m_soilZMX[i];
		if(m_soilRD > m_soilZMX[i]) m_soilRD = m_soilZMX[i];
		if(m_soilRD < 10.f) m_soilRD = 10.f;   /// minimum root depth is 10mm
	}
	else
		m_soilRD = m_soilZMX[i];
	m_soilStRD[i] = m_soilRD;
	if(m_ppt[i] <= 0.01)
		m_frStrsWa[i] = 1.f;
	else
	{
		/// initialize variables
		gx = 0.f;
		ir = 0;
		sump = 0.f;
		for(int j = 0; j < (int)m_nSoilLayers[i]; j++)
			wuse[j] = 0.f;
		xx = 0.f;
	}
	/// compute aeration stress
	if (m_totSOMO[i] > m_totSoilAWC[i])
	{
		float satco = (m_totSOMO[i] - m_totSoilAWC[i])/(m_totSoilSat[i] - m_totSoilAWC[i]);
		float pl_aerfac = 0.85;
		float scparm = 100. * (satco - pl_aerfac) / (1.0001 - pl_aerfac);
		if(scparm > 0.f)
			m_frStrsAe[i] = 1. - (scparm / (scparm + exp(2.9014 - 0.03867 * scparm)));
		else
			m_frStrsAe[i] = 1.f;
	}
	for(int j = 0; j < (int)m_nSoilLayers[i]; j++)
	{
		if(ir > 0) break;
		if (m_soilRD <= m_soilDepth[i][j])
		{
			gx = m_soilRD;
			ir = j;
		}
		else
			gx = m_soilDepth[i][j];
		sum = 0.f;
		if(m_soilRD <= 0.01)
			sum = m_ppt[i] / uobw;
		else
			sum = m_ppt[i] * (1. - exp(-ubw * gx / m_soilRD)) / uobw;
		wuse[j] = sum - sump + 1. * m_epco[i];
		wuse[j] = sum - sump + (sump -xx) * m_epco[i];
		sump = sum;
		/// adjust uptake if sw is less than 25% of plant available water
		reduc = 0.f;
		if(m_somo[i][j] < m_soilAWC[i][j] / 4.)
			reduc = exp(5. * (4. * m_somo[i][j] / m_soilAWC[i][j] - 1.));
		else
			reduc = 1.f;
		reduc = 1.f;  ///// Is SWAT wrong here? by LJ
		wuse[j] *= reduc;
		if(m_somo[i][j] < wuse[j])
			wuse[j] = m_somo[i][j];
		m_somo[i][j] = max(UTIL_ZERO, m_somo[i][j] - wuse[j]);
		xx += wuse[j];
	}
	/// update total soil water in profile 
	m_totSOMO[i] = 0.f;
	for(int j = 0; j < m_nSoilLayers[i]; j++)
		m_totSOMO[i] += m_somo[i][j];
	m_frStrsWa[i] = xx / m_ppt[i];
	m_plantEPDay[i] = xx;
}

void Biomass_EPIC::CalTempStress(int i)
{
	float tgx = 0.f, rto = 0.f;
	tgx = m_tMean[i] - m_tBase[i];
	if(tgx <= 0.f)
		m_frStrsTmp[i] = 0.;
	else
		if(m_tMean[i] > m_tOpt[i])
			tgx = 2. * m_tOpt[i] - m_tBase[i] - m_tMean[i];
	rto = (m_tOpt[i] - m_tBase[i])/pow((tgx + UTIL_ZERO), 2.);
	if(rto <= 200. && tgx > 0)
		m_frStrsTmp[i] = exp(-0.1054 * rto);
	else
		m_frStrsTmp[i] = 0.f;
	if(m_tMin[i] <= m_tMeanAnn - 15.)
		m_frStrsTmp[i] = 0.f;
}

void Biomass_EPIC::AdjustPlantGrowth(int i)
{
	/// Update accumulated heat units for the plant
	float delg = 0.f;
	if(m_PHUPlt[i] > 0.1)
		delg = (m_tMean[i] - m_tBase[i]) / m_PHUPlt[i];
	if(delg < 0.)
		delg = 0.f;
	m_frPHUacc[i] += delg;
	/// If plant hasn't reached maturity
	if (m_frPHUacc[i] <= 1.)
	{
		///compute temperature stress - strstmp(j) , tstr.f in SWAT
		CalTempStress(i);
		/// Calculate optimal biomass
		/// 1. calculate photosynthetically active radiation
		float activeRadiation = 0.f;
		activeRadiation = 0.5 * m_SR[i] * (1. - exp(-m_lightExtCoef[i] * (m_LAIDay[i] + 0.05)));
		/// 2. Adjust radiation-use efficiency for CO2
		////  determine shape parameters for the radiation use efficiency equation, readplant.f in SWAT
		if (FloatEqual(m_co2Hi[i], 330.0f)) m_co2Hi[i] = 660.f;
		float m_RadUseEffiShpCoef1 = 0.f;
		float m_RadUseEffiShpCoef2 = 0.f;
		PGCommon::getScurveShapeParameter(m_BIOE[i] * 0.01f, m_BIOEHI[i] * 0.01f, m_co2, m_co2Hi[i], &m_RadUseEffiShpCoef1, &m_RadUseEffiShpCoef2);

		float beadj = 0.f;
		if(m_co2 > 330.)
			beadj = 100. * m_co2 / (m_co2 + exp(m_RadUseEffiShpCoef1 - m_co2 * m_RadUseEffiShpCoef2));
		else
			beadj = m_BIOE[i];
		/// 3. adjust radiation-use efficiency for vapor pressure deficit
		///     assumes vapor pressure threshold of 1.0 kPa
		float ruedecl = 0.f;
		if (m_VPD[i] > 1.0)
		{
			ruedecl = m_VPD[i] -1.;
			beadj -= m_wavp[i] * ruedecl;
			beadj = max(beadj, 0.27 * m_BIOE[i]);
		}
		m_biomassDelta[i] = max(0.f, beadj * activeRadiation);
		/// 4. Calculate plant uptake of N and P to make sure no plant N and P uptake under temperature, water and aeration stress
		float reg = min(min(m_frStrsWa[i], m_frStrsTmp[i]), m_frStrsAe[i]);   /// m_frStrsWa and m_frStrsAe are from DistributePlantET()
		reg = min(reg, 0.f);
		if (reg > 0.)
		{
			/// call nup to calculates plant nitrogen uptake
			PlantNitrogenUptake(i);
			/// call npup to calculates plant phosphorus uptake
			PlantPhosphorusUptake(i);
		}
		else
		{
			m_frStrsN[i] = 1.f;
			m_frStrsP[i] = 1.f;
		}
		/// 5. auto fertilization-nitrogen demand (non-legumes only)
		int idc = int(m_landCoverCls[i]);
		//if((idc == 4 || idc == 5 || idc == 6 || idc == 7) && auto_nstrs[i] > 0.)
			/// call anfert
		///////// TODO: Finish auto fertilization-nitrogen later. by LJ
		/// 6. reduce predicted biomass due to stress on plant
		reg = min(m_frStrsWa[i], min(m_frStrsTmp[i], min(m_frStrsN[i], m_frStrsP[i])));
		if(reg < 0.f) reg = 0.f;
		if(reg > 1.f) reg = 1.f;
		//// bio_targ in SWAT is not corporated in SEIMS.
		m_biomass[i] += m_biomassDelta[i] * reg;
		float rto = 1.f;
		if (idc == 7)
		{
			if (m_matYrs[i] > 0.)
			{
				float curYrMat = m_initTreeMatYr[i] + m_yearIdx;
				rto = curYrMat / m_matYrs[i];
				m_biomass[i] = min(m_biomass[i], rto * m_maxBiomass[i] * 1000.f);  /// convert tons/ha -> kg/ha
			}
			else
				rto = 1.f;
		}
		m_biomass[i] = max(m_biomass[i], 0.f);
		/// 7. calculate fraction of total biomass that is in the roots
		float m_rootShootRatio1 = 0.4;
		float m_rootShootRatio2 = 0.2;
		m_frRoot[i] = m_rootShootRatio1 * (m_rootShootRatio1 - m_rootShootRatio2) * m_frPHUacc[i];
		float LAIShpCoef1 = 0.f, LAIShpCoef2 = 0.f;
		PGCommon::getScurveShapeParameter( m_frMaxLAI1[i], m_frMaxLAI2[i], m_frGrowOptLAI1[i], m_frGrowOptLAI2[i], &LAIShpCoef1, &LAIShpCoef2);
		float f = m_frPHUacc[i] / (m_frPHUacc[i] + exp(LAIShpCoef1 - LAIShpCoef2 * m_frPHUacc[i]));
		float ff = f - m_LAIMaxFr[i];
		m_LAIMaxFr[i] = f;
		/// 8. calculate new canopy height
		if (idc == 7)
			m_cht[i] = rto * m_chtMax[i];
		else
			m_cht[i] = m_chtMax[i] * sqrt(f);
		/// 9. calculate new leaf area index (LAI)
		float laiMax = 0.f;
		float laiDelta = 0.f;
		if (m_frPHUacc[i] <= m_frDLAI[i])
		{
			laiMax = 0.f;
			laiDelta = 0.f;
			if (idc == 7)
				laiMax = rto * m_maxLAI[i];
			else
				laiMax = m_maxLAI[i];
			if(m_LAIDay[i] > laiMax) m_LAIDay[i] = laiMax;
			laiDelta = ff * laiMax * (1.f - exp(5.0 * (m_LAIDay[i] - laiMax))) * sqrt(reg);
			m_LAIDay[i] += laiDelta;
			if(m_LAIDay[i] > laiMax) m_LAIDay[i] = laiMax;
			m_oLAI[i] = m_LAIDay[i];
			if(m_LAIDay[i] > m_LAIYrMax[i]) m_LAIYrMax[i] = m_LAIDay[i];
		}
		else
			m_LAIDay[i] = m_oLAI[i] * (1.f - m_frPHUacc[i]) / (1.f - m_frDLAI[i]);
		if(m_LAIDay[i] < m_aLAIMin[i])
			m_LAIDay[i] = m_aLAIMin[i];
		/// 10. calculate plant ET values
		if (m_frPHUacc[i] > 0.5 && m_frPHUacc[i] < m_frDLAI[i])
		{
			m_pltET[i] += (m_plantEPDay[i] + m_soilESDay[i]);
			m_pltPET[i] += m_PET[i];
		}
		m_hvstIdxAdj[i] = m_hvstIdx[i] * 100. * m_frPHUacc[i] / (100. * m_frPHUacc[i] + 
			exp(11.1 - 10. * m_frPHUacc[i]));
	}
	else
	{
		if (m_frDLAI[i] > 1.f)
		{
			if(m_frPHUacc[i] > m_frDLAI[i])
				m_LAIDay[i] = m_oLAI[i] * (1.f - (m_frPHUacc[i] - m_frDLAI[i]) / (1.2 - m_frDLAI[i]));
		}
		if(m_LAIDay[i] < 0.f) m_LAIDay[i] = 0.f;
	}
}

void Biomass_EPIC::PlantNitrogenUptake(int i)
{
	float uobn = PGCommon::getNormalization(m_NUpDis);
	float n_reduc = 300.f; /// nitrogen uptake reduction factor (not currently used; defaulted 300.)
	float tno3 = 0.f;
	for(int l = 0; l < m_nSoilLayers[i]; l++)
		tno3 += m_soilNO3[i][l];
	tno3 /= n_reduc;
	float up_reduc = tno3 / (tno3 + exp(1.56 - 4.5 * tno3)); /// However, up_reduc is not used hereafter.
	/// icrop is land cover code in SWAT.
	/// in SEIMS, it is no need to use it.
	//// determine shape parameters for plant nitrogen uptake equation, from readplant.f
	m_frPlantN[i] = PGCommon::NPBiomassFraction(m_frPlantN1[i], m_frPlantN2[i], m_frPlantN3[i], m_frPHUacc[i]);
	float un2 = 0.f; /// ideal (or optimal) plant nitrogen content (kg/ha)
	un2 = m_frPlantN[i] * m_biomass[i];
	if(un2 < m_plantN[i]) un2 = m_plantN[i];
	m_NO3Defic[i] = un2 - m_plantN[i];
	m_NO3Defic[i] = min(4.f * m_frPlantN3[i] * m_biomassDelta[i], m_NO3Defic[i]);
	m_frStrsN[i] = 1.f;
	int ir = 0;
	if(m_NO3Defic[i] >= UTIL_ZERO)
	{
		for(int l = 0; l < m_nSoilLayers[i]; l++)
		{
			float gx = 0.f;
			if (m_soilRD <= m_soilDepth[i][l])
			{
				gx = m_soilRD;
				ir = 1;
			}
			else
				gx = m_soilDepth[i][l];
			float unmx = 0.f;
			float uno3l = 0.f; /// plant nitrogen demand (kg/ha)
			unmx = m_NO3Defic[i] * (1.f - exp(-m_NUpDis * gx / m_soilRD)) / uobn;
			uno3l = min(unmx - m_plantUpTkN[i], m_soilNO3[i][l]);
			m_plantUpTkN[i] += uno3l;
			m_soilNO3[i][l] -= uno3l;
		}
		if(m_plantUpTkN[i] < 0.f) m_plantUpTkN[i] = 0.f;
		/// If crop is a legume, call nitrogen fixation routine
		if(FloatEqual(m_landCoverCls[i], 1) ||FloatEqual(m_landCoverCls[i], 2) ||
			FloatEqual(m_landCoverCls[i], 3))
			PlantNitrogenFixed(i);
		m_plantUpTkN[i] += m_fixN[i];
		m_plantN[i] += m_plantUpTkN[i];

		/// compute nitrogen stress
		if(FloatEqual(m_landCoverCls[i], 1) ||FloatEqual(m_landCoverCls[i], 2) ||
			FloatEqual(m_landCoverCls[i], 3))
			m_frStrsN[i] = 1.f;
		else
		{
			PGCommon::calPlantStressByLimitedNP(m_plantN[i], un2, &m_frStrsN[i]);
			float xx = 0.f;
			if(m_NO3Defic[i] > 1.e-5)
				xx = m_plantUpTkN[i] / m_NO3Defic[i];
			else
				xx = 1.f;
			m_frStrsN[i] = max(m_frStrsN[i], xx);
			m_frStrsN[i] = min(m_frStrsN[i], 1.f);
		}
	}
}

void Biomass_EPIC::PlantNitrogenFixed(int i)
{
	/// compute the difference between supply and demand
	float uno3l = 0.f;
	if(m_NO3Defic[i] > m_plantUpTkN[i])
		uno3l = m_NO3Defic[i] - m_plantUpTkN[i];
	else /// if supply is being met, fixation = 0 and return
	{
		m_fixN[i] = 0;
		return;
	}
	/// compute fixation as a function of no3, soil water, and growth stage
	//// 1. compute soil water factor
	float fxw = 0.f;
	fxw = m_totSOMO[i] / (0.85 * m_totSoilAWC[i]);
	//// 2. compute no3 factor
	float sumn = 0.f; /// total amount of nitrate stored in soil profile (kg/ha)
	float fxn = 0.f;
	for(int l = 0; l < m_nSoilLayers[i]; l++)
		sumn += m_soilNO3[i][l];
	if(sumn > 300.) fxn = 0.f;
	if(sumn > 100. && sumn <= 300.) fxn = 1.5 - 0.0005 * sumn;
	if(sumn <= 100.) fxn = 1.0;
	//// 3. compute growth stage factor
	float fxg = 0.f;
	if(m_frPHUacc[i] > 0.15 && m_frPHUacc[i] <= 0.30)
		fxg = 6.67 * m_frPHUacc[i] - 1.f;
	if(m_frPHUacc[i] > 0.30 && m_frPHUacc[i] <= 0.55)
		fxg = 1.f;
	if(m_frPHUacc[i] > 0.55 && m_frPHUacc[i] <= 0.75)
		fxg = 3.75 - 5. * m_frPHUacc[i];
	float fxr = min(1.f, min(fxw, fxn)) * fxg;
	fxr = max(0.f, fxr);
	if(m_NFixCoef <= 0.) m_NFixCoef = 0.5;
	if(m_NFixMax <= 0.) m_NFixMax = 20.f;
	m_fixN[i] = min(6.f, fxr * m_NO3Defic[i]);
	m_fixN[i] = m_NFixCoef * m_fixN[i] + (1.f - m_NFixCoef) * uno3l;
	m_fixN[i] = min(m_fixN[i], uno3l);
	m_fixN[i] = min(m_NFixMax, m_fixN[i]);
}

void Biomass_EPIC::PlantPhosphorusUptake(int i)
{
	float uobp = PGCommon::getNormalization(m_PUpDis);
	//// determine shape parameters for plant phosphorus uptake equation, from readplant.f
	m_frPlantP[i] = PGCommon::NPBiomassFraction(m_frPlantP1[i], m_frPlantP2[i], m_frPlantP3[i], m_frPHUacc[i]);
	float up2 = 0.f; /// optimal plant phosphorus content
	float uapd = 0.f; /// plant demand of phosphorus
	float upmx = 0.f; /// maximum amount of phosphorus that can be removed from the soil layer
	float uapl = 0.f; /// amount of phosphorus removed from layer
	float gx = 0.f; /// lowest depth in layer from which phosphorus may be removed
	up2 = m_frPlantP[i] * m_biomass[i];
	if(up2 < m_plantP[i]) up2 = m_plantP[i];
	uapd = up2 - m_plantP[i];
	uapd *= 1.5;   /// luxury p uptake
	m_frStrsP[i] =1.f;
	int ir = 0;
	if(uapd < UTIL_ZERO) return;
	for (int l = 0; l < m_nSoilLayers[i]; l++)
	{
		if(ir > 0) break;
		if(m_soilRD <= m_soilDepth[i][l])
		{
			gx = m_soilRD;
			ir = 1;
		}
		else
			gx = m_soilDepth[i][l];
		upmx = uapd * (1.f - exp(-m_PUpDis * gx / m_soilRD)) / uobp;
		uapl = min(upmx - m_plantUpTkP[i], m_soilPsol[i][l]);
		m_plantUpTkP[i] += uapl;
		m_soilPsol[i][l] -= uapl;
	}
	if(m_plantUpTkP[i] < 0.f) m_plantUpTkP[i] = 0.f;
	m_plantP[i] += m_plantUpTkP[i];
	/// compute phosphorus stress
	PGCommon::calPlantStressByLimitedNP(m_plantP[i], up2, &m_frStrsP[i]);
}

int Biomass_EPIC::Execute()
{
	CheckInputData();
	/// Initialization
	initialOutputs();
	/// calculate albedo in current day, albedo.f of SWAT
#pragma omp parallel for
	for (int i = 0; i < m_nCells; i++) 
	{
		float cej = -5.e-5, eaj = 0.f;
		eaj = exp(cej * (m_soilCov[i] + 0.1));
		if (m_snowAcc[i] < 0.5)
		{
			m_albedo[i] = m_soilALB[i];
			if(m_LAIDay[i] > 0.)  /// include the situation that m_LAIDay[i] == NODATA
				m_albedo[i] = 0.23 * (1.f - eaj) + m_soilALB[i] * eaj;
		}
		else
			m_albedo[i] = 0.8;
	}
#pragma omp parallel for
	for (int i = 0; i < m_nCells; i++) 
	{
		/// calculate residue on soil surface for current day
		m_soilCov[i] = 0.8 * m_biomass[i] + m_soilRsd[i][0];
		m_soilCov[i] = max(m_soilCov[i], 0.f);
		if (FloatEqual(m_igro[i], 1.0f))			/// land cover growing
		{
			DistributePlantET(i);						/// swu.f
			if(FloatEqual(m_dormIdx[i], 0.))	/// plant will not undergo stress if dormant
				AdjustPlantGrowth(i);					/// plantmod.f
			//CheckDormantStatus(i);				/// dormant.f
		}
	}
	return 0;
}

void Biomass_EPIC::Get1DData(const char* key, int* n, float** data)
{
	CheckInputData();
	initialOutputs();
	string sk(key);
	*n = m_nCells;
	if(StringMatch(sk,VAR_BIOMASS))	*data = m_biomass;
	else if (StringMatch(sk, VAR_PLANT_P)) *data = m_plantP;
	else if (StringMatch(sk, VAR_PLANT_N)) *data = m_plantN;
	else if (StringMatch(sk, VAR_AET_PLT)) *data = m_plantEPDay;
	else if (StringMatch(sk, VAR_PLTPET_TOT)) *data = m_pltPET;
	else if (StringMatch(sk, VAR_PLTET_TOT)) *data = m_pltET;
	else if (StringMatch(sk, VAR_FR_PHU_ACC)) *data = m_frPHUacc;
	else if (StringMatch(sk, VAR_LAIDAY)) *data = m_LAIDay;
	else if (StringMatch(sk, VAR_LAIYRMAX)) *data = m_LAIYrMax;
	else if (StringMatch(sk, VAR_DORMI)) *data = m_dormIdx;
	else if (StringMatch(sk, VAR_ALBDAY)) *data = m_albedo;
	else if (StringMatch(sk, VAR_CHT)) *data = m_cht;
	else if (StringMatch(sk, VAR_SOL_COV)) *data = m_soilCov;
	else if (StringMatch(sk, VAR_SOMO_TOT)) *data = m_totSOMO;
	else									
		throw ModelException(MID_BIO_EPIC,"Get1DData","Result " + sk + 
		" does not exist in current module. Please contact the module developer.");
}

void Biomass_EPIC::Get2DData(const char* key, int* nRows, int* nCols, float*** data)
{
	CheckInputData();
	initialOutputs();
	string sk(key);
	*nRows = m_nCells;
	*nCols = m_soilLayers;
	if(StringMatch(sk, VAR_SOMO)) *data = m_somo;
	else if(StringMatch(sk, VAR_SOL_NO3)) *data = m_soilNO3;
	else if(StringMatch(sk, VAR_SOL_SOLP)) *data = m_soilPsol;
	else if(StringMatch(sk, VAR_SOL_RSD)) *data = m_soilRsd;
	else									
		throw ModelException(MID_BIO_EPIC,"Get2DData","Result " + sk + 
		" does not exist in current module. Please contact the module developer.");
}