#! /usr/bin/env python
#coding=utf-8
from osgeo import gdal, osr 
import math, os, sys
import sqlite3
from numpy import *
from config import *
import util

def RunoffCoefficent(filepath, sqliteFile):
    #read landuselookup table from sqlite
    prcFields = ["PRC_ST%d" % (i,) for i in range(1,13)]
    scFields = ["SC_ST%d" % (i,) for i in range(1,13)]
    sqlLanduse = 'select LANDUSE_ID, %s, %s from LanduseLookup' % \
                    (','.join(prcFields), ','.join(scFields))
    
    conn = sqlite3.connect(sqliteFile)
    cursor = conn.cursor()
    cursor.execute(sqlLanduse)
    
    runoff_c0 = {}
    runoff_s0 = {}    
    for row in cursor:
        id = int(row[0])
        runoff_c0[id] = [float(item) for item in row[1:13]]
        runoff_s0[id] = [float(item) for item in row[13:25]]
    
    cursor.close()
    conn.close()
    # end read data
    
    slope = filepath + os.sep + slopeM
    ds = gdal.Open(slope)
    band = ds.GetRasterBand(1)
    slo_data = band.ReadAsArray()
        
    mask = filepath + os.sep + mask_to_ext
    ds = gdal.Open(mask)
    band = ds.GetRasterBand(1)
    mask_data = band.ReadAsArray()   
    noDataValue1 = band.GetNoDataValue() 
        
    landuse = filepath + os.sep + landuseMFile
    ds = gdal.Open(landuse)
    band = ds.GetRasterBand(1)
    landu_data = band.ReadAsArray()
        
    soilTextureFile = filepath + os.sep + soilTexture
    ds = gdal.Open(soilTextureFile)
    band = ds.GetRasterBand(1)
    soilTextureArray = band.ReadAsArray()    
        
    xsize = band.XSize
    ysize = band.YSize
    #print xsize,ysize  226,77
    noDataValue2 = band.GetNoDataValue()
    geotransform = ds.GetGeoTransform()
    
    srs = osr.SpatialReference()
    srs.ImportFromWkt(ds.GetProjection())
    
    cellsize = geotransform[1]
    coef = zeros((ysize, xsize))
    #Set impervious percentage for urban cells
    imp = 0.3    
    
    defaultLanduseId = 8
    idOmited = []
    for i in range(0, ysize):
        for j in range(0, xsize):
            landuID = int(landu_data[i][j])
            if(abs(mask_data[i][j]-noDataValue1) < util.DELTA or landuID < 0):
                coef[i][j] = noDataValue2
                continue
            
            if not landuID in runoff_c0.keys():
                if not landuID in idOmited:
                    print 'The landuse ID: %d does not exist.' % (landuID,) 
                    idOmited.append(landuID)
                landuID = defaultLanduseId
            
            stid = int(soilTextureArray[i][j]) - 1
            
            c0 = runoff_c0[landuID][stid]
            s0 = runoff_s0[landuID][stid] / 100
            slp = slo_data[i][j] / 100
            
            if slp + s0 < 0.0001:
                coef[i][j] = c0
                continue
            
            coef1 = (1-c0) * slp /(slp+s0)
            coef2 = c0 + coef1
            if(landuID == 106 or landuID == 107 or landuID ==105):
                coef[i][j] = coef2*(1-imp)+imp
            else:
                coef[i][j] = coef2
                
            if coef[i][j] < 0:
                print c0, slp, s0, coe1, coef2
                            
    filename = filepath + os.sep + runoff_coefFile
    util.WriteGTiffFile(filename, ysize, xsize, coef, \
                               geotransform, srs, noDataValue2, gdal.GDT_Float32)
    
    print 'The Runoffcoefficient file is generated!'
        
