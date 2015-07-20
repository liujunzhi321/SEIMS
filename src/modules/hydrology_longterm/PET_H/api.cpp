#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "text.h"
#include "PETHargreaves.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new PETHargreaves();
}

/// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Junzhi Liu");
	mdi.SetClass("Potential Evapotranspiration", "Calculate the potential evapotranspiration for an array of climate inputs.");
	mdi.SetDescription("Hargreaves method for calculating the potential evapotranspiration.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("PET_H");
	mdi.SetName("PET_H");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("PET_H.html");

	// set the parameters (non-time series)
	//mdi.AddInput("T_MEAN","degree","Mean Air Temperature","interpolation module", Array1D);
	mdi.AddParameter("K_pet", "", "Correction Factor for PET", "ParameterDB", DT_Single);
	mdi.AddInput("Tmax","degree","Maximum Air Temperature","Module", DT_Array1D);
	mdi.AddInput("Tmin","degree","Minimum Air Temperature","Module", DT_Array1D);
	//mdi.AddInput("SR_MAX","MJ/m2/d","Maximum Solar Radiation","climate generation module", Array1D);
	mdi.AddParameter(Tag_Latitude_Meteorology,"degree","Latitude","HydroClimateDB", DT_Array1D);

	// set the output variables
	mdi.AddOutput("T_PET","mm/d", "Potential Evapotranspiration", DT_Array1D);

	// set the dependencies
	mdi.AddDependency("CLIMATE_GEN", "");

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}