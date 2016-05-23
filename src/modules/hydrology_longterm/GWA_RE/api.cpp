#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "ReservoirMethod.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new ReservoirMethod();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Wu Hui");
	mdi.SetClass(MCLS_GW, MCLSDESC_GW);
	mdi.SetDescription("Reservoir Method to calculate groundwater balance and baseflow.");
	mdi.SetEmail(SEIMS_EMAIL);
	mdi.SetHelpfile("GWA_RE.chm");
	mdi.SetID("GWA_RE");
	mdi.SetName("GWA_RE");
	mdi.SetVersion("0.1");
	mdi.SetWebsite(SEIMS_SITE);

	mdi.AddParameter(Tag_TimeStep, UNIT_TIMESTEP_HOUR, DESC_TIMESTEP ,File_Config, DT_Single); 
	mdi.AddParameter(Tag_CellSize, UNIT_NON_DIM, DESC_CellSize, Source_ParameterDB, DT_Single);
	mdi.AddParameter(Tag_CellWidth, UNIT_LEN_M, DESC_CellWidth, Source_ParameterDB, DT_Single);

	mdi.AddParameter(VAR_GW0, UNIT_DEPTH_MM, DESC_GW0, Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_GWMAX, UNIT_DEPTH_MM, DESC_GWMAX, Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_DF_COEF, UNIT_NON_DIM, DESC_DF_COEF, Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_KG, UNIT_NON_DIM, DESC_KG, Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_Base_ex, UNIT_NON_DIM, DESC_BASE_EX, Source_ParameterDB, DT_Single);

	mdi.AddParameter(VAR_ROOTDEPTH, UNIT_DEPTH_MM, DESC_ROOTDEPTH, Source_ParameterDB,DT_Raster1D);
	//mdi.AddParameter(Tag_SubbasinSelected, UNIT_NON_DIM, "The subbasion ids listed in file.out", File_Output, DT_Array1D);
	mdi.AddParameter(VAR_SUBBSN, UNIT_NON_DIM, DESC_SUBBSN, Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_SLOPE, UNIT_PERCENT, DESC_SLOPE, Source_ParameterDB, DT_Raster1D);
	mdi.AddInput(VAR_INET, UNIT_DEPTH_MM, DESC_INET, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_DEET, UNIT_DEPTH_MM, DESC_DEET, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_SOET, UNIT_DEPTH_MM, DESC_SOET, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_PET, UNIT_DEPTH_MM, DESC_PET, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_GWNEW, UNIT_DEPTH_MM, DESC_GWNEW, Source_Module, DT_Array1D);//from IUH_CH or other channel routing module

	mdi.AddInput(VAR_PERCO,UNIT_DEPTH_MM, DESC_PERCO, Source_Module,DT_Raster2D);
	mdi.AddInput(VAR_SOMO, UNIT_DEPTH_MM, DESC_SOMO, Source_Module, DT_Raster2D);

	mdi.AddOutput(VAR_GWWB, UNIT_NON_DIM, DESC_NONE, DT_Array2D);					//used for output file
	mdi.AddOutput(VAR_REVAP, UNIT_NON_DIM, DESC_NONE, DT_Raster1D);		//used by soil water balance module
	mdi.AddOutput(VAR_RG, UNIT_NON_DIM, DESC_NONE, DT_Array1D);					//used by soil water balance module
	mdi.AddOutput(VAR_SBQG, UNIT_NON_DIM, DESC_NONE, DT_Array1D);					//used by channel flow routing module
	mdi.AddOutput(VAR_SBPET, UNIT_NON_DIM, DESC_NONE, DT_Array1D);	
	mdi.AddOutput(VAR_SBGS, UNIT_NON_DIM, DESC_NONE, DT_Array1D);	

	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}