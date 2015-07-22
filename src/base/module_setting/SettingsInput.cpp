/*!
 * \file SettingsInput.cpp
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
#include "SettingsInput.h"
#include "text.h"
#include "util.h"
#include "utils.h"
#include "DBManager.h"
#include "text.h"
#include "ModelException.h"
#include <set>
#include "MongoUtil.h"
#include <cstdlib>

using namespace std;

//! Constructor
SettingsInput::SettingsInput(string fileName, mongo* conn, string dbName, int nSubbasin, int scenarioID):m_subbasinID(nSubbasin)
{
	size_t index = fileName.find_last_of(SEP);
	string projectPath = fileName.substr(0,index+1);
	//m_inputReach = new InputReach(databasePath,projectPath);

	m_conn = conn;
	m_scenario = NULL;
	//if(scenarioID != -1) m_scenario = new Scenario(projectPath,scenarioID); //-1 means this model doesn't need scenario information
	
	LoadSettingsFromFile(fileName,dbName);
}
//! Destructor 
SettingsInput::~SettingsInput(void)
{
	StatusMessage("Start to release SettingsInput ...");
	
	//map<int,clsSiteData*>::iterator it;
	//for(it = m_siteData.begin();it!=m_siteData.end();it++)
	//{
	//	if(it->second != NULL) delete it->second;
	//}
	//if(m_inputReach != NULL) delete m_inputReach;
	if(m_scenario != NULL) delete m_scenario;
	if(m_inputStation!=NULL) delete m_inputStation;
	//map<int,clsReachData*>::iterator it2;
	//for(it2 = m_reachData.begin();it2!=m_reachData.end();it2++)
	//{
	//	if(it2->second != NULL) delete it2->second;
	//}

	StatusMessage("End to release SettingsInput ...");
}
//! Read start and end date, simulation mode and time interval
bool SettingsInput::readDate()
{
	//read start and end time
	utils utl;
	m_startDate = utl.ConvertToTime2(Value(Tag_StartTime), "%d-%d-%d %d:%d:%d", true);
	m_endDate = utl.ConvertToTime2(Value(Tag_EndTime), "%d-%d-%d %d:%d:%d", true);

	if (m_startDate == -1 || m_endDate == -1) return false;
	
	// make sure the start and end times are in the proper order
	if (m_endDate < m_startDate)
	{
		time_t tmp = m_startDate;
		m_startDate = m_endDate;
		m_endDate = tmp;
	}

	m_mode = GetUpper(Value(Tag_Mode));

	//read interval
	vector<string> dtList = utl.SplitString(Value(Tag_Interval), ',');
	m_dtHs = atoi(dtList[0].c_str());
	m_dtCh = m_dtHs;
	if (dtList.size() > 1)
		m_dtCh = atoi(dtList[1].c_str());

	if (StringMatch(m_mode, "Daily"))
	{
		m_dtHs = 86400; // 86400 secs is 1 day
		m_dtCh = 86400;
	}
		// convert the time interval to seconds to conform to time_t struct
	return true;
}

bool SettingsInput::readTimeSeriesData()
{
	for (size_t i=0; i<m_Settings.size(); i++)
	{		
		//if (StringMatch(m_Settings[i][0], Tag_SiteName) && m_Settings[i].size() == 3 )
		//{
		//	m_inputStation->readOneStationData(atoi(m_Settings[i][1].c_str()), trim(m_Settings[i][2]));
		//} 
		//if (StringMatch(m_Settings[i][0], Tag_ReachName) && m_Settings[i].size() == 4)
		//{	
		//	m_inputReach->readOneReachData(m_Settings[i][1],m_Settings[i][2],m_Settings[i][3]);
		//} 
	}
	
	return true;
}

void SettingsInput::buildTimeQuery(time_t startTime, time_t endTime, bson* query)
{
	//bson_append_start_object(query, "MEASURE_TIME");
	//bson_append_date(query, "$gte", startTime*1000);
	//bson_append_date(query, "$lte", endTime*1000);
	//bson_append_finish_object(query);
	
}
//! build query for mongodb
void SettingsInput::buildQuery(const set<int>& idSet, const string& type, bson* query)
{
	set<int>::iterator it;

	// id
	if (!idSet.empty())
	{
		bson con_id[1];
		bson_init(con_id);
		bson_append_start_array(con_id, "$in");
		int i = 0;
		ostringstream oss;
		for (it = idSet.begin(); it != idSet.end(); ++it)
		{
			oss.str("");
			oss << i;
			bson_append_int(con_id, oss.str().c_str(), *it);
			i++;
		}
		bson_append_finish_array(con_id);
		bson_finish(con_id);
		bson_append_bson(query, "ID", con_id);
	}

	// type
	bson_append_string(query, "TYPE", type.c_str());

}


void SettingsInput::ReadSiteList()
{
	bson query[1];
	bson_init(query);

	// subbasin id
	bson_append_int(query, "SubbasinID", m_subbasinID);
	// mode
	bson_append_string(query, "Mode", m_mode.c_str());

	bson_finish(query);
	//bson_print(query);

	string siteListTable = SITELIST_TABLE;
	bool stormMode = false;
	if (StringMatch(m_mode, "Storm"))
		stormMode = true;
	mongo_cursor cursor[1];
	ostringstream oss;
	oss << m_dbName << "." << siteListTable;
	mongo_cursor_init(cursor, m_conn, oss.str().c_str());
	mongo_cursor_set_query(cursor, query);

	// loop stations of one class of climate data, such as "P"
	while( mongo_cursor_next(cursor) == MONGO_OK ) 
	{
		const bson* stationInfo = mongo_cursor_bson(cursor);
		bson_iterator it[1];

		if (bson_find(it, stationInfo, "DB")) 
			m_dbHydro = bson_iterator_string(it);
		else
			throw ModelException("SettingsInput", "ReadSiteList", "The DB field does not exist in SiteList table.");

		if (bson_find(it, stationInfo, "SiteListM")) 
		{
			string siteList = bson_iterator_string(it);
			m_inputStation->ReadSitesData(m_dbHydro, siteList, DataType_MaximumTemperature, m_startDate, m_endDate);
			m_inputStation->ReadSitesData(m_dbHydro, siteList, DataType_MinimumTemperature, m_startDate, m_endDate);
			m_inputStation->ReadSitesData(m_dbHydro, siteList, DataType_WindSpeed, m_startDate, m_endDate);
			m_inputStation->ReadSitesData(m_dbHydro, siteList, DataType_SolarRadiation, m_startDate, m_endDate);
			m_inputStation->ReadSitesData(m_dbHydro, siteList, DataType_RelativeAirMoisture, m_startDate, m_endDate);
			//m_inputStation->ReadSitesData(m_dbHydro, siteList, DataType_Precipitation, m_startDate, m_endDate);
		}
		
		if (bson_find(it, stationInfo, "SiteListP")) 
			m_inputStation->ReadSitesData(m_dbHydro, bson_iterator_string(it), "P", m_startDate, m_endDate, stormMode);

		if (bson_find(it, stationInfo, "SiteListPET")) 
			m_inputStation->ReadSitesData(m_dbHydro, bson_iterator_string(it), "PET", m_startDate, m_endDate, stormMode);

	}

	bson_destroy(query);
	mongo_cursor_destroy(cursor);
}

bool SettingsInput::LoadSettingsFromFile(string filename,string dbName)
{
	m_dbName = dbName;

	//first get the settingstrings from baseclass LoadSettingsFromFile function
	if(!Settings::LoadSettingsFromFile(filename)) 
		throw ModelException("SettingsInput","LoadSettingsFromFile",
		"The file.in is invalid. Please check it.");
	if(!readDate()) 
		throw ModelException("SettingsInput","LoadSettingsFromFile",
		"The start time and end time in file.in is invalid. The format would be YYYY/MM/DD/HH. Please check it.");
	
	m_inputStation = new InputStation(m_conn, m_dtHs, m_dtCh);
	ReadSiteList();

	return true;
}

void SettingsInput::Dump(string fileName)
{
	ofstream fs;
	utils util;
	fs.open(fileName.c_str(), ios::out);
	if (fs.is_open())
	{
		fs << "Start Date :" << util.ConvertToString2(&m_startDate) <<  endl;
		fs << "End Date :" << util.ConvertToString2(&m_endDate) <<  endl;
		fs << "Interval :" << m_dtHs << "\t" << m_dtCh <<  endl;

		//m_inputStation->dump(&fs);
		//m_inputReach->dump(&fs);
		if(m_scenario!=NULL) m_scenario->Dump(&fs);
		
		fs.close();
	}
}

//InputReach* SettingsInput::ReachData()
//{
//	return m_inputReach;
//}

Scenario* SettingsInput::BMPScenario()
{
	return m_scenario;
}

InputStation* SettingsInput::StationData()
{
	return m_inputStation;
}

time_t SettingsInput::getStartTime() const
{
	return m_startDate;
}

time_t SettingsInput::getEndTime() const
{
	return m_endDate;
}

time_t SettingsInput::getDtHillslope() const
{
	return m_dtHs;
}

time_t SettingsInput::getDtChannel() const
{
	return m_dtCh;
}

time_t SettingsInput::getDtDaily() const
{
	return 86400;
}