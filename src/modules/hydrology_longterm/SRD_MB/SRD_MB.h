#pragma once
#include <string>
#include <ctime>
#include "api.h"

using namespace std;
#include "SimulationModule.h"

class SRD_MB:public SimulationModule
{
public:
	SRD_MB(void);
	~SRD_MB(void);
	virtual int Execute();
	virtual void SetValue(const char* key, float data);
	virtual void Set1DData(const char* key, int n, float* data);
	virtual void Get1DData(const char* key, int* n, float** data);

	bool CheckInputSize(const char* key, int n);
	bool CheckInputData(void);

private:

	time_t m_Date;//

	int m_cellSize;

	//single from parameter
	float m_ut0;
	float m_u0;
	float m_t_wind;
	float m_shc_crop;
	float m_k_slope;
	float m_k_curvature;
	float m_swe;
	float m_swe0;
	float m_lastSWE;
	float m_kblow;
	float m_t0;
	float m_tsnow;

	bool m_isInitial;

	//time series from module
	float* m_ws;	//wind speed time series data
	int m_wsSize;	//number of wind speed sites

	//grid from parameter
	float* m_curva_wind;
	float* m_slope_wind;
	float* m_shc;
	float* m_tMin;
	float* m_tMax;

	//grid from module
	float* m_SA;
	float* m_Pnet;

	//result
	float* m_SR;

	//temp array
	float* m_w;	//w array
	float* m_wt; // wt array

	void initalOutputs();
};

