/*----------------------------------------------------------------------
*	Purpose: 	nutrient routing using simple storage method at daily time scale
*
*	Created:	Wang Lin
*	Date:		29-March-2015
*
*	Revision:
*   Date:
*---------------------------------------------------------------------*/
//#include "vld.h"
#include "NutTra_CH.h"
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

NutTra_CH::NutTra_CH(void): m_dt(-1), m_nreach(-1), m_sNittoCh(NULL), m_laNittoCh(NULL), 
	 m_sAmmtoCh(NULL), m_sSolPtoCh(NULL), m_orgNtoCh(NULL), m_attPtoCh(NULL),m_Chs0(NODATA), 
	 m_chOrder(NULL),m_qchOut(NULL),m_prf(NODATA),m_spcon(NODATA),m_spexp(NODATA),m_vcrit(NODATA)
{
	//this->m_T_CHSB = NULL;
	m_NitOut = NULL;
	m_AmmOut = NULL;
	m_SoluPOut = NULL;
	m_OrgNOut = NULL;
	m_AttPOut = NULL;
	m_qsSub = NULL;
	m_qiSub = NULL;
	m_qgSub = NULL;
	m_chStorage = NULL;
	m_chWTdepth = NULL;
	//output
	m_nitStorage = NULL;
	m_nitSub = NULL;
	m_nitUps = NULL;
	//m_nitDep = NULL;
	//m_nitDeg = NULL
	m_ammStorage = NULL;
	m_ammSub = NULL;
	m_ammUps = NULL;
	m_soluPStorage = NULL;
	m_soluPSub = NULL;
	m_soluPUps = NULL;	
	m_orgNStorage = NULL;
	m_orgNUps = NULL;
	m_orgNSub = NULL;	
	m_attPStorage = NULL;
	m_attPUps = NULL;
	m_attPSub = NULL;
}


NutTra_CH::~NutTra_CH(void)
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
	if(m_NitOut != NULL) delete [] m_NitOut;
	if(m_nitStorage != NULL) delete [] m_nitStorage;
	if(m_nitSub != NULL) delete [] m_nitSub;
	if(m_nitUps != NULL) delete [] m_nitUps;
	//if(m_nitDeg != NULL) delete [] m_sedDeg;
	//if(m_nitDep != NULL) delete [] m_sedDep;
	//if(m_CrAreaCh != NULL) delete [] m_CrAreaCh;
	if(m_AmmOut != NULL) delete [] m_AmmOut;
	if(m_ammStorage != NULL) delete [] m_ammStorage;
	if(m_ammUps != NULL) delete [] m_ammUps;
	if(m_ammSub != NULL) delete [] m_ammSub;
	if(m_SoluPOut != NULL) delete [] m_SoluPOut;
	if(m_soluPStorage != NULL) delete [] m_soluPStorage;
	if(m_soluPUps != NULL) delete [] m_soluPUps;
	if(m_soluPSub != NULL) delete [] m_soluPSub;
	if(m_OrgNOut != NULL) delete [] m_OrgNOut;
	if(m_orgNStorage != NULL) delete [] m_orgNStorage;
	if(m_orgNUps != NULL) delete [] m_orgNUps;
	if(m_orgNSub != NULL) delete [] m_orgNSub;
	if(m_AttPOut != NULL) delete [] m_AttPOut;
	if(m_attPStorage != NULL) delete [] m_attPStorage;
	if(m_attPUps != NULL) delete [] m_attPUps;
	if(m_attPSub != NULL) delete [] m_attPSub;
}

bool NutTra_CH::CheckInputData(void)
{
	if (m_dt < 0)
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_dt has not been set.");
	}

	if (m_nreach < 0)
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_nreach has not been set.");
	}

	if (FloatEqual(m_Chs0,NODATA))
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: Chs0 has not been set.");
	}
	if (FloatEqual(m_prf,NODATA))
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_prf has not been set.");
	}
	if (FloatEqual(m_spcon,NODATA))
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_spcon has not been set.");
	}
	if (FloatEqual(m_spexp,NODATA))
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_spexp has not been set.");
	}
	if (FloatEqual(m_vcrit,NODATA))
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_vcrit has not been set.");
	}
	if (m_sNittoCh == NULL)
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_sNittoCh has not been set.");
	}
	if (m_laNittoCh == NULL)
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_laNittoCh has not been set.");
	}
	if (m_sAmmtoCh == NULL)
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_sAmmtoCh has not been set.");
	}
	if (m_sSolPtoCh == NULL)
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_sSolPtoCh has not been set.");
	}
	if (m_orgNtoCh == NULL)
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_orgNtoCh has not been set.");
	}
	if (m_attPtoCh == NULL)
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_attPtoCh has not been set.");
	}
	if (m_chWidth == NULL)
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: ReachParameter has not been set.");
	}
	/*if (m_CrAreaCh == NULL)
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_CrAreaCh has not been set.");
	}*/
	if (m_qgSub == NULL)
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_qgSub has not been set.");
	}
	if (m_qiSub == NULL)
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_qiSub has not been set.");
	}
	if (m_qgSub == NULL)
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_qgSub has not been set.");
	}
	if (m_chStorage == NULL)
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_chStorage has not been set.");
	}
	if (m_chWTdepth == NULL)
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_chWTdepth has not been set.");
	}
	if (m_qchOut == NULL)
	{
		throw ModelException("NutTra_CH","CheckInputData","The parameter: m_qchOut has not been set.");
	}
	return true;
}

void  NutTra_CH::initalOutputs()
{
	
	if(m_nreach <= 0) 
		throw ModelException("NutTra_CH","initalOutputs","The cell number of the input can not be less than zero.");

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
	if (m_NitOut == NULL)
	{
		//m_chStorage = new float[m_nreach+1];
		m_NitOut = new float[m_nreach+1];
		m_nitStorage = new float[m_nreach+1];
		m_nitUps = new float[m_nreach+1];
		m_nitSub = new float[m_nreach+1];
		//m_nitDep = new float[m_nreach+1];
		//m_nitDeg = new float[m_nreach+1];
		//m_T_CHSB = new float*[m_nreach+1];
		m_AmmOut = new float[m_nreach+1];
		m_ammStorage = new float[m_nreach+1];
		m_ammUps = new float[m_nreach+1];
		m_ammSub = new float[m_nreach+1];
		m_SoluPOut = new float[m_nreach+1];
		m_soluPStorage = new float[m_nreach+1];
		m_soluPUps = new float[m_nreach+1];
		m_soluPSub = new float[m_nreach+1];
		m_OrgNOut = new float[m_nreach+1];
		m_orgNStorage = new float[m_nreach+1];
		m_orgNUps = new float[m_nreach+1];
		m_orgNSub = new float[m_nreach+1];
		m_AttPOut = new float[m_nreach+1];
		m_attPStorage = new float[m_nreach+1];
		m_attPUps = new float[m_nreach+1];
		m_attPSub = new float[m_nreach+1];

		#pragma omp parallel for
		for (int i=1; i<=m_nreach; i++)
		{
			//m_widthcurrent[i] = m_chWidth[i];
			//m_depthcurrent[i] = m_chDepth[i];
			//m_slopecurrent[i] = m_chSlope[i];
			//m_widthbottom[i] = m_chWidth[i] - 2*m_sideslopeMain*m_chDepth[i];
			//m_CrAreaCh[i] = 0.0f;
			//m_chStorage[i] = m_Chs0 * m_chLen[i];
			m_NitOut[i] = 0.0f;
			m_nitStorage[i] = m_Chs0 * m_chLen[i];
			m_nitSub[i] = 0.0f;
			m_nitUps[i] = 0.0f;
			//m_nitDep[i] = 0.0f;
			//m_nitDeg[i] = 0.0f;
			m_AmmOut[i] = 0.0f;
			m_ammStorage[i] = m_Chs0 * m_chLen[i];
			m_ammUps[i] = 0.0f;
			m_ammSub[i] = 0.0f;
			m_SoluPOut[i] = 0.0f;
			m_soluPStorage[i] = m_Chs0 * m_chLen[i];
			m_soluPUps[i] = 0.0f;
			m_soluPSub[i] = 0.0f;
			m_OrgNOut[i] = 0.0f;
			m_orgNStorage[i] = m_Chs0 * m_chLen[i];
			m_orgNUps[i] = 0.0f;
			m_orgNSub[i] = 0.0f;		
			m_AttPOut[i] = 0.0f;
			m_attPStorage[i] = m_Chs0 * m_chLen[i];
			m_attPUps[i] = 0.0f;
			m_attPSub[i] = 0.0f;
			

			m_qchOut[i] = m_qgSub[i] + m_qiSub[i] + m_qgSub[i];
			/*m_T_CHSB[i] = new float[SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_COUNT];
			for (int j=0; j<SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_COUNT; j++)
			{
				m_T_CHSB[i][j] = 0.0f;
			}*/
		}
	}
}

int NutTra_CH::Execute()
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

			NutriChannelRouting(reachIndex);
		}
	}

	return 0;
}

bool NutTra_CH::CheckInputSize(const char* key, int n)
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
			throw ModelException("NutTra_CH","CheckInputSize",oss.str());
		}
	}

	return true;
}

void NutTra_CH::GetValue(const char* key, float* value)
{
	string sk(key);
	int iOutlet = m_reachLayers.rbegin()->second[0];
	if (StringMatch(sk, "NITOUTLET"))
	{
		*value = m_NitOut[iOutlet];
	}
	else if (StringMatch(sk, "AMMOUTLET"))
	{
		*value = m_AmmOut[iOutlet];
	}
	else if (StringMatch(sk, "SOLUPOUTLET"))
	{
		*value = m_SoluPOut[iOutlet];
	}
	else if (StringMatch(sk, "ORGNOUTLET"))
	{
		*value = m_OrgNOut[iOutlet];
	}
	else if (StringMatch(sk, "ATTPOUTLET"))
	{
		*value = m_AttPOut[iOutlet];
	}
	else			
		throw ModelException("NutTra_CH","getResult","Result " + sk
		+ " does not exist in NutTra_CH method.Please contact the module developer.");

}

void NutTra_CH::SetValue(const char* key, float value)
{
	string sk(key);

	if (StringMatch(sk, "ThreadNum"))
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
		throw ModelException("NutTra_CH", "SetSingleData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");

}

void NutTra_CH::Set1DData(const char* key, int n, float* value)
{
	string sk(key);
	//check the input data
	if(StringMatch(sk,"SNITTOCH"))
	{
		m_sNittoCh = value;   //for longterm model
	}
	else if(StringMatch(sk,"LANITTOCH"))
	{
		m_laNittoCh = value;   
	}
	else if(StringMatch(sk,"SAMMTOCH"))
	{
		m_sAmmtoCh = value;   
	}
	else if(StringMatch(sk,"SSOLPTOCH"))
	{
		m_sSolPtoCh = value;   
	}
	else if(StringMatch(sk,"ORGNTOCH"))
	{
		m_orgNtoCh = value;   
	}
	else if(StringMatch(sk,"ATTPTOCH"))
	{
		m_attPtoCh = value;   
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
		throw ModelException("NutTra_CH", "Set1DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
	
}

void NutTra_CH::Get1DData(const char* key, int* n, float** data)
{
	string sk(key);
	*n = m_nreach+1;
	int iOutlet = m_reachLayers.rbegin()->second[0];
	if (StringMatch(sk, "NITOUTLET"))
	{
		//m_SedOut[0] = m_SedOut[iOutlet] * 1000/24/3600;    // ton/day coverts to kg/s
 		m_NitOut[0] = m_NitOut[iOutlet];    // ton
		*data = m_NitOut;
	}
	else if (StringMatch(sk, "AMMOUTLET"))
	{
		//m_SedOut[0] = m_SedOut[iOutlet] * 1000/24/3600;    // ton/day coverts to kg/s
 		m_AmmOut[0] = m_AmmOut[iOutlet];    // ton
		*data = m_AmmOut;
	}
	else if (StringMatch(sk, "SOLUPOUTLET"))
	{
 		m_SoluPOut[0] = m_SoluPOut[iOutlet];    // ton
		*data = m_SoluPOut;
	}
	else if (StringMatch(sk, "ORGNOUTLET"))
	{
		//m_SedOut[0] = m_SedOut[iOutlet] * 1000/24/3600;    // ton/day coverts to kg/s
 		m_OrgNOut[0] = m_OrgNOut[iOutlet];    // ton
		*data = m_OrgNOut;
	}
	else if (StringMatch(sk, "ATTPOUTLET"))
	{
		//m_SedOut[0] = m_SedOut[iOutlet] * 1000/24/3600;    // ton/day coverts to kg/s
 		m_AttPOut[0] = m_AttPOut[iOutlet];    // ton
		*data = m_AttPOut;
	}
	else
		throw ModelException("NutTra_CH", "Get1DData", "Output " + sk 
		+ " does not exist in the NutTra_CH module. Please contact the module developer.");

}

void NutTra_CH::Get2DData(const char* key, int *nRows, int *nCols, float*** data)
{
	string sk(key);
	/*if (StringMatch(sk,"T_CHSB"))
	{
	*data = this->m_T_CHSB;
	*nRows = this->m_nreach;
	*nCols = SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_COUNT;
	}
	else
		throw ModelException("NutTra_CH", "Get2DData", "Output " + sk 
		+ " does not exist in the NutTra_CH module. Please contact the module developer.");*/

}

void NutTra_CH::Set2DData(const char* key, int nrows, int ncols, float** data)
{
	string sk(key);

	if (StringMatch(sk, "RchParam"))
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
		throw ModelException("NutTra_CH", "Set2DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");

}

void NutTra_CH::reset(int id)
{
	/*for(int j=0;j<SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_COUNT;j++) 
		this->m_T_CHSB[id][j] = 0.0f;
	m_SedOut[id] = m_T_CHSB[id][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_OUT];*/
	m_nitStorage[id] = 0.0f;
	m_nitUps[id] = 0.0f;
	m_nitSub[id] = 0.0f;
	//m_nitDeg[id] = 0.0f;
	//m_nitDep[id] = 0.0f;
	m_ammStorage[id] = 0.0f;
	m_ammUps[id] = 0.0f;
	m_ammSub[id] = 0.0f;
	m_soluPStorage[id] = 0.0f;
	m_soluPUps[id] = 0.0f;
	m_soluPSub[id] = 0.0f;
	m_orgNStorage[id] = 0.0f;
	m_orgNUps[id] = 0.0f;
	m_orgNSub[id] = 0.0f;
	m_attPStorage[id] = 0.0f;
	m_attPUps[id] = 0.0f;
	m_attPSub[id] = 0.0f;
}
/*
float NutTra_CH::Q(float waterDepth, int id)
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

float NutTra_CH::crossSectionalArea(float waterDepth, int id)
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

float NutTra_CH::wettedPerimeter(float waterDepth, int id)
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


float NutTra_CH::depth(float Q, int id)
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
*/
void NutTra_CH::NutriChannelRouting(int i)
{
	// 1 .whether is no water in channel
	//float storage = m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_STORAGE];
	float nitstorage = m_nitStorage[i];
	float ammstorage = m_ammStorage[i];
	float slouPstorage = m_soluPStorage[i];
	float orgNstorage = m_orgNStorage[i];
	float attPstorage = m_attPStorage[i];
	if (m_qchOut[i] < 0.0001f)
	{
		reset(i);
		m_nitStorage[i] = nitstorage;
		m_NitOut[i] = 0.f;
		m_ammStorage[i] = ammstorage;
		m_AmmOut[i] = 0.f;
		m_soluPStorage[i] = slouPstorage;
		m_SoluPOut[i] = 0.f;
		m_orgNStorage[i] = orgNstorage;
		m_OrgNOut[i] = 0.f;
		m_attPStorage[i] = attPstorage;
		m_AttPOut[i] = 0.f;
	}
	else
	{   
		// 2. nutrient from upstream reaches
		float nitUp = 0.f;
		float ammUp = 0.f;
		float soluPUp = 0.f;
		float orgNUp = 0.f;
		float attPUp = 0.f;
		for (size_t j = 0; j < m_reachUpStream[i].size(); ++j)
		{
			int upReachId = m_reachUpStream[i][j];
			nitUp += m_NitOut[upReachId];
			ammUp += m_AmmOut[upReachId];
			soluPUp += m_SoluPOut[upReachId];
			orgNUp += m_OrgNOut[upReachId];
			attPUp += m_AttPOut[upReachId];
		}
		float allNitrate = nitUp + m_sNittoCh[i] + m_laNittoCh[i] + m_nitStorage[i];
		float allAmmonium = soluPUp + m_sAmmtoCh[i] + m_ammStorage[i];
		float allSoluP = nitUp + m_sSolPtoCh[i] + m_soluPStorage[i];
		float allOrgN = orgNUp + m_orgNtoCh[i] + m_orgNStorage[i];
		float allAttP = attPUp + m_attPtoCh[i] + m_attPStorage[i];
		//cout<<i<<"\t"<<sedUp <<"\t"<<m_sedtoCh[i]<<"\t"<<sedstorage<<endl;
		
		m_nitUps[i] = nitUp;
		m_nitSub[i] = m_sNittoCh[i] + m_laNittoCh[i];
		m_ammUps[i] = ammUp;
		m_ammSub[i] = m_sAmmtoCh[i];
		m_soluPUps[i] = soluPUp;
		m_soluPSub[i] = m_sSolPtoCh[i];
		m_orgNUps[i] = orgNUp;
		m_orgNSub[i] = m_orgNtoCh[i];
		m_attPUps[i] = attPUp;
		m_attPSub[i] = m_attPtoCh[i];
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
		//float maxCon = m_spcon * pow(peakVelocity, m_spexp); 
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
			m_NitOut[i] = 0.0f;
			m_AmmOut[i] = 0.0f;
			m_SoluPOut[i] = 0.0f;
			m_OrgNOut[i] = 0.0f;
			m_AttPOut[i] = 0.0f;
			return;
		}
		float initCon = allNitrate/allWater;
		//cout << i <<" "<<allSediment<<" "<<peakVelocity << " "<<tbase<<" "<<initCon<<" "<<maxCon<<endl;
		/*//deposition and degradation
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
		/*m_sedDep[i] += sedDeposition - sedDegradation1;
		if (m_sedDep[i] < 1.0e-6f)
		{
			m_sedDep[i] = 0.0f;
		}*/
		//update sed degradation
		/*m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_DEGRADATION] += sedDegradation1 + sedDegradation2;
		if(m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_DEGRADATION] < 1.0e-6f) 
			m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_DEGRADATION] = 0.0f;*/
		/*m_sedDeg[i] += sedDegradation1 + sedDegradation2;
		if (m_sedDeg[i] < 1.0e-6f)
		{
			m_sedDeg[i] = 0.0f;
		}
		//get sediment after deposition and degradation
		allSediment += sedDegradation1 + sedDegradation2 - sedDeposition;
		if(allSediment < 1.0e-6f) allSediment = 0.0f;*/

		//get out flow water fraction
		float outFraction = qOutV / allWater;
		if(outFraction < 1.0e-6f) outFraction = 0.0f;
		if(outFraction > 1.0f) outFraction = 1.0f;
		//update sed out
		//m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_OUT] = allSediment * outFraction;
		m_NitOut[i] = allNitrate * outFraction;
		m_AmmOut[i] = allAmmonium * outFraction;
		m_SoluPOut[i] = allSoluP * outFraction;
		m_OrgNOut[i] = allOrgN * outFraction;
		m_AttPOut[i] = allAttP * outFraction;
		/*if(m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_OUT] < 1.0e-6f) 
			m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_OUT] = 0.0f;*/
		if (m_NitOut[i] < 1.0e-6f)
		{
			m_NitOut[i] = 0.0f;
		}
		if (m_AmmOut[i] < 1.0e-6f)
		{
			m_AmmOut[i] = 0.0f;
		}
		if (m_SoluPOut[i] < 1.0e-6f)
		{
			m_SoluPOut[i] = 0.0f;
		}
		if (m_OrgNOut[i] < 1.0e-6f)
		{
			m_OrgNOut[i] = 0.0f;
		}
		if (m_AttPOut[i] < 1.0e-6f)
		{
			m_AttPOut[i] = 0.0f;
		}
		
		//update sed storage
		/*m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_STORAGE] = 
			allSediment - m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_OUT];
		if(m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_STORAGE] < 1.0e-6f) 
			m_T_CHSB[i][SEDIMENT_CHANNEL_ROUTING_RESULT_DISCHARGE_COLUMN_INDEX_STORAGE] = 0.0f;*/
		m_nitStorage[i] = allNitrate - m_NitOut[i];
		if (m_nitStorage[i] < 1.0e-6f)
		{
			m_nitStorage[i] = 0.0f;
		}
		m_ammStorage[i] = allAmmonium - m_AmmOut[i];
		if (m_ammStorage[i] < 1.0e-6f)
		{
			m_ammStorage[i] = 0.0f;
		}
		m_soluPStorage[i] = allSoluP - m_SoluPOut[i];
		if (m_soluPStorage[i] < 1.0e-6f)
		{
			m_soluPStorage[i] = 0.0f;
		}
		m_orgNStorage[i] = allOrgN - m_OrgNOut[i];
		if (m_orgNStorage[i] < 1.0e-6f)
		{
			m_orgNStorage[i] = 0.0f;
		}
		m_attPStorage[i] = allAttP - m_AttPOut[i];
		if (m_attPStorage[i] < 1.0e-6f)
		{
			m_attPStorage[i] = 0.0f;
		}

	}

}

