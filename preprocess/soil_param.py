# -*- coding: utf-8 -*-
"""
Created on Thu Jan 10 15:36:19 2013

@author: junzhi
"""
from math import *
from osgeo import gdal
from pyproj import Proj, transform
from numpy import *
from util import *
from config import *
import os

NODATA = -9999

## Calculate soil properties from sand, clay and organic matter.
## TODO, add reference.
def GetProperties(s, c, om):
    #wilting point
    wpt = -0.024*s + 0.487*c + 0.006*om + 0.005*s*om - 0.013*c*om + 0.068*s*c + 0.031
    wp = 1.14*wpt - 0.02
    
    #field capacity
    fct = -0.251*s + 0.195*c + 0.011*om + 0.006*s*om - 0.027*c*om + 0.452*s*c + 0.299
    fc = fct + 1.283*fct*fct - 0.374*fct - 0.015
    
    s33t = 0.278*s + 0.034*c + 0.022*om - 0.018*s*om - 0.027*c*om - 0.584*s*c + 0.078
    s33 = 1.636*s33t - 0.107
    
    sat = fc + s33 - 0.097*s + 0.043
    
    pn = 2.65*(1.0 - sat)
    
    # df means density effects
    p_df = pn
    sat_df = 1 - p_df/2.65
    fc_df = fc - 0.2*(sat - sat_df)

    # pore disconnectedness index
    b = (log(1500.) - log(33.)) / (log(fc) - log(wp))
    lamda = 1.0/b
    
    # saturated conductivity
    #print s, c, sat, fc, 3-lamda
    ks = 1930*pow(sat-fc, 3-lamda)
    
    #print wpt, fct, s33, fc, sat
    return wp, fc_df, sat_df, p_df, ks, lamda

def GetValue(geoMask, geoMap, data, i, j):
    #pGeo = Proj("+proj=longlat +ellps=krass +no_defs")
    #pAlbers = Proj("+proj=aea +ellps=krass +lon_0=105 +lat_0=0 +lat_1=25 +lat_2=47")
    #xMask = geoMask[0] + (j+0.5)*geoMask[1]
    #yMask = geoMask[3] + (i+0.5)*geoMask[5]
    #xMap, yMap = transform(pAlbers, pGeo, xMask, yMask)

    xMap = geoMask[0] + (j+0.5)*geoMask[1]
    yMap = geoMask[3] + (i+0.5)*geoMask[5]

    jMap = (xMap - geoMap[0])/geoMap[1]
    iMap = (yMap - geoMap[3])/geoMap[5]

    return data[iMap][jMap]

    
def GenerateSoilAttributes(outputFolder, layerNum, sandFile=None, clayFile=None, somFile=None):
    maskFile = outputFolder + os.sep + mask_to_ext
#    if sandFile is None:
#        sandFile = '%s/sand%d_albers.img' % (SPATIAL_DATA_DIR, layerNum)
#    if clayFile is None:
#        clayFile = '%s/clay%d_albers.img' % (SPATIAL_DATA_DIR, layerNum)

    dataSom = None
    if somFile is not None and os.path.exists(somFile):
        dsSom = gdal.Open(somFile)
        bandSom = dsSom.GetRasterBand(1)
        dataSom = bandSom.ReadAsArray()

    print sandFile
    print clayFile
    dsSand = gdal.Open(sandFile)
    if dsSand is None:
        print "Open file:", sandFile, " failed.\n"
        return
    bandSand = dsSand.GetRasterBand(1)
    dataSand = bandSand.ReadAsArray()
    geoSand = dsSand.GetGeoTransform()
    
    dsClay = gdal.Open(clayFile)
    if dsClay is None:
        print "Open file:", clayFile, " failed.\n"
        return
    bandClay = dsClay.GetRasterBand(1)
    dataClay = bandClay.ReadAsArray()
    
    dsMask = gdal.Open(maskFile)
    bandMask = dsMask.GetRasterBand(1)
    dataMask = bandMask.ReadAsArray()
    xSizeMask = bandMask.XSize
    ySizeMask = bandMask.YSize
    geoMask = dsMask.GetGeoTransform()
    noDataValue = bandMask.GetNoDataValue()
    print noDataValue

    srs = osr.SpatialReference()
    srs.ImportFromWkt(dsMask.GetProjection())

    attrMapList = []
    n = 6 
    for i in range(n):
        attrMapList.append(zeros((ySizeMask, xSizeMask)))
    
    attrPool = {}
    lastKey = None
    for i in range(ySizeMask):
        #print "soil", i
        for j in range(xSizeMask):
            if abs(dataMask[i][j] - noDataValue) < DELTA:
                for iAttr in range(n):
                    attrMapList[iAttr][i][j] = NODATA
                continue
            s = GetValue(geoMask, geoSand, dataSand, i, j) 
            c = GetValue(geoMask, geoSand, dataClay, i, j) 

            k = "%d_%d_%d" % (int(s*1000), int(c*1000), layerNum)
            #print s, c

            if dataSom is not None:
                om = dataSom[i][j]
            else:
                om = defaultOrg  # om=2.5
                if layerNum >= 1:
                    om = 1.0

            if s < 0 or c < 0:
                attrs = attrPool[lastKey]
                k = lastKey
            elif k in attrPool.keys():
                attrs = attrPool[k]
            else:
                attrs = GetProperties(s * 0.01, c * 0.01, om)
                print dataMask[i][j], s, c
                #attrs = GetProperties(s, c, om)
                attrPool[k] = attrs
            lastKey = k

            for iAttr in range(n):
                attrMapList[iAttr][i][j] = attrs[iAttr]

    attrList = []
    layerStr = str(layerNum) if layerNum > 1 else ''
    for i in range(n):
        filename = outputFolder + os.sep + r"%s%s.tif" % (SOIL_ATTR_LIST[i+2], layerStr)
        WriteGTiffFile(filename, ySizeMask, xSizeMask, attrMapList[i], \
                                   geoMask, srs, NODATA, gdal.GDT_Float32)
        attrList.append(filename)

    return attrList
    

#if __name__ == "__main__":
    #testList = [[0.84627,0.04302,0.0208], [0.85,0.05,0.025], [0.65,0.1,0.025]]
    #for s in testList:
    #    print s[0], s[1], s[2]
    #    print GetProperties(s[0], s[1], s[2]*100)
#    layerNum = 1
#    inputFolder = r'F:\data\soilmap_china'
#    #outputFolder = r'F:\data\fenkeng_30m\data'
#    outputFolder = r'F:\data\poyanghu\fenkeng\data'
#    GenerateAttrMap(inputFolder, outputFolder, layerNum)
#    print 'done!'
