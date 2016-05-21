#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SNO_WB.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SNO_WB();
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
	mdi.SetID("SNO_WB");
	mdi.SetName("SNO_WB");
	mdi.SetVersion("0.5");
	mdi.SetWebsite(SEIMS_SITE);
	mdi.SetHelpfile("SNO_WB.chm");

	mdi.AddParameter(Tag_CellSize, UNIT_NON_DIM, DESC_CellSize, Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_T0, UNIT_TEMP_DEG, DESC_T0, Source_ParameterDB,DT_Single);	
	mdi.AddParameter(VAR_K_BLOW, UNIT_NON_DIM, DESC_K_BLOW, Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_SNOW_TEMP, UNIT_TEMP_DEG, DESC_SNOW_TEMP, Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_SWE0, UNIT_DEPTH_MM, DESC_SWE0, Source_ParameterDB, DT_Single);
	mdi.AddParameter(Tag_SubbasinSelected,  UNIT_NON_DIM, DESC_SubbasinSelected, File_Output, DT_Array1D); //this list is used to contrain the output size. Its name must be subbasinSelected.
	mdi.AddParameter(VAR_SUBBSN,  UNIT_NON_DIM, DESC_SUBBSN,  Source_ParameterDB, DT_Raster1D); //this list is used to contrain the output size

	mdi.AddInput(VAR_NEPR, UNIT_DEPTH_MM, DESC_NEPR, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_SNRD, UNIT_DEPTH_MM, DESC_SNRD, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_SNSB, UNIT_DEPTH_MM, DESC_SNSB, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_SNME, UNIT_DEPTH_MM, DESC_SNME, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_TMIN, UNIT_TEMP_DEG, DESC_TMIN, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_TMAX, UNIT_TEMP_DEG, DESC_TMAX, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_PRECI, UNIT_DEPTH_MM, DESC_PRECI, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_T_WS, UNIT_SPEED_MS, DESC_T_WS, Source_Module, DT_Array1D);

	// set the output variables
	mdi.AddOutput(VAR_SNAC, UNIT_DEPTH_MM, DESC_SNAC, DT_Raster1D);
	mdi.AddOutput(VAR_SNWB, UNIT_DEPTH_MM, DESC_SNWB, DT_Array2D);
	mdi.AddOutput(VAR_SWE, UNIT_DEPTH_MM, DESC_SWE, DT_Single);

	// set the dependencies
	mdi.AddDependency("T", "average temperature obtained from the interpolation module");
	mdi.AddDependency("P", "average precipitation obtained from the interpolation module"); 
	mdi.AddDependency("Pnet", "net precipitation obtained from interception model used to calculate P in equation ");
	mdi.AddDependency("SA", "snow accumulation");
	mdi.AddDependency("SE", "snow sublimation");
	mdi.AddDependency("SR", "snow redistribution");
	mdi.AddDependency("SM", "snow melt");

	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}