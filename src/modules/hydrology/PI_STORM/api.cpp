#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "Interception.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new clsPI_MSM();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res;

	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Alex Storey, Junzhi Liu");
	mdi.SetClass("Precipitation Interception", "Calculate precipitation interception.");
	mdi.SetDescription("Module to calculate precipitation interception using the maximum storage method.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("PI_STORM");
	mdi.SetName("PI_STORM");
	mdi.SetVersion("0.4");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("PI_STORM.chm");

	// set the input variables (time series)
	mdi.AddInput("D_P","mm","Precipitation","Module",DT_Raster);
	mdi.AddParameter("Slope", "%", "Slope", "ParameterDB_WaterBalance", DT_Raster); 
	//mdi.AddInput("D_PET","mm", "Potential Evapotranspiration", "Module", MapWindowRaster);
	mdi.AddParameter("DT_HS", "second", "Time step of the simulation", "file.in", DT_Single); 
	// set the parameters (non-time series)
	mdi.AddParameter("Interc_max","mm", "Maximum Interception Storage Capacity", "ParameterDB_Interception", DT_Raster);
	mdi.AddParameter("Interc_min","mm", "Minimum Interception Storage Capacity", "ParameterDB_Interception", DT_Raster);
	mdi.AddParameter("Pi_b","", "Interception Storage Capacity Exponent", "ParameterDB_Interception", DT_Single);
	//mdi.AddParameter("K_pet","", "Correction Factor of PET", "ParameterDB_Interception", Single);
	mdi.AddParameter("Init_IS","", "Initial interception storage", "ParameterDB_Interception", DT_Single);

	// set the output variables
	mdi.AddOutput("INLO","mm", "Interception Loss Distribution", DT_Raster);
	//mdi.AddOutput("INET","mm", "Evaporation From Interception Storage", MapWindowRaster);
	mdi.AddOutput("NEPR","mm", "Net Precipitation Distribution", DT_Raster);

	// set the dependencies
	mdi.AddDependency("Interpolation", "Interpolation Module");

	// write out the XML file.
	res = mdi.GetXMLDocument();

	//return res;

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}