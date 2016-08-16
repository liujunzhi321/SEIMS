#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SEDR_VCD.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule *GetInstance()
{
    return new SEDR_VCD();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char *MetadataInformation()
{
    string res = "";
    MetadataInfo mdi;

    // set the information properties
    mdi.SetAuthor("Wu Hui; Junzhi Liu");
    mdi.SetClass(MCLS_SED_ROUTING, MCLSDESC_SED_ROUTING);
    mdi.SetDescription(MDESC_SEDR_VCD);
    mdi.SetEmail(SEIMS_EMAIL);
    mdi.SetHelpfile("SEDR_VCD.chm");
    mdi.SetID(MID_SEDR_VCD);
    mdi.SetName(MID_SEDR_VCD);
    mdi.SetVersion("1.0");
    mdi.SetWebsite(SEIMS_SITE);
#ifdef STORM_MODEL
    mdi.AddParameter(Tag_ChannelTimeStep,UNIT_SECOND,DESC_TIMESTEP,File_Input,DT_Single);
#else
    mdi.AddParameter(Tag_TimeStep, UNIT_SECOND, DESC_TIMESTEP, File_Input, DT_Single); // daily model
#endif
    mdi.AddParameter(VAR_P_RF, UNIT_NON_DIM, DESC_P_RF, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_SPCON, UNIT_NON_DIM, DESC_SPCON, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_SPEXP, UNIT_NON_DIM, DESC_SPEXP, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_VCRIT, UNIT_SPEED_MS, DESC_VCRIT, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_CHS0, UNIT_STRG_M3M, DESC_CHS0, Source_ParameterDB, DT_Single);
	/// load reach parameters, the previous version Tag_RchParam is deprecated!
	mdi.AddParameter(VAR_REACH_PARAM, UNIT_NON_DIM, DESC_REACH_PARAM, Source_ParameterDB, DT_Reach);
    //mdi.AddParameter(Tag_RchParam, UNIT_NON_DIM, DESC_REACH_PARAM, Source_ParameterDB, DT_Array2D);
	/// load point source loading sediment from Scenario
	mdi.AddParameter(VAR_SCENARIO, UNIT_NON_DIM, DESC_SCENARIO, Source_ParameterDB, DT_Scenario);
    mdi.AddInput(VAR_SED_TO_CH, UNIT_KG, DESC_SED_TO_CH, Source_Module, DT_Array1D);
    mdi.AddInput(VAR_QRECH, UNIT_FLOW_CMS, DESC_QRECH, Source_Module, DT_Array1D);
    mdi.AddInput(VAR_CHST, UNIT_VOL_M3, DESC_CHST, Source_Module, DT_Array1D);
    mdi.AddInput(VAR_CHWTDEPTH, UNIT_LEN_M, DESC_CHWTDEPTH, Source_Module, DT_Array1D);

    mdi.AddOutput(VAR_SED_RECH, UNIT_KG, DESC_SED_RECH, DT_Array1D);
    mdi.AddOutput(VAR_SED_OUTLET, UNIT_KG, DESC_SED_OUTLET, DT_Single);

    // set the dependencies
    mdi.AddDependency(MCLS_OL_EROSION, MCLSDESC_OL_EROSION);
    mdi.AddDependency(MCLS_CH_ROUTING, MCLSDESC_CH_ROUTING); 

    res = mdi.GetXMLDocument();

    char *tmp = new char[res.size() + 1];
    strprintf(tmp, res.size() + 1, "%s", res.c_str());
    return tmp;
}

//mdi.AddParameter("Vseep0","m3/s","the initial volume of transmission loss to the deep aquifer over the time interval","ParameterDB_Discharge", DT_Single);

//TODO: add later...
//mdi.AddParameter("Vdiv","m3","diversion loss of the river reach", "diversionloss.txt",DT_Array1D);
//mdi.AddParameter("Vpoint","m3"," point source discharge", "diversionloss.txt",DT_Array1D);

//mdi.AddInput("CROSS_AREA", "m2", "the cross-sectional area of flow in the channel","Module", DT_Array1D);
//mdi.AddParameter("subbasin","","subbasin grid","ParameterDB_Discharge", DT_Raster1D);

//mdi.AddInput("SEEPAGE", "m3", "seepage", "Module",DT_Array1D);
//mdi.AddInput("C_WABA","","Channel water balance in a text format for each reach and at each time step","Module",DT_Array2D);
//mdi.AddOutput("DEPOUTLET", "ton", "sediment concentration at the watershed outlet", DT_Single);

    //mdi.AddOutput(VAR_CHSB, UNIT_NON_DIM, DESC_CHSB, DT_Array2D);