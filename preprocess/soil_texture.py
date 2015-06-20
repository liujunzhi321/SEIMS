#! /usr/bin/env python
#coding=utf-8

import os
from osgeo import gdal
from numpy import zeros
from util import *
from config import *
def GetTexture(clay, sand):
    silt = 100 - sand - clay
    if (clay >= 40 and silt <= 40 and sand <= 45):
        return [12, 1, 0.22] # clay / nian tu
    elif (clay >= 40 and silt >= 40):
        return [11, 1, 0.26] # silt caly / fen nian tu
    elif (clay >= 35) and (sand >= 45):
        return [10, 1, 0.28] # sandy clay / sha nian tu
    elif (clay >= 25 and sand <= 45 and sand >= 20):
        return [9, 2, 0.3] # clay loam / nian rang tu
    elif (clay >= 25 and sand <= 20):
        return [8, 2, 0.32] # silt clay loam / fen zhi nian rang tu
    elif (clay >= 20 and silt <= 30 and sand >= 45):
        return [7, 2, 0.2] # sandy clay loam / sha zhi nian rang tu
    elif (clay >= 10) and (silt >= 30) and (silt <= 50) and (sand <= 50):
        return [6, 3, 0.3] # loam / rang tu
    elif (silt >= 50 and silt <= 80) or (clay >= 15 and silt >= 80):
        return [4, 3, 0.38] # silt loam / fen zhi rang tu
    elif (silt >= 80 and clay <= 15):
        return [5, 3, 0.34] # silt / fen tu
    elif (clay <= 10 and sand <= 50) or (sand >= 50 and sand <= 80):
        return [3, 4, 0.13] # sandy loam / sha zhi rang tu
    elif (sand <= 90):
        return [2, 4, 0.04] # loamy sand / rang zhi sha tu
    else:
        return [1, 4, 0.02] # sand / sha tu



def SoilTexture(workingDir):
    sandFile = workingDir + os.sep + "sand.tif"
    clayFile = workingDir + os.sep + "clay.tif"

    sandRaster = ReadRaster(sandFile)
    clayRaster = ReadRaster(clayFile)

    nCols = sandRaster.nCols
    nRows = sandRaster.nRows
    noDataValue = sandRaster.noDataValue

    dataSand = sandRaster.data
    dataClay = clayRaster.data

    st = zeros((nRows, nCols)) # soil texture code
    hg = zeros((nRows, nCols)) # hydrological group
    usleK_array = zeros((nRows, nCols)) # USLE K factor
    for i in range(nRows):
        for j in range(nCols):
            if abs(dataSand[i][j] - noDataValue) < DELTA:
                st[i][j] = -9999
                hg[i][j] = -9999
                usleK_array[i][j] = -9999
            else:
                values = GetTexture(dataClay[i][j], dataSand[i][j])
                #values = GetTexture(dataClay[i][j]*100, dataSand[i][j]*100)
                st[i][j] = values[0]
                hg[i][j] = values[1]
                usleK_array[i][j] = values[2]

    textureFile = workingDir + os.sep + soilTexture
    WriteGTiffFile(textureFile, nRows, nCols, st, sandRaster.geotrans,\
                          sandRaster.srs, -9999, gdal.GDT_Float32)
    hgFile = workingDir + os.sep + hydroGroup
    WriteGTiffFile(hgFile, nRows, nCols, hg, sandRaster.geotrans,\
                          sandRaster.srs, -9999, gdal.GDT_Float32)
    kFile = workingDir + os.sep + usleK
    WriteGTiffFile(kFile, nRows, nCols, usleK_array, sandRaster.geotrans,\
                          sandRaster.srs, -9999, gdal.GDT_Float32)

    print "Soil texture is generated."

    return [textureFile, hgFile, kFile]

if __name__ == "__main__":
    print GetTexture(60, 30)
    print GetTexture(50, 5)
    print GetTexture(36, 55)
    print GetTexture(36, 30)
    print GetTexture(36, 10)
    print GetTexture(25, 60)
    print GetTexture(15, 45)
    print GetTexture(10, 20)
    print GetTexture(5, 5)
    print GetTexture(10, 60)
    print GetTexture(5, 85)
    print GetTexture(5, 92)
