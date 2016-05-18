/*----------------------------------------------------------------------
*	Purpose: 	Channel sediment routing
*
*	Created:	Wu Hui
*	Date:		21-Dec.-2012
*
*	Revision:
*   Date:
*---------------------------------------------------------------------*/
//#include "vld.h"
#include "SEDR_VCD.h"
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

//#define MINI_SLOPE 0.0001f
//#define NODATA_VALUE -99

using namespace std;

SEDR_VCD::SEDR_VCD(void): m_dt(-1), m_nreach(-1), m_sedtoCh(NULL),m_Chs0(NODATA), m_widthbottom(NULL), 
	 m_Vdiv(NULL), m_Vpoint(NULL), m_widthcurrent(NULL), m_depthcurrent(NULL), m_slopecurrent(NULL),
	m_chOrder(NULL),m_qchOut(NULL), m_sideslopeMain(1.0f), m_sideslopeFloodplain(1.0f),m_w_ratio(1.0f),m_bankfullQ(5.f),
	m_prf(NODATA),m_spcon(NODATA),m_spexp(NODATA),m_vcrit(NODATA),m_coverFactor(0.1),m_erodibilityFactor(0.2) //0.1 for Lyg
{
	//this->m_T_CHSB = NULL;
	m_SedOut = NULL;
	m_qsSub = NULL;
	m_qiSub = NULL;
	m_qgSub = NULL;
	m_chStorage = NULL;
	m_chWTdepth = NULL;
	//output
	m_sedStorage = NULL;
	m_sedSub = NULL;
	m_sedUps = NULL;
	m_sedDep = NULL;
	m_sedDeg = NULL;
}


SEDR_VCD::~SEDR_VCD(void)
{
	/*if (m_T_CHSB != NULL)
	{
		for(int i=0; i<m_nreach; i++)
		{
			if(m_T_CHSB[i] != NULL) delete [] m_T_CHSB[i];
		}
		delete [] m_T_CHSB;
	}*/

	//if(m_area != NULL) delete [] m_area;
	//if(m_chStorage != NULL) delete [] m_chStorage;
	if(m_SedOut != NULL) delete [] m_SedOut;
	if(m_widthcurrent != NULL) delete [] m_widthcurrent;
	if(m_depthcurrent != NULL) delete [] m_depthcurrent;
	if(m_slopecurrent != NULL) delete [] m_slopecurrent;
	if(m_widthbottom != NULL) delete [] m_widthbottom;
	if(m_sedStorage != NULL) delete [] m_sedStorage;
	if(m_sedSub != NULL) delete [] m_sedSub;
	if(m_sedUps != NULL) delete [] m_sedUps;
	if(m_sedDeg != NULL) delete [] m_sedDeg;
	if(m_sedDep != NULL) delete [] m_sedDep;
	//if(m_CrAreaCh != NULL) delete [] m_CrAreaCh;
}

bool SEDR_VCD::CheckInputData(void)
{
	if (m_dt < 0)
	{
		throw ModelException("SEDR_VCD","CheckInputData","The parameter: m_dt has not been set.");
	}

	if (m_nreach < 0)
	{
		throw ModelException("SEDR_VCD","CheckInputData","The parameter: m_nreach has not been set.");
	}

	if (FloatEqual(m_Chs0,NODATA))
	{
		throw ModelException("SEDR_VCD","CheckInputData","The parameter: Chs0 has not been set.");
	}
	if (FloatEqual(m_prf,NODATA))
	{
		throw ModelException("SEDR_VCD","CheckInputData","The parameter: m_prf has not been set.");
	}
	if (FloatEqual(m_spcon,NODATA))
	{
		throw ModelException("SEDR_VCD","CheckInputData","The parameter: m_spcon has not been set.");
	}
	if (FloatEqual(m_spexp,NODATA))
	{
		throw ModelException("SEDR_VCD","CheckInputData","The parameter: m_spexp has not been set.");
	}
	if (FloatEqual(m_vcrit,NODATA))
	{
		throw ModelException("SEDR_VCD","CheckInputData","The parameter: m_vcrit has not been set.");
	}
	if (m_sedtoCh == NULL)
	{
		throw ModelException("SEDR_VCD","CheckInputData","The parameter: m_sedtoCh has not been set.");
	}
	if (m_chWidth == NULL)
	{
		throw ModelException("SEDR_VCD","CheckInputData","The parameter: ReachParameter has not been set.");
	}
	/*if (m_CrAreaCh == NULL)
	{
		throw ModelException("SEDR_VCD","CheckInputData","The parameter: m_CrAreaCh has not been set.");
	}*/
	if (m_qgSub == NULL)
	{
		throw ModelException("SEDR_VCD","CheckInputData","The parameter: m_qgSub has not been set.");
	}
	if (m_qiSub == NULL)
	{
		throw ModelException("SEDR_VCD","CheckInputData","The parameter: m_qiSub has not been set.");
	}
	if (m_qgSub == NULL)
	{
		throw ModelException("SEDR_VCD","CheckInputData","The parameter: m_qgSub has not been set.");
	}
	if (m_chStorage == NULL)
	{
		throw ModelException("SEDR_VCD","CheckInputData","The parameter: m_chStorage has not been set.");
	}
	if (m_chWTdepth == NULL)
	{
		throw ModelException("SEDR_VCD","CheckInputData","The parameter: m_chWTdepth has not been set.");
	}
	if (m_qchOut == NULL)
	{
		throw ModelException("SEDR_VCD","CheckInputData","The parameter: m_qchOut has not been set.");
	}
	return true;
}

void  SEDR_VCD::initalOutputs()
{
	
	if(m_nreach <= 0) 
		throw ModelException("SEDR_VCD","initalOutputs","The cell number of the input can not be less than zero.");

	if (m_reachLayers.empty())
	{
		CheckInputData();
		for (int i = 1; i <= m_nreach; i++)
		{
			int order = (int)m_chOrder[i];
			m_reachLayers[order].push_back(i);
			//m_reachLayers[order].push_back(m_reachId[i]);
		}
	}

	//initial channel storage
	if (m_SedOut == NULL)
	{
		//m_chStorage = new float[m_nreach+1];
		m_SedOut = new float[m_nreach+1];
		m_widthcurrent = new float[m_nreach+1];
		m_depthcurrent = new float[m_nreach+1];
		m_slopecurrent = new float[m_nreach+1];
		m_widthbottom = new float[m_nreach+1];
		m_sedStorage = new float[m_nreach+1];
		m_sedUps = new float[m_nreach+1];
		m_sedSub = new float[m_nreach+1];
		m_sedDep = new float[m_nreach+1];
		m_sedDeg = new float[m_nreach+1];
		//m_T_CHSB = new float*[m_nreach+1];

		#pragma omp parallel for
		for (int i=1; i<=m_nreach; i++)
		{
			m_widthcurrent[i] = m_chWidth[i];
			m_depthcurrent[i] = m_chDepth[i];
			m_slopecurrent[i] = m_chSlope[i];
			m_widthbottom[i] = m_chWidth[i] - 2*m_sideslopeMain*m_chDepth[i];
			//m_CrAreaCh[i] = 0.0f;
			//m_chStorage[i] = m_Chs0 * m_chLen[i];
			m_SedOut[i] = 0.0f;
			m_sedStorage[i] = m_Chs0 * m_chLen[i];
			m_sedSub[i] = 0.0f;
			m_sedDep[i] = 0.0f;
			m_sedDeg[i] = 0.0f;
			m_sedUps[i] = 0.0f;

			m_qchOut[i] = m_qgSub[i] + m_qiSub[i] + m_qgSub[i];
			/*m_T_CHSB[i] = new float[SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_COUNT];
			for (int j=0; j<SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_COUNT; j++)
			{
				m_T_CHSB[i][j] = 0.0f;
			}*/
		}
	}
}

int SEDR_VCD::Execute()
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
		#pragma omp parallel for
		for (int i = 0; i < nReaches; ++i)
		{
			int reachIndex = it->second[i]; // index in the array

			SedChannelRouting(reachIndex);
		}
	}

	return 0;
}

bool SEDR_VCD::CheckInputSize(const char* key, int n)
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
			throw ModelException("SEDR_VCD","CheckInputSize",oss.str());
		}
	}

	return true;
}

void SEDR_VCD::GetValue(const char* key, float* value)
{
	string sk(key);
	int iOutlet = m_reachLayers.rbegin()->second[0];
	if (StringMatch(sk, "SEDOUTLET"))
	{
		*value = m_SedOut[iOutlet];
	}

}

void SEDR_VCD::SetValue(const char* key, float value)
{
	string sk(key);

	if (StringMatch(sk, VAR_OMP_THREADNUM))
	{
		omp_set_num_threads((int)value);
	}
#ifdef STORM_MODEL
	else if (StringMatch(sk, "DT_CH"))
	{
		m_dt = (int) value;
	}
#else
	else if (StringMatch(sk, "TimeStep"))
	{
		m_dt = (int) value;
	}
#endif
	else if (StringMatch(sk, "p_rf"))
	{
		m_prf = value;
	}
	else if (StringMatch(sk, "spcon"))
	{
		m_spcon = value;
	}
	else if (StringMatch(sk, "spexp"))
	{
		m_spexp = value;
	}
	else if (StringMatch(sk, "vcrit"))
	{
		m_vcrit = value;
	}
	else if (StringMatch(sk, "Chs0"))
	{
		m_Chs0 = value;
	}
	else
		throw ModelException("SEDR_VCD", "SetSingleData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");

}

void SEDR_VCD::Set1DData(const char* key, int n, float* value)
{
	string sk(key);
	//check the input data
	if(StringMatch(sk,"SEDTOCH"))
	{
		m_sedtoCh = value;   //for longterm model
	}
	else if(StringMatch(sk,"SubSEDTOCH"))
	{
		m_sedtoCh = value;   //for storm model
	}
	else if (StringMatch(sk, "QRECH"))
	{
		m_qchOut = value;
	}
	else if (StringMatch(sk, "QS"))
	{
		m_qsSub = value;
	}
	else if (StringMatch(sk, "QI"))
	{
		m_qiSub = value;
	}
	else if (StringMatch(sk, "QG"))
	{
		m_qgSub = value;
	}
	else if (StringMatch(sk, "CHST"))
	{
		m_chStorage = value;
	}
	else if (StringMatch(sk, "CHWTDEPTH"))
	{
		m_chWTdepth = value;
	}
	/*else if (StringMatch(sk, "CROSS_AREA"))
	{
	m_CrAreaCh = value;
	}*/
	else
		throw ModelException("SEDR_VCD", "Set1DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
	
}

void SEDR_VCD::Get1DData(const char* key, int* n, float** data)
{
	string sk(key);
	*n = m_nreach+1;
	int iOutlet = m_reachLayers.rbegin()->second[0];
	if (StringMatch(sk, "SEDRECH"))
	{
		//m_SedOut[0] = m_SedOut[iOutlet] * 1000/24/3600;    // ton/day coverts to kg/s
 		m_SedOut[0] = m_SedOut[iOutlet];    // ton
		*data = m_SedOut;
	}
	else
		throw ModelException("SEDR_VCD", "Get1DData", "Output " + sk 
		+ " does not exist in the SEDR_VCD module. Please contact the module developer.");

}

void SEDR_VCD::Get2DData(const char* key, int *nRows, int *nCols, float*** data)
{
	string sk(key);
	/*if (StringMatch(sk,"T_CHSB"))
	{
	*data = this->m_T_CHSB;
	*nRows = this->m_nreach;
	*nCols = SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_COUNT;
	}
	else
		throw ModelException("SEDR_VCD", "Get2DData", "Output " + sk 
		+ " does not exist in the SEDR_VCD module. Please contact the module developer.");*/

}

void SEDR_VCD::Set2DData(const char* key, int nrows, int ncols, float** data)
{
	string sk(key);

	if (StringMatch(sk, Tag_RchParam))
	{
		m_nreach = ncols-1;

		m_reachId = data[0];
		m_reachDownStream = data[1];
		m_chOrder = data[2];
		m_chWidth = data[3];
		m_chLen = data[4];
		m_chDepth = data[5];
		m_chVel = data[6];
		//m_area = data[7];
		m_chSlope = data[9];

		m_reachUpStream.resize(m_nreach+1);
		if (m_nreach > 1)
		{
			for (int i = 1; i <= m_nreach; i++)// index of the reach is the equal to streamlink ID(1 to nReaches)
			{
				int downStreamId = int(m_reachDownStream[i]);
				if(downStreamId <= 0)
					continue;
				m_reachUpStream[downStreamId].push_back(i);
			}
		}
	}
	else
		throw ModelException("SEDR_VCD", "Set2DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
	//--------------for phd work
	//if(StringMatch(sk, Tag_ReachParameter))
	//{
	//	m_nreach = ncols;

	//	m_reachId = data[0];
	//	m_chOrder = data[2];
	//	m_reachDownStream = data[1];
	//	//m_chVel = data[4];
	//	//m_area = data[8];
	//	m_chManning = data[3];
	//	
	//	m_chWidth = data[4];
	//	m_chLen = data[5];
	//	m_chDepth = data[6];
	//	m_chSlope = data[9];

	//	for (int i = 0; i < m_nreach; i++)
	//		m_idToIndex[(int)m_reachId[i]] = i;

	//	m_reachUpStream.resize(m_nreach);
	//	for (int i = 0; i < m_nreach; i++)
	//	{
	//		int downStreamId = int(m_reachDownStream[i]);
	//		if(downStreamId == 0)
	//			continue;
	//		if (m_idToIndex.find(downStreamId) != m_idToIndex.end())
	//		{
	//			int downStreamIndex = m_idToIndex.at(downStreamId);
	//			m_reachUpStream[downStreamIndex].push_back(m_reachId[i]);
	//		}
	//	}
	//}
	/*else if (StringMatch(sk,"C_WABA"))
	{
		m_C_WABA = data;
	}*/
}

void SEDR_VCD::reset(int id)
{
	/*for(int j=0;j<SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_COUNT;j++) 
		this->m_T_CHSB[id][j] = 0.0f;
	m_SedOut[id] = m_T_CHSB[id][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_OUT];*/
	m_sedStorage[id] = 0.0f;
	m_sedUps[id] = 0.0f;
	m_sedSub[id] = 0.0f;
	m_sedDeg[id] = 0.0f;
	m_sedDep[id] = 0.0f;
}

float SEDR_VCD::Q(float waterDepth, int id)
{
	if(waterDepth < DEPTH_INTERVAL) return 0.0f;

	float area = crossSectionalArea(waterDepth, id);
	//m_CrAreaCh[id] = area;
	//float area = m_CrAreaCh[id];
	float wp = wettedPerimeter(waterDepth, id);
	float radius = area / wp;
	return area * 
		pow(radius,0.6666f) * 
		sqrt(m_slopecurrent[id]) / m_chManning[m_idToIndex[id]];
}

float SEDR_VCD::crossSectionalArea(float waterDepth, int id)
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

float SEDR_VCD::wettedPerimeter(float waterDepth, int id)
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


float SEDR_VCD::depth(float Q, int id)
{
	float depth = Q > this->m_bankfullQ ? this->m_depthcurrent[id] : 0.0f;
	float tempQ = 0.0f;
	while(tempQ < Q)
	{
		depth += DEPTH_INTERVAL;		
		//if(depth > this->depth_current() * 10.0f)  //max depth
		//	throw ModelException("reachParameters","depth",
		//	"Two many water in reach!");	

		tempQ = this->Q(depth, id);		
	}

	return depth;
}

void SEDR_VCD::SedChannelRouting(int i)
{
	// 1 .whether is no water in channel
	//float storage = m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_STORAGE];
	float sedstorage = m_sedStorage[i];
	if (m_qchOut[i] < 0.0001f)
	{
		reset(i);
		m_sedStorage[i] = sedstorage;
		m_SedOut[i] = 0.f;
	}
	else
	{   // 2. sediment from upstream reaches
		float sedUp = 0;
		for (size_t j = 0; j < m_reachUpStream[i].size(); ++j)
		{
			int upReachId = m_reachUpStream[i][j];
			sedUp += m_SedOut[upReachId];
		}
		float allSediment = sedUp + m_sedtoCh[i] + m_sedStorage[i];
		//cout<<i<<"\t"<<sedUp <<"\t"<<m_sedtoCh[i]<<"\t"<<sedstorage<<endl;
		
		m_sedUps[i] = sedUp;
		m_sedSub[i] = m_sedtoCh[i];
		//get peak channel velocity (m/s)
		float peakVelocity = 0.0f;
		
		float peakFlowRate = m_qchOut[i] * m_prf;
		
		float crossarea = m_chStorage[i] / m_chLen[i];            // SWAT, eq. 7:1.2.3
		peakVelocity = peakFlowRate / crossarea;

		if(peakVelocity > 5) peakVelocity = 5.0f;
		//get tbase
		float tbase = m_chLen[i] / (m_dt * peakVelocity);
		if(tbase > 1) tbase = 1.0f;
		//max concentration
		float maxCon = m_spcon * pow(peakVelocity, m_spexp); 
		//initial concentration,mix sufficiently
		float qsub = m_qsSub[i];
		float qinsub = m_qiSub[i];
		float qgsub = m_qgSub[i];
		float chstorage = m_chStorage[i];
		float qOutV = m_qchOut[i]*m_dt;

		float allWater = chstorage + qOutV;

		if (allWater < 0.001f)
		{
			reset(i);
			m_SedOut[i] = 0.0f;
			return;
		}
		float initCon = allSediment/allWater;
		//cout << i <<" "<<allSediment<<" "<<peakVelocity << " "<<tbase<<" "<<initCon<<" "<<maxCon<<endl;
		//deposition and degradation
		float sedDeposition = 0.0f;
		float sedDegradation = 0.0f;
		float sedDegradation1 = 0.0f;
		float sedDegradation2 = 0.0f;

		sedDeposition = allWater * (initCon - maxCon);
		if(abs(sedDeposition) < 1.0e-6f)			
			sedDeposition = 0.0f;
		if(peakVelocity < m_vcrit) 
			sedDeposition = 0.0f;
		if(sedDeposition < 1.0e-6f)	//degradation
		{
			sedDegradation = -sedDeposition * tbase;
			if(sedDegradation >= m_sedDep[i])
			{
				sedDegradation1 = m_sedDep[i];
				sedDegradation2 = 
					(sedDegradation - sedDegradation1) * m_erodibilityFactor * m_coverFactor;
			}
			else
			{
				sedDegradation1 = sedDegradation;
			}

			sedDeposition = 0.0f;
		}
		else
		{
			sedDeposition = sedDeposition * tbase;
		}

		//update sed deposition
		/*m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_DEPOSITION] += sedDeposition - sedDegradation1;
		if(m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_DEPOSITION] < 1.0e-6f) 
			m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_DEPOSITION] = 0.0f;*/
		m_sedDep[i] += sedDeposition - sedDegradation1;
		if (m_sedDep[i] < 1.0e-6f)
		{
			m_sedDep[i] = 0.0f;
		}
		//update sed degradation
		/*m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_DEGRADATION] += sedDegradation1 + sedDegradation2;
		if(m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_DEGRADATION] < 1.0e-6f) 
			m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_DEGRADATION] = 0.0f;*/
		m_sedDeg[i] += sedDegradation1 + sedDegradation2;
		if (m_sedDeg[i] < 1.0e-6f)
		{
			m_sedDeg[i] = 0.0f;
		}
		//get sediment after deposition and degradation
		allSediment += sedDegradation1 + sedDegradation2 - sedDeposition;
		if(allSediment < 1.0e-6f) allSediment = 0.0f;

		//get out flow water fraction
		float outFraction = qOutV / allWater;
		if(outFraction < 1.0e-6f) outFraction = 0.0f;
		if(outFraction > 1.0f) outFraction = 1.0f;
		//update sed out
		//m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_OUT] = allSediment * outFraction;
		m_SedOut[i] = allSediment * outFraction;
		/*if(m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_OUT] < 1.0e-6f) 
			m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_OUT] = 0.0f;*/
		if (m_SedOut[i] < 1.0e-6f)
		{
			m_SedOut[i] = 0.0f;
		}
		
		//update sed storage
		/*m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_STORAGE] = 
			allSediment - m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_OUT];
		if(m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_STORAGE] < 1.0e-6f) 
			m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_STORAGE] = 0.0f;*/
		m_sedStorage[i] = allSediment - m_SedOut[i];
		if (m_sedStorage[i] < 1.0e-6f)
		{
			m_sedStorage[i] = 0.0f;
		}

		//get final sediment in water, cannot large than 0.848t/m3
		float maxSedinWt = 0.848 * qOutV;
		if (m_SedOut[i] > maxSedinWt)
		{
			m_sedDep[i] += m_SedOut[i] - maxSedinWt;
			m_SedOut[i] = maxSedinWt;
		}

		/*if(i==37)
		{
			cout<<allWater <<" "<< m_qchOut[i] <<" "<<initCon << " "<< maxCon<<endl;
			cout << i <<" "<<m_sedSub[i]<<" "<<allSediment<<" "<<m_sedDeg[i]<<" "<<m_sedDep[i]<<" "<<m_SedOut[i]<<" "<<m_sedStorage[i]<<endl;
		}*/
		//m_SedOut[i] = m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_OUT];

			////channel downcutting and widening
			//bool dodowncutting = false;
			//if(dodowncutting)
			//{
			//	doChannelDowncuttingAndWidening(i);
			//}
		//}

	}

}

void SEDR_VCD::doChannelDowncuttingAndWidening(int id)
{
	float additionalDepth = m_depthcurrent[id] - m_chDepth[m_idToIndex[id]];
	if(additionalDepth < m_chSlope[m_idToIndex[id]] * m_chLen[m_idToIndex[id]])
	{
		float storage = m_chStorage[id];
		float vout = m_qchOut[id] * m_dt;
		if(storage + vout > 1.4e6f)
		{
			float waterDepth = m_chWTdepth[id];
			float addDepth = 358.6f * waterDepth * 
				m_slopecurrent[id] * m_erodibilityFactor;
			m_depthcurrent[id] += addDepth;

			m_widthcurrent[id] = m_depthcurrent[id] * 
				(m_chWidth[m_idToIndex[id]] / m_chDepth[m_idToIndex[id]]);

			m_slopecurrent[id] -= m_depthcurrent[id] / m_chLen[m_idToIndex[id]];
			m_slopecurrent[id] = max(0.0001f,m_slopecurrent[id]);
		}
	}
}
