#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "PotentialBiomass_EPIC.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new PotentialBiomass_EPIC();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("LiangJun Zhu");
	mdi.SetClass(MCLS_POT_BIOM, MCLSDESC_POT_BIOM);
	mdi.SetDescription(MCLSDESC_POT_BIOM);
	mdi.SetEmail(SEIMS_EMAIL);
	mdi.SetID(MID_PBIO_EPIC);
	mdi.SetName(MID_PBIO_EPIC);
	mdi.SetVersion("1.0");
	mdi.SetWebsite(SEIMS_SITE);
	mdi.SetHelpfile("");

	mdi.AddParameter(VAR_CO2, UNIT_GAS_PPMV,DESC_CO2,Source_ParameterDB,DT_Single);
	
	mdi.AddParameter(VAR_SOILLAYERS, UNIT_NON_DIM, DESC_SOILLAYERS, Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_ROOTDEPTH, UNIT_DEPTH_MM, DESC_ROOTDEPTH,Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_SOILDEPTH, UNIT_DEPTH_MM, DESC_SOILDEPTH, Source_ParameterDB, DT_Raster2D);
	mdi.AddParameter(VAR_SOL_RSD, UNIT_CONT_KGKM2, DESC_SOL_RSD, Source_ParameterDB, DT_Array2D); 
	mdi.AddParameter(VAR_SOL_AWC, UNIT_VOL_FRA_M3M3, DESC_SOL_AWC, Source_ParameterDB,DT_Raster2D); /// NEED to convert unit from mm H2O/mm Soil to mm H2O in preprocess code. LJ 
	mdi.AddParameter(VAR_SOL_SUMAWC, UNIT_VOL_FRA_M3M3, DESC_SOL_SUMAWC, Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_EPCO, UNIT_NON_DIM, DESC_EPCO,Source_ParameterDB,DT_Raster1D);

	/// Crop or land cover related parameters
	mdi.AddParameter(VAR_IGRO, UNIT_NON_DIM, DESC_IGRO,Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_IDC, UNIT_NON_DIM, DESC_IDC, Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_EXT_COEF, UNIT_NON_DIM, DESC_EXT_COEF, Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_BLAI, UNIT_AREA_RATIO, DESC_BLAI, Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_MAT_YRS, UNIT_YEAR, DESC_MAT_YRS,Source_ParameterDB, DT_Raster1D);
	mdi.AddParameter(VAR_T_BASE, UNIT_TEMP_DEG, DESC_T_BASE, Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_FRGRW1, UNIT_NON_DIM, DESC_FRGRW1,Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_FRGRW2, UNIT_NON_DIM, DESC_FRGRW2,Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_LAIMX1, UNIT_NON_DIM,DESC_LAIMX1,Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_LAIMX2, UNIT_NON_DIM,DESC_LAIMX2,Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_DLAI, UNIT_NON_DIM, DESC_DLAI,Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_BN1, UNIT_NON_DIM, DESC_BN1,Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_BN2, UNIT_NON_DIM, DESC_BN2,Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_BN3, UNIT_NON_DIM, DESC_BN3,Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_BP1, UNIT_NON_DIM, DESC_BP1,Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_BP2, UNIT_NON_DIM, DESC_BP2,Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_BP3, UNIT_NON_DIM, DESC_BP3,Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_BIO_E,UNIT_RAD_USE_EFFI, DESC_BIO_E, Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_BIOEHI, UNIT_RAD_USE_EFFI, DESC_BIOEHI, Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_CO2HI, UNIT_GAS_PPMV,DESC_CO2HI,Source_ParameterDB,DT_Raster1D);
	mdi.AddParameter(VAR_WAVP, UNIT_RAD_USE_EFFI, DESC_WAVP, Source_ParameterDB,DT_Raster1D);

	/// climate parameters
	mdi.AddInput(DataType_MeanTemperature,UNIT_TEMP_DEG, DESC_MEANTEMP, Source_Module,DT_Raster1D);
	mdi.AddInput(DataType_MinimumTemperature,UNIT_TEMP_DEG, DESC_MINTEMP, Source_Module,DT_Raster1D);
	mdi.AddInput(DataType_MaximumTemperature,UNIT_TEMP_DEG, DESC_MAXTEMP, Source_Module,DT_Raster1D);						
	mdi.AddInput(DataType_SolarRadiation, UNIT_SR, DESC_SR, Source_Module, DT_Raster1D);
	mdi.AddInput(DataType_RelativeAirMoisture, UNIT_PERCENT, DESC_RM, Source_Module, DT_Raster1D);
	/// maximum plant et
	mdi.AddInput(VAR_PPT,UNIT_WTRDLT_MMD, DESC_PPT, Source_Module, DT_Raster1D);
	/// amount of water stored in soil layers
	mdi.AddInput(VAR_SOMO, UNIT_DEPTH_MM, DESC_SOMO, Source_Module,DT_Raster2D);
	mdi.AddInput(VAR_SOMO_TOT, UNIT_DEPTH_MM, DESC_SOMO_TOT,Source_Module, DT_Raster1D);
	// set the output variables
	mdi.AddOutput(VAR_LAIDAY,UNIT_AREA_RATIO, DESC_LAIDAY,DT_Raster1D);
	mdi.AddOutput(VAR_LAIPRE, UNIT_AREA_RATIO, DESC_LAIPRE,DT_Raster1D);
	mdi.AddOutput(VAR_SOL_COV, UNIT_CONT_KGHA, DESC_SOL_COV, DT_Raster1D);
	mdi.AddOutput(VAR_FR_PHU_ACC, UNIT_NON_DIM, DESC_FR_PHU_ACC,DT_Raster1D);
	mdi.AddOutput(VAR_AET,UNIT_DEPTH_MM,DESC_AET,DT_Raster1D);
	mdi.AddOutput(VAR_BIOMASS, UNIT_CONT_KGHA, DESC_BIOMASS,DT_Raster1D);

	mdi.AddOutput(VAR_SOMO, UNIT_DEPTH_MM, DESC_SOMO,DT_Raster2D);
	mdi.AddOutput(VAR_SOMO_TOT, UNIT_DEPTH_MM, DESC_SOMO_TOT,DT_Raster1D);





	mdi.AddOutput("BIOMASS_Delta","kg/ha","biomass increase on a given day",DT_Raster1D);
	mdi.AddOutput("BIOMASS_NOpt","kg/ha","potential biomass N",DT_Raster1D);
	mdi.AddOutput("BIOMASS_POpt","kg/ha","potential biomass P",DT_Raster1D);
	mdi.AddOutput("VPD","kPa","vapor pressure deficit",DT_Raster1D);
	mdi.AddOutput("Fr_N","","the optimal fraction of nitrogen in the plant biomass ",DT_Raster1D);
	mdi.AddOutput("Fr_P","","the optimal fraction of phosphorus in the plant biomass ",DT_Raster1D);
	mdi.AddOutput("Fr_Root","","the fraction of total biomass partitioned to roots on a given day  ",DT_Raster1D);
	mdi.AddOutput("LAI_Delta","","potential leaf area added on day",DT_Raster1D);

	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}