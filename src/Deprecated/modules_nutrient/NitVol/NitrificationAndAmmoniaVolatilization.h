#pragma once

#include <string>
#include <ctime>
#include <cmath>
#include <map>
#include "SimulationModule.h"
using namespace std;

class NitrificationAndAmmoniaVolatilization : public SimulationModule
{
public:
	NitrificationAndAmmoniaVolatilization(void);
	~NitrificationAndAmmoniaVolatilization(void);

	virtual int Execute();

	virtual void SetValue(const char* key, float data);
	virtual void Set1DData(const char* key, int n, float* data);
	//virtual void Get1DData(const char* key, int* n, float** data);
	virtual void Set2DData(const char* key, int nrows, int ncols, float** data);
	virtual void Get2DData(const char* key, int *nRows, int *nCols, float*** data);

	bool CheckInputSize(const char* key, int n);
	bool CheckInputData(void);

private:

	void initalOutputs();
	
	// size of array 
	int m_size;
	int m_nLayers;
    
	// input
	//float *m_rootDepth;
	// volatilization cation exchange factor
	float m_catEF;

	// soil temperature of layer ly(¡æ)
    float *m_SoilT;
	// water content of layer ly on a given day(mm H2O)
	float **m_SOMO;
	//amount of water held in the soil layer at wilting point water content (mm H2O)
	float **m_Wiltingpoint;
	// water content of layer ly at field capacity (mm H2O)
	float **m_FieldCap;
	// depth of the layer (mm)
	float **m_Depth;
	//amount of ammonium in layer ly (kg N/ha)
	float **m_Ammon;
	// amount of nitrate in layer ly (kg N/ha)
	float **m_Nitrate;

	// output
	//depth from the soil surface to the middle of the layer (mm)
	float **m_midZ;
	// nitrification/volatilization temperature factor 
	float **m_nitvolTF;
	// nitrification soil water factor
	float **m_nitWF;
	//volatilization depth factor
	float **m_volDF;
	//nitrification regulator
	float **m_nitR;
	//volatilization regulator
	float **m_volR;
	//amount of ammonium converted via nitrification and volatilization in layer ly (kg N/ha)
	float **m_nitvolAmmon;
	//estimated fraction of nitrogen lost by nitrification
	float **m_nitLostF;
	//estimated fraction of nitrogen lost by volatilization
	float **m_volLostF;

	//amount of nitrogen converted from NH4 to NO3 in layer ly (kg N/ha)
	float **m_conNitra;
	//amount of nitrogen converted from NH4 to NH3 in layer ly (kg N/ha)
	float **m_conAmm;

};