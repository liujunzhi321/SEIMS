#pragma once
#include <string>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include "subbasin.h"
#include "api.h"
using namespace std;

#include "SimulationModule.h"
/** \defgroup SOL_WB
 * \ingroup Hydrology_longterm
 * \brief Soil water balance
 *
 */

/*!
 * \class SOL_WB
 * \ingroup SOL_WB
 * \brief Soil water balance
 * 
 */
//typedef vector<vector<double>> double2DArray;

class SOL_WB : public SimulationModule
{
private:

	int m_nCells;
	int m_nSoilLayers;
	//time_t m_Date; there is no need to define date here. By LJ.

	float m_upSoilDepth;

	float *m_pNet;
	float *m_Rootdepth;
	float *m_Infil;
	float *m_ES;
	float *m_Revap;

	float **m_RI;
	float **m_Percolation;
	
	float **m_sm;			//distribution of soil moisture		

	float **m_soilWaterBalance;	//time seriese result

	//variables used to output
	float* m_Precipitation;
	float* m_Interception;
	float* m_Depression;
	float* m_EI;
	float* m_ED;
	float* m_RS;
	float* m_RG;
	float* m_SE;
	float* m_tMax;
	float* m_tMin;
	float* m_SoilT;

	int m_subbasinTotalCount;
	
	//used to output timeseries result for soil water balance
	float* m_subbasin;				//subbasin grid
	int m_subbasinSelectedCount;
	float* m_subbasinSelected;		//subbasin selected to output
	map<int,subbasin*>* m_subbasinList;

public:
	SOL_WB(void);
	~SOL_WB(void);

	virtual void SetValue(const char* key, float data);
	virtual void Set1DData(const char* key, int nRows, float* data);
	virtual void Set2DData(const char* key, int nrows, int ncols, float** data);
	virtual void Get2DData(const char* key, int* nRows, int* nCols, float*** data);

	virtual int Execute(void);


private:
	/**
	*	@brief check the input data. Make sure all the input data is available.
	*/
	void CheckInputData(void);

	/**
	*	@brief check the input size. Make sure all the input data have same dimension.
	*	
	*	@param key The key of the input data
	*	@param n The input data dimension
	*	@return bool The validity of the dimension
	*/
	bool CheckInputSize(const char*,int);

	void setValueToSubbasin(void);

	void getSubbasinList(int cellCount, float* subbasinGrid, int subbasinSelectedCount, float* subbasinSelected);


};

