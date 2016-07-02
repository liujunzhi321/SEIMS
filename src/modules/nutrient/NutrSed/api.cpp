/*!
 * \file api.cpp
 * \brief Define MetadataInfo of SurTra module.
/*!
 * \file api.cpp
 * \ingroup SurTra
 * \author Huiran Gao
 * \date May 2016
 */


#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SurrunoffTransfer.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SurrunoffTransfer();
}

//! function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;
	mdi.SetAuthor("Huiran Gao");
	mdi.SetClass(MCLS_SurTra, MCLSDESC_SurTra);
	mdi.SetDescription(MDESC_SurTra);
	mdi.SetEmail(SEIMS_EMAIL);
	mdi.SetID(MDESC_SurTra);
	mdi.SetName(MDESC_SurTra);
	mdi.SetVersion("1.0");
	mdi.SetWebsite(SEIMS_SITE);
	mdi.SetHelpfile("SurTra.html");

	// set the parameters
	mdi.AddParameter(Tag_CellSize, UNIT_NON_DIM, DESC_CellSize, Source_ParameterDB, DT_Single);
	mdi.AddParameter(Tag_CellWidth, UNIT_LEN_M, DESC_CellWidth, Source_ParameterDB, DT_Single);

	mdi.AddParameter(VAR_SOL_AORGN, UNIT_CONT_KGHA, DESC_SOL_AORGN, Source_ParameterDB, DT_Raster2D);
	mdi.AddParameter(VAR_SOL_FON, UNIT_CONT_KGHA, DESC_SOL_FON, Source_ParameterDB, DT_Raster2D);
	mdi.AddParameter(VAR_SOL_FOP, UNIT_CONT_KGHA, DESC_SOL_FOP, Source_ParameterDB, DT_Raster2D);
	mdi.AddParameter(VAR_SOL_ORGN, UNIT_CONT_KGHA, DESC_SOL_ORGN, Source_ParameterDB, DT_Raster2D);
	mdi.AddParameter(VAR_SOL_ORGP, UNIT_CONT_KGHA, DESC_SOL_ORGP, Source_ParameterDB, DT_Raster2D);
	mdi.AddParameter(VAR_ROOTDEPTH, UNIT_CONT_KGHA, DESC_ROOTDEPTH, Source_ParameterDB, DT_Raster2D);
	mdi.AddParameter(VAR_SOL_ACTP, UNIT_CONT_KGHA, DESC_SOL_ACTP, Source_ParameterDB, DT_Raster2D);
	mdi.AddParameter(VAR_SOL_STAP, UNIT_CONT_KGHA, DESC_SOL_STAP, Source_ParameterDB, DT_Raster2D);
	mdi.AddParameter(VAR_SOL_MP, UNIT_CONT_KGHA, DESC_SOL_MP, Source_ParameterDB, DT_Raster2D);
	mdi.AddParameter(VAR_SOL_BD, UNIT_DENSITY, DESC_SOL_BD, Source_ParameterDB, DT_Raster2D);
	
	// set the input
	mdi.AddInput(VAR_SOER, UNIT_TONS, DESC_SOER, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_SURU, UNIT_DEPTH_MM, DESC_SURU, Source_Module, DT_Raster1D);

	// set the output variables
	mdi.AddOutput(VAR_SEDORGN, UNIT_CONT_KGHA, DESC_SEDORGN, DT_Raster1D);
	mdi.AddOutput(VAR_SEDORGP, UNIT_CONT_KGHA, DESC_SEDORGP, DT_Raster1D);
	mdi.AddOutput(VAR_SEDMINPA, UNIT_CONT_KGHA, DESC_SEDMINPA, DT_Raster1D);
	mdi.AddOutput(VAR_SEDMINPS, UNIT_CONT_KGHA, DESC_SEDMINPS, DT_Raster1D);

	mdi.AddOutput(VAR_SOL_AORGN, UNIT_CONT_KGHA, DESC_SOL_AORGN, DT_Raster2D);
	mdi.AddOutput(VAR_SOL_FON, UNIT_CONT_KGHA, DESC_SOL_FON, DT_Raster2D);
	mdi.AddOutput(VAR_SOL_FOP, UNIT_CONT_KGHA, DESC_SOL_FOP, DT_Raster2D);
	mdi.AddOutput(VAR_SOL_ORGN, UNIT_CONT_KGHA, DESC_SOL_ORGN, DT_Raster2D);
	mdi.AddOutput(VAR_SOL_ORGP, UNIT_CONT_KGHA, DESC_SOL_ORGP, DT_Raster2D);
	mdi.AddOutput(VAR_SOL_ACTP, UNIT_CONT_KGHA, DESC_SOL_ACTP, DT_Raster2D);
	mdi.AddOutput(VAR_SOL_STAP, UNIT_CONT_KGHA, DESC_SOL_STAP, DT_Raster2D);

	string res = mdi.GetXMLDocument();
	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}
