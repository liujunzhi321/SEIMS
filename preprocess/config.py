#! /usr/bin/env python
#coding=utf-8
## @Configuration of Preprocessing for SEIMS
# 
#  TODO, give more detailed description here.
import os,platform,sys

## Directionaries 
if platform.system() == "Windows":
    BASE_DATA_DIR = r'D:\SEIMS_model\Model_data\model_dianbu_30m_longterm\data_prepare'
    TXT_DB_DIR = r'E:\code\Hydro\SEIMS\database'
    PREPROC_SCRIPT_DIR = r'E:\code\Hydro\SEIMS\preprocess'
    CPP_PROGRAM_DIR = r'D:\Compile\SEIMS_Preprocess\Release'
    METIS_DIR = r'D:\Compile\SEIMS_Preprocess\metis\programs\Release'
    MPIEXEC_DIR = None
elif platform.system() == "Linux": ### Hasn't tested yet, Apr.,2016, LJ.
    BASE_DATA_DIR = r'/data/liujz/data'
    TXT_DB_DIR = r'/data/liujz/data/database'
    PREPROC_SCRIPT_DIR = r'/data/hydro_preprocessing'
    CPP_PROGRAM_DIR = r'/data/hydro_preprocessing/cpp_programs'
    METIS_DIR = r'/soft/programming/metis-5.1.0/build/programs'
    MPIEXEC_DIR = None
CLIMATE_DATA_DIR = BASE_DATA_DIR + os.sep + 'climate'
SPATIAL_DATA_DIR = BASE_DATA_DIR + os.sep + 'spatial'
WORKING_DIR      = BASE_DATA_DIR + os.sep + 'output'

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
PrecSitesThiessen = CLIMATE_DATA_DIR + os.sep + 'shp' + os.sep + 'Preci_dianbu_Vor.shp'
if stormMode:
    PrecStormSitesThiessen = CLIMATE_DATA_DIR + os.sep + 'shp' + os.sep + 'Preci_dianbu_Vor_storm.shp'
MeteorSitesThiessen = CLIMATE_DATA_DIR + os.sep + 'shp' + os.sep + 'Metero_hefei_Vor.shp'

HydroClimateVarFile = CLIMATE_DATA_DIR + os.sep + 'Variables.txt'
MetroSiteFile = CLIMATE_DATA_DIR + os.sep + 'Sites_M.txt'
PrecSiteFile = CLIMATE_DATA_DIR + os.sep + 'Sites_P.txt'
MeteoDailyFile = CLIMATE_DATA_DIR + os.sep+ 'meteorology_dianbu_daily.txt'
PrecExcelPrefix = CLIMATE_DATA_DIR + os.sep + 'precipitation_by_day_'
PrecDataYear = [2014]
DischargeExcelPrefix = CLIMATE_DATA_DIR + os.sep + 'discharge_by_day_'
DischargeYear = [2014]


## Spatial Input
dem = SPATIAL_DATA_DIR + os.sep + 'dem_30m.tif'
outlet_file = SPATIAL_DATA_DIR + os.sep + 'outlet_30m.shp'

threshold = 0  # threshold for stream extraction from D8-flow accumulation weighted Peuker-Douglas stream sources
               # if threshold is 0, then Drop Analysis is used to select the optimal value.
np = 4  # number of parallel processors
landuseFile = SPATIAL_DATA_DIR + os.sep + 'landuse_30m.tif'
rootDepthFile = SPATIAL_DATA_DIR + os.sep + 'RootDepth.tif'
densityList = []
sandList = []
clayList = []
orgList = []
nlyrs = 2   # soil layers
nlyrs_thick=[]
nlyrs_thick = [100, DEFAULT_NODATA]   #soil layer thick, unit is mm
for i in range(1, nlyrs + 1):
    densityFile = SPATIAL_DATA_DIR + os.sep + "Density" + str(i) + ".tif"
    sandFile = SPATIAL_DATA_DIR + os.sep + "sand" + str(i) + ".tif" 
    clayFile = SPATIAL_DATA_DIR + os.sep + "clay" + str(i) + ".tif"
    orgFile = SPATIAL_DATA_DIR + os.sep + "org" + str(i) + ".tif"
    densityList.append(densityFile)
    sandList.append(sandFile)
    clayList.append(clayFile)
    orgList.append(orgFile)
defaultSand = 40
defaultClay = 30
defaultOrg = 2.5

## Predefined variables

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

## Parameters for soil chemical properties initialization


## Hydrological parameters
coeTable = {"T2":[0.05, 0.48],"T10":[0.12, 0.52], "T100":[0.18,0.55]} ## used in radius.py, TODO: what's meaning
