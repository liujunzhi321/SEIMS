#! /usr/bin/env python
#coding=utf-8
## @Main function entrance for preprocessing
#

from text import *
from txt2db3 import reConstructSQLiteDB
## HydroClimate modules
from hydroclimate_sites import ImportHydroClimateSitesInfo
from PrecipitationDaily import ImportDailyPrecData
from MeteorologicalDaily import ImportDailyMeteoData
from import_measurement import ImportMeasurementData
## BMP modules
from import_bmp_scenario import ImportBMPTables
## Spatial modules
from subbasin_delineation import SubbasinDelineation
from gen_subbasins import GenerateSubbasins
from parameters_extraction import ExtractParameters
## MongoDB modules
from build_db import BuildMongoDB

if __name__ == "__main__":
    ## Update SQLite Parameters.db3 database
    #reConstructSQLiteDB()
    ## Climate Data
    #SitesMList, SitesPList = ImportHydroClimateSitesInfo(HOSTNAME, PORT, ClimateDBName,HydroClimateVarFile, MetroSiteFile, PrecSiteFile)
    #ImportDailyMeteoData(HOSTNAME, PORT, ClimateDBName, MeteoDailyFile, SitesMList)
    #ImportDailyPrecData(HOSTNAME, PORT, ClimateDBName, PrecDailyFile, SitesPList)
    ## Measurement Data, such as discharge, sediment yield.
    #ImportMeasurementData(HOSTNAME,PORT,ClimateDBName,MEASUREMENT_DATA_DIR)
    ## Spatial Data derived from DEM
    #SubbasinDelineation(np, WORKING_DIR, dem, outlet_file, threshold, mpiexeDir=MPIEXEC_DIR,exeDir=CPP_PROGRAM_DIR)
    GenerateSubbasins(WORKING_DIR, exeDir=CPP_PROGRAM_DIR)
    ## Extract parameters from landuse, soil properties etc.
    ExtractParameters(landuseFile, WORKING_DIR, True, True, True, True)
    ## Import to MongoDB database
    BuildMongoDB(WORKING_DIR, SpatialDBName, stormMode, forCluster, ClimateDBName, PrecSitesThiessen, MeteorSitesThiessen)
    ## Import BMP scenario database to MongoDB
    ImportBMPTables()
