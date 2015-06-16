#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "PER_PI.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new PER_PI();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Junzhi Liu");
	mdi.SetClass("Percolation", "Calculate the amount of water percolated out of the root zone within the time step.");
	mdi.SetDescription("The method in DHSVM was adopted.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("PER_PI");
	mdi.SetName("PER_PI");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("PER_PI.chm");


	mdi.AddParameter("TimeStep","s","","file.in",DT_Single);
	//mdi.AddParameter("UpperSoilDepth", "mm", "depth of the upper soil layer", "ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("t_soil","oC","threshold soil freezing temperature","ParameterDB_WaterBalance", DT_Single);							//
	mdi.AddParameter("Rootdepth","m","Root depth","ParameterDB_WaterBalance", DT_Raster);

	mdi.AddParameter("Conductivity_2D","mm/h","saturation hydraulic conductivity","ParameterDB_WaterBalance",DT_Array2D);
	mdi.AddParameter("Porosity_2D","m3/m3","soil porosity","ParameterDB_WaterBalance",DT_Array2D);
	mdi.AddParameter("Poreindex_2D","-","pore size distribution index","ParameterDB_WaterBalance",DT_Array2D);
	mdi.AddParameter("FieldCap_2D","m3/m3","Soil field capacity","ParameterDB_WaterBalance",DT_Array2D);

	mdi.AddInput("D_SOTE","oC", "Soil Temperature","Module", DT_Raster);
	mdi.AddInput("D_INFIL","mm","infiltration","Module",DT_Raster);	
	mdi.AddInput("D_SOMO_2D","mm","Distribution of soil moisture","Module",DT_Array2D);

	// set the output variables
	mdi.AddOutput("Percolation_2D","mm", "Distribution of groundwater recharge (percolation)", DT_Array2D);

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}