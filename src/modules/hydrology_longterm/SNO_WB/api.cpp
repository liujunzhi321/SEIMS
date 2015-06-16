#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SNO_WB.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SNO_WB();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Chunping Ou");
	mdi.SetClass("Snowmelt", "Calculate the amount of snow melt.");
	mdi.SetDescription("For simplicity purpose for the time being, the algorithm used in the original WetSpa is incorporated in the SEIMS.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("SNO_WB");
	mdi.SetName("SNO_WB");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("SNO_WB.chm");

	mdi.AddParameter("cellSize","","the number of invalid cells", "mask.asc",DT_Single);
	mdi.AddParameter("K_blow","-"," a fraction coefficient of snow blowing into or out of the watershed","ParameterDB_Snow",DT_Single);
	mdi.AddParameter("T0","oC","the snowmelt threshold temperature ","ParameterDB_Snow",DT_Single);	
	mdi.AddParameter("T_snow","oC","snowfall temperature","ParameterDB_Snow",DT_Single);	
	mdi.AddParameter("swe0","mm","Initial snow water equivalent","ParameterDB_Snow",DT_Single);
	mdi.AddParameter("subbasinSelected","","The subbasion ids listed in file.out","file.out",DT_Array1D); //this list is used to contrain the output size. Its name must be subbasinSelected.
	mdi.AddParameter("subbasin","","The subbasion grid","ParameterDB_Snow",DT_Raster); //this list is used to contrain the output size

	mdi.AddInput("D_NEPR","mm","net precipitation","Module",DT_Raster);
	mdi.AddInput("D_SNRD","mm","snow redistribution","Module",DT_Raster);
	mdi.AddInput("D_SNSB","mm","snow sublimation","Module",DT_Raster);
	mdi.AddInput("D_SNME","mm","snow melt","Module",DT_Raster);
	mdi.AddInput("D_TMIN","oC","min temperature","Module",DT_Raster);
	mdi.AddInput("D_TMAX","oC","max temperature","Module",DT_Raster);
	mdi.AddInput("D_P","mm","precipitation","Module",DT_Raster);
	mdi.AddInput("T_WS","m/s","wind speed","Module",DT_Array1D);

	// set the output variables
	mdi.AddOutput("SNAC","mm", "distribution of snow accumulation", DT_Raster);
	mdi.AddOutput("SNWB","mm", "snow water balance for selected subbasins", DT_Array2D);
	mdi.AddOutput("SWE","mm", "average SA of the watershed", DT_Single);

	// set the dependencies
	mdi.AddDependency("T", "average temperature obtained from the interpolation module");
	mdi.AddDependency("P", "average precipitation obtained from the interpolation module"); 
	mdi.AddDependency("Pnet", "net precipitation obtained from interception model used to calculate P in equation ");
	mdi.AddDependency("SA", "snow accumulation");
	mdi.AddDependency("SE", "snow sublimation");
	mdi.AddDependency("SR", "snow redistribution");
	mdi.AddDependency("SM", "snow melt");

	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}