/*!
 * \file MUSLE_AS.h
 * \brief use MUSLE method to calculate sediment yield of each cell
 * \author Zhiqiang Yu
 * \date Feb. 2012
 * \revised LiangJun Zhu
 * \revised date May. 2016
 */
#pragma once
#include <string>
#include <ctime>
#include "api.h"

using namespace std;
#include "SimulationModule.h"
/** \defgroup MUSLE_AS
 * \ingroup Erosion
 * \brief use MUSLE method to calculate sediment yield of each cell
 */
/*!
 * \class MUSLE_AS
 * \ingroup MUSLE_AS
 *
 * \brief use MUSLE method to calculate sediment yield of each cell
 *
 */
class MUSLE_AS:public SimulationModule
{
public:
	MUSLE_AS(void);
	~MUSLE_AS(void);
	virtual int Execute();
	virtual void SetValue(const char* key, float data);
	virtual void GetValue(const char* key, float* value);
	virtual void Set1DData(const char* key, int n, float* data);
	virtual void Get1DData(const char* key, int* n, float** data);

	bool CheckInputSize(const char* key, int n);
	bool CheckInputData(void);

private:

	int m_nCells;
	float m_cellWidth;
	int m_nsub;

	//grid from parameter
	float* m_usle_p;
	float* m_usle_k;
	float* m_usle_c;
	float* m_slope;
	float* m_flowacc;
	/// stream link
	float *m_streamLink;
	float *m_subbasin;

	float* m_usle_ls;   //LS factor
	float m_cellAreaKM;	//A in qp, km^2
	float m_cellAreaKM1;//3.79*A^0.7 in qp
	float m_cellAreaKM2;//0.903 * A^0.017 in qp
	float* m_slopeForPq;//S^0.16 in qp

	//grid from module
	float* m_snowAccumulation;
	float* m_surfaceRunoff;

	//result
	float* m_sedimentYield;
	float* m_sedtoCh;
	float m_sedtoCh_T;
	/// deposition ratio 
	float m_depRatio;  // added by Wu Hui, 2013.11.16

	void initalOutputs();

	float getPeakRunoffRate(int);
};

