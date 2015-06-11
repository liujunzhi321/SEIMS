/*!
 * \file Measurement.cpp
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
#include "Measurement.h"
#include "utils.h"
#include "bson.h"
#include "text.h"
#include "ModelException.h"
#include <algorithm>
//! Constructor
Measurement::Measurement(mongo* conn, string hydroDBName, string sitesList, string siteType, time_t startTime, time_t endTime)
	:m_conn(conn), m_startTime(startTime), m_endTime(endTime), pData(NULL)
{
	vector<string> vecSites = utils::SplitString(sitesList, ',');
	int nSites = vecSites.size();
	//convert from string to int and sort
	for (int iSite = 0; iSite < nSites; iSite++)
		m_siteIDList.push_back(atoi(vecSites[iSite].c_str()));
	sort(m_siteIDList.begin(), m_siteIDList.end());
	
	pData = new float[nSites];

}

//! Destructor
Measurement::~Measurement(void)
{
	delete pData;
}

