#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SoilTemperatureFINPL.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SoilTemperatureFINPL();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Junzhi Liu");
	mdi.SetClass("Soil Temperature", "Calculate the soil temperature.");
	mdi.SetDescription("Finn Plauborg Method to compute soil temperature.");
	mdi.SetEmail(SEIMS_EMAIL);
	mdi.SetID("STP_FP");
	mdi.SetName("STP_FP");
	mdi.SetVersion("0.5");
	mdi.SetWebsite(SEIMS_SITE);
	mdi.SetHelpfile("STP_FP.html");

	/// from parameter database
	mdi.AddParameter(VAR_SOL_TA0, UNIT_NON_DIM, DESC_SOL_TA0,  Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_SOL_TA1, UNIT_NON_DIM, DESC_SOL_TA1,  Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_SOL_TA2, UNIT_NON_DIM, DESC_SOL_TA2,  Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_SOL_TA3, UNIT_NON_DIM, DESC_SOL_TA3,  Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_SOL_TB1, UNIT_NON_DIM, DESC_SOL_TB1,  Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_SOL_TB2, UNIT_NON_DIM, DESC_SOL_TB2,  Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_SOL_TD1, UNIT_NON_DIM, DESC_SOL_TD1,  Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_SOL_TD2, UNIT_NON_DIM, DESC_SOL_TD2,  Source_ParameterDB, DT_Single);

	mdi.AddParameter(VAR_K_SOIL10, UNIT_NON_DIM, DESC_K_SOIL10,  Source_ParameterDB, DT_Single);

	mdi.AddParameter(VAR_SOIL_T10, UNIT_NON_DIM, DESC_SOIL_T10,  Source_ParameterDB, DT_Raster1D);
	//mdi.AddParameter(Tag_Mask,  UNIT_NON_DIM, DESC_MASK,  Source_ParameterDB, DT_Raster);

	/// from interpolation module
	/// air temperature
	mdi.AddInput(VAR_TMIN, UNIT_TEMP_DEG, "Minimum air temperature", Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_TMAX, UNIT_TEMP_DEG, "Maximum air temperature", Source_Module, DT_Raster1D);
	//mdi.AddInput(VAR_TMIN1, UNIT_TEMP_DEG, DESC_TMIN1, Source_Module, DT_Raster);
	//mdi.AddInput(VAR_TMAX1, UNIT_TEMP_DEG, DESC_TMAX1, Source_Module, DT_Raster);
	//mdi.AddInput(VAR_TMIN2, UNIT_TEMP_DEG, DESC_TMIN2, Source_Module, DT_Raster);
	//mdi.AddInput(VAR_TMAX2, UNIT_TEMP_DEG, DESC_TMAX2, Source_Module, DT_Raster);

	/// output soil temperature
	mdi.AddOutput( VAR_SOTE, UNIT_TEMP_DEG, DESC_SOTE, DT_Raster1D);

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}