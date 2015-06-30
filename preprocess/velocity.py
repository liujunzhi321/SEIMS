#! /usr/bin/env python
#coding=utf-8

from osgeo import gdal, osr
import math, os
import sqlite3
from numpy import *
from config import *
import util
from util import WriteGTiffFile

def GenerateVelocity(filepath):
    
    slope = filepath + os.sep + slopeM
    ds = gdal.Open(slope)
    band = ds.GetRasterBand(1)
    slo_data = band.ReadAsArray()
    
    radius = filepath + os.sep + radiusFile
    ds = gdal.Open(radius)
    band = ds.GetRasterBand(1)
    rad_data = band.ReadAsArray()
    
    xsize = band.XSize
    ysize = band.YSize
    noDataValue = band.GetNoDataValue()
    geotransform = ds.GetGeoTransform()
    
    srs = osr.SpatialReference()
    srs.ImportFromWkt(ds.GetProjection())    
    
    cellsize = geotransform[1]
    
    Manning = filepath + os.sep + ManningFile
    ds = gdal.Open(Manning)
    band = ds.GetRasterBand(1)
    Man_data = band.ReadAsArray()
    
    vel_max = 3.0
    vel_min = 0.0005
    velocity = zeros((ysize, xsize))
    for i in range(0, ysize):
        for j in range(0, xsize):
            if(abs(rad_data[i][j]-noDataValue) < util.DELTA ):
                velocity[i][j] = -9999
                continue
            velocity[i][j] = math.pow(Man_data[i][j], -1) * math.pow(rad_data[i][j], 2/3) * math.pow(slo_data[i][j]/100, 0.5)
            if(velocity[i][j] < vel_min):
                velocity[i][j] = vel_min
            if(velocity[i][j] > vel_max):
                velocity[i][j] = vel_max
    
    filename = filepath + os.sep + velocityFile
    WriteGTiffFile(filename, ysize, xsize, velocity, \
                            geotransform, srs, -9999, gdal.GDT_Float32)
                    
        
    print 'The velocity file is generated!'

    return filename

if __name__ == "__main__":
    filepath = r'F:\data\guanting\model_zjk_270m\data'
    GenerateVelocity(filepath)
    
