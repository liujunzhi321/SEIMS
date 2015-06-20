#! /usr/bin/env python
#coding=utf-8
## @Reclass landuse
#
#
from osgeo import gdal, osr
import math, os, sys
import sqlite3
import numpy
from time import time
import util
from util import WriteGTiffFile
from config import *

def ReclassLanduse(landuseFile, dbname, dstdir):
    #code:{property_name:value}
    #for example:
    #1:{"clay":0.12, "sand":0.1}
    property_map = {}
    str_sql = 'select landuse_id, ' + ','.join(LANDUSE_ATTR_DB) + ' from LanduseLookup'
    property_namelist = LANDUSE_ATTR_LIST 
    num_propeties = len(property_namelist)
    
    for i in xrange(num_propeties):
        property_map[property_namelist[i]] = {}
            
    conn = sqlite3.connect(dbname)
    cursor = conn.cursor()
    
    cursor.execute(str_sql)
    for row in cursor:
        #print row
        id = int(row[0])
        for i in xrange(num_propeties):
            pName = property_namelist[i]
            dic = property_map[pName]
            if pName != "USLE_P":
                dic[id] = row[i+1]
            else:
                dic[id] = 1
    
    ds = gdal.Open(landuseFile)
    band = ds.GetRasterBand(1)
    data = band.ReadAsArray()

    xsize = band.XSize
    ysize = band.YSize
    noDataValue = band.GetNoDataValue()
    #print noDataValue
    geotransform = ds.GetGeoTransform()
    
    srs = osr.SpatialReference()
    srs.ImportFromWkt(ds.GetProjection())

    n = xsize * ysize
    data.shape = n
        
    attrList = []
    for iprop in xrange(num_propeties):
        pname = property_namelist[iprop]
        print "\t", pname
        
        filename = dstdir + os.sep + pname + ".tif"
        attrList.append(filename)
        
        #t = time()
        
        data_prop = numpy.zeros(n)
        dic = property_map[pname]
        
        for i in xrange(n):
            id = int(data[i])
            data_prop[i] = dic[id] if id > 0 else noDataValue
        
        #print time() - t
        
        data_prop.shape = (ysize, xsize)
        
        WriteGTiffFile(filename, ysize, xsize, data_prop, \
                                    geotransform, srs, noDataValue, gdal.GDT_Float32)
    
    print 'The landuse parameters are generated!'

    return attrList
    
if __name__ == '__main__':
    landuseFile = r'D:\hydro_preprocessing\taudem\landuse.tif'
    dstdir = r'D:\hydro_preprocessing\tmp'
    dbFile = r'D:\hydro_preprocessing\data\Parameter.db3'
    if len(sys.argv) >= 4:
        soilFile = sys.argv[1]
        dstdir = sys.argv[2]
        dbFile = sys.argv[3]
    
    ReclassLanduse(landuseFile, dbFile, dstdir)
    
