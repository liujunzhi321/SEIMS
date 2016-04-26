/*!
 * \file api.cpp
 * \ingroup CLIM
 * \author ZhuLJ
 * \date April 2016
 *
 * 
 */
#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "ClimateParams.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new ClimateParameters();
}

//! function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;
	mdi.SetAuthor("LiangJun Zhu");
	mdi.SetClass(MCLS_CLIMATE, MCLSDESC_CLIMATE);
	mdi.SetDescription(MDESC_CLIM);
	mdi.SetEmail(SEIMS_EMAIL);
	mdi.SetID(MDESC_CLIM);
	mdi.SetName(MDESC_CLIM);
	mdi.SetVersion("1.0");
	mdi.SetWebsite(SEIMS_SITE);
	mdi.SetHelpfile("CLIMATE.html");

	// set the parameters
	//mdi.AddParameter(VAR_CO2,UNIT_GAS_PPMV,DESC_CO2,Source_ParameterDB,DT_Single);
	//mdi.AddParameter(VAR_COND_RATE,UNIT_CONDRATE_MSPA,DESC_CONDRATE,Source_ParameterDB,DT_Single);
	//mdi.AddParameter(VAR_COND_MAX,UNIT_SPEED_MS,DESC_MAXCOND,Source_ParameterDB,DT_Single);
	//mdi.AddParameter(VAR_SNOW_TEMP,UNIT_DEPTH_MM,DESC_SNOW_TEMP,Source_ParameterDB, DT_Single); 
	//mdi.AddParameter(VAR_PET_K, UNIT_NON_DIM, DESC_PET_K, Source_ParameterDB, DT_Single);
	//Now the canopy height is assumed as constant for everyday and is consider as a parameter and read from asc file.
	//After the plant growth module is completed, this should be converted to input variable.
	//mdi.AddParameter("CHT","m","Canopy height for the day","file_height.asc", Array1D);	

	//The elevation of station is read from HydroClimateDB. It would be consider as a parameter. And its name must be Elevation. 
	//This will force the main program to read elevation from HydroClimateDB.
	//mdi.AddParameter(Tag_Elevation_Meteorology,UNIT_LEN_M,CONS_IN_ELEV,Source_HydroClimateDB, DT_Array1D);

	//Latitude is used to calculate max solar radiation. It is read in the similar format with elevation.
	mdi.AddParameter(Tag_Latitude_Meteorology,UNIT_LONLAT_DEG,CONS_IN_LAT,Source_HydroClimateDB, DT_Array1D);

	// set the input
	//These five inputs are read from HydroClimateDB
	//! Current: use TMin and TMax to calculate TMean
	//! TODO: Use mean temperature if exists
	mdi.AddInput(DataType_MeanTemperature,UNIT_TEMP_DEG,DESC_MEANTEMP,Source_HydroClimateDB,DT_Array1D);
	mdi.AddInput(DataType_MinimumTemperature,UNIT_TEMP_DEG,DESC_MINTEMP,Source_HydroClimateDB, DT_Array1D);
	mdi.AddInput(DataType_MaximumTemperature,UNIT_TEMP_DEG,DESC_MAXTEMP,Source_HydroClimateDB, DT_Array1D);
	mdi.AddInput(DataType_RelativeAirMoisture,UNIT_NON_DIM,DESC_RM,Source_HydroClimateDB, DT_Array1D);
	mdi.AddInput(DataType_SolarRadiation,UNIT_SR,DESC_SR,Source_HydroClimateDB, DT_Array1D);
	mdi.AddInput(DataType_WindSpeed,UNIT_SPEED_MS,DESC_WS,Source_HydroClimateDB, DT_Array1D);

	//LAI needs to be calculated by other modules.
	//mdi.AddInput("LAIDAY","m2/m2","Leaf area index","Plant growth module", Array1D); //Wetspa Manual P17, need min LAI,max LAI and Julian day.

	//The albedo also needs plant growth module. 
	//Now, for testing, its value is determined by T_snow and T_MEAN.
	//mdi.AddInput("ALBDAY","","Albedo in the day","Plant growth module", Array1D);

	//Now, assume the grow code of all the stations is 1.
	//mdi.AddInput("IGRO","","Land cover status code","Management database", Array1D);

	// set the output variables
	mdi.AddOutput(VAR_SR_MAX,UNIT_SR,DESC_SR_MAX,DT_Array1D);
	mdi.AddOutput(VAR_VP_SAT,UNIT_PRESSURE,DESC_VP_SAT,DT_Array1D);
	mdi.AddOutput(VAR_VP_ACT,UNIT_PRESSURE,DESC_VP_ACT,DT_Array1D);

	string res = mdi.GetXMLDocument();
	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}