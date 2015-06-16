#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "GWaterReservoir.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new GWaterReservoir();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Junzhi Liu");
	mdi.SetClass("Groundwater", "linear reservoir method.");
	mdi.SetDescription("linear reservoir method.");
	mdi.SetEmail("liujz@lreis.ac.cn");
	mdi.SetHelpfile("GWATER_RESERVOIR.chm");
	mdi.SetID("GWATER_RESERVOIR");
	mdi.SetName("GWATER_RESERVOIR");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("subbasin","","The subbasion grid","ParameterDB_Snow",DT_Raster); 
	mdi.AddParameter("RchParam","","reach parameters", "reachparameters.txt",DT_Array2D);

	mdi.AddParameter("DT_HS", "second", "time step for storm simulation","ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("CellWidth", "m", "Cell width", "file.in", DT_Single); 
	mdi.AddParameter("kg", "", "baseflow recession coefficient", "ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("base_ex", "", "baseflow recession exponent", "ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("gw0", "mm", "Initial groundwater storage", "ParameterDB_WaterBalance", DT_Single);
	//mdi.AddParameter("gwmax", "mm", "", "ParameterDB_WaterBalance", DT_Single);

	mdi.AddInput("D_PERCOLATION","mm", "Distribution of groundwater recharge (percolation)", "Module", DT_Raster);

	mdi.AddOutput("SBQG", "m3/s", "ground water outflow at each subbasin",DT_Array1D);	
	mdi.AddOutput("SBGS", "mm", "ground water storage",DT_Array1D);	

	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}