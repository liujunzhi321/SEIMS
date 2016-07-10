#! /usr/bin/env python
# coding=utf-8
# @Calculate hydrological radius, efficiency improved by numpy
# Author: Junzhi Liu
# Revised: Liang-Jun Zhu
#
from util import *
from config import *


def GenerateRadius(filepath, stormProbability):
    accfile = filepath + os.sep + accM
    acc_R = ReadRaster(accfile)
    xsize = acc_R.nCols
    ysize = acc_R.nRows
    noDataValue = acc_R.noDataValue
    cellsize = acc_R.dx
    data = acc_R.data
    # ds = gdal.Open(accfile)
    # band = ds.GetRasterBand(1)
    # data = band.ReadAsArray()
    # xsize = band.XSize
    # ysize = band.YSize
    # noDataValue = band.GetNoDataValue()
    # geotransform = ds.GetGeoTransform()
    #
    # srs = osr.SpatialReference()
    # srs.ImportFromWkt(ds.GetProjection())
    #
    # cellsize = geotransform[1]
    ap = coeTable[stormProbability][0]
    bp = coeTable[stormProbability][1]

    def radiusCal(acc):
        if (abs(acc - noDataValue) < UTIL_ZERO):
            return DEFAULT_NODATA
        return numpy.power(ap * ((acc + 1) * cellsize * cellsize / 1000000.), bp)

    radiusCal_numpy = numpy.frompyfunc(radiusCal, 1, 1)
    radius = radiusCal_numpy(data)
    # radius = numpy.zeros((ysize, xsize))
    # for i in range(0, ysize):
    #     for j in range(0, xsize):
    #         if (abs(data[i][j] - noDataValue) < UTIL_ZERO):
    #             radius[i][j] = DEFAULT_NODATA
    #             continue
    #         data[i][j] = data[i][j] + 1
    #         temp = ap * (data[i][j] * cellsize * cellsize / 1000000.)
    #         # print temp, bp
    #         radius[i][j] = math.pow(temp, bp)

    filename = filepath + os.sep + radiusFile
    WriteGTiffFile(filename, ysize, xsize, radius,
                   acc_R.geotrans, acc_R.srs, DEFAULT_NODATA, gdal.GDT_Float32)
    print 'The radius file is generated!'
    return filename


if __name__ == "__main__":
    GenerateRadius(WORKING_DIR, "T2")
