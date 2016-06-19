/** \defgroup NutCHRout
 * \ingroup Nutrient
 * \brief Calculates in-stream nutrient transformations.
 */
/*!
 * \file NutrientCHRoute.h
 * \ingroup NutCHRout
 * \author Huiran Gao
 * \date Jun 2016
 */

#pragma once
#ifndef SEIMS_NutCHRout_PARAMS_INCLUDE
#define SEIMS_NutCHRout_PARAMS_INCLUDE
#include <string>
#include <map>
#include <vector>
#include "api.h"
#include "SimulationModule.h"
using namespace std;
/*!
 * \class NutrientCHRoute
 * \ingroup NutCHRout
 *
 * \brief Calculates the concentration of nutrient in reach.
 *
 */

class NutrientCHRoute : public SimulationModule {
	public:
		NutrientCHRoute(void);
		~NutrientCHRoute(void);

	virtual void SetValue(const char* key, float value);
	virtual void Set1DData(const char* key, int n, float *data);
	virtual void Set2DData(const char* key, int nRows, int nCols, float** data);
	virtual int Execute();
	virtual void GetValue(const char* key, float* value);
	virtual void Get1DData(const char* key, int* n, float** data);
	virtual void Get2DData(const char* key, int* nRows, int* nCols, float*** data);
private:
	/// cell width of grid map (m)
	float m_cellWidth;
	/// number of cells
	int m_nCells;
	/// time step (hr)
	int m_dt;
	/// reach number (= subbasin number)
	int m_nreach;
	/// upstream id (The value is -1 if there if no upstream reach)
	vector< vector<int> > m_reachUpStream;

	/// id the reaches
	float* m_reachId;
	/// map from subbasin id to index of the array
	map<int, int> m_idToIndex;
	map<int, vector<int> > m_reachLayers;

	/// input data
	/// bank flow recession constant
	float m_aBank;
	float m_qUpReach;

	float m_ai0;	/// ratio of chlorophyll-a to algal biomass (ug chla/mg alg)
	float m_ai1;	/// fraction of algal biomass that is nitrogen (mg N/mg alg)
	float m_ai2;	/// fraction of algal biomass that is phosphorus (mg P/mg alg)
	float m_ai3;	/// the rate of oxygen production per unit of algal photosynthesis (mg O2/mg alg)
	float m_ai4;	/// the rate of oxygen uptake per unit of algae respiration (mg O2/mg alg)
	float m_ai5;	/// the rate of oxygen uptake per unit of NH3 nitrogen oxidation (mg O2/mg N)
	float m_ai6;	/// the rate of oxygen uptake per unit of NO2 nitrogen oxidation (mg O2/mg N)

	float m_lambda0;	 /// non-algal portion of the light extinction coefficient
	float m_lambda1;	 /// linear algal self-shading coefficient
	float m_lambda2;	 /// nonlinear algal self-shading coefficient

	float m_k_l;		/// half saturation coefficient for light (MJ/(m2*hr))
	float m_k_n;		/// half-saturation constant for nitrogen (mg N/L)
	float m_k_p;		/// half saturation constant for phosphorus (mg P/L)
	/// algal preference factor for ammonia
	float m_p_n;
	/// fraction of solar radiation computed in the temperature heat balance that is photo synthetically active
	float tfact;
	/// fraction of overland flow
	float m_rnum1;
	/// option for calculating the local specific growth rate of algae
	//     1: multiplicative:     u = mumax * fll * fnn * fpp
	//     2: limiting nutrient: u = mumax * fll * Min(fnn, fpp)
	//     3: harmonic mean: u = mumax * fll * 2. / ((1/fnn)+(1/fpp))
	int igropt;
	/// maximum specific algal growth rate at 20 deg C
	float m_mumax;
	/// algal respiration rate at 20 deg C (1/day)  
	float m_rhoq;

	/// day length for current day (h)
	float* m_dayl;
	/// hru_ra(:)    |MJ/m^2        |solar radiation for the day in HRU
	float* m_sra;
	float* m_bankStorage;
	/// overland flow to streams from each subbasin (m3/s)
	float* m_qsSub;
	/// inter-flow to streams from each subbasin (m3/s)
	float* m_qiSub;
	/// groundwater flow out of the subbasin (m3/s)
	float* m_qgSub;

	/// channel outflow
	float* m_qsCh;
	float* m_qiCh;
	float* m_qgCh;
	/// reach storage (m3) at time t
	float* m_chStorage;
	/// channel water depth m
	float *m_chWTdepth;

	/// algal biomass concentration in reach (mg/L)
	float* m_algae;
	/// organic nitrogen concentration in reach (mg/L)
	float* m_organicn;
	/// organic phosphorus concentration in reach (mg/L)
	float* m_organicp;
	/// ammonia concentration in reach (mg/L)
	float* m_ammonian;
	/// nitrite concentration in reach (mg/L)
	float* m_nitriten;
	/// nitrate concentration in reach (mg/L)
	float* m_nitraten;
	/// dissolved phosphorus concentration in reach (mg/L)
	float* m_disolvp;
	/// carbonaceous biochemical oxygen demand in reach (mg/L)
	float* m_rch_cbod;
	/// dissolved oxygen concentration in reach (mg/L)
	float* m_rch_dox;
	/// temperature of water in reach (deg C)
	float* m_wattemp;
	/// chlorophyll-a concentration in reach (mg chl-a/L)
	float* m_chlora;

	float* m_bc1;		/// rate constant for biological oxidation of NH3 to NO2 in reach at 20 deg C
	float* m_bc2;		/// rate constant for biological oxidation of NO2 to NO3 in reach at 20 deg C
	float* m_bc3;		/// rate constant for biological oxidation of organic N to ammonia in reach at 20 deg C
	float* m_bc4;		/// rate constant for biological oxidation of organic P to dissolved P in reach at 20 deg C

	float* m_rs1;		/// local algal settling rate in reach at 20 deg C (m/day)
	float* m_rs2;		/// benthos source rate for dissolved phosphorus in reach at 20 deg C (mg disP-P)/((m**2)*day)
	float* m_rs3;		/// benthos source rate for ammonia nitrogen in reach at 20 deg C (mg NH4-N)/((m**2)*day)
	float* m_rs4;		/// rate coefficient for organic nitrogen settling in reach at 20 deg C (1/day)
	float* m_rs5;		/// organic phosphorus settling rate in reach at 20 deg C (1/day)


	float* m_rk1;      /// CBOD deoxygenation rate coefficient in reach at 20 deg C (1/day)
	float* m_rk2;      /// reaeration rate in accordance with Fickian diffusion in reach at 20 deg C (1/day)
	float* m_rk3;      /// rate of loss of CBOD due to settling in reach at 20 deg C (1/day)
	float* m_rk4;      /// sediment oxygen demand rate in reach at 20 deg C (mg O2/ ((m**2)*day))

	/// subroutine summarizes data from overland flow
	float** varoute;

	/// output data
	// saturation concentration of dissolved oxygen (mg/L)
	float m_soxy;
	// light extinction coefficient
	float m_lambda;

private:

	/*!
	 * \brief check the input data. Make sure all the input data is available.
	 * \return bool The validity of the input data.
	 */
	bool CheckInputData(void);

	/*!
	 * \brief check the input size. Make sure all the input data have same dimension.
	 *
	 * \param[in] key The key of the input data
	 * \param[in] n The input data dimension
	 * \return bool The validity of the dimension
	 */
	bool CheckInputSize(const char*,int);

	/*!
	* \brief In-stream nutrient transformations and water quality calculations.
	 *
	 * \return void
	 */
	void NutrientinChannel(int);
	
	/*!
	* \brief Corrects rate constants for temperature.
	 *
	 *    r20         1/day         value of the reaction rate coefficient at the standard temperature (20 degrees C)
	 *    thk         none          temperature adjustment factor (empirical constant for each reaction coefficient)
	 *    tmp         deg C         temperature on current day
	 *
	 * \return float
	 */
	float corTempc(float r20, float thk, float tmp)
};
#endif





