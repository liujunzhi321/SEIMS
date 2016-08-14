/*!
 * \file NutrientRemviaSr.h
 * \brief Simulates the loss of nitrate and phosphorus via surface runoff, lateral flow, tile flow, and percolation out of the profile.
 * \author Huiran Gao
 * \date May 2016
 */


#pragma once
#ifndef SEIMS_NutRemv_PARAMS_INCLUDE
#define SEIMS_NutRemv_PARAMS_INCLUDE

#include <string>
#include "api.h"
#include "SimulationModule.h"

using namespace std;

/** \defgroup NutRemv
 * \ingroup Nutrient
 * \brief Simulates the loss of nitrate and phosphorus via surface runoff, lateral flow, tile flow, and percolation out of the profile.
 */

/*!
 * \class NutrientRemviaSr
 * \ingroup NutRemv
 *
 * \brief Nutrient removed and loss in surface runoff, lateral flow, tile flow, and percolation out of the profile
 *
 */

class NutrientRemviaSr : public SimulationModule
{
public:
    NutrientRemviaSr(void);
    ~NutrientRemviaSr(void);

    virtual void Set1DData(const char *key, int n, float *data);
    virtual void Set2DData(const char *key, int nRows, int nCols, float **data);
    virtual void SetValue(const char *key, float value);
    virtual int Execute();
    virtual void GetValue(const char *key, float *value);
    virtual void Get1DData(const char *key, int *n, float **data);
    virtual void Get2DData(const char *key, int *nRows, int *nCols, float ***data);
	virtual void SetSubbasins(clsSubbasins *subbasins);

private:

    /// cell width of grid map (m)
    float m_cellWidth;
    /// number of cells
    int m_nCells;
    /// soil layers
    float *m_nSoilLayers;
    /// maximum soil layers
    int m_soiLayers;
	/// stream link
	float *m_streamLink;

    /// input data
    /// drainage tile flow in soil profile
    float m_qtile;
    /// distribution of soil loss caused by water erosion
    float *m_sedimentYield;
    /// Phosphorus soil partitioning coefficient
    float m_phoskd;
    /// phosphorus percolation coefficient (0-1)
    float m_pperco;
    /// nitrate percolation coefficient (0-1)
    float m_nperco;
    // fraction of porosity from which anions are excluded
    float *m_anion_excl;
    // distribution of surface runoff generated
    float *m_surfr;
    /// initial septic operational condition (active-1, failing-2, non_septic-0)
    float m_isep_opt;
    /// soil layer where drainage tile is located
    float *m_ldrain;
    /// crack volume potential of soil
    float *m_sol_crk;
    /// amount of water held in the soil layer at saturation
    float **m_sol_wsatur;

    /// lateral flow in soil layer
    float **m_flat;
    /// percolation from soil layer
    float **m_sol_perco;
    /// bulk density of the soil
    float **m_sol_bd;
    /// depth to bottom of soil layer
    float **m_sol_z;

	/// flow out index
	float *m_flowOutIndex;

    /// amount of organic nitrogen in surface runoff
    float *m_sedorgn;
    /// average air temperature
    float *m_tmean;
	///percent organic matter in soil layer (%)
	float **m_sol_om;
	/// soil thick of each layer (mm)
	float **m_sol_thick;

    /// output data
    /// amount of nitrate transported with lateral flow
    float *m_latno3;
    /// amount of nitrate percolating past bottom of soil profile
    float *m_perco_n;
	/// amount of solute P percolating past bottom of soil profile
	float *m_perco_p;
    /// amount of nitrate transported with surface runoff
    float *m_surqno3;
    /// amount of soluble phosphorus in surface runoff
    float *m_surqsolp;
    /// carbonaceous oxygen demand of surface runoff
    float *m_cod;
    /// chlorophyll-a concentration in water yield
    float *m_chl_a;
    /// dissolved oxygen concentration in the surface runoff
    //float* m_doxq;
    /// dissolved oxygen saturation concentration
    //float* m_soxy;

	// N and P to channel
	float *m_latno3ToCh;  // amount of nitrate transported with lateral flow to channel
	float *m_surqno3ToCh; // amount of nitrate transported with surface runoff to channel
	float *m_surqsolpToCh;// amount of soluble phosphorus in surface runoff to channel
	float *m_perco_n_gw;  // amount of nitrate percolating past bottom of soil profile sum by sub-basin
	float *m_perco_p_gw;  // amount of solute P percolating past bottom of soil profile sum by sub-basin

	/// subbasin related
	/// the total number of subbasins
	int m_nSubbasins;
	//! subbasin IDs
	vector<int> m_subbasinIDs;
	/// subbasin grid (subbasins ID)
	float *m_subbasin;
	/// subbasins information
	clsSubbasins *m_subbasinsInfo;

    /// input & output
    /// average annual amount of phosphorus leached into second soil layer
    float m_wshd_plch;

    /// amount of nitrogen stored in the nitrate pool in soil layer
    float **m_sol_no3;
    /// amount of phosphorus stored in solution
    float **m_sol_solp;



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
    bool CheckInputSize(const char *, int);

    /*!
    * \brief Calculate the loss of nitrate via surface runoff, lateral flow, tile flow, and percolation out of the profile.
    *
    * \return void
    */
    void NitrateLoss();

    /*!
    * \brief Calculates the amount of phosphorus lost from the soil
    *        profile in runoff and the movement of soluble phosphorus from the first
    *        to the second layer via percolation.
    *
    * \return void
    */
    void PhosphorusLoss();

    /*!
    * \brief Calculate enrichment ratio.
     *
     * \return void
     */
    float *CalculateEnrRatio();

    void initialOutputs();

	void SumBySubbasin();
};

#endif





