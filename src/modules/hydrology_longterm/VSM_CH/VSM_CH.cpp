/*----------------------------------------------------------------------
*	Purpose: 	Overland routing using 4-point implicit finite difference method
*
*	Created:	Junzhi Liu
*	Date:		23-Febrary-2011
*
*	Revision:
*   Date:
*---------------------------------------------------------------------*/
//#include "vld.h"
#include "VSM_CH.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>
#include <cmath>
#include <iostream>
#include <set>
#include <sstream>
#include <algorithm> 

#include <omp.h>

#define MINI_SLOPE 0.0001f
#define NODATA_VALUE -99
using namespace std;

VSM_CH::VSM_CH(void): m_dt(-1), m_nreach(-1), m_Kchb(NODATA_VALUE), 
	m_Kbank(NODATA_VALUE), m_Epch(NODATA_VALUE),m_Bnk0(NODATA_VALUE), m_Chs0(NODATA_VALUE), m_aBank(NODATA_VALUE), m_bBank(NODATA_VALUE), m_subbasin(NULL), m_qsSub(NULL),  
	m_qiSub(NULL), m_qgSub(NULL),m_PETch(NULL),m_gwStorage(NULL), m_area(NULL), m_Vseep0(0.f),
	m_Vdiv(NULL), m_Vpoint(NULL), m_bankStorage(NULL), m_seepage(NULL), m_chOrder(NULL), m_qsCh(NULL), m_qiCh(NULL), m_qgCh(NULL),
	m_widthcurrent(NULL), m_depthcurrent(NULL), m_slopecurrent(NULL),m_widthbottom(NULL),m_sideslopeMain(1.0f),
	m_sideslopeFloodplain(0.25),m_w_ratio(1.0f),m_bankfullQ(5.f),m_CrAreaCh(NULL), m_erodibilityFactor(0.3)
{
	this->m_Q_RECH = NULL;
	this->m_C_WABA = NULL;
	m_qOut = NULL;
	m_chStorage = NULL;
}


VSM_CH::~VSM_CH(void)
{
	if (m_Q_RECH != NULL)
	{
		for(int i=0; i<m_nreach; i++)
		{
			if(m_Q_RECH[i] != NULL) delete [] m_Q_RECH[i];
		}
		delete [] m_Q_RECH;
	}
	if (m_C_WABA != NULL)
	{
		for(int i=0; i<m_nreach; i++)
		{
			if(m_C_WABA[i] != NULL) delete [] m_C_WABA[i];
		}
		delete [] m_C_WABA;
	}

	if(m_area != NULL) delete [] m_area;
	if(m_chStorage != NULL) delete [] m_chStorage;
	if(m_qOut != NULL) delete [] m_qOut;
	if (m_bankStorage != NULL)
		delete[] m_bankStorage;
	if (m_seepage != NULL)
		delete[] m_seepage;
	if (m_chStorage != NULL)
		delete[] m_chStorage;
	if (m_qsCh != NULL)
		delete[] m_qsCh;
	if (m_qiCh != NULL)
		delete[] m_qiCh;
	if (m_qgCh != NULL)
		delete[] m_qgCh;
	if(m_widthcurrent != NULL) delete [] m_widthcurrent;
	if(m_depthcurrent != NULL) delete [] m_depthcurrent;
	if(m_slopecurrent != NULL) delete [] m_slopecurrent;
	if(m_widthbottom != NULL) delete [] m_widthbottom;
	if(m_CrAreaCh != NULL) delete [] m_CrAreaCh;
}

bool VSM_CH::CheckInputData(void)
{
	if (m_dt < 0)
	{
		throw ModelException("VSM_CH","CheckInputData","The parameter: m_dt has not been set.");
	}

	if (m_nreach < 0)
	{
		throw ModelException("VSM_CH","CheckInputData","The parameter: m_nreach has not been set.");
	}

	if (FloatEqual(m_Kchb,NODATA_VALUE))
	{
		throw ModelException("VSM_CH","CheckInputData","The parameter: K_chb has not been set.");
	}
	if (FloatEqual(m_Kbank,NODATA_VALUE))
	{
		throw ModelException("VSM_CH","CheckInputData","The parameter: K_bank has not been set.");
	}
	if (FloatEqual(m_Epch,NODATA_VALUE))
	{
		throw ModelException("VSM_CH","CheckInputData","The parameter: Ep_ch has not been set.");
	}
	if (FloatEqual(m_Bnk0,NODATA_VALUE))
	{
		throw ModelException("VSM_CH","CheckInputData","The parameter: Bnk0 has not been set.");
	}
	if (FloatEqual(m_Chs0,NODATA_VALUE))
	{
		throw ModelException("VSM_CH","CheckInputData","The parameter: Chs0 has not been set.");
	}
	if (FloatEqual(m_aBank,NODATA_VALUE))
	{
		throw ModelException("VSM_CH","CheckInputData","The parameter: A_bnk has not been set.");
	}
	if (FloatEqual(m_bBank,NODATA_VALUE))
	{
		throw ModelException("VSM_CH","CheckInputData","The parameter: B_bnk has not been set.");
	}
	if (FloatEqual(m_Vseep0,NODATA_VALUE))
	{
		throw ModelException("VSM_CH","CheckInputData","The parameter: m_Vseep0 has not been set.");
	}
	if (m_subbasin == NULL)
	{
		throw ModelException("VSM_CH","CheckInputData","The parameter: m_subbasin has not been set.");
	}
	if (m_qsSub == NULL)
	{
		throw ModelException("VSM_CH","CheckInputData","The parameter: Q_SBOF has not been set.");
	}
	/*if (m_qiSub == NULL)
	{
	throw ModelException("VSM_CH","CheckInputData","The parameter: Q_SBIF has not been set.");
	}
	if (m_qgSub == NULL)
	{
	throw ModelException("VSM_CH","CheckInputData","The parameter: QG_sub has not been set.");
	}
	if (m_PETch == NULL)
	{
	throw ModelException("VSM_CH","CheckInputData","The parameter: SBPET has not been set.");
	}	
	if (m_gwStorage == NULL)
	{
	throw ModelException("VSM_CH","CheckInputData","The parameter: GW_sub has not been set.");
	}	*/

	if (m_chWidth == NULL)
	{
		throw ModelException("VSM_CH","CheckInputData","The parameter: RchParam has not been set.");
	}
	return true;
}

void  VSM_CH::initalOutputs()
{
	if(this->m_nreach <= 0) 
		throw ModelException("VSM_CH","initalOutputs","The cell number of the input can not be less than zero.");

	if (m_reachLayers.empty())
	{
		for (int i = 0; i < m_nreach; i++)
		{
			int order = (int)m_chOrder[i];
			m_reachLayers[order].push_back(m_reachId[i]);
		}
	}

	//initial channel storage
	if (m_chStorage == NULL)
	{
		m_chStorage = new float[m_nreach+1];
		m_qOut = new float[m_nreach+1];
		m_bankStorage = new float[m_nreach+1];
		m_seepage = new float[m_nreach+1];
		m_qsCh = new float[m_nreach+1];
		m_qiCh = new float[m_nreach+1];
		m_qgCh = new float[m_nreach+1];

		m_widthcurrent = new float[m_nreach+1];
		m_depthcurrent = new float[m_nreach+1];
		m_slopecurrent = new float[m_nreach+1];
		m_widthbottom = new float[m_nreach+1];
		m_CrAreaCh = new float[m_nreach+1];

		m_Q_RECH = new float*[m_nreach+1];
		m_C_WABA = new float*[m_nreach+1];

		//#pragma omp parallel for
		for (int i=1; i<=m_nreach; i++)
		{
			m_widthcurrent[i] = m_chWidth[m_idToIndex[i]];  //Mark: current vars should not be in here
			m_depthcurrent[i] = m_chDepth[m_idToIndex[i]];
			m_slopecurrent[i] = m_chSlope[m_idToIndex[i]];
			m_widthbottom[i] = m_chWidth[m_idToIndex[i]] - 2*m_sideslopeMain*m_chDepth[m_idToIndex[i]];
			m_CrAreaCh[i] = 0.0f;
			
			m_seepage[i] = 0.f;
			m_bankStorage[i] = m_Bnk0 * m_chLen[m_idToIndex[i]];
			m_chStorage[i] = m_Chs0 * m_chLen[m_idToIndex[i]];
			m_qOut[i] = m_qsSub[i];
			m_qsCh[i] = m_qsSub[i];
			m_qiCh[i] = 0.0f;
			m_qgCh[i] = 0.0f;
			if(m_qiSub !=NULL && m_qgSub != NULL )
			{
				m_qOut[i] += m_qiSub[i] + m_qgSub[i];
				m_qiCh[i] = m_qiSub[i];
				m_qgCh[i] = m_qgSub[i];
			}

			m_Q_RECH[i] = new float[5];
			for (int j=0; j<5; j++)
			{
				m_Q_RECH[i][j] = 0.0f;
			}

			m_C_WABA[i] = new float[FLOW_CHANNEL_WB_COLUMN_COUNT];
			for (int j=0; j<11; j++)
			{
				m_C_WABA[i][j] = 0.0f;
			}
		}
	}
}

int VSM_CH::Execute()
{
	//check the data
	CheckInputData();	
	initalOutputs();
	
	map<int, vector<int> >::iterator it;
	for (it = m_reachLayers.begin(); it != m_reachLayers.end(); it++)
	{
		// There are not any flow relationship within each routing layer.
		// So parallelization can be done here.
		int nReaches = it->second.size();
		// the size of m_reachLayers (map) is equal to the maximum stream order
		//#pragma omp parallel for
		for (int i = 0; i < nReaches; ++i)
		{
			int reachId = it->second[i]; // 
			ChannelFlow(reachId);
		}
	}

	return 0;
}

bool VSM_CH::CheckInputSize(const char* key, int n)
{
	if(n <= 0)
	{
		//this->StatusMsg("Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(this->m_nreach != n)
	{
		if(this->m_nreach <=0) this->m_nreach = n;
		else
		{
			//this->StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
			ostringstream oss;
			oss << "Input data for "+string(key) << " is invalid with size: " << n << ". The origin size is " << m_nreach << ".\n";  
			throw ModelException("VSM_CH","CheckInputSize",oss.str());
		}
	}

	return true;
}

//bool VSM_CH::CheckInputSizeChannel(const char* key, int n)
//{
//	if(n <= 0)
//	{
//		//this->StatusMsg("Input data for "+string(key) +" is invalid. The size could not be less than zero.");
//		return false;
//	}
//	if(this->m_chNumber != n)
//	{
//		if(this->m_chNumber <=0) this->m_chNumber = n;
//		else
//		{
//			//this->StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
//			return false;
//		}
//	}
//
//	return true;
//}

//void VSM_CH::GetValue(const char* key, float* value)
//{
//	string sk(key);
//	if (StringMatch(sk, "QOUTLET"))
//	{
//		map<int, vector<int> >::iterator it = m_reachLayers.end();
//		it--;
//		int reachId = it->second[0];
//		int iLastCell = m_reachs[reachId].size() - 1;
//		*value = m_qCh[reachId][iLastCell];
//		//*value = m_hToChannel[m_idOutlet];
//		//*value = m_qsSub[m_idOutlet];
//		//*value = m_qsSub[m_idOutlet] + m_qCh[reachId][iLastCell];
//	}
//}

void VSM_CH::SetValue(const char* key, float value)
{
	string sk(key);

	if (StringMatch(sk, "ThreadNum"))
	{
		omp_set_num_threads((int)value);
	}
	else if (StringMatch(sk, "TimeStep"))
	{
		m_dt = (int) value;
	}
	else if (StringMatch(sk, "K_chb"))
	{
		m_Kchb = value;
	}
	else if (StringMatch(sk, "K_bank"))
	{
		m_Kbank = value;
	}
	else if (StringMatch(sk, "Ep_ch"))
	{
		m_Epch = value;
	}
	else if (StringMatch(sk, "Bnk0"))
	{
		m_Bnk0 = value;
	}
	else if (StringMatch(sk, "Chs0"))
	{
		m_Chs0 = value;
	}
	else if (StringMatch(sk, "Vseep0"))
	{
		m_Vseep0 = value;
	}
	else if (StringMatch(sk, "a_bnk"))
	{
		m_aBank = value;
	}
	else if (StringMatch(sk, "b_bnk"))
	{
		m_bBank = value;
	}
	else
		throw ModelException("VSM_CH", "SetSingleData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");

}

void VSM_CH::GetValue(const char* key, float* value)
{
	string sk(key);
	int iOutlet = m_reachLayers.rbegin()->second[0];
	if (StringMatch(sk, "QOUTLET"))
	{
		*value = m_qOut[iOutlet];
		//cout << m_qsSub[iOutlet]<<endl;   //m_qsSub  m_chStorage
	}

}

void VSM_CH::Set1DData(const char* key, int n, float* value)
{
	string sk(key);
	//check the input data
	if(StringMatch(sk,"subbasin"))
	{
		m_subbasin = value;   //m_subbasin
	}
	else if(StringMatch(sk,"D_SBOF"))
	{
		CheckInputSize(key,n);
		m_qsSub = value;   
	}
	else if (StringMatch(sk,"D_SBIF"))
	{
		CheckInputSize(key,n);
		m_qiSub = value;
	}
	else if (StringMatch(sk, "D_SBQG"))
	{
		m_qgSub = value;
	}
	else if (StringMatch(sk, "D_SBPET"))
	{
		m_PETch = value;
	}
	else if (StringMatch(sk, "D_SBGS"))
	{
		m_gwStorage = value;
	}
	else if (StringMatch(sk, "Vpoint"))
	{
		m_Vpoint = value;
	}
	else
		throw ModelException("VSM_CH", "Set1DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
	
}

void VSM_CH::Get1DData(const char* key, int* n, float** data)
{
	string sk(key);
	*n = m_nreach+1;
	int iOutlet = m_reachLayers.rbegin()->second[0];
	if (StringMatch(sk, "QRECH"))
	{
		m_qOut[0] = m_qOut[iOutlet];
		*data = m_qOut;
	}
	else if (StringMatch(sk, "QS"))
	{
		m_qsCh[0] = m_qsCh[iOutlet];
		*data = m_qsCh;
	}
	else if (StringMatch(sk, "QI"))
	{
		m_qiCh[0] = m_qiCh[iOutlet];
		*data = m_qiCh;
	}
	else if (StringMatch(sk, "QG"))
	{
		m_qgCh[0] = m_qgCh[iOutlet];
		*data = m_qgCh;
	}
	else if (StringMatch(sk, "BKST"))
	{
		m_bankStorage[0] = m_bankStorage[iOutlet];
		*data = m_bankStorage;
	}
	else if (StringMatch(sk, "CHST"))
	{
		m_chStorage[0] = m_chStorage[iOutlet];
		*data = m_chStorage;
	}
	else if (StringMatch(sk, "SEEPAGE"))
	{
		m_seepage[0] = m_seepage[iOutlet];
		*data = m_seepage;
	}
	else if (StringMatch(sk, "CROSS_AREA"))
	{
		*data = m_CrAreaCh;
	}
	else
		throw ModelException("VSM_CH", "Get1DData", "Output " + sk 
		+ " does not exist in the VSM_CH module. Please contact the module developer.");

}

void VSM_CH::Get2DData(const char* key, int *nRows, int *nCols, float*** data)
{
	string sk(key);
	if (StringMatch(sk,"Q_RECH"))
	{
		*data = this->m_Q_RECH;
		*nRows = this->m_nreach;
		*nCols = 5;
	}
	else if (StringMatch(sk,"C_WABA"))
	{
		*data = this->m_C_WABA;
		*nRows = this->m_nreach;
		*nCols = 11;
	}
	else
		throw ModelException("VSM_CH", "Get2DData", "Output " + sk 
		+ " does not exist in the VSM_CH module. Please contact the module developer.");

}

void VSM_CH::Set2DData(const char* key, int nrows, int ncols, float** data)
{
	string sk(key);

	//if (StringMatch(sk, "RchParam"))
	if (StringMatch(sk, "ReachParameter"))
	{
		m_nreach = ncols;

		/*m_reachId = data[0];
		m_reachDownStream = data[1];
		m_chOrder = data[2];
		m_chWidth = data[3];
		m_chLen = data[4];
		m_chDepth = data[5];
		m_chVel = data[6];
		m_area = data[7];
		m_chManning = data[8];
		m_chSlope = data[9];*/
		m_reachId = data[0];
		m_reachDownStream = data[1];
		m_chOrder = data[2];
		m_chWidth = data[4];  
		m_chLen = data[5];    
		m_chDepth = data[6];   
		m_chVel = data[7];  
		m_area = data[8];
		m_chManning = data[3];
		m_chSlope = data[9];

		/*m_reachUpStream.resize(m_nreach+1);
		for (int i = 1; i <= m_nreach; i++)
		{
			int downStreamId = int(m_reachDownStream[i]);
			if(downStreamId <= 0)
				continue;
			m_reachUpStream[downStreamId].push_back(i);
		}*/
		for (int i = 0; i < m_nreach; i++)
			m_idToIndex[(int)m_reachId[i]] = i;

		m_reachUpStream.resize(m_nreach);
		for (int i = 0; i < m_nreach; i++)
		{
			int downStreamId = int(m_reachDownStream[i]);
			if(downStreamId == 0)
				continue;
			if (m_idToIndex.find(downStreamId) != m_idToIndex.end())
			{
				int downStreamIndex = m_idToIndex.at(downStreamId);
				m_reachUpStream[downStreamIndex].push_back(m_reachId[i]);
			}
		}
	}
	else
		throw ModelException("VSM_CH", "Set2DData", "Parameter " + sk 
			+ " does not exist. Please contact the module developer.");
	
}

float VSM_CH::Q(float waterDepth, int id)
{
	if(waterDepth < DEPTH_INTERVAL) return 0.0f;

	float area = crossSectionalArea(waterDepth, id);
	m_CrAreaCh[id] = area;
	float wp = wettedPerimeter(waterDepth, id);
	float radius = area / wp;
	return area * 
		pow(radius,0.6666f) * 
		sqrt(m_slopecurrent[id]) / m_chManning[m_idToIndex[id]];
}

float VSM_CH::crossSectionalArea(float waterDepth, int id)
{
	if(waterDepth < DEPTH_INTERVAL) return 0.0f;

	if(m_depthcurrent[id] < waterDepth)	//flood plain
	{
		float depth_flood = waterDepth - m_depthcurrent[id];
		//if(depth_flood > 9.0f * m_depthcurrent) //max flood depth
		//	throw ModelException("reachParameters","crossSectionalArea",
		//	"Two many water in reach!");	

		float area = (m_widthbottom[id] + m_depthcurrent[id] * this->m_sideslopeMain) * m_depthcurrent[id];
		area += (m_widthcurrent[id] * m_w_ratio + depth_flood * m_sideslopeMain) * depth_flood;
		return area;
	}
	else							//not bankfull
	{
		return (m_widthbottom[id] + waterDepth * m_sideslopeMain) * waterDepth;	
	}
}

float VSM_CH::wettedPerimeter(float waterDepth, int id)
{
	if(waterDepth < DEPTH_INTERVAL) return 0.0f;

	if(m_depthcurrent[id] < waterDepth)	//flood plain
	{
		float depth_flood = waterDepth - m_depthcurrent[id];
		//if(depth_flood > 9.0f * m_depthcurrent) //max flood depth
		//	throw ModelException("reachParameters","crossSectionalArea",
		//	"Two many water in reach!");	

		float wettedPerimeter = m_widthbottom[id] + 
			2.0f * m_depthcurrent[id] * sqrt(1+pow(m_sideslopeMain,2.0f));		
		wettedPerimeter += 4 * m_widthcurrent[id] + 
			2.0f * depth_flood * sqrt(1+pow(m_sideslopeFloodplain,2.0f));
		return wettedPerimeter;
	}
	else							//not bankfull
	{
		return m_widthbottom[id] + 
			2 * waterDepth * sqrt(1+pow(m_sideslopeMain,2.0f));		
	}
}


float VSM_CH::depth(float Qout, int id)
{
	float depth = Qout > this->m_bankfullQ ? this->m_depthcurrent[id] : 0.0f;
	float tempQ = 0.0f;
	while(tempQ < Qout)
	{
		depth += DEPTH_INTERVAL;		
		//if(depth > this->depth_current() * 10.0f)  //max depth
		//	throw ModelException("reachParameters","depth",
		//	"Two many water in reach!");	

		tempQ = this->Q(depth, id);		
	}

	return depth;
}

//float VSM_CH::fSimple(float area1,float Qin,float depth, int id)
//{
//	float area2 = m_CrAreaCh[id];
//	float Qout = Q(depth, id);
//	float left = area2 - area1;
//	left *=  m_chLen[id]/(m_dt * 3600);
//
//	float right = Qin - Qout;
//
//	return left - right;
//}

//float VSM_CH::calOutletDischarge(float qs, float qi, float qg, int id)
//{
//	float Qin = qs + qi + qg;
//	if(Qin * m_dt * 3600 < 1) return 0.0f;
//	float area1 = m_C_WABA[SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_STORAGE]/m_chLen[id];
//	float depth = DEPTH_INTERVAL;
//	float f1 = fSimple(area1,Qin,depth);
//	if(f1 > 0) return 0.0f;
//	float f2 = -99.0f;
//	do
//	{
//		if(f2 != -99.0f) f1 = f2;
//
//		depth += DEPTH_INTERVAL;
//		f2 = fSimple(area1,Qin,depth);
//	}
//	while(f1*f2>0);
//
//	return this->Q(depth);
//}

void VSM_CH::ChannelFlow(int i)
{
	// calculate storage coefficient before change channel storage and qout
	float sc = 1.f;
	if (!FloatEqual(m_qOut[i], 0.f))
	{
		float TT = m_chStorage[i]/m_qOut[i];
		sc = 2*m_dt / (2*TT + m_dt); 
	}
	if (sc > 1.f)
		sc = 1.f;

	m_chStorage[i] -= m_qOut[i]*m_dt;
	if (m_chStorage[i] < 0.f)
		m_chStorage[i] = 0.f;

	//////////////////////////////////////////////////////////////////////////
	// first add all the inflow water
	// 1. water from this subbasin
	float qIn = m_qsSub[i];// + m_qiSub[i] + m_qgSub[i];  //m_qsSub[i] most time step is 0

	// 2. water from upstream reaches
	float qsUp = 0.f;
	float qiUp = 0.f;
	float qgUp = 0.f;
	for (size_t j = 0; j < m_reachUpStream[m_idToIndex[i]].size(); ++j)
	{
		int upReachId = m_reachUpStream[m_idToIndex[i]][j];
		qsUp += m_qsCh[upReachId];
		/*qiUp += m_qiCh[upReachId];
		qgUp += m_qgCh[upReachId];*/
	}
	qIn += qsUp ;//+ qiUp + qgUp;

	// 3. water from bank storage
	float bankOut = m_bankStorage[i] * (1 - exp(-m_aBank));
	m_bankStorage[i] -= bankOut;
	qIn += bankOut/m_dt;
	
	// add inflow water to storage
	m_chStorage[i] += qIn*m_dt;
	if (FloatEqual(m_chStorage[i],0.0f) )
	{
		m_qOut[i] = 0.f; // in case the channel storage turns into zero and return
		m_qsCh[i] = 0.f;
		m_qiCh[i] = 0.f;
		m_qgCh[i] = 0.f;
		return;
	}
	

	//////////////////////////////////////////////////////////////////////////
	// then subtract all the outflow water
	// 1. transmission losses to deep aquifer, which is lost from the system
	// the unit of kchb is mm/hr
	float seepage = m_Kchb/1000.f /3600.f * m_chWidth[m_idToIndex[i]] * m_chLen[m_idToIndex[i]] * m_dt;
	/*if(m_qgSub[i] < 0.001f)
	{
	if (m_chStorage[i] > seepage)
	{
	m_seepage[i] = seepage;
	m_chStorage[i] -= seepage;
	}
	else
	{
	m_seepage[i] = m_chStorage[i];
	m_chStorage[i] = 0.f;
	return;
	}
	}*/

	// 2. calculate transmission losses to bank storage
	//float Dch = qIn / m_chVel[i] / m_chWidth[i];			//m
	//Dch = min(Dch, m_chDepth[i]);                           //Dch <= RchDep
	float dch = m_chStorage[i]/(m_chWidth[m_idToIndex[i]]*m_chLen[m_idToIndex[i]]);
	float bankIn = 2 * m_Kbank/1000.f/3600.f * dch * m_chLen[m_idToIndex[i]] * m_dt ;   // m3/s
	if (m_chStorage[i] > bankIn)
	{
		m_chStorage[i] -= bankIn;
	}
	else
	{
		bankIn = m_chStorage[i];
		m_chStorage[i] = 0.f;
	}
	// water balance of the bank storage
	// loss the water from bank storage to the adjacent unsaturated zone and groundwater storage
	float bankOutGw = m_bankStorage[i] * (1 - exp(-m_bBank));
	m_bankStorage[i] = m_bankStorage[i] + bankIn - bankOutGw;
	// update groundwater storage
	//m_gwStorage[i] += bankOutGw/m_area[m_idToIndex[i]] * 1000.f;   // updated groundwater storage

	/*if (FloatEqual(m_chStorage[i], 0.f))
		return;*/

	// 3. evaporation losses
	//float et = m_Epch * m_PETch[i]/1000.0f *m_chWidth[m_idToIndex[id]] * m_chLen[m_idToIndex[id]];    //m3
	//if (m_chStorage[i] > et)
	//{
	//	m_chStorage[i] -= et;
	//}
	//else
	//{
	//	et = m_chStorage[i];
	//	m_chStorage[i] = 0.f;
	//}

	//////////////////////////////////////////////////////////////////////////
	// routing
	m_qOut[i] = sc * (m_chStorage[i]/m_dt);
	/*float qInSum = m_qsSub[i] + m_qiSub[i] + m_qgSub[i] + qsUp + qiUp + qgUp;
	m_qsCh[i] = m_qOut[i] * (m_qsSub[i] + qsUp) / qInSum;
	m_qiCh[i] = m_qOut[i] * (m_qiSub[i] + qiUp) / qInSum;
	m_qgCh[i] = m_qOut[i] * (m_qgSub[i] + qgUp) / qInSum;*/
	float qInSum = m_qsSub[i] + qsUp + qiUp + qgUp;
	if(m_qiSub != NULL && m_qgSub !=NULL)
		qInSum += m_qiSub[i] + m_qgSub[i];

	m_qsCh[i] = m_qOut[i] * (m_qsSub[i] + qsUp) / qInSum;
	m_qiCh[i] = 0.0f;
	m_qgCh[i] = 0.0f;
	if(m_qiSub != NULL && m_qgSub !=NULL)
	{
		m_qiCh[i] = m_qOut[i] * (m_qiSub[i] + qiUp) / qInSum;
		m_qgCh[i] = m_qOut[i] * (m_qgSub[i] + qgUp) / qInSum;
	}
	//////////////////////////////////////////////////////////////////////////
	// calculate channel water depth

	//float totalDischarge = calOutletDischarge(m_qsCh[i],m_qiCh[i],m_qgCh[i],i);
	float Discharge = m_qOut[i];
	float waterdepth = this->depth(Discharge, i);

	//set waterbalance
	m_C_WABA[i][FLOW_CHANNEL_WB_COLUMN_INDEX_QOVERLAND] = m_qsCh[i];
	m_C_WABA[i][FLOW_CHANNEL_WB_COLUMN_INDEX_QINTERL]	= m_qiCh[i];
	m_C_WABA[i][FLOW_CHANNEL_WB_COLUMN_INDEX_QGROUND]	= m_qgCh[i];
	m_C_WABA[i][FLOW_CHANNEL_WB_COLUMN_INDEX_CHSTORAGE]	= m_chStorage[i];
	m_C_WABA[i][FLOW_CHANNEL_WB_COLUMN_INDEX_BKSTORAGE]	= m_bankStorage[i];
	m_C_WABA[i][FLOW_CHANNEL_WB_COLUMN_INDEX_SEEPAGE]	= m_seepage[i];
	m_C_WABA[i][FLOW_CHANNEL_WB_COLUMN_INDEX_WTDEPTH]	= waterdepth;
}