#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SNO_DD.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SNO_DD();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Chunping Ou");
	mdi.SetClass("Snowmelt", "Calculate the amount of snow melt.");
	mdi.SetDescription("For simplicity purpose for the time being, the algorithm used in the original WetSpa is incorporated in the SEIMS.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("SNO_DD");
	mdi.SetName("SNO_DD");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("SNO_DD.chm");

	mdi.AddParameter("c_snow","mm/oC/delta_t","temperature impact factor ","ParameterDB_Snow",DT_Single);
	mdi.AddParameter("c_rain","mm/mm/oC/delta_t","Rainfall impact factor","ParameterDB_Snow",DT_Single);
	mdi.AddParameter("T0","oC","the snowmelt threshold temperature ","ParameterDB_Snow",DT_Single);	
	//mdi.AddParameter("K_blow","-"," a fraction coefficient of snow blowing into or out of the watershed","ParameterDB_Snow",DT_Single);
	mdi.AddParameter("T_snow","oC","snowfall temperature","ParameterDB_Snow",DT_Single);
	mdi.AddParameter("swe0","mm","Initial snow water equivalent","ParameterDB_Snow",DT_Single);

	mdi.AddInput("D_NEPR","mm","net precipitation","Module",DT_Raster);								// from interception module
	//mdi.AddInput("D_SNAC","mm", "distribution of snow accumulation", "Module",DT_Raster);				// from snow water balance module
	//mdi.AddInput("SWE","mm","average SA of the watershed","Module",DT_Single);									// from snow water balance module
	//mdi.AddInput("D_SNRD","mm", "distribution of snow blowing in or out the cell","Module", DT_Raster); // from snow redistribution module
	//mdi.AddInput("D_SNSB","mm", "Distribution of snow sublimation (water equivalent) for a user defined period","Module",DT_Raster); //from snow sublimation module
	mdi.AddInput("D_TMIN","oC","min temperature","Module",DT_Raster);									// from interpolation module
	mdi.AddInput("D_TMAX","oC","max temperature","Module",DT_Raster);									// from interpolation module


	// set the output variables
	mdi.AddOutput("SNME","mm", "distribution of snowmelt", DT_Raster);
	mdi.AddOutput("SNAC","mm", "distribution of snow accumulation", DT_Raster);
	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}