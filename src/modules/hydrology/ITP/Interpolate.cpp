#include <cmath>
#include "Interpolate.h"
#include "MetadataInfo.h"
#include "util.h"
#include "ModelException.h"
#include <sstream>
#include <omp.h>
#include <iostream>


using namespace std;


Interpolate::Interpolate():m_nCells(-1), m_nStatioins(-1),
	m_month(-1), m_output(NULL), m_stationData(NULL), m_weights(NULL), 
	m_dem(NULL),m_hStations(NULL), m_lapseRate(NULL), m_vertical(false), m_dataType(0)
{
	//omp_set_num_threads(1);
	//ofs.open("c:\\staiondata.txt");
}

void Interpolate::SetDataType(float value)
{
	if(FloatEqual(value, 1.0f))
		m_dataType = 0;
	else if(FloatEqual(value, 2.0f) || FloatEqual(value, 3.0f)) 
		m_dataType = 1;
	else if(FloatEqual(value, 4.0f)) 
		m_dataType = 2;
}

Interpolate::~Interpolate(void)
{
	if (m_output != NULL)
		delete[] m_output;
	//ofs.close();
}
;
int Interpolate::Execute()
{
	if (m_output == NULL)
	{
		CheckInputData();

		m_output = new float[m_nCells];

		#pragma omp parallel for
		for (int i = 0; i < m_nCells; ++i)
			m_output[i] = 0.f;
	}

	//for (int j = 0; j < m_nStatioins; ++j)
	//	ofs << m_stationData[j] << "\t";
	//ofs << endl;

	//this->StatusMsg("execute ITP...");
	#pragma omp parallel for
	for (int i = 0; i < m_nCells; ++i)
	{
		float value = 0.f;
		int index = 0;
		for (int j = 0; j < m_nStatioins; ++j)
		{
			index = i*m_nStatioins + j;
			
			value += m_stationData[j] * m_weights[index];
			//cout << m_weights[index] << endl;
			if (m_vertical)
			{
				float delta = m_dem[i] - m_hStations[j];
				float factor = m_lapseRate[m_month][m_dataType];
				float adjust =  m_weights[index] * (m_dem[i] - m_hStations[j]) * m_lapseRate[m_month][m_dataType] / 100.0f;
				value += adjust; 
			}
		}
		m_output[i] = value;
	}
	//Output1DArray(m_nCells, m_output, "F:\\p.txt");
	
	//this->StatusMsg("end execute ITP...");

	return true;
}


void Interpolate::SetDate(time_t date) 
{ 
	m_date = date; 
	struct tm t;
#ifndef linux
	localtime_s(&t, &m_date);
#else
    localtime_r(&m_date, &t);
#endif
	m_month = t.tm_mon;
}

void Interpolate::SetValue(const char* key, float value)
{
	string sk(key);
	
	if (StringMatch(sk, VAR_OMP_THREADNUM))
	{
		omp_set_num_threads((int)value);
	}
	else if(StringMatch(sk, "Data_Type"))
	{
		if(value == 1.0f) m_dataType = 0;
		if(value == 2.0f || value == 3.0f) m_dataType = 1;
		if(value == 4.0f) m_dataType = 2;
		//m_dataType = int(value); 
	}
	else if (StringMatch(sk, "VerticalInterpolation"))
	{
		if (value > 0)
			m_vertical = true;
		else
			m_vertical = false;
	}
	else
	{
		throw ModelException("ITP", "SetValue", "Parameter " + sk 
			+ " does not exist in the Interpolate module. Please contact the module developer.");
	}
	
}

void Interpolate::Set2DData(const char *key, int nRows, int nCols, float **data)
{
	string sk(key);

	if (StringMatch(sk, "LapseRate"))
	{
		if(m_vertical)
		{
			int nMonth = 12;
			CheckInputSize(sk, nRows, nMonth);
			m_lapseRate = data;
		}
	}
	else
	{
		throw ModelException("ITP", "Set2DData", "Parameter " + sk 
			+ " does not exist in the Interpolate module. Please contact the module developer.");
	}
	
}

void Interpolate::Set1DData(const char* key, int n, float* data)
{
	string sk(key);
	
	if(StringMatch(sk, "DEM"))
	{
		if(m_vertical)
		{
			CheckInputSize(sk, n, m_nCells);
			m_dem = data;
		}
	}
	else if(StringMatch(sk, "Weight"))
	{
		CheckInputSize(sk, n, m_nCells);
		m_weights = data;
	}
	else if (StringMatch(sk, "StationElevation_P") || StringMatch(sk, "StationElevation_M")
		|| StringMatch(sk, "StationElevation_T") || StringMatch(sk, "StationElevation_PET"))
	{
		if(m_vertical)
		{
			CheckInputSize(sk, n, m_nStatioins);
			m_hStations = data;
		}
	}
	else
	{
		string prefix = sk.substr(0,1);
		if (prefix == "T" || prefix == "t")
		{
			CheckInputSize(sk, n, m_nStatioins);
			this->m_stationData = data;
			//for (int i = 0; i < n; i++)
			//	cout << m_stationData[i] << "\t";
			//cout << endl;
		}
		else
		{
			throw ModelException("ITP", "Set1DData", "Parameter " + sk 
				+ " does not exist in the Interpolate module. Please contact the module developer.");
		}
	}
}

bool Interpolate::CheckInputSize(string& key, int n, int& m_n)
{
	if(n <= 0)
	{
		throw ModelException("ITP","CheckInputSize","Input data for " + key 
			+" is invalid. The size could not be less than zero.");
	}
	if(n != m_n)
	{
		if(m_n <=0)
			m_n = n;
		else
		{
			ostringstream oss;
			oss << n;
			throw ModelException("ITP","CheckInputSize",
				"Input data for "+ key +" is invalid." + " The size of input data is " + 
				oss.str()+
				". The number of columns in weight file and the number of stations should be same.");
		}
	}

	return true;
}

void Interpolate::CheckInputData()
{
	if(m_dataType < 0)
		throw ModelException("ITP","CheckInputData","The parameter: DataType has not been set.");
	if(m_month < 0)
		throw ModelException("ITP","CheckInputData","The date has not been set.");
	if(m_weights == NULL)
		throw ModelException("ITP","CheckInputData","The parameter: Weight has not been set.");
	if(m_vertical)
	{
		if(m_lapseRate == NULL)
			throw ModelException("ITP","CheckInputData","The parameter: LapseRate has not been set.");
		if(m_dem == NULL)
			throw ModelException("ITP","CheckInputData","The parameter: DEM has not been set.");
		if(m_hStations == NULL)
			throw ModelException("ITP","CheckInputData","The parameter: StaionElevation have not been set.");
	}
	if(m_stationData == NULL)
		throw ModelException("ITP","CheckInputData","The parameter: T has not been set.");

}

void Interpolate::Get1DData(const char* key, int* n, float** data)
{
	string sk(key);
	
	if(StringMatch(sk, "DEM"))
	{
		*n = m_nCells;
		*data = m_dem;
	}
	else if (StringMatch(sk, "StaionElevation"))
	{
		*n = m_nStatioins;
		*data = m_hStations;
	}
	else if (StringMatch(sk, "T"))
	{
		*n = m_nStatioins;
		*data = m_stationData;
	}
	else
	{
		//throw ModelException("ITP", "Get1DData", "Parameter " + sk 
		//	+ " does not exist in the Interpolate module. Please contact the module developer.");
		*n = m_nCells;
		*data = m_output;

		//cout << m_output[0] << "\t";
	}
	
}
