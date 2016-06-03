#! /usr/bin/env python
#coding=utf-8
from osgeo import gdal, osr
import sqlite3
import numpy
from util import *
from text import *
import os, sys, re

# def ReclassCrop(cropCoverFile, dbname, dstdir):
#     #code:{property_name:value}
#     #for example:
#     #1:{"clay":0.12, "sand":0.1}
#     property_map = {}
#     str_sql = 'select idc, ' + ','.join(CROP_ATTR_LIST) + ' from CropLookup'
#     property_namelist = CROP_ATTR_LIST
#     num_propeties = len(property_namelist)
#
#     for i in xrange(num_propeties):
#         property_map[property_namelist[i]] = {}
#
#     conn = sqlite3.connect(dbname)
#     cursor = conn.cursor()
#
#     cursor.execute(str_sql)
#     for row in cursor:
#         #print row
#         id = int(row[0])
#         for i in xrange(num_propeties):
#             pName = property_namelist[i]
#             dic = property_map[pName]
#
#     ds = gdal.Open(cropCoverFile)
#     band = ds.GetRasterBand(1)
#     data = band.ReadAsArray()
#
#     xsize = band.XSize
#     ysize = band.YSize
#     noDataValue = band.GetNoDataValue()
#     #print noDataValue
#     geotransform = ds.GetGeoTransform()
#
#     srs = osr.SpatialReference()
#     srs.ImportFromWkt(ds.GetProjection())
#
#     n = xsize * ysize
#     data.shape = n
#
#     attrList = []
#     for iprop in xrange(num_propeties):
#         pname = property_namelist[iprop]
#         print "\t", pname
#
#         filename = dstdir + os.sep + pname + ".tif"
#         attrList.append(filename)
#
#         #t = time()
#
#         data_prop = numpy.zeros(n)
#         dic = property_map[pname]
#
#         for i in xrange(n):
#             id = int(data[i])
#             data_prop[i] = dic[id] if id > 0 else noDataValue
#
#         data_prop.shape = (ysize, xsize)
#
#         WriteGTiffFile(filename, ysize, xsize, data_prop, geotransform, srs, noDataValue, gdal.GDT_Float32)
#
#     print 'The landuse parameters are generated!'
#
#     return attrList




## Duplicate the function in reclass_landuse.py
## And deprecated this. By LJ, 2016-6-3
def ReadCropAttrs(cropFile):
    if not os.path.exists(cropFile):
        cropFile = CROP_FILE

    f = open(cropFile)
    lines = f.readlines()
    f.close()

    attrDic = {}

    fields = [item.replace('"', '') \
                      for item in re.split('\t|\n', lines[0]) if item is not '']
    n = len(fields)

    for i in xrange(n):
        attrDic[fields[i]] = {}

    for line in lines[2:]:
        items =[item.replace('"', '') \
                      for item in re.split('\t', line) if item is not '']
        id = int(items[0])

        for i in xrange(n):
            dic = attrDic[fields[i]]
            try:
                dic[id] = float(items[i])
            except:
                dic[id] = items[i]

    return attrDic

def GenerateInitLandcoverFile(landuseFile, dstdir):
    LUFile = ReadRaster(landuseFile)
    xsize = LUFile.nCols
    ysize = LUFile.nRows
    nAll = xsize * ysize
    data = LUFile.data
    data.shape = nAll
    dataAttr = numpy.zeros(nAll)
    noDataValue = LUFile.noDataValue
    geotransform = LUFile.geotrans
    srs = LUFile.srs
    lu2crop = {192:12, 101:DEFAULT_NODATA, 102:DEFAULT_NODATA, 103:DEFAULT_NODATA,
               104:DEFAULT_NODATA,105:DEFAULT_NODATA,106:DEFAULT_NODATA,
               107:DEFAULT_NODATA,108:DEFAULT_NODATA,191:33,98:DEFAULT_NODATA}
    print 'Generating crop/landcover GTIFF file...'
    filename = dstdir + os.sep + cropMFile
    ### Get all crop/landcover code
    attrList = []
    for i in xrange(nAll):
        if data[i] == noDataValue:
            dataAttr[i] = DEFAULT_NODATA
        else:
            id = int(data[i])
            if id in lu2crop.keys():
                dataAttr[i] = lu2crop[id]
            if(id < 98):
                dataAttr[i] = id
                if(id not in attrList):
                    attrList.append(id)
            else:
                dataAttr[i] = DEFAULT_NODATA
    dataAttr.shape = (ysize, xsize)
    WriteGTiffFile(filename, ysize, xsize, dataAttr, geotransform, srs, DEFAULT_NODATA, gdal.GDT_Float32)
    return attrList
    
def ReclassCrop(landuseFile, dstdir):
    LandCoverCodes = GenerateInitLandcoverFile(landuseFile, dstdir)
    cropFile = CROP_FILE
    attrMap = ReadCropAttrs(cropFile)
    attrNames = CROP_ATTR_LIST
    n = len(attrNames)

    replaceDicts = []
    dstCropTifs  = []
    for i in range(n):
        curAttr = attrNames[i]
        curDict = {}
        dic = attrMap[curAttr]
        for code in LandCoverCodes:
            if code not in curDict.keys():
                curDict[code] = dic[code]
        replaceDicts.append(curDict)
        dstCropTifs.append(dstdir+os.sep+curAttr+'.tif')
    # print replaceDicts
    # print(len(replaceDicts))
    # print dstCropTifs
    # print(len(dstCropTifs))
    ## Generate GTIFF
    for i in range(len(dstCropTifs)):
        print dstCropTifs[i]
        replaceByDict(dstdir + os.sep + cropMFile,replaceDicts[i],dstCropTifs[i])
if __name__ == "__main__":
    landuseFile = r'E:\data\model_data\model_dianbu_30m_longterm\data_prepare\output\landuse.tif'
    dstdir = r'E:\data\model_data\model_dianbu_30m_longterm\data_prepare\output'
    
    ReclassCrop(landuseFile, dstdir)
    print 'done!'
