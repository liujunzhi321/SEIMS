/*!
 * \file SiteInfo.cpp
 * \brief Implementation of the methods for the SiteInfo class
 *
 *
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date June 2010
 *
 * 
 */
#include "SiteInfo.h"
#include "utils.h"


SiteInfo::SiteInfo(void)
{
    Reset();
}


SiteInfo::~SiteInfo(void)
{
}


void SiteInfo::Reset(void)
{
    m_ID = 0;
    m_Name = "";
    m_XPR = 0.0;
    m_YPR = 0.0;
    m_Latitude = 0.0;
    m_Longitude = 0.0;
    m_Elevation = 0.0;
    m_Area = 0.0;
}


int SiteInfo::ID(void)
{
    return m_ID;
}


string SiteInfo::Name(void)
{
    return m_Name;
}


float SiteInfo::XPR(void)
{
    return m_XPR;
}

float SiteInfo::YPR(void)
{
    return m_YPR;
}


float SiteInfo::Latitude(void)
{
    return m_Latitude;
}

float SiteInfo::Longitude(void)
{
    return m_Longitude;
}

float SiteInfo::Elevation(void)
{
    return m_Elevation;
}

float SiteInfo::Area(void)
{
    return m_Area;
}
