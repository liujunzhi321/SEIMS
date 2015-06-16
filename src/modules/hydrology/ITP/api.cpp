#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "Interpolate.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new Interpolate();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;	

	// set the information properties
	mdi.SetAuthor("Junzhi Liu");
	mdi.SetClass("Interpolation module", "Interpolation module for P, E, PET and so on.");
	mdi.SetDescription("Interpolation module for P, E, PET and so on.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("ITP");
	mdi.SetName("ITP");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("ITP.html");

	//from txt file
	mdi.AddParameter("Weight", "", "Weight used for interpolation", "ParameterDB_Climate", DT_Array1D);
	// This is the climate data type. It is used to get the specific lapse rate from lapse_rate table. 
	// It is also used to create a string which can match the output id. 
	// For example, if data_type = 1, i.e. the data type is P, main program will connect the output variable name ¡°D¡±
	// and the data type to create a string like ¡°D_P¡±,
	// this string is the same with the output id in the output lookup table and file.out.
	//mdi.AddParameter("Data_Type", "", "Climate data type", "Config.fig", DT_Single); 

	// from config.fig, e.g. Interpolation_P_1
	mdi.AddParameter("VerticalInterpolation", "", "Whether the vertical interpolation will be executed", "Config.fig", DT_Single);
	// these three parameters are just read when it will do vertical interpolation
	mdi.AddParameter("DEM", "m", "Elevation grid", "ParameterDB_Climate", DT_Raster);//from file
	mdi.AddParameter("StationElevation", "m", "Site Elevation","HydroClimateDB", DT_Array1D);//from stations table
	//Lapse_rate is the combined lapse rate table name in HydroClimate database.
	mdi.AddParameter("LapseRate", "/100m", "Lapse Rate", "HydroClimateDB", DT_Array2D); 

	// This is the climate data of all sites. 
	// T means time series and it is same with first part of output id, e.g T_P. It may be P,PET,TMin or TMax data.
	mdi.AddInput("T", "", "", "Module", DT_Array1D);

	// Must be ¡°D¡±. This is used to match with output id in file.out with data type.
	mdi.AddOutput("D", "", "", DT_Raster); 

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}