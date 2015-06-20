#! /usr/bin/env python
#coding=utf-8

from osgeo import gdal, osr
import math, os
import sqlite3
from numpy import *
import util
from util import WriteGTiffFile
from config import *

def GenerateRadius(filepath, stormProbability):
    
    accfile = filepath + os.sep + accM
    ds = gdal.Open(accfile)
    band = ds.GetRasterBand(1)
    data = band.ReadAsArray()
    xsize = band.XSize
    ysize = band.YSize
    noDataValue = band.GetNoDataValue()
    geotransform = ds.GetGeoTransform()
    
    srs = osr.SpatialReference()
    srs.ImportFromWkt(ds.GetProjection())    
    
    cellsize = geotransform[1]
    ap = coeTable[stormProbability][0]
    bp = coeTable[stormProbability][1]
    radius = zeros((ysize, xsize))
    for i in range(0, ysize):
        for j in range(0, xsize):
            if(abs(data[i][j]-noDataValue) < util.DELTA ):
                radius[i][j] = -9999
                continue
            data[i][j] = data[i][j] + 1
            temp = ap * (data[i][j]*cellsize*cellsize/1000000)
            #print temp, bp
            radius[i][j] = math.pow(temp, bp)
    
    filename = filepath + os.sep + radiusFile
    WriteGTiffFile(filename, ysize, xsize, radius, \
                               geotransform, srs, -9999, gdal.GDT_Float32)
            
    print 'The radius file is generated!'

    return filename

if __name__ == "__main__":
    accfilepath = r'F:\data\guanting\model_zjk_270m\data'
    GenerateRadius(accfilepath, "T10")
    
