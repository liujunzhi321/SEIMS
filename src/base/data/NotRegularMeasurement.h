/*!
 * \file NotRegularMeasurement.h
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
#include "Measurement.h"
#include <map>
/*!
 * \ingroup data
 * \class NotRegularMeasurement
 *
 * \brief inherited from Measurement class
 *
 *
 *
 */
class NotRegularMeasurement : public Measurement
{
public:
	NotRegularMeasurement(mongo* conn, string hydroDBName, string sitesList, string siteType, time_t startTime, time_t endTime);
	~NotRegularMeasurement(void);

	virtual float* GetSiteDataByTime(time_t t);

private:
	//! site data : the first dimension is site, and the second dimension is time
	vector< vector<time_t> > m_timeList;
	vector< vector<float> > m_valueList;
	vector<int> m_curIndexList; ///< initialized as 0

	
};

