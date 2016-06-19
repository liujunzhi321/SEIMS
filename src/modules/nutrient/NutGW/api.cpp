/*!
 * \file api.cpp
 * \brief Define MetadataInfo of NutGW module.
/*!
 * \file api.cpp
 * \ingroup NutGW
 * \author Huiran Gao
 * \date Jun 2016
 */


#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "NutrientinGroundwater.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new NutrientinGroundwater();
}

//! function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;
	mdi.SetAuthor("Huiran Gao");
	mdi.SetClass(MCLS_NutGW, MCLSDESC_NutGW);
	mdi.SetDescription(MDESC_NutGW);
	mdi.SetEmail(SEIMS_EMAIL);
	mdi.SetID(MDESC_NutGW);
	mdi.SetName(MDESC_NutGW);
	mdi.SetVersion("1.0");
	mdi.SetWebsite(SEIMS_SITE);
	mdi.SetHelpfile("NutGW.html");

	// set the parameters
	mdi.AddParameter(Tag_CellSize, UNIT_NON_DIM, DESC_CellSize, Source_ParameterDB, DT_Single);
	mdi.AddParameter(Tag_CellWidth, UNIT_LEN_M, DESC_CellWidth, Source_ParameterDB, DT_Single);

	mdi.AddParameter(VAR_GWNO3, UNIT_KGM3, DESC_GWNO3, Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_GWMINP, UNIT_KGM3, DESC_GWMINP, Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_GW_Q, UNIT_DEPTH_MM, DESC_GW_Q, Source_ParameterDB, DT_Raster1D);

	// set the output variables
	mdi.AddOutput(VAR_NO3GW, UNIT_CONT_KGKM2, DESC_NO3GW, DT_Raster1D);
	mdi.AddOutput(VAR_MINPGW, UNIT_CONT_KGKM2, DESC_MINPGW, DT_Raster1D);

	string res = mdi.GetXMLDocument();
	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}
