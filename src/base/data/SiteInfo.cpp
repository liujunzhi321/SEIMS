/*!
 * \file SiteInfo.cpp
 * \brief Implementation of the methods for the SiteInfo class
 *
 *
 *
 * \author [your name]
 * \version 
 * \date June 2015
 *
 * 
 */
#include "SiteInfo.h"
#include "utils.h"

//! Constructor
SiteInfo::SiteInfo(void)
{
	Reset();
}

//! Destructor
SiteInfo::~SiteInfo(void)
{
}

//! Reset the contents of the object to default
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

//! Return the ID for the site
int SiteInfo::ID(void)
{
	return m_ID;
}

//! Return the name for the site
string SiteInfo::Name(void)
{
	return m_Name;
}

//! Return the XPR value for the site
float SiteInfo::XPR(void)
{
	return m_XPR;
}

//! Return the YPR value for the site
float SiteInfo::YPR(void)
{
	return m_YPR;
}

//! Return the latitude for the site
float SiteInfo::Latitude(void)
{
	return m_Latitude;
}

//! Return the longitude for the site
float SiteInfo::Longitude(void)
{
	return m_Longitude;
}

//! Return the elevation for the site
float SiteInfo::Elevation(void)
{
	return m_Elevation;
}

//! Return the area for the site
float SiteInfo::Area(void)
{
	return m_Area;
}
