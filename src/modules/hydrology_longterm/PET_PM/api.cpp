#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "text.h"
#include "PETPenmanMonteith.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new PETPenmanMonteith();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Junzhi Liu");
	mdi.SetClass("Potential Evapotranspiration", "Calculate the potential evapotranspiration for an array of climate inputs.");
	mdi.SetDescription("Penman Monteith method for calculating the potential evapotranspiration.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("PET_PM");
	mdi.SetName("PET_PM");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("PET_PM.html");

	// set the parameters
	mdi.AddParameter("Co2","ppmv","CO2 Concentration","ParameterDB_Climate",DT_Single);
	mdi.AddParameter("Cond_rate","m/s/kPa","rate of decline in stomatal conductance per unit increase in vapor pressure deficit","ParameterDB_Climate",DT_Single);
	mdi.AddParameter("Cond_max","m/s","maximum stomatal conductance","ParameterDB_Climate",DT_Single);
	//This temperature is used to determine the value of variable m_snow
	//if T_MEAN is larger than T_snow, then m_snow = 0;
	//else m_snow = 1.
	mdi.AddParameter("T_snow","mm","Amount of water in snow","ParameterDB_Snow", DT_Single); 
	mdi.AddParameter("K_pet", "", "Correction Factor for PET", "ParameterDB", DT_Single);
	//Now the canopy height is assumed as constant for everyday and is consider as a parameter and read from asc file.
	//After the plant growth module is completed, this should be converted to input variable.
	//mdi.AddParameter("CHT","m","Canopy height for the day","file_height.asc", Array1D);	

	//The elevation of station is read from HydroClimateDB. It would be consider as a paramter. And its name must be Elevation. 
	//This will force the main program to read elevation from HydroClimateDB. See Line 1078 of main.cpp.
	mdi.AddParameter(Tag_Elevation_Meteorology,"m","Elevation","HydroClimateDB", DT_Array1D);

	//Latitude is used to calculate max solar radiation. It is read in the similiar format with elevation.
	mdi.AddParameter(Tag_Latitude_Meteorology,"degree","Latitude","HydroClimateDB", DT_Array1D);

	// set the input
	//These five inputs are read from HydroClimateDB
	mdi.AddInput("TMin","degree","Mean air temperature","Module", DT_Array1D); //use TMin and TMax to calculate TMean
	mdi.AddInput("TMax","degree","Mean air temperature","Module", DT_Array1D);
	mdi.AddInput("RM","","Relative humidity","Module", DT_Array1D);
	mdi.AddInput("SR","MJ/m2/d","Solar radiation","Module", DT_Array1D);
	mdi.AddInput("WS","m/s","Wind speed (measured at 10 meters above surface)","Module", DT_Array1D);

	//LAI needs to be calculated by other modules.
	//mdi.AddInput("LAIDAY","m2/m2","Leaf area index","Plant growth module", Array1D); //Wetspa Manual P17, need min LAI,max LAI and Julian day.

	//The albedo also needs plant growth module. 
	//Now, for testing, its value is determined by T_snow and T_MEAN.
	//mdi.AddInput("ALBDAY","","Albedo in the day","Plant growth module", Array1D);

	//Now, assume the grow code of all the stations is 1.
	//mdi.AddInput("IGRO","","Land cover status code","Management database", Array1D);

	// set the output variables
	mdi.AddOutput("T_PET","mm/d", "Potential evapotranspiration", DT_Array1D);
	//mdi.AddOutput("T_PET","mm/d", "Potential evapotranspiration", Array1D);

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}