#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SOL_WB.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SOL_WB();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	mdi.SetAuthor("Chunping Ou");
	mdi.SetClass("Soil water balance", "Soil water balance calculation.");
	mdi.SetDescription("Soil water balance.");
	mdi.SetEmail(SEIMS_EMAIL);
	mdi.SetID("SOL_WB");
	mdi.SetName("SOL_WB");
	mdi.SetVersion("0.3");
	mdi.SetWebsite(SEIMS_SITE);
	mdi.SetHelpfile("SOL_WB.chm");

	//mdi.AddParameter(Tag_SubbasinSelected, UNIT_NON_DIM, DESC_SubbasinSelected, File_Output, DT_Array1D); //this list is used to contrain the output size. Its name must be subbasinSelected.
	//mdi.AddParameter(VAR_SUBBSN, UNIT_NON_DIM, DESC_SUBBSN, Source_ParameterDB, DT_Raster1D); //this list is used to contrain the output size
	mdi.AddParameter(VAR_SOILDEPTH, UNIT_DEPTH_MM, DESC_SOILDEPTH, Source_ParameterDB, DT_Raster1D);

	mdi.AddInput(VAR_PCP, UNIT_DEPTH_MM, DESC_D_P, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_NEPR, UNIT_DEPTH_MM, DESC_NEPR, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_INLO, UNIT_DEPTH_MM, DESC_INLO, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_INET, UNIT_DEPTH_MM, DESC_INET, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_DPST, UNIT_DEPTH_MM, DESC_DPST, Source_Module, DT_Raster1D);	
	mdi.AddInput(VAR_DEET, UNIT_DEPTH_MM, DESC_DEET, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_INFIL, UNIT_DEPTH_MM, DESC_INFIL, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_SOET, UNIT_DEPTH_MM, DESC_SOET, Source_Module, DT_Raster1D);
	
	mdi.AddInput(VAR_REVAP, UNIT_DEPTH_MM, DESC_REVAP, Source_Module,DT_Raster1D);
	mdi.AddInput(VAR_SURU, UNIT_DEPTH_MM, DESC_SURU, Source_Module,DT_Raster1D);
	mdi.AddInput(VAR_T_RG, UNIT_DEPTH_MM, DESC_T_RG, Source_Module,DT_Array1D);	
	
	//mdi.AddInput(VAR_SNSB, UNIT_DEPTH_MM, DESC_SNSB, Source_Module,DT_Raster);
	mdi.AddInput(VAR_TMIN, UNIT_TEMP_DEG, DESC_TMIN, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_TMAX, UNIT_TEMP_DEG, DESC_TMAX, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_SOTE, UNIT_TEMP_DEG, DESC_SOTE, Source_Module, DT_Raster1D);

	mdi.AddInput(VAR_PERCO,  UNIT_DEPTH_MM, DESC_PERCO,  Source_Module, DT_Raster2D);	
	mdi.AddInput(VAR_SOMO,  UNIT_DEPTH_MM, DESC_SOMO,  Source_Module, DT_Raster2D);
	mdi.AddInput(VAR_SSRU,  UNIT_DEPTH_MM, DESC_SSRU,  Source_Module, DT_Raster2D);

	// set the output variables
	mdi.AddOutput(VAR_SOWB, UNIT_DEPTH_MM, DESC_SOWB, DT_Array2D);

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}