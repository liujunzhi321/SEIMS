#! /usr/bin/env python
#coding=utf-8
from osgeo import gdal,osr
import os, sys, re
import numpy
from time import time
import util
from config import *

def ReadCropAttrs(cropFile):
    if not os.path.exists(cropFile):
        cropFile = CROP_FILE
        
    f = open(cropFile)
    lines = f.readlines()
    f.close()
    
    attrDic = {}
    
    fields = [item.replace('"', '') \
                      for item in re.split(',|\n', lines[0]) if item is not '']
    n = len(fields)
    
    for i in xrange(n):
        attrDic[fields[i]] = {}
    
    for line in lines[1:]:
        items =[item.replace('"', '') \
                      for item in re.split(',|\n', line) if item is not '']
        id = int(items[0])
        
        for i in xrange(n):
            dic = attrDic[fields[i]]
            try:
                dic[id] = float(items[i])
            except:
                dic[id] = items[i]
    
    return attrDic

def ReclassCrop(landuseFile, dstdir):
    cropFile = CROP_FILE 
    attrMap = ReadCropAttrs(cropFile)
    
    attrNames = CROP_ATTR_LIST
    n = len(attrNames)
    
    ds = gdal.Open(landuseFile)
    band = ds.GetRasterBand(1)
    data = band.ReadAsArray()
    xsize = band.XSize
    ysize = band.YSize
    nAll = xsize * ysize
    data.shape = nAll
    
    noDataValue = band.GetNoDataValue()
    geotransform = ds.GetGeoTransform()
    
    srs = osr.SpatialReference()
    srs.ImportFromWkt(ds.GetProjection())    
    
    print 'Generating crop properties...'
    attrList = []
    for iAttr in range(n):
        name = attrNames[iAttr]
        print "\t", name
        filename = dstdir + os.sep + name + ".tif"
        attrList.append(filename)
        #t = time()
        
        dataAttr = numpy.zeros(nAll)
        
        dic = attrMap[name]
        for i in xrange(nAll):
            id = int(data[i])
            if id == 192:
                id = 12
            if(id > 0):
                dataAttr[i] = dic[id]
            else:
                dataAttr[i] = -9999
        
        #print time() - t
        
        dataAttr.shape = (ysize, xsize)
        util.WriteGTiffFile(filename, ysize, xsize, dataAttr, \
                                geotransform, srs, -9999, gdal.GDT_Float32)
            
    
    print 'The crop parameters are generated!'

    return attrList
    

if __name__ == "__main__":
    landuseFile = r'D:\hydro_preprocessing\taudem\landuse.tif'
    dstdir = r'D:\hydro_preprocessing\tmp'
    
    ReclassCrop(landuseFile, dstdir)
    
    print 'done!'
