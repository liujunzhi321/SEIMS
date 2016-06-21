#include <stdio.h>
#include <string>
#include "util.h"
#include "api.h"
#include "managementOperation_SWAT.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new MGTOpt_SWAT();
}

/// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;
	string res;

	mdi.SetAuthor("Liang-Jun Zhu");
	mdi.SetClass(MCLS_MGT, MCLSDESC_MGT);
	mdi.SetDescription(MDESC_MGT_SWAT);
	mdi.SetID(MID_MGT_SWAT);
	mdi.SetName(MID_MGT_SWAT);
	mdi.SetVersion("1.0");
	mdi.SetEmail(SEIMS_EMAIL);
	mdi.SetWebsite(SEIMS_SITE);
	mdi.SetHelpfile("");
	/// set parameters from database
	mdi.AddParameter(VAR_DAYLEN_MIN, UNIT_TIMESTEP_HOUR, DESC_DAYLEN_MIN,Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_PHUTOT, UNIT_TIMESTEP_HOUR, DESC_PHUTOT,Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_IGRO, UNIT_NON_DIM, DESC_IGRO, Source_ParameterDB, DT_Raster1D);
	/// set input from other modules
	mdi.AddInput(VAR_DAYLEN, UNIT_TIMESTEP_HOUR, DESC_DAYLEN, Source_Module, DT_Raster1D); /// PET modules
	mdi.AddInput(VAR_PHUBASE, UNIT_TIMESTEP_HOUR, DESC_PHUBASE, Source_Module, DT_Raster1D); /// PET modules
	mdi.AddInput(VAR_FR_PHU_ACC, UNIT_HEAT_UNIT, DESC_FR_PHU_ACC,Source_Module,DT_Raster1D);

	/// set the output variables
	mdi.AddOutput("VAR_NAME","UNIT", "DESC", "DT");
	/// write out the XML file.
	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}