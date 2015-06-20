#! /usr/bin/env python
#coding=utf-8
from osgeo import gdal,osr
import math, os
from numpy import *
import util
from util import WriteGTiffFile
from config import *
def InitMoisture(dstdir):
    acc_name = dstdir + os.sep + accM
    slope_name = dstdir + os.sep + slopeM
    
    ds = gdal.Open(acc_name)
    band = ds.GetRasterBand(1)
    dataAcc = band.ReadAsArray()
    xsize = band.XSize
    ysize = band.YSize
    noDataValue = band.GetNoDataValue()
    if noDataValue is None:
        noDataValue = -9999
    
    srs = osr.SpatialReference()
    srs.ImportFromWkt(ds.GetProjection())    
            
    dsSlope = gdal.Open(slope_name)
    dataSlope = dsSlope.GetRasterBand(1).ReadAsArray()
    
    geotransform = ds.GetGeoTransform()
    dx = geotransform[1]
    cellArea = dx*dx
                
    wiGrid = zeros((ysize, xsize))
    wiMax = -1
    wiMin = 1000
    for i in range(0, ysize):
        for j in range(0, xsize):
            if(abs(dataAcc[i][j] - noDataValue) < util.DELTA):
                wiGrid[i][j] = -99
            else:
                if(abs(dataSlope[i][j]) < util.DELTA):
                    dataSlope[i][j] = 0.1/dx*100

                wiGrid[i][j] =  math.log( (dataAcc[i][j] + 1)*cellArea / (dataSlope[i][j]/100.) )
                if(wiGrid[i][j] > wiMax):
                    wiMax = wiGrid[i][j]
                elif(wiGrid[i][j] < wiMin):
                    wiMin = wiGrid[i][j]
    
    sMin = 0.6 #minimum relative saturation
    sMax = 1.0
    #wiUplimit = 0.8*wiMax
    wiUplimit = wiMax
    a = (sMax-sMin) / (wiUplimit-wiMin)
    b = sMin - a*wiMin
    
    moisture = zeros((ysize, xsize))
    for i in range(0, ysize):
        for j in range(0, xsize):
            if(abs(dataAcc[i][j] - noDataValue) < util.DELTA):
                moisture[i][j] = -9999
            else:
                moisture[i][j] =  a*wiGrid[i][j] + b
    
    filename = dstdir + os.sep + initSoilMoist
    WriteGTiffFile(filename, ysize, xsize, moisture, \
                       geotransform, srs, -9999, gdal.GDT_Float32)
    
    print 'The initial moisture is generated!'

    return filename
    
if __name__ == "__main__":
    init_moisture(r'F:\data\guanting\30m')
