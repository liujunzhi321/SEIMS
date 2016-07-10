#! /usr/bin/env python
# coding=utf-8
## Generation masked subbasin spatial data
# Author: Junzhi Liu
# Revised: Liang-Jun Zhu
# Note: Improve calculation efficiency by numpy
#
from post_process_taudem import *
from text import *
from config import *


def GetMaskFromRaster(rasterFile, dstdir):
    rasterR = ReadRaster(rasterFile)
    xsize = rasterR.nCols
    ysize = rasterR.nRows
    noDataValue = rasterR.noDataValue
    srs = rasterR.srs
    xMin = rasterR.xMin
    yMax = rasterR.yMax
    dx = rasterR.dx
    data = rasterR.data
    #
    # ds = gdal.Open(rasterFile)
    # band = ds.GetRasterBand(1)
    # data = band.ReadAsArray()
    # xsize = band.XSize
    # ysize = band.YSize
    # noDataValue = band.GetNoDataValue()
    # geotrans = ds.GetGeoTransform()
    #
    # srs = osr.SpatialReference()
    # srs.ImportFromWkt(ds.GetProjection())
    #
    # xMin = geotrans[0]
    # yMax = geotrans[3]
    # dx = geotrans[1]
    # print ysize, xsize

    iMin = ysize - 1
    iMax = 0
    jMin = xsize - 1
    jMax = 0

    for i in range(ysize):
        for j in range(xsize):
            if abs(data[i][j] - noDataValue) > UTIL_ZERO:
                iMin = min(i, iMin)
                iMax = max(i, iMax)
                jMin = min(j, jMin)
                jMax = max(j, jMax)

    # print iMin, iMax, jMin, jMax
    ySizeMask = iMax - iMin + 1
    xSizeMask = jMax - jMin + 1
    xMinMask = xMin + jMin * dx
    yMaxMask = yMax - iMin * dx
    print "%dx%d -> %dx%d" % (xsize, ysize, xSizeMask, ySizeMask)

    mask = zeros((ySizeMask, xSizeMask))

    for i in range(ySizeMask):
        for j in range(xSizeMask):
            if abs(data[i + iMin][j + jMin] - noDataValue) > UTIL_ZERO:
                mask[i][j] = 1
            else:
                mask[i][j] = DEFAULT_NODATA

    outputFile = dstdir + os.sep + mask_to_ext
    maskGeotrans = [xMinMask, dx, 0, yMaxMask, 0, -dx]
    WriteGTiffFile(outputFile, ySizeMask, xSizeMask, mask, maskGeotrans, srs, DEFAULT_NODATA, gdal.GDT_Int32)
    return outputFile, Raster(ySizeMask, xSizeMask, mask, DEFAULT_NODATA, maskGeotrans, srs)


def MaskDEMFiles(workingDir, exeDir = None):
    tauDir = workingDir + os.sep + DIR_NAME_TAUDEM
    subbasinTauFile = tauDir + os.sep + subbasin
    originalFiles = [subbasin, flowDir, streamRaster,
                     slope, filledDem, acc, streamOrder,
                     flowDirDinf, dirCodeDinf, slopeDinf, weightDinf, cellLat, daylMin, dormhr, dist2StreamD8]
    originalFiles = [(tauDir + os.sep + item) for item in originalFiles]
    originalFiles.append(mgtFiedlFile)
    maskedFiles = [subbasinM, flowDirM, streamRasterM]
    maskedFiles = [(tauDir + os.sep + item) for item in maskedFiles]
    outputList = [slopeM, filldemM, accM, streamOrderM, flowDirDinfM,
                  dirCodeDinfM, slopeDinfM, weightDinfM, cellLatM, daylMinM, dormhrM, dist2StreamD8M, mgtFiedlMFile]
    for output in outputList:
        maskedFiles.append(workingDir + os.sep + output)

    maskFile, mask = GetMaskFromRaster(subbasinTauFile, workingDir)

    print "Mask files..."
    n = len(originalFiles)
    # write mask config file
    configFile = "%s%s%s" % (workingDir, os.sep, FN_STATUS_MASKRASTERS)
    f = open(configFile, 'w')
    f.write(maskFile + "\n")
    f.write("%d\n" % (n,))
    for i in range(n):
        s = "%s\t%d\t%s\n" % (originalFiles[i], DEFAULT_NODATA, maskedFiles[i])
        f.write(s)
    f.close()

    s = "%s/mask_raster %s" % (exeDir, configFile)
    os.system(s)


def GenerateSubbasins():
    # f = open(WORKING_DIR + os.sep + "ProjConfig.txt")
    # proj4Str = f.readlines()[2].strip()
    # f.close()
    statusFile = WORKING_DIR + os.sep + FN_STATUS_GENSUBBSN
    fStatus = open(statusFile, 'w')

    fStatus.write("%d,%s\n" % (10, "Masking subbasin files..."))
    fStatus.flush()
    MaskDEMFiles(WORKING_DIR, exeDir = CPP_PROGRAM_DIR)

    fStatus.write("%d,%s\n" % (50, "Output files..."))
    fStatus.flush()
    PostProcessTauDEM(WORKING_DIR)

    os.chdir(WORKING_DIR)
    src_srs = ReadRaster(dem).srs
    proj_srs = src_srs.ExportToProj4()
    # print proj_srs
    wgs84_srs = "EPSG:4326"

    def convert2GeoJson(jsonFile, src_srs, dst_srs, src_file):
        if os.path.exists(jsonFile):
            os.remove(jsonFile)
        s = 'ogr2ogr -f GeoJSON -s_srs "%s" -t_srs %s %s %s' % \
            (src_srs, dst_srs, jsonFile, src_file)
        os.system(s)

    geoJson_dict = {GEOJSON_REACH : WORKING_DIR + os.sep + DIR_NAME_REACH + os.sep + reachesOut,
                    GEOJSON_SUBBSN: WORKING_DIR + os.sep + DIR_NAME_SUBBSN + os.sep + subbasinVec,
                    GEOJSON_OUTLET: WORKING_DIR + os.sep + DIR_NAME_TAUDEM + os.sep + modifiedOutlet}
    for jsonName in geoJson_dict.keys():
        convert2GeoJson(jsonName, proj_srs, wgs84_srs, geoJson_dict.get(jsonName))

    # fileName = WORKING_DIR + os.sep + "river.json"
    # if os.path.exists(fileName):
    #     os.remove(fileName)
    # s = 'ogr2ogr -f GeoJSON -s_srs "%s" -t_srs EPSG:4326 river.json reaches/reach.shp' % (proj_srs)
    # print s
    # os.system(s)
    #
    # fileName = WORKING_DIR + os.sep + "subbasin.json"
    # if os.path.exists(fileName):
    #     os.remove(fileName)
    # s = 'ogr2ogr -f GeoJSON -s_srs "%s" -t_srs EPSG:4326 subbasin.json subbasins/subbasin.shp' % (proj4Str)
    # os.system(s)
    #
    # fileName = WORKING_DIR + os.sep + "outlet.json"
    # if os.path.exists(fileName):
    #     os.remove(fileName)
    # s = 'ogr2ogr -f GeoJSON -s_srs "%s" -t_srs EPSG:4326 outlet.json %s/outletM.shp' % (proj4Str, DIR_NAME_TAUDEM)
    # os.system(s)
    fStatus.write("%d,%s\n" % (100, "Finished!"))
    fStatus.close()


if __name__ == "__main__":
    GenerateSubbasins()
