#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SSM_PE.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SSM_PE();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Chunping Ou");
	mdi.SetClass("Snow sublimation", "Calculate the amount of snow sublimation .");
	mdi.SetDescription("A simple method that used in the old WetSpa will be incorporated in the SEIMS as the default method.");
	mdi.SetEmail(SEIMS_EMAIL);
	mdi.SetID("SSM_PE");
	mdi.SetName("SSM_PE");
	mdi.SetVersion("0.5");
	mdi.SetWebsite(SEIMS_SITE);
	mdi.SetHelpfile("SSM_PE.chm");

	mdi.AddParameter(VAR_K_SUBLI, UNIT_NON_DIM, DESC_K_SUBLI, Source_ParameterDB,DT_Single);
	mdi.AddParameter(VAR_K_BLOW, UNIT_NON_DIM, DESC_K_BLOW, Source_ParameterDB,DT_Single);
	mdi.AddParameter(VAR_T0, UNIT_TEMP_DEG, DESC_T0, Source_ParameterDB,DT_Single);
	mdi.AddParameter(VAR_SNOW_TEMP, UNIT_TEMP_DEG, DESC_SNOW_TEMP, Source_ParameterDB,DT_Single);
	mdi.AddParameter(VAR_SWE0, UNIT_DEPTH_MM, DESC_SWE0, Source_ParameterDB,DT_Single);

	mdi.AddInput(VAR_PET, UNIT_DEPTH_MM, DESC_PET, Source_Module,DT_Raster1D);											// from interpolation module
	mdi.AddInput(VAR_NEPR, UNIT_DEPTH_MM, DESC_NEPR, Source_Module,DT_Raster1D);								// from interception module
	mdi.AddInput(VAR_SNAC, UNIT_DEPTH_MM, DESC_SNAC,  Source_Module,DT_Raster1D);				// from snow water balance module
	mdi.AddInput(VAR_SWE, UNIT_DEPTH_MM, DESC_SWE, Source_Module,DT_Single);									// from snow water balance module
	mdi.AddInput(VAR_SNRD, UNIT_DEPTH_MM, DESC_SNRD, Source_Module, DT_Raster1D); // from snow redistribution module
	mdi.AddInput(VAR_TMIN, UNIT_TEMP_DEG, DESC_TMIN, Source_Module,DT_Raster1D);									// from interpolation module
	mdi.AddInput(VAR_TMAX, UNIT_TEMP_DEG, DESC_TMAX, Source_Module,DT_Raster1D);									// from interpolation module

	// set the output variables
	mdi.AddOutput(VAR_SNSB, UNIT_DEPTH_MM, DESC_SNSB, DT_Raster1D);

	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}