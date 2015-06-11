#pragma once

#include <string>
#include <ctime>
#include <cmath>
#include <map>
#include "SimulationModule.h"
using namespace std;

class MineralizationAndDecomposition : public SimulationModule
{
public:
	MineralizationAndDecomposition(void);
	~MineralizationAndDecomposition(void);

	virtual int Execute();

	virtual void SetValue(const char* key, float data);
	virtual void Set1DData(const char* key, int n, float* data);
	//virtual void Get1DData(const char* key, int* n, float** data);
	virtual void Set2DData(const char* key, int nrows, int ncols, float** data);
	virtual void Get2DData(const char* key, int *nRows, int *nCols, float*** data);

	bool CheckInputSize(const char* key, int n);
	bool CheckInputData(void);

private:

	void initalOutputs();
	
	// size of array 
	int m_size;
	int m_nLayers;

    
	//input
	//float *m_rootDepth;
	
	// rate constant(0.00001) 
	float m_RateConst;
    // fraction of humic nitrogen in the active pool (0.02)
	float m_HumNF;
	// rate coefficient for mineralization of the humus active organic nutrients
	float m_RateCoeH;
	// rate coefficient for mineralization of the residue fresh organic nutrients
	float m_RateCoeR;	
	// concentration of solution phosphorus all layers(5 mg/kg)
	float m_conSoluP;

    // soil temperature of layer ly(¡æ)
    float *m_SoilT;
	
	// bulk density of the layer (Mg/m3)
	float **m_Density;
	// amount of soil organic matter in the layer(%)
    //float **m_som;
	// amount of organic carbon in the layer(%)
    float **m_orgCar;
	// water content of layer ly on a given day(mm H2O)
	float **m_SOMO;
	// water content of layer ly at field capacity (mm H2O)
	float **m_FieldCap;
	// depth of the layer (mm)
	float **m_Depth;
	// the residue in layer ly (kg N/ha)
	float **m_Res;
	// amount of nitrate in layer ly (kg N/ha)
	float **m_Nitrate;
	


	//output
	// amount of phosphorus in layer ly (kg P/ha)
	float **m_SoluP;
	// concentration of humic organic nitrogen in the layer(mg/kg or ppm)
	float **m_conHumOrgN;
	// concentration of nitrogen in the active organic pool(mg/kg)
	float **m_conActOrgN;
	// concentration of nitrogen in the stable organic pool (mg/kg)
	float **m_conStaOrgN;
	// amount of humic organic nitrogen in the layer (kg N/ha)
	float **m_HumOrgN;
	// amount of nitrogen in the active organic pool in layer ly (kg N/ha)
	float **m_ActOrgN;
	// amount of nitrogen in the stable organic pool (kg N/ha)
	float **m_StaOrgN;
	// nitrogen in the fresh organic pool in layer ly (kg N/ha)
	float **m_FreOrgN;
	// nutrient cycling temperature factor for layer ly
	float **m_TF;
	// nutrient cycling water factor for layer ly
	float **m_WF;
	// amount of nitrogen transferred between the active and stable organic pools (kg N/ha)
	float **m_TraOrgN;
	// nitrogen mineralized from the humus active organic N pool
	float **m_MinHumN;
	// C:N ratio of the residue in the soil layer
	float **m_CNRat;
	// C:P ratio of the residue in the soil layer
	float **m_CPRat;
	// residue decay rate constant
	float **m_RateConRD;
	// nutrient cycling residue composition factor for layer ly 
	float **m_ResComF;
	// nitrogen mineralized from the fresh organic N pool (kg N/ha)
	float **m_MinFreOrgN;
	// nitrogen decomposed from the fresh organic N pool (kg N/ha)
	float **m_DecFreOrgN;
	
	// concentration of humic organic phosphorus in the layer (kg P/ha)
	float **m_humOrgP;
    // phosphorus in the fresh organic pool in layer ly (kg P/ha)
	float **m_FreOrgP;
	// amount of phosphorus in the active organic pool (kg P/ha)
	float **m_actOrgP;
	// amount of phosphorus in the stable organic pool (kg P/ha)
	float **m_staOrgP;
	// phosphorus mineralized from the humus active organic P pool (ka P/ha)
	float **m_minHumP;
	// phosphorus mineralized from the fresh organic P pool (kg P/ha)
	float **m_minFreOrgP;
	// phosphorus decomposed from the fresh organic P pool (kg P/ha)
	float **m_decFreOrgP;


};