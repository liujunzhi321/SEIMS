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
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("STP_FP");
	mdi.SetName("STP_FP");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("STP_FP.html");

	/// from parameter database
	mdi.AddParameter("soil_ta0","","Coefficient a0 for Finn Plauborg Method",
		"ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("soil_ta1","","Coefficient a1 for Finn Plauborg Method",
		"ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("soil_ta2","","Coefficient a2 for Finn Plauborg Method",
		"ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("soil_ta3","","Coefficient a3 for Finn Plauborg Method",
		"ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("soil_tb1","","Coefficient b1 for Finn Plauborg Method",
		"ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("soil_tb2","","Coefficient b2 for Finn Plauborg Method",
		"ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("soil_td1","","Coefficient d1 for Finn Plauborg Method",
		"ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("soil_td2","","Coefficient d2 for Finn Plauborg Method",
		"ParameterDB_WaterBalance", DT_Single);

	mdi.AddParameter("k_soil10","","Ratio between soil temperature at 10 cm and the mean",
		"ParameterDB_WaterBalance", DT_Single);

	mdi.AddParameter("soil_t10","","Factor of soil temperature relative to short grass (degree)",
		"ParameterDB_WaterBalance", DT_Raster);
	//mdi.AddParameter("Mask", "", "Array containing the row and column numbers for valid cells",
	//	"ParameterDB_WaterBalance", DT_Raster);

	/// from interpolation module
	/// air temperature
	mdi.AddInput("D_Tmin","oC","Minimum air temperature","Module", DT_Raster);
	mdi.AddInput("D_Tmax","oC","Maximum air temperature","Module", DT_Raster);
	//mdi.AddInput("Tmin1","oC","Minimum air temperature of the (d-1)th day","Module", DT_Raster);
	//mdi.AddInput("Tmax1","oC","Maximum air temperature of the (d-1)th day","Module", DT_Raster);
	//mdi.AddInput("Tmin2","oC","Minimum air temperature of the (d-2)th day","Module", DT_Raster);
	//mdi.AddInput("Tmax2","oC","Maximum air temperature of the (d-2)th day","Module", DT_Raster);

	/// output soil temperature
	mdi.AddOutput("SOTE","oC", "Soil Temperature", DT_Raster);

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}