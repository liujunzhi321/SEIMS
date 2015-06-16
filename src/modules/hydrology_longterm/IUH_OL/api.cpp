#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "IUH_OL.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new IUH_OL();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Wu Hui");
	mdi.SetClass("Overland flow", "Calculate overland flow routing.");
	mdi.SetDescription("IUH overland method to calculate overland flow routing.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("IUH_OL.chm");
	mdi.SetID("IUH_OL");
	mdi.SetName("IUH_OL");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");


	mdi.AddParameter("TimeStep","hr","time step of the simulation","file.in",DT_Single); 
	mdi.AddParameter("CellWidth","m","","file.in",DT_Single); 
	mdi.AddParameter("subbasin","","The subbasion grid","ParameterDB_Snow",DT_Raster); 
	mdi.AddParameter("Ol_iuh","","IUH of each grid cell","ParameterDB_Discharge",DT_Array2D);
	//mdi.AddParameter("uhminCell","","start time of IUH for each grid cell","ParameterDB_Discharge",DT_Array1D);
	//mdi.AddParameter("uhmaxCell","","end time of IUH for each grid cell","ParameterDB_Discharge",DT_Array1D);

	mdi.AddInput("D_SURU","mm","The depression storage","Module",DT_Raster);					//from depression module

	mdi.AddOutput("SBOF", "mm","Overland flow to streams for each subbasin", DT_Array1D);

	// set the dependencies
	mdi.AddDependency("DEP_FS","Depression Storage module");  

	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}