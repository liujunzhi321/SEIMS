#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "InterFlow.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new InterFlow();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Junzhi Liu");
	mdi.SetClass("interflow routing", "interflow routing.");
	mdi.SetDescription("interflow routing using the method of WetSpa model.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("INTERFLOW_IKW.chm");
	mdi.SetID("INTERFLOW_IKW");
	mdi.SetName("INTERFLOW_IKW");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("DT_HS", "second", "Time step of the simulation", "file.in", DT_Single); 
	mdi.AddParameter("CellWidth", "m", "Cell size", "file.in", DT_Single); 

	mdi.AddParameter("Slope", "%", "Slope", "ParameterDB_WaterBalance", DT_Raster); 
	mdi.AddParameter("CHWIDTH", "m", "Channel width", "ParameterDB_Discharge", DT_Raster); 

	mdi.AddParameter("STREAM_LINK", "", "Stream link", "ParameterDB_Discharge", DT_Raster); 
	mdi.AddParameter("FLOWIN_INDEX_D8", "", "The index of flow in cell in the compressed array,"
		" and the first element in each sub-array is the number of flow in cells in this sub-array", "ParameterDB_Discharge", DT_Array2D);
	mdi.AddParameter("ROUTING_LAYERS", "", "Routing layers according to the flow direction"
		"There are not flow relationships within each layer, and the first element in each layer is the number of cells in the layer", "ParameterDB_Discharge", DT_Array2D);
	
	mdi.AddParameter("Conductivity","mm/h","Soil hydraulic conductivity","ParameterDB_WaterBalance", DT_Raster); 
	mdi.AddParameter("Porosity", "%", "Soil porosity","ParameterDB_WaterBalance", DT_Raster);
	mdi.AddParameter("FieldCap", "%", "Field capacity","ParameterDB_WaterBalance", DT_Raster);
	mdi.AddParameter("RootDepth", "mm", "Root depth","ParameterDB_WaterBalance", DT_Raster);
	mdi.AddParameter("Poreindex","-","pore size distribution index","ParameterDB_WaterBalance",DT_Raster);
	mdi.AddParameter("Ki","-","Interflow scale factor","ParameterDB_WaterBalance",DT_Single);

	mdi.AddInput("D_SOMO","%","Soil Moisture","Module", DT_Raster);
	mdi.AddInput("D_SURU", "mm", "surface runoff (considering return flow)", "Module", DT_Raster);
	
	mdi.AddOutput("QSoil", "m3/s", "Flux in the downslope boundary of cells", DT_Raster);
	mdi.AddOutput("ReturnFlow", "mm", "water depth of return flow", DT_Raster);

	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}