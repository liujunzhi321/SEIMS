/*!
 * \file clsTSD_RD.cpp
 * \author Zhiqiang Yu
 * \date Apr. 2010
 *
 * 
 */
#include "api.h"
#include <iostream>
#include "clsTSD_RD.h"
#include "MetadataInfo.h"
#include "ModelException.h"

using namespace std;

clsTSD_RD::clsTSD_RD(void)
{
    this->m_Rows = -1;
    this->m_Data = NULL;
}


clsTSD_RD::~clsTSD_RD(void)
{
}

void clsTSD_RD::Set1DData(const char *key, int n, float *data)
{
    this->m_Rows = n;
    this->m_Data = data;
    /// Test Code of Reading time series data
    //for (int i = 0; i < n; i++)
    //	cout << key << ": " << data[i] << " ";
    //cout<<endl;
}

void clsTSD_RD::Get1DData(const char *key, int *n, float **data)
{
    string sk(key);
    if (this->m_Rows == -1 || this->m_Data == NULL)
    {
        throw ModelException(MID_TSD_RD, "GetData", "The data " + string(key) + " is NULL.");
    }
    *data = this->m_Data;
    *n = this->m_Rows;
    //cout<<"TSD_RD: ";
    //for(int i = 0; i < m_Rows; i++)
    //	cout << m_Data[i]<<",";
    //cout<<endl;
}
