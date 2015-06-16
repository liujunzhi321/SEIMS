#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "NitrogenFixation.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new NitrogenFixation();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Wang Lin");
	mdi.SetClass("NitrogenFixation", "NitrogenFixation.");
	mdi.SetDescription("NitrogenFixation.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("NitrogenFixation.chm");
	mdi.SetID("NitrogenFixation");
	mdi.SetName("NitrogenFixation");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	
	mdi.AddParameter("FieldCap_2D", "%", "Field capacity","ParameterDB_WaterBalance", DT_Array2D);
	
	mdi.AddInput("Fr_PHU","","fraction of phu accumulated on a given day ","Module",DT_Raster);
	
	mdi.AddInput("SOMO_2D","%","Soil Moisture","Module", DT_Array2D);
	mdi.AddInput("D_Nitrate", "kg N/ha", "amount of nitrate", "Module", DT_Array2D);
	mdi.AddInput("D_DemNit", "kg N/ha", "plant nitrogen demand not met by uptake from the soil", "Module", DT_Array2D);

	mdi.AddOutput("F_GS", "", "growth stage factor", DT_Raster);
	
	mdi.AddOutput("F_SW", "", "soil water factor", DT_Array2D);
	mdi.AddOutput("F_SN", "", "soil nitrate factor", DT_Array2D);
	mdi.AddOutput("FixN", "kg N/ha", "amount of nitrogen added to the plant biomass by fixation", DT_Array2D); 

	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}