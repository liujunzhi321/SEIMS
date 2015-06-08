/** 
*	@file
*	@version	1.0
*	@author    Wu Hui
*	@date	24-January-2011
*
*	@brief	Reservoir Method to calculate groundwater balance and baseflow
*
*	Revision: Zhiqiang Yu
*   Date:	  2011-2-11
*
*	Revision: Zhiqiang Yu
*	Date:	  2011-2-18
*	Description:
*	1.  Add judgement to calculation of EG (Revap). The average percolation of 
*		one subbasin is first calculated. If the percolation is less than 0.01,
*		EG is set to 0 directly. (in function setInputs of class subbasin)
*	2.	Add member variable m_isRevapChanged to class subbasin. This variable 
*		is the flag whether the Revap is changed by current time step. This flag
*		can avoid repeating setting values when converting subbasin average Revap
*		to cell Revap.(in function Execute of class ReservoirMethod)
*
*	Revision:	Zhiqiang Yu
*	Date:		2011-3-14
*	Description:
*	1.	Add codes to process the groundwater which comes from bank storage in 
*		channel routing module. The water volumn of this part of groundwater is
*		added to the groudwater storage. The input variable "T_GWNEW" is used
*		for this purpose. One additional parameter is added to function setInputs 
*		of class subbasin. See equation 8 in memo "Channel water balance" for detailed
*		reference.
*/

#ifndef NEW_WETSPA_GWA_RESERVOIR_METHOD_INCLUED
#define NEW_WETSPA_GWA_RESERVOIR_METHOD_INCLUED

#include <string>
#include <vector>
#include <map>
#include "api.h"
#include "subbasin.h"
#include "SimulationModule.h"

using namespace std;

class ReservoirMethod : public SimulationModule
{
public:
	ReservoirMethod(void);
	~ReservoirMethod(void);

	virtual void SetValue(const char* key, float value);
	virtual void Set1DData(const char* key, int n, float* data);
	virtual void Set2DData(const char* key, int nrows, int ncols, float** data);

	virtual int Execute(void);

	virtual void Get1DData(const char* key, int* nRows, float **data);
	virtual void Get2DData(const char* key, int* nRows, int* nCols, float ***data);

	//virtual TimeStepType GetTimeStepType()
	//{
	//	return TIMESTEP_CHANNEL;
	//};

private:

	/**
	*	@brief check the input data. Make sure all the input data is available.
	*
	*	@return bool The validity of the input data.
	*/
	bool CheckInputData(void);

	/**
	*	@brief check the input size. Make sure all the input data have same dimension.
	*	
	*	@param key: The key of the input data
	*	@param n: The input data dimension
	*	@return bool The validity of the dimension
	*/
	bool CheckInputSize(const char*,int n);

	void clearInputs(void);
private:
	//inputs
	/// time step (hr)
	int   m_TimeStep;
	/// cell size of the grid (m)
	int   m_nCells;
	int   m_CellWidth;
	int   m_nSoilLayers;
	float m_upSoilDepth;

	/// the amount of water percolated from the soil water reservoir and input to the groundwater reservoir from the percolation module(mm)
	float **m_perc;
	/// evaporation from interception storage (mm) from the interception module
	float* m_D_EI;
	/// evaporation from the depression storage (mm) from the depression module
	float* m_D_ED;
	/// evaporation from the soil water storage (mm) from the soil ET module
	float* m_D_ES;
	/// PET(mm) from the interpolation module
	float* m_D_PET;
	/// initial ground water storage (or at time t-1)
	float m_GW0;
	/// maximum ground water storage
	float m_GWMAX;
	float *m_gwStore;
	/// groundwater Revap coefficient
	float m_dp_co;
	/// Baseflow recession coefficient
	float m_Kg;
	/// baseflow recession exponent
	float m_Base_ex;
	/// 1D array of valid cells to store subbasin ID 
	///float* m_Subbasin;
	/// the average slope of the subbasin
	float* m_Slope;
	/// the average slope of the watershed
	///float m_Slope_basin;
	float* m_petSubbasin;

	float **m_soilMoisture;
	float *m_rootDepth;
	
	float* m_VgroundwaterFromBankStorage; //ground water from bank storage, passed from channel routing module

	//output
	float* m_T_RG;
	float* m_T_QG; 
	float* m_D_Revap;
	float** m_T_GWWB;


	//used to output timeseries result for soil water balance
	float* m_subbasin;				//subbasin grid
	int m_subbasinSelectedCount;
	float* m_subbasinSelected;		//subbasin selected to output
	vector<subbasin*> m_subbasinList;
	int m_nSubbasins;
	//map<int, vector<int>* > m_cellListMap;

	void getSubbasinList();
};
#endif

