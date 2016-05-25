/*!
 * \file api.cpp
 *
 * \author JunZhi Liu, LiangJun Zhu
 * \date April 2016
 *
 * 
 */
#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "PETPenmanMonteith.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"
/** \defgroup PET_PM
 * \ingroup Hydrology_longterm
 * \brief Calculate potential evapotranspiration using Penman-Monteith method
 *
 */
//! Get instance of SimulationModule class
extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new PETPenmanMonteith();
}

/*!
 * \ingroup PET_H
 * \brief function to return the XML Metadata document string
 */
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Junzhi Liu");
	mdi.SetClass(MCLS_PET, MCLSDESC_PET);
	mdi.SetDescription(MDESC_PET_PM);
	mdi.SetEmail(SEIMS_EMAIL);
	mdi.SetID(MID_PET_PM);
	mdi.SetName(MID_PET_PM);
	mdi.SetVersion("1.0");
	mdi.SetWebsite(SEIMS_SITE);
	mdi.SetHelpfile("PET_PM.html");

	// set the parameters
	mdi.AddParameter(VAR_CO2,UNIT_GAS_PPMV,DESC_CO2,Source_ParameterDB,DT_Single);
	mdi.AddParameter(VAR_COND_RATE,UNIT_CONDRATE_MSPA,DESC_CONDRATE,Source_ParameterDB,DT_Single);
	mdi.AddParameter(VAR_COND_MAX,UNIT_SPEED_MS,DESC_MAXCOND,Source_ParameterDB,DT_Single);
	mdi.AddParameter(VAR_SNOW_TEMP,UNIT_DEPTH_MM,DESC_SNOW_TEMP,Source_ParameterDB, DT_Single); 
	mdi.AddParameter(VAR_K_PET, UNIT_NON_DIM, DESC_PET_K, Source_ParameterDB, DT_Single);

	mdi.AddParameter(VAR_DEM,UNIT_LEN_M,CONS_IN_ELEV,Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_CELL_LAT, UNIT_LONLAT_DEG, DESC_CELL_LAT, Source_ParameterDB, DT_Raster1D);

	// set the input from other modules
	mdi.AddInput(DataType_MeanTemperature,UNIT_TEMP_DEG,DESC_MAXTEMP,Source_Module, DT_Raster1D);
	mdi.AddInput(DataType_MinimumTemperature,UNIT_TEMP_DEG,DESC_MINTEMP,Source_Module, DT_Array1D);
	mdi.AddInput(DataType_MaximumTemperature,UNIT_TEMP_DEG,DESC_MAXTEMP,Source_Module, DT_Array1D);
	mdi.AddInput(DataType_RelativeAirMoisture,UNIT_NON_DIM,DESC_RM,Source_Module, DT_Array1D);
	mdi.AddInput(DataType_SolarRadiation,UNIT_SR,DESC_SR,Source_Module, DT_Array1D);
	mdi.AddInput(DataType_WindSpeed,UNIT_SPEED_MS,DESC_WS,Source_Module, DT_Array1D);
	//Now the canopy height is assumed as constant for everyday and is consider as a parameter and read from asc file.
	//After the plant growth module is completed, this should be converted to input variable.
	//mdi.AddParameter("CHT","m","Canopy height for the day","file_height.asc", Array1D);	
	//LAI needs to be calculated by other modules.
	//mdi.AddInput("LAIDAY","m2/m2","Leaf area index","Plant growth module", Array1D); //Wetspa Manual P17, need min LAI,max LAI and Julian day?

	//The albedo also needs plant growth module. 
	//Now, for testing, its value is determined by T_snow and T_MEAN.
	//mdi.AddInput("ALBDAY","","Albedo in the day","Plant growth module", Array1D);

	//Now, assume the grow code of all the stations is 1.
	//mdi.AddInput("IGRO","","Land cover status code","Management database", Array1D);

	// set the output variables
	mdi.AddOutput(VAR_PET,UNIT_WTRDLT_MMD, DESC_PET, DT_Raster1D);
	mdi.AddOutput(VAR_PPT,UNIT_WTRDLT_MMD, DESC_PPT, DT_Raster1D);
	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}