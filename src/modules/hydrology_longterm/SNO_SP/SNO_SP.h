#pragma once
#include <string>
#include <ctime>
#include "api.h"

using namespace std;
#include "SimulationModule.h"

class SNO_SP:public SimulationModule
{
public:
	SNO_SP(void);
	~SNO_SP(void);
	virtual int Execute();
	virtual void SetValue(const char* key, float data);
	virtual void Set1DData(const char* key, int n, float* data);
	virtual void Get1DData(const char* key, int* n, float** data);

	bool CheckInputSize(const char* key, int n);
	bool CheckInputData(void);

private:

	int m_cellSize;
	time_t m_Date;//

	float m_t0;
	//float m_csnow;
	//float m_crain;
	float m_kblow;
	float m_swe;
	float m_lastSWE;
	float m_tsnow;
	float m_swe0;
	float m_lagSnow;
	float m_csnow6;
	float m_csnow12;

	float* m_tMin;
	float* m_tMax;
	float* m_Pnet;
	float* m_SA;	
	float* m_SR;
	float* m_SE;

	//temp fro snow pack temperature
	float* m_packT;

	//result
	float* m_SM;

	bool m_isInitial;

	/**
	*	@brief Get the Julian day of one day
	*/
	int JulianDay(time_t);

	float CMelt();

	void initalOutputs();
};

