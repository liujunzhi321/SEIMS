/*!
 * \file SiteInfo.h
 * \brief Class to store the site information from the Hydroclimate database stations table
 *
 *
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date June 2010
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
    //! HydroClimate station ID
    int m_ID;
    //! Station Name
    string m_Name;
    //! X coordinate (projection coordinate system)
    float m_XPR;
    //! Y coordinate (projection coordinate system)
    float m_YPR;
    //! Latitude
    float m_Latitude;
    //! Longitude
    float m_Longitude;
    //! Elevation
    float m_Elevation;
    //! Area
    float m_Area;
public:
    //! Constructor, set Reset()
    SiteInfo(void);

    //! Destructor
    ~SiteInfo(void);

    //! Return the ID for the site
    int ID(void);

    //! Return the name for the site
    string Name(void);

    //! Return the  X coordinate value for the site
    float XPR(void);

    //! Return the  X coordinate value for the site
    float YPR(void);

    //! Return the latitude of the site
    float Latitude(void);

    //! Return the Longitude of the site
    float Longitude(void);

    //! Return the Elevation of the site
    float Elevation(void);

    //! Return the Area of the site
    float Area(void);

    //! Reset all attributes values to zero or null
    void Reset(void);
};

