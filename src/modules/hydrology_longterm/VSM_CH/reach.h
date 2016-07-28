#pragma once
#include "ModelException.h"
#include <vector>

using namespace std;

class reach
{
public:
	reach(void);
	~reach(void);

private:
	/// time step
	/// time step = time interval(hr) 
	int   m_dt;
	//reach parameters
	float* m_RchParam;
	///subbasin id = reach id = outlet id
	int   m_id;
	/// receiving reach ID (downstream reach ID)
	int   m_RecRchID;
	///  reach order with Shreve ordering method
	int   m_RchOrd;
	/// reach width (m)
	float m_RchWid;
	/// reach length (m)
	float m_RchLen;
	/// reach depth (m)
	float m_RchDep;
	/// average flow velocity of the reach (m/s)
	float m_RchVel;
	/// contributing area (ha) to the river reach
	float m_ContArea;
	/// code of flow diversion of the reach (0 = no diversion, 1= diversion)
	int m_RchDiv;
	/// code of point source into the reach (0 = no point source, 1 = point source)
	int m_RchPs;

	// using output time series of the reach
	/// surface flow calculated at the reach outlet
	float m_qsSub;
	/// interflow calculated at the reach outlet
	float m_qiSub;
	/// groundwater flow calculated at the reach outlet
	float m_qgSub;
	/// total flow rate in m3/s calculated at the reach outlet
	float m_Qm3s;
	/// total flow rate in mm calculated at the reach outlet (Q*time step/contributing area)
	float m_Qmm;

	/// total inflow from upstream reaches
	float m_Vin;
	/// the total side inflow from the subbasin
	float m_Vside;
	/// the total inflow (m3/s) to the reach = Vin + Vside
	float m_Qin;
	/// diversion loss (Vdiv ) of the river reach 
	float m_Vdiv;
	///volume of point source discharge over the time interval (m3)
	float m_Vpoint;
	/// the volume of transmission loss to the deep aquifer over the time interval (m3) 
	float m_Vseep;
	///The net volume of water moved from channel to the bank storage (-) or from bank storage to the channel (+) over the time interval (m3)
	float m_Vnb;
	/// the evaporation loss from the reach over the time interval (m3) 
	float m_Ech;	
	/// loss of water volume from the bank storage to the adjacent unsaturated zone and groundwater storage (m3)
	float m_Lbank;
	/// average bank storage over the time interval (m3)
	float m_Vbank;
	/// total outflow (m3) of the reach
	float m_Vout;
	/// reach storage (m3) at time t
	float m_chStorage;
	/// reach storage (m3) at time t+1
	float m_Vch2;
	/// the raw transmission loss to the bank storage from both sides of the channel (m3)
	float m_VbankIn;

	/// the volume of water added to the reach via return flow from bank storage (m3)
	float m_VbankOut;
	/// updated groundwater storage of the subbasin
	float m_gwStorage;

	/// the reach inlet which has already accounted for the transmission losses, evaporation losses, flow diversion and point source of the reach
	float m_Vinflow;
	/// at the reach outlet after reach flow routing
	float m_Voutflow;
		
	/// the total loss of the river reach (transmission loss, evaporation loss, and diversion loss)
	float m_Vtl;

	/// total outflow of previous timestep
	float m_TlOutflow;

	//temparory
	/// bank storage (m3) at previous time step
	float m_VBank1;

	void clear();

public:
	void setRchParameter(int id, int RecRchID, int RchOrd, float RchWid, float RchLen, float RchDep, float RchVel, float ContArea, int RchDiv, int RchPs);
	void setRchParameter(float* rchParam);
	void setVside(float QSside, float QIside, float QGside);
	void setRchInputs(float QS, float QI, float QG);
	void setRoutedQ(float QS, float QI, float QG);
	void setTlOutFlow(float tloutflow);
	void setTimeStep(int dt) {m_dt = dt;}

	//water balance 
	//void setWBParameters(int timestep, float Kchb, float Kbank, float Epch, float Bnk0, float Chs0, float Abnk, float Bbnk); 

	//Adjust Qs, Qi, and Qg of the reach accounting for channel losses and other sources
	void update_QS_QI_QG(float Kchb, float Kbank, float Epch, float Bnk0, float Chs0, float Abnk, float Bbnk, float SubArea,float Vdiv, float Vpoint,float PETch, float GWSsub,float Vseep0);

	float* getRchParam();

	int getId();
	int getOrder();
	int getRecRchID();

	float getQS();
	float getQI();
	float getQG();
	float getQin();
	
	float getVout();
	float getVinflow();
	float getContArea();
	float getRchLen();

	// output
	float getVin();
	float getVside();
	float getVdiv();
	float getVpoint();
	float getVseep();
	float getVnb();
	float getEch();
	float getLbank();
	float getVbank();

	// the upstream IDs of the reach
	vector<reach*> m_Inlet;  

	void Route(float* iuhriver);

	bool operator < (const reach & R)const;
};

