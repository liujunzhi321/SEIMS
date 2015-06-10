/*!
 * \file SimulationModule.h
 * \brief Parent class for all modules in SEIMS
 *
 * 
 *
 * 
 */
#pragma once
#include <string>
#include "ModelException.h"
#include <omp.h>


using namespace std;
//! enumeration of time step
enum TimeStepType
{
	TIMESTEP_HILLSLOPE,///< Hillslope scale
	TIMESTEP_CHANNEL,  ///< Channel scale
	TIMESTEP_ECOLOGY   ///< Ecology scle
};
/*!
 * \class SimulationModule
 *
 * \brief 
 *
 *
 *
 */
class SimulationModule
{
public:
	//! Constructor
	SimulationModule(void);
	//! Destructor
	virtual ~SimulationModule(void);
	//! Execute the simulation
	virtual int Execute() {return -1;};
	//! Set date time
	virtual void SetDate(time_t t)
	{
		m_date = t;
	};
	//! Set thread number for OpenMP 
	virtual void SetTheadNumber(int threadNum)
	{
		omp_set_num_threads(threadNum);
	};
	//! Set data type among 
	virtual void SetDataType(float value)
	{
		
	};


	virtual void SetValue(const char* key, float data)
	{
		throw ModelException("SimulationModule", "SetValue", "This function is not implemented.");
	};
	virtual void Set1DData(const char* key, int n, float* data)
	{
		throw ModelException("SimulationModule", "Set1DData", "This function is not implemented.");
	};
	virtual void Set2DData(const char* key, int nRows, int nCols, float** data)
	{
		throw ModelException("SimulationModule", "Set2DData", "This function is not implemented.");
	};

	
	virtual void GetValue(const char* key, float* value)
	{
		throw ModelException("SimulationModule", "GetValue", "This function is not implemented.");
	};
	virtual void Get1DData(const char* key, int* n, float** data)
	{
		throw ModelException("SimulationModule", "Get1DData", "This function is not implemented.");
	};
	virtual void Get2DData(const char* key, int* nRows, int* nCols, float*** data)
	{
		throw ModelException("SimulationModule", "Get2DData", "This function is not implemented.");
	};

	virtual TimeStepType GetTimeStepType()
	{
		return  TIMESTEP_HILLSLOPE;
	};

	virtual void ResetSubTimeStep()
	{
		m_tsCounter = 1;
	};

	//virtual void SetID(string& id)
	//{
	//	m_id = id;
	//};

	//virtual string GetID()
	//{
	//	return m_id;
	//};
protected:
	time_t m_date;
	//string m_id;

	int m_tsCounter;  //! sub-timestep counter
};

