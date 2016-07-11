#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SSR_DA.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule *GetInstance()
{
    return new SSR_DA();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char *MetadataInformation()
{
    MetadataInfo mdi;

    mdi.SetAuthor("Zhiqiang Yu; Junzhi Liu");
    mdi.SetClass("Subsurface Runoff", UNIT_NON_DIM);
    mdi.SetDescription(
            "Darcy's law and the kinematic approximation; Water is routed cell-to-cell according to D8 or Dinf flow direction");
    mdi.SetEmail(SEIMS_EMAIL);
    mdi.SetID("SSR_DA");
    mdi.SetName("SSR_DA");
    mdi.SetVersion("0.5");
    mdi.SetWebsite(SEIMS_SITE);
    mdi.SetHelpfile("SSR_DA.chm");

    mdi.AddParameter(Tag_CellSize, UNIT_NON_DIM, DESC_CellSize, Source_ParameterDB, DT_Single);
    mdi.AddParameter(Tag_CellWidth, UNIT_LEN_M, DESC_CellWidth, Source_ParameterDB, DT_Single);
    mdi.AddParameter(Tag_TimeStep, UNIT_TIMESTEP_HOUR, UNIT_NON_DIM, File_Input, DT_Single);
    //mdi.AddParameter(VAR_UPSOLDEP, UNIT_DEPTH_MM, DESC_UPSOLDEP, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_KI, UNIT_NON_DIM, DESC_KI, Source_ParameterDB, DT_Single);
    mdi.AddParameter(VAR_T_SOIL, UNIT_TEMP_DEG, DESC_T_SOIL, Source_ParameterDB, DT_Single);

    mdi.AddParameter(VAR_CONDUCT, UNIT_WTRDLT_MMH, DESC_CONDUCT, Source_ParameterDB, DT_Array2D);
    mdi.AddParameter(VAR_POROST, UNIT_VOL_FRA_M3M3, DESC_POROST, Source_ParameterDB, DT_Array2D);
    mdi.AddParameter(VAR_POREID, UNIT_NON_DIM, DESC_POREID, Source_ParameterDB, DT_Array2D);
    mdi.AddParameter(VAR_FIELDCAP, UNIT_VOL_FRA_M3M3, DESC_FIELDCAP, Source_ParameterDB, DT_Array2D);

    mdi.AddParameter(VAR_SOILDEPTH, UNIT_DEPTH_MM, DESC_SOILDEPTH, Source_ParameterDB, DT_Raster1D);
    mdi.AddParameter(VAR_SLOPE, UNIT_PERCENT, DESC_SLOPE, Source_ParameterDB, DT_Raster1D);
    mdi.AddParameter(VAR_CHWIDTH, UNIT_LEN_M, DESC_CHWIDTH, Source_ParameterDB, DT_Raster1D);
    mdi.AddParameter(VAR_STREAM_LINK, UNIT_NON_DIM, DESC_STREAM_LINK, Source_ParameterDB, DT_Raster1D);
    //mdi.AddParameter(Tag_FLOWOUT_INDEX_DINF, UNIT_NON_DIM, DESC_FLOWOUT_INDEX_DINF,  Source_ParameterDB, DT_Array2D);
    mdi.AddParameter(Tag_FLOWIN_INDEX_D8, UNIT_NON_DIM, DESC_FLOWIN_INDEX_D8, Source_ParameterDB, DT_Array2D);
    //mdi.AddParameter(Tag_FLOWIN_PERCENTAGE_DINF, UNIT_NON_DIM, DESC_Tag_FLOWIN_PERCENTAGE_DINF,  Source_ParameterDB, DT_Array2D);
    mdi.AddParameter(Tag_ROUTING_LAYERS, UNIT_NON_DIM, DESC_ROUTING_LAYERS, Source_ParameterDB, DT_Array2D);
    mdi.AddParameter(Tag_FLOWIN_PERCENTAGE_D8, UNIT_NON_DIM, DESC_FLOWIN_PERCENTAGE_D8, Source_ParameterDB, DT_Array2D);

    mdi.AddParameter(VAR_SUBBSN, UNIT_NON_DIM, DESC_SUBBSN, Source_ParameterDB, DT_Raster1D);

    // set the parameters (non-time series)
    mdi.AddInput(VAR_GRRE, UNIT_DEPTH_MM, DESC_GRRE, Source_Module, DT_Raster1D);
    mdi.AddInput(VAR_SOTE, UNIT_TEMP_DEG, DESC_SOTE, Source_Module, DT_Raster1D);
    mdi.AddInput(VAR_SOMO, UNIT_DEPTH_MM, DESC_SOMO, Source_Module, DT_Raster2D);

    // set the output variables
    mdi.AddOutput(VAR_SSRU, UNIT_DEPTH_MM, DESC_SSRU, DT_Array2D);
    mdi.AddOutput(VAR_SSRUVOL, UNIT_DEPTH_MM, DESC_SSRUVOL, DT_Array2D);
    mdi.AddOutput(VAR_SBIF, UNIT_DEPTH_MM, DESC_SBIF, DT_Array1D);

    string res = mdi.GetXMLDocument();

    char *tmp = new char[res.size() + 1];
    strprintf(tmp, res.size() + 1, "%s", res.c_str());
    return tmp;
}
