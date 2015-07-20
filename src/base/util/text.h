/*!
 * \file text.h
 * \brief Predefined of some const string names.
 *
 *
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date June 2010
 *
 * 
 */
#ifndef TEXT_H
#define TEXT_H

// string constants used in the code
// TODO - you may consider moving these to an external resource file 
//        to make it easier to change them later 
#define App_Config_File "app.config"

//for contant input variables
#define Contant_Input_Elevation						"Elevation"
#define Contant_Input_Latitude						"Latitude"
#define Contant_Input_Xpr							"xpr"
#define Contant_Input_Ypr							"ypr"
//#define Contant_Input_FlowdiversionProperty			"FlowDiversion_property"
//#define Contant_Input_PointsourceProperty			"PointSource_property"
//#define Contant_Input_ReservoirProperty				"Reservoir_property"
//#define Contant_Input_ReservoirRatingCurve			"Reservoir_RatingCurve"
//#define Contant_Input_ReservoirOperationSchedual	"Reservoir_OperationSchedual"

//for data type
#define DataType_Precipitation "P"					//1
#define DataType_MinimumTemperature "TMIN"			//2
#define DataType_MaximumTemperature "TMAX"			//3
#define DataType_PotentialEvapotranspiration "PET"	//4
#define DataType_SolarRadiation "SR"				//5
#define DataType_WindSpeed "WS"						//6
#define DataType_RelativeAirMoisture "RM"			//7
#define DataType_Meteorology "M"

#define Tag_Elevation "Elevation"
#define Tag_Configuration "Configuration"
#define Tag_ProjectsPath "ProjectsPath"
#define Tag_StationElevation "StationElevation"
#define Tag_Elevation_Meteorology "StationElevation_M"
#define Tag_Elevation_Precipitation "StationElevation_P"
#define Tag_Latitude_Meteorology "Latitude_M"
#define Tag_LapseRate "LapseRate"

#ifndef linux
    #define Tag_ModuleDirectoryName "\\"
    #define Tag_ModuleExt ".dll"
#else
    #define Tag_ModuleDirectoryName "/"
    #define Tag_ModuleExt ".so"
#endif

#define Tag_OutputID "OUTPUTID"
#define Tag_Interval "INTERVAL"
#define Tag_SiteCount "SITECOUNT"
#define Tag_SiteName "SITENAME"
#define Tag_SiteID "SITEID"
#define Tag_ReachName "REACHNAME"
#define Tag_StartTime "STARTTIME"
#define Tag_EndTime "ENDTIME"
#define Tag_FileName "FILENAME"
#define Tag_Type "TYPE"
#define Tag_Count "COUNT"
#define Tag_DataType "DATA_TYPE"
#define Tag_Weight "WEIGHT"
#define Tag_VerticalInterpolation "VERTICALINTERPOLATION"
#define Tag_SubbasinCount "SUBBASINCOUNT"
#define Tag_SubbasinId "SUBBASINID"
#define Tag_ReservoirCount "RESERVOIRCOUNT"
#define Tag_ReservoirId "RESERVOIRID"
#define Tag_SubbasinSelected "subbasinSelected"
#define Tag_CellSize "CELLSIZE"
#define Tag_Mask "MASK"
#define Tag_TimeStep "TimeStep"
#define Tag_StormTimeStep "DT_HS"
#define Tag_ChannelTimeStep "DT_CH"
#define Tag_CellWidth "CellWidth"
#define Tag_PStat "P_STAT"


// D8 Flow model
#define Tag_FLOWIN_INDEX "FLOWIN_INDEX_D8"
#define Tag_FLOWOUT_INDEX "FLOWOUT_INDEX_D8"
#define Tag_ROUTING_LAYERS "ROUTING_LAYERS"

// TODO: Dinf, MFD, MFD-md integrated into SEIMS.
// D-infinity Flow model after Tarboton et al.(1991)
#define Tag_FLOWIN_INDEX_DINF "FLOWIN_INDEX_DINF"
#define Tag_FLOWIN_PERCENTAGE_DINF "FLOWIN_PERCENTAGE_DINF"
#define Tag_FLOWOUT_INDEX_DINF "FLOWOUT_INDEX_DINF"
#define Tag_FLOWOUT_PERCENTAGE_DINF "FLOWOUT_PERCENTAGE_DINF"
#define Tag_ROUTING_LAYERS_DINF "ROUTING_LAYERS_DINF"
// Multi-Flow model after Quinn et al.(1991)
#define Tag_FLOWIN_INDEX_MFD "FLOWIN_INDEX_MFD"
#define Tag_FLOWIN_PERCENTAGE_MFD "FLOWIN_PERCENTAGE_MFD"
#define Tag_FLOWOUT_INDEX_MFD "FLOWOUT_INDEX_MFD"
#define Tag_FLOWOUT_PERCENTAGE_MFD "FLOWOUT_PERCENTAGE_MFD"
#define Tag_ROUTING_LAYERS_MFD "ROUTING_LAYERS_MFD"
//MFD-md flow model after Qin et al.(2007) (md means maximum downslope)
#define Tag_FLOWIN_INDEX_MFD_MD "FLOWIN_INDEX_MFD_MD"
#define Tag_FLOWIN_PERCENTAGE_MFD_MD "FLOWIN_PERCENTAGE_MFD_MD"
#define Tag_FLOWOUT_INDEX_MFD_MD "FLOWOUT_INDEX_MFD_MD"
#define Tag_FLOWOUT_PERCENTAGE_MFD_MD "FLOWOUT_PERCENTAGE_MFD_MD"
#define Tag_ROUTING_LAYERS_MFD_MD "ROUTING_LAYERS_MFD_MD"

#define Tag_ReachParameter "ReachParameter"
#define Tag_RchParam "RchParam"
#define Tag_QOUTLET "QOUTLET"
#define Tag_QTotal "QTotal"
#define Tag_SEDOUTLET "SEDOUTLET"
#define Tag_OL_IUH "OL_IUH"
#define Tag_DisPOutlet "DissovePOutlet"
#define Tag_AmmoOutlet "AmmoniumOutlet"
#define Tag_NitrOutlet "NitrateOutlet"
#define Tag_QSUBBASIN "QSUBBASIN"

#define ModID_PET_RD "PET_RD"
#define ModId_ITP_AU "ITP_AU"
#define ModId_PI_MSM "PI_MSM"

//for reach hydroclimate data
#define DIVERSION	"flowdiversion"
#define POINTSOURCE "pointsource"
#define RESERVOIR	"reservoir"
#define FLOW		"flow"
#define SDEIDMENT	"sediment"
#define ORGANICN	"organicN"
#define ORGANICP	"organicP"
#define NO3			"NO3"
#define NH3			"NH3"
#define NO2			"NO2"
#define MINERALP	"mineralP"

//! string constants used in the code
#define MapWindowRasterExtension ".asc"
#define RasterExtension ".asc"
#define GTiffExtension ".tif"
#define TextExtension ".txt"
#define File_HydroClimateDB "HydroClimate.db3"
#define File_ParameterDB "Parameter.db3"
#define File_BMPDB "BMP.db3"
#define Table_LapseRate "lapse_rate"
#define Table_WGN "wgt"
#define NAME_MASK "MASK"
#define File_Mask "MASK.asc"
#define File_DEM "dem.asc"

#define File_Config "config.fig"
#define File_Input "file.in"
#define File_Output "file.out"
#define Source_HydroClimateDB "HydroClimateDB"
#define Source_ParameterDB "ParameterDB"
#define Source_Module "Module"


//! define string constants used in the code
#define Tag_NoDataValue "NoDataValue"
#define Tag_SubbasinSelected "subbasinSelected"
#define Tag_Mode "MODE"

#define Type_Scenario "SCENARIO"
#define Type_LapseRateArray "LAPSERATEARRAY"
#define Type_SiteInformation "SITEINFORMATION"
#define Type_MapWindowRaster "MAPWINDOWRASTER"
#define Type_Array1DDateValue "ARRAY1DDATEVALUE"
#define Type_Array3D "ARRAY3D"
#define Type_Array2D "ARRAY2D"
#define Type_Array1D "ARRAY1D"
#define Type_Single "SINGLE"

#define Print_D_PREC "D_PREC"
#define Print_D_POET "D_POET"
#define Print_D_TEMP "D_TEMP"
#define Print_D_INLO "D_INLO"
#define Print_D_INET "D_INET"
#define Print_D_NEPR "D_NEPR"

#define ModID_PI_MSM "PI_MSM"
#define ModID_ITP_AU "ITP_AU"

#define PARAMETERS_TABLE "parameters"
#define LANDUSE_TABLE "LanduseLookup"
#define SOIL_TABLE "SoilLookup"
#define STATIONS_TABLE "stations"
#define TOPOLOGY_TABLE "reaches"
#define SPATIAL_TABLE "spatial.files"
#define MEASUREMENT_TABLE "measurement"

#define SITES_TABLE "Sites"
#define DATADAY_TABLE_PREFIX "DataValuesDay_"
#define SITELIST_TABLE "SiteList"
#define SITELIST_TABLE_STORM "StormSiteList"

#ifndef linux
#define CH_ROUTING_MODULE "IKW_CH"
#define SEP "\\"
#else
#define CH_ROUTING_MODULE "libIKW_CH"
#define SEP "/"
#endif

#endif

