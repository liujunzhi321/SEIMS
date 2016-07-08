#! /usr/bin/env python
# coding=utf-8
# Calculating depression capacity
# Author: Junzhi Liu
# Revised: Liang-Jun Zhu, 2016-7-6
# Note: Code optimization by using numpy.
#
import sqlite3
from util import *
from config import *


def DepressionCap(filepath, sqliteFile):
    # read landuselookup table from sqlite
    stFields = ["DSC_ST%d" % (i,) for i in range(1, 13)]
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
    # ds = gdal.Open(slope)
    # band = ds.GetRasterBand(1)
    # slo_data = band.ReadAsArray()
    slo_data = ReadRaster(slope).data

    soilTextureFile = filepath + os.sep + soilTexture
    # ds = gdal.Open(soilTextureFile)
    # band = ds.GetRasterBand(1)
    # soilTextureArray = band.ReadAsArray()
    soilTextureArray = ReadRaster(soilTextureFile).data

    mask = filepath + os.sep + mask_to_ext
    # ds = gdal.Open(mask)
    # band = ds.GetRasterBand(1)
    # mask_data = band.ReadAsArray()
    # noDataValue1 = band.GetNoDataValue()
    # print mask_data
    mask_R = ReadRaster(mask)
    mask_data = mask_R.data
    noDataValue1 = mask_R.noDataValue
    xsize = mask_R.nCols
    ysize = mask_R.nRows

    landuse = filepath + os.sep + landuseMFile
    # ds = gdal.Open(landuse)
    # band = ds.GetRasterBand(1)
    # landu_data = band.ReadAsArray()
    landu_R = ReadRaster(landuse)
    landu_data = landu_R.data
    noDataValue2 = landu_R.noDataValue

    # xsize = band.XSize
    # ysize = band.YSize
    # print xsize,ysize
    # noDataValue2 = band.GetNoDataValue()
    # geotransform = ds.GetGeoTransform()
    #
    # srs = osr.SpatialReference()
    # srs.ImportFromWkt(ds.GetProjection())
    #
    # cellsize = geotransform[1]
    # depStorageCap = numpy.zeros((ysize, xsize))

    defaultLanduseId = 8
    idOmited = []

    def calDep(mask, landu, soilTexture, slp):
        lastStid = 0
        if (abs(mask - noDataValue1) < UTIL_ZERO):
            return noDataValue2
        landuID = int(landu)
        if not landuID in dep_sd0.keys():
            if not landuID in idOmited:
                print 'The landuse ID: %d does not exist.' % (landuID,)
                idOmited.append(landuID)
            landuID = defaultLanduseId
        stid = int(soilTexture) - 1
        try:
            depressionGrid0 = dep_sd0[landuID][stid]
            lastStid = stid
        except:
            depressionGrid0 = dep_sd0[landuID][lastStid]

        depressionGrid = math.exp(numpy.log(depressionGrid0 + 0.0001) + slp / 100. * (-9.5))
        ## TODO, check if it is  (landuID >= 98)? By LJ
        if (landuID == 106 or landuID == 107 or landuID == 105):
            return 0.5 * imperviousPercInUrbanCell + (1. - imperviousPercInUrbanCell) * depressionGrid
        else:
            return depressionGrid

    calDep_numpy = numpy.frompyfunc(calDep, 4, 1)
    depStorageCap = calDep_numpy(mask_data, landu_data, soilTextureArray, slo_data)
    # for i in range(0, ysize):
    #     for j in range(0, xsize):
    #         if (abs(mask_data[i][j] - noDataValue1) < UTIL_ZERO):
    #             depStorageCap[i][j] = noDataValue2
    #             continue
    #         landuID = int(landu_data[i][j])
    #
    #         if not landuID in dep_sd0.keys():
    #             if not landuID in idOmited:
    #                 print 'The landuse ID: %d does not exist.' % (landuID,)
    #                 idOmited.append(landuID)
    #             landuID = defaultLanduseId
    #
    #         stid = int(soilTextureArray[i][j]) - 1
    #         try:
    #             depressionGrid0 = dep_sd0[landuID][stid]
    #             lastStid = stid
    #         except:
    #             depressionGrid0 = dep_sd0[landuID][lastStid]
    #
    #         tempGrid1 = numpy.log(depressionGrid0 + 0.0001)
    #         theSlopeGrid = slo_data[i][j] / 100.
    #         tempGrid2 = theSlopeGrid * (-9.5)
    #         depressionGrid = math.exp(tempGrid1 + tempGrid2)
    #         ## TODO, check if it is  (landuID >= 98)? By LJ
    #         if (landuID == 106 or landuID == 107 or landuID == 105):
    #             depStorageCap[i][j] = 0.5 * imperviousPercInUrbanCell + \
    #                                   (1. - imperviousPercInUrbanCell) * depressionGrid
    #         else:
    #             depStorageCap[i][j] = depressionGrid

    filename = filepath + os.sep + depressionFile
    WriteGTiffFile(filename, ysize, xsize, depStorageCap,
                   mask_R.geotrans, mask_R.srs, noDataValue2, gdal.GDT_Float32)

    print 'The depression storage capacity is generated!'
    return filename


if __name__ == '__main__':
    DepressionCap(WORKING_DIR, TXT_DB_DIR + os.sep + sqliteFile)
