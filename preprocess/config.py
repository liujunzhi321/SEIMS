#! /usr/bin/env python
#coding=utf-8
## @Configuration of Preprocessing for SEIMS
#
import os,platform,sys

## Directionaries 
if platform.system() == "Windows":
    ### GAO HR
    # BASE_DATA_DIR = r'D:\GaohrWS\GithubPrj\SEIMS\model_data\model_dianbu_30m_longterm\data_prepare'
    # TXT_DB_DIR = r'D:\GaohrWS\GithubPrj\SEIMS\database'
    # PREPROC_SCRIPT_DIR = r'D:\GaohrWS\GithubPrj\SEIMS\preprocess'
    # CPP_PROGRAM_DIR = r'D:\SEIMS_model\SEIMS_preprocessing\build\Release'
    # METIS_DIR = r'D:\SEIMS_model\SEIMS_preprocessing\build\metis\programs'
    # MPIEXEC_DIR = r'"D:\Program Files\Microsoft HPC Pack 2012\Bin"'
    # WORKING_DIR = r'E:\data_m\SEIMS\dianbu_10m_output'
    ### SHEN F
    # BASE_DATA_DIR = r'C:\Users\Administrator\Desktop\seims\SEIMS\model_data\model_dianbu_30m_longterm\data_prepare'
    # TXT_DB_DIR = r'C:\Users\Administrator\Desktop\seims\SEIMS\database'
    # PREPROC_SCRIPT_DIR = r'C:\Users\Administrator\Desktop\seims\SEIMS\preprocess'
    # CPP_PROGRAM_DIR = r'C:\Users\Administrator\Desktop\seims\preprocessBuild\Release'
    # METIS_DIR = r'C:\Users\Administrator\Desktop\seims\preprocessBuild\metis\programs\Release'
    # MPIEXEC_DIR = None
    # WORKING_DIR = r'E:\data_m\SEIMS\dianbu_10m_output'
    ### ZHU LJ
    BASE_DATA_DIR = r'E:\code\Hydro\SEIMS\model_data\dianbu\data_prepare'
    TXT_DB_DIR = r'E:\code\Hydro\SEIMS\database'
    PREPROC_SCRIPT_DIR = r'E:\code\Hydro\SEIMS\preprocess'
    CPP_PROGRAM_DIR = r'D:\Compile\SEIMS_Preprocess\Release'
    METIS_DIR = r'D:\Compile\SEIMS_Preprocess\metis\programs\Release'
    MPIEXEC_DIR = None
    WORKING_DIR = r'E:\data_m\SEIMS\dianbu_30m_output'
    ### ZHU LJ-WindowsServer-6.57
    # BASE_DATA_DIR = r'G:\code_zhulj\SEIMS\model_data\model_dianbu_10m_longterm\data_prepare'
    # TXT_DB_DIR = r'G:\code_zhulj\SEIMS\database'
    # PREPROC_SCRIPT_DIR = r'G:\code_zhulj\SEIMS\preprocess'
    # CPP_PROGRAM_DIR = r'G:\Compile\SEIMS_Preprocess\Release'
    # METIS_DIR = r'G:\compile\SEIMS_Preprocess\metis\programs\Release'
    # MPIEXEC_DIR = None
    # WORKING_DIR = r'G:\data_m\SEIMS_dataprepare\model_dianbu_10m_longterm'
elif platform.system() == "Linux": ### Hasn't tested yet, Apr.,2016, LJ.
    BASE_DATA_DIR = r'/data/liujz/data'
    TXT_DB_DIR = r'/data/liujz/data/database'
    PREPROC_SCRIPT_DIR = r'/data/hydro_preprocessing'
    CPP_PROGRAM_DIR = r'/data/hydro_preprocessing/cpp_programs'
    METIS_DIR = r'/soft/programming/metis-5.1.0/build/programs'
    MPIEXEC_DIR = None
    WORKING_DIR = r'E:\data_m\SEIMS\dianbu_10m_output'

CLIMATE_DATA_DIR = BASE_DATA_DIR + os.sep + 'climate'
SPATIAL_DATA_DIR = BASE_DATA_DIR + os.sep + 'spatial'
MEASUREMENT_DATA_DIR = BASE_DATA_DIR + os.sep + 'observed'
BMP_DATA_DIR     = BASE_DATA_DIR + os.sep + 'management'

## MongoDB related
#HOSTNAME = '192.168.6.57'
HOSTNAME = '127.0.0.1'
PORT = 27017
ClimateDBName = 'HydroClimate_dianbu'
BMPScenarioDBName = 'BMP_Scenario_dianbu'
SpatialDBName = 'model_dianbu_30m_longterm'
forCluster = False 
stormMode = False
if forCluster and 'cluster_' not in SpatialDBName.lower():
    SpatialDBName = 'cluster_' + SpatialDBName
## Climate Input
HydroClimateVarFile = CLIMATE_DATA_DIR + os.sep + 'Variables.txt'
MetroSiteFile = CLIMATE_DATA_DIR + os.sep + 'Sites_M.txt'
PrecSiteFile = CLIMATE_DATA_DIR + os.sep + 'Sites_P.txt'
MeteoDailyFile = CLIMATE_DATA_DIR + os.sep+ 'meteorology_dianbu_daily.txt'
PrecDailyFile = CLIMATE_DATA_DIR + os.sep+ 'precipitation_dianbu_daily.txt'

## Spatial Input
PrecSitesThiessen = SPATIAL_DATA_DIR + os.sep + 'Thiessen_P.shp'
if stormMode:
    PrecStormSitesThiessen = SPATIAL_DATA_DIR + os.sep + 'Thiessen_P_STORM.shp'
MeteorSitesThiessen = SPATIAL_DATA_DIR + os.sep + 'Thiessen_M.shp'
dem = SPATIAL_DATA_DIR + os.sep + 'dem_30m.tif'
outlet_file = SPATIAL_DATA_DIR + os.sep + 'outlet.shp'

threshold = 0  # threshold for stream extraction from D8-flow accumulation weighted Peuker-Douglas stream sources
               # if threshold is 0, then Drop Analysis is used to select the optimal value.
np = 4  # number of parallel processors for TauDEM
landuseFile = SPATIAL_DATA_DIR + os.sep + 'landuse_30m.tif'
soilSEQNFile = SPATIAL_DATA_DIR + os.sep + 'soil_SEQN_30m.tif'
soilSEQNText = SPATIAL_DATA_DIR + os.sep + 'soil_properties_lookup.txt'

## Management Fields, optional, but required for BMPs optimization
mgtFiedlFile = SPATIAL_DATA_DIR + os.sep + 'mgt_fields_30m.tif'
## Hydrological parameters
## Adopted from WetSpa, used in radius.py
coeTable = {"T2":[0.05, 0.48],
            "T10":[0.12, 0.52],
            "T100":[0.18,0.55]}
## time threshold used to define dormant period for plant, default is -1.
dorm_hr = -1.
## minimum temperature for plant growth， deg C
T_base = 0.