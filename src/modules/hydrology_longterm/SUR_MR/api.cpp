#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SUR_MR.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SUR_MR();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Junzhi Liu");
	mdi.SetClass("Surface runoff", "Calculate infiltration and excess precipitation.");
	mdi.SetDescription("Modified rational method to calculate infiltration and excess precipitation.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("SUR_MR.chm");
	mdi.SetID("SUR_MR");
	mdi.SetName("SUR_MR");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("DT_HS", "second", "Time step of the simulation", "file.in", DT_Single); 
	mdi.AddParameter("T_snow","degree Celsius","snowfall temperature","ParameterDB_Snow",DT_Single); 
	mdi.AddParameter("t_soil","degree Celsius","threshold soil freezing temperature","ParameterDB_WaterBalance",DT_Single);
	mdi.AddParameter("T0","degree Celsius","snowmelt threshold temperature","ParameterDB_Snow",DT_Single);

	mdi.AddParameter("K_run","-","Runoff exponent","ParameterDB_WaterBalance",DT_Single);
	mdi.AddParameter("P_max","mm","Maximum P corresponding to runoffCo","ParameterDB_WaterBalance",DT_Single);

	mdi.AddParameter("Rootdepth","m","Root depth","ParameterDB_WaterBalance", DT_Raster);
	mdi.AddParameter("Runoff_co", "-", "Potential runoff coefficient","ParameterDB_WaterBalance", DT_Raster);
	mdi.AddParameter("Moist_in","m3/m3","Initial soil moisture","ParameterDB_WaterBalance", DT_Raster);

	mdi.AddParameter("Fieldcap_2D","m3/m3","Soil field capacity","ParameterDB_WaterBalance", DT_Array2D);
	mdi.AddParameter("porosity_2D", "-", "Soil porosity","ParameterDB_WaterBalance", DT_Array2D);

	mdi.AddParameter("s_frozen", "m3/m3", "frozen soil moisture","ParameterDB_WaterBalance", DT_Single);

	mdi.AddInput("D_NEPR","mm","The net precipitation","Module", DT_Raster);
	mdi.AddInput("D_Tmin","oC","Minimum air temperature","Module", DT_Raster);
	mdi.AddInput("D_Tmax","oC","Maximum air temperature","Module", DT_Raster);
	//mdi.AddInput("D_SOMO","m3/m3","The soil moisture","Module", DT_Raster);
	mdi.AddInput("D_DPST","mm","The depression storage","Module",DT_Raster);
	mdi.AddInput("D_SOTE","degree Celsius","The soil temperature","Module",DT_Raster);
	mdi.AddInput("D_SNAC","mm","The snow accumulation","Module",DT_Raster);
	mdi.AddInput("D_SNME","mm","The snowmelt","Module",DT_Raster);

	mdi.AddOutput("EXCP", "mm","The excess precipitation", DT_Raster);
	mdi.AddOutput("INFIL","mm","Infiltration map of watershed", DT_Raster);
	mdi.AddOutput("SOMO_2D","m3/m3", "Average soil moisture distribution for a user defined period.", DT_Array2D);

	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}