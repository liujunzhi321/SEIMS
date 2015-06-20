#! /usr/bin/env python
#coding=utf-8
#Author: Junzhi Liu
#Date: 2012-04-12
import os
from osgeo import gdal,ogr
from numpy import zeros
from config import *
import util
from util import CopyShpFile
from util import ReadRaster
from chwidth import chwidth

def GenerateSubbasinVector(dstdir, subbasinRaster, subbasinVector):
    subbasinVectorTmp = dstdir + os.sep + "subbasin_tmp.shp"
    util.RemoveShpFile(subbasinVectorTmp)
    strCmd = 'python %s/gdal_polygonize.py -f "ESRI Shapefile" %s %s' % (PREPROC_SCRIPT_DIR, subbasinRaster, subbasinVectorTmp)
    print strCmd
    os.system(strCmd)

    ds = ogr.Open(subbasinVectorTmp)
    layer = ds.GetLayer(0)
    layerDef = layer.GetLayerDefn()
    iDN = layerDef.GetFieldIndex("DN")

    dnDic = {}
    layer.ResetReading()
    ft = layer.GetNextFeature()
    while ft is not None:
        dn = ft.GetFieldAsInteger(iDN)
        dnDic.setdefault(dn,[]).append(ft)
        ft = layer.GetNextFeature()

    drv = ogr.GetDriverByName("ESRI Shapefile")
    util.RemoveShpFile(subbasinVector)
    dsNew = drv.CreateDataSource(subbasinVector)
    lyrName = "subbasin"
    lyr = dsNew.CreateLayer(lyrName, layer.GetSpatialRef(), ogr.wkbPolygon)
    layerDefSubbasin = lyr.GetLayerDefn()
    iSubbasin = layerDefSubbasin.GetFieldIndex("Subbasin")
    if(iSubbasin < 0):
        newField = ogr.FieldDefn("Subbasin", ogr.OFTInteger)
        lyr.CreateField(newField)
    for k in dnDic.keys():
        newFt = ogr.Feature(layerDefSubbasin)
        geom = dnDic[k][0].GetGeometryRef()
        if len(dnDic[k]) > 1:
            geom = ogr.Geometry(ogr.wkbPolygon)
            for ft in dnDic[k]:
                geom.AddGeometry(ft.GetGeometryRef().GetGeometryRef(0))
        newFt.SetGeometry(geom)
        newFt.SetField("Subbasin", k)
        lyr.CreateFeature(newFt)

    lyr.SyncToDisk()
    dsNew.Destroy()
    ds.Destroy()

def SerializeStreamNet(streamNetFile, outputReachFile):
    CopyShpFile(streamNetFile, outputReachFile)
    
    dsReach = ogr.Open(outputReachFile, update=True)
    layerReach = dsReach.GetLayer(0)
    layerDef = layerReach.GetLayerDefn()
    iLink = layerDef.GetFieldIndex("LINKNO")
    iLinkDownSlope = layerDef.GetFieldIndex("DSLINKNO")
    iLen = layerDef.GetFieldIndex("Length")
    
    oldIdList = []
    # there are some reaches with zero length. 
    # this program will remove these zero-length reaches
    # outputDic is used to store the downstream reaches of these zero-length reaches 
    outputDic = {} 
    ft = layerReach.GetNextFeature()
    while ft is not None:
        id = ft.GetFieldAsInteger(iLink)
        reachLen = ft.GetFieldAsDouble(iLen)
        if not id in oldIdList:
            if reachLen < util.DELTA:
                downstreamId = ft.GetFieldAsInteger(iLinkDownSlope)
                outputDic[id] = downstreamId 
            else:
                oldIdList.append(id)
        
        ft = layerReach.GetNextFeature()
    oldIdList.sort()

    idMap = {}
    n = len(oldIdList)
    for i in range(n):
        idMap[oldIdList[i]] = i+1
    #print idMap
    #change old ID to new ID
    layerReach.ResetReading()
    ft = layerReach.GetNextFeature()
    while ft is not None:
        id = ft.GetFieldAsInteger(iLink)
        if not id in idMap.keys():
            layerReach.DeleteFeature(ft.GetFID())
            ft = layerReach.GetNextFeature()
            continue

        dsId = ft.GetFieldAsInteger(iLinkDownSlope)
        dsId = outputDic.get(dsId, dsId)
        dsId = outputDic.get(dsId, dsId)

        ft.SetField("LINKNO", idMap[id])
        if dsId in idMap.keys():
            ft.SetField("DSLINKNO", idMap[dsId])
        else:
            #print dsId
            ft.SetField("DSLINKNO", -1)
        layerReach.SetFeature(ft)
        ft = layerReach.GetNextFeature()
    dsReach.ExecuteSQL("REPACK reach")
    layerReach.SyncToDisk()
    
    #add width field
    
    dsReach.Destroy()
    del dsReach
    
    return idMap

def SerializeSubbasin(subbasinFile, streamRasterFile, idMap, \
                            outputSubbasinFile, outputStreamLinkFile):
    subbasin = ReadRaster(subbasinFile)
    nRows = subbasin.nRows
    nCols = subbasin.nCols
    noDataValue = subbasin.noDataValue
    data = subbasin.data
    
    streamRaster = ReadRaster(streamRasterFile)
    dataStream = streamRaster.data
    noDataValueStream = streamRaster.noDataValue
    #print noDataValueStream
    
    outputSubbasin = zeros((nRows, nCols))
    outputStream = zeros((nRows, nCols))
    n = len(idMap)
    print "number of reaches: ", n
    for i in range(nRows):
        for j in range(nCols):
            if abs(data[i][j] - noDataValue) < util.DELTA:
                outputSubbasin[i][j] = noDataValue
            else:
                outputSubbasin[i][j] = idMap[int(data[i][j])]#error if the outlet subbasin contains only one grid, so there is no reach for this subbasin
                
            if dataStream[i][j] < util.DELTA:
                outputStream[i][j] = noDataValueStream  
            else:
                outputStream[i][j] = outputSubbasin[i][j]
    
    util.WriteGTiffFile(outputSubbasinFile, nRows, nCols, outputSubbasin, \
            subbasin.geotrans, subbasin.srs, noDataValue, gdal.GDT_Int32)
    util.WriteGTiffFile(outputStreamLinkFile, nRows, nCols, outputStream, \
            streamRaster.geotrans, streamRaster.srs, noDataValue, gdal.GDT_Int32)

            
def ChangeFlowDir(flowDirFileTau, flowDirFileEsri):
    dirMap = [1, 128, 64, 32, 16, 8, 4, 2]
    flowDirTau = ReadRaster(flowDirFileTau)
    nRows = flowDirTau.nRows
    nCols = flowDirTau.nCols
    noDataValue = flowDirTau.noDataValue
    dataTau = flowDirTau.data
    
    n = nRows * nCols
    dataTau.shape = n
    output = zeros(n)
    for i in range(n):
        if abs(dataTau[i] - noDataValue) < util.DELTA:
            output[i] = noDataValue
        else:
            value = int(dataTau[i]) - 1
            output[i] = dirMap[value]
    output.shape = (nRows, nCols)    
    util.WriteGTiffFile(flowDirFileEsri, nRows, nCols, output, \
            flowDirTau.geotrans, flowDirTau.srs, noDataValue, gdal.GDT_Int32)
        

def ChangeSlope(slopeFile, outputFile):
    slopeTau = ReadRaster(slopeFile)
    nRows = slopeTau.nRows
    nCols = slopeTau.nCols
    noDataValue = slopeTau.noDataValue
    data = slopeTau.data

    n = nRows * nCols
    data.shape = n
    output = zeros(n)
    
    for i in range(n):
        if abs(data[i] - noDataValue) < util.DELTA:
            output[i] = noDataValue
        else:
            output[i] = 100*data[i]
    output.shape = (nRows, nCols)    
    util.WriteGTiffFileByMask(outputFile, output, slopeTau, gdal.GDT_Float32)

def AddWidthToReach(reachFile, stramLinkFile, width):
    streamLink = ReadRaster(stramLinkFile)
    nRows = streamLink.nRows
    nCols = streamLink.nCols
    noDataValue = streamLink.noDataValue
    dataStream = streamLink.data
    dx = streamLink.dx
    
    chWidthDic = {}
    chNumDic = {}
    #streamRaster = zeros((nRows,nCols))
    for i in range(nRows):
        for j in range(nCols):
            #streamRaster[i][j] = dataStream[i][j]
            #if width[i][j] > dx:
            #    pass
            if abs(dataStream[i][j] - noDataValue) > util.DELTA:
                id = int(dataStream[i][j])
                chNumDic.setdefault(id, 0)
                chWidthDic.setdefault(id, 0)
                chNumDic[id] = chNumDic[id] + 1
                chWidthDic[id] = chWidthDic[id] + width[i][j]
    
    for k in chNumDic.keys():
        chWidthDic[k] = chWidthDic[k]/chNumDic[k]
    
    # add channel width field to reach shp file
    dsReach = ogr.Open(reachFile, update = True)
    layerReach = dsReach.GetLayer(0)
    layerDef = layerReach.GetLayerDefn()
    iLink = layerDef.GetFieldIndex("LINKNO")
    iWidth = layerDef.GetFieldIndex("Width")
    iDepth = layerDef.GetFieldIndex("Depth")
    if(iWidth < 0):
        new_field = ogr.FieldDefn("Width", ogr.OFTReal)
        layerReach.CreateField(new_field)
    if(iDepth < 0):
        new_field = ogr.FieldDefn("Depth", ogr.OFTReal)
        layerReach.CreateField(new_field)            
    #grid_code:feature map
    ftmap = {}
    layerReach.ResetReading()
    ft = layerReach.GetNextFeature()
    while ft is not None:
        id = ft.GetFieldAsInteger(iLink)
        w = 1
        if id in chWidthDic.keys():
            w = chWidthDic[id]
        ft.SetField("Width", w)
        ft.SetField("Depth", 5)
        layerReach.SetFeature(ft)
        ft = layerReach.GetNextFeature()
    
    layerReach.SyncToDisk()
    dsReach.Destroy()
    del dsReach

def PostProcessTauDEM(dstdir):
    tauDir = dstdir + os.sep + "taudir"
    streamNetFile = tauDir + os.sep + streamNet
    subbasinFile = tauDir + os.sep + subbasinM
    flowDirFileTau = tauDir + os.sep + flowDirM
    streamRasterFile = tauDir + os.sep + streamRasterM
    
    reachDir = dstdir + os.sep + "reaches"
    if not os.path.exists(reachDir):
        os.mkdir(reachDir)   
         
    outputReachFile = reachDir + os.sep + reachesOut
    outputSubbasinFile = dstdir + os.sep + subbasinOut
    outputFlowDirFile = dstdir + os.sep + flowDirOut
    outputStreamLinkFile = dstdir + os.sep + streamLinkOut

    subbasinDir = dstdir + os.sep + "subbasins"
    if not os.path.exists(subbasinDir):
        os.mkdir(subbasinDir)
    subbasinVectorFile = subbasinDir + os.sep + subbasinVec
        
    idMap = SerializeStreamNet(streamNetFile, outputReachFile)
    SerializeSubbasin(subbasinFile, streamRasterFile, idMap, \
                        outputSubbasinFile, outputStreamLinkFile)
    ChangeFlowDir(flowDirFileTau, outputFlowDirFile)
    
    accFile = dstdir + os.sep + accM
    chwidthFile = dstdir + os.sep + chwidthName
    width = chwidth(accFile, chwidthFile)
    AddWidthToReach(outputReachFile, outputStreamLinkFile, width)

    print "Generating subbasin vector..."
    GenerateSubbasinVector(dstdir, outputSubbasinFile, subbasinVectorFile)

    maskFile = dstdir + os.sep + mask_to_ext
    basinVectorTmp = dstdir + os.sep + "basin_tmp.shp"
    util.RemoveShpFile(basinVectorTmp)
    strCmd = 'python %s/gdal_polygonize.py -f "ESRI Shapefile" %s %s' % (PREPROC_SCRIPT_DIR, maskFile, basinVectorTmp)
    os.system(strCmd)

    ds = ogr.Open(basinVectorTmp)
    layer = ds.GetLayer(0)
    layerDef = layer.GetLayerDefn()

    basinVector = dstdir + os.sep + basinVec
    drv = ogr.GetDriverByName("ESRI Shapefile")
    util.RemoveShpFile(basinVector)
    dsNew = drv.CreateDataSource(basinVector)
    lyrName = "basin"
    lyr = dsNew.CreateLayer(lyrName, layer.GetSpatialRef(), ogr.wkbPolygon)
    newField = ogr.FieldDefn("Subbasin", ogr.OFTInteger)
    lyr.CreateField(newField)

    layerDefBasin = lyr.GetLayerDefn()
    newFt = ogr.Feature(layerDefBasin)

    geom = ogr.Geometry(ogr.wkbPolygon)
    ft = layer.GetNextFeature()
    while ft is not None:
        geom.AddGeometry(ft.GetGeometryRef().GetGeometryRef(0))
        ft = layer.GetNextFeature()
    newFt.SetGeometry(geom)
    newFt.SetField("Subbasin", 1)
    lyr.CreateFeature(newFt)
    lyr.SyncToDisk()
    dsNew.Destroy()
    ds.Destroy()
    
    #os.system("rm -rf %s" % (tauDir,))

if __name__ == '__main__':
    dstdir = r"D:\hydro_preprocessing\taudem"
    streamNetFile = dstdir + os.sep + "streamNet.shp"
    subbasinFile = dstdir + os.sep + "subbasinTau.tif"
    flowDirFileTau = dstdir + os.sep + "flowDir.tif"
    streamRasterFile = dstdir + os.sep + "streamRaster.tif"
    
    outputReachFile = dstdir + os.sep + "reach.shp"
    outputSubbasinFile = dstdir + os.sep + "subbasin.tif"
    outputFlowDirFile = dstdir + os.sep + "flow_dir.tif"
    outputStreamFile = dstdir + os.sep + "stream_link.tif"
    
    idMap = SerializeStreamNet(streamNetFile, outputReachFile)
    SerializeSubbasin(subbasinFile, streamRasterFile, idMap, \
                            outputSubbasinFile, outputStreamFile)
    #ChangeFlowDir(flowDirFileTau, outputFlowDirFile)
    
    print "The outputs of TauDEM are serialized."
