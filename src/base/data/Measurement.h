/*!
 * \file Measurement.h
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
#include <string>
#include <vector>
#include "mongo.h"

using namespace std;
/*!
 * \ingroup 
 * \class Measurement
 *
 * \brief Get site data by site type and time
 *
 *
 *
 */
class Measurement
{
public:
	Measurement(mongo* conn, string hydroDBName, string sitesList, string siteType, time_t startDate, time_t endDate);
	~Measurement(void);
	//! Get site data by time
	virtual float* GetSiteDataByTime(time_t t) = 0;
	//! sites number
	int NumberOfSites()
	{
		return m_siteIDList.size();
	}
	//! site type
	string Type()
	{
		return m_type;
	}
	//! start time
	time_t StartTime()
	{
		return m_startTime;
	}
	//! end time
	time_t EndTime()
	{
		return m_endTime;
	}

protected:
	mongo* m_conn; ///< mongo object
	string m_hydroDBName; ///< hydro database name

	vector<int> m_siteIDList; ///< site ID list
	string m_type; ///< site type

	time_t m_startTime;///< start time
	time_t m_endTime;///< end time

	float *pData; ///< site data
};

