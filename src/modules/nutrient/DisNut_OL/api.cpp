#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "DissolvedNutrient_OL.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new DissolvedNutrient_OL();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Hui Wu");
	mdi.SetClass("Dissolved Nutrient OL", "Dissolved Nutrient Overland.");
	mdi.SetDescription("Dissolved Nutrient Overland transport method using Knisel functions.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("DissolvedNutrient_OL.chm");
	mdi.SetID("DissolvedNutrient_OL");
	mdi.SetName("DissolvedNutrient_OL");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("DT_HS", "second", "Time step of the simulation", "file.in", DT_Single); 
	mdi.AddParameter("CellWidth", "m", "Cell size", "file.in", DT_Single); 
	mdi.AddParameter("Slope", "%", "Slope", "ParameterDB_WaterBalance", DT_Raster); 
	//mdi.AddParameter("Theta", "", "theta parameter for 4-point implicit finite difference", "ParameterDB_Discharge", DT_Single); 
	mdi.AddParameter("InitConc_P", "kg/kg", "Initial average concentration of P in the soil", "ParameterDB_soil", DT_Single);
	mdi.AddParameter("InitConc_NH4", "kg/kg", "Initial average concentration of ammonium in the soil", "ParameterDB_soil", DT_Single);
	mdi.AddParameter("InitConc_NO3", "kg/kg", "Initial average concentration of nitrate in the soil", "ParameterDB_soil", DT_Single);
	mdi.AddParameter("Clay", "", "Clay proportion in soil", "ParameterDB_WaterBalance", DT_Raster); 
	mdi.AddParameter("Porosity", "m/m", "Soil porosity", "ParameterDB_WaterBalance", DT_Raster); 
	mdi.AddParameter("CHWIDTH", "m", "Channel width", "ParameterDB_Discharge", DT_Raster); 
	mdi.AddInput("D_FlowWidth","m", "Flow width of overland plane","Module",DT_Raster);
	//mdi.AddInput("D_FlowLen","m", "Flow length of overland plane","Module",DT_Raster);
	mdi.AddParameter("STREAM_LINK", "", "Stream link", "ParameterDB_Discharge", DT_Raster); 
	mdi.AddParameter("Manning", "", "Manning's roughness", "ParameterDB_Discharge", DT_Raster); 
	//mdi.AddParameter("FLOW_DIR", "", "Flow direction  by the rule of ArcGIS", "ParameterDB_Discharge", DT_Raster);	

	//mdi.AddParameter("FLOWOUT_INDEX", "", "The index of flow in cell in the compressed array", "ParameterDB_Discharge", DT_Array1D);
	mdi.AddParameter("FLOWIN_INDEX_D8", "", "The index of flow in cell in the compressed array,"
		" and the first element in each sub-array is the number of flow in cells in this sub-array", "ParameterDB_Discharge", DT_Array2D);
	mdi.AddParameter("ROUTING_LAYERS", "", "Routing layers according to the flow direction"
		"There are not flow relationships within each layer, and the first element in each layer is the number of cells in the layer", "ParameterDB_Discharge", DT_Array2D);

	mdi.AddInput("D_CELLQ", "m3/s", "Flux in the downslope boundary of cells", "Module",DT_Raster);
	mdi.AddInput("D_CELLH","mm","Water depth in the downslope boundary of cells","Module",DT_Raster);	
	mdi.AddInput("D_INFIL","mm","Infiltration map of watershed", "Module", DT_Raster);
	mdi.AddInput("D_DPST", "mm", "Distribution of depression storage", "Module", DT_Raster);
	mdi.AddInput("D_HTOCH", "mm", "Water depth added to channel water depth","Module", DT_Raster);

	mdi.AddOutput("DissovP", "kg/s", "distribution of dissovlved P", DT_Raster);
	mdi.AddOutput("Ammonium", "kg/s", "distribution of nutrient, ammonium", DT_Raster);
	mdi.AddOutput("NitrateDisNutOL", "kg/s", "distribution of nutrient, nitrate", DT_Raster);

	mdi.AddOutput("DissovPToCh", "kg", "dissovlved P flow to channel", DT_Raster);
	mdi.AddOutput("AmmoniumToCh", "kg", "ammonium flow to channel", DT_Raster);
	mdi.AddOutput("NitrateToCh", "kg", "nitrate flow to channel", DT_Raster);
	//test
	mdi.AddOutput("TESTDPST", "mm", "Distribution of depression storage", DT_Raster);
	//mdi.AddOutput("ID_OUTLET", "", "index of outlet in the compressed array", DT_Single);

	// set the dependencies
	mdi.AddDependency("Depression","Calculate depression storage"); 
	mdi.AddDependency("Overland routing","Overland routing module");      //for WH
	//mdi.AddDependency("Soil Detachment","Rain and overland soil detachment erosion module");    

	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}