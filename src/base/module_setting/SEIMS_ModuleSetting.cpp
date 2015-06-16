/*!
 * \file SEIMS_ModuleSetting.cpp
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
#include "SEIMS_ModuleSetting.h"
#include "utils.h"
#include "ModelException.h"
#include "util.h"
#include "text.h"
//! Constructor
SEIMSModuleSetting::SEIMSModuleSetting(string moduleId,string setting)
{
	m_moduleId = moduleId;
	m_settingString = setting;

	getSettings();
}

//! Destructor
SEIMSModuleSetting::~SEIMSModuleSetting(void)
{

}
//! Get settings
void SEIMSModuleSetting::getSettings()
{
	utils util;
	m_settings = util.SplitString(m_settingString,'_');
}

string SEIMSModuleSetting::dataTypeString(){return dataType2String(dataType());}

float SEIMSModuleSetting::dataType()
{
	if(m_moduleId.find("ITP") == string::npos && m_moduleId.find("TSD") == string::npos) 
	{
		return -1.0f;
	}
	else
	{
		if(m_settings.size() < 2) throw ModelException("SEIMSModuleSetting","dataType","Module "+ m_moduleId + " does not appoint data type in the second column.");
		float dataType = dataTypeString2Float(m_settings.at(1));
		if(dataType == -1.0f) throw ModelException("SEIMSModuleSetting","dataType","The data type of module "+ m_moduleId + " is not correct. It must be P, TMIN, TMAX or PET.");
		return dataType;
	}
}

bool SEIMSModuleSetting::needDoVerticalInterpolation()
{
	if(m_moduleId.find("ITP") == string::npos ) 
		return false;
	else
	{
		if(m_settings.size() < 3) throw ModelException("SEIMSModuleSetting","needDoVerticalInterpolation","Module "+ m_moduleId + " does not appoint vertical interpolation in the third column.");
		int iIsDoVerticalInterpolation = atoi(m_settings.at(2).c_str());
						
		return iIsDoVerticalInterpolation == 0 ? false : true;
	}
}

string  SEIMSModuleSetting::channelRoutingMethod(int methodIndex)
{
	if(!StringMatch(m_moduleId,"ChannelRouting")) return "";
	else
	{
		if(int(m_settings.size()) < methodIndex + 1)
			throw ModelException("SEIMSModuleSetting","needDoVerticalInterpolation","Module "+ m_moduleId + " does not appoint vertical interpolation in the third column.");
		
		return m_settings.at(methodIndex);
	}
}

string SEIMSModuleSetting::channelFlowRoutingMethod()
{
	return channelRoutingMethod(1);
}
string SEIMSModuleSetting::channelSedimentRoutingMethod()
{
	return channelRoutingMethod(2);
}
string SEIMSModuleSetting::channelNutrientRoutingMethod()
{
	return channelRoutingMethod(3);
}

float SEIMSModuleSetting::dataTypeString2Float(string dataType)
{
	if(StringMatch(dataType,DataType_Precipitation)) return 1.0f;
	if(StringMatch(dataType,DataType_MinimumTemperature)) return 2.0f;
	if(StringMatch(dataType,DataType_MaximumTemperature)) return 3.0f;
	if(StringMatch(dataType,DataType_PotentialEvapotranspiration)) return 4.0f;
	if(StringMatch(dataType,DataType_SolarRadiation)) return 5.0f;
	if(StringMatch(dataType,DataType_WindSpeed)) return 6.0f;
	if(StringMatch(dataType,DataType_RelativeAirMoisture)) return 7.0f;
	return -1.0f;
}

string SEIMSModuleSetting::dataType2String(float dataType)
{
	switch((int)dataType)
	{
		case 1:
			return DataType_Precipitation;
		case 2:
			return DataType_MinimumTemperature;
		case 3:
			return DataType_MaximumTemperature;
		case 4:
			return DataType_PotentialEvapotranspiration;
		case 5:
			return DataType_SolarRadiation;
		case 6:
			return DataType_WindSpeed;
		case 7:
			return DataType_RelativeAirMoisture;
		default:
			return "";
	}
}
