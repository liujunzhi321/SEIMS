#! /usr/bin/env python
#coding=utf-8
## @Subbasin delineation based on TauDEM
# 
#  InitStream, GetOutlet, SubbasinDelineation

import os, sys
from TauDEM import *
from osgeo import gdal
from osgeo import osr,ogr
from osr import SpatialReference
from gen_dinf import GenerateDinf
from util import *
import numpy

def GenerateCellLatRaster():
    ds = ReadRaster(WORKING_DIR + os.sep + "taudir" + os.sep + filledDem)
    src_srs = ds.srs

    dst_srs = osr.SpatialReference()
    dst_srs.ImportFromEPSG(4326) ## WGS84
    dst_wkt = dst_srs.ExportToWkt()

    transform = osr.CoordinateTransformation(src_srs, dst_srs)

    point_ll = ogr.CreateGeometryFromWkt("POINT (%f %f)" % (ds.xMin, ds.yMin))
    point_ur = ogr.CreateGeometryFromWkt("POINT (%f %f)" % (ds.xMax, ds.yMax))

    point_ll.Transform(transform)
    point_ur.Transform(transform)

    lowerLat = point_ll.GetY()
    upLat = point_ur.GetY()

    rows = ds.nRows
    cols = ds.nCols

    data = ds.data
    dataLat = numpy.copy(data)
    deltaLat = (upLat - lowerLat) / float(rows)
    for row in range(rows):
        for col in range(cols):
            if dataLat[row][col] != ds.noDataValue:
                dataLat[row][col] = upLat - (row + 0.5) * deltaLat
    WriteGTiffFile(WORKING_DIR + os.sep + "taudir" + os.sep + cellLat,rows,cols,dataLat,
                   ds.geotrans,ds.srs,ds.noDataValue,GDT_Float32)
    #print lowerLat,upLat
def CalLatDependParas():
    '''
    Calculate latitude dependent parameters, include:
       1. minimum daylength (daylmn), 2. day length threshold for dormancy (dormhr)
    :return: GeoTIFF files
    '''
    ### calculate minimum daylength, from readwgn.f of SWAT
    ## daylength=2*acos(-tan(sd)*tan(lat))/omega
    ## where solar declination, sd, = -23.5 degrees for minimum daylength in
    ##                      northern hemisphere and -tan(sd) = .4348
    ##       absolute value is taken of tan(lat) to convert southern hemisphere
    ##                      values to northern hemisphere
    ##       the angular velocity of the earth's rotation, omega, = 15 deg/hr or
    ##                      0.2618 rad/hr and 2/0.2618 = 7.6394
    cellLatR = ReadRaster(WORKING_DIR + os.sep + "taudir" + os.sep + cellLat)
    latData = cellLatR.data
    daylmnData = numpy.copy(latData)
    zero = numpy.zeros((cellLatR.nRows, cellLatR.nCols))
    nodata = numpy.ones((cellLatR.nRows, cellLatR.nCols)) * cellLatR.noDataValue
    ## convert degrees to radians (2pi/360=1/57.296)
    daylmnData = 0.4348 * numpy.abs(numpy.tan(daylmnData / 57.296))
    condition = daylmnData < 1.
    daylmnData = numpy.where(condition,numpy.arccos(daylmnData),zero)
    condition2 = latData != cellLatR.noDataValue
    daylmnData = daylmnData * 7.6394
    daylmnData = numpy.where(condition2, daylmnData, nodata)
    WriteGTiffFile(WORKING_DIR + os.sep + "taudir" + os.sep + daylMin,cellLatR.nRows, cellLatR.nCols,daylmnData,
                   cellLatR.geotrans,cellLatR.srs,cellLatR.noDataValue, GDT_Float32)
    ## calculate day length threshold for dormancy
    dormhrData = numpy.copy(latData)
    if dorm_hr < -UTIL_ZERO:
        for i in range(cellLatR.nRows):
            for j in range(cellLatR.nCols):
                if dormhrData[i][j] != cellLatR.noDataValue:
                    tmpLat = dormhrData[i][j]
                    if tmpLat <= 40. and tmpLat >= 20.:
                        dormhrData[i][j] = (numpy.abs(tmpLat - 20.)) / 20.
                    elif tmpLat > 40.:
                        dormhrData[i][j] = 1.
                    elif tmpLat < 20.:
                        dormhrData[i][j] = -1.
    else:
        defaultNormHr = numpy.ones((cellLatR.nRows, cellLatR.nCols)) * dorm_hr
        dormhrData = numpy.where(condition2,defaultNormHr,nodata)
    WriteGTiffFile(WORKING_DIR + os.sep + "taudir" + os.sep + dormhr,cellLatR.nRows, cellLatR.nCols,dormhrData,
                   cellLatR.geotrans,cellLatR.srs,cellLatR.noDataValue, GDT_Float32)
def SubbasinDelineation(np, workingDir, dem, outlet, threshold, mpiexeDir=None,exeDir=None):
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
        maxAccum, minAccum, meanAccum, STDAccum = GetRasterStat(accD8)
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

    status = "Calculating distance to stream (D8)..."
    fStatus.write("%d,%s\n" % (95, status))
    fStatus.flush()
    print D8DistDownToStream(np, tauDir, flowDir, filledDem, streamRaster, dist2StreamD8, D8DownMethod, 1, mpiexeDir=mpiexeDir, exeDir=exeDir)
    print "[Output], %s, %s" % (workingDir, status)

    fStatus.write("100,subbasin delineation is finished!")
    fStatus.close()

    ## Get spatial reference from Source DEM file
    ds = gdal.Open(dem)
    projWkt = ds.GetProjection()
    srs = SpatialReference()
    srs.ImportFromWkt(projWkt)
    ## Write Projection Configuration file
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

    ## Convert to WGS84 (EPSG:4326)
    GenerateCellLatRaster()
    ## Calculate parameters dependent on latitude
    CalLatDependParas()
if __name__ == "__main__":
    CalLatDependParas()