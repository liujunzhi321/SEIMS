/*!
 * \file clsSiteData.h
 * \brief Read hydrological or meteorological site data 
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
class clsSiteData : public SiteInfo
{
public:
    //clsSiteData(string databasePath,string tableName);
    //clsSiteData(string databasePath,int id,string type);
    //! Constructor for MongoDB
    clsSiteData(int id, const bson_t *info);

    //! Destructor
    ~clsSiteData(void);

    void addDataType(string type);

    //! Add hydro data type
    void addDataTypeMongo(const string &type);

    void loadTimeSeriesData(time_t startTime, time_t endTime, int interval);

    //! Get hydro climate data
    clsHydroClimateData *getHydroClimateData(const string &type) { return m_timeSerieseData[type]; }

    float getTimeSeriesData(time_t time, string type);

    bool isHasDataOfType(string type);

    void dump(ostream *fs);

private:
    //! database path
    string m_databasePath;
    //! time series data, key is data type and value is the pointer of \a clsHydroClimateData
    map<string, clsHydroClimateData *> m_timeSerieseData;

    void checkData(string, time_t, float);

    ///< check data
    void readSiteBasicInfo(string tableName); ///< read site basic information
    static string toString(time_t time, float value); ///< convert date time to string
}; 

