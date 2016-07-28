/*----------------------------------------------------------------------
*	Purpose: 	channel flow routing using variable storage method at daily time scale
*
*	Created:	Wu Hui
*	Date:		26-Jule-2012
*
*	Revision:
*   Date:
*---------------------------------------------------------------------*/
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


class VSM_CH : public SimulationModule
{
public:
	VSM_CH(void);
	~VSM_CH(void);

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

private:
	/// time step (hr)
	int m_dt;
	/// reach number (= subbasin number)
	int m_nreach;
	
	/// diversion loss (Vdiv) of the river reach .. m_Vid[id], id is the reach id
	float* m_Vdiv;
	/// The point source discharge .. m_Vpoint[id], id is the reach id
	float* m_Vpoint;
	
	/// hydraulic conductivity of the channel bed (mm/h)
	float m_Kchb;
	/// hydraulic conductivity of the channel bank (mm/h)
	float m_Kbank;
	/// reach evaporation adjustment factor;
	float m_Epch;
	/// initial bank storage per meter of reach length (m3/m)
	float m_Bnk0;
	/// initial channel storage per meter of reach length (m3/m)
	float m_Chs0;
	/// the initial volume of transmission loss to the deep aquifer over the time interval (m3/s)
	float m_Vseep0;   //added
	/// bank flow recession constant
	float m_aBank;
	/// bank storage loss coefficient
	float m_bBank;
	/// flood plain side slope of channel, is a fixed number
	float m_sideslopeFloodplain;
	/// side slope of main channel, is a fixed number
	float m_sideslopeMain;
	/// reach erodibility factor, a fixed number
	float m_erodibilityFactor; 
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
	/// sediment from subbasin
	float* m_sedtoCh; 
	/// the subbasin area (m2)  //add to the reach parameters file
	float* m_area;
	/// cross-sectional area of flow in the channel (m2) 
	float* m_CrAreaCh;
	
	/// 
	float* m_subbasin;				//subbasin grid

	/// Average PET for each subbasin
	float* m_PETch;

	/// overland flow to streams from each subbasin (m3/s)
	float* m_qsSub;
	/// interflow to streams from each subbasin (m3/s)
	float* m_qiSub;
	/// groundwater flow out of the subbasin (m3/s)
	float* m_qgSub;
	///  Groundwater storage (mm) of the subbasin
	float* m_gwStorage;

	/// channel outflow
	float *m_qsCh;
	float *m_qiCh;
	float *m_qgCh;

	float *m_chOrder;
	float *m_chWidth;
	float *m_chDepth;
	float *m_chLen;
	float *m_chVel;
	float *m_chSlope;
	float *m_chManning;

	float *m_bankStorage;

	/// seepage to deep aquifer
	float *m_seepage;

	/// downstream id (The value is 0 if there if no downstream reach)
	float *m_reachDownStream;
	/// upstream id (The value is -1 if there if no upstream reach)
	vector< vector<int> > m_reachUpStream;

	// id the reaches
	float* m_reachId;
	/// map from subbasin id to index of the array
	map<int, int> m_idToIndex;

	// OUTPUT
	/// Discharge in a text format at each reach outlet and at each time step
	float** m_Q_RECH;
	/// Channel water balance in a text format for each reach and at each time step
	float** m_C_WABA;

	//temporary at routing time
	/// reach storage (m3) at time t
	float* m_chStorage;
	/// initial reach outflow (m3/s) at time t
	float* m_qOut;

	map<int, vector<int> > m_reachLayers;

	void initalOutputs();

	void ChannelFlow(int i);

	float Q(float watDepth, int id);
	float crossSectionalArea(float waterDepth, int id);
	float wettedPerimeter(float waterDepth, int id);
	float depth(float Q, int id);
	float calOutletDischarge(float qs, float qi, float qg, int id);
	float fSimple(float area1,float Qin,float depth, int id);

};

