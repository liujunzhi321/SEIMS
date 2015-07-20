#include <stdio.h>
#include <string>
#include "util.h"
#include "api.h"
#include "clsTSD_RD.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new clsTSD_RD();
}

/// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;
	string res;

	mdi.SetAuthor("Zhiqiang Yu");
	mdi.SetClass("Time series data","read time series data from hydroclimate database.");
	mdi.SetDescription("read time series data from hydroclimate database.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("");
	mdi.SetID("TSD_RD");
	mdi.SetName("TSD_RD");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("data_type","","climate data type","Config.fig",DT_Single);

	/// set the input variables (time series)
	mdi.AddInput("T","", "the climate data of all the sites", "HydroClimateDB", DT_Array1D); ///T means time series. D means distribution.

	/// set the output variables
	mdi.AddOutput("T","", "the climate data of all the sites", DT_Array1D);

	/// write out the XML file.
	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}