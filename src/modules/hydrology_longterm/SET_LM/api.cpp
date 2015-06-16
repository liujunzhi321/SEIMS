#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SET_LM.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SET_LM();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res;
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Chunping Ou");
	mdi.SetClass("Evapotranspiration from soil", "Calculate the amount of the evapotranspiration from soil water reservoir within the time step.");
	mdi.SetDescription("The method relating ET linearly with actual soil moisture used in the original WetSpa will be the default method to estimate actual ET from the soil water reservoir.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("SET_LM");
	mdi.SetName("SET_LM");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("SET_LM.chm");

	mdi.AddParameter("Rootdepth","m","Root depth","ParameterDB_WaterBalance",DT_Raster);
	mdi.AddParameter("T_Soil","oC","threshold soil freezing temperature","ParameterDB_WaterBalance", DT_Single);					//

	mdi.AddParameter("FieldCap_2D","m3/m3","Soil field capacity","ParameterDB_WaterBalance",DT_Array2D);
	mdi.AddParameter("Wiltingpoint_2D","m3/m3","Plant wilting point moisture","ParameterDB_WaterBalance", DT_Array2D);					//0w				
	
	// set the parameters (non-time series)
	mdi.AddInput("D_PET","mm","pet","Module",DT_Raster);
	mdi.AddInput("D_INET","mm","Evaporation From Interception Storage","Module",DT_Raster);
	mdi.AddInput("D_DEET","mm","Distribution of depression ET","Module",DT_Raster);
	mdi.AddInput("D_SOTE","oC", "Soil Temperature","Module", DT_Raster);

	mdi.AddInput("D_SOMO_2D", "mm", "Distribution of soil moisture", "Module", DT_Array2D);

	// set the output variables
	mdi.AddOutput("SOET","mm", "Distribution of soil evapotranspiration for a user defined period.", DT_Raster);

	// write out the XML file.
	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}