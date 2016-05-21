/*!
 * \file api.cpp
 *
 * \author ZhuLJ
 * \date April 2016
 *
 * 
 */
#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SNO_DD.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"
/** \defgroup SNO_DD
 * \ingroup Hydrology_longterm
 * \brief 
 *
 */
extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SNO_DD();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Chunping Ou");
	mdi.SetClass("Snowmelt", "Calculate the amount of snow melt.");
	mdi.SetDescription("For simplicity purpose for the time being, the algorithm used in the original WetSpa is incorporated in the SEIMS.");
	mdi.SetEmail(SEIMS_EMAIL);
	mdi.SetID("SNO_DD");
	mdi.SetName("SNO_DD");
	mdi.SetVersion("0.5");
	mdi.SetWebsite(SEIMS_SITE);
	mdi.SetHelpfile("SNO_DD.chm");

	mdi.AddParameter(VAR_C_SNOW, UNIT_TEMP_FACTOR, DESC_C_SNOW, Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_C_RAIN, UNIT_TEMP_FACTOR, DESC_C_RAIN, Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_T0, UNIT_TEMP_DEG, DESC_T0, Source_ParameterDB,DT_Single);	
	//mdi.AddParameter(VAR_K_BLOW, UNIT_NON_DIM, DESC_K_BLOW, Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_SNOW_TEMP, UNIT_TEMP_DEG, DESC_SNOW_TEMP, Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_SWE0, UNIT_DEPTH_MM, DESC_SWE0, Source_ParameterDB, DT_Single);

	mdi.AddInput(VAR_NEPR, UNIT_DEPTH_MM, DESC_NEPR, Source_Module, DT_Raster1D);								// from interception module
	//mdi.AddInput(VAR_SNAC, UNIT_DEPTH_MM, DESC_SNAC, Source_Module, DT_Raster1D);				// from snow water balance module
	//mdi.AddInput(VAR_SWE, UNIT_DEPTH_MM, DESC_SWE, Source_Module, DT_Single);									// from snow water balance module
	//mdi.AddInput(VAR_SNRD, UNIT_DEPTH_MM, DESC_SNRD, Source_Module, DT_Raster1D); // from snow redistribution module
	mdi.AddInput(VAR_SNSB, UNIT_DEPTH_MM, DESC_SNSB, Source_Module, DT_Raster1D); //from snow sublimation module
	mdi.AddInput(VAR_TMIN, UNIT_TEMP_DEG, DESC_TMIN, Source_Module, DT_Raster1D);									// from interpolation module
	mdi.AddInput(VAR_TMAX, UNIT_TEMP_DEG, DESC_TMAX, Source_Module, DT_Raster1D);									// from interpolation module


	// set the output variables
	mdi.AddOutput(VAR_SNME, UNIT_DEPTH_MM, DESC_SNME, DT_Raster1D);
	mdi.AddOutput(VAR_SNAC, UNIT_DEPTH_MM, DESC_SNAC, DT_Raster1D);
	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}