#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "UptakeGrowth.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new UptakeGrowth();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Cheng Wei");
	mdi.SetClass("UptakeGrowth", "Calculate the amount of UptakeGrowth.");
	mdi.SetDescription("For simplicity purpose for the time being, the algorithm used in the original WetSpa is incorporated in the SEIMS.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("UptakeGrowth");
	mdi.SetName("UptakeGrowth");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("UptakeGrowth.chm");

	mdi.AddParameter("IDC","","land cover/plant classification","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("nFixFactor","","nitrogen fixation factor","ParameterDB_PlantGrowth",DT_Single);
	mdi.AddParameter("nFixMax","kg/ha","maximum daily-n fixation","ParameterDB_PlantGrowth",DT_Single);
	mdi.AddParameter("wDis","","water-use distribution parameter","ParameterDB_PlantGrowth",DT_Single);
	mdi.AddParameter("wCom","","plant uptake compensation factor","ParameterDB_PlantGrowth",DT_Single);
	mdi.AddParameter("nDis","","nitrogen uptake distribution parameter","ParameterDB_PlantGrowth",DT_Single);
	mdi.AddParameter("pDis","","phosphorus uptake distribution parameter, P_UPDIS","ParameterDB_PlantGrowth",DT_Single);
	mdi.AddParameter("soilLayer","","the num of soil layers","ParameterDB",DT_Single);
	//mdi.AddParameter("startDate","","the date begin to simulate ","ParameterDB",DT_Raster);
	//mdi.AddParameter("endDate","","the simulation end of date","ParameterDB",DT_Raster);

	mdi.AddParameter("T_OPT","oC"," optimal temperature for growth","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("T_BASE","oC"," base or minimum temperature for growth","ParameterDB_Crop",DT_Raster);
	
	mdi.AddParameter("RDMX","mm","the maximum depth for root development in the soil","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("Fieldcap_2D","m3/m3","Soil field capacity","ParameterDB_WaterBalance", DT_Array2D);								//0f
	mdi.AddParameter("Wiltingpoint_2D","m3/m3","Plant wilting point moisture","ParameterDB_WaterBalance", DT_Array2D);					//0w	

	mdi.AddParameter("BMX_TREES","kg/ha"," biomass of a fully develop tree stand for the specific tree species ","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("MAT_YRS","years","the number of years for the tree species to reach full development","ParameterDB_Crop",DT_Raster);
	
	mdi.AddParameter("BN1",""," the normal fraction of nitrogen in the plant biomass at emergence","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("BN2",""," the normal fraction of nitrogen in the plant biomass at 50% maturity ","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("BN3","","  the normal fraction of nitrogen in the plant biomass at maturity","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("BP1",""," the normal fraction of phosphorus in the plant biomass at emergence","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("BP2",""," the normal fraction of phosphorus in the plant biomass at 50% maturity","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("BP3","","the normal fraction of phosphorus in the plant biomass at maturity","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("DLAI",""," the fraction of growing season(PHU) when senescence becomes dominant ","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("BLAI","","maximum leaf area index","ParameterDB_Crop",DT_Raster);

	mdi.AddInput("D_TMIN","oC","min temperature","Module",DT_Raster);									// from interpolation module
	mdi.AddInput("D_TMAX","oC","max temperature","Module",DT_Raster);
	mdi.AddInput("D_PET","mm/d", "Potential evapotranspiration", "Module",DT_Raster);

	mdi.AddInput("D_SOMO_2D", "%", "soil moisture","Module", DT_Array2D);
	mdi.AddInput("D_Nitrate", "kg N/ha", "amount of nitrate", "Module", DT_Array2D);
	mdi.AddInput("D_SoluP", "kg P/ha", "amount of phosphorus in solution in layer ly", "Module", DT_Array2D);

	mdi.AddInput("LAI_Delta","","potential leaf area added on day","Module",DT_Raster);
	mdi.AddInput("LAI","","leaf area index for a given day","Module", DT_Raster);
	mdi.AddInput("LAI_Pre","","leaf area index for the previous day","Module", DT_Raster);

	mdi.AddInput("Fr_PHU","","fraction of phu accumulated on a given day ","Module",DT_Raster);
	mdi.AddInput("Fr_N","","the optimal fraction of nitrogen in the plant biomass ","Module",DT_Raster);
	mdi.AddInput("Fr_P","","the optimal fraction of phosphorus in the plant biomass ","Module",DT_Raster);
	mdi.AddInput("Fr_Root","","the fraction of total biomass partitioned to roots on a given day  ","Module",DT_Raster);

	mdi.AddInput("BIOMASS","kg/ha","total biomass","Module",DT_Raster);
	mdi.AddInput("BIOMASS_Delta","kg/ha","biomass increase on a given day","Module",DT_Raster);
	mdi.AddInput("BIOMASS_NOpt","kg/ha","potential biomass N","Module",DT_Raster);
	mdi.AddInput("BIOMASS_POpt","kg/ha","potential biomass P","Module",DT_Raster);
	
	
	// set the output variables
	mdi.AddOutput("RootDepth","kg/ha","N fixation",DT_Raster);
	mdi.AddOutput("Transpiration_Max","mm","maximum transpiration under ideal conditions",DT_Raster);

	mdi.AddOutput("BIOMASS_N","kg/ha","actual biomass N",DT_Raster);
	mdi.AddOutput("BIOMASS_P","kg/ha","actual biomass P",DT_Raster);
	mdi.AddOutput("BIOMASS_ROOT","kg/ha","biomass root",DT_Raster);
	mdi.AddOutput("BIOMASS_AG","kg/ha"," aboveground biomass on the day of harvest",DT_Raster);

	mdi.AddOutput("Layer_WaterUptake","mm","layer water uptake",DT_Array2D);	//for nutrient cycling
	mdi.AddOutput("Layer_NUptake","kg/ha","layer N uptake",DT_Array2D);
	mdi.AddOutput("Layer_PUptake","kg/ha","layer P uptake",DT_Array2D);

	mdi.AddOutput("N_Fixation","kg/ha","N fixation",DT_Raster);
	mdi.AddOutput("Total_WaterUptake","mm","total water uptake",DT_Raster); //for file
	mdi.AddOutput("Total_NUptake","kg/ha","total N uptake",DT_Raster);
	mdi.AddOutput("Total_PUptake","kg/ha","total P uptake",DT_Raster);
	


	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}