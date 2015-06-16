#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "KinWavSed_OL.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new KinWavSed_OL();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res;

	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Hui Wu");
	mdi.SetClass("Overland Erosion", "Calculate the amount overland erosion.");
	mdi.SetDescription("use energy function(Govers) method to calculate sediment yield routing of each hillslope cell");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("KinWavSed_OL");
	mdi.SetName("KinWavSed_OL");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("KinWavSed_OL.chm");

	mdi.AddParameter("CellWidth","m","the width (length) of cell","mask.asc",DT_Single);
	mdi.AddParameter("DT_HS", "second", "Time step of the simulation", "file.in", DT_Single);
	mdi.AddParameter("Slope", "%", "Slope", "ParameterDB_WaterBalance", DT_Raster); 
	mdi.AddParameter("eco1","","calibration coefficient of transport capacity calculation","ParameterDB_Sediment",DT_Single);
	mdi.AddParameter("eco2","","calibration coefficient of transport capacity calculation","ParameterDB_Sediment",DT_Single);
	mdi.AddParameter("ccoe","","calibration coefficient of overland erosion","ParameterDB_Sediment",DT_Single);
	mdi.AddParameter("USLE_K","","the soil erodibility factor","ParameterDB_Sediment",DT_Raster);
	mdi.AddParameter("USLE_C","","the cover management factor","ParameterDB_Sediment",DT_Raster);
	//mdi.AddParameter("StoneFrac","","the fraction of stones on the surface, affects splash","ParameterDB_Sediment",DT_Raster);
	//mdi.AddParameter("GrassFrac","","the fraction of grasstrip in a cell","ParameterDB_Sediment",DT_Raster);
	mdi.AddParameter("STREAM_LINK", "", "Stream link", "ParameterDB_Discharge", DT_Raster); 
	mdi.AddParameter("Manning","","Manning's roughness","ParameterDB_Discharge",DT_Raster);
	mdi.AddParameter("CHWIDTH", "m", "Channel width", "ParameterDB_Discharge", DT_Raster); 
	mdi.AddParameter("FLOWIN_INDEX_D8", "", "The index of flow in cell in the compressed array,"
		" and the first element in each sub-array is the number of flow in cells in this sub-array", "ParameterDB_Discharge", DT_Array2D);
	mdi.AddParameter("ROUTING_LAYERS", "", "Routing layers according to the flow direction"
		"There are not flow relationships within each layer, and the first element in each layer is the number of cells in the layer", "ParameterDB_Discharge", DT_Array2D);

	//input from other module	
	//mdi.AddInput("D_CELLH","mm","Water depth in the downslope boundary of cells","Module",DT_Raster);		//WH, from Overland routing module
	mdi.AddInput("D_SURU","mm","Distribution of surface runoff","Module",DT_Raster);
	mdi.AddInput("D_FlowWidth","m", "Flow width of overland plane","Module",DT_Raster);		//FlowWidth, from Overland routing module
	mdi.AddInput("D_DETSplash","kg","the distribution of splash detachment","Module",DT_Raster);	//Splash eosion, from splash erosion module
	mdi.AddInput("D_QOverland", "m3/s", "Flux in the downslope boundary of cells", "Module",DT_Raster);
	//mdi.AddInput("D_HTOCH", "mm", "Water depth added to channel water depth", "Module",DT_Raster);
	// set the output variables
	mdi.AddOutput("DETOverland","kg", "distribution of overland flow detachment", DT_Raster);
	//mdi.AddOutput("DETFLOW","kg/m^2", "distribution of flow detachment", DT_Raster);
	mdi.AddOutput("SEDDEP","kg", "distribution of sediment deposition", DT_Raster);
	//mdi.AddOutput("SEDCONC","kg/m^3", "distribution of sediment concentration in flow", DT_Raster);
	//mdi.AddOutput("SedKG","kg", "distribution of sediment content in flow", DT_Raster);
	mdi.AddOutput("TestV","m/s", "distribution of sediment content in flow", DT_Raster);
	mdi.AddOutput("TestQV","m/s", "distribution of sediment content in flow", DT_Raster);
	mdi.AddOutput("SEDTOCH","kg", "distribution of sediment flowing to channel", DT_Raster);

	mdi.AddDependency("Depression","Depression storage module"); //for WH
	mdi.AddDependency("Overland routing","Overland routing module");      
	mdi.AddDependency("Soil Detachment","Rain and overland soil detachment erosion module");          // for DETSplash

	// write out the XML file.
	res = mdi.GetXMLDocument();

	//return res;

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}
