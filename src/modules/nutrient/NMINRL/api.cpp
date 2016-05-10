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
#include "NandPmi.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new NandPim();
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
	mdi.AddParameter(VAR_CMN, UNIT_NON_DIM, DESC_CMN,Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_NACTFR, UNIT_NON_DIM, DESC_NACTFR, Source_ParameterDB, DT_Single);
	
	// set the input
	mdi.AddInput(Tag_CellSize, UNIT_NON_DIM, DESC_CellSize, Source_ParameterDB, DT_Single);
	mdi.AddInput(Tag_CellWidth, UNIT_LEN_M, DESC_CellWidth, Source_ParameterDB, DT_Single);

	mdi.AddInput(VAR_WSHD_DNIT, UNIT_CONT_KGKM2, DESC_WSHD_DNIT, Source_ParameterDB, DT_Single);
	mdi.AddInput(VAR_WSHD_HMN, UNIT_CONT_KGKM2, DESC_WSHD_HMN, Source_ParameterDB, DT_Single);
	mdi.AddInput(VAR_WSHD_HMP, UNIT_CONT_KGKM2, DESC_WSHD_HMP, Source_ParameterDB, DT_Single);
	mdi.AddInput(VAR_WSHD_RMN, UNIT_CONT_KGKM2, DESC_WSHD_RMN, Source_ParameterDB, DT_Single);
	mdi.AddInput(VAR_WSHD_RMP, UNIT_CONT_KGKM2, DESC_WSHD_RMP, Source_ParameterDB, DT_Single);
	mdi.AddInput(VAR_WSHD_RWN, UNIT_CONT_KGKM2, DESC_WSHD_RWN, Source_ParameterDB, DT_Single);
	mdi.AddInput(VAR_WSHD_NITN, UNIT_CONT_KGKM2, DESC_WSHD_NITN, Source_ParameterDB, DT_Single);
	mdi.AddInput(VAR_WSHD_VOLN, UNIT_CONT_KGKM2, DESC_WSHD_VOLN, Source_ParameterDB, DT_Single);
	mdi.AddInput(VAR_WSHD_PAL, UNIT_CONT_KGKM2, DESC_WSHD_PAL, Source_ParameterDB, DT_Single);
	mdi.AddInput(VAR_WSHD_PAS, UNIT_CONT_KGKM2, DESC_WSHD_PAS, Source_ParameterDB, DT_Single);

	mdi.AddInput(VAR_CDN, UNIT_NON_DIM, DESC_CDN, Source_ParameterDB, DT_Array1D);
	mdi.AddInput(VAR_LCC, UNIT_NON_DIM, DESC_LCC, Source_ParameterDB, DT_Array1D);
	mdi.AddInput(VAR_PL_RSDCO, UNIT_NON_DIM, DESC_PL_RSDCO, Source_ParameterDB, DT_Array1D);
	mdi.AddInput(VAR_PSP, UNIT_NON_DIM, DESC_PSP, Source_ParameterDB, DT_Array1D);

	mdi.AddInput(VAR_SOL_CBN, UNIT_PERCENT, DESC_SOL_CBN, Source_ParameterDB, DT_Array2D);
	mdi.AddInput(VAR_SOL_WST, UNIT_DEPTH_MM, DESC_SOL_WST, Source_ParameterDB, DT_Array2D);
	mdi.AddInput(VAR_SOL_WFC, UNIT_DEPTH_MM, DESC_SOL_WFC, Source_ParameterDB, DT_Array2D);
	mdi.AddInput(VAR_SOL_TMP, UNIT_TEMP_DEG, DESC_SOL_TMP, Source_ParameterDB, DT_Array2D);
	mdi.AddInput(VAR_SOL_WH, UNIT_DEPTH_MM, DESC_SOL_WH, Source_ParameterDB, DT_Array2D);
	mdi.AddInput(VAR_SOL_AORGN, UNIT_CONT_KGKM2, DESC_SOL_AORGN, Source_ParameterDB, DT_Array2D);
	mdi.AddInput(VAR_SOL_FON, UNIT_CONT_KGKM2, DESC_SOL_FON, Source_ParameterDB, DT_Array2D);
	mdi.AddInput(VAR_SOL_FOP, UNIT_CONT_KGKM2, DESC_SOL_FOP, Source_ParameterDB, DT_Array2D);
	mdi.AddInput(VAR_SOL_NO3, UNIT_CONT_KGKM2, DESC_SOL_NO3, Source_ParameterDB, DT_Array2D);
	mdi.AddInput(VAR_SOL_ORGN, UNIT_CONT_KGKM2, DESC_SOL_ORGN, Source_ParameterDB, DT_Array2D);
	mdi.AddInput(VAR_SOL_ORGP, UNIT_CONT_KGKM2, DESC_SOL_ORGP, Source_ParameterDB, DT_Array2D);
	mdi.AddInput(VAR_SOL_RSD, UNIT_CONT_KGKM2, DESC_SOL_RSD, Source_ParameterDB, DT_Array2D);
	mdi.AddInput(VAR_SOL_SOLP, UNIT_CONT_KGKM2, DESC_SOL_SOLP, Source_ParameterDB, DT_Array2D);
	mdi.AddInput(VAR_SOL_NH3, UNIT_CONT_KGKM2, DESC_SOL_NH3, Source_ParameterDB, DT_Array2D);
	mdi.AddInput(VAR_SOL_WPMM, UNIT_CONT_KGKM2, DESC_SOL_WPMM, Source_ParameterDB, DT_Array2D);
	mdi.AddInput(VAR_ROOTDEPTH, UNIT_CONT_KGKM2, DESC_ROOTDEPTH, Source_ParameterDB, DT_Array2D);
	mdi.AddInput(VAR_SOL_ACTP, UNIT_CONT_KGKM2, DESC_SOL_ACTP, Source_ParameterDB, DT_Array2D);
	mdi.AddInput(VAR_SOL_STAP, UNIT_CONT_KGKM2, DESC_SOL_STAP, Source_ParameterDB, DT_Array2D);

	// set the output variables
	mdi.AddOutput(VAR_HMNTL, UNIT_CONT_KGKM2, DESC_HMNTL, DT_Single);
	mdi.AddOutput(VAR_HMPTL, UNIT_CONT_KGKM2, DESC_HMPTL, DT_Single);
	mdi.AddOutput(VAR_RMN2TL, UNIT_CONT_KGKM2, DESC_RMN2TL, DT_Single);
	mdi.AddOutput(VAR_RMPTL, UNIT_CONT_KGKM2, DESC_RMPTL, DT_Single);
	mdi.AddOutput(VAR_RWNTL, UNIT_CONT_KGKM2, DESC_RWNTL, DT_Single);
	mdi.AddOutput(VAR_WDNTL, UNIT_CONT_KGKM2, DESC_WDNTL, DT_Single);
	mdi.AddOutput(VAR_RMP1TL, UNIT_CONT_KGKM2, DESC_RMP1TL, DT_Single);
	mdi.AddOutput(VAR_ROCTL, UNIT_CONT_KGKM2, DESC_ROCTL, DT_Single);
	mdi.AddOutput(VAR_WSHD_DNIT, UNIT_CONT_KGKM2, DESC_WSHD_DNIT, DT_Single);
	mdi.AddOutput(VAR_WSHD_HMN, UNIT_CONT_KGKM2, DESC_WSHD_HMN, DT_Single);
	mdi.AddOutput(VAR_WSHD_HMP, UNIT_CONT_KGKM2, DESC_WSHD_HMP, DT_Single);
	mdi.AddOutput(VAR_WSHD_RMN, UNIT_CONT_KGKM2, DESC_WSHD_RMN, DT_Single);
	mdi.AddOutput(VAR_WSHD_RMP, UNIT_CONT_KGKM2, DESC_WSHD_RMP, DT_Single);
	mdi.AddOutput(VAR_WSHD_RWN, UNIT_CONT_KGKM2, DESC_WSHD_RWN, DT_Single);
	mdi.AddOutput(VAR_WSHD_NITN, UNIT_CONT_KGKM2, DESC_WSHD_NITN, DT_Single);
	mdi.AddOutput(VAR_WSHD_VOLN, UNIT_CONT_KGKM2, DESC_WSHD_VOLN, DT_Single);
	mdi.AddOutput(VAR_WSHD_PAL, UNIT_CONT_KGKM2, DESC_WSHD_PAL, DT_Single);
	mdi.AddOutput(VAR_WSHD_PAS, UNIT_CONT_KGKM2, DESC_WSHD_PAS, DT_Single);

	mdi.AddOutput(VAR_SOL_AORGN, UNIT_CONT_KGKM2, DESC_SOL_AORGN, DT_Array2D);
	mdi.AddOutput(VAR_SOL_FON, UNIT_CONT_KGKM2, DESC_SOL_FON, DT_Array2D);
	mdi.AddOutput(VAR_SOL_FOP, UNIT_CONT_KGKM2, DESC_SOL_FOP, DT_Array2D);
	mdi.AddOutput(VAR_SOL_NO3, UNIT_CONT_KGKM2, DESC_SOL_NO3, DT_Array2D);
	mdi.AddOutput(VAR_SOL_ORGN, UNIT_CONT_KGKM2, DESC_SOL_ORGN, DT_Array2D);
	mdi.AddOutput(VAR_SOL_ORGP, UNIT_CONT_KGKM2, DESC_SOL_ORGP, DT_Array2D);
	mdi.AddOutput(VAR_SOL_RSD, UNIT_CONT_KGKM2, DESC_SOL_RSD, DT_Array2D);
	mdi.AddOutput(VAR_SOL_SOLP, UNIT_CONT_KGKM2, DESC_SOL_SOLP, DT_Array2D);
	mdi.AddOutput(VAR_SOL_NH3, UNIT_CONT_KGKM2, DESC_SOL_NH3, DT_Array2D);
	mdi.AddOutput(VAR_SOL_WPMM, UNIT_CONT_KGKM2, DESC_SOL_WPMM, DT_Array2D);
	mdi.AddOutput(VAR_ROOTDEPTH, UNIT_CONT_KGKM2, DESC_ROOTDEPTH, DT_Array2D);
	mdi.AddOutput(VAR_SOL_ACTP, UNIT_CONT_KGKM2, DESC_SOL_ACTP, DT_Array2D);
	mdi.AddOutput(VAR_SOL_STAP, UNIT_CONT_KGKM2, DESC_SOL_STAP, DT_Array2D);

	string res = mdi.GetXMLDocument();
	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}
