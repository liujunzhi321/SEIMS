#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "MineralizationAndDecomposition.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new MineralizationAndDecomposition();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Wang Lin");
	mdi.SetClass("MineralizationAndDecomposition", "Mineralization and decomposition/immobilization.");
	mdi.SetDescription("Mineralization and decomposition/immobilization of residue and humus.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("MineralizationAndDecomposition.chm");
	mdi.SetID("MineralizationAndDecomposition");
	mdi.SetName("MineralizationAndDecomposition");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("RateConst", "", "rate constant", "file.in", DT_Single); 
	mdi.AddParameter("HumNF", "m", "fraction of humic nitrogen in the active pool", "file.in", DT_Single); 
	mdi.AddParameter("RateCoeH", "", "rate coefficient for mineralization of the humus active organic nutrients", "file.in", DT_Single); 
	mdi.AddParameter("RateCoeR", "", "rate coefficient for mineralization of the residue fresh organic nutrients", "file.in", DT_Single); 
	mdi.AddParameter("ConSoluP", "mg/kg", "concentration of solution phosphorus all layers", "file.in", DT_Single);

	//mdi.AddParameter("RootDepth", "mm", "depth from the soil surface", "file.in", DT_Raster);

	//mdi.AddParameter("Res", "kg/ha", "residue in layer ly", "ParameterDB_soil", DT_Array2D);
	//mdi.AddParameter("OrgCar", "%", "amount of organic carbon in the layer", "ParameterDB_soil", DT_Array2D);  
	//mdi.AddParameter("SOM", "%", "amount of soil organic matter in the layer", "ParameterDB_soil", DT_Array2D);
	mdi.AddParameter("Density_2D", "g/cm3", "Soil density", "ParameterDB_WaterBalance", DT_Array2D);
	mdi.AddParameter("FieldCap_2D", "%", "Field capacity","ParameterDB_WaterBalance", DT_Array2D);

	mdi.AddInput("D_SOTE","oC", "Soil Temperature","Module", DT_Raster);
	mdi.AddInput("D_Nitrate", "kg N/ha", "amount of nitrate", "Module", DT_Array2D);
	mdi.AddInput("D_SOMO_2D","%","Soil Moisture","Module", DT_Array2D);
	mdi.AddInput("D_Depth","mm","depth of the layer","Module", DT_Array2D);
	
	//mdi.AddOutput("OrgCar", "%", "amount of organic carbon in the layer", DT_Array2D);
	mdi.AddOutput("ConHumOrgN", "mg/kg", "concentration of humic organic nitrogen in the layer", DT_Array2D); 
	mdi.AddOutput("ConActOrgN", "mg/kg", "concentration of nitrogen in the active organic pool", DT_Array2D); 
	mdi.AddOutput("ConStaOrgN", "mg/kg", "concentration of nitrogen in the stable organic pool", DT_Array2D);

	mdi.AddOutput("HumOrgN", "kg N/ha", "amount of humic organic nitrogen in the layer", DT_Array2D);
	mdi.AddOutput("ActOrgN", "kg N/ha", "amount of nitrogen in the active organic pool in layer ly", DT_Array2D);
    mdi.AddOutput("StaOrgN", "kg N/ha", "amount of nitrogen in the stable organic pool", DT_Array2D);
	mdi.AddOutput("FreOrgN", "kg N/ha", "nitrogen in the fresh organic pool in layer ly", DT_Array2D);
	
	mdi.AddOutput("TF","", "nutrient cycling temperature factor", DT_Array2D);
	mdi.AddOutput("WF", "", "nutrient cycling water factor", DT_Array2D); 

	mdi.AddOutput("TraOrgN", "kg N/ha", "amount of nitrogen transferred between the active and stable organic pools", DT_Array2D); 
	mdi.AddOutput("MinHumN", "", "nitrogen mineralized from the humus active organic N pool", DT_Array2D);
	mdi.AddOutput("CNRat", "", "C:N ratio of the residue in the soil layer", DT_Array2D);
	mdi.AddOutput("CPRat", "", "C:P ratio of the residue in the soil layer", DT_Array2D);
	mdi.AddOutput("ResComF", "", "nutrient cycling residue composition factor", DT_Array2D);

	mdi.AddOutput("RateConRD", "", "residue decay rate constant", DT_Array2D);
	mdi.AddOutput("MinFreOrgN", "kg N/ha", "nitrogen mineralized from the fresh organic N pool", DT_Array2D);
	mdi.AddOutput("DecFreOrgN", "kg N/ha", "nitrogen decomposed from the fresh organic N pool", DT_Array2D);

	mdi.AddOutput("SoluP", "kg P/ha", "amount of phosphorus in solution in layer ly", DT_Array2D);
	mdi.AddOutput("HumOrgP", "kg P/ha", "amount of humic organic phosphorus in the layer", DT_Array2D);
	mdi.AddOutput("FreOrgP", "kg P/ha", "phosphorus in the fresh organic pool in the top 10mm", DT_Array2D);

	mdi.AddOutput("ActOrgP", "kg P/ha", "amount of phosphorus in the active organic pool", DT_Array2D);
	mdi.AddOutput("StaOrgP", "kg P/ha", "amount of phosphorus in the stable organic pool", DT_Array2D);
	mdi.AddOutput("MinHumP", "kg P/ha", "phosphorus mineralized from the humus active organic P pool", DT_Array2D);
	mdi.AddOutput("MinFreOrgP", "kg P/ha", "phosphorus mineralized from the fresh organic P pool", DT_Array2D);
	mdi.AddOutput("DecFreOrgP", "kg P/ha", "phosphorus decomposed from the fresh organic P pool", DT_Array2D);
	//??
	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}