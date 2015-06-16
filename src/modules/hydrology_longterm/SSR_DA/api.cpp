#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SSR_DA.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SSR_DA();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	mdi.SetAuthor("Zhiqiang Yu; Junzhi Liu");
	mdi.SetClass("Subsurface Runoff", "");
	mdi.SetDescription("Darcy's law and the kinematic approximation; Water is routed cell-to-cell according to D8 or Dinf flow direction");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("SSR_DA");
	mdi.SetName("SSR_DA");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("SSR_DA.chm");

	mdi.AddParameter("CellWidth","m","","file.in",DT_Single);
	mdi.AddParameter("TimeStep","h","","file.in",DT_Single);
	//mdi.AddParameter("UpperSoilDepth", "mm", "depth of the upper soil layer", "ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("Ki","-","Interflow scale factor","ParameterDB_WaterBalance",DT_Single);
	mdi.AddParameter("T_Soil","oC","threshold soil freezing temperature","ParameterDB_WaterBalance", DT_Single);				

	mdi.AddParameter("Conductivity_2D","mm/h","saturation hydraulic conductivity","ParameterDB_WaterBalance",DT_Array2D);
	mdi.AddParameter("Porosity_2D","m3/m3","soil porosity","ParameterDB_WaterBalance",DT_Array2D);
	mdi.AddParameter("Poreindex_2D","-","pore size distribution index","ParameterDB_WaterBalance",DT_Array2D);
	mdi.AddParameter("FieldCap_2D","m3/m3","Soil field capacity","ParameterDB_WaterBalance",DT_Array2D);

	mdi.AddParameter("Rootdepth","mm","Root depth","ParameterDB_WaterBalance",DT_Raster);
	mdi.AddParameter("Slope","%","Soil field capacity","ParameterDB_WaterBalance", DT_Raster);		

	mdi.AddParameter("CHWIDTH", "m", "Channel width", "ParameterDB_Discharge", DT_Raster); 
	mdi.AddParameter("STREAM_LINK", "", "Stream link", "ParameterDB_Discharge", DT_Raster); 
	//mdi.AddParameter("FLOWOUT_INDEX_DINF", "", "The index of flow in cell in the compressed array", "ParameterDB_Discharge", DT_Array2D);
	mdi.AddParameter("FLOWIN_INDEX_D8", "", "The index of flow in cell in the compressed array,"
		" and the first element in each sub-array is the number of flow in cells in this sub-array", "ParameterDB_Discharge", DT_Array2D);
	//mdi.AddParameter("FLOWIN_PERCENTAGE_DINF", "", "Percentage of flow in", "ParameterDB_Discharge", DT_Array2D);
	mdi.AddParameter("ROUTING_LAYERS", "", "Routing layers according to the flow direction"
		"There are not flow relationships within each layer","ParameterDB_Discharge", DT_Array2D);

	mdi.AddParameter("subbasin","","The subbasion grid","ParameterDB_Snow",DT_Raster);

	// set the parameters (non-time series)
	mdi.AddInput("D_GRRE","mm","perculation","Module",DT_Raster);
	mdi.AddInput("D_SOTE","oC", "Soil Temperature","Module", DT_Raster);
	mdi.AddInput("D_SOMO_2D","mm","Distribution of soil moisture","Module",DT_Array2D);

	// set the output variables
	mdi.AddOutput("SSRU_2D","mm", "Distribution of subsurface runoff (mm).", DT_Array2D);
	mdi.AddOutput("SSRUVOL_2D","mm", "Distribution of subsurface runoff (m3).", DT_Array2D);
	mdi.AddOutput("SBIF", "mm","Interflow to streams for each subbasin", DT_Array1D);

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}
