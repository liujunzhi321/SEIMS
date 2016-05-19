/** \defgroup SurTra
 * \ingroup Nutrient
 * \brief Nutrient removed and lost in surface runoff.
 */
/*!
 * \file SurrunoffTransfer.h
 * \ingroup SurTra
 * \author Huiran Gao
 * \date April 2016
 */

#pragma once
#ifndef SEIMS_SurTra_PARAMS_INCLUDE
#define SEIMS_SurTra_PARAMS_INCLUDE
#include <string>
#include "api.h"
#include "SimulationModule.h"
using namespace std;
/*!
 * \class SurrunoffTransfer
 * \ingroup SurTra
 *
 * \brief Nutrient removed and lost in surface runoff
 *
 */

class SurrunoffTransfer : public SimulationModule {
	public:
	SurrunoffTransfer(void);
	~SurrunoffTransfer(void);

	virtual void Set1DData(const char* key, int n, float *data);
	virtual void Set2DData(const char* key, int nRows, int nCols, float** data);
	virtual void SetValue(const char* key, float value);
	virtual int Execute();
	//virtual void GetValue(const char* key, float* value);
	virtual void Get1DData(const char* key, int* n, float** data);
	virtual void Get2DData(const char* key, int* nRows, int* nCols, float*** data);
private:
	/// cell width of grid map (m)
	float m_cellWidth;
	/// number of cells
	int m_nCells;
	int m_nSolLyrs;

	///input data
	//distribution of soil loss caused by water erosion
	float* m_sedimentYield;
	//distribution of surface runoff generated
	float* m_surfr;
	//bulk density of the soil
	float** m_sol_bd;
	//depth to bottom of soil layer
	float** m_sol_z;
	//??
	float** m_sol_mp;

	///output data
	//amount of organic nitrogen in surface runoff
	float* m_sedorgn;
	//amount of organic phosphorus in surface runoff
	float* m_sedorgp;
	//amount of active mineral phosphorus sorbed to sediment in surface runoff
	float* m_sedminpa;
	//amount of stable mineral phosphorus sorbed to sediment in surface runoff
	float* m_sedminps;

	///input & output
	//amount of nitrogen stored in the active organic (humic) nitrogen pool
	float** m_sol_aorgn;
	//amount of nitrogen stored in the fresh organic (residue) pool
	float** m_sol_fon;
	//amount of nitrogen stored in the stable organic N pool
	float** m_sol_orgn;
	//amount of phosphorus stored in the organic P pool
	float** m_sol_orgp;
	//amount of phosphorus stored in the fresh organic (residue) pool
	float** m_sol_fop;
	//amount of phosphorus in the soil layer stored in the stable mineral phosphorus pool
	float** m_sol_stap;
	//amount of phosphorus stored in the active mineral phosphorus pool
	float** m_sol_actp;

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
	* \brief Calculate enrichment ratio.
	 *
	 * \return void
	 */
	float* CalculateEnrRatio();

	/*!
	* \brief calculates the amount of organic nitrogen removed in surface runoff.
	 *
	 * \return void
	 */
	void OrgnRemoveinSr();
	
	/*!
	* \brief Calculates the amount of organic and mineral phosphorus attached to sediment in surface runoff.
	 *
	 * \return void
	 */
	void OrgpAttachedtoSed();

	
};
#endif





