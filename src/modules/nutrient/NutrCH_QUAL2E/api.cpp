/*!
 * \file api.cpp
 * \brief Define MetadataInfo of NutCHRout module.
/*!
 * \file api.cpp
 * \ingroup NutrCH_QUAL2E
 * \author Huiran Gao
 * \date Jun 2016
 */


#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "NutrCH_QUAL2E.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule *GetInstance()
{
    return new NutrCH_QUAL2E();
}

//! function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char *MetadataInformation()
{
    MetadataInfo mdi;
    mdi.SetAuthor("Huiran Gao");
    mdi.SetClass(MCLS_NutCHRout, MCLSDESC_NutCHRout);
    mdi.SetDescription(MDESC_NutCHRout);
    mdi.SetEmail(SEIMS_EMAIL);
    mdi.SetID(MDESC_NutCHRout);
    mdi.SetName(MDESC_NutCHRout);
    mdi.SetVersion("1.0");
    mdi.SetWebsite(SEIMS_SITE);
    mdi.SetHelpfile("NutrCH_QUAL2E.html");

    // set the parameters
	mdi.AddParameter(Tag_ChannelTimeStep, UNIT_TIMESTEP_SEC, DESC_TIMESTEP, File_Input, DT_Single);
    mdi.AddParameter(VAR_RNUM1, UNIT_NON_DIM, DESC_RNUM1, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_IGROPT, UNIT_NON_DIM, DESC_IGROPT, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_AI0, UNIT_NUT_RATIO, DESC_AI0, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_AI1, UNIT_NUT_RATIO, DESC_AI1, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_AI2, UNIT_NUT_RATIO, DESC_AI2, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_AI3, UNIT_NUT_RATIO, DESC_AI3, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_AI4, UNIT_NUT_RATIO, DESC_AI4, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_AI5, UNIT_NUT_RATIO, DESC_AI5, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_AI6, UNIT_NUT_RATIO, DESC_AI6, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_LAMBDA0, UNIT_NON_DIM, DESC_LAMBDA0, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_LAMBDA1, UNIT_NON_DIM, DESC_LAMBDA1, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_LAMBDA2, UNIT_NON_DIM, DESC_LAMBDA2, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_K_L, UNIT_SR, DESC_K_L, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_K_N, UNIT_DENSITY_L, DESC_K_N, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_K_P, UNIT_DENSITY_L, DESC_K_P, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_P_N, UNIT_DENSITY_L, DESC_P_N, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_TFACT, UNIT_NON_DIM, DESC_TFACT, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_MUMAX, UNIT_TEMP_DEG, DESC_MUMAX, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_RHOQ, UNIT_PER_DAY, DESC_RHOQ, Source_ParameterDB, DT_Single);
    //mdi.AddParameter(VAR_MSK_X, UNIT_NON_DIM, DESC_MSK_X, Source_ParameterDB, DT_Single);
    //mdi.AddParameter(VAR_MSK_CO1, UNIT_NON_DIM, DESC_MSK_CO1, Source_ParameterDB, DT_Single);
    //mdi.AddParameter(VAR_VSF, UNIT_NON_DIM, DESC_VSF, Source_ParameterDB, DT_Single);

    //mdi.AddParameter(VAR_WATTEMP, UNIT_TEMP_DEG, DESC_WATTEMP, Source_ParameterDB, DT_Array1D);
    mdi.AddParameter(VAR_REACH_PARAM, UNIT_NON_DIM, DESC_REACH_PARAM, Source_ParameterDB, DT_Reach);
	mdi.AddParameter(VAR_STREAM_LINK, UNIT_NON_DIM, DESC_STREAM_LINK, Source_ParameterDB, DT_Raster1D);

    // set the input variables
    mdi.AddInput(DataType_SolarRadiation, UNIT_SR, DESC_SR, Source_Module, DT_Raster1D);
    mdi.AddInput(VAR_DAYLEN, UNIT_TIMESTEP_HOUR, DESC_DAYLEN, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_SOTE, UNIT_TEMP_DEG, DESC_SOTE, Source_Module, DT_Raster1D);
	mdi.AddInput(VAR_QRECH, UNIT_FLOW_CMS, DESC_QRECH, Source_Module, DT_Array1D);
	mdi.AddInput(VAR_CHST, UNIT_VOL_M3, DESC_CHST, Source_Module, DT_Array1D);
	mdi.AddInput(VAR_CHWTDEPTH, UNIT_LEN_M, DESC_CHWTDEPTH, Source_Module, DT_Array1D);

	/// input from hillslope
	//nutrient from surface water
	mdi.AddInput(VAR_SUR_NO3_TOCH, UNIT_KG, DESC_SUR_NO3_CH, Source_Module, DT_Array1D);
	mdi.AddInput(VAR_SUR_SOLP_TOCH, UNIT_KG, DESC_SUR_SOLP_CH, Source_Module, DT_Array1D);
	mdi.AddInput(VAR_SEDORGN_TOCH, UNIT_KG, DESC_SEDORGN_CH, Source_Module, DT_Array1D);
	mdi.AddInput(VAR_SEDORGP_TOCH, UNIT_KG, DESC_SEDORGP_CH, Source_Module, DT_Array1D);
	
	mdi.AddInput(VAR_SEDMINPA_TOCH, UNIT_KG, DESC_SEDMINPA_CH, Source_Module, DT_Array1D);
	mdi.AddInput(VAR_SEDMINPS_TOCH, UNIT_KG, DESC_SEDMINPS_CH, Source_Module, DT_Array1D);
	//mdi.AddInput(VAR_SUR_NH4_TOCH, UNIT_KG, DESC_SUR_NH4_CH, Source_Module, DT_Array1D);
	//nutrient from interflow
	mdi.AddInput(VAR_LATNO3_TOCH, UNIT_KG, DESC_LATNO3_CH, Source_Module, DT_Array1D);
	//nutrient from ground water
	mdi.AddInput(VAR_NO3GW_TOCH, UNIT_KG, DESC_NO3GW_CH, Source_Module, DT_Array1D);
	mdi.AddInput(VAR_MINPGW_TOCH, UNIT_KG, DESC_MINPGW_CH, Source_Module, DT_Array1D);

    // set the output variables
    mdi.AddOutput(VAR_SOXY, UNIT_DENSITY_L, DESC_SOXY, DT_Array1D);
    mdi.AddOutput(VAR_CH_ALGAE, UNIT_DENSITY_L, DESC_CH_ALGAE, DT_Array1D);
    mdi.AddOutput(VAR_CH_ORGN, UNIT_DENSITY_L, DESC_CH_ORGN, DT_Array1D);
    mdi.AddOutput(VAR_CH_ORGP, UNIT_DENSITY_L, DESC_CH_ORGP, DT_Array1D);
    mdi.AddOutput(VAR_CH_NH4, UNIT_DENSITY_L, DESC_CH_NH4, DT_Array1D);
    mdi.AddOutput(VAR_CH_NO2, UNIT_DENSITY_L, DESC_CH_NO2, DT_Array1D);
    mdi.AddOutput(VAR_CH_NO3, UNIT_DENSITY_L, DESC_CH_NO3, DT_Array1D);
    mdi.AddOutput(VAR_CH_SOLP, UNIT_DENSITY_L, DESC_CH_SOLP, DT_Array1D);
    mdi.AddOutput(VAR_CH_COD, UNIT_DENSITY_L, DESC_CH_COD, DT_Array1D);
    //mdi.AddOutput(VAR_RCH_DOX, UNIT_DENSITY_L, DESC_RCH_DOX, DT_Array1D);
    mdi.AddOutput(VAR_CH_CHLORA, UNIT_DENSITY_L, DESC_CH_CHLORA, DT_Array1D);

    string res = mdi.GetXMLDocument();
    char *tmp = new char[res.size() + 1];
    strprintf(tmp, res.size() + 1, "%s", res.c_str());
    return tmp;
}
