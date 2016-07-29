#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SEDOL_IUH.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SEDOL_IUH();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Wu Hui");
	mdi.SetClass("Overland Sediment Routing", "Calculate overland sediment routing.");
	mdi.SetDescription("IUH overland method to calculate overland sediment routing, from Hossein Zeinivand (2009).");
	mdi.SetEmail("");
	mdi.SetHelpfile("SEDOL_IUH.chm");
	mdi.SetID("SEDOL_IUH");
	mdi.SetName("SEDOL_IUH");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://www.website.com");


	mdi.AddParameter("TimeStep","hr","time step of the simulation","file.in",DT_Single); 
	mdi.AddParameter("CellWidth","m","","file.in",DT_Single); 
	mdi.AddParameter(VAR_OL_SED_ECO1, UNIT_NON_DIM, DESC_OL_SED_ECO1, Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_OL_SED_ECO2, UNIT_NON_DIM, DESC_OL_SED_ECO2, Source_ParameterDB, DT_Single);
	mdi.AddParameter("ccoe","","calibration coefficient of overland erosion","ParameterDB_Sediment",DT_Single);
	mdi.AddParameter("USLE_K","","the soil erodibility factor","ParameterDB_Sediment",DT_Raster1D);
	mdi.AddParameter("USLE_C","","the cover management factor","ParameterDB_Sediment",DT_Raster1D);
	mdi.AddParameter("subbasin","","The subbasion grid","ParameterDB_Snow",DT_Raster1D); 
	mdi.AddParameter("Ol_iuh","","IUH of each grid cell","ParameterDB_Discharge",DT_Array2D);
	mdi.AddParameter("Manning","","Manning's roughness","ParameterDB_Discharge",DT_Raster1D);
	mdi.AddParameter("Slope", "%", "Slope", "ParameterDB_WaterBalance", DT_Raster1D); 

	mdi.AddInput("D_DETSplash","kg", "distribution of splash detachment","Module",DT_Raster1D);
	mdi.AddInput("D_SURU","mm","Distribution of surface runoff","Module",DT_Raster1D);
	
	// set the output variables
	mdi.AddOutput("SOER","kg", "distribution of soil erosion", DT_Raster1D);
	mdi.AddOutput("DETOverland","kg", "distribution of overland flow detachment", DT_Raster1D);
	mdi.AddOutput("SEDTOCH","kg", "sediment flowing to channel", DT_Array1D);
	mdi.AddOutput("SEDTOCH_T","kg", "Total sediment flowing to channel", DT_Single); // added by wu hui
	mdi.AddOutput(VAR_SED_DEP, UNIT_KG, DESC_SED_DEP, DT_Raster1D); // added by Gao, 2016-07-28
	mdi.AddOutput(VAR_SED_FLOW, UNIT_KG, DESC_SED_FLOW, DT_Raster1D); // added by Gao, 2016-07-28

	// set the dependencies
	mdi.AddDependency("DEP_FS","Depression Storage module"); 
	mdi.AddDependency("Soil Detachment","Rain and overland soil detachment erosion module");          // for DETSplash
	//mdi.AddDependency("Depression","Depression module");

	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}