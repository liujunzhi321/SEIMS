#include "MineralizationAndDecomposition.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>
#include <cmath>
#include <iostream>

using namespace std;

MineralizationAndDecomposition::MineralizationAndDecomposition(void):m_nLayers(3), m_size(-1),m_RateConst(0.00001f), m_HumNF(0.02f), m_RateCoeH(0.002f),    //input
	m_RateCoeR(0.05f), m_conSoluP(5.0f), m_Density(NULL), m_SoilT(NULL), m_SOMO(NULL), m_FieldCap(NULL), m_Depth(NULL), 
	m_TF(NULL), m_WF(NULL), m_ActOrgN(NULL), m_StaOrgN(NULL), m_TraOrgN(NULL), m_orgCar(NULL),                                                              //output
	m_MinHumN(NULL), m_Res(NULL), m_Nitrate(NULL), m_SoluP(NULL), m_CNRat(NULL), m_CPRat(NULL), m_RateConRD(NULL), m_ResComF(NULL),
	m_MinFreOrgN(NULL), m_DecFreOrgN(NULL), m_actOrgP(NULL), m_conHumOrgN(NULL), m_conActOrgN(NULL), m_conStaOrgN(NULL), 
	m_HumOrgN(NULL), m_FreOrgN(NULL), m_humOrgP(NULL), m_FreOrgP(NULL), m_staOrgP(NULL), m_minHumP(NULL), m_minFreOrgP(NULL), m_decFreOrgP(NULL)
{
}

MineralizationAndDecomposition::~MineralizationAndDecomposition(void)
{
	if(m_TF != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_TF[i];
		delete [] m_TF;
	}
	if(m_WF != NULL)
	{	
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_WF;
		delete [] m_WF;
	}
	if(m_conHumOrgN != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_conHumOrgN;
		delete [] m_conHumOrgN;
	}
	if(m_conActOrgN != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_conActOrgN;
		delete [] m_conActOrgN;
	}
	if(m_conStaOrgN != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_conStaOrgN;
		delete [] m_conStaOrgN;
	}
	if(m_HumOrgN != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_HumOrgN;
		delete [] m_HumOrgN;
	}
	if(m_ActOrgN != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
            delete [] m_ActOrgN;
		delete [] m_ActOrgN;
	}
	if(m_StaOrgN != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_StaOrgN;
		delete [] m_StaOrgN;
	}
	if(m_FreOrgN != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_FreOrgN;
		delete [] m_FreOrgN;
	}
	if(m_TraOrgN != NULL)
	{	
		for (int i=0; i < m_nLayers; ++i)
	        delete [] m_TraOrgN;
	    delete [] m_TraOrgN;
	}
	if(m_MinHumN != NULL)
	{	
		for (int i=0; i < m_nLayers; ++i)
	        delete [] m_MinHumN;
	    delete [] m_MinHumN;
	}
	if(m_CNRat != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_CNRat;
		delete [] m_CNRat;
	}
	if(m_CPRat != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_CPRat;
		delete [] m_CPRat;
	}
	if(m_RateConRD != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_RateConRD;
		delete [] m_RateConRD;
	}
	if(m_ResComF != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_ResComF;
		delete [] m_ResComF;
	}
	if(m_MinFreOrgN != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)		
		    delete [] m_MinFreOrgN;
		delete [] m_MinFreOrgN;
	}
	if(m_DecFreOrgN != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_DecFreOrgN;
		delete [] m_DecFreOrgN;
	}
	if(m_SoluP != NULL)
	{	
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_SoluP;
		delete [] m_SoluP;
	}
	if(m_humOrgP != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_humOrgP;
		delete [] m_humOrgP;
	}
	if(m_FreOrgP != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_FreOrgP;
		delete [] m_FreOrgP;
	}
	if(m_actOrgP != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_actOrgP;
		delete [] m_actOrgP;
	}
	if(m_staOrgP != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_staOrgP;
		delete [] m_staOrgP;
	}
	if(m_minHumP != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_minHumP;
		delete [] m_minHumP;
	}
	if(m_minFreOrgP != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_minFreOrgP;
		delete [] m_minFreOrgP;
	}
	if(m_decFreOrgP != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_decFreOrgP;
		delete [] m_decFreOrgP;
	}
}

bool MineralizationAndDecomposition::CheckInputData(void)
{
	if(this->m_size <= 0)
	{
		throw ModelException("MineralizationAndDecomposition","CheckInputData","The cell number of the input can not be less than zero.");
		return false;
	}
	if(this->m_RateConst <= 0)
	{
		throw ModelException("MineralizationAndDecomposition","CheckInputData","You have not set the rate constant.");
		return false;
	}
	if(this->m_HumNF <= 0)
	{
		throw ModelException("MineralizationAndDecomposition","CheckInputData","You have not set the fraction of humic nitrogen in the active pool.");
		return false;
	}
	if(this->m_RateCoeH <= 0)
	{
	    throw ModelException("MineralizationAndDecomposition","CheckInputData","You have not set the rate coefficient for mineralization of the humus active organic nutrients.");
	}
	if(this->m_RateCoeR <= 0)
	{
		throw ModelException("MineralizationAndDecomposition","CheckInputData","You have not set the rate coefficient for mineralization of the residue fresh organic nutrients.");
	}
	if(this->m_conSoluP <= 0)
	{
		throw ModelException("MineralizationAndDecomposition","CheckInputData","You have not set the concentration of solution phosphorus all layers.");
		return false;
	}
	
	//if(m_rootDepth == NULL)
	//	throw ModelException("MineralizationAndDecomposition","CheckInputData","You have not set the depth of the root.");
	if(m_Density == NULL)
		throw ModelException("MineralizationAndDecomposition","CheckInputData","You have not set the bulk density of the layer.");
	//if(m_orgCar == NULL)
	//{
	//	m_orgCar = new float*[m_size];
	//
	//	for (int i = 0; i < m_size; ++i)
	//	{
	//		m_orgCar[i] = new float[m_nLayers];
	//		for (int j = 0; j < m_nLayers; ++j)
	//			m_orgCar[i][j] = 1.12f;
	//	}	
	//}
	if(m_SoilT == NULL)
		throw ModelException("MineralizationAndDecomposition","CheckInputData","You have not set the soil temperature.");
	if(m_SOMO == NULL)
		throw ModelException("MineralizationAndDecomposition","CheckInputData","You have not set the water content on a given day.");
	if(m_FieldCap == NULL)
		throw ModelException("MineralizationAndDecomposition","CheckInputData","You have not set the water content at field capacity.");
	if(m_Depth == NULL)
		throw ModelException("MineralizationAndDecomposition","CheckInputData","You have not set the depth of the layer.");
    //if(m_som == NULL)
	//	throw ModelException("MineralizationAndDecomposition","CheckInputData","You have not set the amount of soil organic matter.");
	//if(m_Res == NULL)
	//{
	//	m_Res = new float*[m_size];

	//	for (int i = 0; i < m_size; ++i)
	//	{
	//		m_Res[i] = new float[m_nLayers];
	//		m_Res[i][0] = 10.0f;
	//		m_Res[i][1] = 0.0f;
	//		m_Res[i][2] = 0.0f;
	//	}	
	//}
	if(m_Nitrate == NULL)
	    throw ModelException("MineralizationAndDecomposition","CheckInputData","You have not set the amount of nitrate in layer ly.");

	return true;
}

void  MineralizationAndDecomposition::initalOutputs()
{
	if(this->m_size <= 0) throw ModelException("MineralizationAndDecomposition","initalOutputs","The cell number of the input can not be less than zero.");

	if(m_TF == NULL)
	{
		m_TF = new float*[m_nLayers];
		m_WF = new float*[m_nLayers];
		//m_orgCar = new float*[m_nLayers];
		m_conHumOrgN = new float*[m_nLayers];
		m_conActOrgN = new float*[m_nLayers];
		m_conStaOrgN = new float*[m_nLayers];
		m_ActOrgN = new float*[m_nLayers];
		m_StaOrgN = new float*[m_nLayers];
		m_HumOrgN = new float*[m_nLayers];
		m_TraOrgN = new float*[m_nLayers];
		m_MinHumN = new float*[m_nLayers];
		m_CNRat = new float*[m_nLayers];
		m_CPRat = new float*[m_nLayers];
		m_RateConRD = new float*[m_nLayers];
		m_ResComF = new float*[m_nLayers];
		m_MinFreOrgN = new float*[m_nLayers];
		m_DecFreOrgN = new float*[m_nLayers];
		m_SoluP = new float*[m_nLayers];
		m_humOrgP = new float*[m_nLayers];
		m_FreOrgP = new float*[m_nLayers];
		m_actOrgP = new float*[m_nLayers];
		m_staOrgP = new float*[m_nLayers];
		m_FreOrgN = new float*[m_nLayers];
		m_minHumP = new float*[m_nLayers];
		m_minFreOrgP = new float*[m_nLayers];
		m_decFreOrgP = new float*[m_nLayers];

	#pragma omp parallel for
		for (int i = 0; i < m_nLayers; ++i)
		{
			m_TF[i] = new float[m_size];	
			m_WF[i] = new float[m_size];
			//m_orgCar[i] = new float[m_size];
			m_conHumOrgN[i] = new float[m_size];	
			m_conActOrgN[i] = new float[m_size];	
			m_conStaOrgN[i] = new float[m_size];	
			m_ActOrgN[i] = new float[m_size];
			m_StaOrgN[i] = new float[m_size];
			m_HumOrgN[i] = new float[m_size];
			m_TraOrgN[i] = new float[m_size];	
			m_MinHumN[i] = new float[m_size];
			m_CNRat[i] = new float[m_size];
			m_CPRat[i] = new float[m_size];
			m_RateConRD[i] = new float[m_size];
			m_ResComF[i] = new float[m_size];
			m_MinFreOrgN[i] = new float[m_size];
			m_DecFreOrgN[i] = new float[m_size];
			m_SoluP[i] = new float[m_size]; 
			m_humOrgP[i] = new float[m_size];
			m_FreOrgP[i] = new float[m_size];
			m_actOrgP[i] = new float[m_size];
			m_staOrgP[i] = new float[m_size];
			m_FreOrgN[i] = new float[m_size];
			m_minHumP[i] = new float[m_size];
			m_minFreOrgP[i] = new float[m_size];
			m_decFreOrgP[i] = new float[m_size];

			for (int j = 0; j < m_size; j++)
			{
				m_TF[i][j] = 0.0f;
			    m_WF[i][j] = 0.0f;
				//m_orgCar[i][j] = 0.0f;
				m_conHumOrgN[i][j] = 0.0f;
				m_conActOrgN[i][j] = 0.0f;
				m_conStaOrgN[i][j] = 0.0f;
				m_ActOrgN[i][j] = 0.0f;
				m_StaOrgN[i][j] = 0.0f;
				m_HumOrgN[i][j] = 0.0f;
				m_TraOrgN[i][j] = 0.0f;
				m_MinHumN[i][j] = 0.0f;
				m_CNRat[i][j] = 0.0f;
				m_CPRat[i][j] = 0.0f;
				m_RateConRD[i][j] = 0.0f;
				m_ResComF[i][j] = 0.0f;
				m_MinFreOrgN[i][j] = 0.0f;
				m_DecFreOrgN[i][j] = 0.0f;
				m_SoluP[i][j] = 0.0f;
				m_humOrgP[i][j] = 0.0f;
				m_actOrgP[i][j] = 0.0f;
				m_staOrgP[i][j] = 0.0f;
				m_FreOrgN[i][j] = 0.0f;
				m_minHumP[i][j] = 0.0f;
				m_minFreOrgP[i][j] = 0.0f;
				m_decFreOrgP[i][j] = 0.0f;
			}
		}
	}

}

bool MineralizationAndDecomposition::CheckInputSize(const char* key, int n)
{
	if(n <= 0)
	{
		//this->StatusMsg("Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(this->m_size != n)
	{
		if(this->m_size <=0) this->m_size = n;
		else
		{
			//this->StatusMsg("Input data for "+string(key) +" is invalid. All the input data should have same size.");
			ostringstream oss;
			oss << "Input data for "+string(key) << " is invalid with size: " << n << ". The origin size is " << m_size << ".\n";  
			throw ModelException("MineralizationAndDecomposition","CheckInputSize",oss.str());
		}
	}

	return true;
}

void MineralizationAndDecomposition::SetValue(const char* key, float data)
{
	string sk(key);
	if (StringMatch(sk, "RateConst"))
	{
		if(data > 0)
			m_RateConst = data;
	}
	else if (StringMatch(sk, "HumNF"))
	{
		if(data > 0)
			m_HumNF = data;
	}
	else if (StringMatch(sk, "RateCoeH"))
	{
		if(data > 0)
			m_RateCoeH = data;
	}
	else if (StringMatch(sk, "RateCoeR"))
	{
		if(data > 0)
			m_RateCoeR = data;
	}
	else if (StringMatch(sk, "ConSoluP"))
	{
		if(data > 0)
			m_conSoluP = data;
	}
	else if (StringMatch(sk, "ThreadNum"))
	{
		omp_set_num_threads((int)data);
	}
	else
		throw ModelException("MineralizationAndDecomposition", "SetSingleData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
	
}

void MineralizationAndDecomposition::Set1DData(const char* key, int n, float* data)
{
	//check the input data
	CheckInputSize(key,n);
	string sk(key);
	//if (StringMatch(sk, "RootDepth"))
	//	m_rootDepth = data;
	if (StringMatch(sk, "D_SOTE"))
		m_SoilT = data;
	else
		throw ModelException("MineralizationAndDecomposition", "Set1DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
	
}

void MineralizationAndDecomposition::Set2DData(const char* key, int nrows, int ncols, float** data)
{
	//check the input data
	CheckInputSize(key, ncols);

	string sk(key);
	if(StringMatch(sk, "D_Nitrate"))
		m_Nitrate = data;
	else if(StringMatch(sk,"D_SOMO_2D"))		
		m_SOMO = data;
	else if (StringMatch(sk, "FieldCap_2D"))
		m_FieldCap = data;
	else if(StringMatch(sk, "Density_2D"))
		m_Density = data;
	else if(StringMatch(sk, "D_Depth"))
		m_Depth = data;
	//else if(StringMatch(sk,"SOM"))		
	//	m_som = data;
	else if(StringMatch(sk, "Res"))
		m_Res = data;
	else
		throw ModelException("MineralizationAndDecomposition", "Set2DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
}

void MineralizationAndDecomposition::Get2DData(const char* key, int *nRows, int *nCols, float*** data)
{
	string sk(key);
	*nRows = m_nLayers;
	*nCols = m_size;
	if (StringMatch(sk, "TF"))
		*data = m_TF; 
	else if (StringMatch(sk, "WF"))
		*data = m_WF;
	//else if (StringMatch(sk, "OrgCar"))
		//*data = m_orgCar;
	else if (StringMatch(sk, "ConHumOrgN"))
		*data = m_conHumOrgN;
	else if (StringMatch(sk, "ConActOrgN"))
		*data = m_conActOrgN;
	else if (StringMatch(sk, "ConStaOrgN"))
		*data = m_conStaOrgN;
	else if (StringMatch(sk, "HumOrgN"))
		*data = m_HumOrgN;
	else if (StringMatch(sk, "ActOrgN"))
		*data = m_ActOrgN;
	else if (StringMatch(sk, "StaOrgN"))
		*data = m_StaOrgN;
	else if (StringMatch(sk, "FreOrgN"))
		*data = m_FreOrgN;
	else if (StringMatch(sk, "TraOrgN"))
		*data = m_TraOrgN;
	else if (StringMatch(sk, "MinHumN"))
		*data = m_MinHumN;
	else if (StringMatch(sk, "CNRat"))
		*data = m_CNRat;
	else if (StringMatch(sk, "CPRat"))
		*data = m_CPRat;
	else if (StringMatch(sk, "RateConRD"))
		*data = m_RateConRD;
	else if (StringMatch(sk, "ResComF"))
		*data = m_ResComF;
	else if (StringMatch(sk, "MinFreOrgN"))
		*data = m_MinFreOrgN;
	else if (StringMatch(sk, "DecFreOrgN"))
		*data = m_DecFreOrgN;
	else if (StringMatch(sk, "SoluP"))
		*data = m_SoluP;
	else if (StringMatch(sk, "HumOrgP"))
		*data = m_humOrgP;
	else if (StringMatch(sk, "FreOrgP"))
		*data = m_FreOrgP;
	else if (StringMatch(sk, "ActOrgP"))
		*data = m_actOrgP;
	else if (StringMatch(sk, "StaOrgP"))
		*data = m_staOrgP;
	else if (StringMatch(sk, "MinHumP"))
		*data = m_minHumP;
	else if (StringMatch(sk, "MinFreOrgP"))
		*data = m_minFreOrgP;
	else if (StringMatch(sk, "DecFreOrgP"))
		*data = m_decFreOrgP;
	else
		throw ModelException("MineralizationAndDecomposition", "Get2DData", "Output " + sk 
		+ " does not exist in the Mineralization And Decomposition module. Please contact the module developer.");

}

int MineralizationAndDecomposition::Execute()
{
	//check the data
	CheckInputData();	

	initalOutputs();

	#pragma omp parallel for
	for(int i=0; i < m_nLayers; i++)
    {
		for(int j=0; j < m_size; j++)
		{
			float residual[3];
		    if (m_Res == NULL)
		    {
			    residual[0] = 10.f;
			    residual[1] = 0.f;
			    residual[2] = 0.f;
		    }
		    else
		    {
			    residual[i] = m_Res[i][j];
		    }
		   	
			if (m_SoilT[j] < 0)
			continue;

			int hydroIndex = max(0, i-1);
			float m_NumNF = 0.02f;
			float orgCar = 1.12f;
			if (m_orgCar != NULL)
		    orgCar = m_orgCar[i][j];

		    m_conHumOrgN[i][j] = 10000.f * orgCar / 14.f;
			m_conActOrgN[i][j] = m_conHumOrgN[i][j] * m_NumNF;
			m_conStaOrgN[i][j] = m_conHumOrgN[i][j] * (1 - m_NumNF);

			m_HumOrgN[i][j] = m_conHumOrgN[i][j] * m_Density[hydroIndex][j] * m_Depth[i][j] / 100;
			m_ActOrgN[i][j] = m_conActOrgN[i][j] * m_Density[hydroIndex][j] * m_Depth[i][j] / 100;
			m_StaOrgN[i][j] = m_conStaOrgN[i][j] * m_Density[hydroIndex][j] * m_Depth[i][j] / 100;
			m_SoluP[i][j] = m_conSoluP * m_Density[hydroIndex][j] * m_Depth[i][j] / 100;

			m_FreOrgN[i][j] = 0.0015f * residual[i];  

			m_TF[i][j] = (0.9f * m_SoilT[j] / (m_SoilT[j] + exp(9.93f - 0.312f * m_SoilT[j])))+0.1f;
		    m_WF[i][j] = m_SOMO[hydroIndex][j] / m_FieldCap[hydroIndex][j];

			float m_RateConst = 0.00001f;
			m_TraOrgN[i][j] = m_RateConst * m_ActOrgN[i][j] * (1 / m_NumNF - 1) - m_StaOrgN[i][j];
			m_MinHumN[i][j] = m_RateCoeH * pow(m_TF[i][j] * m_WF[i][j], 0.5f) * m_ActOrgN[i][j];
			m_CNRat[i][j] = 0.58f * residual[i] / (m_FreOrgN[i][j] + m_Nitrate[i][j]);
			m_CPRat[i][j] = 0.58f * residual[i] / (m_FreOrgP[i][j] + m_SoluP[i][j]);
			float r1 = exp(-0.693f * (m_CNRat[i][j] - 25) / 25);
			float r2 = exp(-0.693f * (m_CPRat[i][j] - 200) / 200);
			float r = min(r1,r2);
			m_ResComF[i][j] = min(r,1.f);
			m_RateConRD[i][j] = m_RateCoeR * m_ResComF[i][j] * pow(m_TF[i][j] * m_WF[i][j],0.5f);
			m_MinFreOrgN[i][j] = 0.8f * m_RateConRD[i][j] * m_FreOrgN[i][j];
			m_DecFreOrgN[i][j] = 0.2f * m_RateConRD[i][j] * m_FreOrgN[i][j];
			
			m_Nitrate[i][j] += m_MinFreOrgN[i][j] + m_MinHumN[i][j];
			m_ActOrgN[i][j] += m_DecFreOrgN[i][j] - m_TraOrgN[i][j];
			m_StaOrgN[i][j] += m_TraOrgN[i][j];
			m_FreOrgN[i][j] -= m_MinFreOrgN[i][j] + m_DecFreOrgN[i][j];
			
			m_humOrgP[i][j] = 0.125f * m_HumOrgN[i][j];
            m_FreOrgP[i][j] = 0.0003f * residual[i];
			
			m_actOrgP[i][j] = m_humOrgP[i][j] * m_ActOrgN[i][j] / (m_ActOrgN[i][j] + m_StaOrgN[i][j]);
			m_staOrgP[i][j] = m_humOrgP[i][j] * m_StaOrgN[i][j] / (m_ActOrgN[i][j] + m_StaOrgN[i][j]);
			m_minHumP[i][j]= 1.4f * m_RateCoeH * pow(m_TF[i][j] * m_WF[i][j], 0.5f) * m_actOrgP[i][j];
			m_minFreOrgP[i][j] = 0.8f * m_RateConRD[i][j] * m_FreOrgP[i][j];
			m_decFreOrgP[i][j] = 0.2f * m_RateConRD[i][j] * m_FreOrgP[i][j];
	
			m_humOrgP[i][j] -= m_minHumP[i][j];
			m_FreOrgP[i][j] -= (m_minFreOrgP[i][j] + m_decFreOrgP[i][j]);
			m_SoluP[i][j] += m_minHumP[i][j] + m_minFreOrgP[i][j];
			m_actOrgP[i][j] += m_decFreOrgP[i][j];

		}
	}

	return 0;
}