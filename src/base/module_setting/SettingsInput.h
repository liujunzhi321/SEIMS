/*!
 * \file SettingsInput.h
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
#include "Settings.h"
#include <map>
#include "InputStation.h"
#include "Scenario.h"
#include "mongo.h"
#include <set>

using namespace MainBMP;
/*!
 * \ingroup module_setting
 * \class SettingsInput
 *
 * \brief inherited from Settings class
 *
 *
 *
 */
class SettingsInput :
	public Settings
{
public:
	SettingsInput(string fileName, mongo* conn, string dbName, int nSubbasin = 1, int scenarioID = -1);
	~SettingsInput(void);
		
	void Dump(string);

	time_t getStartTime(void) const;
	time_t getEndTime(void) const;
	time_t getDtHillslope(void) const;
	time_t getDtChannel(void) const;
	time_t getDtDaily() const;

	//InputReach* ReachData();//This part is replaced by new scenario class. Zhiqiang,2011-5-31
	Scenario* BMPScenario();
	InputStation* StationData();
private:
	//basic information
	time_t			m_startDate;	///< start date
	time_t			m_endDate;		///< end date
	time_t			m_dtHs;		///< time interval for hillslope
	time_t			m_dtCh;     ///< time interval for channel

	//data of stations and reaches
	//InputReach*		m_inputReach; //This part is replaced by new scenario class. Zhiqiang,2011-5-31
	InputStation*	m_inputStation;
	Scenario*		m_scenario;

	//the path of database
	string m_dbName;
	string m_dbHydro;
	mongo* m_conn;

	map<string, vector<int> > m_siteListMap;
	int m_subbasinID;
	string m_mode; ///< simulation mode, can be DAILY or HOURLY
private:
	bool LoadSettingsFromFile(string,string);
	bool readDate(void);
	bool readTimeSeriesData(void);
	
	void buildQuery(const set<int>& idSet, const string& type, bson* query);
	void buildTimeQuery(time_t startTime, time_t endTime, bson* query);

	void ReadSiteList();
};

