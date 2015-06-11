/*!
 * \file RegularMeasurement.cpp
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
#include <fstream>
#include "RegularMeasurement.h"
#include "utils.h"
#include "bson.h"
#include "text.h"
#include "ModelException.h"
#include "util.h"
//! Constructor 
RegularMeasurement::RegularMeasurement(mongo* conn, string hydroDBName, string sitesList, string siteType, time_t startTime, time_t endTime, time_t interval)
	:Measurement(conn, hydroDBName, sitesList, siteType, startTime, endTime), m_interval(interval)
{
	int nSites = m_siteIDList.size();
	//////////////////////////////////////////////////////////////////////////
	// build query statement
	bson query[1];
	bson_init(query);

	bson_append_start_object( query, "$query" );
		// id in idlist
	    bson_append_start_object(query, "StationID");
		bson_append_start_array(query, "$in");
		ostringstream ossIndex;
		for (int iSite = 0; iSite < nSites; iSite++)
		{
			ossIndex.str("");
			ossIndex << iSite;
			bson_append_int(query, ossIndex.str().c_str(), m_siteIDList[iSite]);
		}
		bson_append_finish_object(query);
		bson_append_finish_object(query);

		// type == Type
		bson_append_string(query, "Type", siteType.c_str());
		
		// startTime < t < endTime
		bson_append_start_object(query, "UTCDateTime");
		bson_date_t bsonStart = bson_date_t(startTime)*1000;
		bson_date_t bsonEnd = bson_date_t(endTime)*1000;
		bson_append_date(query, "$gte", bsonStart);
		bson_append_date(query, "$lte", bsonEnd);
		bson_append_finish_object(query);

	bson_append_finish_object(query);

	// sort by time
	bson_append_start_object(query, "$orderby");
		bson_append_int(query, "StationID", 1);
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


	float value;
	int stationIDLast = -1;
	int stationID = -1;
	int iSite = -1;
	int index = 0;
	while(mongo_cursor_next(cursor) == MONGO_OK) 
	{
		const bson* record = mongo_cursor_bson(cursor);
		bson_iterator it[1];

		if (bson_find(it, record, "StationID"))
			stationID = bson_iterator_int(it);
		else
			throw ModelException("Measurement", "Measurement", "The Value field does not exist in DataValuesDay table.");

		if(stationID != stationIDLast)
		{
			iSite++;
			index = 0;
			stationIDLast = stationID;
		}

		if (m_siteData.size() < (index+1) )
		{
			float* pData = new float[nSites];
			for(int i = 0; i < nSites; i++)
				pData[i] = 0.f;
			m_siteData.push_back(pData);
		}

		if (bson_find(it, record, "Value"))
			value = (float)bson_iterator_double(it);
		else
			throw ModelException("RegularMeasurement", "Constructor", "The Value field does not exist in DataValuesDay table.");

		m_siteData[index][iSite] = value;

		
		index++;
	}
	
	if (index <= 0)
	{
		ostringstream oss;
		utils util;
		oss << "There are no " << siteType << " data available for sites:[" << sitesList << "] in database:" << hydroDBName 
			<< " during " << util.ConvertToString2(&m_startTime) << " to " << util.ConvertToString2(&m_endTime);
		throw ModelException("RegularMeasurement", "Constructor", oss.str());
	}
	else if (iSite+1 != nSites)
	{
		ostringstream oss;
		utils util;
		oss << "The number of sites should be " << nSites << " while the query result is " << iSite+1 <<" for sites:[" << sitesList << "] in database:" << hydroDBName 
			<< " during " << util.ConvertToString2(&m_startTime) << " to " << util.ConvertToString2(&m_endTime);
		throw ModelException("RegularMeasurement", "Constructor", oss.str());
	}

	//ofstream ofs;
	//ofs.open("C:/test.txt", ios::out);
	//for(int i = 0; i < m_siteData.size(); i++)
	//{
	//	for(int j = 0; j < nSites; j++)
	//		ofs << m_siteData[i][j];
	//	ofs << "\n";
	//}
	//ofs.close();
	mongo_cursor_destroy(cursor);
}

//! Destructor
RegularMeasurement::~RegularMeasurement(void)
{
	for (int i = 0; i < m_siteData.size(); i++)
		delete m_siteData[i];
}
//! Get site date by time
float* RegularMeasurement::GetSiteDataByTime(time_t t)
{
	int index = (t - m_startTime)/m_interval;

	if (index < 0)
		index = 0;
	
	int nSites = m_siteIDList.size();
	for (int i = 0; i < nSites; i++)
		pData[i] = m_siteData[index][i];

	return pData;
}
