#include <cmath>
#include <iostream>
#include "SUR_CN.h"
#include "MetadataInfo.h"
#include "ModelException.h"
#include "util.h"

#include <omp.h>

SUR_CN::SUR_CN(void):m_nCells(-1),m_porosity(NULL),m_rootDepth(NULL),m_CN2(NULL),m_fieldCap(NULL),m_wiltingPoint(NULL),
	m_soilMoisture(NULL),m_SD(NULL),m_tMin(NULL),m_tMax(NULL),m_Tsnow(NODATA),m_Tsoil(NODATA),
	m_Sfrozen(NODATA),m_T0(NODATA),m_SM(NULL),m_SA(NULL),m_TS(NULL),m_julianDay(-1),m_INFIL(NULL),m_PE(NULL), 
	m_w1(NULL),m_w2(NULL),m_sMax(NULL), m_initSoilMoisture(NULL)
{
	m_depth[0] = 10.f;
	m_depth[1] = 90.f;
}

SUR_CN::~SUR_CN(void)
{
	//// cleanup
	if (m_PE != NULL)
	{
		delete [] m_PE;
	}
	if (m_INFIL != NULL)
	{
		delete [] m_INFIL;
	}
	if (m_soilMoisture != NULL)
	{
		delete m_soilMoisture;
	}
	if(m_w1 != NULL) delete [] m_w1;
	if(m_w2 != NULL) delete [] m_w2;
	if(m_sMax != NULL) delete [] m_sMax;
}

bool SUR_CN::CheckInputData(void)
{
	if(m_date < 0)
	{
		throw ModelException("SUR_CN","CheckInputData","You have not set the time.");
		return false;
	}

	if(m_nCells <= 0)
	{
		throw ModelException("SUR_CN","CheckInputData","The cell number of the input can not be less than zero.");
		return false;
	}

	if (m_porosity == NULL)
	{
		throw ModelException("SUR_CN","CheckInputData","The soil porosity of the input data can not be NULL.");
		return false;
	}

	if (m_rootDepth == NULL)
	{
		throw ModelException("SUR_CN","CheckInputData","The root depth of the input data can not be NULL.");
		return false;
	}

	if (m_CN2 == NULL)
	{
		throw ModelException("SUR_CN","CheckInputData","The CN under moisture condition II of the input data can not be NULL.");
		return false;
	}

	if (m_P_NET == NULL)
	{
		throw ModelException("SUR_CN","CheckInputData","The net precipitation of the input data can not be NULL.");
		return false;
	}

	if (m_fieldCap == NULL)
	{
		throw ModelException("SUR_CN","CheckInputData","The water content of soil at field capacity of the input data can not be NULL.");
		return false;
	}

	if (m_wiltingPoint == NULL)
	{
		throw ModelException("SUR_CN","CheckInputData","The plant wilting point moisture of the input data can not be NULL.");
		return false;
	}

	if (m_initSoilMoisture == NULL)
	{
		throw ModelException("SUR_CN","CheckInputData","The initial soil moisture or soil moisture of the input data can not be NULL.");
		return false;
	}

	if (FloatEqual(m_Sfrozen, NODATA))
	{
		throw ModelException("SUR_CN","CheckInputData","The frozen soil moisture of the input data can not be NULL.");
		return false;
	}

	if (m_tMax == NULL)
	{
		throw ModelException("SUR_CN","CheckInputData","The maximum temperature of the input data can not be NULL.");
		return false;
	}

	if (m_tMin == NULL)
	{
		throw ModelException("SUR_CN","CheckInputData","The minimum temperature of the input data can not be NULL.");
		return false;
	}

	if (FloatEqual(m_Tsnow, NODATA))
	{
		throw ModelException("SUR_CN","CheckInputData","The snowfall temperature of the input data can not be NULL.");
		return false;
	}

	if (FloatEqual(m_Tsoil, NODATA))
	{
		throw ModelException("SUR_CN","CheckInputData","The soil freezing temperature of the input data can not be NULL.");
		return false;
	}

	if (FloatEqual(m_T0, NODATA))
	{
		throw ModelException("SUR_CN","CheckInputData","The snowmelt threshold temperature of the input data can not be NULL.");
		return false;
	}

	if (m_TS == NULL)
	{
		throw ModelException("SUR_CN","CheckInputData","The soil temperature of the input data can not be NULL.");
		return false;
	}

	return true;
}

void SUR_CN::initalOutputs()
{
	if(m_nCells <= 0)				
		throw ModelException("SUR_CN","CheckInputData","The dimension of the input data can not be less than zero.");
	// allocate the output variables
	if (m_PE == NULL)
	{
		m_PE = new float[m_nCells];
		m_INFIL = new float[m_nCells];

		m_soilMoisture = new float*[m_nCells];
#pragma omp parallel for
		for (int i = 0; i < m_nCells; i++)
		{
			m_PE[i] = 0.0f;
			m_INFIL[i] = 0.0f;
            
            m_soilMoisture[i] = new float[m_nLayers];
            for (int j = 0; j < m_nLayers; j++)
                m_soilMoisture[i][j] = m_initSoilMoisture[i] * m_fieldCap[i][j];
		}

		initalW1W2();
	}
}

int SUR_CN::Execute()
{
	CheckInputData();
	initalOutputs();

	float cnday;
	float pNet, surfq, infil;

	#pragma omp parallel for
	for (int iCell = 0; iCell < m_nCells; iCell++)
	{
		//initialize the variables
		surfq = 0.0f;
		infil = 0.0f; 
		pNet = 0.0f;
		float pcp = m_P_NET[iCell];  //rainfall - interception
		float dep = 0.0f;  //depression storage
		float snm = 0.0f;  //snowmelt
		float t = 0.0f;  // air temperature

		//set values to variables
		if (m_SD == NULL)        // the depression storage module is not available, set the initial depression storage
		{
			//dep = m_Depre_in * m_Depression[iCell];
			dep = 0.f;
		}
		else
		{
			dep = m_SD[iCell];
		}

		if (m_SM == NULL)
			snm = 0.f;
		else
			snm = m_SM[iCell];

		t = (m_tMin[iCell] + m_tMax[iCell]) / 2;

		//account for the effects of snowmelt and soil temperature
		// snow, without snow melt
		if (t <= m_Tsnow)
		{
			pNet = 0.0f;
			//m_SA[iCell] += pcp;
		}
		// rain on snow, no snow melt
		else if (t > m_Tsnow && t <= m_T0 && m_SA != NULL && m_SA[iCell] > pcp)
		{
			pNet = 0.0f;
			//m_SA[iCell] += pcp;
		}
		else
		{
			pNet = pcp + dep + snm;    // default
		}

		if(pNet > 0.0f)
		{
			float sm = 0.f;
			float por = 0.f;
			float aboveDepth = 0.f;
			for (int j = 0; j < m_nLayers - 1; j++)
			{
				sm += m_soilMoisture[iCell][j]*m_depth[j];
				por += m_porosity[iCell][j]*m_depth[j];
				aboveDepth += m_depth[j];
			}
			//sm += m_soilMoisture[i][m_nLayers - 1] * (m_rootDepth[i] - aboveDepth);
			//por += m_porosity[i][m_nLayers - 1] * (m_rootDepth[i] - aboveDepth);

			sm /= por;
			sm = min(sm, 1.0f);

			//for frozen soil
			if (m_TS[iCell] <= m_Tsoil && sm >= m_Sfrozen*por)
			{
				m_PE[iCell] = pcp + snm;
				m_INFIL[iCell] = 0.0f;
			}
			//for saturation overland flow
			else if (sm > por)
			{
				m_PE[iCell] = pcp + snm;
				m_INFIL[iCell] = 0.0f;
			}
			//for CN method
			else
			{
				float sw,s,xx;

				s = 0.;
				sw = sm * m_rootDepth[iCell];       
				xx = m_w1[iCell] - m_w2[iCell] * sw;
				if (xx < -20.f)
					xx = -20.f;
				else if (xx > 20.f)
					xx = 20.f;
				
				// traditional CN method (function of soil water)
				float tmp = sw + exp(xx);
				if (tmp > 0.001f)
				{
					s = m_sMax[iCell] * (1.f - sw /tmp );  //2:1.1.6
				}

				cnday = 25400.f / (s + 254.f);  //2:1.1.11

				//cnday = Calculate_CN(sm, iCell);
				float bb, pb;
				s = 0.0f;
				bb = 0.0f;
				pb = 0.0f;
				s = 25400.0f/cnday -254.0f;
				bb=0.2f * s;
				pb = pNet - bb;		
				// calculate infiltration and excess rainfall
				if (pb > 0.0f)
					surfq = pb * pb / (pNet + 0.8f * s);

				if (surfq < 0.0f) 
					surfq = 0.0f;

				infil = pNet - surfq;

				// set output value
				m_INFIL[iCell] = infil;
				m_PE[iCell] = pcp + snm - infil;	//excess precipitation could be negative
			}

			if(m_INFIL[iCell] < 0)
			{
				m_INFIL[iCell] = 0.f;
				m_PE[iCell] = pcp + snm - m_INFIL[iCell];
			}

			// check the output data
			if (m_INFIL[iCell] != m_INFIL[iCell] || m_INFIL[iCell] < 0.0f)
			{
				cout << m_INFIL[iCell] << endl;
				throw ModelException("SUR_MR","Execute","Output data error: infiltration is less than zero. Where:\n Please contact the module developer. ");
			}
			
		}
		else
		{
			m_PE[iCell] = 0.0f;
			m_INFIL[iCell] = 0.0f;
		}

	}
	return 0;
}

bool SUR_CN::CheckInputSize(const char* key, int n)
{
	if(n<=0)
	{
		throw ModelException("SUR_CN","CheckInputSize","Input data for "+string(key) +" is invalid. The size could not be less than zero.");
		return false;
	}
	if(m_nCells != n)
	{
		if(m_nCells <=0) m_nCells = n;
		else
		{
			throw ModelException("SUR_CN","CheckInputSize","Input data for "+string(key) +" is invalid. All the input data should have same size.");
			return false;
		}
	}

	return true;
}

void SUR_CN::SetValue(const char* key, float value)
{
	string sk(key);
	if (StringMatch(sk, "ThreadNum"))
	{
		omp_set_num_threads((int)value);
	}
	else if (StringMatch(sk,"T_snow"))
	{
		m_Tsnow = value;
	}
	else if (StringMatch(sk,"t_soil"))
	{
		m_Tsoil = value;
	}
	else if (StringMatch(sk,"T0"))
	{
		m_T0 = value;
	}
	else if (StringMatch(sk,"s_frozen"))
	{
		m_Sfrozen = value;
	}
	else									throw ModelException("SUR_CN","SetValue","Parameter " + sk 
		+ " does not exist in SUR_CN method. Please contact the module developer.");
	
}

void SUR_CN::Set1DData(const char* key, int n, float* data)
{

	CheckInputSize(key,n);

	//set the value
	string sk(key);

	if(StringMatch(sk,"rootDepth"))
	{
		m_rootDepth = data;
	}
	else if (StringMatch(sk,"cn2"))
	{
		m_CN2 = data;
	}
	else if (StringMatch(sk,"D_NEPR"))
	{
		m_P_NET = data;
	}
	else if (StringMatch(sk,"Moist_in"))
	{
		m_initSoilMoisture = data;
	}
	//else if (StringMatch(sk,"S_M_frozen"))
	//{
	//	m_S_M_frozen = data;
	//}
	//else if (StringMatch(sk,"depression"))
	//{
	//	m_Depression = data;
	//}
	else if (StringMatch(sk,"D_DPST"))  //depression storage 
	{
		m_SD = data;
	}
	else if (StringMatch(sk,"D_TMin"))
	{
		m_tMin = data;
	}
	else if (StringMatch(sk,"D_TMax"))
	{
		m_tMax = data;
	}
	else if (StringMatch(sk,"D_SNME"))
	{
		m_SM = data;
	}
	else if (StringMatch(sk,"D_SNAC"))
	{
		m_SA = data;
	}
	else if (StringMatch(sk,"D_SOTE"))
	{
		m_TS = data;
	}
	else									
		throw ModelException("SUR_CN","SetValue","Parameter " + sk + 
		" does not exist in SUR_CN method. Please contact the module developer.");

}


void SUR_CN::Set2DData(const char* key, int nrows, int ncols, float** data)
{
	string sk(key);
	CheckInputSize(key, nrows);
	m_nLayers = ncols;

	if (StringMatch(sk, "porosity_2D"))
	{
		m_porosity = data;
	}
	else if (StringMatch(sk,"fieldCap_2D"))
	{
		m_fieldCap = data;
	}
	else if (StringMatch(sk,"Wiltingpoint_2D"))
	{
		m_wiltingPoint = data;
	}
	else
		throw ModelException("SUR_MR", "Set1DData", "Parameter " + sk 
		+ " does not exist. Please contact the module developer.");

}

void SUR_CN::Get1DData(const char* key, int* n, float** data)
{
	string sk(key);
	
	if (StringMatch(sk,"INFIL"))
	{
		*data = m_INFIL;
	}
	else if (StringMatch(sk,"EXCP"))
	{
		*data = m_PE;
	}
	else									
        throw ModelException("SUR_CN", "Get1DData", 
            "Result " + sk + " does not exist in SUR_CN method. Please contact the module developer.");

	*n = m_nCells;
}

void SUR_CN::Get2DData(const char* key, int *nRows, int *nCols, float*** data)
{
    string sk(key);
    *nRows = m_nCells;
    *nCols = m_nLayers;

    if (StringMatch(sk, "SOMO_2D"))   // excess precipitation
    {
        *data = m_soilMoisture;
    }
    else
        throw ModelException("SUR_CN", "Get2DData", "Output " + sk
        + " does not exist. Please contact the module developer.");

}

float SUR_CN::Calculate_CN(float sm, int cell)
{
	float sw,s,CNday,xx;

	s = 0.;
	sw = sm * m_rootDepth[cell];       
	xx = m_w1[cell] - m_w2[cell] * sw;
	if (xx < -20.f)
	{
		xx = -20.f;
	}
	if (xx > 20.f)
	{
		xx = 20.f;
	}
	// traditional CN method (function of soil water)
	if ((sw + exp(xx)) > 0.001f)
	{
		s = m_sMax[cell] * (1.f - sw /(sw + exp(xx)) );  //2:1.1.6
	}

	CNday = 25400.f / (s + 254.f);  //2:1.1.11
	return CNday;
}

void SUR_CN::initalW1W2()
{
	m_w1 = new float[m_nCells];
	m_w2 = new float[m_nCells];
	m_sMax = new float[m_nCells];

	for(int i=0;i<m_nCells;i++)
	{
        float fieldcap = 0.f;
        float wsat = 0.f;
        float aboveDepth = 0.f;
        for (int j = 0; j < m_nLayers-1; j++)
        {
            fieldcap += m_fieldCap[i][j] * m_depth[j];
            wsat += m_porosity[i][j] * m_depth[j];
            aboveDepth += m_depth[j];
        }
        fieldcap += m_fieldCap[i][m_nLayers - 1] * (m_rootDepth[i] - aboveDepth);
        wsat += m_porosity[i][m_nLayers - 1] * (m_rootDepth[i] - aboveDepth);

		float cnn = m_CN2[i];
		//float fieldcap = m_fieldCap[i] * m_rootDepth[i];
		//float wsat = m_porosity[i] * m_rootDepth[i];
		float c1,c3,c2,smx,s3,rto3,rtos,xx,wrt1,wrt2;
		c2 = 100.0f - cnn;
		c1 = cnn - 20.f * c2 / (c2 + exp(2.533f - 0.0636f * c2));	//CN1  2:1.1.4
		c1 = max( c1, 0.4f * cnn);
		c3 = cnn * exp(0.006729f * c2);								//CN3  2:1.1.5

		//calculate maximum retention parameter value
		smx = 254.f * (100.f / c1 - 1.f);							//2:1.1.2

		//calculate retention parameter value for CN3
		s3 = 254.f * (100.f / c3 - 1.f);

		rto3 = 1.f - s3 / smx;
		rtos = 1.f - 2.54f / smx;

		xx = log(fieldcap / rto3 - fieldcap);
		wrt2 = (xx - log(wsat / rtos - wsat)) / (wsat - fieldcap);    //soilWater :completely saturated  (= soil_por - sol_wp)  w1  2:1.1.8
		wrt1 = xx + (fieldcap * wrt2); //w2 2:1.1.7

		m_w1[i] = wrt1;
		m_w2[i] = wrt2;
		m_sMax[i] = smx;
	}
}
