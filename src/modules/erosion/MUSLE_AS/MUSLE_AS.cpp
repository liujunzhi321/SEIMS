#include <cmath>
#include "MUSLE_AS.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <math.h>
#include <omp.h>
#include <map>

MUSLE_AS::MUSLE_AS(void):m_nCells(-1), m_cellWidth(-1.f), m_nsub(-1), m_usle_c(NULL), m_usle_p(NULL), m_usle_k(NULL), m_usle_ls(NULL),
	m_flowacc(NULL), m_slope(NULL), m_streamLink(NULL), m_slopeForPq(NULL), m_snowAccumulation(NULL), m_surfaceRunoff(NULL),
	m_sedimentYield(NULL), m_sedtoCh(NULL), m_sedtoCh_T(0.f), m_depRatio(0.8f)
{

}

MUSLE_AS::~MUSLE_AS(void)
{
	if(m_sedimentYield != NULL) 
		delete [] m_sedimentYield;
	if(m_sedtoCh != NULL) 
		delete [] m_sedtoCh;
	if(m_usle_ls != NULL) 
		delete [] m_usle_ls;
	if(m_slopeForPq != NULL) 
		delete [] m_slopeForPq;
}

bool MUSLE_AS::CheckInputData(void)
{
	if(m_nCells <= 0)				throw ModelException("MUSLE_AS","CheckInputData","The dimension of the input data can not be less than zero.");
	if(m_cellWidth <= 0)			throw ModelException("MUSLE_AS","CheckInputData","The cell width can not be less than zero.");
	if(m_usle_c == NULL)		throw ModelException("MUSLE_AS","CheckInputData","The factor C can not be NULL.");
	if(m_usle_k == NULL)		throw ModelException("MUSLE_AS","CheckInputData","The factor K can not be NULL.");
	if(m_usle_p == NULL)		throw ModelException("MUSLE_AS","CheckInputData","The factor P can not be NULL.");
	if(m_flowacc == NULL)		throw ModelException("MUSLE_AS","CheckInputData","The flow accumulation can not be NULL.");
	if(m_slope == NULL)		throw ModelException("MUSLE_AS","CheckInputData","The slope can not be NULL.");
	if(m_snowAccumulation == NULL)	throw ModelException("MUSLE_AS","CheckInputData","The snow accumulation can not be NULL.");
	if(m_surfaceRunoff == NULL)		throw ModelException("MUSLE_AS","CheckInputData","The surface runoff can not be NULL.");
	if(m_streamLink == NULL)
		throw ModelException("IKW_CH","CheckInputData","The parameter: STREAM_LINK has not been set.");
	return true;
}

void MUSLE_AS::initalOutputs()
{
	if(m_nCells <= 0)				throw ModelException("MUSLE_AS","CheckInputData","The dimension of the input data can not be less than zero.");
	// allocate the output variables
	if(m_nsub <= 0)
	{
		map<int,int> subs;
		for(int i=0;i<m_nCells;i++)
		{
			subs[int(m_subbasin[i])] += 1;
		}
		m_nsub = subs.size();
	}
	if (m_sedtoCh == NULL)
	{
		m_sedtoCh = new float[m_nsub+1];
		//for (int i=0; i<=m_nsub; i++)
		//{
		//	m_sedtoCh[i] = 0.f;   // i=0 not used
		//}	
	}

	if(m_sedimentYield == NULL) 
		m_sedimentYield = new float[m_nCells];
	if(m_usle_ls == NULL) 
	{
		float constant = pow(22.13f,0.4f);
		m_usle_ls = new float[m_nCells];
		m_slopeForPq = new float[m_nCells];
		#pragma omp parallel for
		for(int i=0;i<m_nCells;i++)
		{
			if(m_usle_c[i] < 0.001f)
				m_usle_c[i] = 0.001f;
			if(m_usle_c[i] > 1.0f)
				m_usle_c[i] = 1.0f;
			float lambda_i1 = m_flowacc[i] * m_cellWidth;
			float lambda_i  = lambda_i1 + m_cellWidth;
			float L = pow(lambda_i,1.4f) - pow(lambda_i1,1.4f);
			L /= m_cellWidth * constant;

			//float S = pow(m_slope[i] / 0.0896f,1.3f);  
			float S = pow(sin(atan(m_slope[i])) / 0.0896f,1.3f); 

			m_usle_ls[i] = L * S;//equation 3 in memo, LS factor

			m_slopeForPq[i] = pow(m_slope[i] * 1000.0f,0.16f);
		}
	}
	m_cellAreaKM = pow(m_cellWidth/1000.0f,2.0f);
	m_cellAreaKM1 = 3.79f * pow(m_cellAreaKM,0.7f);
	m_cellAreaKM2 = 0.903f *pow(m_cellAreaKM,0.017f);

	m_sedtoCh_T = 0.0f;  // for every time step
	for (int i=0; i<=m_nsub; i++)
	{
		m_sedtoCh[i] = 0.f;   // i=0 not used
	}	
}

float MUSLE_AS::getPeakRunoffRate(int cell)
{
	return m_cellAreaKM1 * m_slopeForPq[cell] * pow(m_surfaceRunoff[cell] / 25.4f,m_cellAreaKM2); //equation 2 in memo, peak flow
}

int MUSLE_AS::Execute()
{
	CheckInputData();

	initalOutputs();

#pragma omp parallel for
	for(int i=0;i<m_nCells;i++)
	{
		if(m_surfaceRunoff[i] < 0.0001f || m_streamLink[i] > 0) 
			m_sedimentYield[i] = 0.0f;
		else
		{
			float q = getPeakRunoffRate(i); //equation 2 in memo, peak flow
			float Y = 11.8f * pow(m_surfaceRunoff[i] * m_cellAreaKM * 1000.0f * q, 0.56f) 
				* m_usle_k[i] * m_usle_ls[i] * m_usle_c[i] * m_usle_p[i];    //equation 1 in memo, sediment yield

			if(m_snowAccumulation[i] > 0.0001f) Y /= exp(3.0f * m_snowAccumulation[i] / 25.4f);  //equation 4 in memo, the snow pack effect
			m_sedimentYield[i] = Y;
		}
	}

	for(int i=0;i<m_nCells;i++)
	{
		// add each subbasin's sediment yield to channel
		if(m_nsub > 1)
			m_sedtoCh[int(m_subbasin[i])] += m_sedimentYield[i] * (1 - m_depRatio);
		else
			m_sedtoCh[1] += m_sedimentYield[i] * (1 - m_depRatio);
			
		m_sedtoCh_T += m_sedimentYield[i];
	}

	return 0;
}

//int MUSLE_AS::Execute()
//{
//	CheckInputData();
//
//	initalOutputs();
//	omp_lock_t lock;
//	omp_init_lock(&lock);
//
//	#pragma omp parallel for
//	for(int i=0;i<m_nCells;i++)
//	{
//		if(m_surfaceRunoff[i] < 0.0001f || m_streamLink[i] > 0) 
//			m_sedimentYield[i] = 0.0f;
//		else
//		{
//			float q = getPeakRunoffRate(i); //equation 2 in memo, peak flow
//			float Y = 11.8f * pow(m_surfaceRunoff[i] * m_cellAreaKM * 1000.0f * q, 0.56f) 
//				* m_usle_k[i] * m_usle_ls[i] * m_usle_c[i] * m_usle_p[i];    //equation 1 in memo, sediment yield
//
//			if(m_snowAccumulation[i] > 0.0001f) Y /= exp(3.0f * m_snowAccumulation[i] / 25.4f);  //equation 4 in memo, the snow pack effect
//			m_sedimentYield[i] = Y;
//
//			// add each subbasin's sediment yield to channel
//			int subid = m_subbasin[i];
//			omp_set_lock(&lock);
//			m_sedtoCh[subid] += Y * (1 - m_depRatio);
//
//			m_sedtoCh_T += Y;
//			omp_unset_lock(&lock);
//		}
//	}
//
//	omp_destroy_lock(&lock);
//	return 0;
//}

bool MUSLE_AS::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException("MUSLE_AS","CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(m_nCells != n)
	{
		if(m_nCells <=0) m_nCells = n;
		else
		{
			throw ModelException("MUSLE_AS","CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}

	return true;
}

void MUSLE_AS::SetValue(const char* key, float data)
{
	string sk(key);
	if(StringMatch(sk,"cellwidth"))			
		m_cellWidth = int(data);
	else if (StringMatch(sk, "ThreadNum"))
	{
		omp_set_num_threads((int)data);
	}
	else									throw ModelException("MUSLE_AS","SetValue","Parameter " + sk 
		+ " does not exist in MUSLE_AS method. Please contact the module developer.");
	
}

void MUSLE_AS::Set1DData(const char* key, int n, float* data)
{

	CheckInputSize(key,n);

	//set the value
	string s(key);

	if(StringMatch(s,"USLE_C"))				m_usle_c = data;
	else if(StringMatch(s,"USLE_P"))		m_usle_p = data;
	else if(StringMatch(s,"USLE_K"))		m_usle_k = data;
	else if(StringMatch(s,"acc"))		m_flowacc = data;
	else if(StringMatch(s,"slope"))			m_slope = data;
	else if(StringMatch(s,"subbasin"))			m_subbasin = data;
	else if(StringMatch(s,"D_SURU"))		m_surfaceRunoff = data;
	else if(StringMatch(s,"D_SNAC"))		m_snowAccumulation = data;
	else if(StringMatch(s, "STREAM_LINK"))
		m_streamLink = data;
	else									throw ModelException("MUSLE_AS","SetValue","Parameter " + s + 
		" does not exist in MUSLE_AS method. Please contact the module developer.");

}

void MUSLE_AS::GetValue(const char* key, float* value)
{
	string s(key);								
	if(StringMatch(s,"SEDTOCH_T"))				
	{
		*value = m_sedtoCh_T; // ton, // * 1000;    // metric tons convert to kg
	}
	else			
		throw ModelException("MUSLE_AS","getResult","Result " + s + " does not exist in MUSLE_AS method. Please contact the module developer.");
}

void MUSLE_AS::Get1DData(const char* key, int* n, float** data)
{
	string sk(key);
	if(StringMatch(sk,"SOER"))				
	{
		*data = m_sedimentYield;
	}
	else if(StringMatch(sk,"USLE_LS"))
	{
		*data = m_usle_ls;
	}
	else if (StringMatch(sk,"SEDTOCH"))
	{
		*data = m_sedtoCh;   // from each subbasin to channel
		*n = m_nsub+1;
	}
	else									throw ModelException("MUSLE_AS","getResult","Result " + sk + " does not exist in MUSLE_AS method. Please contact the module developer.");

	*n = m_nCells;
}


