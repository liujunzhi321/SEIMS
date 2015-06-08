#pragma once

#include <map>
#include "SiteInfo.h"
#include "clsHydroClimateData.h"
#include "bson.h"

class clsSiteData :
	public SiteInfo
{
public:
	clsSiteData(string databasePath,string tableName);
	clsSiteData(string databasePath,int id,string type);

	~clsSiteData(void);

	void addDataType(string type);
	void loadTimeSeriesData(time_t startTime,time_t endTime,int interval);
	float getTimeSeriesData(time_t time,string type);
	bool isHasDataOfType(string type);
	void dump(ostream* fs);

	// for mongodb
	clsSiteData(int id, const bson* info);
	void addDataTypeMongo(const string& type);
	clsHydroClimateData* getHydroClimateData(const string& type)
	{
		return m_timeSerieseData[type];
	}

private:
	void checkData(string,time_t,float);
	string m_databasePath;
	void readSiteBasicInfo(string tableName);

	/*
	** Key is data type
	** value is the pointer of clsHydroClimateData
	*/
	map<string,clsHydroClimateData*> m_timeSerieseData;

	static string toString(time_t time, float value);
};

