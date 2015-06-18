#! /usr/bin/env python
#coding=utf-8
## @Main function entrance for preprocessing
#
#
from config import *
## Climate modules
from PrecipitationDaily import *
from MeteorologicalDaily import *
from DischargeDaily import *
## Spatial modules
from subbasin_delineation import SubbasinDelineation
from gen_subbasins import GenerateSubbasins
from parameters_extraction import ExtractParameters
## Import to MongoDB
#from build_db import BuildMongoDB
if __name__ == "__main__":
    ## Climate Data
    #ImPORTDailyMeteoData(HOSTNAME,PORT,ClimateDBName,MeteoVarFile,MeteoDailyFile,MetroSiteFile)
    #ImPORTDailyPrecData(HOSTNAME,PORT,ClimateDBName,PrecSitesVorShp,PrecExcelPrefix,PrecDataYear)
    ## Discharge Data
    #ImPORTDailyDischargeData(HOSTNAME,PORT,ClimateDBName,DischargeExcelPrefix,DischargeYear)
    ## Spatial Data
    #SubbasinDelineation(np, WORKING_DIR,dem,outlet_file, threshold, mpiexeDir=MPIEXEC_DIR,exeDir=CPP_PROGRAM_DIR)
    #f = open(WORKING_DIR + os.sep + "ProjConfig.txt")
    #proj4Str = f.readlines()[2].strip()
    ##print proj4Str
    #f.close()
    #GenerateSubbasins(WORKING_DIR, proj4Str,exeDir=CPP_PROGRAM_DIR)
    ## Extract parameters from landuse, soil properties etc.
    ExtractParameters(landuseFile, sandList, clayList, orgList, WORKING_DIR, False, False, False, False)
    ## Import to MongoDB database
    #BuildMongoDB(WORKING_DIR, SpatialDBName, stormMode, forCluster, ClimateDBName, PrecSitesVorShp,MeteorSitesVorShp)