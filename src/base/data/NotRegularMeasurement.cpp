/*!
 * \file NotRegularMeasurement.cpp
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
#include "NotRegularMeasurement.h"
#include "utils.h"
#include "bson.h"
#include "text.h"
#include "ModelException.h"
//! Constructor
NotRegularMeasurement::NotRegularMeasurement(mongo* conn, string hydroDBName, string sitesList, string siteType, time_t startTime, time_t endTime)
	:Measurement(conn, hydroDBName, sitesList, siteType, startTime, endTime)
{
	int nSites = m_siteIDList.size();
	
	m_valueList.resize(nSites);
	m_timeList.resize(nSites);
	m_curIndexList.resize(nSites, 0);

	for (int iSite = 0; iSite < nSites; iSite++)
	{
		//////////////////////////////////////////////////////////////////////////
		// build query statement
		bson query[1];
		bson_init(query);
		bson_append_start_object( query, "$query" );
			// id == id
			bson_append_int(query, "StationID", m_siteIDList[iSite]);
			// type == Type
			bson_append_string(query, "Type", siteType.c_str());
		

			// startTime < t < endTime
			bson_date_t bsonStart = bson_date_t(startTime)*1000;
			bson_date_t bsonEnd = bson_date_t(endTime)*1000;
			bson_append_start_object(query, "UTCDateTime");
				bson_append_date(query, "$gte", bsonStart);
				bson_append_date(query, "$lte", bsonEnd);
			bson_append_finish_object(query);
		bson_append_finish_object(query);
		// sort by time
		bson_append_start_object(query, "$orderby");
		bson_append_int(query, "UTCDateTime", 1);
		bson_append_finish_object(query);

		bson_finish(query);
		//bson_print(query);

		//////////////////////////////////////////////////////////////////////////
		// perform query and read measurement data
		mongo_cursor cursor[1];
		ostringstream oss;
		oss.str("");
		oss << hydroDBName << ".DataValues";
		mongo_cursor_init(cursor, m_conn, oss.str().c_str());
		mongo_cursor_set_query(cursor, query);
		bson_date_t dt;
		float value;

		bool hasData = false;
		while( mongo_cursor_next(cursor) == MONGO_OK ) 
		{
			const bson* record = mongo_cursor_bson(cursor);
			bson_iterator it[1];

			if (bson_find(it, record, "Value"))
				value = (float)bson_iterator_double(it);
			else
				throw ModelException("NotRegularMeasurement", "NotRegularMeasurement", "The Value field does not exist in DataValuesDay table.");

			if (bson_find(it, record, "UTCDateTime"))
				dt = bson_iterator_date(it)/1000;
			else
				throw ModelException("NotRegularMeasurement", "NotRegularMeasurement", "The UTCDateTime field does not exist in DataValuesDay table.");

			m_timeList[iSite].push_back(dt);
			m_valueList[iSite].push_back(value);

			hasData = true;
		}
		mongo_cursor_destroy(cursor);

		if (!hasData)
		{
			ostringstream oss;
			utils util;
			oss << "There are no " << siteType << " data available for sites:[" << m_siteIDList[iSite] << "] in database:" << hydroDBName 
				<< " during " << util.ConvertToString2(&m_startTime) << " to " << util.ConvertToString2(&m_endTime);
			throw ModelException("NotRegularMeasurement", "Constructor", oss.str());
		}
	}
}

//! Destructor
NotRegularMeasurement::~NotRegularMeasurement(void)
{
}
//! Get site data by time
float* NotRegularMeasurement::GetSiteDataByTime(time_t t)
{
	for (int iSite = 0; iSite < m_siteIDList.size(); iSite++)
	{
		vector<time_t>& tlist = m_timeList[iSite];
		vector<float>& vlist = m_valueList[iSite];
		int curIndex = m_curIndexList[iSite];

		// find the index for current time
		// the nearest record before t
		while (curIndex < tlist.size() && tlist[curIndex] <= t)
			curIndex++;
		curIndex--;

		if (curIndex < 0)
		{
			pData[iSite] = 0.f;
			m_curIndexList[iSite] = 0;
		}
		else
		{
			pData[iSite] = vlist[curIndex];
			m_curIndexList[iSite] = curIndex;
		}

	}
	return pData;
}
