#! /usr/bin/env python
# coding=utf-8
# Import all model parameters and spatial datasets to MongoDB
# Author: Junzhi Liu
# Revised: Liang-Jun Zhu
#

from config import *
from pymongo import MongoClient
from pymongo.errors import ConnectionFailure
from import_bmp_scenario import ImportBMPTables
from import_parameters import ImportParameters
from import_parameters import ImportLookupTables
from generate_stream_input import GenerateReachTable
from find_sites import FindSites
from weights_mongo import GenerateWeightInfo, GenerateWeightDependentParameters


def BuildMongoDB():
    statusFile = WORKING_DIR + os.sep + FN_STATUS_MONGO
    f = open(statusFile, 'w')
    # build mongodb database
    try:
        conn = MongoClient(host = HOSTNAME, port = PORT)
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)

    db = conn[SpatialDBName]
    # import parameters information to MongoDB
    ImportParameters(TXT_DB_DIR + os.sep + sqliteFile, db)
    # import lookup tables from to MongoDB as GridFS. By LJ, 2016-6-13
    ImportLookupTables(TXT_DB_DIR + os.sep + sqliteFile, db)
    f.write("10, Generating reach table...\n")
    f.flush()
    GenerateReachTable(WORKING_DIR, db, forCluster)

    ## prepare meteorology data
    subbasinFile = WORKING_DIR + os.sep + basinVec  # basin.shp
    fldID = FLD_BASINID
    subbasinRaster = WORKING_DIR + os.sep + mask_to_ext  # mask.tif
    if forCluster:
        subbasinFile = WORKING_DIR + os.sep + DIR_NAME_SUBBSN + os.sep + subbasinVec  # subbasin.shp
        fldID = FLD_SUBBASINID
        subbasinRaster = WORKING_DIR + os.sep + subbasinOut  # subbasin.tif

    if stormMode:
        meteoThiessenList = [PrecSitesThiessen]
        meteoTypeList = [DataType_Precipitation]
    else:
        meteoThiessenList = [MeteorSitesThiessen, PrecSitesThiessen]
        meteoTypeList = [DataType_Meteorology, DataType_Precipitation]

    f.write("20, Finding nearby stations for each sub-basin...\n")
    f.flush()
    nSubbasins = FindSites(db, ClimateDBName, subbasinFile, fldID, meteoThiessenList, meteoTypeList, simuMode)
    print "Number of sub-basins:%d" % nSubbasins

    if not forCluster:
        nSubbasins = 1

    # import raster data to MongoDB
    f.write("40, Importing raster to MongoDB...\n")
    f.flush()

    tifFolder = WORKING_DIR + os.sep + DIR_NAME_TIFFIMPORT
    if not os.path.exists(tifFolder):
        os.mkdir(tifFolder)
    for i in range(1, nSubbasins + 1):
        subdir = tifFolder + os.sep + str(i)
        if not os.path.exists(subdir):
            os.mkdir(subdir)
    strCmd = "%s/import_raster %s %s %s %s %s %d %s" % (CPP_PROGRAM_DIR, subbasinRaster,
                                                        WORKING_DIR, SpatialDBName, DB_TAB_SPATIAL.upper(), HOSTNAME,
                                                        PORT, tifFolder)
    print strCmd
    os.system(strCmd)

    print 'Generating weight data...'
    f.write("70, Generating weight data for interpolation of meteorology data...\n")
    f.flush()
    for i in range(nSubbasins):
        GenerateWeightInfo(conn, SpatialDBName, i + 1, stormMode)
        GenerateWeightDependentParameters(conn, i + 1)  ##　added by Liangjun, 2016-6-17
    if genIUH:
        f.write("80, Generating IUH (Instantaneous Unit Hydrograph)...\n")
        f.flush()
        dt = 24
        print 'Generating IUH (Instantaneous Unit Hydrograph)...'
        strCmd = "%s/iuh %s %d %s %s %s %d" % (CPP_PROGRAM_DIR, HOSTNAME, PORT,
                                               SpatialDBName, DB_TAB_SPATIAL.upper(), dt, nSubbasins)
        print strCmd
        os.system(strCmd)

    f.write("90, Generating Grid layering...\n")
    f.flush()
    layeringDir = WORKING_DIR + os.sep + DIR_NAME_LAYERINFO
    if not os.path.exists(layeringDir):
        os.mkdir(layeringDir)
    print 'Generating Grid layering...'
    strCmd = "%s/grid_layering %s %d %s %s %s %d" % (
        CPP_PROGRAM_DIR, HOSTNAME, PORT, layeringDir, SpatialDBName, DB_TAB_SPATIAL.upper(), nSubbasins)
    print strCmd
    os.system(strCmd)

    ## Import BMP scenario database to MongoDB
    ImportBMPTables()

    f.write("100,Finished!")
    f.close()
    print 'Build DB: %s finished!' % (SpatialDBName)


## test code
if __name__ == "__main__":
    BuildMongoDB()
