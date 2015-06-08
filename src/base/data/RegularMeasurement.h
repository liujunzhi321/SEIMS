#pragma once
#include "Measurement.h"

class RegularMeasurement : public Measurement
{
public:
	RegularMeasurement(mongo* conn, string hydroDBName, string sitesList, string siteType, time_t startTime, time_t endTime, time_t interval);
	~RegularMeasurement(void);

	virtual float* GetSiteDataByTime(time_t t);

private:
	/// site data : the first dimension is time, and the second dimension is sites
	vector<float*> m_siteData;

	time_t m_interval;
};

