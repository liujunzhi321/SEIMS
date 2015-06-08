/** 
*	@file
*	@version	1.0
*	@author	Junzhi Liu
*	@date	17-January-2010
*
*	@brief	Interpolation Module
*
*	Revision:
*   Date:
*/
#ifndef SEIMS_INTERPOLATE_INCLUDE
#define SEIMS_INTERPOLATE_INCLUDE

#include <string>
#include <ctime>
#include <fstream>
#include "SimulationModule.h"
//#include "SEIMSModuleSetting.h"
using namespace std;

class Interpolate : public SimulationModule
{
public:
	Interpolate();
	//Interpolate(SEIMSModuleSetting* setting);
	~Interpolate(void);

	virtual void SetDataType(float value);

	virtual int Execute();
	virtual void SetDate(time_t date);
	virtual void SetValue(const char* key, float value);
	virtual void Set1DData(const char* key, int n, float* value);
	virtual void Set2DData(const char *key, int nRows, int nCols, float **data);
	virtual void Get1DData(const char* key, int* n, float** data);

	/** 
	*	@brief check length of the input variable
	*
	*	@param const char* key: the key to identify the requested data [in]
	*	@param int n: size of the input 1D data [in]
	*	@param int& m_n: the corresponding member varaible of length [in/out]
	*/
	bool CheckInputSize(string& key, int n, int& m_n);
	void CheckInputData();
	
private:

	/// the data type
	int m_dataType;
	/// count of stations
	int m_nStatioins;
	/// data of stations
	float *m_stationData; 
	/// count of valid cells
	int m_nCells;
	/// weights of input points for cells
	float *m_weights;

	/// whether using vertical interpolation
	bool m_vertical;
	/// elevation of stations
	float *m_hStations;
	/// elevation of cells
	float *m_dem;
	/** 
	*	@brief lapse rate
	*
	*	This is a 2D array. The first level is by month, and the second level is by data type in order of (P,T,PET).
	*/
	float **m_lapseRate;
	/// month
	int m_month;

	/// interpolation result
	float *m_output;

	//ofstream ofs;
};

#endif