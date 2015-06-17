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
#include "DiffusiveWave.h"
#include <iostream>
#include "SimulationModule.h"
#include "MetadataInfo.h"
#include "MetadataInfoConst.h"
/** \defgroup Hydrology
 * \brief Hydrology modules, mainly includes storm modules
 *
 *
 *
 */
/** \defgroup CH_DW
 * \ingroup Hydrology
 * \brief 
 *
 *
 *
 */
//! Get instance of SimulationModule class
extern "C" SEIMS_MODULE_API SimulationModule* GetInstance()
{
	return new DiffusiveWave();
}
/*!
 * \ingroup CH_DW
 * \brief function to return the XML Metadata document string
 *
 *   
 * 
 */
extern "C" SEIMS_MODULE_API const char* MetadataInformation()
{
	string res = "";
	MetadataInfo mdi;

	// set the information properties
	mdi.SetAuthor("Junzhi Liu");
	mdi.SetClass("Channel routing", "Channel routing.");
	mdi.SetDescription("Channel routing using diffusive wave equation.");
	mdi.SetEmail("SEIMS2015@163.com");
	mdi.SetHelpfile("CH_DW.chm");
	mdi.SetID("CH_DW");
	mdi.SetName("CH_DW");
	mdi.SetVersion("0.1");
	mdi.SetWebsite("http://seims.github.io/SEIMS");

	mdi.AddParameter("DT_HS", "second", "Time step of the simulation", "file.in", DT_Single); /// \param[in] DT_HS "Time step of the simulation from file.in, DT_Single"
	mdi.AddParameter("CellWidth", "m", "Cell width", "file.in", DT_Single); /// \param[in] CellWidth "Cell width from file.in, DT_Single"
	
	//mdi.AddParameter("ID_UPREACH", "", "the cell adjacent to channel of the upstream subbasin", "main", DT_Single);
	//mdi.AddParameter("Q_UPREACH", "m2/s", "channel flow in discharge from the upstream subbasin", "main", DT_Single);	
	mdi.AddParameter("DEM","m","Elevation","Parameter", DT_Raster);
	mdi.AddParameter("Slope", "%", "Slope", "ParameterDB_WaterBalance", DT_Raster); 
	mdi.AddParameter("FLOW_DIR", "", "Flow direction  by the rule of ArcGIS", "ParameterDB_Discharge", DT_Raster);
	mdi.AddParameter("CHWIDTH", "m", "Channel width", "ParameterDB_Discharge", DT_Raster); 
	// reach information
	//mdi.AddParameter("Reach_Layers", "", "Routing layers according to the flow direction"
	//	"There are not flow relationships within each layer, and the first element in each layer is the number of cells in the layer", "subbasins.shp", DT_Array2D);
	//mdi.AddParameter("Reach_Links", "", "id of cells in reach links", "streamlink.txt", DT_Array2D);
	//mdi.AddParameter("Reach_UpStream", "", "id of reach upstream", "subbasin.shp", DT_Array1D);
	//mdi.AddParameter("Manning_nCh", "", "Manning's n of channel cells", "manning.txt", DT_Array1D);
	mdi.AddParameter("FLOWOUT_INDEX_D8", "", "The index of flow in cell in the compressed array", "ParameterDB_Discharge", DT_Array1D);
	mdi.AddParameter("FLOWIN_INDEX_D8", "", "The index of flow in cell in the compressed array,"
		" and the first element in each sub-array is the number of flow in cells in this sub-array", "ParameterDB_Discharge", DT_Array2D);
	mdi.AddParameter("STREAM_LINK", "", "Stream link (id of reaches)", "ParameterDB_Discharge", DT_Raster);
	mdi.AddParameter("ReachParameter", "", "Reach parameters such stream order, manning's n and downstream subbasin id", "ParameterDB_Discharge", DT_Array2D);

	// from other module
	//mdi.AddInput("D_ID_OUTLET", "", "ID of watershed outlet", "Module", DT_Single);
	mdi.AddInput("D_QOverland","m3/s","discharge added to channel flow from overland flow","Module", DT_Raster);
	mdi.AddInput("D_QSoil","m3/s","discharge added to channel flow from interflow","Module", DT_Raster);
	mdi.AddInput("D_P","mm","Precipitation","Module", DT_Raster);

	// output
	mdi.AddOutput("QCH", "m3/s", "Flux in the downslope boundary of cells", DT_Array2D); /// \param[out] QCH "Flux in the downslope boundary of cells"
	mdi.AddOutput("QOUTLET", "m3/s", "discharge at the watershed outlet", DT_Single);
	mdi.AddOutput("QSUBBASIN", "m3/s", "discharge at the subbasin outlet", DT_Array1D);
	mdi.AddOutput("HCH", "mm", "Water depth in the downslope boundary of cells", DT_Array2D);

	//test output
	mdi.AddOutput("CHWATH", "mm", "Water depth in the downslope boundary of cells", DT_Raster);
	mdi.AddOutput("CHQCH", "m3/s", "Flux in the downslope boundary of cells", DT_Raster);

	res = mdi.GetXMLDocument();

	char* tmp = new char[res.size()+1];
	strprintf(tmp, res.size()+1, "%s", res.c_str());
	return tmp;
}