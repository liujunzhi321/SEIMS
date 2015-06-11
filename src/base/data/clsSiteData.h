/*!
 * \file clsSiteData.h
 * \brief Read hydrological or meteorological site data 
 *
 *
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date June 2010
 *
 * 
 */
#pragma once

#include <map>
#include "SiteInfo.h"
#include "clsHydroClimateData.h"
#include "bson.h"
/*!
 * \ingroup data
 * \class clsSiteData
 *
 * \brief Read hydrological or meteorological site data
 *
 *
 *
 */
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

	///< Constructor for mongodb
	clsSiteData(int id, const bson* info);
	///< add hydro data type 
	void addDataTypeMongo(const string& type);
	///< get hydro data from mongodb
	clsHydroClimateData* getHydroClimateData(const string& type)
	{
		return m_timeSerieseData[type];
	}

private:
	void checkData(string,time_t,float);///< check data
	string m_databasePath; ///< database's path
	void readSiteBasicInfo(string tableName); ///< read site basic information

	map<string,clsHydroClimateData*> m_timeSerieseData; ///< time series data, key is data type and value is the pointer of \a clsHydroClimateData

	static string toString(time_t time, float value); ///< convert date time to string
}; 

