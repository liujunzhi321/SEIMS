/*!
 * \file api.cpp
 * \brief 
 *
 *
 *
 * \author [your name]
 * \version 
 * \date June 2015
 *
 * 
 */
#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "DepressionFSDaily.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"
/** \defgroup Hydrology_longterm
 * \brief Hydrology modules, mainly includes longterm modules
 *
 *
 *
 */
/** \defgroup DEP_LINSLEY
 * \ingroup Hydrology_longterm
 * \brief 
 *
 *
 *
 */
//! Get instance of SimulationModule class
extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new DepressionFSDaily();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Junzhi Liu");
	mdi.SetClass("Depression", "Calculate depression storage.");
	mdi.SetDescription("A simple fill and spill method method to calculate depression storage.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("DEP_LINSLEY.chm");
	mdi.SetID("DEP_LINSLEY");
	mdi.SetName("DEP_LINSLEY");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("Depre_in","-","initial depression storage coefficient","ParameterDB_WaterBalance", DT_Single); 
	mdi.AddParameter("Depression","mm","Depression storage capacity","ParameterDB_WaterBalance", DT_Raster);

	mdi.AddInput("D_INET","mm","evaporation from the interception storage obtained from the interception module","Module", DT_Raster);	//EI
	mdi.AddInput("D_PET", "mm","PET calculated from the spatial interpolation module", "Module", DT_Raster);							//PET
	//mdi.AddInput("D_INFIL","mm","Infiltration calculated in the infiltration module", "Module", DT_Raster);							//Infiltration
	mdi.AddInput("D_EXCP","mm","excess precipitation calculated in the infiltration module", "Module", DT_Raster);						//PE

	mdi.AddOutput("DPST", "mm", "Distribution of depression storage", DT_Raster);
	mdi.AddOutput("DEET", "mm", "Distribution of evaporation from depression storage", DT_Raster);
	mdi.AddOutput("SURU", "mm", "Distribution of surface runoff", DT_Raster);

	// set the dependencies
	mdi.AddDependency("Interpolation","Interpolation module");
	mdi.AddDependency("Interception","Interception module");
	mdi.AddDependency("Infiltration","Infiltration module");

	string res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}