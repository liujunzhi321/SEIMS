/*!
 * \file PETHargreaves.h
 *
 * \author Junzhi Liu, LiangJun Zhu
 * \date Nov. 2010
 */
#ifndef SEIMS_PET_HARGREAVES_INCLUDE
#define SEIMS_PET_HARGREAVES_INCLUDE

#include <string>
#include "api.h"
#include "SimulationModule.h"

using namespace std;
/** \defgroup PET_H
 * \ingroup Hydrology_longterm
 * \brief Calculate potential evapotranspiration using Hargreaves method
 */
/*!
 * \class PETHargreaves
 * \ingroup PET_H
 *
 * \brief Hargreaves method to Compute PET
 *
 */
class PETHargreaves : public SimulationModule
{
public:
	PETHargreaves(void);
	~PETHargreaves(void);

	virtual void SetValue(const char* key, float value);
	virtual void Set1DData(const char* key, int n, float *value);
	virtual void Get1DData(const char* key, int* n, float **data);
	virtual int Execute();

private:
	// input from Database
	/// maximum air temperature for a given day(degree)
	float *m_tMax;
	/// minimum air temperature for a given day(degree)
	float *m_tMin;
	///latitude of the stations
	float *m_latitude;
	// input from other module
	/// mean air temperature for a given day(degree)
	float *m_tMean;
	/// maximum solar radiation
	float *m_srMax;
	/// size of the input array, i.e., the HydroClimate sites number
	int m_size;
	/// coefficient related to radiation used in Hargreaves method
	float m_HCoef_pet;
	/// Correction Factor for PET
	float m_petFactor;
	/// Julian day
	int m_jday;
	/// output PET array
	float *m_pet;
	
private:

	/*!
	 * \brief check the input data. Make sure all the input data is available.
	 * \return bool The validity of the input data.
	 */
	bool CheckInputData(void);

	/*!
	 * \brief check the input size. Make sure all the input data have same dimension.
	 *
	 *
	 * \param[in] key The key of the input data
	 * \param[in] n The input data dimension
	 * \return bool The validity of the dimension
	 */
	bool CheckInputSize(const char*,int);

// 	/*!
// 	 * \brief Calculate the max solar radiation for a station of one day
// 	 *
// 	 *
// 	 * \param[in] day Julian day.
// 	 * \param[in] lat Latitude of the station
// 	 * \return float The max solar radiation.
// 	 */
// 	float MaxSolarRadiation(int,float);

// 	/*!
// 	 * \brief Get the Julian day of one day
// 	 * \return int Julian day
// 	 */
// 	int JulianDay(time_t);
};

#endif