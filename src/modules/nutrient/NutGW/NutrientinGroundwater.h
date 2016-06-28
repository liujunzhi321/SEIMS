/** \defgroup NutGW
 * \ingroup Nutrient
 * \brief Calculates the nitrate and soluble phosphorus loading contributed by groundwater flow.
 */
/*!
 * \file NutrientinGroundwater.h
 * \ingroup NutGW
 * \author Huiran Gao
 * \date Jun 2016
 */

#pragma once
#ifndef SEIMS_NutRemv_PARAMS_INCLUDE
#define SEIMS_NutRemv_PARAMS_INCLUDE
#include <string>
#include "api.h"
#include "SimulationModule.h"
using namespace std;
/*!
 * \class NutrientinGroundwater
 * \ingroup NutGW
 *
 * \brief Calculates the nitrate and soluble phosphorus loading contributed by groundwater flow.
 *
 */

class NutrientinGroundwater : public SimulationModule {
	public:
	NutrientinGroundwater(void);
	~NutrientinGroundwater(void);

	virtual void SetValue(const char* key, float value);
	virtual void Set1DData(const char* key, int n, float *data);
	//virtual void Set2DData(const char* key, int nRows, int nCols, float** data);
	virtual int Execute();
	//virtual void GetValue(const char* key, float* value);
	virtual void Get1DData(const char* key, int* n, float** data);
	//virtual void Get2DData(const char* key, int* nRows, int* nCols, float*** data);
private:
	/// cell width of grid map (m)
	float m_cellWidth;
	/// number of cells
	int m_nCells;

	/// input data
	/// nitrate N concentration in groundwater loading to reach (mg/L)
	float* m_gwno3;
	/// soluble P concentration in groundwater loading to reach (mg/L)
	float* m_gwminp;
	/// groundwater contribution to stream flow
	float* m_gw_q;

	/// output data
	/// nitrate loading to reach in groundwater
	float* m_no3gw;
	/// soluble P loading to reach in groundwater
	float* m_minpgw;

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

	void initialOutputs();
	
};
#endif





