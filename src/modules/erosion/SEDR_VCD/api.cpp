#include <stdio.h>
#include <string>
#include "api.h"
#include "util.h"
#include "SEDR_VCD.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"

extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new SEDR_VCD();
}

// function to return the XML Metadata document string
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Wu Hui");
	mdi.SetClass("Sediment", "Sediment channel routing using variable channel dimention method.");
	mdi.SetDescription("Sediment channel routing using variable channel dimention method as used in SWAT (in SWAT document, page 379).");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("SEDR_VCD.chm");
	mdi.SetID("SEDR_VCD");
	mdi.SetName("SEDR_VCD");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");
#ifdef STORM_MODEL
	mdi.AddParameter("DT_CH","s","time step","file.in",DT_Single);  //for long term model
#else
	mdi.AddParameter("TimeStep","s","time step","file.in",DT_Single); 
#endif
	mdi.AddParameter("p_rf","", "Peak rate adjustment factor","ParameterDB_Sediment", DT_Single);
	mdi.AddParameter("spcon","", "Coefficient in sediment transport equation","ParameterDB_Sediment", DT_Single);
	mdi.AddParameter("spexp","", "Exponent in sediment transport equation","ParameterDB_Sediment", DT_Single);
	mdi.AddParameter("vcrit","m/s", "critical veloctiy for sediment deposition","ParameterDB_Sediment", DT_Single);

	//mdi.AddParameter("CellSize","","the size of the cell (the validate cells of the whole basin)","file.in",DT_Single); 
	//mdi.AddParameter("nPETS","","the number of the PET stations","file.in",DT_Single);
	//mdi.AddParameter("CellWidth","m","cell width of the grid","file.in",DT_Single);
	mdi.AddParameter("Chs0","m3/m","initial channel storage per meter of reach length","ParameterDB_Discharge", DT_Single);
	//mdi.AddParameter("Vseep0","m3/s","the initial volume of transmission loss to the deep aquifer over the time interval","ParameterDB_Discharge", DT_Single);

	mdi.AddParameter("RchParam","","reach parameters", "reachparameters.txt",DT_Array2D);
	//mdi.AddParameter("ReachParameter", "", "Reach parameters such stream order, manning's n and downstream subbasin id", "ParameterDB_Discharge", DT_Array2D);
	//mdi.AddParameter("Vdiv","m3","diversion loss of the river reach", "diversionloss.txt",DT_Array1D);
	//mdi.AddParameter("Vpoint","m3"," point source discharge", "diversionloss.txt",DT_Array1D);

	//mdi.AddParameter("subbasin","","subbasin grid","ParameterDB_Discharge", DT_MapWindowRaster);
	mdi.AddInput("SEDTOCH","ton","sediment to streams from each subbasin","Module", DT_Array1D);  // for longterm model
	mdi.AddInput("SubSEDTOCH","kg","sediment to streams from each subbasin","Module", DT_Array1D);   // for storm model
	//mdi.AddInput("CROSS_AREA", "m2", "the cross-sectional area of flow in the channel","Module", DT_Array1D);
	mdi.AddInput("QRECH","","Discharge in a text format at each reach outlet and at each time step","Module",DT_Array1D);
	mdi.AddInput("QS","","Overland discharge at each reach outlet and at each time step","Module",DT_Array1D);
	mdi.AddInput("QI","","Interflow at each reach outlet and at each time step","Module",DT_Array1D);
	mdi.AddInput("QG","","Groundwater discharge at each reach outlet and at each time step","Module",DT_Array1D);
	mdi.AddInput("CHST", "m3", "channel storage", "Module",DT_Array1D);
	//mdi.AddInput("SEEPAGE", "m3", "seepage", "Module",DT_Array1D);
	mdi.AddInput("CHWTDEPTH", "m", "channel water depth", "Module",DT_Array1D);
	//mdi.AddInput("C_WABA","","Channel water balance in a text format for each reach and at each time step","Module",DT_Array2D);

	mdi.AddOutput("SEDRECH","ton","Sediment in a text format at each reach outlet and at each time step",DT_Array1D);
	mdi.AddOutput("T_CHSB","","Channel sediment balance in a text format for each reach and at each time step",DT_Array2D);
	mdi.AddOutput("SEDOUTLET", "ton", "sediment concentration at the watershed outlet", DT_Single);
	//mdi.AddOutput("DEPOUTLET", "ton", "sediment concentration at the watershed outlet", DT_Single);

	// set the dependencies
	mdi.AddDependency("MUSLE_AS","MUSLE_AS module");   
	mdi.AddDependency("MUSK_CH","MUSK_CH module");   	//for module GWA_RE to update groundwater storage

	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}