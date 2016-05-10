/*!
 * \file text.h
 * \brief Predefined string constants used in the code
 
 * \author Junzhi Liu, LiangJun Zhu, Huiran Gao
 * \version 1.1
 * \date Jun. 2010, Apr.2016
 *
 * 
 */
#ifndef SEIMS_TEXT_H
#define SEIMS_TEXT_H

// 
// TODO - you may consider moving these to an external resource file 
//        to make it easier to change them later 

#define SEIMS_EMAIL "SEIMS2015@163.com"
#define SEIMS_SITE "http://seims.github.io/SEIMS"

//! Constant input variables
#define CONS_IN_ELEV						"Elevation"
#define CONS_IN_LAT						    "Latitude"
#define CONS_IN_XPR							"xpr"
#define CONS_IN_YPR							"ypr"

//! Climate data type, used in SEIMS_ModuleSetting class, ITP, etc. // LJ, Apr.,2016
#define DataType_Precipitation "P"					//1, Suffix of precipitation data
#define DataType_MeanTemperature "TMEAN"      	    //2
#define DataType_MinimumTemperature "TMIN"			//3
#define DataType_MaximumTemperature "TMAX"			//4
#define DataType_PotentialEvapotranspiration "PET"	//5
#define DataType_SolarRadiation "SR"				//6
#define DataType_WindSpeed "WS"						//7
#define DataType_RelativeAirMoisture "RM"			//8
#define DataType_Meteorology "M"                    // Suffix of meteorology data
#define DataType_Prefix_TS "T"                      // Prefix of time series data
#define DataType_Prefix_DIS "D"                     // Prefix of distributed data

//! Tags
#define Tag_Configuration "Configuration"
#define Tag_DEM "DEM"
#define Tag_Elevation_Meteorology "StationElevation_M"
#define Tag_Elevation_PET "StationElevation_PET"
#define Tag_Elevation_Precipitation "StationElevation_P"
#define Tag_Elevation_Temperature "StationElevation_T"
#define Tag_LapseRate "LapseRate"
#define Tag_Latitude_Meteorology "Latitude_M"
#define Tag_ProjectsPath "ProjectsPath"
#define Tag_StationElevation "StationElevation"
#define Tag_VerticalInterpolation "VERTICALINTERPOLATION"
#define Tag_Weight "WEIGHT"

///////  define parameter-related string constants used in the SEIMS ///////
#define PARAM_USE_Y			"Y"
#define PARAM_USE_N			"N"
#define PARAM_CHANGE_RC		"RC"
#define PARAM_CHANGE_AC		"AC"
#define PARAM_CHANGE_NC		"NC"
#define PARAM_FLD_NAME		"NAME"
#define PARAM_FLD_DESC		"DESCRIPTION"
#define PARAM_FLD_UNIT		"UNIT"
#define PARAM_FLD_MIDS		"MODULE"
#define PARAM_FLD_VALUE		"VALUE"
#define PARAM_FLD_IMPACT	"IMPACT"
#define PARAM_FLD_CHANGE	"CHANGE"
#define PARAM_FLD_MAX		"MAX"
#define PARAM_FLD_MIN		"MIN"
#define PARAM_FLD_USE		"USE"


#ifndef linux
    #define Tag_ModuleDirectoryName "\\"
	#define SEP "\\"
    #define Tag_ModuleExt ".dll"
#else
    #define Tag_ModuleDirectoryName "/"
	#define SEP "/"
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


#define Tag_SubbasinCount "SUBBASINCOUNT"
#define Tag_SubbasinId "SubbasinID"
#define Tag_ReservoirCount "RESERVOIRCOUNT"
#define Tag_ReservoirId "RESERVOIRID"
#define Tag_SubbasinSelected "subbasinSelected"
#define Tag_CellNUM "CELLNUM" /// by LJ, have not been tested!
#define Tag_Mask "MASK"
#define Tag_TimeStep "TimeStep"
#define Tag_StormTimeStep "DT_HS"
#define Tag_ChannelTimeStep "DT_CH"
#define Tag_CellWidth "CELLSIZE"
#define Tag_PStat "P_STAT"


// D8 Flow model
#define Tag_FLOWIN_INDEX "FLOWIN_INDEX_D8"
#define Tag_FLOWOUT_INDEX "FLOWOUT_INDEX_D8"
#define Tag_ROUTING_LAYERS "ROUTING_LAYERS"

// TODO: Dinf, MFD, MFD-md integrated into SEIMS.
//! D-infinity Flow model after Tarboton et al.(1991)
#define Tag_FLOWIN_INDEX_DINF "FLOWIN_INDEX_DINF"
#define Tag_FLOWIN_PERCENTAGE_DINF "FLOWIN_PERCENTAGE_DINF"
#define Tag_FLOWOUT_INDEX_DINF "FLOWOUT_INDEX_DINF"
#define Tag_FLOWOUT_PERCENTAGE_DINF "FLOWOUT_PERCENTAGE_DINF"
#define Tag_ROUTING_LAYERS_DINF "ROUTING_LAYERS_DINF"
//! Multi-Flow model after Quinn et al.(1991)
#define Tag_FLOWIN_INDEX_MFD "FLOWIN_INDEX_MFD"
#define Tag_FLOWIN_PERCENTAGE_MFD "FLOWIN_PERCENTAGE_MFD"
#define Tag_FLOWOUT_INDEX_MFD "FLOWOUT_INDEX_MFD"
#define Tag_FLOWOUT_PERCENTAGE_MFD "FLOWOUT_PERCENTAGE_MFD"
#define Tag_ROUTING_LAYERS_MFD "ROUTING_LAYERS_MFD"
//! MFD-md flow model after Qin et al.(2007) (md means maximum downslope)
#define Tag_FLOWIN_INDEX_MFD_MD "FLOWIN_INDEX_MFD_MD"
#define Tag_FLOWIN_PERCENTAGE_MFD_MD "FLOWIN_PERCENTAGE_MFD_MD"
#define Tag_FLOWOUT_INDEX_MFD_MD "FLOWOUT_INDEX_MFD_MD"
#define Tag_FLOWOUT_PERCENTAGE_MFD_MD "FLOWOUT_PERCENTAGE_MFD_MD"
#define Tag_ROUTING_LAYERS_MFD_MD "ROUTING_LAYERS_MFD_MD"

#define Tag_ReachParameter "ReachParameter"
#define Tag_RchParam "RchParam"

////////////  Output Tags   ///////////////
//// Output aggregation type //////
#define Tag_Unknown			"UNKNOWN" 
#define Tag_Sum				"SUM"
#define Tag_Average			"AVERAGE"
#define Tag_Average2		"AVE"
#define Tag_Minimum			"MIN"
#define Tag_Maximum			"MAX"
#define Tag_SpecificCells	"SPECIFIC"

#define TAG_OUT_QOUTLET "QOUTLET"
#define TAG_OUT_QTOTAL "QTotal"
#define TAG_OUT_SEDOUTLET "SEDOUTLET"
#define TAG_OUT_OL_IUH "OL_IUH"
#define Tag_DisPOutlet "DissovePOutlet"
#define Tag_AmmoOutlet "AmmoniumOutlet"
#define Tag_NitrOutlet "NitrateOutlet"
#define TAG_OUT_QSUBBASIN "QSUBBASIN"



//!for reach hydro climate data
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

/// Files or database constant strings
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

#define File_Config				"config.fig"
#define File_Input				"file.in"
#define File_Output				"file.out"
#define Source_HydroClimateDB	"HydroClimateDB"
#define Source_ParameterDB		"ParameterDB"
#define Source_Module			"Module"

///////// Table Names required in MongoDB /////////
#define DB_TAB_PARAMETERS		"parameters"
#define DB_TAB_LOOKUP_LANDUSE	"LanduseLookup"
#define DB_TAB_LOOKUP_SOIL		"SoilLookup"
#define DB_TAB_SITELIST			"SiteList"
#define DB_TAB_REACH			"reaches"
#define DB_TAB_SPATIAL			"spatial"  /// spatial.files
#define DB_TAB_SITES			"Sites"
#define DB_TAB_DATAVALUES		"DataValues" // hydroClimate data values
#define DB_TAB_MEASUREMENT		"measurement" // TODO: discharge, nutrient etc. should be stored in this collection

#define DB_TAB_OUT_SPATIAL		"output"

/// Fields in DB_TAB_REACH ///
#define REACH_SUBBASIN			"SUBBASIN"
#define REACH_GROUPDIVIDED		"GROUP_DIVIDE"
#define REACH_DOWNSTREAM		"DOWNSTREAM"
#define REACH_UPDOWN_ORDER		"UP_DOWN_ORDER"
#define REACH_DOWNUP_ORDER		"DOWN_UP_ORDER"
#define REACH_WIDTH				"WIDTH"
#define REACH_LENGTH			"LENGTH"
#define REACH_DEPTH				"DEPTH"
#define REACH_V0				"V0"
#define REACH_AREA				"AREA"
#define REACH_MANNING			"MANNING"
#define REACH_SLOPE				"SLOPE" 

/// these four are defined in DB_TAB_SITELIST in Source_ParameterDB
#define SITELIST_TABLE_M		"SiteListM"
#define SITELIST_TABLE_P		"SiteListP"
#define SITELIST_TABLE_PET		"SiteListPET"
#define SITELIST_TABLE_STORM	"StormSiteList"

//! define string constants used in the code, also used in the mongoDB.SiteList table's header
#define Tag_NoDataValue			"NoDataValue"
#define Tag_SubbasinSelected	"subbasinSelected"
#define Tag_Mode				"Mode"
#define Tag_Mode_Storm			"STORM"
#define Tag_Mode_Daily			"DAILY"

#define Type_Scenario			"SCENARIO"
#define Type_LapseRateArray		"LAPSERATEARRAY"
#define Type_SiteInformation	"SITEINFORMATION"
#define Type_MapWindowRaster	"MAPWINDOWRASTER"
#define Type_Array1DDateValue	"ARRAY1DDATEVALUE"
#define Type_Array3D			"ARRAY3D"
#define Type_Array2D			"ARRAY2D"
#define Type_Array1D			"ARRAY1D"
#define Type_Single				"SINGLE"

//// Optional Output Data ////
#define Print_D_PREC "D_PREC"
#define Print_D_POET "D_POET"
#define Print_D_TEMP "D_TEMP"
#define Print_D_INLO "D_INLO"
#define Print_D_INET "D_INET"
#define Print_D_NEPR "D_NEPR"

/// 2D Soil Attribute Data ///
#define Print_2D_CONDUCTIVITY	"Conductivity_2D"
#define Print_2D_POROSITY		"Porosity_2D"
#define Print_2D_POREINDEX		"Poreindex_2D"
#define Print_2D_FIELDCAP		"FieldCap_2D"
#define Print_2D_WILTINGPOINT	"WiltingPoint_2D"
#define Print_2D_DENSITY		"Density_2D"

//////////////////////////////////////////////////////////////////////////
/// Define models' ID and description in SEIMS  //////////////////////////
/// By LiangJun Zhu, Apr. 26, 2016  //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// Climate related Modules
#define MCLS_CLIMATE "Climate Variables"
#define MCLSDESC_CLIMATE "Calculate climate related intermediate parameters."
#define MID_ICLIM "ICLIM"
#define MDESC_ICLIM "Calculate climate related intermediate parameters."
#define MCLS_TSD "Time series data"
#define MID_TSD_RD "TSD_RD"
#define MDESC_TSD_RD "Read time series data from HydroClimate database."
#define MCLS_ITP "Interpolation module"
#define MID_ITP "ITP"
#define MDESC_ITP "Interpolation of P, E, PET, etc."

/// Potential Evapotranspiration Modules
#define MCLS_PET "Potential Evapotranspiration"
#define MCLSDESC_PET "Calculate the potential evapotranspiration for an array of climate inputs."
#define MID_PET_H "PET_H"
#define MDESC_PET_H "Hargreaves method for calculating the potential evapotranspiration."
#define MID_PET_PT "PET_PT"
#define MDESC_PET_PT "PriestleyTaylor method for calculating the potential evapotranspiration."
#define MID_PET_PM "PET_PM"
#define MDESC_PET_PM "Penman Monteith method for calculating the potential evapotranspiration."


//////////////////////////////////////////////////////////////////////////
/// Define unit names common used in SEIMS, in case of inconsistency /////
/// By LiangJun Zhu, Apr. 25, 2016  //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define VAR_CO2 "Co2"                               /// CO2 Concentration
#define VAR_COND_MAX "Cond_max"                     /// Maximum stomatal conductance
#define VAR_COND_RATE "Cond_rate"                   /// Rate of decline in stomatal conductance per unit increase in vapor pressure deficit
#define VAR_DEM "DEM"                               /// Digital Elevation Model
#define VAR_JULIAN_DAY "JDay"                       /// Julian day (int)
#define VAR_LAP_RATE "LapseRate"                    /// Lapse rate
#define VAR_OMP_THREADNUM "ThreadNum"               /// Thread numbers for OMP
#define VAR_PET_HCOEF "HCoef_pet"                   /// Coefficient related to radiation used in Hargreaves method
#define VAR_PET_K "K_pet"                           /// Correction factor for PET
#define VAR_PET_T "T_PET"                           /// Potential Evapotranspiration of day
#define VAR_VP_SAT "svp"                            /// Saturated vapor pressure
#define VAR_VP_ACT "avp"                            /// actual vapor pressure
#define VAR_SNOW_TEMP "T_snow"                      /// Snowfall temperature
#define VAR_SR_MAX "srMax"                          /// Max solar radiation
#define VAR_TSD_DT "data_type"                      /// Time series data type


//////////////////////////////////////////////////////////////////////////
/// Define units common used in SEIMS, in case of inconsistency //////////
/// By LiangJun Zhu, Apr. 25, 2016  //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define UNIT_AREA_KM2 "km2"                         /// Square kilometer of area
#define UNIT_CONDRATE_MSPA "m/s/kPa"                /// Rate of decline in stomatal conductance per unit increase in vapor pressure deficit
#define UNIT_CONT_KGKM2 "kg/km2"                    /// Kilograms per Square kilometers of nutrient content
#define UNIT_DEPTH_MM "mm"                          /// Depth related unit, mm
#define UNIT_FLOW_CMS "m3/s"                        /// Cubic meters per second of flow discharge
#define UNIT_GAS_PPMV "ppmv"                        /// Concentration of gas, e.g., CO2
#define UNIT_LAP_RATE "/100m"                       /// Lapse rate
#define UNIT_LEN_M "m"                              /// Meter of length
#define UNIT_LONLAT_DEG "degree"                    /// Degree of longitude and latitude
#define UNIT_NON_DIM ""                             /// Non dimension  
#define UNIT_PRESSURE "kPa"                         /// Vapor pressure
#define UNIT_SPEED_MS "m/s"                         /// Speed related
#define UNIT_SR "MJ/m2/d"                           /// Solar Radiation
#define UNIT_TEMP_DEG "deg C"                       /// Celsius degree of air temperature 
#define UNIT_WTRDLT_MMD "mm/d"                      /// Millimeter per day of water changes

//////////////////////////////////////////////////////////////////////////
/// Define description of units common used in SEIMS            //////////
/// By LiangJun Zhu, Apr. 25, 2016  //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define DESC_CO2 "CO2 Concentration"
#define DESC_CONDRATE "Rate of decline in stomatal conductance per unit increase in vapor pressure deficit"
#define DESC_DEM "Digital Elevation Model"
#define DESC_JULIAN_DAY "Julian day (int)"
#define DESC_LAP_RATE "Lapse Rate"
#define DESC_MAXCOND "Maximum stomatal conductance"
#define DESC_MAXTEMP "Maximum Celsius degree of air temperature"
#define DESC_MEANTEMP "Mean Celsius degree of air temperature"
#define DESC_METEOLAT "Latitude of MeteoClimate station"
#define DESC_MINTEMP "Minimum Celsius degree of air temperature"
#define DESC_NONE ""
#define DESC_PET_HCOEF "Coefficient related to radiation used in Hargreaves method"
#define DESC_PET_K "Correction factor for PET"
#define DESC_PET_T "Potential Evapotranspiration of day"
#define DESC_RM "Relative humidity"
#define DESC_SNOW_TEMP "Snowfall temperature"
#define DESC_SR "Solar radiation"
#define DESC_SR_MAX "Max solar radiation"
#define DESC_TSD_DT "Time series data type, e.g., climate data"
#define DESC_TSD_CLIMATE "Climate data of all the stations"
#define DESC_VER_ITP "Execute vertical interpolation (1) or not (0), defined in config.fig"
#define DESC_VP_SAT "Saturated vapor pressure"
#define DESC_VP_ACT "actual vapor pressure"
#define DESC_WEIGHT_ITP "Weight used for interpolation"
#define DESC_WS "Wind speed (measured at 10 meters above surface)"



//////////////////////////////////////////////////////////////////////////
/// Define MongoDB related constant strings used in SEIMS and preprocess//
/// By LiangJun Zhu, May. 4, 2016  ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define MONG_GRIDFS_FN				"filename"
#define MONG_GRIDFS_WEIGHT_CELLS	"NUM_CELLS"
#define MONG_GRIDFS_WEIGHT_SITES	"NUM_SITES"
#define MONG_GRIDFS_ID				"ID"
#define MONG_GRIDFS_SUBBSN			"SUBBASIN"
#define MONG_HYDRO_SITE_TYPE		"Type"
#define MONG_HYDRO_SITE_LAT			"Lat"
#define MONG_HYDRO_SITE_ELEV		"Elevation"
#define MONG_HYDRO_DATA				"DataValues"
#define MONG_HYDRO_DATA_SITEID		"StationID"
#define MONG_HYDRO_DATA_UTC			"UTCDateTime"
#define MONG_HYDRO_DATA_LOCALT		"LocalDateTime"
#define MONG_HYDRO_DATA_VALUE		"Value"
#define MONG_SITELIST_SUBBSN		"SubbasinID"
#define MONG_SITELIST_DB			"DB"


//////////////////////////////////////////////////////////////////////////
/// Define Raster/ related constant strings used in SEIMS and preprocess//
/// By LiangJun Zhu, May. 5, 2016  ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define HEADER_RS_TAB	    "Header"
#define HEADER_RS_NODATA	"NODATA_VALUE"
#define HEADER_RS_XLL		"XLLCENTER"
#define HEADER_RS_YLL		"YLLCENTER"
#define HEADER_RS_NROWS		"NROWS"
#define HEADER_RS_NCOLS		"NCOLS"
#define HEADER_RS_CELLSIZE	"CELLSIZE"
#endif