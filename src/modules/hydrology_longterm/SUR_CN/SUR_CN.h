/** 
*	@file
*	@version	1.0
*	@author    Wu Hui
*	@date	29-December-2010
*
*	@brief	SCS Curve Number Method to calculate infiltration and excess precipitation
*
*	Revision: Zhiqiang Yu	
*   Date:     2011-2-14
*   1. Because w1 and w2 in equation 2:1.1.6 is fixed, add two variable to store these
*      two coeffiences to avoid repeating calculation.
*   2. When w1 and w2 is calculated using equation 2:1.1.7 and 2:1.1.8, FC and SAT should
*	   be FC*rootdepth and SAT*rootdepth rather than (FC-WP)*rootdepth and (SAT-WP)*rootdepth.
*	   And the unit conversion of rootdepth would be from m to mm.
*   3. Overload the funtion Calculate_CN to calculate the CN of one cell.
*   4. Delete some parameters and input variables. They are Depre_in,Moist_in and Depression.
*      D_SOMO from soil water balance module and D_DPST from depression module is enough. The 
*      initalization of soil moisture and depression storage is the task of soil water balance 
*	   module and depression module.
*	5. Modify the name of input and output variables to make it consistent with other modules.
*/

#pragma once
#include <string>
#include <ctime>
#include "api.h"

using namespace std;
#include "SimulationModule.h"

class SUR_CN:public SimulationModule
{
public:
	SUR_CN(void);
	~SUR_CN(void);
	virtual int Execute();
	virtual void SetValue(const char* key, float data);
	virtual void Set1DData(const char* key, int n, float* data);
	virtual void Set2DData(const char* key, int nrows, int ncols, float** data);
	virtual void Get1DData(const char* key, int* n, float** data);
    virtual void Get2DData(const char* key, int *nRows, int *nCols, float*** data);

	bool CheckInputSize(const char* key, int n);
	bool CheckInputData(void);

private:
	/// number of soil layers
	int m_nLayers;
	/// depth of the up two layers(The depth are 10mm and 100 mm, respectively).
	float m_depth[2];

	/// count of valid cells
	int m_nCells;
	/// soil porosity
	float** m_porosity;
	/// water content of soil at field capacity 
	float** m_fieldCap;
	/// plant wilting point moisture
	float** m_wiltingPoint;

	/// root depth of plants (m)
	float* m_rootDepth;
	/// CN under moisture condition II    
	float* m_CN2;
	/// Net precipitation calculated in the interception module (mm)
	float* m_P_NET;
	/// Initial soil moisture or
	/// soil moisture of each time step
	float** m_soilMoisture;
	float* m_initSoilMoisture;
	/// Initial depression storage coefficient
	//float m_Depre_in;
	/// Depression storage capacity
	//float* m_Depression;
	/// depression storage  
	float* m_SD;    // SD(t-1) from the depression storage module
	/// from interpolation module
	/// air temperature of the current day
	float *m_tMin, *m_tMax;
	/// snowfall temperature from the parameter database (¡æ)
	float m_Tsnow;
	/// threshold soil freezing temperature (¡æ)
	float m_Tsoil;
	/// frozen soil moisture relative to saturation above which no infiltration occur (m3/m3)
	float m_Sfrozen;
	/// snowmelt threshold temperature from the parameter database (¡æ)
	float m_T0;
	/// snowmelt from the snowmelt module  (mm)
	float* m_SM; 
	/// snow accumulation from the snow balance module (mm) at t+1 timestep
	float* m_SA;
	/// soil temperature obtained from the soil temperature module (¡æ)
	float* m_TS;

	/// from GIS interface Project/model subdirectory
	/// Julian day
	int m_julianDay;

	// output
	/// the excess precipitation (mm) of the total nCells
	float* m_PE;
	/// infiltration map of watershed (mm) of the total nCells
	float* m_INFIL;

	//add by Zhiqiang
	float* m_w1;
	float* m_w2;
	float* m_sMax;
	void   initalW1W2();
	float  Calculate_CN(float sm, int cell);

	void initalOutputs();
};

