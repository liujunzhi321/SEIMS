#! /usr/bin/env python
#coding=utf-8

from osgeo import gdal, osr
import math, os, sys
import sqlite3
from numpy import *
from config import *
import util
from util import WriteGTiffFile

sys.setrecursionlimit(10000)

#the flowout of outletId is nodata
sqrt2 = math.sqrt(2)
celllen = {1:1,4:1,16:1,64:1, 2:sqrt2, 8:sqrt2, 32:sqrt2, 128:sqrt2}
deffer = {1:[0,1], 2:[1,1], 4:[1,0], 8:[1,-1],16:[0,-1],32:[-1,-1],64:[-1,0],128:[-1,1]}

def flowlen_cell(i,j,ysize,xsize,fdir,cellsize, weight, length):  
    #print i,j, weight[i][j]
    if(i<ysize and j<xsize):
        if(length[i][j] == 0):
            if(weight[i][j] > 0):
                prei = i 
                prej = j
                wt = weight[i][j]
                fdirV = fdir[i][j]
                di = deffer[fdirV][0]
                dj = deffer[fdirV][1]
                i = i + di
                j = j + dj
                relen = flowlen_cell(i,j,ysize,xsize,fdir,cellsize,weight, length)
                #print i,j,fdirV
                length[prei][prej] = cellsize * celllen[fdirV] * wt + relen
                return length[prei][prej]
            else:
                return 0
        if(length[i][j] > 0):
            return length[i][j]
        
        if(length[i][j] < 0):
            print "Error in calculating flowlen_cell function! i,j:"
            print i,j
            return -1  
    return 0
    
def cal_flowlen(filepath, weight):
    flow_dir = filepath + os.sep + flowDirOut
    ds = gdal.Open(flow_dir)
    band = ds.GetRasterBand(1)
    fdir_data = band.ReadAsArray()
    xsize = band.XSize
    ysize = band.YSize
    noDataValue = band.GetNoDataValue()
    geotransform = ds.GetGeoTransform()
    cellsize = geotransform[1]
    length = zeros((ysize, xsize))
    #weight = zeros((ysize, xsize))
    #weight[:] = 1
    
    for i in range(0, ysize):
        for j in range(0, xsize):
            if(abs(fdir_data[i][j]-noDataValue) < util.DELTA):
                length[i][j] = noDataValue
                continue
        
            flowlen_cell(i,j,ysize,xsize,fdir_data, cellsize, weight, length)
            
    return length


#coeTable = {"T2":[0.05, 0.48],"T10":[0.12, 0.52], "T100":[0.18,0.55]}

def GenerateDelta_s(filepath):
    
    streamlink = filepath + os.sep + streamLinkOut
    ds = gdal.Open(streamlink)
    band = ds.GetRasterBand(1)
    strlk_data = band.ReadAsArray() 
    
    radius = filepath + os.sep + radiusFile
    ds = gdal.Open(radius)
    band = ds.GetRasterBand(1)
    rad_data = band.ReadAsArray() 
    
    slope = filepath + os.sep + slopeM
    ds = gdal.Open(slope)
    band = ds.GetRasterBand(1)
    slo_data = band.ReadAsArray()
    
    velocity = filepath + os.sep + velocityFile
    ds = gdal.Open(velocity)
    band = ds.GetRasterBand(1)
    vel_data = band.ReadAsArray()
    
    xsize = band.XSize
    ysize = band.YSize
    noDataValue = band.GetNoDataValue()
    geotransform = ds.GetGeoTransform()
    
    srs = osr.SpatialReference()
    srs.ImportFromWkt(ds.GetProjection())    
    
    cellsize = geotransform[1]
    weight = zeros((ysize, xsize))
    delta_s = zeros((ysize, xsize))
    for i in range(0, ysize):
        for j in range(0, xsize):
            if(abs(vel_data[i][j]-noDataValue) < util.DELTA ):
                delta_s[i][j] = -9999
                continue
            if(strlk_data[i][j] <= 0):
                weight[i][j] = 1
            else:
                weight[i][j] = 0
            # 0 is river
            if(slo_data[i][j] < 0.0005 ):
                slo_data[i][j] = 0.0005
            dampGrid = vel_data[i][j] * rad_data[i][j] / (slo_data[i][j]/100*2)
            celerity = vel_data[i][j] * 5 / 3
            weight[i][j] = dampGrid*2 / math.pow(celerity, 3) * weight[i][j]
            
    delta_s_sqr = cal_flowlen(filepath, weight)
    for i in range(0, ysize):
        for j in range(0, xsize):
            if(abs(vel_data[i][j]-noDataValue) < util.DELTA ):
                continue
            
            delta_s[i][j] = math.sqrt(delta_s_sqr[i][j]) / 3600
                            
    filename = filepath + os.sep + delta_sFile
    WriteGTiffFile(filename, ysize, xsize, delta_s, \
                                geotransform, srs, -9999, gdal.GDT_Float32)
                     
    
    
    print 'The delta_s file is generated!'

    return filename

if __name__ == "__main__":
    filepath = r'E:\data\lygtest'
    GenerateDelta_s(filepath)
