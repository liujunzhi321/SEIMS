/*!
 * \file api.cpp
 * \brief Define MetadataInfo of MUSK_CH module.
 *
 * 
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date 23-Febrary-2011
 *
 * 
 */
#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "MUSK_CH.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"
/** \defgroup MUSKINGUM_CH
 * \ingroup Hydrology_longterm
 * \brief 
 *
 *
 *
 */
//! Get instance of SimulationModule class
extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new MUSK_CH();
}

/*!
 * \ingroup MUSKINGUM_CH
 * \brief function to return the XML Metadata document string
 *
 * 
 *
 * \param[in] 
 * \param[out] 
 * \return
 */
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Liu Junzhi");
	mdi.SetClass("Channelflow", "channel flow routing using variable storage method.");
	mdi.SetDescription("channel flow routing using variable storage method.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("MUSK_CH.chm");
	mdi.SetID("MUSK_CH");
	mdi.SetName("MUSK_CH");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("DT_CH","s","time step","file.in",DT_Single); 
	mdi.AddParameter("K_chb","mm/h","hydraulic conductivity of the channel bed","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("K_bank","mm/h","hydraulic conductivity of the channel bank","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("Ep_ch","mm/h","reach evaporation adjustment factor","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("Bnk0","m3/m","initial bank storage per meter of reach length","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("Chs0","m3/m","initial channel storage per meter of reach length","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("Vseep0","m3/s","the initial volume of transmission loss to the deep aquifer over the time interval","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("a_bnk","","bank flow recession constant","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("b_bnk","","bank storage loss coefficient","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("MSK_X","","muskingum weighting factor","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("MSK_co1","","Weighting factor of bankful flow","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("VelScaleFactor","","flow velocity scaling factor for calibration","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("RchParam","","reach parameters", "reachparameters.txt",DT_Array2D);
	//mdi.AddParameter("Vdiv","m3","diversion loss of the river reach", "diversionloss.txt",DT_Array1D);
	//mdi.AddParameter("Vpoint","m3"," point source discharge", "diversionloss.txt",DT_Array1D);
	mdi.AddParameter("subbasin","","subbasin grid","ParameterDB_Discharge", DT_Raster);

	mdi.AddInput("D_SBOF","m3/s","overland flow to streams from each subbasin","Module", DT_Array1D);
	mdi.AddInput("D_SBIF","m3/s","interflow to streams from each subbasin","Module", DT_Array1D);
	mdi.AddInput("D_SBQG","m3/s","groundwater flow out of the subbasin","Module", DT_Array1D);
	mdi.AddInput("D_SBPET","mm","the potential evapotranspiration rate of the subbasin","Module", DT_Array1D);
	mdi.AddInput("D_SBGS","mm","Groundwater storage of the subbasin","Module", DT_Array1D);

	mdi.AddOutput("QRECH","","Discharge in a text format at each reach outlet and at each time step",DT_Array1D);
	mdi.AddOutput("QOUTLET", "m3/s", "discharge at the watershed outlet", DT_Single);
	mdi.AddOutput("QSOUTLET", "m3/s", "discharge at the watershed outlet", DT_Single);
	mdi.AddOutput("QS","","Overland discharge at each reach outlet and at each time step",DT_Array1D);
	mdi.AddOutput("QI","","Interflow at each reach outlet and at each time step",DT_Array1D);
	mdi.AddOutput("QG","","Groundwater discharge at each reach outlet and at each time step",DT_Array1D);
	mdi.AddOutput("CHST", "m3", "channel storage", DT_Array1D);
	mdi.AddOutput("BKST", "m3", "bank storage", DT_Array1D);
	mdi.AddOutput("SEEPAGE", "m3", "seepage", DT_Array1D);
	mdi.AddOutput("CHWTDEPTH", "m", "channel water depth", DT_Array1D);
	//mdi.AddOutput("C_WABA","","Channel water balance in a text format for each reach and at each time step",DT_Array2D);

	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}