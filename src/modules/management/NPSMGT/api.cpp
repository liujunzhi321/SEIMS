#include <stdio.h>
#include <string>
#include "util.h"
#include "api.h"
#include "NonPointSource_Management.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule *GetInstance()
{
    return new NPS_Management();
}

/// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char *MetadataInformation()
{
    MetadataInfo mdi;
    string res;

    mdi.SetAuthor("Liang-Jun Zhu");
    mdi.SetClass(MCLS_MGT, MCLSDESC_MGT);
    mdi.SetDescription(MDESC_NPSMGT);
    mdi.SetID(MID_NPSMGT);
    mdi.SetName(MID_NPSMGT);
    mdi.SetVersion("0.1");
    mdi.SetEmail(SEIMS_EMAIL);
    mdi.SetWebsite(SEIMS_SITE);
    mdi.SetHelpfile("NPSMGT.html");
    /// set parameters from database
    mdi.AddParameter(VAR_SCENARIO, UNIT_NON_DIM, DESC_SCENARIO, Source_ParameterDB, DT_Scenario);
    /// set input from other modules
    //mdi.AddInput("VAR_NAME", "UNIT", "DESC", "Source", "DT");
    /// outputs

    /// write out the XML file.
    res = mdi.GetXMLDocument();

    char *tmp = new char[res.size() + 1];
    strprintf(tmp, res.size() + 1, "%s", res.c_str());
    return tmp;
}