#include "NitrificationAndAmmoniaVolatilization.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"
#include <omp.h>
#include <cmath>
#include <iostream>

using namespace std;

NitrificationAndAmmoniaVolatilization::NitrificationAndAmmoniaVolatilization(void):m_size(-1), m_nLayers(3), m_catEF(0.15f), m_SoilT(NULL), m_SOMO(NULL), m_Depth(NULL), //input
	m_Wiltingpoint(NULL), m_FieldCap(NULL), m_Ammon(NULL), m_Nitrate(NULL),      
	m_midZ(NULL), m_nitvolTF(NULL), m_nitWF(NULL), m_volDF(NULL), m_nitR(NULL), m_volR(NULL), m_nitvolAmmon(NULL), m_nitLostF(NULL), m_volLostF(NULL), m_conNitra(NULL), m_conAmm(NULL) //output
{

}

NitrificationAndAmmoniaVolatilization::~NitrificationAndAmmoniaVolatilization(void)
{
	if(m_midZ != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_midZ;
		delete [] m_midZ;
	}
	if(m_nitvolTF != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_nitvolTF;
		delete [] m_nitvolTF;
	}
	if(m_nitWF != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_nitWF;
		delete [] m_nitWF;
	}
	if(m_volDF != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_volDF;
		delete [] m_volDF;
	}
	if(m_nitR != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_nitR;
		delete [] m_nitR;
	}
	if(m_volR != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_volR;
		delete [] m_volR;
	}
	if(m_nitvolAmmon != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
		    delete [] m_nitvolAmmon;
		delete [] m_nitvolAmmon;
	}
	if(m_nitLostF != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_nitLostF;
		delete [] m_nitLostF;
	}
	if(m_volLostF != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_volLostF;
		delete [] m_volLostF;
	}
	if(m_conNitra != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_conNitra;
		delete [] m_conNitra;
	}
	if(m_conAmm != NULL)
	{
		for (int i=0; i < m_nLayers; ++i)
			delete [] m_conAmm;
		delete [] m_conAmm;
	}
}

bool NitrificationAndAmmoniaVolatilization::CheckInputData(void)
{
	if(m_size <= 0)
	{
		throw ModelException("NitVol","CheckInputData","The cell number of the input can not be less than zero.");
		return false;
	}
	if(m_catEF <= 0)
	{
		cout << "The volatilization cation exchange factor: " << m_catEF << endl;
		throw ModelException("NitrificationAndAmmoniaVolatilization","CheckInputData",
			"The volatilization cation exchange factor of the input can not be less than zero.");
		return false;
	}
	
	if(m_SoilT == NULL)
		throw ModelException("NitVol","CheckInputData","You have not set the soil temperature.");
	if(m_SOMO == NULL)
		throw ModelException("NitVol","CheckInputData","You have not set the water content on a given day.");
	if(m_Nitrate == NULL)
	    throw ModelException("NitVol","CheckInputData","You have not set the amount of nitrate in layer ly.");
	if(m_Ammon == NULL)
	    throw ModelException("NitVol","CheckInputData","You have not set the amount of ammonium in layer ly.");
	if(m_Wiltingpoint == NULL)
		throw ModelException("NitVol","CheckInputData","You have not set the amount of water held in the soil layer at wilting point water content.");
	if(m_FieldCap == NULL)
		throw ModelException("NitVol","CheckInputData","You have not set the water content at field capacity.");
	if(m_Depth == NULL)
		throw ModelException("NitVol","CheckInputData","You have not set the depth of the layer.");
	
	return true;
}

void  NitrificationAndAmmoniaVolatilization::initalOutputs()
{
	if(this->m_size <= 0) throw ModelException("NitrificationAndAmmoniaVolatilization","initalOutputs","The cell number of the input can not be less than zero.");

	if(m_nitvolTF == NULL)
	{
		m_midZ = new float*[m_nLayers];
		m_nitvolTF = new float*[m_nLayers];
		m_nitWF = new float*[m_nLayers];
		m_volDF = new float*[m_nLayers];
		m_nitR = new float*[m_nLayers];
		m_volR = new float*[m_nLayers];
		m_nitvolAmmon = new float*[m_nLayers];
		m_nitLostF = new float*[m_nLayers];
		m_volLostF = new float*[m_nLayers];
		m_conNitra = new float*[m_nLayers];
		m_conAmm = new float*[m_nLayers];
	
	#pragma omp parallel for
		for (int i = 0; i < m_nLayers; ++i)
		{
			m_midZ[i] = new float[m_size];
			m_nitvolTF[i] = new float[m_size];	
			m_nitWF[i] = new float[m_size];	
			m_volDF[i] = new float[m_size];	
			m_nitR[i] = new float[m_size];
			m_volR[i] = new float[m_size];
			m_nitvolAmmon[i] = new float[m_size];
			m_nitLostF[i] = new float[m_size];
			m_volLostF[i] = new float[m_size];
			m_conNitra[i] = new float[m_size];
			m_conAmm[i] = new float[m_size];

			for (int j = 0; j < m_size; j++)
			{
				m_midZ[i][j] = 0.0f;
				m_nitvolTF[i][j] = 0.0f;
				m_nitWF[i][j] = 0.0f; 
				m_volDF[i][j] = 0.0f; 
				m_nitR[i][j] = 0.0f; 
				m_volR[i][j] = 0.0f; 
				m_nitvolAmmon[i][j] = 0.0f; 
				m_nitLostF[i][j] = 0.0f; 
				m_volLostF[i][j] = 0.0f; 
				m_conNitra[i][j] = 0.0f; 
				m_conAmm[i][j] = 0.0f; 
			}
		}
	}

}

bool NitrificationAndAmmoniaVolatilization::CheckInputSize(const char* key, int n)
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
			throw ModelException("NitrificationAndAmmoniaVolatilization","CheckInputSize",oss.str());
		}
	}

	return true;
}

void NitrificationAndAmmoniaVolatilization::SetValue(const char* key, float data)
{
	string sk(key);
	if (StringMatch(sk, "CatEF"))
	{
		if (data > 0)
			m_catEF = data;
	}
	else if (StringMatch(sk, "ThreadNum"))
	{
		omp_set_num_threads((int)data);
	}
	else
		throw ModelException("NitrificationAndAmmoniaVolatilization", "SetSingleData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
	
}

void NitrificationAndAmmoniaVolatilization::Set1DData(const char* key, int n, float* data)
{
	//check the input data
	CheckInputSize(key,n);
	string sk(key);
	//if (StringMatch(sk, "RootDepth"))
	//	m_rootDepth = data;
	if(StringMatch(sk, "D_SOTE"))
		m_SoilT = data;
	else
		throw ModelException("NitVol", "Set1DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
	
}

void NitrificationAndAmmoniaVolatilization::Set2DData(const char* key, int nrows, int ncols, float** data)
{
	//check the input data
	CheckInputSize(key, ncols);

	string sk(key);
	if(StringMatch(sk, "D_Nitrate"))
		m_Nitrate = data;
	else if(StringMatch(sk,"D_SOMO_2D"))		
		m_SOMO = data;
	else if(StringMatch(sk, "D_Ammon"))
		m_Ammon = data;
	else if(StringMatch(sk, "WiltingPoint_2D"))
		m_Wiltingpoint = data;
	else if (StringMatch(sk, "FieldCap_2D"))
		m_FieldCap = data;
	else if(StringMatch(sk, "D_Depth"))
		m_Depth = data;
	else
		throw ModelException("DissolvedNutrient_OL", "Set2DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");
}

void NitrificationAndAmmoniaVolatilization::Get2DData(const char* key, int *nRows, int *nCols, float*** data)
{
	string sk(key);
	*nRows = m_nLayers;
	*nCols = m_size;
	if (StringMatch(sk, "NitvolTF"))
		*data = m_nitvolTF; 
	else if (StringMatch(sk, "MidZ"))
		*data = m_midZ; 
	else if (StringMatch(sk, "NitWF"))
		*data = m_nitWF;
	else if (StringMatch(sk, "VolDF"))
		*data = m_volDF;
	else if (StringMatch(sk, "NitR"))
		*data = m_nitR;
	else if (StringMatch(sk, "VolR"))
		*data = m_volR;
	else if (StringMatch(sk, "NitvolAmmon"))
		*data = m_nitvolAmmon;
	else if (StringMatch(sk, "NitLostF"))
		*data = m_nitLostF;
	else if (StringMatch(sk, "VolLostF"))
		*data = m_volLostF;
	else if (StringMatch(sk, "ConNitra"))
		*data = m_conNitra;
	else if (StringMatch(sk, "ConAmm"))
		*data = m_conAmm;
	else
		throw ModelException("AtmosphericDeposition", "Get2DData", "Output " + sk 
		+ " does not exist in the Atmospheric Deposition module. Please contact the module developer.");
}

int NitrificationAndAmmoniaVolatilization::Execute()
{
	//check the data
	CheckInputData();	

	initalOutputs();

	#pragma omp parallel for
	for(int i=0; i < m_nLayers; i++)
    {
		for(int j=0; j < m_size; j++)
		{
			if (m_SoilT[j] < 5)
			continue;

		    m_nitvolTF[i][j] = 0.41f * (m_SoilT[j] - 5) / 10;
		 
			int hydroIndex = max(0, i-1);
		    float r = 0.25f * m_FieldCap[hydroIndex][j] - 0.75f * m_Wiltingpoint[hydroIndex][j];
		    if (m_SOMO[hydroIndex][j] < r)
				m_nitWF[i][j] = (m_SOMO[hydroIndex][j]- m_Wiltingpoint[hydroIndex][j]) / (0.25f * (m_FieldCap[hydroIndex][j] - m_Wiltingpoint[hydroIndex][j]));
		    else
			    m_nitWF[i][j] = 1.0f;
		 
		    m_midZ[0][j] = m_Depth[0][j] / 2;
			m_midZ[1][j] = (m_Depth[1][j] - m_Depth[0][j]) / 2;
			m_midZ[2][j] = (m_Depth[2][j] - m_Depth[1][j]) / 2;
		    
			m_volDF[i][j] = 1 - m_midZ[i][j] / (m_midZ[i][j] + exp(4.706f - 0.0305f * m_midZ[i][j]));
		    float m_catEF = 0.15f;
		    m_nitR[i][j] = m_nitvolTF[i][j] * m_nitWF[i][j];
		    m_volR[i][j] = m_nitvolTF[i][j] * m_volDF[i][j] * m_catEF;
		    m_nitvolAmmon[i][j] = m_Ammon[i][j] * (1 - exp(- m_nitR[i][j] - m_volR[i][j]));
		    m_nitLostF[i][j] = 1 - exp(- m_nitR[i][j]);
		    m_volLostF[i][j] = 1 - exp(- m_volR[i][j]);
		    m_conNitra[i][j] = m_nitvolAmmon[i][j] * m_nitLostF[i][j] / (m_nitLostF[i][j] + m_volLostF[i][j]);
		    m_conAmm[i][j] = m_nitvolAmmon[i][j] * m_volLostF[i][j] / (m_nitLostF[i][j] + m_volLostF[i][j]);

		    m_Nitrate[i][j] += m_conNitra[i][j];
		    m_Ammon[i][j] -= m_nitvolAmmon[i][j]; 
		}
	}

	return 0;
}