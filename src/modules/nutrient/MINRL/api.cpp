#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "NandPmi.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule *GetInstance()
{
    return new NandPim();
}

//! function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char *MetadataInformation()
{
    MetadataInfo mdi;
    mdi.SetAuthor("Huiran Gao");
    mdi.SetClass(MCLS_MINRL, MCLSDESC_MINRL);
    mdi.SetDescription(MDESC_MINRL);
    mdi.SetEmail(SEIMS_EMAIL);
    mdi.SetID(MDESC_MINRL);
    mdi.SetName(MDESC_MINRL);
    mdi.SetVersion("1.0");
    mdi.SetWebsite(SEIMS_SITE);
    mdi.SetHelpfile("MINRL.html");

    mdi.AddParameter(Tag_CellWidth, UNIT_LEN_M, DESC_CellWidth, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_CMN, UNIT_NON_DIM, DESC_CMN, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_CDN, UNIT_NON_DIM, DESC_CDN, Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_NACTFR, UNIT_NON_DIM, DESC_NACTFR, Source_ParameterDB, DT_Single);
	mdi.AddParameter(VAR_SDNCO, UNIT_NON_DIM, DESC_SDNCO, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_PSP, UNIT_NON_DIM, DESC_PSP, Source_ParameterDB, DT_Single);

	/// idplt in SWAT is a lookup array. in SEIMS, use landcover
    mdi.AddParameter(VAR_LCC, UNIT_NON_DIM, DESC_LCC, Source_ParameterDB, DT_Raster1D);  
    mdi.AddParameter(VAR_PL_RSDCO, UNIT_NON_DIM, DESC_PL_RSDCO, Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_SOILLAYERS, UNIT_NON_DIM, DESC_SOILLAYERS, Source_ParameterDB, DT_Raster1D);
    mdi.AddParameter(VAR_SOILDEPTH, UNIT_DEPTH_MM, DESC_SOILDEPTH, Source_ParameterDB, DT_Raster2D);
	mdi.AddParameter(VAR_SOILTHICK, UNIT_DEPTH_MM, DESC_SOILTHICK, Source_ParameterDB, DT_Raster2D);
	mdi.AddParameter(VAR_SOL_RSDIN, UNIT_CONT_KGHA, DESC_SOL_RSDIN, Source_ParameterDB, DT_Raster1D);

	mdi.AddParameter(VAR_SOL_CBN, UNIT_PERCENT, DESC_SOL_CBN, Source_ParameterDB, DT_Raster2D);
	mdi.AddParameter(VAR_CLAY, UNIT_PERCENT, DESC_CLAY, Source_ParameterDB, DT_Raster2D);
	mdi.AddParameter(VAR_SOL_BD, UNIT_DENSITY, DESC_SOL_BD, Source_ParameterDB, DT_Raster2D);
    mdi.AddParameter(VAR_SOL_AWC, UNIT_DEPTH_MM, DESC_SOL_AWC, Source_ParameterDB, DT_Raster2D);
	mdi.AddParameter(VAR_SOL_WPMM, UNIT_DEPTH_MM, DESC_SOL_WPMM, Source_ParameterDB, DT_Raster2D);
    mdi.AddParameter(VAR_SOL_NO3, UNIT_CONT_KGHA, DESC_SOL_NO3, Source_ParameterDB, DT_Raster2D);
    mdi.AddParameter(VAR_SOL_NH3, UNIT_CONT_KGHA, DESC_SOL_NH3, Source_ParameterDB, DT_Raster2D);
    mdi.AddParameter(VAR_SOL_SORGN, UNIT_CONT_KGHA, DESC_SOL_SORGN, Source_ParameterDB, DT_Raster2D);
    mdi.AddParameter(VAR_SOL_HORGP, UNIT_CONT_KGHA, DESC_SOL_HORGP, Source_ParameterDB, DT_Raster2D);
    mdi.AddParameter(VAR_SOL_SOLP, UNIT_CONT_KGHA, DESC_SOL_SOLP, Source_ParameterDB, DT_Raster2D);
	//mdi.AddParameter(VAR_SOL_RSD, UNIT_CONT_KGHA, DESC_SOL_RSD, Source_ParameterDB, DT_Raster2D);

	// from other modules
	mdi.AddInput(VAR_SOTE, UNIT_TEMP_DEG, DESC_SOTE, Source_Module, DT_Raster1D);
    mdi.AddInput(VAR_SOL_ST, UNIT_DEPTH_MM, DESC_SOL_ST, Source_Module, DT_Raster2D);

	mdi.AddOutput(VAR_SOL_COV, UNIT_CONT_KGHA, DESC_SOL_COV, DT_Raster1D);
	mdi.AddOutput(VAR_SOL_RSD, UNIT_CONT_KGHA, DESC_SOL_RSD, DT_Raster2D);
    // set the output variables
	/// watershed statistics, total 18
    mdi.AddOutput(VAR_HMNTL, UNIT_CONT_KGHA, DESC_HMNTL, DT_Raster1D);
    mdi.AddOutput(VAR_HMPTL, UNIT_CONT_KGHA, DESC_HMPTL, DT_Raster1D);
    mdi.AddOutput(VAR_RMN2TL, UNIT_CONT_KGHA, DESC_RMN2TL, DT_Raster1D);
    mdi.AddOutput(VAR_RMPTL, UNIT_CONT_KGHA, DESC_RMPTL, DT_Raster1D);
    mdi.AddOutput(VAR_RWNTL, UNIT_CONT_KGHA, DESC_RWNTL, DT_Raster1D);
    mdi.AddOutput(VAR_WDNTL, UNIT_CONT_KGHA, DESC_WDNTL, DT_Raster1D);
    mdi.AddOutput(VAR_RMP1TL, UNIT_CONT_KGHA, DESC_RMP1TL, DT_Raster1D);
    mdi.AddOutput(VAR_ROCTL, UNIT_CONT_KGHA, DESC_ROCTL, DT_Raster1D);
    mdi.AddOutput(VAR_WSHD_DNIT, UNIT_CONT_KGHA, DESC_WSHD_DNIT, DT_Single);
    mdi.AddOutput(VAR_WSHD_HMN, UNIT_CONT_KGHA, DESC_WSHD_HMN, DT_Single);
    mdi.AddOutput(VAR_WSHD_HMP, UNIT_CONT_KGHA, DESC_WSHD_HMP, DT_Single);
    mdi.AddOutput(VAR_WSHD_RMN, UNIT_CONT_KGHA, DESC_WSHD_RMN, DT_Single);
    mdi.AddOutput(VAR_WSHD_RMP, UNIT_CONT_KGHA, DESC_WSHD_RMP, DT_Single);
    mdi.AddOutput(VAR_WSHD_RWN, UNIT_CONT_KGHA, DESC_WSHD_RWN, DT_Single);
    mdi.AddOutput(VAR_WSHD_NITN, UNIT_CONT_KGHA, DESC_WSHD_NITN, DT_Single);
    mdi.AddOutput(VAR_WSHD_VOLN, UNIT_CONT_KGHA, DESC_WSHD_VOLN, DT_Single);
    mdi.AddOutput(VAR_WSHD_PAL, UNIT_CONT_KGHA, DESC_WSHD_PAL, DT_Single);
    mdi.AddOutput(VAR_WSHD_PAS, UNIT_CONT_KGHA, DESC_WSHD_PAS, DT_Single);



    mdi.AddOutput(VAR_SOL_AORGN, UNIT_CONT_KGHA, DESC_SOL_AORGN, DT_Raster2D);
    mdi.AddOutput(VAR_SOL_FON, UNIT_CONT_KGHA, DESC_SOL_FON, DT_Raster2D);
    mdi.AddOutput(VAR_SOL_NO3, UNIT_CONT_KGHA, DESC_SOL_NO3, DT_Raster2D);
    mdi.AddOutput(VAR_SOL_NH3, UNIT_CONT_KGHA, DESC_SOL_NH3, DT_Raster2D);
    mdi.AddOutput(VAR_SOL_SORGN, UNIT_CONT_KGHA, DESC_SOL_SORGN, DT_Raster2D);

	mdi.AddOutput(VAR_SOL_FOP, UNIT_CONT_KGHA, DESC_SOL_FOP, DT_Raster2D);
    mdi.AddOutput(VAR_SOL_HORGP, UNIT_CONT_KGHA, DESC_SOL_HORGP, DT_Raster2D);
    mdi.AddOutput(VAR_SOL_SOLP, UNIT_CONT_KGHA, DESC_SOL_SOLP, DT_Raster2D);
    mdi.AddOutput(VAR_SOL_ACTP, UNIT_CONT_KGHA, DESC_SOL_ACTP, DT_Raster2D);
	mdi.AddOutput(VAR_SOL_STAP, UNIT_CONT_KGHA, DESC_SOL_STAP, DT_Raster2D);

	//mdi.AddOutput(VAR_SOL_WPMM, UNIT_DEPTH_MM, DESC_SOL_WPMM, DT_Raster2D);
	//mdi.AddOutput(VAR_ROOTDEPTH, UNIT_DEPTH_MM, DESC_ROOTDEPTH, DT_Raster2D);

    string res = mdi.GetXMLDocument();
    char *tmp = new char[res.size() + 1];
    strprintf(tmp, res.size() + 1, "%s", res.c_str());
    return tmp;
}
