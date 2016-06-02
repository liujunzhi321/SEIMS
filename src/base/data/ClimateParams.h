/*!
 * \file ClimateParams.h
 * \brief Functions for climate related intermediate parameters, 
 *  e.g., saturation vapor pressure, max solar radiation.
 * \ingroup data
 * \author LiangJun Zhu
 * \date April 2016
 *
 */
#pragma once
#include <string>

using namespace std;
/*!
 * \brief Get the year
 * \return int year
*/
int		GetYear(time_t&);
/*!
 * \brief Get the Julian day of one day
 * \return int Julian day
*/
int		JulianDay(time_t&);
/*!
 * \brief Calculate latent heat of vaporization(MJ/kg)
 * \param[in] tmean Mean temperature
 * \return Latent heat of vaporization
*/
float	LatentHeatVapor(float &tmean);
/*!
 * \brief Calculate the max solar radiation for a station of one day
 *
 *
 * \param[in] day Julian day.
 * \param[in] lat Latitude of the station
 * \return float The max solar radiation.
*/
float	MaxSolarRadiation(int&,float&);
/*!
 * \brief Calculate mean barometric pressure
 * \param[in] elev elevation of current cell or site
 * \return mean atmospheric pressure (kPa)
*/
float	MeanBarometricPressure(float &elev);
/*!
 * \brief Calculate psychrometric constant
 * \param[in] elev elevation of current cell or site
 * \param[in] tmean Mean temperature
 * \sa MeanBarometricPressure(), LatentHeatVapor()
 * \return  Psychrometric constant
*/
float PsychrometricConst(float &tmean, float &elev);
/*!
 * \brief Calculates saturation vapor pressure at a given air temperature.
 * \param[in] float t: mean air temperature(deg C)
 * \return saturation vapor pressure(kPa)
*/
float	SaturationVaporPressure(float &t);
