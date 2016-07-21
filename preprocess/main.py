#! /usr/bin/env python
# coding=utf-8
## @Main function entrance for preprocessing
# Author: Liang-Jun Zhu
#
import sys
import os
## Load configuration file
from util import GetINIfile, LoadConfiguration
## Intermediate SQLite database
from txt2db3 import reConstructSQLiteDB
## HydroClimate modules
from hydroclimate_sites import ImportHydroClimateSitesInfo
from PrecipitationDaily import ImportDailyPrecData
from MeteorologicalDaily import ImportDailyMeteoData
from import_measurement import ImportMeasurementData
## Spatial modules
from subbasin_delineation import SubbasinDelineation
from parameters_extraction import ExtractParameters
## MongoDB modules
from build_db import BuildMongoDB

if __name__ == "__main__":
    ## Load Configuration file
    LoadConfiguration(GetINIfile())
    ## Update SQLite Parameters.db3 database
    reConstructSQLiteDB()
    ## Climate Data
    SitesMList, SitesPList = ImportHydroClimateSitesInfo()
    ImportDailyMeteoData(SitesMList)
    ImportDailyPrecData(SitesPList)
    ## Measurement Data, such as discharge, sediment yield.
    ImportMeasurementData()
    ## Spatial Data derived from DEM
    SubbasinDelineation()
    ## Extract parameters from landuse, soil properties etc.
    ExtractParameters()
    ## Import to MongoDB database
    BuildMongoDB()
