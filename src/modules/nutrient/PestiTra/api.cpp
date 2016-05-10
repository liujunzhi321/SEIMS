/*!
 * \file api.cpp
 * \brief Define MetadataInfo of NMINRL module.
/*!
 * \file api.cpp
 * \ingroup NMINRL
 * \author Huiran Gao
 * \date April 2016
 */


#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "PesticideTransfer.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new PesticideTransfer();
}

//! function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;
	mdi.SetAuthor("Huiran Gao");
	mdi.SetClass(MCLS_NMINRL, MCLSDESC_NMINRL);
	mdi.SetDescription(MDESC_NMINRL);
	mdi.SetEmail(SEIMS_EMAIL);
	mdi.SetID(MDESC_NMINRL);
	mdi.SetName(MDESC_NMINRL);
	mdi.SetVersion("1.0");
	mdi.SetWebsite(SEIMS_SITE);
	mdi.SetHelpfile("NMINRL.html");

	// set the parameters
	//Latitude is used to calculate max solar radiation. It is read in the similar format with elevation.
	
	// set the input
	mdi.AddInput(Tag_CellSize, UNIT_NON_DIM, DESC_CellSize, Source_ParameterDB, DT_Single);
	mdi.AddInput(Tag_CellWidth, UNIT_LEN_M, DESC_CellWidth, Source_ParameterDB, DT_Single);

	// set the output variables
	mdi.AddOutput(VAR_HMNTL, UNIT_CONT_KGKM2, DESC_HMNTL, DT_Single);
	mdi.AddOutput(VAR_HMPTL, UNIT_CONT_KGKM2, DESC_HMPTL, DT_Single);
	
	string res = mdi.GetXMLDocument();
	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}
