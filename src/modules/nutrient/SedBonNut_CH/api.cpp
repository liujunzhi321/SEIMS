#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SedBonNutrient_CH.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SedBonNutrient_CH();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Wang Lin");
	mdi.SetClass("Sediment Bound Nutrient CH", "Channel Sediment Bound Nutrient.");
	mdi.SetDescription("Sediment Bound Nutrient Channel transport method using KinematicWave Method.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("SedBonNutrient_CH.chm");
	mdi.SetID("SedBonNutrient_CH");
	mdi.SetName("SedBonNutrient_CH");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("STORM_DT", "second", "Time step of the simulation", "file.in", DT_Single); 
	mdi.AddParameter("CellWidth", "m", "Cell size", "file.in", DT_Single); 
	mdi.AddParameter("Slope", "%", "Slope", "ParameterDB_WaterBalance", DT_Raster); 
	mdi.AddParameter("CHWIDTH", "m", "Channel width", "ParameterDB_Discharge", DT_Raster); 
	mdi.AddParameter("FLOWOUT_INDEX_D8", "", "The index of flow in cell in the compressed array", "ParameterDB_Discharge", DT_Array1D);
	mdi.AddParameter("FLOWIN_INDEX_D8", "", "The index of flow in cell in the compressed array,"
		" and the first element in each sub-array is the number of flow in cells in this sub-array", "ParameterDB_Discharge", DT_Array2D);
	mdi.AddParameter("STREAM_LINK", "", "Stream link (id of reaches)", "ParameterDB_Discharge", DT_Raster);
	mdi.AddParameter("ReachParameter", "", "Reach parameters such stream order, manning's n and downstream subbasin id", "ParameterDB_Discharge", DT_Array2D);
	
	mdi.AddInput("D_SedBonPToCh", "m3/s", "Flux in the downslope boundary of cells", "Module",DT_Raster);
	mdi.AddInput("D_SedBonAmmoniumToCh","mm","Water depth in the downslope boundary of cells","Module",DT_Raster);	
	mdi.AddInput("HCH","mm", "Water depth in the downslope boundary of cells","Module",DT_Array2D);
	mdi.AddInput("QCH", "m3/s", "Flux in the downslope boundary of cells", "Module",DT_Array2D);

	mdi.AddOutput("SedBonP", "kg/s", "distribution of sediment bound P in reaches", DT_Array2D);
	mdi.AddOutput("SedBonAmmonium", "kg/s", "distribution of nutrient, sediment bound ammonium in reaches", DT_Array2D);

	mdi.AddOutput("SedBonPOutlet", "mg/s", "sediment bound P flows to outlet", DT_Single);
	mdi.AddOutput("SedBonAmmoniumOutlet", "mg/s", "sediment bound ammonium flows to outlet", DT_Single);

	// set the dependencies
	mdi.AddDependency("Sediment Bound Nutrient OL","Sediment Bound Nutrient Overland module");      //for WH
	mdi.AddDependency("Channel routing","Channel routing module"); 

	// write out the XML file.
	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}