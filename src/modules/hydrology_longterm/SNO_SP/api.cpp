#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SNO_SP.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SNO_SP();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	// set the information properties
	mdi.SetAuthor("Zhiqiang Yu");
	mdi.SetClass("Snowmelt", "Calculate the amount of snow melt.");
	mdi.SetDescription("Snowpack Daily, SWAT p57-59");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("SNO_SP");
	mdi.SetName("SNO_SP");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("SNO_SP.chm");

	//mdi.AddParameter("c_snow","mm/oC/delta_t","temperature impact factor ","ParameterDB_Snow",DT_Single);
	//mdi.AddParameter("c_rain","mm/mm/oC/delta_t","Rainfall impact factor","ParameterDB_Snow",DT_Single);
	mdi.AddParameter("T0","oC","the snowmelt threshold temperature ","ParameterDB_Snow",DT_Single);	
	mdi.AddParameter("K_blow","-"," a fraction coefficient of snow blowing into or out of the watershed","ParameterDB_Snow",DT_Single);
	mdi.AddParameter("T_snow","oC","snowfall temperature","ParameterDB_Snow",DT_Single);
	mdi.AddParameter("swe0","mm","Initial snow water equivalent","ParameterDB_Snow",DT_Single);
	mdi.AddParameter("lag_snow","-","Snow temperature lag factor","ParameterDB_Snow",DT_Single);
	mdi.AddParameter("c_snow6","mm/oC/day","Melt factor on June 21","ParameterDB_Snow",DT_Single);
	mdi.AddParameter("c_snow12","mm/oC/day","Melt factor on Decemeber 21","ParameterDB_Snow",DT_Single);

	mdi.AddInput("D_NEPR","mm","net precipitation","Module",DT_Raster);								// from interception module
	mdi.AddInput("D_SNAC","mm", "distribution of snow accumulation", "Module",DT_Raster);				// from snow water balance module
	mdi.AddInput("SWE","mm","average SA of the watershed","Module",DT_Single);									// from snow water balance module
	mdi.AddInput("D_SNRD","mm", "distribution of snow blowing in or out the cell","Module", DT_Raster); // from snow redistribution module
	mdi.AddInput("D_SNSB","mm", "Distribution of snow sublimation (water equivalent) for a user defined period","Module",DT_Raster); //from snow sublimation module
	mdi.AddInput("D_TMIN","oC","min temperature","Module",DT_Raster);									// from interpolation module
	mdi.AddInput("D_TMAX","oC","max temperature","Module",DT_Raster);									// from interpolation module


	// set the output variables
	mdi.AddOutput("SNME","mm", "distribution of snowmelt", DT_Raster);

	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}