#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "DepressionFS.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new DepressionFS();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Junzhi Liu");
	mdi.SetClass("Depression", "Calculate depression storage.");
	mdi.SetDescription("A simple fill and spill method method to calculate depression storage.");
	mdi.SetEmail(SEIMS_EMAIL);
	mdi.SetHelpfile("DEP_FS.chm");
	mdi.SetID("DEP_FS");
	mdi.SetName("DEP_FS");
	mdi.SetVersion("0.1");
	mdi.SetWebsite(SEIMS_SITE);

	mdi.AddParameter("Depre_in","-","initial depression storage coefficient","ParameterDB_WaterBalance", DT_Single); 
	mdi.AddParameter("Depression","mm","Depression storage capacity","ParameterDB_WaterBalance", DT_Raster1D);

#ifndef STORM_MODE		
	mdi.AddInput("D_INLO","mm","evaporation from the interception storage obtained from the interception module","Module", DT_Raster1D);	//EI
	mdi.AddInput("D_PET", "mm","PET calculated from the spatial interpolation module", "Module", DT_Raster1D);		
	mdi.AddOutput("DEET", "mm", "Distribution of evaporation from depression storage", DT_Raster1D);//PET
#endif
	//mdi.AddInput("D_INFIL","mm","Infiltration calculated in the infiltration module", "Module", DT_Raster);							//Infiltration

	mdi.AddOutput("DPST", "mm", "Distribution of depression storage", DT_Raster1D);
	mdi.AddOutput("SURU", "mm", "Distribution of surface runoff", DT_Raster1D);
	mdi.AddOutput("STCAPSURPLUS", "mm", "surplus of storage capacity", DT_Raster1D);

	res = mdi.GetXMLDocument();
	//return res;

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}