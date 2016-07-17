#! /usr/bin/env python
# coding=utf-8
## @ Constant strings used in SEIMS, means both in
##   data preprocessing and SEIMS modules
# Author: Liang-Jun Zhu
#

## Simulation Model related tages
Tag_Model = "model"
Tag_Cluster = "cluster"
Tag_Mode = "MODE"
Tag_Mode_Storm = "STORM"
Tag_Mode_Daily = "DAILY"
Tag_CLIM_STORM_Suf = "storm_cmorph"

## Names for folders in output workspace for Spatial data preprocessing
DIR_NAME_TAUDEM = "taudir"
DIR_NAME_REACH = "reaches"
DIR_NAME_SUBBSN = "subbasins"
DIR_NAME_LAYERINFO = "layering_info"
DIR_NAME_METISOUT = "metis_output"
DIR_NAME_TIFFIMPORT = "tif_files"
DIR_NAME_LOOKUP = "lookup"
## File names
FILE_IN = "file.in"
FILE_OUT = "file.out"
## Regenerated SQLite database of Parameters for SEIMS
Tag_Params = "param"
Tag_Lookup = "lookup"
init_params = 'model_param_ini'
lookup_tabs = ['SoilLookup', 'LanduseLookup', 'TillageLookup',
               'UrbanLookup', 'CropLookup', 'FertilizerLookup']
CROP_FILE = 'CropLookup.txt'
sqliteFile = "Parameter.db3"

### CROP, LANDUSE, and SOIL attribute are imported to mongoDB
### Match to the new lookup table of SWAT 2012 rev.637. LJ
CROP_ATTR_LIST = ["IDC", "BIO_E", "HVSTI", "BLAI", "FRGRW1", "LAIMX1", "FRGRW2",
                  "LAIMX2", "DLAI", "CHTMX", "RDMX", "T_OPT", "T_BASE", "CNYLD",
                  "CPYLD", "BN1", "BN2", "BN3", "BP1", "BP2", "BP3", "WSYF",
                  "USLE_C", "GSI", "VPDFR", "FRGMAX", "WAVP", "CO2HI", "BIOEHI",
                  "RSDCO_PL", "OV_N", "CN2A", "CN2B", "CN2C", "CN2D", "FERTFIELD",
                  "ALAI_MIN", "BIO_LEAF", "MAT_YRS", "BMX_TREES", "EXT_COEF", "BM_DIEOFF",
                  "PLTHU", "CNOP", "LAI_INIT", "BIO_INIT", "CURYR_INIT"]

### USLE_C is extracted from cropLookup database
LANDUSE_ATTR_LIST = ["CN2A", "CN2B", "CN2C", "CN2D", "ROOTDEPTH", "MANNING",
                     "INTERC_MAX", "INTERC_MIN", "SHC", "SOIL_T10",
                     "PET_FR", "PRC_ST1", "PRC_ST2", "PRC_ST3", "PRC_ST4",
                     "PRC_ST5", "PRC_ST6", "PRC_ST7", "PRC_ST8", "PRC_ST9",
                     "PRC_ST10", "PRC_ST11", "PRC_ST12", "SC_ST1", "SC_ST2",
                     "SC_ST3", "SC_ST4", "SC_ST5", "SC_ST6", "SC_ST7", "SC_ST8",
                     "SC_ST9", "SC_ST10", "SC_ST11", "SC_ST12", "DSC_ST1", "DSC_ST2",
                     "DSC_ST3", "DSC_ST4", "DSC_ST5", "DSC_ST6", "DSC_ST7", "DSC_ST8",
                     "DSC_ST9", "DSC_ST10", "DSC_ST11", "DSC_ST12"]

## Metadata field names
META_LOOKUP_ITEM_COUNT = 'ITEM_COUNT'
META_LOOKUP_FIELD_COUNT = 'FIELD_COUNT'

## SOIL PARAMETERS NAMES, which will be appeared in MongoDB
SOL_SEQN = "SEQN"
SOL_NAME = "SNAM"
## required inputs
SOL_NLYRS = "SoilLAYERS"
SOL_Z = "soildepth"
SOL_OM = "om"
SOL_CLAY = "clay"
SOL_SILT = "silt"
SOL_SAND = "sand"
SOL_ROCK = "rock"
## optional inputs, which can be auto-calculated
SOL_ZMX = "sol_zmx"
SOL_ANIONEXCL = "anion_excl"
SOL_CRK = "sol_crk"
SOL_BD = "density"
SOL_K = "conductivity"
SOL_WP = "wiltingpoint"
SOL_FC = "fieldcap"
SOL_AWC = "sol_awc"
SOL_POROSITY = "porosity"
SOL_P_INDEX = "poreindex"
SOL_USLE_K = "usle_k"
SOL_ALB = "sol_alb"
# SOL_RM      = "Residual"
SOL_TEXTURE = "soil_texture"
## soil chemical properties

## Climate datatype tags, MUST BE coincident with text.h in SEIMS
## /src/base/util/text.h
## BE AWARE: TMIN, TMAX, WS, Residual is required, and TMEAN, PET, SR is optional
##           WHEN SR is not provided, the SSD MUST be there!

DataType_Precipitation = "P"  # 1, Suffix of precipitation data
DataType_MeanTemperature = "TMEAN"  # 2
DataType_MinimumTemperature = "TMIN"  # 3
DataType_MaximumTemperature = "TMAX"  # 4
DataType_PotentialEvapotranspiration = "PET"  # 5
DataType_SolarRadiation = "SR"  # 6
DataType_WindSpeed = "WS"  # 7
DataType_RelativeAirMoisture = "RM"  # 8
DataType_SunDurationHour = "SSD"  # 9
DataType_YearlyHeatUnit = "PHUTOT"
Datatype_PHU0 = "PHU0"
DataType_MeanTemperature0 = "TMEAN0"

DataType_Meteorology = "M"  # Suffix of meteorology data
DataType_Prefix_TS = "T"  # Prefix of time series data
DataType_Prefix_DIS = "D"  # Prefix of distributed data
Tag_DT_StationID = 'StationID'
Tag_DT_Year = 'Y'
Tag_DT_Month = 'M'
Tag_DT_Day = 'D'
Tag_DT_Type = 'Type'
Tag_DT_LocalT = 'LocalDateTime'
Tag_DT_Zone = 'UTCOffset'
Tag_DT_UTC = 'UTCDateTime'
Tag_DT_Value = 'Value'

Tag_ClimateDB_ANNUAL_STATS = 'Annual_Stats'
Tag_ClimateDB_Measurement = 'Measurement'
Tag_ClimateDB_VARs = 'Variables'
Tag_VAR_ID = 'ID'
Tag_VAR_Type = 'Type'
Tag_VAR_UNIT = 'Unit'
Tag_VAR_IsReg = 'IsRegular'
Tag_VAR_Time = 'TimeSupport'

Tag_ClimateDB_Sites = "Sites"
Tag_ST_StationID = 'StationID'
Tag_ST_StationName = 'Name'
Tag_ST_LocalX = 'LocalX'
Tag_ST_LocalY = 'LocalY'
Tag_ST_LocalPrjID = 'LocalPrjID'
Tag_ST_Longitude = 'Lon'
Tag_ST_Latitude = 'Lat'
Tag_ST_DatumID = 'DatumID'
Tag_ST_Elevation = 'Elevation'
Tag_ST_Type = 'Type'
Tag_ST_IsOutlet = 'isOutlet'
Tag_ST_UNIT = 'Unit'

## Table Names required in MongoDB
DB_TAB_PARAMETERS = "parameters"
DB_TAB_LOOKUP_LANDUSE = "LanduseLookup"
DB_TAB_LOOKUP_SOIL = "SoilLookup"
DB_TAB_SPATIAL = "spatial"
DB_TAB_SITES = "Sites"
DB_TAB_DATAVALUES = "Data_Values"
DB_TAB_MEASUREMENT = "Measurements"
DB_TAB_SITELIST = "SiteList"
DB_TAB_REACH = "reaches"
DB_TAB_BMP_DB = "BMPDatabase"
DB_TAB_FILE_IN = "FILE_IN"
DB_TAB_FILE_OUT = "FILE_OUT"
### Fields in DB_TAB_REACH
REACH_SUBBASIN = "SUBBASIN"
REACH_NUMCELLS = "NUM_CELLS"
REACH_GROUP = "GROUP"
REACH_GROUPDIVIDED = "GROUP_DIVIDE"
REACH_DOWNSTREAM = "DOWNSTREAM"
REACH_UPDOWN_ORDER = "UP_DOWN_ORDER"
REACH_DOWNUP_ORDER = "DOWN_UP_ORDER"
REACH_WIDTH = "WIDTH"
REACH_LENGTH = "LENGTH"
REACH_DEPTH = "DEPTH"
REACH_V0 = "V0"
REACH_AREA = "AREA"
REACH_MANNING = "MANNING"
REACH_SLOPE = "SLOPE"
REACH_KMETIS = 'GROUP_KMETIS'
REACH_PMETIS = 'GROUP_PMETIS'
REACH_BC1 = 'BC1'
REACH_BC2 = 'BC2'
REACH_BC3 = 'BC3'
REACH_BC4 = 'BC4'
REACH_RK1 = 'RK1'
REACH_RK2 = 'RK2'
REACH_RK3 = 'RK3'
REACH_RK4 = 'RK4'
REACH_RS1 = 'RS1'
REACH_RS2 = 'RS2'
REACH_RS3 = 'RS3'
REACH_RS4 = 'RS4'
REACH_RS5 = 'RS5'

# Field in SiteList table or other tables, such as subbasin.shp
FLD_SUBBASINID = 'SUBBASINID'
FLD_BASINID = 'BASIN'
FLD_MODE = 'MODE'
FLD_DB = 'DB'

# Field in Model Configuration Collections, FILE_IN and FILE_OUT
FLD_CONF_TAG = "TAG"
FLD_CONF_VALUE = "VALUE"
FLD_CONF_MODCLS = "MODULE_CLASS"
FLD_CONF_OUTPUTID = "OUTPUTID"
FLD_CONF_DESC = "DESCRIPTION"
FLD_CONF_UNIT = "UNIT"
FLD_CONF_TYPE = "TYPE"
FLD_CONF_STIME = "STARTTIME"
FLD_CONF_ETIME = "ENDTIME"
FLD_CONF_INTERVAL = "INTERVAL"
FLD_CONF_INTERVALUNIT = "INTERVAL_UNIT"
FLD_CONF_SUBBSN = "SUBBASIN"
FLD_CONF_FILENAME = "FILENAME"
FLD_CONF_USE = "USE"


FLD_LINKNO = "LINKNO"
FLD_DSLINKNO = "DSLINKNO"

BMP_FLD_PT_DISTDOWN = "DIST2REACH"

## Field metadata in MongoDB

## Spatial Data related string or values
## intermediate data files' names
filledDem = "demFilledTau.tif"
flowDir = "flowDirTauD8.tif"
slope = "slopeTau.tif"
acc = "accTauD8.tif"
accWithWeight = "accTauD8WithWeight.tif"
streamRaster = "streamRasterTau.tif"

flowDirDinf = "flowDirDinfTau.tif"
dirCodeDinf = "dirCodeDinfTau.tif"
slopeDinf = "slopeDinfTau.tif"
weightDinf = "weightDinfTau.tif"
cellLat = "cellLatOrg.tif"
daylMin = "dayLenMinOrg.tif"
dormhr = "dormhrOrg.tif"

modifiedOutlet = "outletM.shp"
streamSkeleton = "streamSkeleton.tif"
streamOrder = "streamOrderTau.tif"
chNetwork = "chNetwork.txt"
chCoord = "chCoord.txt"
streamNet = "streamNet.shp"
dist2StreamD8 = "dist2StreamD8Org.tif"
subbasin = "subbasinTau.tif"
mask_to_ext = "mask.tif"
## masked file names
subbasinM = "subbasinTauM.tif"
flowDirM = "flowDirTauM.tif"
streamRasterM = "streamRasterTauM.tif"
## output to mongoDB file names
reachesOut = "reach.shp"
subbasinOut = "subbasin.tif"
flowDirOut = "flow_dir.tif"
streamLinkOut = "stream_link.tif"
## masked and output to mongoDB file names
slopeM = "slope.tif"
filldemM = "dem.tif"
accM = "acc.tif"
streamOrderM = "stream_order.tif"
flowDirDinfM = "flow_dir_angle_dinf.tif"
dirCodeDinfM = "flow_dir_dinf.tif"
slopeDinfM = "slope_dinf.tif"
weightDinfM = "weight_dinf.tif"
cellLatM = "celllat.tif"
daylMinM = "dayLenMin.tif"
dormhrM = "dormhr.tif"
dist2StreamD8M = "dist2Stream.tif"

subbasinVec = "subbasin.shp"
basinVec = "basin.shp"
chwidthName = "chwidth.tif"

landuseMFile = "landuse.tif"
cropMFile = "landcover.tif"  ### added by LJ.
soiltypeMFile = "soiltype.tif"
mgtFiedlMFile = "mgt_fields.tif"

soilTexture = "Soil_Texture.tif"
hydroGroup = "Hydro_Group.tif"
usleK = "USLE_K.tif"

initSoilMoist = "moist_in.tif"
depressionFile = "depression.tif"

CN2File = "CN2.tif"
radiusFile = "radius.tif"
ManningFile = "MANNING.tif"
velocityFile = "velocity.tif"
## flow time to the main river from each grid cell
t0_sFile = "t0_s.tif"
## standard deviation of t0_s
delta_sFile = "delta_s.tif"
## potential runoff coefficient
runoff_coefFile = "runoff_co.tif"

## GeoJson format files (WGS 1984 coordinate)
GEOJSON_REACH = "river.json"
GEOJSON_SUBBSN = "subbasin.json"
GEOJSON_OUTLET = "outlet.json"
## Other filenames used in preprocessing
FN_STATUS_DELINEATION = "status_SubbasinDelineation.txt"
FN_STATUS_MASKRASTERS = "maskDemConfig.txt"
FN_STATUS_GENSUBBSN = "status_GenerateSubbasins.txt"
FN_STATUS_EXTRACTPARAM = "status_ExtractParameters.txt"
FN_STATUS_MONGO = "status_BuildMongoDB.txt"
## Header information of raster data (Derived from Mask.tif)
HEADER_RS_TAB = "Header"
HEADER_RS_NODATA = "NODATA_VALUE"
HEADER_RS_XLL = "XLLCENTER"
HEADER_RS_YLL = "YLLCENTER"
HEADER_RS_NROWS = "NROWS"
HEADER_RS_NCOLS = "NCOLS"
HEADER_RS_CELLSIZE = "CELLSIZE"

## Fields in parameter table of MongoDB
PARAM_FLD_NAME = "NAME"
PARAM_FLD_DESC = "DESCRIPTION"
PARAM_FLD_UNIT = "UNIT"
PARAM_FLD_MODS = "MODULE"
PARAM_FLD_VALUE = "VALUE"
PARAM_FLD_IMPACT = "IMPACT"
PARAM_FLD_CHANGE = "CHANGE"
PARAM_FLD_MAX = "MAX"
PARAM_FLD_MIN = "MIN"
PARAM_FLD_USE = "USE"
PARAM_CHANGE_RC = "RC"
PARAM_CHANGE_AC = "AC"
PARAM_CHANGE_NC = "NC"
PARAM_USE_Y = "Y"
PARAM_USE_N = "N"
