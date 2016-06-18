#pragma once
#include <string>
#include <vector>
#include <string>
#include <sstream>
#include "api.h"
using namespace std;

#include "SimulationModule.h"
/** \defgroup SSR_DA
 * \ingroup Hydrology_longterm
 * \brief --
 *
 */

/*!
 * \class SSR_DA
 * \ingroup SSR_DA
 * \brief --
 * 
 */
class SSR_DA : public SimulationModule
{
private:
	/// number of soil layers
	int m_nSoilLayers;
	/// depth of the up soil layer
	float m_upSoilDepth;

	int m_nCells;
	int m_dt;
	float m_CellWidth;

	float m_ki;
	float m_frozenT;

	float *m_rootDepth;
	float *m_slope;

	float **m_ks;
	float **m_porosity;
	float **m_poreIndex;
	float **m_fc;
	float **m_sm;

	float *m_soilT;	

	/// channel width
	float *m_chWidth;
	float *m_streamLink;

	/**
	*	@brief 2d array of flow in cells
	*
	*	The first element in each sub-array is the number of flow in cells in this sub-array
	*/
	float **m_flowInIndex;

	/**
	*	@brief percentage of flow out to current cell from each upstream cells
	*
	*	It has the same data stucture as m_flowInIndex.
	*/
	float **m_flowInPercentage;

	/**
	*	@brief Routing layers according to the flow direction
	*
	*	There are not flow relationships within each layer.
	*	The first element in each layer is the number of cells in the layer
	*/
	float **m_routingLayers;
	int m_nRoutingLayers;

	int m_nSubbasin;  
	/// subbasin grid ( subwatersheds ID)
	float* m_subbasin;


	//result
	float **m_qi;
	float **m_qiVol;
	float *m_qiSubbasin;

public:
	SSR_DA(void);
	~SSR_DA(void);

	virtual int Execute();

	virtual void SetValue(const char* key, float data);
	virtual void Set1DData(const char* key, int nRows, float* data);
	virtual void Set2DData(const char* key, int nrows, int ncols, float** data);
	virtual void Get1DData(const char* key, int* n, float** data);
	virtual void Get2DData(const char* key, int *nRows, int *nCols, float*** data);

private:
	/**
	*	@brief check the input data. Make sure all the input data is available.
	*
	*	@return bool The validity of the input data.
	*/
	bool CheckInputData(void);

	/**
	*	@brief checke the input size. Make sure all the input data have same dimension.
	*	
	*	@param key The key of the input data
	*	@param n The input data dimension
	*	@return bool The validity of the dimension
	*/
	bool CheckInputSize(const char*,int);

	void initialOutputs();

	void FlowInSoil(int id);
};

