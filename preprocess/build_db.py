#! /usr/bin/env python
#coding=utf-8
import sys, os, thread
from pymongo import Connection
from pymongo.errors import ConnectionFailure
from import_parameters import ImportParameters
from generate_stream_input import GenerateReachTable
from find_sites import FindSites
from weights_mongo import GenerateWeightInfo 
import util
from config import *

def BuildMongoDB(workingDir, modelName, stormMode, forCluster, \
                   hydroDBName=None, thiessenFilePreci=None, thiessenFileMeteo=None):
    genIUH = True

    statusFile = workingDir + os.sep + "status_BuildMongoDB.txt"
    f = open(statusFile, 'w')
    
    simuMode = 'DAILY'
    if stormMode:
        simuMode = 'STORM'
        if not "storm" in modelName.lower():
            modelName = modelName + "_storm"
        genIUH = False

    if not "model" in modelName.lower():
        modelName = "model_" + modelName

    if forCluster and (not "cluster" in modelName.lower()):
        modelName = "cluster_" + modelName

    # build mongodb database
    try:
        conn = Connection(host=HOSTNAME, port=PORT)
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
    if modelName in conn.database_names():
        conn.drop_database(modelName)
    db = conn[modelName]

    ImportParameters(sqliteFile, db)
    f.write("10, Generating reach table...\n")
    f.flush()
    GenerateReachTable(workingDir, db, forCluster)
    
    ## prepare meteo data
    subbasinFile = workingDir + os.sep + basinVec
    subbasinRaster = workingDir + os.sep + mask_to_ext
    if forCluster:
        subbasinFile = workingDir + os.sep + "subbasins" + os.sep + subbasinVec
        subbasinRaster = workingDir + os.sep + subbasinOut

    if hydroDBName is not None:
        if stormMode:
            hydroDBName = hydroDBName + "_storm_cmorph"
    if stormMode:
        meteoThiessenList = [thiessenFilePreci]
        meteoTypeList = ['P']
    else:
        meteoThiessenList = [thiessenFileMeteo, thiessenFilePreci]
        meteoTypeList = ['M', 'P']

    f.write("20, Finding nearby stations for each sub-basin...\n")
    f.flush()
    nSubbasins = FindSites(db, hydroDBName, subbasinFile, meteoThiessenList, meteoTypeList, simuMode) 
    print "Number of sub-basins:", nSubbasins

    if not forCluster:
        nSubbasins = 1

    # import raster data to mongodb
    f.write("40, Importing raster to database...\n")
    f.flush()

    tifFolder = workingDir + os.sep + "tif_files"
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

    if genIUH:
        f.write("80, Generating IUH...\n")
        f.flush()
        dt = 24
        print 'Generating IUH...'
        strCmd = "%s/iuh %s %d %s %s %d" % (CPP_PROGRAM_DIR, HOSTNAME, PORT, modelName, dt, nSubbasins)
        print strCmd
        os.system(strCmd)

    f.write("90, Grid layering...\n")
    f.flush()
    layeringDir = workingDir + os.sep + "layering_info"
    if not os.path.exists(layeringDir):
        os.mkdir(layeringDir)
    print 'Grid layering...'
    strCmd = "%s/grid_layering %s %d %s %s %d" % (CPP_PROGRAM_DIR, HOSTNAME, PORT, layeringDir, modelName, nSubbasins)
    print strCmd
    os.system(strCmd)

    f.write("100,Finished!")
    f.close()
    print 'Build DB: %s finished!' % (modelName)

if __name__ == "__main__":

    workingDir = r"/data/hydro_preprocessing/fenkeng"

    modelName = "model_fenkeng"
    genIUH = True 
    forCluster = True 
    stormMode = True 
    
    if len(sys.argv) >= 5:
        workingDir = sys.argv[1]
        modelName = sys.argv[2]
        if int(sys.argv[3]) > 0:
            stormMode = True
        if int(sys.argv[4]) > 0:
            forCluster = True

    if stormMode:
        genIUH = False
    else:
        genIUH = True

    if forCluster and 'cluster_' not in modelName.lower():
        modelName = 'cluster_' + modelName

    BuildMongoDB(workingDir, modelName, stormMode, genIUH, forCluster) 

