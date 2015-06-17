#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "StormGreenAmpt.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new StormGreenAmpt();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Junzhi Liu");
	mdi.SetClass("Surface runoff", "Calculate infiltration and excess precipitation.");
	mdi.SetDescription("Modified rational method to calculate infiltration and excess precipitation.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("SUR_SGA.chm");
	mdi.SetID("SUR_SGA");
	mdi.SetName("SUR_SGA");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("DT_HS", "second", "time step for storm simulation","ParameterDB_WaterBalance", DT_Single);
	//mdi.AddParameter("t_soil","degree Celsius","threshold soil freezing temperature","ParameterDB_WaterBalance",DT_Single);

	//mdi.AddParameter("T0","degree Celsius","snowmelt threshold temperature","ParameterDB_Snow",DT_Single);
	//mdi.AddParameter("T_snow","degree Celsius","snowfall temperature","ParameterDB_Snow",DT_Single); 
	//mdi.AddParameter("s_frozen", "m3/m3", "frozen soil moisture","ParameterDB_WaterBalance", DT_Single);

	mdi.AddParameter("Conductivity","mm/h","Soil hydraulic conductivity","ParameterDB_WaterBalance", DT_Raster);  //soil_k
	mdi.AddParameter("Moist_in","m3/m3","Initial soil moisture","ParameterDB_WaterBalance", DT_Raster);
	mdi.AddParameter("Clay","%","Percent of clay content","ParameterDB_WaterBalance",DT_Raster);
	mdi.AddParameter("Sand","%","Percent of sand content","ParameterDB_WaterBalance",DT_Raster);
	mdi.AddParameter("porosity", "-", "Soil porosity","ParameterDB_WaterBalance", DT_Raster);
	mdi.AddParameter("RootDepth", "mm", "Root depth","ParameterDB_WaterBalance", DT_Raster);
	mdi.AddParameter("FieldCap", "%", "Field capacity","ParameterDB_WaterBalance", DT_Raster);

	mdi.AddInput("D_NEPR","mm","The net precipitation","Module", DT_Raster);
	//mdi.AddInput("D_TEMP","oC","Air temperature","Module", DT_Raster);
	//mdi.AddInput("D_SOMO","m3/m3","The soil moisture","Module", DT_Raster);
	mdi.AddInput("D_DPST","mm","The depression storage","Module",DT_Raster);
	mdi.AddInput("D_SURU", "mm", "Distribution of surface runoff", "Module", DT_Raster);

	//mdi.AddInput("D_SNAC","mm","The snow accumulation","Module",DT_Raster);
	//mdi.AddInput("D_SNME","mm","The snowmelt","Module",DT_Raster);

	//mdi.AddOutput("EXCP", "mm","The excess precipitation", DT_Raster);
	mdi.AddOutput("SOMO","m3/m3", "Average soil moisture distribution for a user defined period.", DT_Raster);
	mdi.AddOutput("INFIL","mm","Infiltration map of watershed", DT_Raster);
	mdi.AddOutput("INFILCAPSURPLUS","mm","surplus of infiltration capacity", DT_Raster);
	mdi.AddOutput("AccumuInfil","mm","accumulative infiltration", DT_Raster);

	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}