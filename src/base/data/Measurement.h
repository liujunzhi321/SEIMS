#pragma once
#include <string>
#include <vector>
#include "mongo.h"

using namespace std;

class Measurement
{
public:
	Measurement(mongo* conn, string hydroDBName, string sitesList, string siteType, time_t startDate, time_t endDate);
	~Measurement(void);

	virtual float* GetSiteDataByTime(time_t t) = 0;

	int NumberOfSites()
	{
		return m_siteIDList.size();
	}

	string Type()
	{
		return m_type;
	}

	time_t StartTime()
	{
		return m_startTime;
	}

	time_t EndTime()
	{
		return m_endTime;
	}

protected:
	mongo* m_conn;
	string m_hydroDBName;

	vector<int> m_siteIDList;
	string m_type;

	time_t m_startTime;
	time_t m_endTime;

	float *pData;
};

