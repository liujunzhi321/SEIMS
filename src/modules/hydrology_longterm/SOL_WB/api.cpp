#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SOL_WB.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SOL_WB();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	mdi.SetAuthor("Chunping Ou");
	mdi.SetClass("Soil water balance", "Soil water balance calculation.");
	mdi.SetDescription("Soil water balance.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("SOL_WB");
	mdi.SetName("SOL_WB");
	mdi.SetVersion("0.3");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("SOL_WB.chm");

	//mdi.AddParameter("subbasinSelected","","The subbasion ids listed in file.out","file.out",DT_Array1D); //this list is used to contrain the output size. Its name must be subbasinSelected.
	mdi.AddParameter("subbasin","","The subbasion grid","ParameterDB_Snow",DT_Raster); //this list is used to contrain the output size
	mdi.AddParameter("Rootdepth","mm","Root depth","ParameterDB_WaterBalance",DT_Raster);

	mdi.AddInput("D_P","mm","precipitation","Module",DT_Raster);
	mdi.AddInput("D_NEPR","mm", "net precipitation","Module", DT_Raster);
	mdi.AddInput("D_INLO","mm", "Interception Loss Distribution","Module", DT_Raster);
	mdi.AddInput("D_INET","mm", "Evaporation From Interception Storage","Module", DT_Raster);
	mdi.AddInput("D_DPST","mm", "depression storage","Module", DT_Raster);	
	mdi.AddInput("D_DEET","mm", "evaporation from depression storage","Module", DT_Raster);
	mdi.AddInput("D_INFIL","mm","Infiltration map of watershed","Module", DT_Raster);
	mdi.AddInput("D_SOET","mm","soil evapotranpiration","Module",DT_Raster);
	
	mdi.AddInput("D_Revap","mm","revap","Module",DT_Raster);
	mdi.AddInput("D_SURU","mm","surface runoff","Module",DT_Raster);
	mdi.AddInput("T_RG","mm","groundwater runoff","Module",DT_Array1D);	
	
	//mdi.AddInput("D_SNSB","mm", "snow sublimation","Module",DT_Raster);
	mdi.AddInput("D_TMIN","oC","min temperature","Module",DT_Raster);
	mdi.AddInput("D_TMAX","oC","max temperature","Module",DT_Raster);
	mdi.AddInput("D_SOTE","oC", "Soil Temperature","Module", DT_Raster);

	mdi.AddInput("D_Percolation_2D", "mm", "percolation", "Module", DT_Array2D);	
	mdi.AddInput("D_SOMO_2D", "mm", "Distribution of soil moisture", "Module", DT_Array2D);
	mdi.AddInput("D_SSRU_2D", "mm", "subsurface runoff", "Module", DT_Array2D);

	// set the output variables
	mdi.AddOutput("SOWB","mm", "soil water balance", DT_Array2D);

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}