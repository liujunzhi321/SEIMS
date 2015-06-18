#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "NutrientTransport_OL.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new NutrientTransport_OL();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Wang Lin");
	mdi.SetClass("NutrientTransport_OL", "Nutrient Transport Overland.");
	mdi.SetDescription("Nutrient Transport Overland.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("NutrientTransport_OL.chm");
	mdi.SetID("NutrientTransport_OL");
	mdi.SetName("NutrientTransport_OL");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	// Nitrate
	mdi.AddParameter("AniExcF", "", "the fraction of porosity from which anions are excluded", "file.in", DT_Single); 
	mdi.AddParameter("NitPC", "", "the nitrate percolation coefficient", "file.in", DT_Single);
	
	mdi.AddParameter("Porosity_2D","m3/m3","soil porosity","ParameterDB_WaterBalance",DT_Array2D);
	
	mdi.AddInput("D_Nitrate", "kg N/ha", "amount of nitrate", "Module", DT_Array2D);
	
	mdi.AddInput("D_SURU", "mm", "Distribution of surface runoff", "Module",DT_Raster);   //from DEP_LINSLEY
	mdi.AddInput("D_SSRU_2D","mm", "Distribution of subsurface runoff(lateral flow / interflow).", "Module", DT_Array2D);  //from SSR_DA
	mdi.AddInput("D_Percolation_2D","mm", "Distribution of groundwater recharge (percolation)", "Module", DT_Array2D);  // from PER_PI

	// Ammonium
	mdi.AddParameter("Clay_2D", "", "Clay fraction in soil", "ParameterDB_WaterBalance", DT_Raster); 
	mdi.AddInput("D_Ammon", "kg N/ha", "ammonium pool for soil nitrogen", "Module", DT_Array2D);

	// Organic N
	mdi.AddParameter("CellWidth","m","the width (length) of cell","mask.asc",DT_Single);
	mdi.AddParameter("Density_2D", "g/cm3", "Soil density", "ParameterDB_WaterBalance", DT_Array2D);
    //mdi.AddParameter("RootDepth", "mm", "depth from the soil surface", "file.in", DT_Raster);
	
	mdi.AddInput("D_SOER","metric tons", "distribution of soil erosion", "Module", DT_Raster);  // from MUSLE_AS
	mdi.AddInput("D_Depth","mm","depth of the layer","Module", DT_Array2D);
	mdi.AddInput("D_ActOrgN", "kg N/ha", "amount of nitrogen in the active organic pool in layer ly", "Module", DT_Array2D);
    mdi.AddInput("D_StaOrgN", "kg N/ha", "amount of nitrogen in the stable organic pool", "Module", DT_Array2D);
	mdi.AddInput("D_FreOrgN", "kg N/ha", "nitrogen in the fresh organic pool in layer ly", "Module", DT_Array2D);

	// soluble P
	mdi.AddParameter("ParCoeP", "m3/Mg", "phosphorus soil partitioning coefficient", "file.in", DT_Single);

	mdi.AddInput("D_SoluP", "kg P/ha", "amount of phosphorus in solution in layer ly", "Module", DT_Array2D);

	// Attached P
	mdi.AddInput("D_ActMinP", "kg P/ha", "amount of phosphorus in the active mineral pool","Module", DT_Array2D);
	mdi.AddInput("D_StaMinP", "kg P/ha", "amount of phosphorus in the stable mineral pool","Module", DT_Array2D);
	mdi.AddInput("D_HumOrgP", "kg P/ha", "amount of humic organic phosphorus in the layer", "Module", DT_Array2D);
	mdi.AddInput("D_FreOrgP", "kg P/ha", "phosphorus in the fresh organic pool in the top 10mm", "Module", DT_Array2D);

	// Nitrate
	mdi.AddOutput("SurNit", "kg N/ha", "the nitrate removed in surface runoff", DT_Raster); 

	mdi.AddOutput("SatW","mm H2O", "the saturated water content of the soil layer", DT_Array2D);
	mdi.AddOutput("MobQ", "mm H2O", "the amount of mobile water in the layer", DT_Array2D); 
	mdi.AddOutput("ConNit", "kg N/mm H2O", "the concentration of nitrate in the mobile water for a given layer", DT_Array2D);
	mdi.AddOutput("LatNit", "kg N/ha", " the nitrate removed in lateral flow from a layer", DT_Array2D); 
	mdi.AddOutput("PerNit", "kg N/ha", "the nitrate moved to the underlying layer by percolation", DT_Array2D); 

	// Ammonium
	mdi.AddOutput("SurAmm", "kg N/ha", "the ammonium removed in surface runoff", DT_Raster); 
	mdi.AddOutput("ConAmm", "kg N/mm H2O", "the concentration of Ammonium in the mobile water for a given layer", DT_Array2D);

	// Organic N
	mdi.AddOutput("ConSed", "Mg sed/m3 H2O", "the concentration of sediment in surface runoff", DT_Raster);
	mdi.AddOutput("EnrN", "", "nitrogen enrichment ratio",DT_Raster);
	mdi.AddOutput("ConOrgN", "g N/ metric ton soil", "the concentration of organic nitrogen in the top 10 mm", DT_Raster);
	mdi.AddOutput("TraOrgN", "kg N/ha", "the amount of organic nitrogen transported to the next cell in surface runoff", DT_Raster);

	// soluble P
	mdi.AddOutput("SurSolP", "kg P/ha", "the amount of soluble phosphorus lost in surface runoff", DT_Raster);

	// Attached P
	mdi.AddOutput("EnrP", "", "phosphorus enrichment ratio",DT_Raster);
	mdi.AddOutput("ConAttP", "g P/ metric ton soil", "the concentration of phosphorus attached to sediment in the top 10 mm", DT_Raster);
	mdi.AddOutput("TraAttP", "kg P/ha", "the amount of phosphorus attached to sediment transported to the next cell in surface runoff", DT_Raster);

	// to subbasin
	mdi.AddParameter("subbasin","","subbasin grid","ParameterDB_Discharge", DT_Raster);
	mdi.AddOutput("SNITTOCH","kg", "surface nitrate to channel", DT_Array1D); 
	mdi.AddOutput("SNITTOCH_T","kg", "Total surface nitrate flowing to channel", DT_Single); 
	mdi.AddOutput("SAMMTOCH","kg", "surface ammonium to channel", DT_Array1D); 
	mdi.AddOutput("SAMMTOCH_T","kg", "Total surface ammonium flowing to channel", DT_Single);
	mdi.AddOutput("SSOLPTOCH","kg", "surface soluble phosphorus to channel", DT_Array1D); 
	mdi.AddOutput("SSOLPTOCH_T","kg", "Total surface soluble phosphorus flowing to channel", DT_Single);
	mdi.AddOutput("ORGNTOCH","kg", "the amount of organic nitrogen transported to channel in surface runoff(", DT_Array1D); 
	mdi.AddOutput("ORGNTOCH_T","kg", "Total organic nitrogen transported to channel in surface runoff", DT_Single);
	mdi.AddOutput("ATTPTOCH","kg", "the amount of phosphorus attached to sediment transported to channel in surface runoff", DT_Array1D); 
	mdi.AddOutput("ATTPTOCH_T","kg", "Total phosphorus attached to sediment transported to channel in surface runoff", DT_Single);
	mdi.AddOutput("LANITTOCH","kg", "lateral nitrate to channel", DT_Array1D); 
	mdi.AddOutput("LANITTOCH_T","kg", "Total lateral nitrate flowing to channel", DT_Single); 

	// cell to cell method
	mdi.AddParameter("CHWIDTH", "m", "Channel width", "ParameterDB_Discharge", DT_Raster); 
	mdi.AddParameter("STREAM_LINK", "", "Stream link", "ParameterDB_Discharge", DT_Raster); 
	//mdi.AddParameter("FLOWOUT_INDEX_DINF", "", "The index of flow in cell in the compressed array", "ParameterDB_Discharge", DT_Array2D);
	mdi.AddParameter("FLOWIN_INDEX_D8", "", "The index of flow in cell in the compressed array,"
		" and the first element in each sub-array is the number of flow in cells in this sub-array", "ParameterDB_Discharge", DT_Array2D);
	//mdi.AddParameter("FLOWIN_PERCENTAGE_DINF", "", "Percentage of flow in", "ParameterDB_Discharge", DT_Array2D);
	mdi.AddParameter("ROUTING_LAYERS", "", "Routing layers according to the flow direction"
		"There are not flow relationships within each layer","ParameterDB_Discharge", DT_Array2D);

	// set the dependencies
	mdi.AddDependency("DEP_FS","Depression Storage module");
	mdi.AddDependency("SSR_DA","Subsurface Runoff module");
	mdi.AddDependency("MUSLE_AS","Calculate the amount of sediment yield.");

	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}