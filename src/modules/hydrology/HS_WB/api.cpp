#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "HS_WB.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new HS_WB();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	mdi.SetAuthor("Junzhi Liu");
	mdi.SetClass("Hillslope water balance", "water balance calculation.");
	mdi.SetDescription("Hillsloope water balance.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("HS_WB");
	mdi.SetName("HS_WB");
	mdi.SetVersion("0.3");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("HS_WB.chm");

	//mdi.AddParameter("subbasinSelected","","The subbasion ids listed in file.out","file.out",DT_Array1D); //this list is used to contrain the output size. Its name must be subbasinSelected.
	mdi.AddParameter("subbasin","","The subbasion grid","ParameterDB_Snow",DT_Raster); 
	mdi.AddParameter("Rootdepth","mm","Root depth","ParameterDB_WaterBalance",DT_Raster);
	mdi.AddParameter("Porosity","m3/m3","soil porosity","ParameterDB_WaterBalance",DT_Raster);
	mdi.AddParameter("FieldCap","m3/m3","Soil field capacity","ParameterDB_WaterBalance",DT_Raster);
	mdi.AddParameter("RchParam","","reach parameters", "reachparameters.txt",DT_Array2D);
	mdi.AddParameter("STREAM_LINK", "", "Stream link (id of reaches)", "ParameterDB_Discharge", DT_Raster);

	mdi.AddInput("D_QOverland","m3/s","discharge added to channel flow from overland flow","Module", DT_Raster);
	mdi.AddInput("D_QSoil","m3/s","discharge added to channel flow from interflow","Module", DT_Raster);

	//mdi.AddInput("D_P","mm","precipitation","Module",DT_Raster);
	//mdi.AddInput("D_NEPR","mm", "net precipitation","Module", DT_Raster);			//interception
	//mdi.AddInput("D_INLO","mm", "Interception Loss Distribution","Module", DT_Raster);			//interception
	//mdi.AddInput("D_INET","mm", "Evaporation From Interception Storage","Module", DT_Raster);	//EI
	//mdi.AddInput("D_DPST","mm", "depression storage","Module", DT_Raster);						//depression
	//mdi.AddInput("D_DEET","mm", "evaporation from depression storage","Module", DT_Raster);	//depression ED
	//mdi.AddInput("D_INFIL","mm","Infiltration map of watershed","Module", DT_Raster);		    //infiltration from infiltration module
	//mdi.AddInput("D_SOET","mm","soil evapotranpiration","Module",DT_Raster);					//actual evapotranspiration
	//mdi.AddInput("D_GRRE","mm","percolation","Module",DT_Raster);								//percolation
	//mdi.AddInput("D_Revap","mm","revap","Module",DT_Raster);									//revap
	//mdi.AddInput("D_SURU","mm","surface runoff","Module",DT_Raster);							//depression RS
	//mdi.AddInput("D_SSRU","mm","subsurface runoff","Module",DT_Raster);						//subsurface runoff RI 
	//mdi.AddInput("T_RG","mm","groundwater runoff","Module",DT_Array1D);									//groundwater runoff RG  special,not distribution data
	//mdi.AddInput("D_SOMO","mm","Distribution of soil moisture","Module",DT_Raster);
	//mdi.AddInput("D_SNSB","mm", "snow sublimation","Module",DT_Raster);						//snow sublimation SE
	//mdi.AddInput("D_TMIN","oC","min temperature","Module",DT_Raster);							//interpolation	
	//mdi.AddInput("D_TMAX","oC","max temperature","Module",DT_Raster);							//interpolation
	//mdi.AddInput("D_SOTE","oC", "Soil Temperature","Module", DT_Raster);						//soil temperature


	// set the output variables
	mdi.AddOutput("SOWB","mm", "soil water balance", DT_Array2D);
	mdi.AddOutput("SBOF", "mm","Overland flow to streams for each subbasin", DT_Array1D);
	mdi.AddOutput("SBIF", "mm","Interflow to streams for each subbasin", DT_Array1D);

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}