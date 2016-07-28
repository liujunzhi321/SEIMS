#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "VSM_CH.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new VSM_CH();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Wu Hui; Liu Junzhi");
	mdi.SetClass("Channelflow", "channel flow routing using variable storage method.");
	mdi.SetDescription("channel flow routing using variable storage method.");
	mdi.SetEmail("");
	mdi.SetHelpfile("VSM_CH.chm");
	mdi.SetID("VSM_CH");
	mdi.SetName("VSM_CH");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://www.website.com");

	mdi.AddParameter("TimeStep","s","time step","file.in",DT_Single); 
	//mdi.AddParameter("CellSize","","the size of the cell (the validate cells of the whole basin)","file.in",DT_Single); 
	//mdi.AddParameter("nPETS","","the number of the PET stations","file.in",DT_Single);
	//mdi.AddParameter("CellWidth","m","cell width of the grid","file.in",DT_Single);
	mdi.AddParameter("K_chb","mm/h","hydraulic conductivity of the channel bed","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("K_bank","mm/h","hydraulic conductivity of the channel bank","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("Ep_ch","mm/h","reach evaporation adjustment factor","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("Bnk0","m3/m","initial bank storage per meter of reach length","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("Chs0","m3/m","initial channel storage per meter of reach length","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("Vseep0","m3/s","the initial volume of transmission loss to the deep aquifer over the time interval","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("a_bnk","","bank flow recession constant","ParameterDB_Discharge", DT_Single);
	mdi.AddParameter("b_bnk","","bank storage loss coefficient","ParameterDB_Discharge", DT_Single);

	//mdi.AddParameter("RchParam","","reach parameters", "reachparameters.txt",DT_Array2D);
	mdi.AddParameter("ReachParameter", "", "Reach parameters such stream order, manning's n and downstream subbasin id", "ParameterDB_Discharge", DT_Array2D);
	//mdi.AddParameter("Vdiv","m3","diversion loss of the river reach", "diversionloss.txt",DT_Array1D);
	//mdi.AddParameter("Vpoint","m3"," point source discharge", "diversionloss.txt",DT_Array1D);

	mdi.AddParameter("subbasin","","subbasin grid","ParameterDB_Discharge", DT_Raster1D);
	mdi.AddInput("D_SBOF","m3/s","overland flow to streams from each subbasin","Module", DT_Array1D);
	mdi.AddInput("D_SBIF","m3/s","interflow to streams from each subbasin","Module", DT_Array1D);
	mdi.AddInput("D_SBQG","m3/s","groundwater flow out of the subbasin","Module", DT_Array1D);
	mdi.AddInput("D_SBPET","mm","the potential evapotranspiration rate of the subbasin","Module", DT_Array1D);
	mdi.AddInput("D_SBGS","mm","Groundwater storage of the subbasin","Module", DT_Array1D);

	mdi.AddOutput("QRECH","","Discharge in a text format at each reach outlet and at each time step",DT_Array1D);
	mdi.AddOutput("QS","","Overland discharge at each reach outlet and at each time step",DT_Array1D);
	mdi.AddOutput("QI","","Interflow at each reach outlet and at each time step",DT_Array1D);
	mdi.AddOutput("QG","","Groundwater discharge at each reach outlet and at each time step",DT_Array1D);
	mdi.AddOutput("CHST", "m3", "channel storage", DT_Array1D);
	mdi.AddOutput("BKST", "m3", "bank storage", DT_Array1D);
	mdi.AddOutput("SEEPAGE", "m3", "seepage", DT_Array1D);
	mdi.AddOutput("CROSS_AREA", "m2", "the cross-sectional area of flow in the channel", DT_Array1D);
	mdi.AddOutput("C_WABA","","Channel water balance in a text format for each reach and at each time step",DT_Array2D);
	//Added by Wu Hui, 2012.12.9
	mdi.AddOutput("QOUTLET", "m3/s", "discharge at the watershed outlet", DT_Single);

	// set the dependencies
	mdi.AddDependency("VSM_OL","VSM_OL module");   
	mdi.AddDependency("IUH_IF","IUH_IF module");   
	mdi.AddDependency("GWA_RE","GWA_RE module");   
	mdi.AddDependency("PET","PET module");   
	mdi.AddDependency("GWA_RE","GWA_RE module");   	//for module GWA_RE to update groundwater storage

	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}