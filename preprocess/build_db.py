#! /usr/bin/env python
#coding=utf-8
import sys, os, thread
from pymongo import MongoClient
from pymongo.errors import ConnectionFailure
from import_parameters import ImportParameters
from import_parameters import ImportLookupTables
from generate_stream_input import GenerateReachTable
from find_sites import FindSites
from weights_mongo import GenerateWeightInfo,GenerateWeightDependentParameters
from text import *
from config import *

def BuildMongoDB(workingDir, modelName, stormMode, forCluster,
                   hydroDBName=None, thiessenFilePreci=None, thiessenFileMeteo=None):
    genIUH = True

    statusFile = workingDir + os.sep + FN_STATUS_MONGO
    f = open(statusFile, 'w')
    
    simuMode = Tag_Mode_Daily
    if stormMode:
        simuMode = Tag_Mode_Storm
        if not Tag_Mode_Storm.lower() in modelName.lower():
            modelName = modelName + "_" + Tag_Mode_Storm.lower()
        genIUH = False

    if not Tag_Model.lower() in modelName.lower():
        modelName = Tag_Model.lower() + "_" + modelName

    if forCluster and (not Tag_Cluster.lower() in modelName.lower()):
        modelName = Tag_Cluster.lower() + "_" + modelName

    # build mongodb database
    try:
        conn = MongoClient(host=HOSTNAME, port=PORT)
        # conn = Connection(host=HOSTNAME, port=PORT)
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
    if modelName in conn.database_names():
        conn.drop_database(modelName)
    db = conn[modelName]

    ImportParameters(sqliteFile, db)   ## import parameters information from parameter.db3 to mongoDB
    ImportLookupTables(sqliteFile, db) ## import lookup tables from parameter.db3 to mongoDB. By LJ, 2016-6-13
    f.write("10, Generating reach table...\n")
    f.flush()
    GenerateReachTable(workingDir, db, forCluster)
    
    ## prepare meteo data
    subbasinFile = workingDir + os.sep + basinVec
    subbasinRaster = workingDir + os.sep + mask_to_ext  ## mask file
    if forCluster:
        subbasinFile = workingDir + os.sep + DIR_SUBBASIN + os.sep + subbasinVec
        subbasinRaster = workingDir + os.sep + subbasinOut

    if hydroDBName is not None:
        if stormMode:
            hydroDBName = hydroDBName + "_" + Tag_CLIM_STORM_Suf.lower()
    if stormMode:
        meteoThiessenList = [thiessenFilePreci]
        meteoTypeList = [DataType_Precipitation]
    else:
        meteoThiessenList = [thiessenFileMeteo, thiessenFilePreci]
        meteoTypeList = [DataType_Meteorology, DataType_Precipitation]

    f.write("20, Finding nearby stations for each sub-basin...\n")
    f.flush()
    nSubbasins = FindSites(db, hydroDBName, subbasinFile, meteoThiessenList, meteoTypeList, simuMode) 
    print "Number of sub-basins:", nSubbasins

    if not forCluster:
        nSubbasins = 1

    # import raster data to mongodb
    f.write("40, Importing raster to database...\n")
    f.flush()

    tifFolder = workingDir + os.sep + DIR_TIFF
    if not os.path.exists(tifFolder):
        os.mkdir(tifFolder)
    for i in range(1,nSubbasins+1):
        subdir = tifFolder + os.sep + str(i)
        if not os.path.exists(subdir):
            os.mkdir(subdir)
    strCmd = "%s/import_raster %s %s %s %s %d %s" % \
                 (CPP_PROGRAM_DIR, subbasinRaster, workingDir, modelName, HOSTNAME, PORT, tifFolder)
    print strCmd
    os.system(strCmd)
    
    print 'Generating weight data...'
    f.write("70, Generating weight data for interpolation of meteorology data...\n")
    f.flush()
    for i in range(nSubbasins):
        GenerateWeightInfo(conn, modelName, i+1, stormMode)
        GenerateWeightDependentParameters(conn, i+1)  ##　added by Liangjun, 2016-6-17
    if genIUH:
        f.write("80, Generating IUH (Instantaneous Unit Hydrograph)...\n")
        f.flush()
        dt = 24
        print 'Generating IUH (Instantaneous Unit Hydrograph)...'
        strCmd = "%s/iuh %s %d %s %s %d" % (CPP_PROGRAM_DIR, HOSTNAME, PORT, modelName, dt, nSubbasins)
        print strCmd
        os.system(strCmd)

    f.write("90, Generating Grid layering...\n")
    f.flush()
    layeringDir = workingDir + os.sep + DIR_LAYERING
    if not os.path.exists(layeringDir):
        os.mkdir(layeringDir)
    print 'Generating Grid layering...'
    strCmd = "%s/grid_layering %s %d %s %s %d" % (CPP_PROGRAM_DIR, HOSTNAME, PORT, layeringDir, modelName, nSubbasins)
    print strCmd
    os.system(strCmd)

    f.write("100,Finished!")
    f.close()
    print 'Build DB: %s finished!' % (modelName)

## test code
if __name__ == "__main__":
    BuildMongoDB(WORKING_DIR, SpatialDBName, stormMode, forCluster, ClimateDBName, PrecSitesThiessen, MeteorSitesThiessen)