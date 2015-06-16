#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "KinWavSed_CH.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new KinWavSed_CH();
}

// function to return the XML Metadata document string
//Set up metadata information
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res;

	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Hui Wu");
	mdi.SetClass("Channel Erosion", "Calculate the amount channel erosion.");
	mdi.SetDescription("use Srinivasan & Galvao function to calculate sediment yield routing of each channel cell");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("KinWavSed_CH");
	mdi.SetName("KinWavSed_CH");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("KinWavSed_CH.chm");

	mdi.AddParameter("CellWidth","m","the width (length) of cell","mask.asc",DT_Single);
	mdi.AddParameter("DT_HS", "second", "Time step of the simulation", "file.in", DT_Single);
	mdi.AddParameter("ChTcCo","","calibration coefficient of transport capacity","ParameterDB_Sediment",DT_Single);
	/*mdi.AddParameter("eco1","","calibration coefficient of transport capacity calculation","ParameterDB_Sediment",DT_Single);
	mdi.AddParameter("eco2","","calibration coefficient of transport capacity calculation","ParameterDB_Sediment",DT_Single);*/
	mdi.AddParameter("ChDetCo","","calibration coefficient of channel flow detachment","ParameterDB_Sediment",DT_Single);

	mdi.AddParameter("Slope", "%", "Slope", "ParameterDB_WaterBalance", DT_Raster); 
	mdi.AddParameter("USLE_K","","the soil erodibility factor","ParameterDB_Sediment",DT_Raster);
	
	//mdi.AddParameter("Manning_nCh","","Manning's n of channel cells", "manning.txt", DT_Array1D); //from the reach parameters 
	mdi.AddParameter("CHWIDTH", "m", "Channel width", "ParameterDB_Discharge", DT_Raster); 
	mdi.AddParameter("FLOWOUT_INDEX_D8", "", "The index of flow in cell in the compressed array", "ParameterDB_Discharge", DT_Array1D);
	mdi.AddParameter("FLOWIN_INDEX_D8", "", "The index of flow in cell in the compressed array,"
		" and the first element in each sub-array is the number of flow in cells in this sub-array", "ParameterDB_Discharge", DT_Array2D);
	mdi.AddParameter("STREAM_LINK", "", "Stream link (id of reaches)", "ParameterDB_Discharge", DT_Raster);
	mdi.AddParameter("ReachParameter", "", "Reach parameters such stream order, manning's n and downstream subbasin id", "ParameterDB_Discharge", DT_Array2D);

	//input from other module	
	//mdi.AddInput("T_HCH","mm", "Water depth in the downslope boundary of cells","Module",DT_Array2D);		//chWH, from channel routing module
	mdi.AddInput("HCH","mm", "Water depth in the downslope boundary of cells","Module",DT_Array2D);
	//mdi.AddInput("FlowWidth","m", "Flow width of overland plane","Module",DT_Raster);		//FlowWidth, from Overland routing module
	//mdi.AddInput("T_QCH", "m3/s", "Flux in the downslope boundary of cells", "Module",DT_Array2D);
	mdi.AddInput("QRECH", "m3/s", "Flux in the downslope boundary of cells", "Module",DT_Array2D);
	mdi.AddInput("D_SEDTOCH","kg", "distribution of sediment flowing to channel", "Module",DT_Raster);
	// set the output variables
	/*mdi.AddOutput("CHDETFLOW","kg/m^2", "distribution of channel flow detachment", DT_Raster);
	mdi.AddOutput("CHSEDDEP","kg", "distribution of channel sediment deposition", DT_Raster);
	mdi.AddOutput("CHSEDCONC","kg/m^3", "distribution of sediment concentration in channel flow", DT_Raster);
	mdi.AddOutput("CHSEDINFLOW","kg", "distribution of sediment content in channel flow", DT_Raster);*/
	mdi.AddOutput("SEDSUBBASIN","ton/s", "sediment yield production of each subbasin", DT_Raster);
	mdi.AddOutput("SEDOUTLET", "kg/m3", "sediment concentration at the watershed outlet", DT_Single);

	//test
	mdi.AddOutput("DEP","kg", "distribution of channel sediment deposition", DT_Raster);
	mdi.AddOutput("DET","kg", "distribution of channel flow detachment", DT_Raster);
	mdi.AddOutput("QSN","kg/s", "distribution of channel sediment rate", DT_Raster);
	mdi.AddOutput("CAP","kg", "distribution of channel flow capacity", DT_Raster);
	mdi.AddOutput("CHANV","kg", "distribution of channel flow capacity", DT_Raster);
	mdi.AddOutput("CHANVOL","kg", "distribution of channel flow capacity", DT_Raster);

	//mdi.AddDependency("Overland routing","Overland routing module");      //for WH
	mdi.AddDependency("Overland erosion","Overland erosion module");  
	mdi.AddDependency("Channel routing","Channel routing module"); 
	
	// write out the XML file.
	res = mdi.GetXMLDocument();

	//return res;

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}