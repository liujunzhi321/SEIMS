#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "Yield.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new Yield();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Cheng Wei");
	mdi.SetClass("Yield", "Calculate the amount of yield.");
	mdi.SetDescription("For simplicity purpose for the time being, the algorithm used in the original WetSpa is incorporated in the SEIMS.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("Yield");
	mdi.SetName("Yield");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("Yield.chm");

	mdi.AddParameter("IDC","","land cover/plant classification","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("CNYLD","","fraction of nitrogen in the yield","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("CPYLD","kg/ha"," fraction of phosphorus in the yield","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("HVSTI","kg/ha"," potential harvest index for the plant at maturity given ideal growing conditions","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("WSYF","","harvest index for the plant in drought conditions and represents the minimum harvest index allowed for the plant","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("DLAI","","Decline LAI Fraction","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("harvestDate","","the simulation end of date","ParameterDB",DT_Raster);

	mdi.AddInput("LAI","","leaf area index for a given day","Module", DT_Raster);
	
	mdi.AddInput("D_PET","mm/d", "Potential evapotranspiration", "Module",DT_Raster);///////????zhiqinag: "D_PETC"
	mdi.AddInput("D_SOET","mm","soil evapotranpiration","Module",DT_Raster);					//actual evapotranspiration
	mdi.AddInput("Fr_PHU","","fraction of phu accumulated on a given day ","Module",DT_Raster);
	mdi.AddInput("BIOMASS_N","kg/ha","actual biomass N","Module",DT_Raster);
	mdi.AddInput("BIOMASS_P","kg/ha","actual biomass P","Module",DT_Raster);
	mdi.AddInput("BIOMASS_AG","kg/ha"," aboveground biomass on the day of harvest","Module",DT_Raster);
	mdi.AddInput("BIOMASS_ROOT","kg/ha","biomass root","Module",DT_Raster);
	mdi.AddInput("BIOMASS","kg/ha","total biomass","Module",DT_Raster);
	mdi.AddInput("Total_WaterUptake","mm","total water uptake","Module",DT_Raster); 
	// set the output variables
	mdi.AddOutput("Yield_N","kg/ha","amount of nitrogen removed in the yield",DT_Raster);
	mdi.AddOutput("Yield_P","kg/ha","amount of phosphorus removed in the yield",DT_Raster);
	mdi.AddOutput("Yield","kg/ha","crop yield",DT_Raster);

	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}