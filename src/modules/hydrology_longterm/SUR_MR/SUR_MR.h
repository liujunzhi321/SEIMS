/** 
*	@file
*	@version	1.0
*	@author    Junzhi Liu
*	@date	19-January-2011
*
*	@brief	Modified Rational Method to calculate infiltration and excess precipitation
*
*	Revision:	Zhiqiang Yu
*   Date:		2011-2-15
*	Description:
*	1.	Parameter S_M_frozen would be s_frozen and DT_Single.
*	2.	Parameter sfrozen would be t_soil and in WaterBalance table.
*	3.  Delete parameter Moist_in.
*	4.  Rename the input and output variables. See metadata rules for names.
*	5.  In function execute, do not change m_pNet[i] directly. This will have influence
*		on another modules who will use net precipitation. Use local variable to replace it.
*	6.  Add API function GetValue.
*
*	Revision:	Junzhi Liu
*   Date:		2011-2-19
*	1.	Rename m_excess to m_pe
*	2.	Take snowmelt into consideration when calculating PE, PE=P_NET+Snowmelt-F
*
*	Revision:	Junzhi Liu
*   Date:		2013-10-28
*	1.	Add multi-layers support for soil parameters
*/

#pragma once
#include <string>
#include <ctime>
#include "api.h"

using namespace std;
#include "SimulationModule.h"

class SUR_MR:public SimulationModule
{
public:
	SUR_MR(void);
	~SUR_MR(void);
	virtual int Execute();
	virtual void SetValue(const char* key, float data);
	virtual void Set1DData(const char* key, int n, float* data);
	virtual void Get1DData(const char* key, int* n, float** data);
	virtual void Set2DData(const char* key, int nrows, int ncols, float** data);
	virtual void Get2DData(const char* key, int *nRows, int *nCols, float*** data);

	bool CheckInputSize(const char* key, int n);
	void CheckInputData(void);

private:
	float m_dt;
	/// count of valid cells
	int m_nCells;
	/// precipitation of each cell
	float *m_pNet;
	/// potential runoff coefficient
	float *m_runoffCo;
	/// root depth
	float *m_rootDepth;

	/// number of soil layers
	int m_nSoilLayers;
	/// depth of the up soil layer
	float m_upSoilDepth;

	/// soil porosity
	float **m_porosity; 
	/// soil moisture
	float **m_soilMoisture;
	/// water content of soil at field capacity 
	float **m_fieldCap;

	float *m_initSoilMoisture;

	/// runoff exponent
	float m_kRunoff;
	/// maximum P corresponding to runoffCo
	float m_pMax;
	/// depression storage
	float* m_sd;    // SD(t-1) from the depression storage module


	/// maximum temperature
	float *m_tMax;
	/// minimum temperature
	float *m_tMin;
	/// snow fall temperature
	float m_tSnow;
	/// snow melt threshold temperature
	float m_t0;
	/// snow melt from the snow melt module  (mm)
	float* m_snowMelt; 
	/// snow accumulation from the snow balance module (mm) at t+1 timestep
	float* m_snowAccu;

	/// threshold soil freezing temperature (¡æ)
	float m_tFrozen;
	/// frozen soil moisture relative to saturation above which no infiltration occur (m3/m3)
	float m_sFrozen;
	/// soil temperature obtained from the soil temperature module (¡æ)
	float* m_soilTemp;

	// output
	/// the excess precipitation (mm) of the total nCells
	float* m_pe;
	/// infiltration map of watershed (mm) of the total nCells
	float* m_infil;

	void initalOutputs();
};

