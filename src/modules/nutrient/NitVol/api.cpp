#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "NitrificationAndAmmoniaVolatilization.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new NitrificationAndAmmoniaVolatilization();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Wang Lin");
	mdi.SetClass("NitrificationAndAmmoniaVolatilization", "Nitrification and Ammonia volatilization.");
	mdi.SetDescription("Nitrification and Ammonia volatilization.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("NitrificationAndAmmoniaVolatilization.chm");
	mdi.SetID("NitrificationAndAmmoniaVolatilization");
	mdi.SetName("NitrificationAndAmmoniaVolatilization");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("catEF", "", "volatilization cation exchange factor", "file.in", DT_Single); 

	//mdi.AddParameter("RootDepth","m","Root depth","ParameterDB_WaterBalance", DT_Raster);
	mdi.AddParameter("Wiltingpoint_2D","m3/m3","Plant wilting point moisture","ParameterDB_WaterBalance", DT_Array2D);
	mdi.AddParameter("FieldCap_2D", "%", "Field capacity","ParameterDB_WaterBalance", DT_Array2D);
	
	mdi.AddInput("D_SOTE","oC", "Soil Temperature","Module", DT_Raster);
	mdi.AddInput("D_SOMO_2D", "%", "Soil Moisture", "Module", DT_Array2D);
	mdi.AddInput("D_Depth","mm","depth of the layer","Module", DT_Array2D);
	mdi.AddInput("D_Nitrate", "kg N/ha", "amount of nitrate", "Module", DT_Array2D);
	mdi.AddInput("D_Ammon", "kg N/ha", "ammonium pool for soil nitrogen", "Module", DT_Array2D);
	
	mdi.AddOutput("MidZ", "mm", "depth from the soil surface to the middle of the layer",  DT_Array2D); 
	mdi.AddOutput("NitvolTF","", "nitrification/volatilization temperature factor", DT_Array2D);
	mdi.AddOutput("nitWF", "", "nitrification soil water factor", DT_Array2D); 
	mdi.AddOutput("VolDF", "", "volatilization depth factor", DT_Array2D); 
	mdi.AddOutput("NitR", "", "nitrification regulator", DT_Array2D);
	mdi.AddOutput("VolR", "", "volatilization regulator", DT_Array2D);
	mdi.AddOutput("NitvolAmmon", "kg N/ha", "amount of ammonium converted via nitrification and volatilization in layer ly", DT_Array2D);
	mdi.AddOutput("NitLostF", "", "estimated fraction of nitrogen lost by nitrification", DT_Array2D);
    mdi.AddOutput("VolLostF", "", "estimated fraction of nitrogen lost by volatilization", DT_Array2D);
	
	mdi.AddOutput("ConNitra", "kg N/ha", "amount of nitrogen converted from NH4 to NO3 in layer ly", DT_Array2D);
	mdi.AddOutput("ConAmm", "kg N/ha", "amount of nitrogen converted from NH4 to NH3 in layer ly", DT_Array2D);
	//??
	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}