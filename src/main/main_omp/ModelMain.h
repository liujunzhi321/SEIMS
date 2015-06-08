#pragma once

#include "SettingsInput.h"
#include "SettingsOutput.h"
//#include "SettingsConfig.h"
#include "clsRasterData.h"
#include "mongo.h"
#include "gridfs.h"
#include <string>
#include <set>
#include <map>
#include "ModuleFactory.h"
//#include "SimulationModule.h"

using namespace std;

class ModelMain
{
public:
	/**
	*	@brief constructor
	*	@param projectPath The path of the project. This path contains cofig.fig, file.in and file.out.
	*	@param databasePath The path of the database. This path constains HydroClimat.db3 and paramter.db3.
	*	@param modulePath The path of the modules. This path constains all the dlls of Wetspa modules.
	*/
	ModelMain(mongo* conn, string dbName, string projectPath, SettingsInput* input, 
		ModuleFactory *factory, int subBasinID = 1, int scenarioID = 0, int numThread = 1, LayeringMethod layeringMethod = UP_DOWN);
    ModelMain(mongo* conn, string dbName, string projectPath,
                 ModuleFactory *factory, int subBasinID, int scenarioID, LayeringMethod layeringMethod = UP_DOWN);
	~ModelMain(void);


	/**
	*	@brief Execute all the modules and create output.
	*/
	void Execute();
	void Output();
    void Init(SettingsInput* input, int numThread);

	void Step(time_t time);
	void StepOverland(time_t t);
	void StepChannel(time_t t);
	void SetChannelFlowIn(float value);

	time_t getStartTime()
	{
		return m_input->getStartTime();
	};

	time_t getEndTime()
	{
		return m_input->getEndTime();
	};

	time_t getDtHillSlope()
	{
		return m_dtHs;
	};

	time_t getDtChannel()
	{
		return m_dtCh;
	};

	time_t getDtDaily()
	{
		return m_dtDaily;
	};

	float GetQOutlet()
	{
		if(m_channelModules.size() == 0)
			return 0.f;
		float value;
		int index = m_channelModules[0];
		m_simulationModules[index]->GetValue("QOUTLET", &value);
		return value;
	}

	void SetLayeringMethod(LayeringMethod method)
	{
		m_layeringMethod = method;
	}

	void OutputExecuteTime();
	int GetReadDataTime()
	{
		return m_readFileTime;
	}

	int GetModuleCount()
	{
		return m_simulationModules.size();
	}

	string GetModuleID(int i)
	{
		return m_factory->GetModuleID(i);
	}

	int GetModuleExecuteTime(int i)
	{
		return m_executeTime[i];
	}

	bool IncludeChannelProcesses()
	{
		return m_channelModules.size() != 0;
	}
	
	void CloseGridFS();

	void StepHillSlope(time_t t, int subIndex);
	void Output(time_t time);
    bool IsInitialized() { return m_initialized; }
	
private:
	mongo *m_conn;
	gridfs m_outputGfs[1];
	
	string m_dbName;
	int m_subBasinID;
	map<string, ParamInfo*> m_parameters;
	ModuleFactory *m_factory;
	vector<SimulationModule*> m_simulationModules;
	vector<int> m_hillslopeModules;
	vector<int> m_channelModules;
	vector<int> m_ecoModules;
	
	vector<int> m_executeTime;
	int m_readFileTime;

	LayeringMethod m_layeringMethod;

	void Step(time_t t, vector<int>& moduleIndex, bool firstRun);

	//! check whether the output file is valid. 
	//! 1. The output id should be valid for moduls in config files;
	//! 2. The date range should be in the data range of file.in;
	//! This method should be called after config, input and output is initialed.
	void CheckOutput(gridfs* gfs);
	void CheckOutput(SettingsOutput* output,SettingsInput* input);

private:
	bool m_firstRun;
	bool m_firstRunOverland;
	bool m_firstRunChannel;

	string m_projectPath;
	string m_databasePath;
	string m_modulePath;

	/**
	*	@brief The input setting of the project.
	*/
	SettingsInput*	m_input;
	/**
	*	@brief The output setting of the project.
	*/
	SettingsOutput*	m_output;

	clsRasterData* m_templateRasterData;

	time_t m_dtDaily, m_dtHs, m_dtCh;

    bool m_initialized;
};

