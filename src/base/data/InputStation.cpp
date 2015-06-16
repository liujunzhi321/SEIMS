/*!
 * \file InputStation.cpp
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
#include <sstream>
#include <ctime>
#include "InputStation.h"
#include "util.h"
#include "utils.h"
#include "ModelException.h"
#include "text.h"


#include "RegularMeasurement.h"
#include "NotRegularMeasurement.h"

using namespace std;
//! Constructor
InputStation::InputStation(mongo *conn, time_t dtHillslope, time_t dtChannel) : m_conn(conn), m_dtHs(dtHillslope), m_dtCh(dtChannel)
{
}

//! Destructor
InputStation::~InputStation(void)
{
	map<string, Measurement*>::iterator it;
	for(it = m_measurement.begin(); it != m_measurement.end(); it++)
	{
		if(it->second != NULL) 
			delete it->second;
	}

	map<string, float*>::iterator it2;
	for (it2 = m_latitude.begin(); it2 != m_latitude.end(); it2++)
	{
		if(it2->second != NULL) 
			delete it2->second;
	}

	for (it2 = m_elevation.begin(); it2 != m_elevation.end(); it2++)
	{
		if(it2->second != NULL) 
			delete it2->second;
	}
}
//! build query bson object
void InputStation::build_query_bson(int nSites, vector<int>& siteIDList, string& siteType,  bson *query)
{
	bson_init(query);
		// id in idlist
	    bson_append_start_object(query, "ID");
		bson_append_start_array(query, "$in");
		ostringstream ossIndex;
		for (int iSite = 0; iSite < nSites; iSite++)
		{
			ossIndex.str("");
			ossIndex << iSite;
			bson_append_int(query, ossIndex.str().c_str(), siteIDList[iSite]);
		}
		bson_append_finish_object(query);
		bson_append_finish_object(query);
        if(siteType == "P")
        {
	        bson_append_start_object(query, "Type");
		    bson_append_start_array(query, "$in");
            bson_append_string(query, "0", "P");
            bson_append_string(query, "1", "M");
		    bson_append_finish_object(query);
		    bson_append_finish_object(query);
        }
        else
            bson_append_string(query, "Type", siteType.c_str());

	bson_append_finish_object(query);

	// sort by time
	bson_append_start_object(query, "$orderby");
		bson_append_int(query, "ID", 1);
	bson_append_finish_object(query);

	bson_finish(query);

}
//! Read site information
void InputStation::ReadSitesInfo(string siteType, string hydroDBName, string sitesList)
{
	vector<string> vecSites = utils::SplitString(sitesList, ',');
	int nSites = vecSites.size();
	//convert from string to int, the IDList is in order in MongoDB
	vector<int> siteIDList;
	for (int iSite = 0; iSite < nSites; iSite++)
		siteIDList.push_back(atoi(vecSites[iSite].c_str()));
	//sort(siteIDList.begin(), siteIDList.end());

	bson query[1];
    build_query_bson(nSites, siteIDList, siteType, query); 
	//bson_print(query);

	mongo_cursor cursor[1];
	ostringstream oss;
	oss.str("");
	oss << hydroDBName << ".Sites";
	//cout << hydroDBName << endl;
	mongo_cursor_init(cursor, m_conn, oss.str().c_str());
	mongo_cursor_set_query(cursor, query);

	float *pEle = new float[nSites];
	float *pLat = new float[nSites];
	const bson* record = NULL;
	float ele, lat;
	for (int i = 0; i < nSites; i++)
	{
		if( mongo_cursor_next(cursor) == MONGO_OK ) 
		{
			record = mongo_cursor_bson(cursor);
			bson_iterator it[1];
		
			if (bson_find(it, record, "Lat"))
				lat = (float)bson_iterator_double(it);
			else
				throw ModelException("InputStation", "ReadSitesInfo", "The Lat field does not exist in Sites table.");

			if (bson_find(it, record, "Elevation"))
				ele = (float)bson_iterator_double(it);
			else
				throw ModelException("InputStation", "ReadSitesInfo", "The Lat field does not exist in Sites table.");

			pLat[i] = lat;
			pEle[i] = ele;
		}
		else
		{
			throw ModelException("InputStation", "ReadSitesInfo", "Query failed.");
		}
		
	}
	mongo_cursor_destroy(cursor);

	m_elevation[siteType] = pEle;
	m_latitude[siteType] = pLat;
	m_numSites[siteType] = nSites;
}
//! Read site data
void InputStation::ReadSitesData(string hydroDBName, string sitesList, string siteType, time_t startDate, time_t endDate, bool stormMode)
{
	siteType = GetUpper(siteType);

	//clock_t start = clock();
	if (stormMode)
	{
		m_measurement[siteType] = new NotRegularMeasurement(m_conn, hydroDBName, sitesList, siteType, startDate, endDate);
	}
	else
	{
		m_measurement[siteType] = new RegularMeasurement(m_conn, hydroDBName, sitesList, siteType, startDate, endDate, m_dtHs);
	}
	//clock_t end = clock();
	//cout << "Read measurement " << siteType << " " << end - start << endl;

	//start = clock();
	if (StringMatch(siteType, "P"))
	{
		ReadSitesInfo("P", hydroDBName, sitesList);
	}
	else if (m_elevation.find("M") == m_elevation.end())
	{
		ReadSitesInfo("M", hydroDBName, sitesList);
	}
	//end = clock();
	//cout << "ReadSitesInfo " << siteType << " " << end - start << endl;
}
//! Get time series data
void InputStation::GetTimeSeriesData(time_t time, string type, int* nRow, float** data)
{
	Measurement* m = m_measurement[type];
	*nRow = m->NumberOfSites();
	//cout << type << "\t" << *nRow << endl;
	*data = m->GetSiteDataByTime(time);
}
