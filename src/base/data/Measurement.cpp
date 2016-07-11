/*!
 * \file Measurement.cpp
 * \brief Measurement class to store HydroClimate site data
 *
 *
 *
 * \author Junzhi Liu, LiangJun Zhu
 * \version 1.1
 * \date May 2016
 *
 * 
 */
#include "Measurement.h"
#include "utils.h"
//#include "bson.h"
#include "util.h"
#include "ModelException.h"
//#include <algorithm>

Measurement::Measurement(mongoc_client_t *conn, string hydroDBName, string sitesList, string siteType, time_t startTime,
                         time_t endTime)
        : m_conn(conn), m_startTime(startTime), m_endTime(endTime), pData(NULL)
{
    vector<string> vecSites = utils::SplitString(sitesList, ',');
    int nSites = vecSites.size();
    //convert from string to int and sort
    for (int iSite = 0; iSite < nSites; iSite++)
        m_siteIDList.push_back(atoi(vecSites[iSite].c_str()));
    sort(m_siteIDList.begin(), m_siteIDList.end());
    pData = new float[nSites];
}

Measurement::~Measurement(void)
{
}

