/*!
 * \file clsTSD_RD.cpp
 * \ingroup TSD_RD
 * \author Zhiqiang Yu
 * \date Apr. 2010
 *
 * 
 */
#include "api.h"
#include "clsTSD_RD.h"
#include "MetadataInfo.h"
#include "ModelException.h"

clsTSD_RD::clsTSD_RD(void)
{
	this->m_Rows = -1;
	this->m_Data = NULL;
}


clsTSD_RD::~clsTSD_RD(void)
{
}

void clsTSD_RD::Set1DData(const char* key, int n, float* data)
{
	this->m_Rows = n;
	this->m_Data = data;
}

void clsTSD_RD::Get1DData(const char* key, int* n, float** data)
{
	string sk(key);
	if(this->m_Rows == -1 || this->m_Data == NULL)
	{
		throw ModelException(MID_TSD_RD,"GetData","The data "+string(key) +" is NULL.");
	}
	*data = this->m_Data;
	*n = this->m_Rows;
}
