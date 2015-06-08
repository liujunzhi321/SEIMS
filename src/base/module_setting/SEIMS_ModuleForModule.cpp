#include "SEIMS_ModuleForModule.h"
#include "ModelException.h"

using namespace Module;

WetSpaModule::WetSpaModule(void)
{
	m_StatusMsg = NULL;
	m_date = (time_t)-1;

	m_metadataInfo = NULL;
	//CreateMetadataInfo();
}


WetSpaModule::~WetSpaModule(void)
{
	if(m_metadataInfo != NULL) delete m_metadataInfo;
}

bool WetSpaModule::SetDate(time_t date)
{
	m_date = date;
	return true;
}
void WetSpaModule::SetStatusCallback(pICStatusMessage cb)
{
	m_StatusMsg = cb;
}

void WetSpaModule::StatusMsg(string msg)
{
	if (m_StatusMsg != NULL)
	{
		m_StatusMsg(msg.c_str());
	}
}

void WetSpaModule::CreateBasicMetadataInfo(void)
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
		m_metadataInfo->SetWebsite("http://www.website.com");
	}
}

string WetSpaModule::MetadataInformation(void)
{
	if(m_metadataInfo == NULL)
		throw ModelException("WetSpaModule","MetadataInformation",
		"The metadata info is null. Please call CreateMetadataInfo first!");
	return m_metadataInfo->GetXMLDocument();
}

bool WetSpaModule::CheckInputSize(const char* key,int sizeFromMain, int* sizeInModule)
{
	if(sizeFromMain<=0)
		throw ModelException("WetSpaModule","CheckInputSize",
			"Input data for "+string(key) +" in module "+ModuleName()+
			" is invalid. The size could not be less than zero.");

	if(*sizeInModule != sizeFromMain)
	{
		if(*sizeInModule <=0) *sizeInModule = sizeFromMain;
		else
			throw ModelException("WetSpaModule","CheckInputSize",
			"Input data for "+string(key) +" in module "+ModuleName()+
			" is invalid. All the input data should have same size.");
	}

	return true;
}
