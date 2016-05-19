#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "IUH_OL.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new IUH_OL();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Wu Hui");
	mdi.SetClass("Overland flow", "Calculate overland flow routing.");
	mdi.SetDescription("IUH overland method to calculate overland flow routing.");
	mdi.SetEmail(SEIMS_EMAIL);
	mdi.SetHelpfile("IUH_OL.chm");
	mdi.SetID("IUH_OL");
	mdi.SetName("IUH_OL");
	mdi.SetVersion("0.1");
	mdi.SetWebsite(SEIMS_SITE);


	mdi.AddParameter(Tag_TimeStep, UNIT_TIMESTEP_HOUR, DESC_TIMESTEP, File_Input, DT_Single); 
	mdi.AddParameter(Tag_CellSize, UNIT_LEN_M, DESC_CellSize, Source_ParameterDB, DT_Single); 
	mdi.AddParameter(Tag_CellWidth, UNIT_LEN_M, DESC_CellWidth, Source_ParameterDB, DT_Single); 

	mdi.AddParameter(VAR_OL_IUH, UNIT_NON_DIM, DESC_OL_IUH, Source_ParameterDB, DT_Array2D);

	mdi.AddParameter(VAR_SUBBSN, UNIT_NON_DIM, DESC_SUBBSN, Source_ParameterDB, DT_Raster1D);
	//mdi.AddParameter("uhminCell","","start time of IUH for each grid cell","ParameterDB_Discharge",DT_Array1D);
	//mdi.AddParameter("uhmaxCell","","end time of IUH for each grid cell","ParameterDB_Discharge",DT_Array1D);

	mdi.AddInput(VAR_SURU, UNIT_DEPTH_MM, DESC_SURU, Source_Module, DT_Raster1D);					//from depression module

	mdi.AddOutput(VAR_SBOF, UNIT_DEPTH_MM, DESC_SBOF, DT_Array1D);

	// set the dependencies
	mdi.AddDependency("DEP_FS","Depression Storage module");  

	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}