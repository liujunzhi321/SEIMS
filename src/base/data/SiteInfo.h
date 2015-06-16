/*!
 * \file SiteInfo.h
 * \brief Class to store the site information from the Hydroclimate database stations table
 *
 *
 *
 * \author [your name]
 * \version 
 * \date June 2015
 *
 * 
 */
//! 
#pragma once

#include <string>

using namespace std;
/*!
 * \ingroup data
 * \class SiteInfo
 *
 * \brief 
 *
 *
 *
 */
class SiteInfo
{
protected:
	int		m_ID;
	string	m_Name;
	float	m_XPR;
	float	m_YPR;
	float	m_Latitude;
	float	m_Longitude;
	float	m_Elevation;	
	float	m_Area;
public:
	SiteInfo(void);
	~SiteInfo(void);

	int		ID(void);
	string	Name(void);
	float	XPR(void);
	float	YPR(void);
	float	Latitude(void);
	float	Longitude(void);
	float	Elevation(void);
	float	Area(void);

	void	Reset(void);
};

