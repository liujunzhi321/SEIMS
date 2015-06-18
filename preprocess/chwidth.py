#! /usr/bin/env python
#coding=utf-8
from osgeo import gdal,osr
import math, os
from numpy import *
import util

def chwidth(accFile, chwidthFile):
    ds = gdal.Open(accFile)
    band = ds.GetRasterBand(1)
    dataAcc = band.ReadAsArray()
    xsize = band.XSize
    ysize = band.YSize
    noDataValue = band.GetNoDataValue()
    if noDataValue is None:
        noDataValue = -9999
        
    srs = osr.SpatialReference()
    srs.ImportFromWkt(ds.GetProjection())
    
    geotransform = ds.GetGeoTransform()
    dx = geotransform[1]
    cellArea = dx*dx
    
    # storm frequency   a      b
    # 2                 1      0.56
    # 10                1.2    0.56
    # 100               1.4    0.56
    a = 1.2
    b = 0.56
    
    noDataValue = -9999
    width = zeros((ysize, xsize))
    for i in range(0, ysize):
        for j in range(0, xsize):
            if(abs(dataAcc[i][j] - noDataValue) < util.DELTA):
                width[i][j] = noDataValue
            else:
                width[i][j] =  math.pow(a*(dataAcc[i][j] + 1)*cellArea/1000000, b)
    
    util.WriteGTiffFile(chwidthFile, ysize, xsize, width, \
                        geotransform, srs, noDataValue, gdal.GDT_Float32)
    
    return width