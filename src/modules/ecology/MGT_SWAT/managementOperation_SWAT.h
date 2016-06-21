/*!
 * \file managementOperation_SWAT.h
 * \brief All management operation in SWAT, e.g., plantop, killop, harvestop, etc.
 * \author Liang-Jun Zhu
 * \date June 2016
 *           1. Source code of SWAT include: operatn.f, sched_mgt.f, plantop.f, dormant.f, harvkillop.f, harvestop.f, killop.f, tillmix.f
 * 
 */
#pragma once
#include <string>
#include "api.h"
#include "util.h"
#include "SimulationModule.h"
using namespace std;
/** \defgroup MGT_SWAT
 * \ingroup Ecology
 * \brief All management operation in SWAT, e.g., plantop, killop, harvestop, etc.
 */
/*!
 * \class MGTOpt_SWAT
 * \ingroup MGT_SWAT
 *
 * \brief All management operation in SWAT, e.g., plantop, killop, harvestop, etc.
 * 
 */
class MGTOpt_SWAT : public SimulationModule
{
private:
	/// valid cells number
	int m_nCells;
public:
	//! Constructor
	MGTOpt_SWAT(void);
	//! Destructor
	~MGTOpt_SWAT(void);
	virtual int Execute();
	virtual void Set1DData(const char* key, int n, float* data);
	virtual void Get1DData(const char* key, int* n, float** data);
	virtual void Set2DData(const char* key, int n, int col, float** data);
	virtual void Get2DData(const char* key, int* n, int* col, float*** data);
private:
	/*!
	 * \brief check the input data. Make sure all the input data is available.
	 * \return bool The validity of the input data.
	 */
	bool CheckInputData(void);

	/*!
	 * \brief check the input size. Make sure all the input data have same dimension.
	 *
	 *
	 * \param[in] key The key of the input data
	 * \param[in] n The input data dimension
	 * \return bool The validity of the dimension
	 */
	bool CheckInputSize(const char*,int);
};

