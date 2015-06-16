#include "ModelMain.h"
#include "text.h"
#include "utils.h"
#include "util.h"
#include "ModelException.h"
#include "PrintInfo.h"
#include "MongoUtil.h"
#include <ctime>
#include <sstream>



ModelMain::ModelMain(mongo* conn, string dbName, string projectPath, SettingsInput* input, 
						ModuleFactory *factory, int subBasinID, int scenarioID, int numThread, LayeringMethod layeringMethod):m_readFileTime(0), m_initialized(false)
{
	m_templateRasterData = NULL;
	m_subBasinID = subBasinID;

	m_projectPath = projectPath;
	m_conn = conn;
	m_dbName = dbName;

	gridfs spatialData[1];
	gridfs_init(m_conn, dbName.c_str(), "spatial", spatialData); 
	if(gridfs_init(m_conn, dbName.c_str(), "output", m_outputGfs) != MONGO_OK)
		cout << "gridfs_init output failed in ModelMain::ModelMain.\n"; 
	//cout << subBasinID << "\t" << m_outputGfs << endl;

	string dbNameStr = dbName;

	//if(!this->m_config->needScenario()) scenarioID = -1;	//-1 means this model doesn't need scenario information
	m_input = input;
	m_dtDaily = m_input->getDtDaily();
	m_dtHs = m_input->getDtHillslope();
	m_dtCh = m_input->getDtChannel();

	//m_output = output;
	m_output = new SettingsOutput(subBasinID, projectPath + File_Output, m_conn, m_outputGfs);
	CheckOutput(spatialData);
	m_factory = factory;
	m_layeringMethod = layeringMethod;
    
	m_readFileTime = factory->CreateModuleList(dbNameStr, subBasinID, numThread, m_layeringMethod, m_templateRasterData, m_input, m_simulationModules);
	//cout << "Read file time: " << m_readFileTime << endl;
	size_t n = m_simulationModules.size();
	m_executeTime.resize(n, 0);
	for (size_t i = 0; i < n; i++)
	{
		SimulationModule *pModule = m_simulationModules[i];

		switch(pModule->GetTimeStepType())
		{
		case TIMESTEP_HILLSLOPE:
			m_hillslopeModules.push_back(i);
			break;
		case TIMESTEP_CHANNEL:
			m_channelModules.push_back(i);
			break;
		case TIMESTEP_ECOLOGY:
			m_ecoModules.push_back(i);

		}
	}

	CheckOutput(this->m_output,this->m_input);

	//gridfs_destroy(spatialData);
	
	m_firstRunChannel = true;
	m_firstRunOverland = true;

    m_initialized = true;
}

ModelMain::ModelMain(mongo* conn, string dbName, string projectPath,  
						ModuleFactory *factory, int subBasinID, int scenarioID, LayeringMethod layeringMethod):m_readFileTime(0), m_initialized(false)
{
	m_templateRasterData = NULL;
	m_subBasinID = subBasinID;

	m_projectPath = projectPath;
	m_conn = conn;
	m_dbName = dbName;

	//if(!this->m_config->needScenario()) scenarioID = -1;	//-1 means this model doesn't need scenario information
	m_factory = factory;
	m_layeringMethod = layeringMethod;
    
	m_firstRunChannel = true;
	m_firstRunOverland = true;

}

void ModelMain::Init(SettingsInput* input, int numThread)
{
	m_input = input;
	m_dtDaily = m_input->getDtDaily();
	m_dtHs = m_input->getDtHillslope();
	m_dtCh = m_input->getDtChannel();

	m_output = new SettingsOutput(m_subBasinID, m_projectPath + File_Output, m_conn, m_outputGfs);

	gridfs spatialData[1];
	gridfs_init(m_conn, m_dbName.c_str(), "spatial", spatialData); 
	if(gridfs_init(m_conn, m_dbName.c_str(), "output", m_outputGfs) != MONGO_OK)
		cout << "gridfs_init output failed in ModelMain::ModelMain.\n"; 
	//cout << subBasinID << "\t" << m_outputGfs << endl;
	CheckOutput(spatialData);
    
	m_readFileTime = m_factory->CreateModuleList(m_dbName, m_subBasinID, numThread, m_layeringMethod, m_templateRasterData, m_input, m_simulationModules);
	//cout << "Read file time: " << m_readFileTime << endl;
	size_t n = m_simulationModules.size();
	m_executeTime.resize(n, 0);
	for (size_t i = 0; i < n; i++)
	{
		SimulationModule *pModule = m_simulationModules[i];

		switch(pModule->GetTimeStepType())
		{
		case TIMESTEP_HILLSLOPE:
			m_hillslopeModules.push_back(i);
			break;
		case TIMESTEP_CHANNEL:
			m_channelModules.push_back(i);
			//cout << "channel module: " << i << endl; 
			break;
		case TIMESTEP_ECOLOGY:
			m_ecoModules.push_back(i);

		}

		//string id = GetUpper(m_factory->GetModuleID(i));
		////if (StringMatch(m_factory->GetModuleID(i), CH_ROUTING_MODULE))
		//if (id.find("CH_") != string::npos || id.find("_CH") != string::npos)
		//{
		//	m_channelModules.push_back(i);
		//}
		//else
		//{
		//	m_hillslopeModules.push_back(i);
		//}
	}

	CheckOutput(this->m_output,this->m_input);

	//gridfs_destroy(spatialData);

    m_initialized = true;
}

void ModelMain::CloseGridFS()
{
	gridfs_destroy(m_outputGfs);
}

ModelMain::~ModelMain(void)
{
	StatusMessage("Start to release ModelMain ...");

	try
	{
		if(this->m_templateRasterData != NULL) delete this->m_templateRasterData;
		for (map<string, ParamInfo*>::iterator it = m_parameters.begin(); it != m_parameters.end(); ++it)
			delete it->second;
		delete m_output;
	}
	catch(exception e)
	{
		cout << e.what() << endl;
	}

	if(m_input != NULL)
		delete m_input;
}

void ModelMain::Step(time_t time)
{
	m_factory->UpdateInput(m_simulationModules, m_input, time);
	
	for (size_t i = 0; i < m_simulationModules.size(); i++)
	{
		SimulationModule *pModule = m_simulationModules[i];

		if (m_firstRun)
			m_factory->GetValueFromDependencyModule(i, m_simulationModules);
		//cout << m_factory->GetModuleID(i) << endl;
		clock_t sub_t1 = clock();
		
		try
		{
			pModule->SetDate(time);
			pModule->Execute();
		}
		catch (ModelException e)
		{
			cout << e.toString();
			exit(-1);
		}

		clock_t sub_t2 = clock();
		
		m_executeTime[i] += (sub_t2 - sub_t1);
	}
	
	Output(time);
	m_firstRun = false;
	m_firstRunOverland = false;
	m_firstRunChannel = false;
	//cout << m_subBasinID << "Step\n";
}

void ModelMain::StepOverland(time_t t)
{
	m_factory->UpdateInput(m_simulationModules, m_input, t);
	Step(t, m_hillslopeModules, m_firstRunOverland);
	m_firstRunOverland = false;
	//cout << m_subBasinID << "StepOverland\n";
}

void ModelMain::StepHillSlope(time_t t, int subIndex)
{
	m_factory->UpdateInput(m_simulationModules, m_input, t);

	for (size_t i = 0; i < m_hillslopeModules.size(); i++)
	{
		int index = m_hillslopeModules[i];
		SimulationModule *pModule = m_simulationModules[index];

		clock_t sub_t1 = clock();

		if (m_firstRunOverland)
			m_factory->GetValueFromDependencyModule(index, m_simulationModules);
		if (subIndex == 0)
			pModule->ResetSubTimeStep();
		pModule->SetDate(t);
		//cout << "\tHillslope process:" << i << endl;
		pModule->Execute();

		clock_t sub_t2 = clock();

		m_executeTime[index] += (sub_t2 - sub_t1);
	}

	m_firstRunOverland = false;
}

void ModelMain::StepChannel(time_t t)
{
	//cout << m_subBasinID << "StepChannelBegin\n";
	//cout << "Size of channel modules: " << m_channelModules.size() << endl;
	Step(t, m_channelModules, m_firstRunChannel);
	m_firstRunChannel = false;
}

void ModelMain::Step(time_t t, vector<int>& moduleIndex, bool firstRun)
{
	for (size_t i = 0; i < moduleIndex.size(); i++)
	{
		int index = moduleIndex[i];
		SimulationModule *pModule = m_simulationModules[index];

		if (firstRun)
			m_factory->GetValueFromDependencyModule(index, m_simulationModules);

		clock_t sub_t1 = clock();
		pModule->SetDate(t);
		pModule->Execute();
		clock_t sub_t2 = clock();

		m_executeTime[index] += (sub_t2 - sub_t1);
	}
}

void ModelMain::Execute()
{
	utils util;
	clock_t t1 = clock();

	time_t endTime = m_input->getEndTime();
	int nHs = 0;
	for(time_t t = m_input->getStartTime(); t < endTime; t += m_dtCh)
	{
		//cout << util.ConvertToString2(&t) << endl;
		nHs = int(m_dtCh/m_dtHs);
		for (int i = 0; i < nHs; i++)
			StepHillSlope(t+i*m_dtHs, i);
		StepChannel(t);

		Output(t);
	}

	//int n = m_simulationModules.size();
	//for(time_t t = m_input->getStartTime(); t <= m_input->getEndTime(); t += m_input->getIntervale())
	//{
	//	cout << util.ConvertToString2(&t) << endl;
	//	//StepOverland(t);
	//	//StepChannel(t);
	//	Step(t);
	//}
	clock_t t2 = clock();
	//cout << "time(ms):  " << t2-t1 << endl;
	cout << "[TIMESPAN][COMPUTING]\tALL\t" << (t2-t1)/1000. << endl;
	OutputExecuteTime();

}

void ModelMain::Output()
{
	vector<PrintInfo *>::iterator it;
	for(it=this->m_output->m_printInfos.begin();it<m_output->m_printInfos.end();it++)
	{
		vector<PrintInfoItem *>::iterator itemIt;
		for(itemIt=(*it)->m_PrintItems.begin();itemIt<(*it)->m_PrintItems.end();itemIt++)
		{	
			PrintInfoItem * item = *itemIt;
			item->Flush(m_projectPath,m_templateRasterData,(*it)->getOutputTimeSeriesHeader());			
		}
	}
}

void ModelMain::CheckOutput(gridfs* gfs)
{
	if(this->m_input == NULL) return;
	if(this->m_output == NULL) return;

	this->m_output->checkDate(m_input->getStartTime(),m_input->getEndTime());

	//if need to output to a asc file, create the asc header and position
	//if(this->m_output->isOutputASCFile())
	//{
	ostringstream oss;
#ifndef USE_MONGODB
	//oss << m_projectPath << m_subBasinID << "_" << NAME_MASK << GTiffExtension;
	m_templateRasterData = new clsRasterData();
	//m_templateRasterData->ReadFromGDAL(oss.str());
	oss << m_projectPath << m_subBasinID << "_" << NAME_MASK << RasterExtension;
	m_templateRasterData->readASCFile(oss.str());

#else
	oss << m_subBasinID << "_" << GetUpper(NAME_MASK);
	m_templateRasterData = new clsRasterData(gfs, oss.str().c_str());
#endif	
	//}
	this->m_output->setSpecificCellRasterOutput(this->m_projectPath,this->m_databasePath,m_templateRasterData);
}

void ModelMain::OutputExecuteTime()
{
	for (size_t i = 0; i < m_simulationModules.size(); i++)
	{
		cout << "[TIMESPAN][COMPUTING]\t" << m_factory->GetModuleID(i) << "\t" << m_executeTime[i]/1000. << endl;
	}
}

void ModelMain::CheckOutput(SettingsOutput* output,SettingsInput* input)
{
	vector<PrintInfo *>::iterator it;
	for(it=output->m_printInfos.begin();it<output->m_printInfos.end();it++)
	{
		string outputid = (*it)->getOutputID();
	    outputid = trim(outputid);

		//try to find module output which match the outputid
		m_factory->FindOutputParameter(outputid, (*it)->m_moduleIndex, (*it)->m_param);
		

		if((*it)->m_moduleIndex < 0) 
			throw ModelException("ModelMain","CheckOutput","Can't find output variable for output id "+ outputid + ". Please check config.fig, file.out and module's metadata.");
	
		//find site index
		//if((*it)->m_param->Dimension == DT_Array1D)
		//{
		//	vector<PrintInfoItem *>::iterator itemIt;
		//	for(itemIt=(*it)->m_PrintItems.begin();itemIt<(*it)->m_PrintItems.end();itemIt++)
		//	{	
		//		PrintInfoItem * item = *itemIt;

		//		if(item->SubbasinID != -1) continue;

		//		if(item->SiteID == -1) throw  ModelException("SettingsConfig","CheckOutput","The site id can't be -1 in output id "+outputid+".");

		//		//because different stations can have different types of data, so when looking for the station index,
		//		//the data type should be used.
		//		string dataType = (*it)->getOutputID();
		//		dataType = dataType.substr(2,dataType.length() -2);
		//		int siteIndex = input->StationData()->getSiteIndexFromID(item->SiteID,dataType); 

		//		if(siteIndex == -1) 
		//		{
		//			char s[20];
		//			strprintf(s,20,"%d",item->SiteID);
		//			throw ModelException("SettingsConfig","CheckOutput","Can't find site "+string(s) + " in input sites.");
		//		}

		//		item->SiteIndex = siteIndex;
		//	}
		//}

		//for output subbasin information
		//if((*it)->m_param->Dimension == DT_Array2D)
		//{
		//	int subbasinSeletedCount = -1;
		//	float* subbasinSelected = NULL;
		//	(*it)->getSubbasinSelected(&subbasinSeletedCount,&subbasinSelected);

		//	if(subbasinSeletedCount <= 0 || subbasinSelected == NULL) 
		//		throw ModelException("SettingsConfig","CheckOutput","Can't find subbasin information of output id "+outputid+".");

		//	(*it)->m_param->SubbasinSelected = subbasinSelected;
		//	(*it)->m_param->SubbasinSelectedCount = subbasinSeletedCount;

		//	if(module!=NULL) module->initialSubbasin(subbasinSeletedCount,subbasinSelected); //initial the subbasinselected parameter
		//}
	}
}

void ModelMain::Output(time_t time)
{	
	vector<PrintInfo *>::iterator it;
	for(it = m_output->m_printInfos.begin(); it < m_output->m_printInfos.end(); it++)
	{
		int iModule = (*it)->m_moduleIndex;

		//find the corresponding output variable and module
		ParamInfo* param = (*it)->m_param;
		if(param == NULL) 
			throw ModelException("ModelMain","Output","Output id "+ (*it)->getOutputID() + " does not have corresponding output variable.");

		SimulationModule* module = m_simulationModules[iModule];
		if(module == NULL) 
			throw ModelException("ModelMain","Output","Output id "+ (*it)->getOutputID() + " does not have corresponding module.");

		//process every output file
		vector<PrintInfoItem *>::iterator itemIt;
		for(itemIt=(*it)->m_PrintItems.begin();itemIt<(*it)->m_PrintItems.end();itemIt++)
		{	
			PrintInfoItem * item = *itemIt;
			const char* keyName = param->Name.c_str();
			//time_t t1 = item->getStartTime();
			//time_t t2 = item->getEndTime();
			if(time>=item->getStartTime() && time<=item->getEndTime())
			{		
				if (param->Dimension == DT_Single)
				{
					float value;
					module->GetValue(keyName, &value);
					item->TimeSeriesData[time] = value;
				}
				else if(param->Dimension == DT_Array1D)  //time series data for sites or some time series data for subbasins, such as T_SBOF,T_SBIF
				{
					//int index = item->SiteID;				//time series data for sites
					int index = -1;
					if(index < 0) 
						index = item->SubbasinID;		//time series data for some time series data for subbasins
					if(index < 0) 
						index = 0;
					//if(index < 0) throw ModelException("SettingsConfig","Output","The output "+(*it)->getOutputID()+" have some errors. There is no corresponding sites or subbasins.");
					
					int n;
					float *data;
					module->Get1DData(keyName, &n, &data);
					item->TimeSeriesData[time] = data[index];
				}
				else if(param->Dimension == DT_Array2D)  //time series data for subbasins
				{
					//some modules will calculate result for all subbasins or all reaches, 
					//regardless of whether they are output to file or not. In this case,
					//the 2-D array will contain all the results and the subbasinid or reachid
					//will be used to locate the result.
					if(	StringMatch(param->BasicName,"RECH") ||	//discharge of reach
						StringMatch(param->BasicName,"WABA") ||	//channel water balance
						StringMatch(param->BasicName,"RSWB")	||	//reservoir water balance
						StringMatch(param->BasicName,"RESB")	||  //reservoir sediment balance
						StringMatch(param->BasicName,"CHSB")	)
						//more conditions will be added in the future.
					{
						//for modules in which only the results of output subbasins are calculated.
						//In this case, the 2-D array just contain the results of selected subbasins in file.out.
						//So, the index of Subbasin in file.out will be used to locate the result.
						int subbasinIndex = item->SubbasinIndex;	

						subbasinIndex = item ->SubbasinID;

						if(subbasinIndex == -1) 
						{
							char s[20];
							strprintf(s,20,"%d",item->SubbasinID);
							throw ModelException("SettingsConfig","Output","Can't find subbasin "+string(s) + " in input sites.");
						}

						float** data;
						int nRows, nCols;
						module->Get2DData(param->BasicName.c_str(), &nRows, &nCols, &data);
						item->add1DTimeSeriesResult(time, nCols, data[0]);

						//if(subbasinIndex >= param->DataLength) 
						//{
						//	char s[20];
						//	strprintf(s,20,"%d",item->SubbasinID);
						//	throw ModelException("SettingsConfig","Output","The data index of subbasin "+string(s) + " is larger than data size.");
						//}

						//float* data = (param->FloatData2D)[subbasinIndex];
						//item->add1DTimeSeriesResult(time,param->DataWidth,data);
					}
					else
					{
						float** data;
						int nRows, nCols;
						module->Get2DData(param->BasicName.c_str(), &nRows, &nCols, &data);
						item->AggregateData2D(time, nRows, nCols, data);
					}
				}
				else if(param->Dimension == DT_Raster) //spatial distribution, calculate average,sum,min or max
				{
					int n;
					float *data;
                    //cout << keyName << " " << n << endl;
					module->Get1DData(keyName, &n, &data);
					item->AggregateData(time, n, data);
				}
			}
		}
	}
}


void ModelMain::SetChannelFlowIn(float value)
{
	int index = m_channelModules[0];
	m_simulationModules[index]->SetValue("QUPREACH", value);
}
