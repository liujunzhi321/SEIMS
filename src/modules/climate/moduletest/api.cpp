#include <stdio.h>
#include <string>
#include "util.h"
#include "api.h"
#include "moduletest.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new ModulesTest();
}

/// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;
	string res;

	mdi.SetAuthor("LiangJun Zhu");
	mdi.SetClass("TEST", "Base functionality test!");
	mdi.SetDescription("Module test.");
	mdi.SetID("moduletest");
	mdi.SetName("moduletest");
	mdi.SetVersion("1.0");
	mdi.SetEmail(SEIMS_EMAIL);
	mdi.SetWebsite(SEIMS_SITE);
	mdi.SetHelpfile("");

	mdi.AddParameter(VAR_CN2, UNIT_NON_DIM, DESC_CN2, Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_CONDUCT, UNIT_WTRDLT_MMH, DESC_CONDUCT, Source_ParameterDB, DT_Raster2D);
	//mdi.AddParameter(Tag_ROUTING_LAYERS, UNIT_NON_DIM, DESC_ROUTING_LAYERS, Source_ParameterDB, DT_Array2D);
	/// set the output variables
	mdi.AddOutput("CN2_M",UNIT_NON_DIM, DESC_CN2, DT_Raster1D);
	mdi.AddOutput("K_M",UNIT_WTRDLT_MMH,DESC_CONDUCT,DT_Raster2D);
	//mdi.AddOutput("ROUTE",UNIT_NON_DIM,DESC_ROUTING_LAYERS,DT_Array2D);
	/// write out the XML file.
	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}