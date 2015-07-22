/*!
 * \file ModuleFactory.h
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
#pragma once
#include <map>
#include <string>
#include <vector>
#include "ParamInfo.h"
#include "SimulationModule.h"
#include "tinyxml.h"
#include "SEIMS_ModuleSetting.h"
#include "clsRasterData.h"
#include "mongo.h"
#include "gridfs.h"
#include "clsInterpolationWeighData.h"
#include "clsRasterData.h"
#include "SettingsInput.h"
#ifndef linux
#include <Windows.h>
#endif


using namespace std;
/*!
 * \ingroup module_setting
 * \class ModuleFactory
 *
 * \brief 
 *
 *
 *
 */
class ModuleFactory 
{
public:
	ModuleFactory(const string& configFileName, const string& modulePath, mongo* conn, const string& dbName);
	~ModuleFactory(void);

	/// create a set of objects and set up the relationship among them
	int CreateModuleList(string dbName, int subbasinID, int numThreads, LayeringMethod layeringMethod, 
		clsRasterData* templateRaster, SettingsInput *settingsInput, vector<SimulationModule*>& modules);
	void UpdateInput(vector<SimulationModule*>& modules, SettingsInput* input, time_t t);
	void GetValueFromDependencyModule(int iModule, vector<SimulationModule*>& modules);
	void FindOutputParameter(string& outputID, int& iModule, ParamInfo*& paraInfo);
	string GetModuleID(int i)
	{
		return m_moduleIDs[i];
	}
private:
	typedef SimulationModule* (*InstanceFunction)(void);
	typedef const char* (*MetadataFunction)(void);
	/// function pointers used to create class instance
	map<string, InstanceFunction> m_instanceFuncs;
	map<string, MetadataFunction> m_metadataFuncs;

	string m_modulePath;
	LayeringMethod m_layingMethod;

	vector<string> m_moduleIDs;
#ifndef linux
	vector<HINSTANCE> m_dllHandles;
#else
	vector<void*> m_dllHandles;
#endif
	map<string, SEIMSModuleSetting*> m_settings;
	map<string, const char*> m_metadata;
	map<string, vector<ParamInfo> > m_parameters;
	map<string, vector<ParamInfo> > m_inputs;
	map<string, vector<ParamInfo> > m_outputs;

	SettingsInput *m_setingsInput;

	//! for mongodb
	string m_host;
	int m_port;
	string m_dbName;

	mongo *m_conn;
	gridfs m_spatialData[1];
	map<string, ParamInfo*> m_parametersInDB; ///< Store parameters from Database

	// data management
	map<string, float*> m_1DArrayMap;
	map<string, int> m_1DLenMap;
	map<string, float**> m_2DArrayMap;
	map<string, int> m_2DRowsLenMap;
	map<string, int> m_2DColsLenMap;
	map<string, clsInterpolationWeighData*> m_weightDataMap;
	map<string, clsRasterData*> m_rsMap;

private:
	/// read the config.fig file and initialize
	void Init(const string& configFileName);
	
	bool LoadSettingsFromFile(const char* filename, vector< vector<string> >& settings);
	void ReadConfigFile(const char* configFileName);
	void ReadDLL(string& moduleID, string& dllID);
	SimulationModule* GetInstance(string& moduleID);

	dimensionTypes MatchType(string strType);
	bool IsConstantInputFromName(string& name);
	void ReadParameterSetting(string& moduleID, TiXmlDocument& doc, SEIMSModuleSetting* setting);
	void ReadInputSetting(string& moduleID, TiXmlDocument& doc, SEIMSModuleSetting* setting);
	void ReadOutputSetting(string& moduleID, TiXmlDocument& doc, SEIMSModuleSetting* setting);

	string GetComparableName(string& paraName);
	ParamInfo* FindDependentParam(ParamInfo& paramInfo);

	void ConnectMongoDB();
	void ReadParametersFromMongoDB();
	void ReadParametersFromSQLite();

	void SetData(string& dbName, int nSubbasin, SEIMSModuleSetting* setting, ParamInfo* param, clsRasterData* templateRaster, 
		SettingsInput* settingsInput, SimulationModule* pModule, bool vertitalItp);
	void SetValue(ParamInfo* param, clsRasterData* templateRaster, SettingsInput* settingsInput, SimulationModule* pModule);
	void Set1DData(string& dbName, string& paraName, string& remoteFileName, clsRasterData* templateRaster, SimulationModule* pModule, SettingsInput* settingsInput, bool vertitalItp);
	void Set2DData(string& dbName, string& paraName, int nSubbasin, string& remoteFileName, clsRasterData* templateRaster, SimulationModule* pModule);
	void SetRaster(string& dbName, string& paraName, string& remoteFileName, clsRasterData* templateRaster, SimulationModule* pModule);

	void ReadMultiReachInfo(const string &filename, LayeringMethod layeringMethod, int& nRows, int& nCols, float**& data);
	void ReadSingleReachInfo(int nSubbasin, const string &filename, LayeringMethod layeringMethod, int& nAttr, int& nReaches, float**& data);
};

