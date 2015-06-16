#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "InorganicPSorption.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new InorganicPSorption();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Wang Lin");
	mdi.SetClass("InorganicPSorption", "Sorption of inorganic P.");
	mdi.SetDescription("Sorption of inorganic P.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("InorganicPSorption.chm");
	mdi.SetID("InorganicPSorption");
	mdi.SetName("InorganicPSorption");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("SloER", "d-1", "slow equilibration rate constant", "file.in", DT_Single); 

	//mdi.AddParameter("AftSolP", "", "amount of phosphorus in solution after fertilization and incubation", "ParameterDB_soil", DT_Raster);
	//mdi.AddParameter("BefSolP", "", "amount of phosphorus in solution before fertilization", "ParameterDB_soil", DT_Raster);
	//mdi.AddParameter("SolPFer", "", "amount of soluble P fertilizer added to the sample", "ParameterDB_soil", DT_Raster);
	
	mdi.AddInput("D_SoluP", "kg P/ha", "amount of phosphorus in solution in layer ly", "Module", DT_Array2D);

	mdi.AddOutput("Pai", "", "phosphorus availability index", DT_Array2D);
	mdi.AddOutput("ActMinP", "kg P/ha", "amount of phosphorus in the active mineral pool", DT_Array2D);
	mdi.AddOutput("StaMinP", "kg P/ha", "amount of phosphorus in the stable mineral pool", DT_Array2D);
	mdi.AddOutput("TSolActP", "kg P/ha", "amount of phosphorus transferred between the soluble and active mineral pool", DT_Array2D); 
	mdi.AddOutput("TStaActP", "kg P/ha", "amount of phosphorus transferred between the active and stable mineral pools", DT_Array2D);

	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}