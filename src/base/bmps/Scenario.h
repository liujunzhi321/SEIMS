#pragma once

#include <string>
#include "DBManager.h"
#include <map>
#include "BMPText.h"
#include "BMPBase.h"
#include <map>
#include <vector>
#include "BMPFactory.h"
#include "BMPReachFactory.h"
#include "BMPReachFlowDiversion.h"
#include "BMPReachPointSource.h"
#include "BMPReachReservoir.h"
#include "ManagementOperation.h"
#include "ManagementOperationPlant.h"
#include "ManagementOperationHarvest.h"
#include "ManagementOperationFertilizer.h"
#include "ManagementOperationTillage.h"
#include <ostream>
#include <iomanip>

using namespace std;

namespace MainBMP
{
	/*
	** Main class of scenario in BMP database
	** 
	** Scenario contains a collection of BMPFactory. Each BMPFactory
	** is corresponding to one type of BMP.
	**
	** Usage: (1) instantiate the class
	** 		  (2) load time series data using function loadTimeSeriesData
	*/
	class Scenario
	{
	public:
		Scenario(string projectPath,int scenarioID = 0);
		~Scenario(void);
		int ID();
		bool IsBaseScenario();
		string Name();

		void Dump(string fileName);
		void Dump(ostream* fs);
		/*
		** Load time seriese data for some reach structure
		*/
		void loadTimeSeriesData(string databasePath, time_t startTime, time_t endTime,int interval);

	public:
		//get reach structure
		BMPReachFlowDiversion*	getFlowDiversion(int reach);
		BMPReachPointSource*	getPointSource(int reach);
		BMPReachReservoir*		getReservoir(int reach);

		//reservoir max reservoir id
		int getMaxReservoirId();
		
		//-----------------------------------------------
		//areal non-structural bmp information

		//plant operation
		NonStructural::ManagementOperationPlant* getOperationPlant(
			int validCellIndex,int startYear,time_t currentTime)
		{return (NonStructural::ManagementOperationPlant*)getOperation(validCellIndex,startYear,currentTime,BMP_TYPE_CROP);};

		//harvest operation
		NonStructural::ManagementOperationHarvest* getOperationHarvest(
			int validCellIndex,int startYear,time_t currentTime)
		{return (NonStructural::ManagementOperationHarvest*)getOperation(validCellIndex,startYear,currentTime,BMP_TYPE_CROP,true);};

		//fertilizer operation
		NonStructural::ManagementOperationFertilizer* getOperationFertilizer(
			int validCellIndex,int startYear,time_t currentTime)
		{return (NonStructural::ManagementOperationFertilizer*)getOperation(validCellIndex,startYear,currentTime,BMP_TYPE_FERTILIZER);};

		//tillage operation
		NonStructural::ManagementOperationTillage* getOperationTillage(
			int validCellIndex,int startYear,time_t currentTime)
		{return (NonStructural::ManagementOperationTillage*)getOperation(validCellIndex,startYear,currentTime,BMP_TYPE_TILLAGE);};
		//-----------------------------------------------
	private:
		BMPReach* getReachStructure(int reach,int reachBMPid);
		NonStructural::ManagementOperation* getOperation(
			int validCellIndex,int startYear,time_t currentTime,int BMPid,bool useSecond = false);
	private:
		int m_id;
		string m_name;
		string m_projectPath;
	private:		
		void loadScenario(int scenarioID);
		void loadScenarioName();
		void loadBMPs();

		map<int,BMPFactory*> m_bmpFactories;
		void loadBMPDetail();

		string bmpPath();
	};
}



