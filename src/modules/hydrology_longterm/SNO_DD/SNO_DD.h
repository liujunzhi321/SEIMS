#pragma once
#include <string>
#include <ctime>
#include "api.h"

using namespace std;
#include "SimulationModule.h"

/** \defgroup SNO_DD
 * \ingroup Hydrology_longterm
 * \brief Calculate snow accumulation and istribution of snowmelt
 *
 */

/*!
 * \class SNO_DD
 * \ingroup SNO_DD
 * \brief Calculate snow accumulation and istribution of snowmelt
 * 
 */
class SNO_DD:public SimulationModule
{
public:
	SNO_DD(void);
	~SNO_DD(void);
	virtual int Execute();
	virtual void SetValue(const char* key, float data);
	virtual void Set1DData(const char* key, int n, float* data);
	virtual void Get1DData(const char* key, int* n, float** data);

	bool CheckInputSize(const char* key, int n);
	bool CheckInputData(void);

private:

	int m_nCells;

	float m_t0;
	float m_csnow;
	float m_crain;
	float m_kblow;
	float m_swe;
	float m_lastSWE;
	float m_tsnow;
	float m_swe0;

	float* m_tMin;
	float* m_tMax;
	float* m_Pnet;
	float* m_SA;	
	float* m_SR;
	float* m_SE;

	//result
	float* m_SM;

	bool m_isInitial;

	void initalOutputs();
};

