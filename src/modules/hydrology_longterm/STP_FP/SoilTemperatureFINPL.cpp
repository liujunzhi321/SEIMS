#include "SoilTemperatureFINPL.h"
#include "MetadataInfo.h"
#include "util.h"
#include "ModelException.h"
#include <cmath>

#include <omp.h>

using namespace std;

SoilTemperatureFINPL::SoilTemperatureFINPL(void):m_a0(NODATA),m_a1(NODATA), m_a2(NODATA), m_a3(NODATA),
	m_b1(NODATA), m_b2(NODATA), m_d1(NODATA), m_d2(NODATA), m_kSoil10(NODATA), m_date(-1), m_julianDay(-1), m_size(-1),
	m_relativeFactor(NULL), m_soilTemp(NULL), m_tMax(NULL), m_tMin(NULL), m_t1(NULL),m_t2(NULL)
{
	w = 3.1415926f*2/365;
}

SoilTemperatureFINPL::~SoilTemperatureFINPL(void)
{
	if (m_soilTemp != NULL)
	{
		delete []m_soilTemp;
	}
	if (m_t1 != NULL)
	{
		delete []m_t1;
	}
	if (m_t2 != NULL)
	{
		delete []m_t2;
	}
}

int SoilTemperatureFINPL::Execute()
{
	//check the data
	CheckInputData();	
	if(this->m_soilTemp == NULL)
	{
		this->m_soilTemp = new float[this->m_size];
		this->m_t1 = new float[this->m_size];
		this->m_t2 = new float[this->m_size];

		#pragma omp parallel for
		for (int i=0; i<m_size; ++i)
		{
			m_t1[i] = (m_tMax[i] + m_tMin[i])/2;
			m_t2[i] = m_t1[i];
		}		
	}

	#pragma omp parallel for
	for (int i=0; i<m_size; ++i)
	{
		float t = (m_tMax[i] + m_tMin[i])/2;
		float t1 = m_t1[i];
		float t2 = m_t2[i];
		float t10 = m_a0 + m_a1*t2 + m_a2*t1 + m_a3*t 
			+ m_b1*sin(w*m_julianDay) + m_d1*cos(w*m_julianDay) 
			+ m_b2*sin(2*w*m_julianDay) + m_d2*cos(2*w*m_julianDay);
		m_soilTemp[i] = t10*m_kSoil10 + m_relativeFactor[i];
		if (m_soilTemp[i] > 60.0f || m_soilTemp[i] < -90.0f)
		{
			ostringstream oss;
			oss << "The calculated soil temperature at cell (" << i
			<< ") is out of reasonable range, whose value is " << m_soilTemp[i] << ".\nJulianDay: " 
			<< m_julianDay << endl;
			throw ModelException("STP_FP","Execute", oss.str());
		}

		//save the temperature
		m_t2[i] = m_t1[i];
		m_t1[i] = t;
	}
	
	return 0;
}

bool SoilTemperatureFINPL::CheckInputData(void)
{
	if(FloatEqual(m_a0, NODATA))
		throw ModelException("STP_FP","CheckInputData","The parameter: SoilTa0 has not been set.");
	if(FloatEqual(m_a1, NODATA))
		throw ModelException("STP_FP","CheckInputData","The parameter: SoilTa1 has not been set.");
	if(FloatEqual(m_a2, NODATA))
		throw ModelException("STP_FP","CheckInputData","The parameter: SoilTa2 has not been set.");
	if(FloatEqual(m_a3, NODATA))
		throw ModelException("STP_FP","CheckInputData","The parameter: SoilTa3 has not been set.");
	if(FloatEqual(m_b1, NODATA))
		throw ModelException("STP_FP","CheckInputData","The parameter: SoilTb1 has not been set.");
	if(FloatEqual(m_b2, NODATA))
		throw ModelException("STP_FP","CheckInputData","The parameter: SoilTb2 has not been set.");
	if(FloatEqual(m_d1, NODATA))
		throw ModelException("STP_FP","CheckInputData","The parameter: SoilTd1 has not been set.");
	if(FloatEqual(m_d2, NODATA))
		throw ModelException("STP_FP","CheckInputData","The parameter: SoilTd2 has not been set.");
	if(FloatEqual(m_kSoil10, NODATA))
		throw ModelException("STP_FP","CheckInputData","The parameter: KSoil10 has not been set.");
	if(m_date < 0)
		throw ModelException("STP_FP","CheckInputData","The date has not been set.");
	if(m_relativeFactor == NULL)
		throw ModelException("STP_FP","CheckInputData","The parameter: LandcoverFactor has not been set.");
	if(m_tMin == NULL)
		throw ModelException("STP_FP","CheckInputData","The parameter: Tmin has not been set.");
	if(m_tMax == NULL)
		throw ModelException("STP_FP","CheckInputData","The parameter: Tmax has not been set.");
	//if(m_tMin1 == NULL)
	//	throw ModelException("STP_FP","CheckInputData","The parameter: Tmin1 has not been set.");
	//if(m_tMax1 == NULL)
	//	throw ModelException("STP_FP","CheckInputData","The parameter: Tmax1 has not been set.");
	//if(m_tMin2 == NULL)
	//	throw ModelException("STP_FP","CheckInputData","The parameter: Tmin2 has not been set.");
	//if(m_tMax2 == NULL)
	//	throw ModelException("STP_FP","CheckInputData","The parameter: Tmax2 has not been set.");
	//if(m_mask == NULL)
	//	throw ModelException("STP_FP","CheckInputData","The parameter: Mask has not been set.");
	return true;
}

bool SoilTemperatureFINPL::CheckInputSize(const char* key, int n)
{
	if(n <= 0)
	{
		return false;
	}
	if(this->m_size != n)
	{
		if(this->m_size <=0) this->m_size = n;
		else
		{
			return false;
		}
	}

	return true;
}

void SoilTemperatureFINPL::SetValue(const char* key, float value)
{
	//set the value
	string sk(key);
	if (StringMatch(sk, "ThreadNum"))
	{
		omp_set_num_threads((int)value);
	}
	else if (StringMatch(sk, "soil_ta0"))
		m_a0 = value;
	else if (StringMatch(sk, "soil_ta1"))
		m_a1 = value;
	else if (StringMatch(sk, "soil_ta2"))
		m_a2 = value;
	else if (StringMatch(sk, "soil_ta3"))
		m_a3 = value;
	else if (StringMatch(sk, "soil_tb1"))
		m_b1 = value;
	else if (StringMatch(sk, "soil_tb2"))
		m_b2 = value;
	else if (StringMatch(sk, "soil_td1"))
		m_d1 = value;
	else if (StringMatch(sk, "soil_td2"))
		m_d2 = value;
	else if (StringMatch(sk, "k_soil10"))
		m_kSoil10 = value;

}

void SoilTemperatureFINPL::Set1DData(const char* key, int n, float* data)
{
	//check the input data
	CheckInputSize(key,n);
	string sk(key);
	if (StringMatch(sk, "SOIL_T10"))
		m_relativeFactor = data;
	else if(StringMatch(sk, "D_Tmin"))
		m_tMin = data;
	else if (StringMatch(sk, "D_Tmax"))
		m_tMax = data;
	//else if(StringMatch(sk, "Tmin1"))
	//	m_tMin1 = data;
	//else if (StringMatch(sk, "Tmax1"))
	//	m_tMax1 = data;
	//else if(StringMatch(sk, "Tmin2"))
	//	m_tMin2 = data;
	//else if (StringMatch(sk, "Tmax2"))
	//	m_tMax2 = data;
	else
		throw ModelException("STP_FP", "Set1DData", "Parameter " + sk 
		+ " does not exist in the STP_FP module. Please contact the module developer.");

}

void SoilTemperatureFINPL::Get1DData(const char* key, int* n, float** data)
{
	string sk(key);
	*n = m_size;
	*data = m_soilTemp;
	//if (StringMatch(sk, "D_SOTE"))
	//	*data = m_soilTemp;
	//else if (StringMatch(sk, "SOIL_T10"))
	//	*data = m_relativeFactor;
	//else if(StringMatch(sk, "Tmin"))
	//	*data = m_tMin;
	//else if (StringMatch(sk, "Tmax"))
	//	*data = m_tMax;
	//else if(StringMatch(sk, "Tmin1"))
	//	*data = m_tMin1;
	//else if (StringMatch(sk, "Tmax1"))
	//	*data = m_tMax1;
	//else if(StringMatch(sk, "Tmin2"))
	//	*data = m_tMin2;
	//else if (StringMatch(sk, "Tmax2"))
	//	*data = m_tMax2;
	//else
	//	throw ModelException("STP_FP", "Get1DData", "Parameter " + sk 
	//	+ " does not exist in the STP_FP module. Please contact the module developer.");

}

void SoilTemperatureFINPL::Set2DData(const char *key, int nRows, int nCols, float **data)
{
	//CheckInputSize(key, nRows);
	//string sk(key);
	//if (StringMatch(sk, "Mask"))
	//	m_mask = data;
	//else
	//	throw ModelException("STP_FP", "Set2DData", "Parameter " + sk 
	//	+ " does not exist in the STP_FP module. Please contact the module developer.");
}

void SoilTemperatureFINPL::SetDate(time_t t)
{ 
	m_date = t; 
	struct tm tmStruct;
	LocalTime(t, &tmStruct);
	m_julianDay = tmStruct.tm_yday + 1;

}