#! /usr/bin/env python
#coding=utf-8
from osgeo import gdal,osr
import math, os
import sqlite3
from numpy import *
import util
from config import *

def DepressionCap(filepath, sqliteFile):
    #read landuselookup table from sqlite
    stFields = ["DSC_ST%d" % (i,) for i in range(1,13)]
    sqlLanduse = 'select LANDUSE_ID,%s from LanduseLookup' % (','.join(stFields),)
    
    conn = sqlite3.connect(sqliteFile)
    cursor = conn.cursor()
    cursor.execute(sqlLanduse)
    
    dep_sd0 = {}   
    for row in cursor:
        id = int(row[0])
        dep_sd0[id] = [float(item) for item in row[1:]]
        
    cursor.close()
    conn.close()
    # end read data
    
    slope = filepath + os.sep + slopeM
    ds = gdal.Open(slope)
    band = ds.GetRasterBand(1)
    slo_data = band.ReadAsArray()
    
    soilTextureFile = filepath + os.sep + soilTexture
    ds = gdal.Open(soilTextureFile)
    band = ds.GetRasterBand(1)
    soilTextureArray = band.ReadAsArray()   
     
    mask = filepath + os.sep + mask_to_ext
    ds = gdal.Open(mask)
    band = ds.GetRasterBand(1)
    mask_data = band.ReadAsArray()    
    noDataValue1 = band.GetNoDataValue() 
    #print mask_data
    
    landuse = filepath + os.sep + landuseMFile
    ds = gdal.Open(landuse)
    band = ds.GetRasterBand(1)
    landu_data = band.ReadAsArray()
    
    xsize = band.XSize
    ysize = band.YSize
    #print xsize,ysize
    noDataValue2 = band.GetNoDataValue()
    geotransform = ds.GetGeoTransform()
    
    srs = osr.SpatialReference()
    srs.ImportFromWkt(ds.GetProjection())    
    
    cellsize = geotransform[1]
    depStorageCap = zeros((ysize, xsize))
    #Set impervious percentage for urban cells
    imp = 0.3    
    
    defaultLanduseId = 8
    idOmited = []
    lastStid = 0
    for i in range(0, ysize):
        for j in range(0, xsize):
            if(abs(mask_data[i][j]-noDataValue1) < util.DELTA):
                depStorageCap[i][j] = noDataValue2
                continue
            landuID = int(landu_data[i][j])
            
            if not landuID in dep_sd0.keys():
                if not landuID in idOmited:
                    print 'The landuse ID: %d does not exist.' % (landuID,) 
                    idOmited.append(landuID)
                landuID = defaultLanduseId
            
            stid = int(soilTextureArray[i][j]) - 1
            try:
                depressionGrid0 = dep_sd0[landuID][stid]
                lastStid = stid
            except:
                depressionGrid0 = dep_sd0[landuID][lastStid]
                
            tempGrid1 = log(depressionGrid0 + 0.0001)
            theSlopeGrid = slo_data[i][j] / 100
            tempGrid2 = theSlopeGrid * (-9.5)
            depressionGrid = math.exp(tempGrid1 + tempGrid2)
            
            if(landuID == 106 or landuID == 107 or landuID ==105):
                depStorageCap[i][j] = 0.5*imp+(1-imp)*depressionGrid
            else:
                depStorageCap[i][j] = depressionGrid
                
    filename = filepath + os.sep + depressionFile   
    util.WriteGTiffFile(filename, ysize, xsize, depStorageCap, \
                            geotransform, srs, noDataValue2, gdal.GDT_Float32)
        
    
    print 'The depression storage capacity is generated!'

    return filename
