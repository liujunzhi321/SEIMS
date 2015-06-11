/*!
 * \file InputStation.h
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

#include <map>
#include <vector>
#include <istream>
#include "mongo.h"
#include "Measurement.h"

using namespace std;
/*!
 * \ingroup data
 * \class InputStation
 *
 * \brief 
 *
 *
 *
 */
class InputStation
{
public:
	InputStation(mongo *conn, time_t dtHillslope, time_t dtChannel);
	~InputStation(void);

	int NumberOfSites(string siteType) {return m_numSites[siteType];}

	void GetTimeSeriesData(time_t time, string type, int* nRow, float** data);
	float* GetElevation(string type) {return m_elevation[type];}
	float* GetLatitude(string type) { return m_latitude[type];	}

	void ReadSitesData(string hydroDBName, string sitesList, string siteType, time_t startDate, time_t endDate, bool stormMode = false);

private:
	map<string, Measurement*> m_measurement;
	map<string, float*> m_elevation;
	map<string, float*> m_latitude;
	map<string, int> m_numSites;

	//for mongoDB
	mongo *m_conn;

	time_t m_dtHs, m_dtCh;

    void build_query_bson(int nSites, vector<int>& siteIDList, string& siteType,  bson *query);
	void ReadSitesInfo(string siteType, string hydroDBName, string sitesList);
};

