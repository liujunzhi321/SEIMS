/*!
 * \file api.cpp
 * \brief Define MetadataInfo of NutOLRout module.
/*!
 * \file api.cpp
 * \ingroup NutOLRout
 * \author Huiran Gao
 * \date Jun 2016
 */


#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "NutrientOLRoute.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new NutrientOLRoute();
}

//! function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;
	mdi.SetAuthor("Huiran Gao");
	mdi.SetClass(MCLS_NutOLRout, MCLSDESC_NutOLRout);
	mdi.SetDescription(MDESC_NutOLRout);
	mdi.SetEmail(SEIMS_EMAIL);
	mdi.SetID(MDESC_NutOLRout);
	mdi.SetName(MDESC_NutOLRout);
	mdi.SetVersion("1.0");
	mdi.SetWebsite(SEIMS_SITE);
	mdi.SetHelpfile("NutOLRout.html");

	// set the parameters
	mdi.AddParameter(Tag_CellSize, UNIT_NON_DIM, DESC_CellSize, Source_ParameterDB, DT_Single);
	mdi.AddParameter(Tag_CellWidth, UNIT_LEN_M, DESC_CellWidth, Source_ParameterDB, DT_Single);
	mdi.AddParameter(Tag_HillSlopeTimeStep, UNIT_SECOND, DESC_DT_HS, File_Input, DT_Single);

	mdi.AddParameter(VAR_STREAM_LINK, UNIT_NON_DIM, DESC_STREAM_LINK, Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_CHWIDTH, UNIT_LEN_M, DESC_CHWIDTH, Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_LATNO3, UNIT_CONT_KGKM2, DESC_LATNO3, Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_SURQNO3, UNIT_CONT_KGKM2, DESC_SURQNO3, Source_ParameterDB, DT_Raster1D);
	//mdi.AddParameter(VAR_AMMONIAN, UNIT_CONT_KGKM2, DESC_AMMONIAN, Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_SURQSOLP, UNIT_CONT_KGKM2, DESC_SURQSOLP, Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_NO3GW, UNIT_CONT_KGKM2, DESC_NO3GW, Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_MINPGW, UNIT_CONT_KGKM2, DESC_MINPGW, Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_SEDORGN, UNIT_CONT_KGKM2, DESC_SEDORGN, Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_SEDORGP, UNIT_CONT_KGKM2, DESC_SEDORGP, Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_SEDMINPA, UNIT_CONT_KGKM2, DESC_SEDMINPA, Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_SEDMINPS, UNIT_CONT_KGKM2, DESC_SEDMINPS, Source_ParameterDB, DT_Raster1D);
	
	mdi.AddParameter(Tag_FLOWIN_INDEX_D8, UNIT_NON_DIM, DESC_FLOWIN_INDEX_D8, Source_ParameterDB, DT_Array2D);
	mdi.AddParameter(Tag_ROUTING_LAYERS, UNIT_NON_DIM, DESC_ROUTING_LAYERS, Source_ParameterDB, DT_Array2D);

	// set the output variables
	mdi.AddOutput(VAR_SURQNO3_CH, UNIT_KG, DESC_SURQNO3_CH, DT_Raster1D);
	mdi.AddOutput(VAR_LATNO3_CH, UNIT_KG, DESC_LATNO3_CH, DT_Raster1D);
	mdi.AddOutput(VAR_NO3GW_CH, UNIT_KG, DESC_NO3GW_CH, DT_Raster1D);
	mdi.AddOutput(VAR_SURQSOLP_CH, UNIT_KG, DESC_SURQSOLP_CH, DT_Raster1D);
	mdi.AddOutput(VAR_MINPGW_CH, UNIT_KG, DESC_MINPGW_CH, DT_Raster1D);
	mdi.AddOutput(VAR_SEDORGN_CH, UNIT_KG, DESC_SEDORGN_CH, DT_Raster1D);
	mdi.AddOutput(VAR_SEDORGP_CH, UNIT_KG, DESC_SEDORGP_CH, DT_Raster1D);
	mdi.AddOutput(VAR_SEDMINPA_CH, UNIT_KG, DESC_SEDMINPA_CH, DT_Raster1D);
	mdi.AddOutput(VAR_SEDMINPS_CH, UNIT_KG, DESC_SEDMINPS_CH, DT_Raster1D);
	mdi.AddOutput(VAR_AMMO_CH, UNIT_KG, DESC_AMMO_CH, DT_Raster1D);
	mdi.AddOutput(VAR_NITRITE_CH, UNIT_KG, DESC_NITRITE_CH, DT_Raster1D);

	string res = mdi.GetXMLDocument();
	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}
