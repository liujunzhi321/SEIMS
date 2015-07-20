/*!
 * \file clsTSD_RD.cpp
 * \brief 
 *
 *
 *
 * \author [your name]
 * \version 
 * \date July 2015
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

/*	char sValue[30];
	for(int i=0;i<rows;i++)
	{
		sprintf_s(sValue,30,"Set %d=%f",i,data[i]);
		this->StatusMsg(sValue);
	}*/	
}

void clsTSD_RD::Get1DData(const char* key, int* n, float** data)
{
	//if(this->m_dataType == -1.0f) throw ModelException("TSD_RD","GetData","You should appoint the data type.");
	if(this->m_Rows == -1 || this->m_Data == NULL)
	{
		throw ModelException("TSD_RD","GetData","The data is NULL.");
	}
	*data = this->m_Data;
	*n = this->m_Rows;
}
