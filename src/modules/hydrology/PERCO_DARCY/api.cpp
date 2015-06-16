#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "Percolation.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new Percolation();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Chunping Ou");
	mdi.SetClass("Percolation", "Calculate the amount of water percolated out of the root zone within the time step.");
	mdi.SetDescription("The method relating percolation with soil moisture and pore size distribution index used in the original WetSpa will be the default method to estimate percolation out of the root zone.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("Percolation");
	mdi.SetName("Percolation");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("Percolation.chm");

	//mdi.AddParameter("t_soil","oC","threshold soil freezing temperature","ParameterDB_WaterBalance", DT_Single);							//
	mdi.AddParameter("DT_HS", "second", "time step for storm simulation","ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("CellWidth", "m", "Cell size", "file.in", DT_Single); 

	mdi.AddParameter("Conductivity","mm/h","saturation hydraulic conductivity","ParameterDB_WaterBalance",DT_Raster);
	mdi.AddParameter("Porosity","m3/m3","soil porosity","ParameterDB_WaterBalance",DT_Raster);
	//mdi.AddParameter("Residual","m3/m3","residual moisture content","ParameterDB_WaterBalance",DT_Raster);
	mdi.AddParameter("Poreindex","-","pore size distribution index","ParameterDB_WaterBalance",DT_Raster);
	mdi.AddParameter("FieldCap","m3/m3","Soil field capacity","ParameterDB_WaterBalance",DT_Raster);
	mdi.AddParameter("RootDepth", "mm", "Root depth","ParameterDB_WaterBalance", DT_Raster);

	//mdi.AddInput("D_SOTE","oC", "Soil Temperature","Module", DT_Raster);						//soil temperature
	mdi.AddInput("D_SOMO","mm","Distribution of soil moisture","Module",DT_Raster);			//from soil water balance module	0
	//mdi.AddInput("D_ES","mm","ES","Module",DT_Raster);											//from actual evapotranspiration module, the output id may not be correct.

	// set the output variables
	mdi.AddOutput("PERCOLATION","mm", "Distribution of groundwater recharge (percolation)", DT_Raster);

	// write out the XML file.
	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}