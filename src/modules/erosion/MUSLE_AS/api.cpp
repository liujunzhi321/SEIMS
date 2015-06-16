#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "MUSLE_AS.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new MUSLE_AS();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Zhiqiang Yu");
	mdi.SetClass("erosion", "Calculate the amount of sediment yield.");
	mdi.SetDescription("use MUSLE method to calcualte sediment yield of each cell");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("MUSLE_AS");
	mdi.SetName("MUSLE_AS");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("MUSLE_AS.chm");

	//3 grid parameter
	mdi.AddParameter("CellWidth","m","the width (length) of cell","mask.asc",DT_Single);
	mdi.AddParameter("USLE_C","","the cover management factor","ParameterDB_Sediment",DT_Raster);
	mdi.AddParameter("USLE_P","","the erosion control practice factor ","ParameterDB_Sediment",DT_Raster);
	mdi.AddParameter("USLE_K","","the soil erodibility factor","ParameterDB_Sediment",DT_Raster);
	mdi.AddParameter("acc","","the number of flow accumulation cells of each cell","ParameterDB_Sediment",DT_Raster);
	mdi.AddParameter("slope","%","slope of the cell","ParameterDB_WaterBalance",DT_Raster);
	mdi.AddParameter("subbasin","","subbasin grid","ParameterDB_Snow", DT_Raster);
	mdi.AddParameter("STREAM_LINK", "", "Stream link (id of reaches)", "ParameterDB_Discharge", DT_Raster);
	
	//input from other module	
	mdi.AddInput("D_SURU","mm","surface runoff","Module",DT_Raster);		//from depression module
	mdi.AddInput("D_SNAC","mm","snow accumulation","Module",DT_Raster);	//from snow water balance module

	// set the output variables
	mdi.AddOutput("SOER","metric tons", "distribution of soil erosion", DT_Raster);
	mdi.AddOutput("USLE_LS","USLE LS factor", "USLE LS factor", DT_Raster);

	mdi.AddOutput("SEDTOCH","metric tons", "sediment to channel", DT_Array1D);  // added by wu hui
	mdi.AddOutput("SEDTOCH_T","kg", "Total sediment flowing to channel", DT_Single); // added by wu hui
	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}