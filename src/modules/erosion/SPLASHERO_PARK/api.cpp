#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SplashEro_Park.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SplashEro_Park();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res;

	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Hui Wu");
	mdi.SetClass("Splash Erosion", "Calculate the amount soil detachment erosion.");
	mdi.SetDescription("use Park equation to calculate sediment yield of each cell");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("SplashEro_Park");
	mdi.SetName("SplashEro_Park");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("SplashEro_Park.chm");

	mdi.AddParameter("cellwidth","m","the width (length) of cell","mask.asc",DT_Single);
	mdi.AddParameter("DT_HS", "second", "Time step of the simulation", "file.in", DT_Single);
	mdi.AddParameter("Omega","","calibration coefficient of splash erosion","ParameterDB_Sediment",DT_Single);
	mdi.AddParameter("USLE_K","","the soil erodibility factor","ParameterDB_Sediment",DT_Raster);
	mdi.AddParameter("USLE_C","","the cover management factor","ParameterDB_Sediment",DT_Raster);
	//mdi.AddParameter("Hplant","m","the height of the plants","ParameterDB_Sediment",DT_Raster);
	//mdi.AddParameter("GRAD","","slope gradient (sine of slope angle)","ParameterDB_WaterBalance",DT_Raster);
	mdi.AddParameter("Slope", "%", "Slope", "ParameterDB_WaterBalance", DT_Raster); 
	//mdi.AddParameter("StoneFrac","","the fraction of stones on the surface, affects splash","ParameterDB_Sediment",DT_Raster);
	//mdi.AddParameter("GrassFrac","","the fraction of grasstrip in a cell","ParameterDB_Sediment",DT_Raster);
	//mdi.AddParameter("CoverFrac","","the fraction of vegetation cover","ParameterDB_Interception",DT_Raster);
	//mdi.AddParameter("RandRough","cm","The random roughness","ParameterDB_Sediment",DT_Raster);
	//mdi.AddParameter("CHWIDTH", "m", "channel width", "ParameterDB_Discharge", DT_Raster); 

	//input from other module	
	//WaterDepth = Depression + SurfaceRunoffDepth
	mdi.AddInput("D_DPST","mm","The depression storage","Module",DT_Raster);
	mdi.AddInput("D_SURU","mm","surface runoff","Module",DT_Raster);		
	mdi.AddInput("D_QOverland","m3/s","Water discharge in the downslope boundary of cells","Module",DT_Raster);	
	//mdi.AddInput("D_SNAC","mm","snow accumulation","Module",DT_Raster);	//SnowCover, from snow water balance module
	mdi.AddInput("D_NEPR","mm","The net precipitation obtained from the interception module","Module",DT_Raster);	//Rain, from interception module
	//mdi.AddInput("D_LeafDrain","m","leaf drainage from canopy","Module",DT_Raster);			//from Interception module

	// set the output variables
	mdi.AddOutput("DETSplash","kg", "distribution of splash detachment", DT_Raster);

	mdi.AddDependency("Interception","Interception module");      //for pNet, Leafdrain
	mdi.AddDependency("Overland routing","Overland routing module");          // cellh
	//mdi.AddDependency("Depression","Depression storage module");     // suru


	// write out the XML file.
	res = mdi.GetXMLDocument();

	//return res;

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}