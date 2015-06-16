#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "IUH_IF.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new IUH_IF();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Wu Hui");
	mdi.SetClass("Interflow", "Calculate interflow routing.");
	mdi.SetDescription("IUH overland method to calculate interflow routing.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("IUH_IF.chm");
	mdi.SetID("IUH_IF");
	mdi.SetName("IUH_IF");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("TimeStep","hr","time step of the simulation","file.in",DT_Single); 
	mdi.AddParameter("CellWidth","m","","file.in",DT_Single); 
	mdi.AddParameter("Ol_iuh","","IUH of each grid cell","ParameterDB_Discharge",DT_Array2D);
	mdi.AddParameter("subbasin","","The subbasion grid","ParameterDB_Snow",DT_Raster);
	//mdi.AddParameter("uhminCell","","start time of IUH for each grid cell","ParameterDB_Discharge",DT_Array1D);
	//mdi.AddParameter("uhmaxCell","","end time of IUH for each grid cell","ParameterDB_Discharge",DT_Array1D);

	mdi.AddInput("D_SSRU","mm","The subsurface runoff","Module",DT_Raster);					

	mdi.AddOutput("SBIF", "mm","Interflow to streams for each subbasin", DT_Array1D);

	// set the dependencies
	mdi.AddDependency("SSR_DA","Subsurface Runoff module");   

	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}