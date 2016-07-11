#! /usr/bin/env python
# coding=utf-8
# Post process of TauDEM
#   1. convert subbasin raster to polygon shapefile
#   2. add width and default depth to reach.shp
# Author: Junzhi Liu, 2012-4-12
# Revised: Liang-Jun Zhu, 2016-7-7
#
import sys
from numpy import zeros
from config import *
from util import *
from chwidth import chwidth


def GenerateSubbasinVector(dstdir, subbasinRaster, subbasinVector):
    # subbasinVectorTmp = dstdir + os.sep + "subbasin_tmp.shp"
    # RemoveShpFile(subbasinVectorTmp)
    RemoveShpFile(subbasinVector)
    # raster to polygon vector
    strCmd = '%s %s/gdal_polygonize.py -f "ESRI Shapefile" %s %s %s %s' % \
             (sys.executable, PREPROC_SCRIPT_DIR, subbasinRaster, subbasinVector, "subbasin", FLD_SUBBASINID)
    print strCmd
    os.system(strCmd)
    # read polygon shape and generate another subbasin.shp.
    # This is redunctant and should be deprecated. By LJ
    # ds = ogr.Open(subbasinVectorTmp)
    # layer = ds.GetLayer(0)
    # layerDef = layer.GetLayerDefn()
    # iDN = layerDef.GetFieldIndex(FLD_SUBBASINID)
    #
    # dnDic = {}
    # layer.ResetReading()
    # ft = layer.GetNextFeature()
    # while ft is not None:
    #     dn = ft.GetFieldAsInteger(iDN)
    #     dnDic.setdefault(dn, []).append(ft)
    #     ft = layer.GetNextFeature()
    #
    # drv = ogr.GetDriverByName("ESRI Shapefile")
    # RemoveShpFile(subbasinVector)
    # dsNew = drv.CreateDataSource(subbasinVector)
    # lyrName = "subbasin"
    # lyr = dsNew.CreateLayer(lyrName, layer.GetSpatialRef(), ogr.wkbPolygon)
    # layerDefSubbasin = lyr.GetLayerDefn()
    # iSubbasin = layerDefSubbasin.GetFieldIndex("Subbasin")
    # if (iSubbasin < 0):
    #     newField = ogr.FieldDefn("Subbasin", ogr.OFTInteger)
    #     lyr.CreateField(newField)
    # for k in dnDic.keys():
    #     newFt = ogr.Feature(layerDefSubbasin)
    #     geom = dnDic[k][0].GetGeometryRef()
    #     if len(dnDic[k]) > 1:
    #         geom = ogr.Geometry(ogr.wkbPolygon)
    #         for ft in dnDic[k]:
    #             geom.AddGeometry(ft.GetGeometryRef().GetGeometryRef(0))
    #     newFt.SetGeometry(geom)
    #     newFt.SetField("Subbasin", k)
    #     lyr.CreateFeature(newFt)
    #
    # lyr.SyncToDisk()
    # dsNew.Destroy()
    # ds.Destroy()


def SerializeStreamNet(streamNetFile, outputReachFile):
    CopyShpFile(streamNetFile, outputReachFile)

    dsReach = ogr.Open(outputReachFile, update = True)
    layerReach = dsReach.GetLayer(0)
    layerDef = layerReach.GetLayerDefn()
    iLink = layerDef.GetFieldIndex(FLD_LINKNO)
    iLinkDownSlope = layerDef.GetFieldIndex(FLD_DSLINKNO)
    iLen = layerDef.GetFieldIndex(REACH_LENGTH)

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
            if reachLen < UTIL_ZERO:
                downstreamId = ft.GetFieldAsInteger(iLinkDownSlope)
                outputDic[id] = downstreamId
            else:
                oldIdList.append(id)

        ft = layerReach.GetNextFeature()
    oldIdList.sort()

    idMap = {}
    n = len(oldIdList)
    for i in range(n):
        idMap[oldIdList[i]] = i + 1
    # print idMap
    # change old ID to new ID
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

        ft.SetField(FLD_LINKNO, idMap[id])
        if dsId in idMap.keys():
            ft.SetField(FLD_DSLINKNO, idMap[dsId])
        else:
            # print dsId
            ft.SetField(FLD_DSLINKNO, -1)
        layerReach.SetFeature(ft)
        ft = layerReach.GetNextFeature()
    dsReach.ExecuteSQL("REPACK reach")
    layerReach.SyncToDisk()

    # add width field

    dsReach.Destroy()
    del dsReach

    return idMap


def SerializeSubbasin(subbasinFile, streamRasterFile, idMap,
                      outputSubbasinFile, outputStreamLinkFile):
    subbasin = ReadRaster(subbasinFile)
    nRows = subbasin.nRows
    nCols = subbasin.nCols
    noDataValue = subbasin.noDataValue
    data = subbasin.data

    streamRaster = ReadRaster(streamRasterFile)
    dataStream = streamRaster.data
    noDataValueStream = streamRaster.noDataValue
    # print noDataValueStream

    outputSubbasin = zeros((nRows, nCols))
    outputStream = zeros((nRows, nCols))
    n = len(idMap)
    print "number of reaches: ", n
    for i in range(nRows):
        for j in range(nCols):
            if abs(data[i][j] - noDataValue) < UTIL_ZERO:
                outputSubbasin[i][j] = noDataValue
            else:
                # error if the outlet subbasin contains only one grid, i.e., there is no reach for this subbasin
                outputSubbasin[i][j] = idMap[int(data[i][j])]
            if dataStream[i][j] < UTIL_ZERO:
                outputStream[i][j] = noDataValueStream
            else:
                outputStream[i][j] = outputSubbasin[i][j]

    WriteGTiffFile(outputSubbasinFile, nRows, nCols, outputSubbasin,
                   subbasin.geotrans, subbasin.srs, noDataValue, gdal.GDT_Int32)
    WriteGTiffFile(outputStreamLinkFile, nRows, nCols, outputStream,
                   streamRaster.geotrans, streamRaster.srs, noDataValue, gdal.GDT_Int32)


def ChangeFlowDir(flowDirFileTau, flowDirFileEsri):
    dirMap = {1: 1, 2: 128, 3: 64, 4: 32, 5: 16, 6: 8, 7: 4, 8: 2}
    replaceByDict(flowDirFileTau, dirMap, flowDirFileEsri)
    # dirMap = [1, 128, 64, 32, 16, 8, 4, 2]
    # flowDirTau = ReadRaster(flowDirFileTau)
    # nRows = flowDirTau.nRows
    # nCols = flowDirTau.nCols
    # noDataValue = flowDirTau.noDataValue
    # dataTau = flowDirTau.data
    #
    # n = nRows * nCols
    # dataTau.shape = n
    # output = zeros(n)
    # for i in range(n):
    #     if abs(dataTau[i] - noDataValue) < UTIL_ZERO:
    #         output[i] = noDataValue
    #     else:
    #         value = int(dataTau[i]) - 1
    #         output[i] = dirMap[value]
    # output.shape = (nRows, nCols)
    # WriteGTiffFile(flowDirFileEsri, nRows, nCols, output,
    #                flowDirTau.geotrans, flowDirTau.srs, noDataValue, gdal.GDT_Int32)


# NO NEED, by lj
# def ChangeSlope(slopeFile, outputFile):
#     slopeTau = ReadRaster(slopeFile)
#     # nRows = slopeTau.nRows
#     # nCols = slopeTau.nCols
#     # noDataValue = slopeTau.noDataValue
#     data = slopeTau.data
#     output = numpy.where(slopeTau.validZone, data * 100., data)
#     # n = nRows * nCols
#     # data.shape = n
#     # output = zeros(n)
#     #
#     # for i in range(n):
#     #     if abs(data[i] - noDataValue) < UTIL_ZERO:
#     #         output[i] = noDataValue
#     #     else:
#     #         output[i] = 100 * data[i]
#     # output.shape = (nRows, nCols)
#
#     WriteGTiffFileByMask(outputFile, output, slopeTau, gdal.GDT_Float32)


def AddWidthToReach(reachFile, stramLinkFile, width):
    streamLink = ReadRaster(stramLinkFile)
    nRows = streamLink.nRows
    nCols = streamLink.nCols
    noDataValue = streamLink.noDataValue
    dataStream = streamLink.data
    # dx = streamLink.dx

    chWidthDic = {}
    chNumDic = {}

    for i in range(nRows):
        for j in range(nCols):
            if abs(dataStream[i][j] - noDataValue) > UTIL_ZERO:
                id = int(dataStream[i][j])
                chNumDic.setdefault(id, 0)
                chWidthDic.setdefault(id, 0)
                chNumDic[id] = chNumDic[id] + 1
                chWidthDic[id] = chWidthDic[id] + width[i][j]

    for k in chNumDic.keys():
        chWidthDic[k] = chWidthDic[k] / chNumDic[k]

    # add channel width field to reach shp file
    dsReach = ogr.Open(reachFile, update = True)
    layerReach = dsReach.GetLayer(0)
    layerDef = layerReach.GetLayerDefn()
    iLink = layerDef.GetFieldIndex(FLD_LINKNO)
    iWidth = layerDef.GetFieldIndex(REACH_WIDTH)
    iDepth = layerDef.GetFieldIndex(REACH_DEPTH)
    if (iWidth < 0):
        new_field = ogr.FieldDefn(REACH_WIDTH, ogr.OFTReal)
        layerReach.CreateField(new_field)
    if (iDepth < 0):
        new_field = ogr.FieldDefn(REACH_DEPTH, ogr.OFTReal)
        layerReach.CreateField(new_field)
        # grid_code:feature map
    # ftmap = {}
    layerReach.ResetReading()
    ft = layerReach.GetNextFeature()
    while ft is not None:
        id = ft.GetFieldAsInteger(iLink)
        w = 1
        if id in chWidthDic.keys():
            w = chWidthDic[id]
        ft.SetField(REACH_WIDTH, w)
        ft.SetField(REACH_DEPTH, default_reach_depth)
        layerReach.SetFeature(ft)
        ft = layerReach.GetNextFeature()

    layerReach.SyncToDisk()
    dsReach.Destroy()
    del dsReach


def PostProcessTauDEM(dstdir):
    tauDir = dstdir + os.sep + DIR_NAME_TAUDEM
    streamNetFile = tauDir + os.sep + streamNet
    subbasinFile = tauDir + os.sep + subbasinM
    flowDirFileTau = tauDir + os.sep + flowDirM
    streamRasterFile = tauDir + os.sep + streamRasterM

    reachDir = dstdir + os.sep + DIR_NAME_REACH
    if not os.path.exists(reachDir):
        os.mkdir(reachDir)

    outputReachFile = reachDir + os.sep + reachesOut
    outputSubbasinFile = dstdir + os.sep + subbasinOut
    outputFlowDirFile = dstdir + os.sep + flowDirOut
    outputStreamLinkFile = dstdir + os.sep + streamLinkOut

    subbasinDir = dstdir + os.sep + DIR_NAME_SUBBSN
    if not os.path.exists(subbasinDir):
        os.mkdir(subbasinDir)
    subbasinVectorFile = subbasinDir + os.sep + subbasinVec

    idMap = SerializeStreamNet(streamNetFile, outputReachFile)
    SerializeSubbasin(subbasinFile, streamRasterFile, idMap,
                      outputSubbasinFile, outputStreamLinkFile)
    ## Change TauDEM code to ArcGIS. Now, it is deprecated, By LJ.
    # ChangeFlowDir(flowDirFileTau, outputFlowDirFile)
    shutil.copy(flowDirFileTau, outputFlowDirFile)

    accFile = dstdir + os.sep + accM
    chwidthFile = dstdir + os.sep + chwidthName
    width = chwidth(accFile, chwidthFile)
    AddWidthToReach(outputReachFile, outputStreamLinkFile, width)

    print "Generating subbasin vector..."
    GenerateSubbasinVector(dstdir, outputSubbasinFile, subbasinVectorFile)

    maskFile = dstdir + os.sep + mask_to_ext
    basinVector = dstdir + os.sep + basinVec
    # basinVectorTmp = dstdir + os.sep + "basin_tmp.shp"
    # RemoveShpFile(basinVectorTmp)
    RemoveShpFile(basinVector)
    strCmd = '%s %s/gdal_polygonize.py -f "ESRI Shapefile" %s %s %s %s' % \
             (sys.executable, PREPROC_SCRIPT_DIR, maskFile, basinVector, "basin", FLD_BASINID)
    os.system(strCmd)

    # ds = ogr.Open(basinVectorTmp)
    # layer = ds.GetLayer(0)
    # layerDef = layer.GetLayerDefn()
    #
    #
    # drv = ogr.GetDriverByName("ESRI Shapefile")
    # RemoveShpFile(basinVector)
    # dsNew = drv.CreateDataSource(basinVector)
    # lyrName = "basin"
    # lyr = dsNew.CreateLayer(lyrName, layer.GetSpatialRef(), ogr.wkbPolygon)
    # newField = ogr.FieldDefn("Subbasin", ogr.OFTInteger)
    # lyr.CreateField(newField)
    #
    # layerDefBasin = lyr.GetLayerDefn()
    # newFt = ogr.Feature(layerDefBasin)
    #
    # geom = ogr.Geometry(ogr.wkbPolygon)
    # ft = layer.GetNextFeature()
    # while ft is not None:
    #     geom.AddGeometry(ft.GetGeometryRef().GetGeometryRef(0))
    #     ft = layer.GetNextFeature()
    # newFt.SetGeometry(geom)
    # newFt.SetField("Subbasin", 1)
    # lyr.CreateFeature(newFt)
    # lyr.SyncToDisk()
    # dsNew.Destroy()
    # ds.Destroy()

    # os.system("rm -rf %s" % (tauDir,))


if __name__ == '__main__':
    PostProcessTauDEM(WORKING_DIR)
