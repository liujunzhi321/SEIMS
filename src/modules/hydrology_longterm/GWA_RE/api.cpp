#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "ReservoirMethod.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new ReservoirMethod();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Wu Hui");
	mdi.SetClass("Groundwater", "Calculate groundwater balance and baseflow.");
	mdi.SetDescription("Reservoir Method to calculate groundwater balance and baseflow.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("GWA_RE.chm");
	mdi.SetID("GWA_RE");
	mdi.SetName("GWA_RE");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("TimeStep","hr","time step","config.fig",DT_Single); 
	mdi.AddParameter("CellSize","m","cell size of the grid ","mask",DT_Single);
	mdi.AddParameter("CellWidth","m","cell size of the grid ","mask",DT_Single);

	mdi.AddParameter("GW0","mm","initial ground water storage ","ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("GWMAX","mm","maximum ground water storage","ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("df_coef","","Deep percolation coefficient","ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("Kg","","Baseflow recession coefficient","ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("Base_ex","","baseflow recession exponent","ParameterDB_WaterBalance", DT_Single);
	mdi.AddParameter("Rootdepth","mm","Root depth","ParameterDB_WaterBalance",DT_Raster);
	//mdi.AddParameter("subbasinSelected","","The subbasion ids listed in file.out","file.out",DT_Array1D);

	mdi.AddParameter("subbasin","","The subbasion grid","ParameterDB_Snow",DT_Raster);
	mdi.AddParameter("Slope","%","the average slope of the subbasin","ParameterDB_WaterBalance", DT_Raster);

	mdi.AddInput("D_Percolation_2D","mm","the amount of water percolated from the soil water reservoir","Module",DT_Array2D);
	mdi.AddInput("D_SOMO_2D", "mm", "Distribution of soil moisture", "Module", DT_Array2D);

	mdi.AddInput("D_INET","mm","evaporation from interception storage","Module",DT_Raster);
	mdi.AddInput("D_DEET","mm","evaporation from the depression storage","Module",DT_Raster);
	mdi.AddInput("D_SOET","mm","evaporation from the soil water storage","Module",DT_Raster);
	mdi.AddInput("D_PET","mm","PET from the interpolation module","Module",DT_Raster);
	
	mdi.AddInput("T_GWNEW","mm",
		"The volumn of water from the bank storage to the adjacent unsaturated zone and groundwater storage",
		"Module",DT_Array1D);//from IUH_CH or other channel routing module

	mdi.AddOutput("GWWB","","",DT_Array2D);					//used for output file
	mdi.AddOutput("Revap","","",DT_Raster);		//used by soil water balance module
	mdi.AddOutput("RG","","",DT_Array1D);					//used by soil water balance module
	mdi.AddOutput("SBQG","","",DT_Array1D);					//used by channel flow routing module
	mdi.AddOutput("SBPET","","",DT_Array1D);	
	mdi.AddOutput("SBGS","","",DT_Array1D);	

	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}