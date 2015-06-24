#! /usr/bin/env python
#coding=utf-8
## @Configuration of Preprocessing for SEIMS
# 
#  TODO, give more detailed description here.
import os,platform

## Directionaries 
if platform.system() == "Windows":
    DATA_BASE_DIR = r'E:\github-zlj\model_data\model_dianbu_30m_longterm\data_prepare'
    PREPROC_SCRIPT_DIR = r'E:\github-zlj\SEIMS\preprocess'
    CPP_PROGRAM_DIR = r'E:\github-zlj\SEIMS_Preprocess\Debug'
    METIS_DIR = r'E:\github-zlj\SEIMS_Preprocess\metis\programs\Debug'
    MPIEXEC_DIR = None
elif platform.system() == "Linux":
    DATA_BASE_DIR = r'/data/liujz/data'
    PREPROC_SCRIPT_DIR = r'/data/hydro_preprocessing'
    CPP_PROGRAM_DIR = r'/data/hydro_preprocessing/cpp_programs'
    METIS_DIR = r'/soft/programming/metis-5.1.0/build/programs'
    MPIEXEC_DIR = None
CLIMATE_DATA_DIR = DATA_BASE_DIR + os.sep + 'climate'
SPATIAL_DATA_DIR = DATA_BASE_DIR + os.sep + 'spatial'
WORKING_DIR      = DATA_BASE_DIR + os.sep + 'output'

## MongoDB related
#HOSTNAME = '192.168.6.55'
HOSTNAME = '127.0.0.1'
PORT = 27017
ClimateDBName = 'climate_dianbu'
SpatialDBName = 'model_dianbu_30m_longterm'
forCluster = False 
stormMode = False
if forCluster and 'cluster_' not in SpatialDBName.lower():
    SpatialDBName = 'cluster_' + SpatialDBName
## Climate Input
PrecSitesVorShp = CLIMATE_DATA_DIR + os.sep + 'shp' + os.sep + 'Preci_dianbu_Vor.shp'
if stormMode:
    PrecStormSitesVorShp = CLIMATE_DATA_DIR + os.sep + 'shp' + os.sep + 'Preci_dianbu_Vor_storm.shp'
MeteorSitesVorShp = CLIMATE_DATA_DIR + os.sep + 'shp' + os.sep + 'Metero_hefei_Vor.shp'
PrecExcelPrefix = CLIMATE_DATA_DIR + os.sep + 'precipitation_by_day_'
PrecDataYear = [2014]
MeteoVarFile = CLIMATE_DATA_DIR + os.sep + 'Variables.txt'
MeteoDailyFile = CLIMATE_DATA_DIR + os.sep+ 'meteorology_dianbu_daily.txt'
MetroSiteFile = CLIMATE_DATA_DIR + os.sep + 'sites_hefei.txt'
DischargeExcelPrefix = CLIMATE_DATA_DIR + os.sep + 'discharge_by_day_'
DischargeYear = [2014]

## Parameters for SEIMS
sqliteFile = DATA_BASE_DIR + os.sep + "Parameter.db3"
## Spatial Input
dem = SPATIAL_DATA_DIR + os.sep + 'dem_30m.tif'
outlet_file = SPATIAL_DATA_DIR + os.sep + 'outlet_30m.shp'

threshold = 0  # threshold for stream extraction from D8-flow accumulation weighted Peuker-Douglas stream sources
               # if threshold is 0, then Drop Analysis is used to select the optimal value.
np = 4  # number of parallel processors
landuseFile = SPATIAL_DATA_DIR + os.sep + 'landuse_30m.tif'
sandList = []
clayList = []
orgList = []
for i in [1,2]:
    sandFile = SPATIAL_DATA_DIR + os.sep + "sand" + str(i) + ".tif" 
    clayFile = SPATIAL_DATA_DIR + os.sep + "clay" + str(i) + ".tif"
    orgFile = SPATIAL_DATA_DIR + os.sep + "org" + str(i) + ".tif"
    sandList.append(sandFile)
    clayList.append(clayFile)
    orgList.append(orgFile)
defaultSand = 40
defaultClay = 30
defaultOrg = 2.5

## Predefined variables
CROP_FILE = PREPROC_SCRIPT_DIR + os.sep + 'crop.txt'
CROP_ATTR_LIST  = ["IDC", "EXT_COEF",  "BMX_TREES", "BLAI", "HVSTI",\
                    "MAT_YRS", "T_BASE", "FRGRW1", "FRGRW2", "LAIMX1",\
                    "LAIMX2", "DLAI", "BN1", "BN2", "BN3", "BP1", "BP2",\
                    "BP3", "BIO_E", "BIOEHI", "CO2HI", "WAVP", "BIO_LEAF",\
                    "RDMX","CNYLD", "CPYLD", "WSYF", "DLAI", "T_OPT"]
                    
# LANDUSE_ATTR_LIST and SOIL_ATTR_LIST is selected from sqliteFile database
LANDUSE_ATTR_LIST = ["Manning", "Interc_max", "Interc_min", "RootDepth", \
                    "USLE_C", "SOIL_T10","USLE_P"]
LANDUSE_ATTR_DB = ["manning","i_max","i_min", "root_depth", "usle_c", "SOIL_T10"]
## Be caution, the sequence from "Sand" to "Poreindex" if fixed because of soil_param.py.
SOIL_ATTR_LIST = ["Sand", "Clay", "WiltingPoint", "FieldCap", "Porosity","Density",\
                "Conductivity", "Poreindex", "USLE_K", "Residual", ]
SOIL_ATTR_DB  = ["sand", "clay","wp", "fc", "porosity","B_DENSITY","ks", "P_INDEX",\
                "usle_k", "rm"]

### There are 15 attributes in SoilLookup table now. 
### They are [SOILCODE], [SNAM], [KS](Conductivity), [POROSITY], [FC](field capacity), [P_INDEX](Poreindex), [RM],
### [WP](wiltingpoint), [B_DENSITY], [SAND], [CLAY], [SILT], [USLE_K], [TEXTURE], [HG]


## Hydrological parameters
coeTable = {"T2":[0.05, 0.48],"T10":[0.12, 0.52], "T100":[0.18,0.55]} ## used in radius.py

## Conventional Spatial Raster Data File Names
filledDem = "demFilledTau.tif"
flowDir = "flowDirTauD8.tif"
slope = "slopeTau.tif"
acc = "accTauD8.tif"
streamRaster = "streamRasterTau.tif"

flowDirDinf = "flowDirDinfTau.tif"
dirCodeDinf = "dirCodeDinfTau.tif"
slopeDinf = "slopeDinfTau.tif"
weightDinf = "weightDinfTau.tif"

modifiedOutlet = "outletM.shp"
streamSkeleton = "streamSkeleton.tif"
streamOrder = "streamOrderTau.tif"
chNetwork = "chNetwork.txt"
chCoord = "chCoord.txt"
streamNet = "streamNet.shp"
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

subbasinVec = "subbasin.shp"
basinVec = "basin.shp"
chwidthName = "chwidth.tif"

landuseMFile = "landuse.tif"

soilTexture = "soil_texture.tif"
hydroGroup = "hydro_group.tif"
usleK = "usle_k.tif"

initSoilMoist = "moist_in.tif"
depressionFile = "depression.tif"

CN2File = "CN2.tif"
radiusFile = "radius.tif"
ManningFile = "Manning.tif"
velocityFile = "velocity.tif"
## flow time to the main river from each grid cell
t0_sFile = "t0_s.tif"
## standard deviation of t0_s
delta_sFile = "delta_s.tif"
## potential runoff coefficient
runoff_coefFile = "runoff_co.tif"

