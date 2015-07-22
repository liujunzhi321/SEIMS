#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "AtmosphericDeposition.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"
#include "text.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new AtmosphericDeposition();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Wang Lin");
	mdi.SetClass("AtmosphericDeposition", "Atmospheric Deposition.");
	mdi.SetDescription("Atmospheric Deposition.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("AtmosphericDeposition.chm");
	mdi.SetID("AtmosphericDeposition");
	mdi.SetName("AtmosphericDeposition");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("RootDepth", "mm", "Depth from the soil surface", Source_ParameterDB, DT_Raster); 
	mdi.AddParameter("ConRainNitra", "mg N/L", " Concentration of nitrogen in rainfall", Source_ParameterDB, DT_Single); 
	mdi.AddParameter("ConRainAmm", "mg N/L", "Concentration of ammonia in rainfall", Source_ParameterDB, DT_Single);

	mdi.AddInput("D_P","mm","Precipitation",Source_Module,DT_Raster); ///< from ITP_P

	mdi.AddOutput("Nitrate", "kg N/ha", "amount of nitrate", DT_Array2D); ///< invoked by the other nutrient modules
	mdi.AddOutput("Ammon", "kg N/ha", "ammonium pool for soil nitrogen", DT_Array2D); ///< invoded by NITVOL
	mdi.AddOutput("Depth", "mm", "depth of the layer", DT_Array2D);  ///< invoked by MINDEC,NITVOL,NUTTRA_OL
	mdi.AddOutput("AddRainNitra", "kg N/ha", "nitrate added by rainfall", DT_Raster); ///< invoked by no module
	mdi.AddOutput("AddRainAmm", "kg N/ha", "ammonium added by rainfall", DT_Raster);  ///< invoked by no module

	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}