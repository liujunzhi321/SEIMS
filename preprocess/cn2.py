#! /usr/bin/env python
#coding=utf-8

from osgeo import gdal,osr
import math, os
import sqlite3
from numpy import *
import util
from util import WriteGTiffFile
from config import *

def GenerateCN2(dstdir, dbname):
    maskFile = dstdir + os.sep + mask_to_ext
    landuseFile = dstdir + os.sep + landuseMFile
    hgFile = dstdir + os.sep + hydroGroup
    
    str_sql_lu = 'select LANDUSE_ID, CN2A, CN2B, CN2C, CN2D from LanduseLookup'
    
    conn = sqlite3.connect(dbname)
    cursor = conn.cursor()
    
    #cn2 list for each landuse type and hydrological group
    cn2Map = {}
    cursor.execute(str_sql_lu)
    for row in cursor:
        id = int(row[0])
        cn2List = []
        for i in range(4):
            cn2List.append(float(row[i+1]))
        cn2Map[id] = cn2List
    #print cn2Map
    
    ds = gdal.Open(maskFile)
    band = ds.GetRasterBand(1)
    data = band.ReadAsArray()
    xsize = band.XSize
    ysize = band.YSize
    noDataValue = band.GetNoDataValue()
    if noDataValue is None:
        noDataValue = 0
    geotransform = ds.GetGeoTransform()
    
    srs = osr.SpatialReference()
    srs.ImportFromWkt(ds.GetProjection())    
    
    ds = gdal.Open(hgFile)
    band = ds.GetRasterBand(1)    
    dataHg = band.ReadAsArray()
    
    ds = gdal.Open(landuseFile)
    band = ds.GetRasterBand(1)
    dataLanduse = band.ReadAsArray()    
    
    filename = dstdir + os.sep + CN2File
    data_prop = zeros((ysize, xsize))
    for i in range(0, ysize):
        for j in range(0, xsize):
            landuseID = int(dataLanduse[i][j])
            if(landuseID < 0):
                data_prop[i][j] = -9999
            else:
                hg = int(dataHg[i][j]) - 1 
                data_prop[i][j] = cn2Map[landuseID][hg]
    
    WriteGTiffFile(filename, ysize, xsize, data_prop, \
                           geotransform, srs, -9999, gdal.GDT_Float32)
            
    print 'The CN2 file is generated!'

    return filename
    
