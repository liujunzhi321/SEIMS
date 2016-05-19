#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "ImplicitKinematicWave.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new ImplicitKinematicWave();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Junzhi Liu");
	mdi.SetClass("Overland routing", "Overland routing.");
	mdi.SetDescription("Overland routing using 4-point implicit finite difference method.");
	mdi.SetEmail(SEIMS_EMAIL);
	mdi.SetHelpfile("IKW_OL.chm");
	mdi.SetID("IKW_OL");
	mdi.SetName("IKW_OL");
	mdi.SetVersion("0.1");
	mdi.SetWebsite(SEIMS_SITE);

	mdi.AddParameter("DT_HS", "second", "Time step of overland flow simulation", "file.in", DT_Single); 
	mdi.AddParameter(Tag_CellSize, UNIT_NON_DIM, DESC_CellSize, Source_ParameterDB, DT_Single);
	mdi.AddParameter(Tag_CellWidth, UNIT_LEN_M, DESC_CellWidth, Source_ParameterDB, DT_Single);
	
	mdi.AddParameter("Slope", "%", "Slope", "ParameterDB_WaterBalance", DT_Raster1D); 
	mdi.AddParameter("CHWIDTH", "m", "Channel width", "ParameterDB_Discharge", DT_Raster1D); 
	mdi.AddParameter("STREAM_LINK", "", "Stream link", "ParameterDB_Discharge", DT_Raster1D); 
	mdi.AddParameter("Manning", "", "Manning's roughness", "ParameterDB_Discharge", DT_Raster1D); 
	mdi.AddParameter("FLOW_DIR", "", "Flow direction  by the rule of ArcGIS", "ParameterDB_Discharge", DT_Raster1D);	

	mdi.AddParameter("FLOWOUT_INDEX_D8", "", "The index of flow in cell in the compressed array", "ParameterDB_Discharge", DT_Array1D);
	mdi.AddParameter("FLOWIN_INDEX_D8", "", "The index of flow in cell in the compressed array,"
		" and the first element in each sub-array is the number of flow in cells in this sub-array", "ParameterDB_Discharge", DT_Array2D);
	mdi.AddParameter("ROUTING_LAYERS", "", "Routing layers according to the flow direction"
		"There are not flow relationships within each layer, and the first element in each layer is the number of cells in the layer", "ParameterDB_Discharge", DT_Array2D);

	mdi.AddInput("D_SURU", "mm", "Distribution of surface runoff", "Module", DT_Raster1D);
	mdi.AddInput("D_INFILCAPSURPLUS","mm","surplus of infiltration capacity", "Module", DT_Raster1D);
	mdi.AddInput("D_INFIL","mm","Infiltration map of watershed", "Module", DT_Raster1D);
	mdi.AddInput("D_AccumuInfil","mm","accumulative infiltration", "Module", DT_Raster1D);

	mdi.AddOutput("QOverland", "m3/s", "overland runoff", DT_Raster1D);

	mdi.AddOutput("Reinfiltration", "mm", "reinfiltration", DT_Raster1D);
	//mdi.AddOutput("CELLH", "mm", "Water depth in the downslope boundary of cells", DT_Raster);
	mdi.AddOutput("FlowWidth", "m", "Flow width of overland plane", DT_Raster1D);
	mdi.AddOutput("ChWidth", "m", "Flow length of overland plane", DT_Raster1D);  //Flowlen add by Wu hui
	
	mdi.AddOutput("RadianSlope", "", "radian slope", DT_Raster1D);

	mdi.AddOutput("ID_OUTLET", "", "index of outlet in the compressed array", DT_Single);

	// set the dependencies
	mdi.AddDependency("Depression","Depression module");

	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}