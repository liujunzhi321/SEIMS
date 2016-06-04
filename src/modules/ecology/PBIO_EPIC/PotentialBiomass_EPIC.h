/*!
 * \file PotentialBiomass_EPIC.h
 * \brief Predicts daily potential growth of total plant biomass and roots and calculates leaf area index
 * incorporated a simplified version of the EPIC plant growth model as in SWAT rev. 637, plantmod.f
 * \author LiangJun Zhu, Wei Cheng
 * \date June 2016
 *
 * 
 */

#pragma once
#include <string>
#include <ctime>
#include "api.h"
#include "PlantGrowthCommon.h"
#include "ClimateParams.h"
#include "SimulationModule.h"
using namespace std;
/** \defgroup PBIO_EPIC
 * \ingroup Ecology
 * \brief Predicts daily potential growth of total plant biomass and roots and calculates leaf area index
 * incorporated a simplified version of the EPIC plant growth model as in SWAT rev. 637
 */
/*!
 * \class PotentialBiomass_EPIC
 * \ingroup PBIO_EPIC
 * \brief Predicts daily potential growth of total plant biomass and roots and calculates leaf area index
 * incorporated a simplified version of the EPIC plant growth model as in SWAT rev. 637
 */
class PotentialBiomass_EPIC: public SimulationModule
{
public:
	//! Constructor
	PotentialBiomass_EPIC(void);
	//! Destructor
	~PotentialBiomass_EPIC(void);

	virtual int Execute();

	virtual void SetValue(const char* key, float data);
	virtual void Set1DData(const char* key, int n, float* data);
	virtual void Set1DData(const char* key, int n, time_t* data);
	virtual void Get1DData(const char* key, int* n, float** data);

	bool CheckInputSize(const char* key, int n);
	bool CheckInputData(void);

private:
	/// valid cells number
	int m_nCells;

	/**  climate inputs  **/

	/// mean air temperature
	float* m_tMean;
	/// min air temperature
	float* m_tMin;
	/// max air temperature
	float* m_tMax;
	/// solar radiation
	float* m_SR;
	/// relative air moisture
	float* m_RM;

	/**  soil properties  **/

	/// soil layers
	float* m_nSoilLayers;
	/// maximum soil layers
	int m_soilLayers;
	/// maximum root depth
	float* m_soilZMX;
	/// soil depth of all layers
	float** m_soilDepth;
	/// amount of organic matter in the soil layer classified as residue
	float** m_soilRsd;
	/// amount of water available to plants in soil layer at field capacity (fc - wp water)
	float** m_soilAWC;
	/// total m_soilAWC in soil profile
	float* m_totSoilAWC;
	/// amount of water held in soil profile at saturation
	float* m_totSoilSat;
	/// amount of water stored in soil layers on current day
	float** m_somo;

	/**  crop or land cover related inputs  **/

	/// land cover status code, 0 means no crop while 1 means land cover growing
	float* m_igro;
	/// land cover/crop  classification:1-7, i.e., IDC
	float* m_landCoverCode;
	/// maximum (potential) leaf area index (BLAI in cropLookup db)
	float* m_maxLAI;
	/// initial Potential Biomass
	float* m_initBiomass;
	/// initial LAI
	float* m_initLAI;
	/// fraction of plant heat units (PHU) accumulated
	float* m_frPHUacc;
	/// plant water uptake compensation factor
	float* m_epco;
	/// biomass-energy ratio
	float* m_bioE;
	/**  input from other modules  **/

	/// maximum plant et (mm H2O), ep_max in SWAT
	float* m_ppt;

	/**  intermediate variables  **/

	/// water uptake distribution parameter, NOT ALLOWED TO MODIFIED BY USERS
	float ubw;
	/// water uptake normalization parameter, NOT ALLOWED TO MODIFIED BY USERS
	float uobw;
	/// soil aeration stress
	float* m_frStrsAe;

	/**  set output variables  **/

	/// amount of residue on soil surface
	float* m_soilCov;
	/// current rooting depth
	float m_soilRD;
	/// last soil root depth for use in harvestkillop/killop 
	float* m_soilStRD;
	/// amount of water stored in soil profile
	float* m_totSOMO;
	/// actual amount of transpiration (mm H2O), ep_day in SWAT
	float* m_actET;
	/// fraction of potential plant growth achieved where the reduction is caused by water stress
	float* m_frStrsWa;
	/// land cover/crop biomass (dry weight)
	float* m_biomass;


	
	/// light extinction coefficient
	float* m_lightExtinctionCoef;
	
	
	///float m_curYear; /// replaced by m_yearIdx
	float* m_fullDevYear;
	float* m_tBase;
	//float* m_tOpt;
	float m_PHU;
	float* m_frPHU1;
	float* m_frPHU2;
	float* m_frLAI1;
	float* m_frLAI2;
	float* m_frDPHU;
	float m_co2;
	
	float* m_co2Hi;
	float* m_rueHi;
	float* m_rueDcl;
	float* m_frN1;
	float* m_frN2;
	float* m_frN3;
	float* m_frP1;
	float* m_frP2;
	float* m_frP3;
	float* m_frLAImx;
	//float* m_frTreeDormentResidue;
	float* m_LAIShapeCoefficient1;
	float* m_LAIShapeCoefficient2;
	float* m_CO2ShapeCoefficient1;
	float* m_CO2ShapeCoefficient2;
	float* m_activeRadiation;
	float* m_rue;
	float* m_ee;
	
	//float* m_latitude;
	//the fraction of the plant¡¯s maximum leaf area index for day i-1
	float* m_prefrLAImx;
	//the fraction of the plant¡¯s maximum leaf area index for day i
	
	// total plant water uptake, it is also the actual amount of transpiration
	//float* m_totalWUptake;

	//float* m_transpirationMax;

	//float m_LAIBeforeDecline;

	//result
	// the amount of biomass a tree can accumulate in a single year
	//float* m_bioTreeAnnual;
	float* m_biomassDelta;
	//float* m_prebiomass;
	
	float* m_biomassNOpt;
	float* m_biomassPOpt;
	//float* m_biomassN;
	//float* m_biomassP;
	//float* m_biomassRoot;
	//float* m_biomassAG;
	float* m_VPD;
	float* m_frPHU;
	// the leaf area indices for day i-1
	float* m_preLAI;
	// the leaf area indices for day i
	float* m_LAI;
	float* m_LAIdelta;
	float* m_frN;
	float* m_frP;
	float* m_frRoot;
	
	PGCommon* m_pgCommon;

	//////////////////////////////////////////////////////////////////////////
	//  The following code is transferred from swu.f of SWAT rev. 637
	//  Distribute potential plant evaporation through
	//	the root zone and calculates actual plant water use based on soil
	//	water availability. Also estimates water stress factor.  
	//////////////////////////////////////////////////////////////////////////
	void DistributePlantET(int i);
	//////////////////////////////////////////////////////////////////////////
	//  The following code is transferred from grow.f of SWAT rev. 637
	//  Adjust plant biomass, leaf area index, and canopy height
	//	taking into account the effect of water, temperature and nutrient stresses  on the plant
	//////////////////////////////////////////////////////////////////////////
	void AdjustPlantGrowth(int i);

	//accumulate heat unit to determine the growth stage
	//it's the base of plant growth
	float doHeatUnitAccumulation(float potentialHeatUnit, float tMin, float tMax,float tBase);
	void getScurveShapeParameter(float xMid, float xEnd, float yMid, float yEnd, float* shape1, float* shape2);
	float RadiationUseEfficiencyAdjustByVPD(float vpd,float radiationUseEfficiencyDeclineRateWithVPD); 
	void getNPShapeParameter(float* fr1, float* fr2, float* fr3, float* shape1, float* shape2);
	float getNPFraction(float fr1, float fr3, float shape1, float shape2, float frPHU);
	float NPBiomassFraction(float x1, float x2, float x3, float frPHU);
	
	void initialOutputs();

	bool IsTree(int classification); 
	bool IsAnnual(int classification);
	bool IsLegume(int classification); 
	bool IsPerennial(int classification); 
	bool IsCoolSeasonAnnual(int classification); 
	
};


//// Deprecated code original from Cheng Wei.  By LJ.
//time_t m_startDate;
//time_t m_endDate;