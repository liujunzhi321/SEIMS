/*!
 * \file ModuleFactory.cpp
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
#ifndef linux
#include <windows.h>
#define DLL_EXT  ".dll"
#else
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#define DLL_EXT  ".so"
#endif


#include <time.h>
#include <string.h>
#include <fstream>
#include "bson.h"

#include "util.h"
#include "utils.h"
#include "text.h"
#include "ModuleFactory.h"
#include "ModelException.h"
#include "MetadataInfoConst.h"
#include "MetadataInfo.h"
#include "MongoUtil.h"
#include "DBManager.h"

//! Constructor, by default the layering method is DOWN_UP
ModuleFactory::ModuleFactory(const string& configFileName, const string& modulePath, mongo* conn, const string& dbName)
	:m_modulePath(modulePath), m_conn(conn), m_dbName(dbName), m_layingMethod(DOWN_UP)
{
	Init(configFileName);
#ifdef USE_MONGODB
	gridfs_init(m_conn, m_dbName.c_str(), "spatial", m_spatialData); 
#endif
	
}
//! Destructor
ModuleFactory::~ModuleFactory(void)
{
#ifdef USE_MONGODB
	gridfs_destroy(m_spatialData);
	mongo_destroy(m_conn);
#endif

	for (map<string, SEIMSModuleSetting*>::iterator it = m_settings.begin(); it != m_settings.end(); ++it)
		delete it->second;
	
	for (map<string, const char*>::iterator it = m_metadata.begin(); it != m_metadata.end(); ++it)
		delete it->second;

	for (map<string, ParamInfo*>::iterator it = m_parametersInDB.begin(); it != m_parametersInDB.end(); ++it)
		delete it->second;

	for (map<string, clsInterpolationWeighData*>::iterator it = m_weightDataMap.begin(); it != m_weightDataMap.end(); ++it)
		delete it->second;

	for (map<string, clsRasterData*>::iterator it = m_rsMap.begin(); it != m_rsMap.end(); ++it)
		delete it->second;

	//for (map<string, float*>::iterator it = m_1DArrayMap.begin(); it != m_1DArrayMap.end(); ++it)
	//	delete it->second;

	for (map<string, float**>::iterator it = m_2DArrayMap.begin(); it != m_2DArrayMap.end(); ++it)
	{
		for (int j = 0; j < m_2DRowsLenMap[it->first]; j++)
			delete it->second[j];
		delete it->second;
	}


	for (size_t i = 0; i < m_dllHandles.size(); i++)
	{
#ifndef linux
		FreeLibrary(m_dllHandles[i]);
#else
		dlclose(m_dllHandles[i]);
#endif
	}


}
//! Initialization
void ModuleFactory::Init(const string& configFileName)
{
	ReadConfigFile(configFileName.c_str());
#ifndef USE_MONGODB
	ReadParametersFromSQLite();
#else
	ReadParametersFromMongoDB();
#endif

	size_t n = m_moduleIDs.size();
	// read all the .dll or .so and create objects
	for (size_t i = 0; i < n; i++)
	{
		string id = m_moduleIDs[i];
		string dllID = id;
		// for ITP modules, the input ids are ITP_T, ITP_P and ITP should be used as ID name
		if(id.find("ITP") != string::npos)
#ifndef linux
			dllID = "ITP";
#else
			dllID = "libITP";
#endif
		else if (id.find("TSD") != string::npos)
#ifndef linux
			dllID = "TSD_RD";
#else
			dllID = "libTSD_RD";
#endif

#ifdef INTEL_COMPILER
		dllID = dllID + "_intel";
#endif
#ifdef INTEL_COMPILER_SINGLE
		dllID = dllID + "_intel_single";
#endif
#ifdef SINGLE
		dllID = dllID + "_single";
#endif
		// load function pointers from DLL
		ReadDLL(id, dllID);

		// load metadata
		MetadataFunction metadataInfo = m_metadataFuncs[id];
		const char* metadata = metadataInfo();
		m_metadata[id] = metadata;
		// parse the metadata
		TiXmlDocument doc;
		doc.Parse(metadata);
		ReadParameterSetting(id, doc, m_settings[id]);
		ReadInputSetting(id, doc, m_settings[id]);
		ReadOutputSetting(id, doc, m_settings[id]);
	}

	// set the connections among objects
	for (size_t i = 0; i < n; i++)
	{
		string id = m_moduleIDs[i];
		//cout << id << endl;
		vector<ParamInfo>& inputs = m_inputs[id];
		for (size_t j = 0; j < inputs.size(); j++)
		{
			ParamInfo& param = inputs[j];
			if(!StringMatch(param.Source, "module"))
				continue;

			param.DependPara = FindDependentParam(param);		
			//cout << "\t" << param.Name << "\t" << param.DependPara->ModuleID << ":" << param.DependPara->Name << endl; 
		}
	}
	
}
//! Read parameters from Sqlite database
//! \deprecated For now, Sqlite is deprecated in SEIMS.
void ModuleFactory::ReadParametersFromSQLite()
{
	DBManager dbman;
	try
	{
		// open the hydrclimate database
		dbman.Open(m_dbName);
		// if there is no error
		if (!dbman.IsError())
		{
			vector<string> paraTables;
			paraTables.push_back("Climate");
			paraTables.push_back("Discharge");
			paraTables.push_back("InstreamWQ");
			paraTables.push_back("Interception");
			paraTables.push_back("PlantGrowth");
			paraTables.push_back("Sediment");
			paraTables.push_back("Snow");
			paraTables.push_back("WaterBalance");

			// loop table list to get parameter information
			for (size_t i = 0; i < paraTables.size(); i++)
			{
				// 'NAME', 'DESCRIPTION', 'UNIT', 'MODULE', 'VALUE', 'IMPACT', 'CHANGE', 'MAX', 'MIN', 'USE'
				string strSQL = "select * from " + paraTables[i];
				slTable *tbl = dbman.Load(strSQL);
				if (tbl != NULL)
				{
					// if there is at least one record
					for (size_t iRow = 0; iRow < tbl->nRows; iRow++)
					{
						ParamInfo * p = new ParamInfo();
						p->Name = tbl->FieldValue(iRow, 0);
						p->Description = tbl->FieldValue(iRow, 1);
						p->Units = tbl->FieldValue(iRow, 2);
						p->Value = atof(tbl->FieldValue(iRow, 4).c_str());
						p->Impact = atof(tbl->FieldValue(iRow, 5).c_str());
						p->Change = tbl->FieldValue(iRow, 6);
						p->Max = atof(tbl->FieldValue(iRow, 7).c_str());
						p->Min = atof(tbl->FieldValue(iRow, 8).c_str());
						p->Use = tbl->FieldValue(iRow, 9);

						m_parametersInDB[GetUpper(p->Name)] = p;

						//cout << p->Name << "\t" << p->Value << endl;
					}
					delete tbl;
				}
				tbl = NULL;
			}

		}
		dbman.Close();
	}
	catch (...)
	{
		dbman.Close();		
		throw;
	}
}
//! Read parameters from mongodb
void ModuleFactory::ReadParametersFromMongoDB()
{
	mongo_cursor cursor[1];
	ostringstream oss;
	oss << m_dbName << "." << PARAMETERS_TABLE;
	mongo_cursor_init(cursor, m_conn, oss.str().c_str());

	
	while( mongo_cursor_next(cursor) == MONGO_OK ) 
	{
		const bson* info = mongo_cursor_bson(cursor);
		ParamInfo * p = new ParamInfo();
		bson_iterator it[1];
		bson_iterator_init(it, info);
		while(bson_iterator_next(it)){
			if(strcmp("NAME", bson_iterator_key(it)) == 0)
				p->Name = bson_iterator_string(it);
			else if(strcmp("UNIT", bson_iterator_key(it)) == 0)
				p->Units = bson_iterator_string(it);
			else if(strcmp("VALUE", bson_iterator_key(it)) == 0)
				p->Value = (float)bson_iterator_double(it);
			else if(strcmp("IMPACT", bson_iterator_key(it)) == 0)
				p->Impact = (float)bson_iterator_double(it);
			else if(strcmp("CHANGE", bson_iterator_key(it)) == 0)
				p->Change = bson_iterator_string(it);
			else if(strcmp("MAX", bson_iterator_key(it)) == 0)
				p->Max = (float)bson_iterator_double(it);
			else if(strcmp("MIN", bson_iterator_key(it)) == 0)
				p->Min = (float)bson_iterator_double(it);
			else if(strcmp("USE", bson_iterator_key(it)) == 0)
				p->Use = bson_iterator_string(it);
		}
		
		m_parametersInDB[GetUpper(p->Name)] = p;
	}

	//bson_destroy(query);
	mongo_cursor_destroy(cursor);

}
//! Get comparable name after underscore if necessary
string ModuleFactory::GetComparableName(string& paraName)
{
    if (paraName.length() <= 2)
        return paraName;

	string compareName;

	string prefix = paraName.substr(0, 2);
	if (prefix == "D_" || prefix == "T_" || prefix == "d_" || prefix == "t_")
		compareName = paraName.substr(2); //use the string after the underscore, T_PET, use PET
	else 
		compareName = paraName;

	return compareName;
}

//! Create module list. Return time-consuming of reading data
int ModuleFactory::CreateModuleList(string dbName, int subbasinID, int numThreads, LayeringMethod layeringMethod, 
	clsRasterData* templateRaster, SettingsInput* settingsInput, vector<SimulationModule*>& modules)
{
	m_layingMethod = layeringMethod;
	size_t n = m_moduleIDs.size();
	for (size_t i = 0; i < n; i++)
	{
		SimulationModule *pModule = GetInstance(m_moduleIDs[i]);
		//pModule->SetID(m_moduleIDs[i]);
		pModule->SetTheadNumber(numThreads);
		modules.push_back(pModule);
	}

	clock_t t1 = clock();
	//initial parameter (reading parameter information from database)
    //cout << "reading parameter information from database...\n";
	for (size_t i = 0; i < n; i++)
	{
		string id = m_moduleIDs[i];
		vector<ParamInfo>& parameters = m_parameters[id];

		bool verticalInterpolation = true;
		if (id.find("ITP") != string::npos)
		{
			modules[i]->SetDataType(m_settings[id]->dataType());
			for (size_t j = 0; j < parameters.size(); j++)
			{
				ParamInfo& param = parameters[j];
				if (StringMatch(param.Name, Tag_VerticalInterpolation))
				{
					if (param.Value > 0.f)
						verticalInterpolation = true;
					else
						verticalInterpolation = false;
					break;
				}
			}
		}
		//cout << "\t" << id << endl;
		for (size_t j = 0; j < parameters.size(); j++)
		{
			//cout << parameters.size() << "\t" << j << "\t";
			ParamInfo& param = parameters[j];
			//if (param.Dimension != DT_Single)
			//	cout << "\t\t" << param.Name << endl;
			SetData(dbName, subbasinID, m_settings[id], &param, templateRaster, settingsInput, modules[i], verticalInterpolation);
		}
	}
	clock_t t2 = clock();
    //cout << "reading parameter finished.\n";

	return t2-t1;
}
//! Find dependent parameters 
ParamInfo* ModuleFactory::FindDependentParam(ParamInfo& paramInfo)
{
	string paraName = GetComparableName(paramInfo.Name);
	dimensionTypes paraType = paramInfo.Dimension;

	size_t n = m_moduleIDs.size();
	for (size_t i = 0; i < n; i++)
	{
		string id = m_moduleIDs[i];
		vector<ParamInfo>& outputs = m_outputs[id];
		for (size_t j = 0; j < outputs.size(); j++)
		{
			ParamInfo& param = outputs[j];
			string compareName = GetComparableName(param.Name);

			if (StringMatch(paraName, compareName) && param.Dimension == paraType)
			{
				param.OutputToOthers = true;
				return &param;
			}
		}
	}
	//throw ModelException("ModuleFactory", "FindDependentParam", "Can not find input for " + paraName + ".\n");
	return NULL;
}
//! Load function pointers from .DLL or .so
void ModuleFactory::ReadDLL(string& id, string& dllID)
{
	// the dll file is already read, return
	if (m_instanceFuncs.find(id) != m_instanceFuncs.end())
		return;

	// check if the dll file exists
	string moduleFileName = trim(m_modulePath) + string(dllID) + string(Tag_ModuleExt);
	utils util;
	if(!util.FileExists(moduleFileName)) throw ModelException("ModuleFactory", "ReadDLL", moduleFileName + " does not exist or has no read permission!");

	//load library
#ifndef linux
	HINSTANCE handle = LoadLibrary(TEXT(moduleFileName.c_str()));
	if (handle == NULL) throw ModelException("ModuleFactory", "ReadDLL", "Could not load " + moduleFileName);
	m_instanceFuncs[id] = InstanceFunction(GetProcAddress(HMODULE(handle), "GetInstance"));
	m_metadataFuncs[id] = MetadataFunction(GetProcAddress(HMODULE(handle), "MetadataInformation"));
#else
	void* handle = dlopen(moduleFileName.c_str(), RTLD_LAZY); 
	if (handle == NULL) 
	{
		cout << dlerror() << endl;
		throw ModelException("ModuleFactory", "ReadDLL", "Could not load " + moduleFileName);
	}
	m_instanceFuncs[id] = InstanceFunction(dlsym(handle, "GetInstance"));
	m_metadataFuncs[id] = MetadataFunction(dlsym(handle, "MetadataInformation"));
#endif
	m_dllHandles.push_back(handle);
	if(m_instanceFuncs[id] == NULL) 
		throw ModelException("ModuleFactory", "ReadDLL", moduleFileName + " does not implement API function: GetInstance");
	if(m_metadataFuncs[id] == NULL) 
		throw ModelException("ModuleFactory", "ReadDLL", moduleFileName + " does not implement API function: MetadataInformation");

}

//! Get instance
SimulationModule* ModuleFactory::GetInstance(string& moduleID)
{
	return m_instanceFuncs[moduleID]();
}
//! Match type
dimensionTypes ModuleFactory::MatchType(string strType)
{
	// default
	dimensionTypes typ = DT_Unknown;

	if (StringMatch(strType,Type_Single)) typ = DT_Single;
	if (StringMatch(strType,Type_Array1D)) typ = DT_Array1D;
	if (StringMatch(strType,Type_Array2D)) typ = DT_Array2D;
	if (StringMatch(strType,Type_Array3D)) typ = DT_Array3D;
	if (StringMatch(strType,Type_Array1DDateValue)) typ = DT_Array1DDateValue;
	if (StringMatch(strType,Type_MapWindowRaster)) typ = DT_Raster;
	if (StringMatch(strType,Type_SiteInformation)) typ = DT_SiteInformation;
	if (StringMatch(strType,Type_LapseRateArray)) typ = DT_LapseRateArray;
	if (StringMatch(strType,Type_Scenario)) typ = DT_Scenario;

	return typ;
}
//! Read module's parameters setting from XML string
void ModuleFactory::ReadParameterSetting(string& moduleID, TiXmlDocument& doc, SEIMSModuleSetting* setting)
{
	m_parameters.insert(map<string, vector<ParamInfo> >::value_type(moduleID, vector<ParamInfo>()));
	vector<ParamInfo>& vecPara = m_parameters[moduleID];

	TiXmlElement* eleMetadata = doc.FirstChildElement(TagMetadata.c_str());

	// start getting the parameters
	TiXmlElement* eleParams = eleMetadata->FirstChildElement(TagParameters.c_str());
	if (eleParams != NULL)
	{
		TiXmlElement* eleParam = eleParams->FirstChildElement(TagParameter.c_str());
		while (eleParam != NULL)
		{
			// clear the object
			ParamInfo *param = new ParamInfo();

			// set the module id
			param->ModuleID = moduleID;

			// get the parameter name
			TiXmlElement* elItm = eleParam->FirstChildElement(TagParameterName.c_str());
			if (elItm != NULL)
			{
				if (elItm->GetText() != NULL)
				{
					param->Name = GetUpper(elItm->GetText());
					param->BasicName = param->Name;
					param->ClimateType = setting->dataTypeString();

					//set climate data type got from config.fig
					//this is used for interpolation module
					if(StringMatch(param->Name,Tag_DataType)) param->Value = setting->dataType();

					//special process for interpolation modules
					if(StringMatch(param->Name,Tag_Weight)) 
					{
						if(setting->dataTypeString().length() == 0) 
							throw ModelException("ModuleFactory","readParameterSetting","The parameter " + string(Tag_Weight) + " should have corresponding data type in module " + moduleID);
						if(StringMatch(setting->dataTypeString(),DataType_MinimumTemperature) || StringMatch(setting->dataTypeString(),DataType_MaximumTemperature))
							param->Name += "_M";  //The weight coefficient file is same for Tmin and Tmax, so just need to read one file named "Weight_T.txt"
						else
							param->Name += "_" + setting->dataTypeString();	//combine weight and data type. e.g. Weight + PET = Weight_PET, this combined string must be the same with the parameter column in the climate table of parameter database.
					}

					//special process for interpolation modules
					if(StringMatch(param->Name, Tag_StationElevation)) 
					{
						if(setting->dataTypeString().length() == 0) 
							throw ModelException("ModuleFactory","readParameterSetting","The parameter " + string(Tag_StationElevation) + " should have corresponding data type in module " + moduleID);
						if(StringMatch(setting->dataTypeString(),DataType_Precipitation))
						{
							param->BasicName += "_P";
							param->Name += "_P";
						}
						else
						{
							param->BasicName += "_M";
							param->Name += "_M";
						}
					}

					if(StringMatch(param->Name,Tag_VerticalInterpolation)) //if do the vertical interpolation
					{
						param->Value = (setting->needDoVerticalInterpolation() ? 1.0f:0.0f);
					}
				}
			}

			// get the parameter description
			elItm = eleParam->FirstChildElement(TagParameterDescription.c_str());
			if (elItm != NULL)
			{
				if (elItm->GetText() != NULL)
				{
					param->Description = elItm->GetText();
				}
			}

			// get the parameter units
			elItm = eleParam->FirstChildElement(TagParameterUnits.c_str());
			if (elItm != NULL)
			{
				if (elItm->GetText() != NULL)
				{
					param->Units = elItm->GetText();
				}
			}

			// get the parameter source
			elItm = eleParam->FirstChildElement(TagParameterSource.c_str());
			if (elItm != NULL)
			{
				if (elItm->GetText() != NULL)
				{
					param->Source = elItm->GetText();
				}
			}

			// get the parameter dimension
			elItm = eleParam->FirstChildElement(TagParameterDimension.c_str());
			if (elItm != NULL)
			{
				if (elItm->GetText() != NULL)
				{
					param->Dimension = MatchType(string(elItm->GetText()));
				}
			}

			// cleanup
			elItm = NULL;

			// parameter must have these values
			if (param->Name.size() <= 0 )
			{
				delete param;
				throw ModelException("ModuleFactory", "ReadParameterSetting", "Some parameters have not name in metadata!");
			}

			if (param->Source.size() <= 0)
			{
				string name = param->Name;
				delete param;
				throw ModelException("ModuleFactory", "ReadParameterSetting", "parameter "+ name + " does not have source!");
			}

			if (param->Dimension == DT_Unknown)
			{
				string name = param->Name;
				delete param;
				throw ModelException("ModuleFactory", "ReadParameterSetting", "parameter "+ name + " does not have dimension!");
			}

			// add to the list
			//m_paramters[GetUpper(param->Name)] = param;
			vecPara.push_back(*param);
			delete param;

			// get the next parameter if it exists
			eleParam = eleParam->NextSiblingElement();
		} // while
	}
}
//! is constant input?
bool ModuleFactory::IsConstantInputFromName(string& name)
{
	if(	StringMatch(name,Contant_Input_Elevation) ||
		StringMatch(name,Contant_Input_Latitude)	||
		StringMatch(name,Contant_Input_Xpr)		||
		StringMatch(name,Contant_Input_Ypr))
		//StringMatch(name,Contant_Input_FlowdiversionProperty) ||
		//StringMatch(name,Contant_Input_PointsourceProperty) ||
		//StringMatch(name,Contant_Input_ReservoirProperty) ||
		//StringMatch(name,Contant_Input_ReservoirRatingCurve) ||
		//StringMatch(name,Contant_Input_ReservoirOperationSchedual))
		return true;
	return false;
}
//! Read module's input setting from XML string
void ModuleFactory::ReadInputSetting(string& moduleID, TiXmlDocument& doc, SEIMSModuleSetting* setting)
{
	m_inputs.insert(map<string, vector<ParamInfo> >::value_type(moduleID, vector<ParamInfo>()));
	vector<ParamInfo>& vecPara = m_inputs[moduleID];

	TiXmlElement* eleMetadata = doc.FirstChildElement(TagMetadata.c_str());

	TiXmlElement* eleInputs = eleMetadata->FirstChildElement(TagInputs.c_str());
	if (eleInputs != NULL)
	{
		TiXmlElement* elInput = eleInputs->FirstChildElement(TagInputVariable.c_str());
		while (elInput != NULL)
		{
			ParamInfo *param = new ParamInfo();

			// set the module id
			param->ModuleID = moduleID;
			param->ClimateType = setting->dataTypeString();

			// get the input variable name
			TiXmlElement* elItm = elInput->FirstChildElement(TagInputVariableName.c_str());
			if (elItm != NULL)
			{
				if (elItm->GetText() != NULL)
				{
					param->Name = GetUpper(elItm->GetText());
					param->BasicName = param->Name;
					param->IsConstant = IsConstantInputFromName(param->Name);
					if(setting->dataTypeString().length() > 0) param->Name = param->Name + "_" + setting->dataTypeString();
				}
			}

			// get the input variable units(
			elItm = elInput->FirstChildElement(TagInputVariableUnits.c_str());
			if (elItm != NULL)
			{
				if (elItm->GetText() != NULL)
				{
					param->Units = elItm->GetText();
				}
			}

			// get the input variable description
			elItm = elInput->FirstChildElement(TagInputVariableDescription.c_str());
			if (elItm != NULL)
			{
				if (elItm->GetText() != NULL)
				{
					param->Description = elItm->GetText();
				}
			}

			// get the input variable source
			elItm = elInput->FirstChildElement(TagInputVariableSource.c_str());
			if (elItm != NULL)
			{
				if (elItm->GetText() != NULL)
				{
					param->Source = elItm->GetText();
				}
			}

			// get the input variable dimension
			elItm = elInput->FirstChildElement(TagInputVariableDimension.c_str());
			if (elItm != NULL)
			{
				if (elItm->GetText() != NULL)
				{
					param->Dimension = MatchType(string(elItm->GetText()));
				}
			}

			elItm = NULL;


			//string keyName = GetUpper(param->Name);
			//string dataTypeString = dataType2String(climateDataType);
			//if(m_setting->dataTypeString().length() > 0) keyName += "_"+m_setting->dataTypeString();		// if the module has corresponding climate data type, 
			//param->Name = keyName;
			//m_inputs[keyName] = param;			

			//if(!StringMatch(param->Source,"module")) param->InitialInputOriginalName();

			// add to the list
			//m_inputs[GetUpper(param->Name)] = param;
			//m_inputs.push_back(param);

			// input must have these values 
			if (param->Name.size() <= 0 )
			{
				delete param;
				throw ModelException("SEIMSModule","readInputSetting","Some input variables have not name in metadata!");
			}

			if (param->Source.size() <= 0)
			{
				string name = param->Name;
				delete param;
				throw ModelException("SEIMSModule","readInputSetting","Input variable "+ name + " does not have source!");
			}

			if (param->Dimension == DT_Unknown)
			{
				string name = param->Name;
				delete param;
				throw ModelException("SEIMSModule","readInputSetting","Input variable "+ name + " does not have dimension!");
			}

			vecPara.push_back(*param);
			delete param;

			// get the next input if it exists
			elInput = elInput->NextSiblingElement();
		}
	}
}
//! Read module's output from XML string
void ModuleFactory::ReadOutputSetting(string& moduleID, TiXmlDocument& doc, SEIMSModuleSetting* setting)
{
	m_outputs.insert(map<string, vector<ParamInfo> >::value_type(moduleID, vector<ParamInfo>()));
	vector<ParamInfo>& vecPara = m_outputs[moduleID];

	TiXmlElement* eleMetadata = doc.FirstChildElement(TagMetadata.c_str());

	TiXmlElement* eleOutputs = eleMetadata->FirstChildElement(TagOutputs.c_str());
	if (eleOutputs != NULL)
	{
		TiXmlElement* elOutput = eleOutputs->FirstChildElement(TagOutputVariable.c_str());
		while (elOutput != NULL)
		{
			ParamInfo *param = new ParamInfo();

			// set the module id
			param->ModuleID = moduleID;
			param->ClimateType = setting->dataTypeString();
			// get the output variable name
			TiXmlElement* elItm = elOutput->FirstChildElement(TagOutputVariableName.c_str());
			if (elItm != NULL)
			{
				if (elItm->GetText() != NULL)
				{
					param->Name = GetUpper(elItm->GetText());
					param->BasicName = param->Name;
					if(setting->dataTypeString().size() > 0) param->Name = param->Name + "_" + setting->dataTypeString();
				}
			}

			// get the output variable units
			elItm = elOutput->FirstChildElement(TagOutputVariableUnits.c_str());
			if (elItm != NULL)
			{
				if (elItm->GetText() != NULL)
				{
					param->Units = elItm->GetText();
				}
			}

			// get the output variable description
			elItm = elOutput->FirstChildElement(TagOutputVariableDescription.c_str());
			if (elItm != NULL)
			{
				if (elItm->GetText() != NULL)
				{
					param->Description = elItm->GetText();
				}
			}

			param->Source = "";

			// get the output variable dimension
			elItm = elOutput->FirstChildElement(TagOutputVariableDimension.c_str());
			if (elItm != NULL)
			{
				if (elItm->GetText() != NULL)
				{
					param->Dimension = MatchType(string(elItm->GetText()));
				}
			}

			elItm = NULL;

			// add to the list
			param->IsOutput = true;
			

			//string keyName = GetUpper(param->Name);
			//string dataTypeString = m_setting->dataTypeString();
			//if(dataTypeString.length() > 0) keyName += "_"+dataTypeString;		// if the module has corresponding climate data type, 
			//param->Name = keyName;																	// the key of m_outputs is a string contains param name and data type. e.g. D_P 
			//m_outputs[keyName] = param;


			// output variable must have these values 
			if (param->Name.size() <= 0 )
			{
				delete param;
				throw ModelException("SEIMSModule","readOutputSetting","Some output variables have not name in metadata!");
			}

			if (param->Dimension == DT_Unknown)
			{
				string name = param->Name;
				delete param;
				throw ModelException("SEIMSModule","readInputSetting","Input variable "+ name + " does not have dimension!");
			}

			vecPara.push_back(*param);
			delete param;
			// get the next input if it exists
			elOutput = elOutput->NextSiblingElement();
		}
	}
}
//! Load settings from file
bool ModuleFactory::LoadSettingsFromFile(const char* filename, vector< vector<string> >& settings)
{
	bool bStatus = false;
	ifstream myfile;
	string line;
	utils utl;

	try
	{
		// open the file
		myfile.open(filename, ios::in);
		if (myfile.is_open())
		{
			while (!myfile.eof())
			{
				if (myfile.good())
				{
					getline(myfile, line);
					//utl.TrimSpaces(line);
					line = trim(line);
					if ((line.size() > 0) && (line[0] != '#')) // ignore comments and empty lines
					{
						// parse the line into separate item
						vector<string> tokens = utl.SplitString(line, '|');
						// is there anything in the token list?
						if (tokens.size() > 0)
						{
							for (size_t i=0; i<tokens.size(); i++)
							{
								//utl.TrimSpaces(tokens[i]);
								tokens[i] = trim(tokens[i]);
							}
							// is there anything in the first item?
							if (tokens[0].size() > 0)
							{
								// there is something to add so resize the header list to append it
								int sz = settings.size(); // get the current number of rows
								settings.resize(sz+1);		// resize with one more row

								settings[sz] = tokens;

								bStatus = true; // consider this a success
							} // if there is nothing in the first item of the token list there is nothing to add to the header list
						}
					}
				}
			}
			bStatus = true;
			myfile.close();
		}
	}
	catch (...)
	{
		myfile.close();
		//throw;
	}

	return bStatus;
}
//! Read configuration file 
void ModuleFactory::ReadConfigFile(const char* configFileName)
{
	vector< vector<string> > settings;
	LoadSettingsFromFile(configFileName, settings);

	try
	{
		//if (moduleList != NULL)
		{
			for (size_t i = 0; i < settings.size(); i++)
			{
				if (settings[i].size() > 3)
				{
					string settingString = settings[i][1];
					string module = GetUpper(settings[i][3]);
#ifdef linux
					module = "lib" + module;
#endif
					
					SEIMSModuleSetting* moduleSetting = new SEIMSModuleSetting(module, settingString);
					if(moduleSetting->dataTypeString().length() > 0) 
						module += "_" + moduleSetting->dataTypeString(); // make the module id unique

					m_moduleIDs.push_back(module);
					m_settings[module] = moduleSetting;
				}
			}
		}
	}
	catch (...)
	{
		throw;
	}

}
//! Set data
void ModuleFactory::SetData(string& dbName, int nSubbasin, SEIMSModuleSetting* setting, ParamInfo* param, clsRasterData* templateRaster,
	SettingsInput* settingsInput, SimulationModule* pModule, bool vertitalItp)
{
	//set the parameter data to the module
	string name = param->BasicName;
	if(setting->dataTypeString().size() == 0 
		&& !StringMatch(param->BasicName,Contant_Input_Elevation) 
		&& !StringMatch(param->BasicName,Contant_Input_latitude) 
		&& !StringMatch(param->BasicName,Contant_Input_Xpr)
		&& !StringMatch(param->BasicName,Contant_Input_Ypr))
	{
		name = param->Name;
		//cout << param->Name << " " << param->BasicName << endl;
	}
	//if(setting->dataTypeString().size()>0 && StringMatch(param->BasicName,param->Name) && param->IsOutput && param->OutputToOthers) //one genetic module needs output of one non-genetic module
	//{
	//	if(param->Dimension == DT_Array1D) name = "T";
	//	if(param->Dimension == DT_Raster) name = "D";
	//}
	//else if(param->IsOutput && param->Name.find("D_") != 0 && param->Name.find("T_") != 0)
	//{
	//	if(param->Dimension == DT_Array1D || param->Dimension == DT_Array2D) name = "T_" + param->Name;
	//	if(param->Dimension == DT_Raster || param->Dimension == DT_Single) name = "D_" + param->Name;
	//}

	//cout << "set " + name + " for module " + m_ModuleID << endl;
	//clock_t start = clock();
	const char* paramName = name.c_str();
	ostringstream oss;
	oss << nSubbasin << "_" << name;
	if (StringMatch(name, "weight"))
	{
		if (setting->dataTypeString() == "P")
			oss << "_P";
		else
			oss << "_M";
		//oss << "_" << setting->dataTypeString();
	}
	string remoteFileName = oss.str();

	switch(param->Dimension)
	{
	case DT_Unknown:
		throw ModelException("ModuleFactory","SetValue","Type of " + param->Name + " is unknown.");
		break;
	case DT_Single:
		SetValue(param, templateRaster, settingsInput, pModule);
		break;
	case DT_Array1D:
		Set1DData(dbName, name, remoteFileName, templateRaster, pModule, settingsInput, vertitalItp);
		break;
	case DT_Array2D:
		Set2DData(dbName, name, nSubbasin, remoteFileName, templateRaster, pModule);
		break;
	case DT_Array3D:
		
		break;
	case DT_Array1DDateValue:
		break;
	case DT_Raster:
		SetRaster(dbName, name, remoteFileName, templateRaster, pModule);
		break;
	case DT_SiteInformation:
		break;
	case DT_LapseRateArray:
		//SetLapseRateArray();
		break;
	default:
		break;
	}
	//clock_t end = clock();
	//if(param->Dimension != DT_Single)
	//	cout << name << "\t" << end-start << endl;
}
//! Set Value
void ModuleFactory::SetValue(ParamInfo* param, clsRasterData* templateRaster, SettingsInput* settingsInput, SimulationModule* pModule)
{
	//get parameter datas
	if(StringMatch(param->Name, Tag_DataType)) 
	{
		// the data type is got from config.fig
		return;
	}	
	else if(StringMatch(param->Name, Tag_CellSize))  //cell size
	{
		param->Value = float(templateRaster->Size());
	}
	else if(StringMatch(param->Name, Tag_CellWidth))  //cell width
	{
		param->Value = float(templateRaster->getCellWidth());
	}
	else if(StringMatch(param->Name,Tag_TimeStep)) 
	{
		param->Value = settingsInput->getDtDaily();
	}
	else if (StringMatch(param->Name, Tag_StormTimeStep))
	{
		param->Value = settingsInput->getDtHillslope();
	}
	else if (StringMatch(param->Name, Tag_ChannelTimeStep)) 
	{
		param->Value = settingsInput->getDtChannel();
	}
	else
	{
		if (m_parametersInDB.find(GetUpper(param->Name)) != m_parametersInDB.end())
			param->Value = m_parametersInDB[GetUpper(param->Name)]->GetAdjustedValue();
	}

	pModule->SetValue(param->Name.c_str(), param->Value);
}
//! Set 1D data
void ModuleFactory::Set1DData(string& dbName, string& paraName, string& remoteFileName, clsRasterData* templateRaster, 
	SimulationModule* pModule, SettingsInput* settingsInput, bool vertitalItp)
{
	int n;
	float* data = NULL;

	if (m_1DArrayMap.find(remoteFileName) != m_1DArrayMap.end())
	{
		if (StringMatch(paraName, Tag_Weight))
		{
			clsInterpolationWeighData *weightData = m_weightDataMap[remoteFileName];
			weightData->getWeightData(&n, &data);
		}
		else
		{
			data = m_1DArrayMap[remoteFileName];
		}
		
		pModule->Set1DData(paraName.c_str(), m_1DLenMap[remoteFileName], data);
		return;
	}

	if (StringMatch(paraName, Tag_FLOWOUT_INDEX))
	{
		try
		{
#ifndef USE_MONGODB
			ostringstream oss;
			oss << dbName << remoteFileName << TextExtension;
			//cout << oss.str() << endl;
			Read1DArray(oss.str().c_str(), n, data);
			if(templateRaster->getCellNumber() != n)
			{
				oss.str("");
				oss << "The data length in " << remoteFileName << "(" << n << ")" << " is not the same as the template(" << templateRaster->getCellNumber() << ").\n";
				throw ModelException("Set1DData","Read1DArray", oss.str());
			}
#else
			Read1DArrayFromMongoDB(m_spatialData, remoteFileName, templateRaster, n, data);
#endif
		}
		catch (ModelException e)
		{
			cout << e.toString() << endl;
			return;
		}
	}
	else if (StringMatch(paraName, Tag_Weight))
	{
		#ifndef USE_MONGODB
			ostringstream ossWeightFile;
			ossWeightFile << dbName << remoteFileName << TextExtension;
			clsInterpolationWeighData *weightData = new clsInterpolationWeighData(ossWeightFile.str());
		#else
			clsInterpolationWeighData *weightData = new clsInterpolationWeighData(m_spatialData, remoteFileName.c_str());
		#endif
			weightData->getWeightData(&n, &data);
			//weightData->dump("c:/weight2.txt");
			m_weightDataMap[remoteFileName] = weightData;
	}
	else if (StringMatch(paraName, Tag_Elevation_Meteorology))
	{
		if (vertitalItp)
		{
			InputStation* pStation = settingsInput->StationData();
			n = pStation->NumberOfSites("M");
			data = pStation->GetElevation("M");
		}
		else
			return;
	}
	else if (StringMatch(paraName, Tag_Elevation_Precipitation))
	{
		if (vertitalItp)
		{
			InputStation* pStation = settingsInput->StationData();
			n = pStation->NumberOfSites("P");
			data = pStation->GetElevation("P");
		}
		else
			return;
	}
	else if (StringMatch(paraName, Tag_Latitude_Meteorology) && vertitalItp)
	{
		if (vertitalItp)
		{
			InputStation* pStation = settingsInput->StationData();
			n = pStation->NumberOfSites("M");
			data = pStation->GetLatitude("M");
		}
		else
			return;
	}

	if (data == NULL)
		throw ModelException("ModuleFactory", "Set1DData", "Failed reading file " + remoteFileName);

	if (!StringMatch(paraName, Tag_Weight))
	{
		m_1DArrayMap[remoteFileName] = data;
		m_1DLenMap[remoteFileName] = n;
	}

	pModule->Set1DData(paraName.c_str(), n, data);
}
//! Set 2D data
void ModuleFactory::Set2DData(string& dbName, string& paraName, int nSubbasin, string& remoteFileName, clsRasterData* templateRaster, SimulationModule* pModule)
{
	int nRows = 0;
	int nCols = 1;
	float **data;

	if (m_2DArrayMap.find(remoteFileName) != m_2DArrayMap.end())
	{
		data = m_2DArrayMap[remoteFileName];
		nRows = m_2DRowsLenMap[remoteFileName];
		nCols = m_2DColsLenMap[remoteFileName];
		
		pModule->Set2DData(paraName.c_str(), nRows, nCols, data);
		return;
	}

	try
	{
		if (StringMatch(paraName, "Conductivity_2D") || StringMatch(paraName, "Porosity_2D") ||
			StringMatch(paraName, "Poreindex_2D") || StringMatch(paraName, "FieldCap_2D") ||
			StringMatch(paraName, "WiltingPoint_2D") || StringMatch(paraName, "Density_2D"))
		{
			Read2DSoilAttr(m_spatialData, remoteFileName.substr(0, remoteFileName.size()-3), templateRaster, nRows, data);
			nCols = 2;
		}
		// routing files
		else if (StringMatch(paraName,Tag_ROUTING_LAYERS))
		{
			//string ascFileName = projectPath + Name + TextExtension;
			ostringstream oss;
			if (m_layingMethod == UP_DOWN)
			{
				oss << remoteFileName << "_UP_DOWN";
				remoteFileName = oss.str();
			}
			else
			{
				oss << remoteFileName << "_DOWN_UP";
				remoteFileName = oss.str();
			}
		#ifndef USE_MONGODB
			ostringstream ossRoutingLayers;
			ossRoutingLayers << dbName << remoteFileName << TextExtension;
			Read2DArray(ossRoutingLayers.str().c_str(), nRows, data);
		#else
			Read2DArrayFromMongoDB(m_spatialData, remoteFileName, templateRaster, nRows, data);
		#endif
		}
		else if(StringMatch(paraName, Tag_FLOWIN_INDEX) || StringMatch(paraName, Tag_FLOWIN_INDEX_DINF)
			|| StringMatch(paraName, Tag_FLOWIN_PERCENTAGE_DINF) || StringMatch(paraName, Tag_FLOWOUT_INDEX_DINF)
			|| StringMatch(paraName, Tag_ROUTING_LAYERS_DINF))
		{
		#ifndef USE_MONGODB
			ostringstream ossFlowIn;
			ossFlowIn << dbName << remoteFileName << TextExtension;
			Read2DArray(ossFlowIn.str().c_str(), nRows, data);
		#else
			//string ascFileName = projectPath + Name + TextExtension;
			Read2DArrayFromMongoDB(m_spatialData, remoteFileName, templateRaster, nRows, data);
		#endif
		}
		else if(StringMatch(paraName, Tag_OL_IUH))
		{
			ReadIUHFromMongoDB(m_spatialData, remoteFileName, templateRaster, nRows, data);
		}
		else if (StringMatch(paraName, Tag_ReachParameter))
		{
		#ifndef USE_MONGODB
			ostringstream ossReachInfo;
			ossReachInfo << dbName << GetUpper(Tag_ReachParameter) << "_1" << TextExtension;
			#ifndef MULTIPLY_REACHES
			ReadSingleReachInfo(nSubbasin, ossReachInfo.str(), m_layingMethod, nRows, nCols, data);
			#else
			ReadMultiReachInfo(ossReachInfo.str(), m_layingMethod, nRows, nCols, data);
			#endif
		#else
			#ifndef MULTIPLY_REACHES
			ReadReachInfoFromMongoDB(m_layingMethod, dbName, m_spatialData, nSubbasin, remoteFileName, templateRaster, nRows, nCols, data);
			#else
			ReadMutltiReachInfoFromMongoDB(m_layingMethod, dbName, m_spatialData, remoteFileName, templateRaster, nRows, nCols, data);//
			#endif
		#endif
		}
		else if (StringMatch(paraName, Tag_RchParam))
		{
#ifndef MULTIPLY_REACHES
			ReadLongTermReachInfo(dbName, m_spatialData, nSubbasin, nRows, nCols, data);
#else
			ReadLongTermMutltiReachInfo(dbName, m_spatialData, nRows, nCols, data);//
#endif
		}
		else if (StringMatch(paraName, Tag_LapseRate))
		{
			nRows = 12;
			nCols = 3;
			data = new float *[nRows];
			for (int i = 0; i < nRows; i++)
			{
				data[i] = new float[nCols];
				data[i][0] = 0.03f; // P
				data[i][1] = -0.65f; // T
				data[i][2] = 0.f;    // PET
			}
		}
		else
		{
			throw ModelException("ModuleFactory", "Set2DData", "Failed reading file " + remoteFileName);
		}
	}
	catch (ModelException e)
	{
		cout << e.toString() << endl;
		return;
	}
	
	m_2DArrayMap[remoteFileName] = data;
	m_2DRowsLenMap[remoteFileName] = nRows;
	m_2DColsLenMap[remoteFileName] = nCols;
	
	
	pModule->Set2DData(paraName.c_str(), nRows, nCols, data);
}
//! Set raster data
void ModuleFactory::SetRaster(string& dbName, string& paraName, string& remoteFileName, clsRasterData* templateRaster, SimulationModule* pModule)
{
	int n;
	float* data;

	clsRasterData *raster = NULL;
	if (m_rsMap.find(remoteFileName) == m_rsMap.end())
	{
		try
		{
#ifndef USE_MONGODB
			ostringstream oss;
			raster = new clsRasterData();
			//oss << dbName << remoteFileName << GTiffExtension;
			//raster->ReadFromGDAL(oss.str(), templateRaster);
			oss << dbName << remoteFileName << RasterExtension;
			raster->readASCFile(oss.str(), templateRaster);
			//oss.str("");
			//oss << dbName << remoteFileName << "_COPY" << RasterExtension;
			//raster->outputAAIGrid(oss.str());
#else
			raster = new clsRasterData(m_spatialData, remoteFileName.c_str(), templateRaster);
			string upperName = GetUpper(paraName);
			raster->getRasterData(&n, &data);
			if (data != NULL && m_parametersInDB.find(upperName) != m_parametersInDB.end())
			    m_parametersInDB[upperName]->Adjust1DArray(n, data);
#endif
		}
		catch (ModelException e)
		{
			cout << e.toString() << endl;
			return;
		}
		m_rsMap[remoteFileName] = raster;
	}
	else
	{
		raster = m_rsMap[remoteFileName];
		//cout << remoteFileName << endl;
	}

	raster->getRasterData(&n, &data);
	
	if (data != NULL)
	{
		string upperName = GetUpper(paraName);
		pModule->Set1DData(paraName.c_str(), n, data);
	}


}
//! Update input
void ModuleFactory::UpdateInput(vector<SimulationModule*>& modules, SettingsInput* inputSetting, time_t t)
{
	size_t n = m_moduleIDs.size();
	string id;
	SimulationModule* pModule;
	for (size_t i = 0; i < n; i++)
	{
		id = m_moduleIDs[i];
		pModule = modules[i];
		vector<ParamInfo>& inputs = m_inputs[id];
		string dataType = m_settings[id]->dataTypeString();

		for (size_t j = 0; j < inputs.size(); j++)
		{
			ParamInfo& param = inputs[j];
			if(param.DependPara != NULL) 
				continue;	//the input which comes from other modules will not change when the date is change.
#ifdef linux
			if(strcasecmp(param.Name.c_str(), Contant_Input_Elevation) == 0 
				|| strcasecmp(param.Name.c_str(),Contant_Input_Latitude) == 0
				|| strcasecmp(param.Name.c_str(),Contant_Input_Xpr) == 0
				|| strcasecmp(param.Name.c_str(),Contant_Input_Ypr) == 0) 
				continue;
#else
			if(_stricmp(param.Name.c_str(), Contant_Input_Elevation) == 0
				|| _stricmp(param.Name.c_str(),Contant_Input_Latitude) == 0
				|| _stricmp(param.Name.c_str(),Contant_Input_Xpr) == 0
				|| _stricmp(param.Name.c_str(),Contant_Input_Ypr) == 0) 
				continue;
#endif
			if (dataType.length() > 0)
			{
				int n;
				float *data;
				
				inputSetting->StationData()->GetTimeSeriesData(t, dataType, &n, &data);
#ifdef linux
				if (strcasecmp(param.Name.c_str(), "PET") == 0)
#else
				if (_stricmp(param.Name.c_str(), "PET") == 0)
#endif
				{
					for (int iData = 0; iData < n; iData++)
					{
						data[iData] *= m_parametersInDB["K_PET"]->GetAdjustedValue();
					}
				}
				pModule->Set1DData("T", n, data);
			}
		}

	}

}
//! Get value from dependency modules
void ModuleFactory::GetValueFromDependencyModule(int iModule, vector<SimulationModule*>& modules)
{
	size_t n = m_moduleIDs.size();
	string id = m_moduleIDs[iModule];
	vector<ParamInfo>& inputs = m_inputs[id];
	for (size_t j = 0; j < inputs.size(); j++)
	{
		ParamInfo* dependParam = inputs[j].DependPara;
		if (dependParam == NULL)
			continue;

		size_t k = 0;
		for (k = 0; k < n; k++)
		{
			if (m_moduleIDs[k] == dependParam->ModuleID)
				break;
		}
		string compareName = GetComparableName(dependParam->Name);
		int dataLen;
		if (dependParam->Dimension == DT_Array1D || dependParam->Dimension == DT_Raster)
		{
			float* data;
			modules[k]->Get1DData(compareName.c_str(), &dataLen, &data);
			modules[iModule]->Set1DData(inputs[j].Name.c_str(), dataLen, data);
		}
		else if (dependParam->Dimension == DT_Array2D)
		{
			int nCol;
			float** data;
			modules[k]->Get2DData(compareName.c_str(), &dataLen, &nCol, &data);
			modules[iModule]->Set2DData(inputs[j].Name.c_str(), dataLen, nCol, data);
		}
		else if (dependParam->Dimension == DT_Single)
		{
			float value;
			modules[k]->GetValue(compareName.c_str(), &value);
			modules[iModule]->SetValue(inputs[j].Name.c_str(), value);
		}
		else
		{
			ostringstream oss;
			oss << "Dimension type: " << dependParam->Dimension << " is currently not supported.";
			throw ModelException("ModuleFactory", "GetValueFromDependencyModule", oss.str());
		}

	}
}
//! Find outputID parameter's module. Return Module index iModule and its ParamInfo
void ModuleFactory::FindOutputParameter(string& outputID, int& iModule, ParamInfo*& paraInfo)
{
	string compareName = outputID;
	//if (!StringMatch(compareName, "D_P"))
	//	compareName = GetComparableName(outputID);

	size_t n = m_moduleIDs.size();
	for (size_t i = 0; i < n; i++)
	{
		string id = m_moduleIDs[i];
		vector<ParamInfo>& vecPara = m_outputs[id];
		for (size_t j = 0; j < vecPara.size(); j++)
		{
			if (StringMatch(compareName, vecPara[j].Name))
			{
				iModule = i;
				paraInfo = &vecPara[j];
				return;
			}
		}
	}
}
//! Read multiply reach information from file
void ModuleFactory::ReadMultiReachInfo(const string &filename, LayeringMethod layeringMethod, int& nAttr, int& nReaches, float**& data)
{
	ifstream ifs(filename.c_str());
	int nAll;
	ifs >> nReaches >> nAll;
	nAttr = 5;
	data = new float*[nAttr];
	for (int i = 0; i < nAttr; i++)
	{
		data[i] = new float[nReaches];
	}

	string line;
	getline(ifs, line);
	utils utl;
	for (int i = 0; i < nReaches; ++i)
	{
		getline(ifs, line);
		vector<string> vec = utl.SplitString(line);
		data[0][i] = atof(vec[0].c_str());
		if (layeringMethod == UP_DOWN)
			data[1][i] = atof(vec[2].c_str());
		else
			data[1][i] = atof(vec[3].c_str());
		data[2][i] = atof(vec[1].c_str());//downstream id
		data[3][i] = atof(vec[4].c_str());//manning's n
		data[4][i] = atof(vec[5].c_str());
	}
	ifs.close();
}
//! Read single reach information 
void ModuleFactory::ReadSingleReachInfo(int nSubbasin, const string &filename, LayeringMethod layeringMethod, int& nAttr, int& nReaches, float**& data)
{
	ifstream ifs(filename.c_str());
	int nReachesAll, nAll;
	ifs >> nReachesAll >> nAll;
	nReaches = 1;
	nAttr = 5;
	data = new float*[nAttr];
	for (int i = 0; i < nAttr; i++)
	{
		data[i] = new float[nReaches];
	}

	string line;
	getline(ifs, line);
	utils utl;
	for (int i = 0; i < nReachesAll; ++i)
	{
		getline(ifs, line);
		vector<string> vec = utl.SplitString(line);
		int curSubbasin = atoi(vec[0].c_str());
		if (curSubbasin == nSubbasin)
		{
			data[0][0] = atof(vec[0].c_str());
			if (layeringMethod == UP_DOWN)
				data[1][0] = atof(vec[2].c_str());
			else
				data[1][0] = atof(vec[3].c_str());
			data[2][0] = atof(vec[1].c_str());//downstream id
			data[3][0] = atof(vec[4].c_str());//manning's n
			data[4][0] = atof(vec[5].c_str());

			//cout << data[0][0] << "\t" << data[1][0] << "\t" << data[2][0] << "\t" << data[3][0] << "\t" << data[4][0] << "\n";
			break;
		}
	}
	ifs.close();
}
