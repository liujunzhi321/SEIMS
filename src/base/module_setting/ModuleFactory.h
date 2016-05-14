/*!
 * \file ModuleFactory.h
 * \brief Constructor of ModuleFactory from config file
 *
 *
 *
 * \author Junzhi Liu, LiangJun Zhu
 * \version 1.1
 * \date June 2010
 *
 * 
 */
#pragma once
#include <map>
#include <string>
#include <vector>
#include "ParamInfo.h"
#include "SimulationModule.h"
#include "tinyxml.h"
#include "SEIMS_ModuleSetting.h"
#include "clsRasterData.h"
#include "mongoc.h"
#include "clsInterpolationWeightData.h"
#include "clsRasterData.h"
#include "SettingsInput.h"
#ifndef linux
#include <Windows.h>
#endif


using namespace std;

class ModuleFactory 
{
public:
	/*!
	 * \brief Constructor of ModuleFactory from config file
	 * By default, the layering method is UP_DOWN
	 * \param[in] configFileName config.fig file which contains modules list for Simulation
	 * \param[in] modelPath the path of your model
	 * \param[in] conn \a mongoc_client_t
	 * \param[in] dbName name of your model
	 */
	ModuleFactory(const string& configFileName, const string& modelPath, mongoc_client_t* conn, const string& dbName);
	//! Destructor
	~ModuleFactory(void);

	//! Create a set of objects and set up the relationship among them. Return time-consuming.
	int CreateModuleList(string dbName, int subbasinID, int numThreads, LayeringMethod layeringMethod, 
		clsRasterData* templateRaster, SettingsInput *settingsInput, vector<SimulationModule*>& modules);
	//! Update input
	void UpdateInput(vector<SimulationModule*>& modules, SettingsInput* input, time_t t);
	//! Get value from dependency modules
	void GetValueFromDependencyModule(int iModule, vector<SimulationModule*>& modules);
	//! Find outputID parameter's module. Return Module index iModule and its ParamInfo
	void FindOutputParameter(string& outputID, int& iModule, ParamInfo*& paraInfo);
	//! Get Module ID by index
	string GetModuleID(int i){return m_moduleIDs[i];}
private:
	typedef SimulationModule*			(*InstanceFunction)(void);
	typedef const char*					(*MetadataFunction)(void);
	//! Modules' instance map
	map<string, InstanceFunction>		m_instanceFuncs;
	//! Metadata map of modules
	map<string, MetadataFunction>		m_metadataFuncs;
	//! Module path
	string								m_modulePath;
	//! Layering method, can be UP_DOWN or DOWN_UP
	LayeringMethod						m_layingMethod;
	//! Module IDs
	vector<string>						m_moduleIDs;
#ifndef linux
	//! .DLL (or .so) handles
	vector<HINSTANCE>					m_dllHandles;
#else
	vector<void*>						m_dllHandles;
#endif
	//! Module settings, \a map<string, SEIMSModuleSetting*>
	map<string, SEIMSModuleSetting*>	m_settings;
	//! Metadata of modules
	map<string, const char*>			m_metadata;
	//! Parameters of modules
	map<string, vector<ParamInfo> >		m_parameters;
	//! Input of modules
	map<string, vector<ParamInfo> >		m_inputs;
	//! Output of modules
	map<string, vector<ParamInfo> >		m_outputs;
	//! Input settings, \sa SettingInput
	SettingsInput						*m_setingsInput;
	//! IP address of MongoDB
	string								m_host;
	//! MongoDB port
	int									m_port;
	//! Database name of the simulation model
	string								m_dbName;
	//! MongoDB Client
	mongoc_client_t						*m_conn;
	//! Mongo GridFS to store spatial data
	mongoc_gridfs_t						*m_spatialData;
	//! Store parameters from Database \sa m_dbName
	map<string, ParamInfo*>				m_parametersInDB; 

	//! 1D array data map
	map<string, float*>					m_1DArrayMap;
	//! 1D array data length map
	map<string, int>					m_1DLenMap;
	//! 2D array data map
	map<string, float**>				m_2DArrayMap;
	//! Row number of 2D array data map
	map<string, int>					m_2DRowsLenMap;
	//! Col number of 2D array data map
	map<string, int>					m_2DColsLenMap;
	//! Interpolation weight data map
	map<string, clsInterpolationWeightData*> m_weightDataMap;
	//! Raster data map
	map<string, clsRasterData*>			m_rsMap;

private:
	//! Initialization, read the config.fig file and initialize
	void Init(const string& configFileName);
	//! Load settings from file
	bool LoadSettingsFromFile(const char* filename, vector< vector<string> >& settings);
	//! Read configuration file 
	void ReadConfigFile(const char* configFileName);
	//! Load function pointers from .DLL or .so
	void ReadDLL(string& moduleID, string& dllID);
	//! Get module instance by moduleID
	SimulationModule* GetInstance(string& moduleID);
	//! Match data type, e.g., 1D array
	dimensionTypes MatchType(string strType);
	//! Is constant input?
	bool IsConstantInputFromName(string& name);
	//! Read module's parameters setting from XML string
	void ReadParameterSetting(string& moduleID, TiXmlDocument& doc, SEIMSModuleSetting* setting);
	//! Read module's input setting from XML string
	void ReadInputSetting(string& moduleID, TiXmlDocument& doc, SEIMSModuleSetting* setting);
	//! Read module's output setting from XML string
	void ReadOutputSetting(string& moduleID, TiXmlDocument& doc, SEIMSModuleSetting* setting);
	//! Get comparable name after underscore if necessary, e.g., T_PET => use PET
	string GetComparableName(string& paraName);
	//! Find dependent parameters 
	ParamInfo* FindDependentParam(ParamInfo& paramInfo);

	void ConnectMongoDB();
	//! Read parameter settings from MongoDB
	void ReadParametersFromMongoDB();
	//! Set data
	void SetData(string& dbName, int nSubbasin, SEIMSModuleSetting* setting, ParamInfo* param, clsRasterData* templateRaster, 
		SettingsInput* settingsInput, SimulationModule* pModule, bool vertitalItp);
	//! Set single Value
	void SetValue(ParamInfo* param, clsRasterData* templateRaster, SettingsInput* settingsInput, SimulationModule* pModule);
	//! Set 1D Data
	void Set1DData(string& dbName, string& paraName, string& remoteFileName, clsRasterData* templateRaster, SimulationModule* pModule, SettingsInput* settingsInput, bool vertitalItp);
	//! Set 2D Data
	void Set2DData(string& dbName, string& paraName, int nSubbasin, string& remoteFileName, clsRasterData* templateRaster, SimulationModule* pModule);
	//! Set raster data
	void SetRaster(string& dbName, string& paraName, string& remoteFileName, clsRasterData* templateRaster, SimulationModule* pModule);
	//! Read multiply reach information from file
	void ReadMultiReachInfo(const string &filename, LayeringMethod layeringMethod, int& nRows, int& nCols, float**& data);
	//! Read single reach information 
	void ReadSingleReachInfo(int nSubbasin, const string &filename, LayeringMethod layeringMethod, int& nAttr, int& nReaches, float**& data);
};

