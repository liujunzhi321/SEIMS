#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SSM_PE.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SSM_PE();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Chunping Ou");
	mdi.SetClass("Snow sublimation", "Calculate the amount of snow sublimation .");
	mdi.SetDescription("A simple method that used in the old WetSpa will be incorporated in the SEIMS as the default method.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("SSM_PE");
	mdi.SetName("SSM_PE");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("SSM_PE.chm");

	mdi.AddParameter("K_subli","-","Praction of PET for sublimation","ParameterDB_Snow",DT_Single);
	mdi.AddParameter("K_blow","-"," a fraction coefficient of snow blowing into or out of the watershed","ParameterDB_Snow",DT_Single);
	mdi.AddParameter("T0","oC","the snowmelt threshold temperature ","ParameterDB_Snow",DT_Single);
	mdi.AddParameter("T_snow","oC","snowfall temperature","ParameterDB_Snow",DT_Single);
	mdi.AddParameter("swe0","mm","Initial snow water equivalent","ParameterDB_Snow",DT_Single);

	mdi.AddInput("D_PET","mm","PET grid","Module",DT_Raster);											// from interpolation module
	mdi.AddInput("D_NEPR","mm","net precipitation","Module",DT_Raster);								// from interception module
	mdi.AddInput("D_SNAC","mm", "distribution of snow accumulation", "Module",DT_Raster);				// from snow water balance module
	mdi.AddInput("SWE","mm","average SA of the watershed","Module",DT_Single);									// from snow water balance module
	mdi.AddInput("D_SNRD","mm", "distribution of snow blowing in or out the cell","Module", DT_Raster); // from snow redistribution module
	mdi.AddInput("D_TMIN","oC","min temperature","Module",DT_Raster);									// from interpolation module
	mdi.AddInput("D_TMAX","oC","max temperature","Module",DT_Raster);									// from interpolation module

	// set the output variables
	mdi.AddOutput("SNSB","mm", "Distribution of snow sublimation (water equivalent) for a user defined period",DT_Raster);

	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}