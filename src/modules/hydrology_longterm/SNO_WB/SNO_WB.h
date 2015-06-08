#pragma once
#include <string>
#include <ctime>
#include "api.h"
#include <map>
#include "subbasin.h"
#include "SimulationModule.h"

using namespace std;

class SNO_WB:public SimulationModule
{
public:
	SNO_WB(void);
	~SNO_WB(void);
	virtual int Execute();
	virtual void SetValue(const char* key, float data);
	virtual void GetValue(const char* key, float* value);
	virtual void Set1DData(const char* key, int n, float* data);
	virtual void Get1DData(const char* key, int* n, float** data);
	virtual void Get2DData(const char* key, int* nRows, int* nCols, float*** data);

	bool CheckInputSize(const char* key, int n);
	bool CheckInputData(void);

private:

	int m_cellSize;
	time_t m_Date;

	float m_t0;
	float m_tsnow;
	float m_kblow;
	float m_swe0; 

	float* m_SA;  //snow accumulation 	
	float** m_snowWaterBalance; //the output average values of selectd subbasin	
	float* m_Pnet;//net precipitation 
	float* m_SR; //snow redistribution 
	float* m_SE; //snow sublimation 
	float* m_SM; //snow melt

	float m_SWE; //total average SA of the watershed

	//following four variable don't partispate caculation, just for output
	float* m_P;	//precipitation
	float* m_tMin; //snow temperature
	float* m_tMax;

	float* m_WindSpeed;
	int m_wsSize;

	float* m_subbasin;	//subbasin grid
	int m_subbasinSelectedCount;
	float* m_subbasinSelected;	//subbasin selected to output
	map<int,subbasin*>* m_subbasinList;

	bool m_isInitial;

	void initalOutputs();

	void setValueToSubbasin(void);

	void getSubbasinList(int cellCount, float* subbasinGrid, int subbasinSelectedCount, float* subbasinSelected);
};

