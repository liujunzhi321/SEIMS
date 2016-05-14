/*!
 * \file SEIMS_ModuleForModule.cpp
 * \brief
 *
 *
 *
 * \author [your name]
 * \version 
 * \date June 2015
 *
 * 
 */
#include "SEIMS_ModuleForModule.h"
#include "ModelException.h"
//! \namespace Module
using namespace Module;
//! Constructor
SEIMSModule::SEIMSModule(void)
{
	m_StatusMsg = NULL;
	m_date = (time_t)-1;

	m_metadataInfo = NULL;
	//CreateMetadataInfo();
}

//! Destructor
SEIMSModule::~SEIMSModule(void)
{
	if(m_metadataInfo != NULL) delete m_metadataInfo;
}
//! Set time
bool SEIMSModule::SetDate(time_t date)
{
	m_date = date;
	return true;
}
void SEIMSModule::SetStatusCallback(pICStatusMessage cb)
{
	m_StatusMsg = cb;
}

void SEIMSModule::StatusMsg(string msg)
{
	if (m_StatusMsg != NULL)
	{
		m_StatusMsg(msg.c_str());
	}
}
//! Create basic metadata information
void SEIMSModule::CreateBasicMetadataInfo(void)
{
	if(m_metadataInfo == NULL)
	{
		m_metadataInfo = new MetadataInfo();

		m_metadataInfo->SetAuthor(ModuleAuthor());
		m_metadataInfo->SetClass(ModuleClassName(), "");
		m_metadataInfo->SetDescription(ModuleDescription());
		m_metadataInfo->SetEmail("");
		m_metadataInfo->SetHelpfile(ModuleName() + ".CHM");
		m_metadataInfo->SetID(ModuleName());
		m_metadataInfo->SetName(ModuleName());
		m_metadataInfo->SetVersion(ModuleVersion());
		m_metadataInfo->SetWebsite("https://github.com/seims/SEIMS");
	}
}
//! Get metadata information
string SEIMSModule::MetadataInformation(void)
{
	if(m_metadataInfo == NULL)
		throw ModelException("SEIMSModule","MetadataInformation",
		"The metadata info is null. Please call CreateMetadataInfo first!");
	return m_metadataInfo->GetXMLDocument();
}
//! Check input size
bool SEIMSModule::CheckInputSize(const char* key,int sizeFromMain, int* sizeInModule)
{
	if(sizeFromMain<=0)
		throw ModelException("SEIMSModule","CheckInputSize",
			"Input data for "+string(key) +" in module "+ModuleName()+
			" is invalid. The size could not be less than zero.");

	if(*sizeInModule != sizeFromMain)
	{
		if(*sizeInModule <=0) *sizeInModule = sizeFromMain;
		else
			throw ModelException("SEIMSModule","CheckInputSize",
			"Input data for "+string(key) +" in module "+ModuleName()+
			" is invalid. All the input data should have same size.");
	}

	return true;
}
