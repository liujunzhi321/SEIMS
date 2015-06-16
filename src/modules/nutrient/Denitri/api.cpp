#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "Denitrification.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new Denitrification();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Wang Lin");
	mdi.SetClass("Denitrification", "Denitrification.");
	mdi.SetDescription("Denitrification.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("Denitrification.chm");
	mdi.SetID("Denitrification");
	mdi.SetName("Denitrification");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("DenRC", "", "rate coefficient for denitrification", "file.in", DT_Single); 

	//mdi.AddParameter("OrgCar", "%", "amount of organic carbon in the layer", "ParameterDB_soil", DT_Array2D); 
	//mdi.AddParameter("DenWF", "", "threshold value of nutrient cycling water factor for denitrification to occur", "ParameterDB_soil", DT_Array2D); 

	mdi.AddInput("D_Nitrate", "kg N/ha", "amount of nitrate", "Module", DT_Array2D);
	mdi.AddInput("D_TF","", "nutrient cycling temperature factor", "Module", DT_Array2D);
	mdi.AddInput("D_WF", "", "nutrient cycling water factor", "Module", DT_Array2D); 
	
	mdi.AddOutput("DenLostN", "kg N/ha", "amount of nitrogen lost to denitrication", DT_Array2D); 

	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}