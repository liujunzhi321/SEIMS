#pragma once

#include <string>
#include <ctime>
#include <cmath>
#include <map>
#include "SimulationModule.h"
using namespace std;

class NutrientTransport_OL : public SimulationModule
{
public:
	NutrientTransport_OL(void);
	~NutrientTransport_OL(void);

	virtual int Execute();

	virtual void SetValue(const char* key, float data);
	virtual void GetValue(const char* key, float* value);
	virtual void Set1DData(const char* key, int n, float* data);
	virtual void Get1DData(const char* key, int* n, float** data);
	virtual void Set2DData(const char* key, int nrows, int ncols, float** data);
	virtual void Get2DData(const char* key, int *nRows, int *nCols, float*** data);

	bool CheckInputSize(const char* key, int n);
	bool CheckInputData(void);

private:

	void initalOutputs();
	
	//input
	// size of array 
	int m_size;
	int m_nLayers;
	
	// nitrate
	//  the fraction of porosity from which anions are excluded, 0.2
	float m_aniExcF;
	//  the nitrate percolation coefficient, 0.8
	float m_nitPC;

	// the surface runoff generated on a given day (mm H2O)
	float* m_sr;

	// the soil porosity (m3/m3)
	float **m_porosity;
	// the water discharged from the layer by lateral flow (mm H2O)
	float **m_ssr;
	// the amount of water percolating to the underlying soil layer on a given day (mm H2O)
	float **m_perc;
	// the amount of nitrate in the layer (kg N/ha)
	float **m_Nitrate;

	// Ammonium
	/// clay fraction
	float **m_clayFrac;
	//amount of ammonium in layer ly (kg N/ha)
	float **m_Ammon;

	// Organic N
	// the width (length) of cell
	int m_cellWidth;

	//float *m_rootDepth;
	// the sediment yield on a given day (metric tons)
    float* m_sedimentYield;
	// Soil density(Mg/m3)
	float **m_Density;
    // depth of the layer (mm)
	float **m_Depth;
	// amount of nitrogen in the active organic pool in layer ly (kg N/ha)
	float **m_ActOrgN;
	// amount of nitrogen in the stable organic pool (kg N/ha)
	float **m_StaOrgN;
	// nitrogen in the fresh organic pool in layer ly (kg N/ha)
	float **m_FreOrgN;

	// soluble P
	// phosphorus soil partitioning coefficient(m3/Mg), 65.0
	int m_parCoeP; 
	// phosphorus percolation coefficient(m3/Mg), 15.0
	int m_phoPC;
	// amount of phosphorus in solution in layer ly(kg P/ha) 
	float **m_SoluP;

	// Attached P
	// amount of phosphorus in the active mineral pool (kg P/ha)
	float **m_actMinP;
	// amount of phosphorus in the stable mineral pool (kg P/ha)
	float **m_staMinP;
	// concentration of humic organic phosphorus in the layer (kg P/ha)
	float **m_humOrgP;
    // phosphorus in the fresh organic pool in layer ly (kg P/ha)
	float **m_FreOrgP;

	// overland
	/// the total number of subbasins
	int m_nsub;  
	/// subbasin grid ( subwatersheds ID)
	float* m_subbasin;
	/// channel width
	float *m_chWidth;
	float *m_streamLink;

	/**
	*	@brief 2d array of flow in cells
	*
	*	The first element in each sub-array is the number of flow in cells in this sub-array
	*/
	float **m_flowInIndex;
	/**
	*	@brief Routing layers according to the flow direction
	*
	*	There are not flow relationships within each layer.
	*	The first element in each layer is the number of cells in the layer
	*/
	float **m_routingLayers;
	int m_nRoutingLayers;


	// output
	// nitrate
	// the nitrate removed in surface runoff (kg N/ha)
	float* m_surNit;

	// the saturated water content of the soil layer (mm H2O)
	float **m_satW;
	// the amount of mobile water in the layer (mm H2O)
	float **m_mobQ;
	// the concentration of nitrate in the mobile water for a given layer (kg N/mm H2O)
	float **m_conNit;
	// the nitrate removed in lateral flow from a layer (kg N/ha)
	float **m_latNit;
	// the nitrate moved to the underlying layer by percolation (kg N/ha)
	float **m_perNit;

	//Ammonium
	// the Ammonium removed in surface runoff (kg N/ha)
	float* m_surAmm;
	// the concentration of ammonium in the mobile water for a given layer (kg N/mm H2O)
	float **m_conAmm;

	// organic N
	// the concentration of sediment in surface runoff(Mg sed/m3 H2O)
	float* m_conSed;
	// nitrogen enrichment ratio
	float* m_enrN; 
	// the concentration of organic nitrogen in the top 10 mm(g N/ metric ton soil)
	float* m_conOrgN;
	// the amount of organic nitrogen transported to the next cell in surface runoff(kg N/ha)
	float* m_traOrgN;

	// soluble P
	// the amount of soluble phosphorus lost in surface runoff (kg P/ha)
	float* m_surSolP;
	// the amount of phosphorus moving from the top 10 mm into the first soil layer 
	float* m_perPho;

	// Attached P
	// phosphorus enrichment ratio
	float* m_enrP; 
	// the concentration of phosphorus attached to sediment in the top 10 mm(g P/ metric ton soil)
	float* m_conAttP;
	// the amount of phosphorus attached to sediment transported to the next cell in surface runoff(kg P/ha)
	float* m_traAttP;


    // overland
	// surface nitrate to channel (kg)
	float* m_sNittoCh;
	// Total surface nitrate flowing to channel (kg)
	float m_sNittoCh_T;
	// surface ammonium to channel (kg)
	float* m_sAmmtoCh;
	// Total surface ammonium flowing to channel (kg)
	float m_sAmmtoCh_T;
	// surface soluble phosphorus to channel (kg)
	float* m_sSolPtoCh;
	// Total surface soluble phosphorus flowing to channel (kg)
	float m_sSolPtoCh_T;
	// the amount of organic nitrogen transported to channel in surface runoff(kg)
	float* m_orgNtoCh;
	// the amount of phosphorus attached to sediment transported to channel in surface runoff(kg)
	float* m_attPtoCh;
	// Total organic nitrogen transported to channel in surface runoff(kg)
	float m_orgNtoCh_T;
	// Total phosphorus attached to sediment transported to channel in surface runoff(kg)
	float m_attPtoCh_T;

	// lateral nitrate to channel (kg)
	float* m_laNittoCh;
	// Total lateral nitrate flowing to channel (kg)
	float m_laNittoCh_T;
	

	void FlowInSoil(int id);

};