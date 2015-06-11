/*----------------------------------------------------------------------
*	Purpose: 	nutrient routing using simple storage method at daily time scale
*
*	Created:	Wang Lin
*	Date:		29-March-2015
*
*	Revision:
*   Date:
*---------------------------------------------------------------------*/
#ifndef REACH_SEDIMENT_ROUTING_CONST
#define REACH_SEDIMENT_ROUTING_CONST

#define SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_NAME								"T_CHSB"

#define SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_COUNT						6

#define SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_UPSTREAM				0
#define SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_SUBBASIN				1
#define SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_DEPOSITION			2
#define SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_DEGRADATION			3
#define SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_STORAGE				4
#define SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_OUT					5

#endif

#ifndef REACH_FLOW_ROUTING_CONST
#define REACH_FLOW_ROUTING_CONST

#define FLOW_CHANNEL_WB_NAME								"T_CHSB"

#define FLOW_CHANNEL_WB_COLUMN_COUNT						7

#define FLOW_CHANNEL_WB_COLUMN_INDEX_QOVERLAND				0
#define FLOW_CHANNEL_WB_COLUMN_INDEX_QINTERL				1
#define FLOW_CHANNEL_WB_COLUMN_INDEX_QGROUND				2
#define FLOW_CHANNEL_WB_COLUMN_INDEX_CHSTORAGE				3
#define FLOW_CHANNEL_WB_COLUMN_INDEX_BKSTORAGE				4
#define FLOW_CHANNEL_WB_COLUMN_INDEX_SEEPAGE				5
#define FLOW_CHANNEL_WB_COLUMN_INDEX_WTDEPTH				6

#endif

#define DEPTH_INTERVAL 0.001f

#pragma once

#include <string>
#include <ctime>
#include <cmath>
#include <map>
#include <vector>
#include "SimulationModule.h"

using namespace std;


class NutTra_CH : public SimulationModule
{
public:
	NutTra_CH(void);
	~NutTra_CH(void);

	virtual int Execute();
	
	virtual void SetValue(const char* key, float data);
	virtual void GetValue(const char* key, float* value);
	virtual void Set1DData(const char* key, int n, float* data);
	virtual void Get1DData(const char* key, int* n, float** data);
	virtual void Set2DData(const char* key, int nrows, int ncols, float** data);
	virtual void Get2DData(const char* key, int *nRows, int *nCols, float*** data);

	bool CheckInputSize(const char* key, int n);
	bool CheckInputSizeChannel(const char* key, int n);
	bool CheckInputData(void);

	virtual TimeStepType GetTimeStepType()
	{
		return TIMESTEP_CHANNEL;
	};
private:
	/// time step (hr)
	int m_dt;
	/// reach number (= subbasin number)
	int m_nreach;
	
/*	/// diversion loss (Vdiv) of the river reach .. m_Vid[id], id is the reach id
	float* m_Vdiv;
	/// The point source discharge .. m_Vpoint[id], id is the reach id
	float* m_Vpoint;
*/	
	/// the peak rate adjustment factor
	float m_prf;
	///  Coefficient in sediment transport equation
	float m_spcon;
	/// Exponent in sediment transport equation
	float m_spexp;
	/// critical veloctiy for sediment deposition
	float m_vcrit;
	/// reach cover factor
	//float m_coverFactor;
	/// channel erodibility factor (cm/hr/Pa)
	//float m_erodibilityFactor;

/*	/// inverse of flood plain side slope of channel, is a fixed number:  1/slope
	float m_sideslopeFloodplain;
	/// inverse of side slope of main channel, is a fixed number:  1/slope
	float m_sideslopeMain;
	/// width ratio
	float m_w_ratio;
	/// bank full water storage
	float m_bankfullQ;
	//temp var
	/// bottom width of the channel(m)
	float *m_widthbottom;
	/// current width of channel
	float *m_widthcurrent;
	/// current depth of channel
	float *m_depthcurrent;
	/// current slope of channel
	float *m_slopecurrent;
*/

	/// nutrient from subbasin


	// surface nitrate to channel (kg)
	float* m_sNittoCh;
	// lateral nitrate to channel (kg)
	float* m_laNittoCh;
	//surface ammonium to channel (kg)
	float* m_sAmmtoCh;
	// surface soluble phosphorus to channel (kg)
	float* m_sSolPtoCh;
	// the amount of organic nitrogen transported to channel in surface runoff(kg)
	float* m_orgNtoCh;
	// the amount of phosphorus attached to sediment transported to channel in surface runoff(kg)
	float* m_attPtoCh;



	/// the subbasin area (m2)  //add to the reach parameters file
	//float* m_area;
	/// cross-sectional area of flow in the channel (m2) 
	float* m_CrAreaCh;
	/// initial channel storage per meter of reach length (m3/m)
	float m_Chs0;

	//temporary at routing time
	/// reach storage (m3) at time t
	//float* m_chStorage;
	/// channel outflow 
	float *m_qchOut;

	float *m_chOrder;
	float *m_chWidth;
	float *m_chDepth;
	float *m_chLen;      //length of reach (m)
	float *m_chVel;
	float *m_chSlope;
	float *m_chManning;

	/// downstream id (The value is 0 if there if no downstream reach)
	float *m_reachDownStream;
	/// upstream id (The value is -1 if there if no upstream reach)
	vector< vector<int> > m_reachUpStream;

	// id the reaches
	float* m_reachId;
	/// map from subbasin id to index of the array
	map<int, int> m_idToIndex;

	/// overland flow to streams from each subbasin (m3/s)
	float* m_qsSub;
	/// interflow to streams from each subbasin (m3/s)
	float* m_qiSub;
	/// groundwater flow out of the subbasin (m3/s)
	float* m_qgSub;
	/// reach storage (m3) at time t
	float* m_chStorage;
	/// channel water depth m
	float *m_chWTdepth;
	
	// OUTPUT
	/// initial reach Nitrate out (kg) at time t
	float* m_NitOut;
	/// Channel sediment balance in a text format for each reach and at each time step
	//float** m_T_CHSB;
	/// channel nitrate storage (ton)
	float* m_nitStorage;
	/// total nitrate from upstream 
	float* m_nitUps;
	/// nitrate from current subbasin
	float* m_nitSub;
	/// nitrate of deposition 
	//float* m_nitDep;
	/// nitrate of degradation
	//float* m_nitDeg;
	/// initial reach Ammonium out (kg) at time t
	float* m_AmmOut;
	/// Channel sediment balance in a text format for each reach and at each time step
	//float** m_T_CHSB;
	/// channel ammonium storage (ton)
	float* m_ammStorage;
	/// total ammonium from upstream 
	float* m_ammUps;
	/// nitrate from ammonium subbasin
	float* m_ammSub;
	/// initial reach solution P out (kg) at time t
	float* m_SoluPOut;	
	/// channel solution P storage (ton)
	float* m_soluPStorage;
	/// total solution P from upstream 
	float* m_soluPUps;
	/// solution P from current subbasin
	float* m_soluPSub;
	/// initial reach organic N out (kg) at time t
	float* m_OrgNOut;
	/// channel organic N storage (ton)
	float* m_orgNStorage;
	/// total organic N from upstream 
	float* m_orgNUps;
	/// organic N from current subbasin
	float* m_orgNSub;
	/// initial reach attached P out (kg) at time t
	float* m_AttPOut;
	/// channel attached P storage (ton)
	float* m_attPStorage;
	/// total attached P from upstream 
	float* m_attPUps;
	/// attached P from current subbasin
	float* m_attPSub;

	map<int, vector<int> > m_reachLayers;

	void initalOutputs();

	void NutriChannelRouting(int i);

	void reset(int id);
	float Q(float watDepth, int id);
	//float crossSectionalArea(float waterDepth, int id);
	//float wettedPerimeter(float waterDepth, int id);
	//float depth(float Q, int id);
	//float calOutletDischarge(float qs, float qi, float qg, int id);
	//float fSimple(float area1,float Qin,float depth, int id);
	//void doChannelDowncuttingAndWidening(int id);
};

