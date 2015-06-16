#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SUR_CN.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SUR_CN();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Wu Hui");
	mdi.SetClass("Surface runoff", "Calculate infiltration and excess precipitation.");
	mdi.SetDescription("SCS curve number method to calculate infiltration and excess precipitation.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("SUR_CN.chm");
	mdi.SetID("SUR_CN");
	mdi.SetName("SUR_CN");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("T_snow","degree Celsius","snowfall temperature","ParameterDB_Snow",DT_Single); //Tsnow->T_snow
	mdi.AddParameter("t_soil","oC","threshold soil freezing temperature","ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("T0","degree Celsius","snowmelt threshold temperature","ParameterDB_Snow",DT_Single);
	//mdi.AddParameter("Depre_in","","Initial depression storage coefficient","ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("s_frozen","","Frozen moisture relative to porosity with no infiltration","ParameterDB_WaterBalance", DT_Single);  

	mdi.AddParameter("Fieldcap_2D","m3/m3","Soil field capacity","ParameterDB_WaterBalance", DT_Array2D);
	mdi.AddParameter("porosity_2D", "-", "Soil porosity","ParameterDB_WaterBalance", DT_Array2D);
	mdi.AddParameter("Wiltingpoint_2D","m3/m3","Plant wilting point moisture","ParameterDB_WaterBalance", DT_Array2D);

	mdi.AddParameter("Rootdepth","m","Root depth","ParameterDB_WaterBalance", DT_Raster);
	mdi.AddParameter("CN2","-","CN under moisture condition II","ParameterDB_WaterBalance", DT_Raster);
	mdi.AddParameter("Moist_in","m3/m3","Initial soil moisture","ParameterDB_WaterBalance", DT_Raster);	
	
	mdi.AddInput("D_NEPR","mm","The net precipitation","Module",DT_Raster);					//from interception module
	mdi.AddInput("D_TMin","degree Celsius","The minimum air temperature","Module",DT_Raster);	//from interpolation module
	mdi.AddInput("D_TMax","degree Celsius","The maximum air temperature","Module",DT_Raster);	//from interpolation module
	mdi.AddInput("D_DPST","mm","The depression storage","Module",DT_Raster);					//from depression module
	mdi.AddInput("D_SOTE","degree Celsius","The soil temperature","Module",DT_Raster);			//from soil temperature module
	mdi.AddInput("D_SNAC","mm","The snow accumulation","Module",DT_Raster);					//from snow accumulation module
	mdi.AddInput("D_SNME","mm","The snowmelt","Module",DT_Raster);								//from snowmelt module

	mdi.AddOutput("EXCP", "mm","The excess precipitation", DT_Raster);// just for depression module.
	mdi.AddOutput("INFIL","mm","Infiltratio n map of watershed", DT_Raster);
	mdi.AddOutput("SOMO_2D","m3/m3", "Average soil moisture distribution for a user defined period.", DT_Array2D);

	// set the dependencies
	mdi.AddDependency("Interpolation","Interpolation module");      //for pNet,T,
	mdi.AddDependency("Soil water","Soil Water Balance module");    // soilMoisture at given day
	mdi.AddDependency("Depression","Depression Storage module");     // SD
	mdi.AddDependency("soil temperature","Soil temperature module");  // TS
	mdi.AddDependency("Snow balance","Snow balance module");          // SA
	mdi.AddDependency("Snowmelt","Snowmelt module");                  // SM

	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}