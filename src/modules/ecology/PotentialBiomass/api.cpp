#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "PotentialBiomass.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new PotentialBiomass();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Cheng Wei");
	mdi.SetClass("PotentialBiomass", "Calculate the amount of PotentialBiomass.");
	mdi.SetDescription("For simplicity purpose for the time being, the algorithm used in the original WetSpa is incorporated in SEIMS.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetID("PotentialBiomass");
	mdi.SetName("PotentialBiomass");
	mdi.SetVersion("0.5");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
	mdi.SetHelpfile("PotentialBiomass.chm");

	mdi.AddParameter("IDC","","land cover/plant classification","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("EXT_COEF","","light extinction coefficient","ParameterDB_Crop",DT_Raster);

	mdi.AddParameter("BLAI","","maximum leaf area index","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("startDate","","the date begin to simulate ","ParameterDB",DT_Raster);
	mdi.AddParameter("endDate","","the simulation end of date","ParameterDB",DT_Raster);


	mdi.AddParameter("MAT_YRS","years","the number of years for the tree species to reach full development","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("T_BASE","oC"," base or minimum temperature for growth","ParameterDB_Crop",DT_Raster);

	mdi.AddParameter("FRGRW1",""," fraction of total potential heat units corresponding to the 1st point on the optimal leaf area development curve","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("FRGRW2",""," fraction of total potential heat units corresponding to the 2nd point on the optimal leaf area development curve","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("LAIMX1",""," fraction of max LAI corresponding to the 1st point on the optimal leaf area development curve","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("LAIMX2",""," fraction of max LAI corresponding to the 2nd point on the optimal leaf area development curve","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("DLAI",""," the fraction of growing season(PHU) when senescence becomes dominant ","ParameterDB_Crop",DT_Raster);

	mdi.AddParameter("BN1",""," the normal fraction of nitrogen in the plant biomass at emergence","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("BN2",""," the normal fraction of nitrogen in the plant biomass at 50% maturity ","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("BN3","","  the normal fraction of nitrogen in the plant biomass at maturity","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("BP1",""," the normal fraction of phosphorus in the plant biomass at emergence","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("BP2",""," the normal fraction of phosphorus in the plant biomass at 50% maturity","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("BP3","","the normal fraction of phosphorus in the plant biomass at maturity","ParameterDB_Crop",DT_Raster);

	mdi.AddParameter("CO2","ppmv","CO2 concentration","ParameterDB_Climate",DT_Single);
	mdi.AddParameter("BIO_E","kg*m*m/ha*MJ","rue of the plant at ambient atmospheric CO_2 concentration","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("BIOEHI","kg*m*m/ha*MJ","rue of the plant at elevated atmospheric CO_2 concentration","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("CO2HI","ppmv","an elevated atmospheric CO2 concentration","ParameterDB_Crop",DT_Raster);
	mdi.AddParameter("WAVP","kg*m*m/ha*MJ","rate of decline in rue per unit increase in vapor pressure deficit","ParameterDB_Crop",DT_Raster);


	mdi.AddInput("D_TMIN","oC","min temperature","Module",DT_Raster);									//! from interpolation module
	mdi.AddInput("D_TMAX","oC","max temperature","Module",DT_Raster);									//! from interpolation module
	mdi.AddInput("D_SR","MJ/m2/d","Solar radiation","Module", DT_Raster);
	mdi.AddInput("D_RM","","Relative humidity","Module", DT_Raster);

	
	// set the output variables
	mdi.AddOutput("BIOMASS_Delta","kg/ha","biomass increase on a given day",DT_Raster);
	mdi.AddOutput("BIOMASS","kg/ha","total biomass",DT_Raster);
	mdi.AddOutput("BIOMASS_NOpt","kg/ha","potential biomass N",DT_Raster);
	mdi.AddOutput("BIOMASS_POpt","kg/ha","potential biomass P",DT_Raster);


	mdi.AddOutput("VPD","kPa","vapor pressure deficit",DT_Raster);
	mdi.AddOutput("Fr_PHU","","fraction of phu accumulated on a given day ",DT_Raster);
	mdi.AddOutput("Fr_N","","the optimal fraction of nitrogen in the plant biomass ",DT_Raster);
	mdi.AddOutput("Fr_P","","the optimal fraction of phosphorus in the plant biomass ",DT_Raster);
	mdi.AddOutput("Fr_Root","","the fraction of total biomass partitioned to roots on a given day  ",DT_Raster);
	mdi.AddOutput("LAI_Delta","","potential leaf area added on day",DT_Raster);
	mdi.AddOutput("LAI","","leaf area index for a given day",DT_Raster);
	mdi.AddOutput("LAI_Pre","","leaf area index for the previous day",DT_Raster);

	// write out the XML file.

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}