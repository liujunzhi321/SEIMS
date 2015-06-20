#! /usr/bin/env python
#coding=utf-8
import sys
from subbasin_delineation import * 
from post_process_taudem import *
#from parameters_extraction import *
from config import *
from osgeo import gdal,osr
from numpy import zeros
import util
from util import Raster
from util import WriteGTiffFile
from util import ReadRaster

def GetMaskFromRaster(rasterFile, dstdir):
    ds = gdal.Open(rasterFile)
    band = ds.GetRasterBand(1)
    data = band.ReadAsArray()
    xsize = band.XSize
    ysize = band.YSize
    noDataValue = band.GetNoDataValue()
    geotrans = ds.GetGeoTransform()
    
    srs = osr.SpatialReference()
    srs.ImportFromWkt(ds.GetProjection())
    
    xMin = geotrans[0]
    yMax = geotrans[3]
    dx = geotrans[1]
    #print ysize, xsize
    
    iMin = ysize-1
    iMax = 0
    jMin = xsize-1
    jMax = 0
    
    for i in range(ysize):
        for j in range(xsize):
            if abs(data[i][j] - noDataValue) > util.DELTA:
                iMin = min(i, iMin)
                iMax = max(i, iMax)
                jMin = min(j, jMin)
                jMax = max(j, jMax)
    
    #print iMin, iMax, jMin, jMax
    ySizeMask = iMax - iMin + 1
    xSizeMask = jMax - jMin + 1
    xMinMask = xMin + jMin*dx
    yMaxMask = yMax - iMin*dx
    print "%dx%d -> %dx%d" % (xsize, ysize, xSizeMask, ySizeMask)
    
    mask = zeros((ySizeMask, xSizeMask))
    
    for i in range(ySizeMask):
        for j in range(xSizeMask):
            if abs(data[i+iMin][j+jMin] - noDataValue) > util.DELTA:
                mask[i][j] = 1
            else:
                mask[i][j] = -9999
    
    outputFile = dstdir + os.sep + mask_to_ext
    maskGeotrans = [xMinMask, dx, 0, yMaxMask, 0, -dx]
    WriteGTiffFile(outputFile, ySizeMask, xSizeMask, mask, \
                            maskGeotrans, srs, -9999, gdal.GDT_Int32)
    
    return outputFile, Raster(ySizeMask, xSizeMask, mask, -9999, maskGeotrans, srs)
    
def MaskDEMFiles(workingDir,exeDir=None):
    tauDir = workingDir + os.sep + "taudir"
    subbasinTauFile = tauDir + os.sep + subbasin
    originalFiles = [subbasin, flowDir, streamRaster, 
                     slope, filledDem, acc, streamOrder,
                     flowDirDinf, dirCodeDinf, slopeDinf, weightDinf]
    originalFiles = [(tauDir + os.sep + item) for item in originalFiles]
    maskedFiles = [subbasinM, flowDirM, streamRasterM]
    maskedFiles = [(tauDir + os.sep + item) for item in maskedFiles]
    outputList = [slopeM, filldemM, accM,  streamOrderM, flowDirDinfM,
                    dirCodeDinfM, slopeDinfM, weightDinfM]
    for output in outputList:
        maskedFiles.append(workingDir + os.sep + output)

    maskFile, mask = GetMaskFromRaster(subbasinTauFile, workingDir)
        
    print "Mask files..."
    n = len(originalFiles)
    # write mask config file
    configFile = "%s%smaskDemConfig.txt" % (workingDir, os.sep)
    f = open(configFile, 'w')
    f.write(maskFile+"\n")
    f.write("%d\n" % (n,))
    for i in range(n):
        s = "%s\t%d\t%s\n" % (originalFiles[i], -9999, maskedFiles[i])
        f.write(s)
    f.close()

    s = "%s/mask_raster %s" % (exeDir, configFile)
    os.system(s)


def GenerateSubbasins(workingDir, proj4Str,exeDir=None):
    statusFile = workingDir + os.sep + "status_GenerateSubbasins.txt"
    fStatus = open(statusFile, 'w')

    fStatus.write("%d,%s\n" % (10, "Masking subbasin files..."))
    fStatus.flush()
    MaskDEMFiles(workingDir,exeDir=exeDir)

    fStatus.write("%d,%s\n" % (50, "Output files..."))
    fStatus.flush()
    PostProcessTauDEM(workingDir)

    os.chdir(workingDir)
    fileName = workingDir + os.sep + "river.json"
    if os.path.exists(fileName):
        os.remove(fileName)
    s = 'ogr2ogr -f GeoJSON -s_srs "%s" -t_srs EPSG:4326 river.json reaches/reach.shp' % (proj4Str)
    os.system(s)

    fileName = workingDir + os.sep + "subbasin.json"
    if os.path.exists(fileName):
        os.remove(fileName)
    s = 'ogr2ogr -f GeoJSON -s_srs "%s" -t_srs EPSG:4326 subbasin.json subbasins/subbasin.shp' % (proj4Str)
    os.system(s)

    fileName = workingDir + os.sep + "outlet.json"
    if os.path.exists(fileName):
        os.remove(fileName)
    s = 'ogr2ogr -f GeoJSON -s_srs "%s" -t_srs EPSG:4326 outlet.json taudir/outletM.shp' % (proj4Str)
    os.system(s)
    fStatus.write("%d,%s\n" % (100, "Finished!"))
    fStatus.close()