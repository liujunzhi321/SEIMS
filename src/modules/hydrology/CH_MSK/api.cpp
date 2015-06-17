#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "Muskingum.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new Muskingum();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Junzhi Liu");
	mdi.SetClass("Channel routing", "Channel routing.");
	mdi.SetDescription("Channel routing using Muskingum-Cunge method.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("CH_MSK.chm");
	mdi.SetID("CH_MSK");
	mdi.SetName("CH_MSK");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("DT_HS", "second", "Time step of the simulation", "file.in", DT_Single); 
	mdi.AddParameter("CellWidth", "m", "Cell width", "file.in", DT_Single); 
	mdi.AddParameter("MSK_X", "", "Muskingum weighting factor", "ParameterDB_Discharge", DT_Single); 

	mdi.AddParameter("Slope", "%", "Slope", "ParameterDB_WaterBalance", DT_Raster); 
	mdi.AddParameter("FLOW_DIR", "", "Flow direction  by the rule of ArcGIS", "ParameterDB_Discharge", DT_Raster);
	mdi.AddParameter("CHWIDTH", "m", "Channel width", "ParameterDB_Discharge", DT_Raster); 
	// reach information
	mdi.AddParameter("FLOWOUT_INDEX_D8", "", "The index of flow in cell in the compressed array", "ParameterDB_Discharge", DT_Array1D);
	mdi.AddParameter("FLOWIN_INDEX_D8", "", "The index of flow in cell in the compressed array,"
		" and the first element in each sub-array is the number of flow in cells in this sub-array", "ParameterDB_Discharge", DT_Array2D);
	mdi.AddParameter("STREAM_LINK", "", "Stream link (id of reaches)", "ParameterDB_Discharge", DT_Raster);
	mdi.AddParameter("ReachParameter", "", "Reach parameters such stream order, manning's n and downstream subbasin id", "ParameterDB_Discharge", DT_Array2D);
	
	mdi.AddParameter("Chs0","m3/m","initial channel storage per meter of reach length","ParameterDB_Discharge", DT_Single);
	
	// from other module
	mdi.AddInput("SBQG","m3/s","groundwater flow to channel","Module", DT_Array1D);
	mdi.AddInput("D_QSoil","m3/s","interflow to channel","Module", DT_Raster);
	mdi.AddInput("D_QOverland","m3/s","overland flow to channel","Module", DT_Raster);
	mdi.AddInput("D_P","mm","Precipitation","Module", DT_Raster);

	// output
	mdi.AddOutput("QCH", "m3/s", "Flux in the downslope boundary of cells", DT_Array2D);
	mdi.AddOutput("QOUTLET", "m3/s", "discharge at the watershed outlet", DT_Single);
	mdi.AddOutput("QSUBBASIN", "m3/s", "discharge at the subbasin outlet", DT_Array1D);
	mdi.AddOutput("HCH", "mm", "Water depth in the downslope boundary of cells", DT_Array2D);

	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}