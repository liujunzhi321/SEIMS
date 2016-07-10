#! /usr/bin/env python
# coding=utf-8
# Generate SCS-CN2 value according to landuse and soil hydrological group.
# Author: Junzhi Liu
# Revised: Liang-Jun Zhu
# Note: 1. Using ReadRaster function from util module.
#       2. Using numpy.frompyfunc to replace native for loops
#
from util import *
from config import *
import sqlite3


def GenerateCN2(dstdir, dbname):
    maskFile = dstdir + os.sep + mask_to_ext
    landuseFile = dstdir + os.sep + landuseMFile
    hgFile = dstdir + os.sep + hydroGroup

    str_sql_lu = 'select LANDUSE_ID, CN2A, CN2B, CN2C, CN2D from LanduseLookup'

    conn = sqlite3.connect(dbname)
    cursor = conn.cursor()

    # cn2 list for each landuse type and hydrological group
    cn2Map = {}
    cursor.execute(str_sql_lu)
    for row in cursor:
        id = int(row[0])
        cn2List = []
        for i in range(4):
            cn2List.append(float(row[i + 1]))
        cn2Map[id] = cn2List
    # print cn2Map
    maskR = ReadRaster(maskFile)
    xsize = maskR.nCols
    ysize = maskR.nRows
    noDataValue = maskR.noDataValue

    # ds = gdal.Open(maskFile)
    # band = ds.GetRasterBand(1)
    # data = band.ReadAsArray()
    # xsize = band.XSize
    # ysize = band.YSize
    # noDataValue = band.GetNoDataValue()
    # if noDataValue is None:
    #     noDataValue = 0
    # geotransform = ds.GetGeoTransform()

    # srs = osr.SpatialReference()
    # srs.ImportFromWkt(ds.GetProjection())

    hgR = ReadRaster(hgFile)
    dataHg = hgR.data

    # ds = gdal.Open(hgFile)
    # band = ds.GetRasterBand(1)
    # dataHg = band.ReadAsArray()

    luR = ReadRaster(landuseFile)
    dataLanduse = luR.data

    # ds = gdal.Open(landuseFile)
    # band = ds.GetRasterBand(1)
    # dataLanduse = band.ReadAsArray()

    filename = dstdir + os.sep + CN2File

    # data_prop = numpy.zeros((ysize, xsize))

    def calCN2(landuseID, hg):
        landuseID = int(landuseID)
        if (landuseID < 0):
            return DEFAULT_NODATA
        else:
            hg = int(hg) - 1
            return cn2Map[landuseID][hg]

    calCN2_numpy = numpy.frompyfunc(calCN2, 2, 1)
    data_prop = calCN2_numpy(dataLanduse, dataHg)
    # for i in range(0, ysize):
    #     for j in range(0, xsize):
    #         landuseID = int(dataLanduse[i][j])
    #         if (landuseID < 0):
    #             data_prop[i][j] = DEFAULT_NODATA
    #         else:
    #             hg = int(dataHg[i][j]) - 1
    #             data_prop[i][j] = cn2Map[landuseID][hg]

    WriteGTiffFile(filename, ysize, xsize, data_prop, maskR.geotrans, maskR.srs, noDataValue, gdal.GDT_Float32)
    print 'The CN2 file is generated!'
    return filename


if __name__ == '__main__':
    GenerateCN2(WORKING_DIR, TXT_DB_DIR + os.sep + sqliteFile)
