#! /usr/bin/env python
#coding=utf-8
## @Subbasin delineation based on TauDEM
# 
#  InitStream, GetOutlet, SubbasinDelineation

import os, sys
from TauDEM import *
from osgeo import gdal
from osr import SpatialReference
from gen_dinf import GenerateDinf
import util
from config import *
def SubbasinDelineation(np, workingDir, dem,outlet, threshold, mpiexeDir=None,exeDir=None):
    if not os.path.exists(workingDir):
        os.mkdir(workingDir)
    statusFile = workingDir + os.sep + "status_SubbasinDelineation.txt"
    fStatus = open(statusFile, 'w')
    tauDir = workingDir + os.sep + "taudir"
    if not os.path.exists(tauDir):
        os.mkdir(tauDir)
    
    status = "Fill DEM..."
    fStatus.write("%d,%s\n" % (10, status))
    fStatus.flush()
    print Fill(np, tauDir, dem, filledDem, mpiexeDir=mpiexeDir,exeDir=exeDir)
    print "[Output], %s, %s" % (workingDir, status)
    
    status = "Calculating D8 and Dinf flow direction..."
    fStatus.write("%d,%s\n" % (20, status))
    fStatus.flush()
    print FlowDirD8(np, tauDir, filledDem, flowDir, slope, mpiexeDir=mpiexeDir,exeDir=exeDir)
    print GenerateDinf(np, tauDir, filledDem, flowDirDinf, slopeDinf, dirCodeDinf, weightDinf,mpiexeDir=mpiexeDir,exeDir=exeDir)
    print "[Output], %s, %s" % (workingDir, status)
    
    status = "Generating stream skeleton..."
    fStatus.write("%d,%s\n" % (30, status))
    fStatus.flush()
    print StreamSkeleton(np, tauDir, filledDem, streamSkeleton,mpiexeDir=mpiexeDir, exeDir=exeDir)
    print "[Output], %s, %s" % (workingDir, status)    
    
    status = "D8 flow accumulation..."
    fStatus.write("%d,%s\n" % (40, status))
    fStatus.flush()
    print FlowAccD8(np, tauDir, flowDir, acc,outlet=None, streamSkeleton=None, mpiexeDir=mpiexeDir,exeDir=exeDir)
    print "[Output], %s, %s" % (workingDir, status)
    
    status = "Generating stream raster initially..."
    fStatus.write("%d,%s\n" % (50, status))
    fStatus.flush()
    if threshold != 0:
        print StreamRaster(np, tauDir, acc, streamRaster, threshold, mpiexeDir=mpiexeDir,exeDir=exeDir)
    else:
        accD8 = tauDir+os.sep+acc
        maxAccum, minAccum, meanAccum, STDAccum = util.GetRasterStat(accD8)
        print StreamRaster(np, tauDir, acc, streamRaster, meanAccum, mpiexeDir=mpiexeDir,exeDir=exeDir)
        print "[Output], %s, %s" % (workingDir, status)
        
    status = "Moving outlet to stream..."
    fStatus.write("%d,%s\n" % (60, status))
    fStatus.flush()
    print MoveOutlet(np, tauDir, flowDir, streamRaster, outlet, modifiedOutlet, mpiexeDir=mpiexeDir,exeDir=exeDir)
    print "[Output], %s, %s" % (workingDir, status)
    
    status = "Flow accumulation with outlet..."
    fStatus.write("%d,%s\n" % (70, status))
    fStatus.flush()
    print FlowAccD8(np, tauDir, flowDir, acc, modifiedOutlet, streamSkeleton, mpiexeDir=mpiexeDir, exeDir=exeDir)
    print "[Output], %s, %s" % (workingDir, status)
    
    if threshold == 0:
        status = "Drop analysis to select optimal threshold..."
        fStatus.write("%d,%s\n" % (75, status))
        fStatus.flush()
        if meanAccum - STDAccum < 0:
            minthresh = meanAccum
        else:
            minthresh = meanAccum - STDAccum
        maxthresh = meanAccum + STDAccum
        numthresh = 20
        logspace = 'true'
        drpFile = 'drp.txt'
        print DropAnalysis(np,tauDir,filledDem,flowDir,acc,acc,modifiedOutlet,minthresh,maxthresh,numthresh,logspace,drpFile, mpiexeDir=mpiexeDir,exeDir=exeDir)
        drpf = open(drpFile,"r")
        tempContents=drpf.read()
        (beg,threshold)=tempContents.rsplit(' ',1)
        print threshold
        drpf.close()
        print "[Output], %s, %s" % (workingDir, status)
        
    status = "Generating stream raster..."
    fStatus.write("%d,%s\n" % (80, status))
    fStatus.flush()
    print StreamRaster(np, tauDir, acc, streamRaster, float(threshold), mpiexeDir=mpiexeDir,exeDir=exeDir)
    print "[Output], %s, %s" % (workingDir, status)

    status = "Generating stream net..."
    fStatus.write("%d,%s\n" % (90, status))
    fStatus.flush()
    print StreamNet(np, tauDir, filledDem, flowDir, acc, streamRaster, modifiedOutlet, streamOrder, chNetwork, chCoord, streamNet, subbasin, mpiexeDir=mpiexeDir, exeDir=exeDir)
    print "[Output], %s, %s" % (workingDir, status)
        
    fStatus.write("100,subbasin delineation is finished!")
    fStatus.close()
    
    ds = gdal.Open(dem)
    configFile = workingDir + os.sep + 'ProjConfig.txt'
    f = open(configFile, 'w')
    f.write(dem + "\n")
    f.write(str(threshold) + "\n")
    projWkt = ds.GetProjection()
    srs = SpatialReference()
    srs.ImportFromWkt(projWkt)
    proj4Str = srs.ExportToProj4()
    f.write(proj4Str + "\n")
    f.close()