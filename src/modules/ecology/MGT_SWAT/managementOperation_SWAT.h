/*!
 * \file managementOperation_SWAT.h
 * \brief All management operation in SWAT, e.g., plantop, killop, harvestop, etc.
 * \author Liang-Jun Zhu
 * \date June 2016
 *           1. Source code of SWAT include: readmgt.f, operatn.f, sched_mgt.f, plantop.f, dormant.f, harvkillop.f, harvestop.f, killop.f, tillmix.f
 * 
 */
#pragma once
#include <string>
#include "api.h"
#include "util.h"
#include "ClimateParams.h"
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
	/// Julian day
	int m_jDay;
	/// current sequence number of management operations
	int* m_curNOP;
	/// current management operation code
	int* m_curMgtOp;
	/// maximum management operations number
	int* m_maxNOP;
	/// plant growth code, 0 or 1
	float* m_igro;
	/// phu base
	float* m_phuBase;
	/// phu accumulated
	float* m_phuAcc;
	/// dormancy flat
	int* m_dormFlag;

	/// plant operation related parameters

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
	 * \brief Manager all operations on schedule
	 * \param[in] cellIdx Index of valid cell
	 * \param[in] mgtOpCode Management operation code, 0~16
	 */
	void ScheduledManagement(int cellIdx, int mgtOpCode);
	/*!
	 * \brief Get operation parameters according to operation sequence number
	 * \param[in] nOp Operation sequence number
	 * \param[out] mgtOpCode Management operation code
	 * \param[out] jDay Operation Julian date
	 * \param[out] phuOp Fraction of heat units (PHU0 or PHUPLT) of current operation
	 , int& mgtOpCode, int& jDay, float& phuOp
	 */
	bool GetOperationParameters(int cellIdx, int nOp);
	void PlantOperation(int cellIdx);
	void IrrigationOperation(int cellIdx);
	void FertilizerOperation(int cellIdx);
	void PesticideOperation(int cellIdx);
	void HarvestKillOperation(int cellIdx);
	void TillageOperation(int cellIdx);
	void HarvestOnlyOperation(int cellIdx);
	void KillOperation(int cellIdx);
	void GrazingOperation(int cellIdx);
	void AutoIrrigationOperation(int cellIdx);
	void AutoFertilizerOperation(int cellIdx);
	void StreetSweepingOperation(int cellIdx);
	void ReleaseImpoundOperation(int cellIdx);
	void ContinuousFertilizerOperation(int cellIdx);
	void ContinuousPesticideOperation(int cellIdx);
	void BurningOperation(int cellIdx);
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

