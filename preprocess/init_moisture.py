#! /usr/bin/env python
# coding=utf-8
# Initializa soil moisture, improve calculation efficiency by numpy
# Author: Junzhi Liu
# Revised: Liang-Jun Zhu
#

from util import *
from config import *


def InitMoisture(dstdir):
    acc_name = dstdir + os.sep + accM
    slope_name = dstdir + os.sep + slopeM

    acc_R = ReadRaster(acc_name)
    dataAcc = acc_R.data
    xsize = acc_R.nCols
    ysize = acc_R.nRows
    noDataValue = acc_R.noDataValue
    srs = acc_R.srs
    geotrans = acc_R.geotrans
    dx = acc_R.dx
    #
    # ds = gdal.Open(acc_name)
    # band = ds.GetRasterBand(1)
    # dataAcc = band.ReadAsArray()
    # xsize = band.XSize
    # ysize = band.YSize
    # noDataValue = band.GetNoDataValue()
    # if noDataValue is None:
    #     noDataValue = DEFAULT_NODATA
    # srs = osr.SpatialReference()
    # srs.ImportFromWkt(ds.GetProjection())
    dataSlope = ReadRaster(slope_name).data
    #
    # dsSlope = gdal.Open(slope_name)
    # dataSlope = dsSlope.GetRasterBand(1).ReadAsArray()
    # geotransform = ds.GetGeoTransform()
    # dx = geotransform[1]
    cellArea = dx * dx

    def wiGridCal(acc, slp):
        if (abs(acc - noDataValue) < UTIL_ZERO):
            return DEFAULT_NODATA
        else:
            if (abs(slp) < UTIL_ZERO):
                slp = 0.1 / dx * 100.
            return math.log((acc + 1) * cellArea / (slp / 100.))

    wiGridCal_numpy = numpy.frompyfunc(wiGridCal, 2, 1)
    wiGrid = wiGridCal_numpy(dataAcc, dataSlope)
    wiGrid_valid = numpy.where(acc_R.validZone, wiGrid, numpy.nan)
    wiMax = numpy.nanmax(wiGrid_valid)
    wiMin = numpy.nanmin(wiGrid_valid)
    # wiGrid = zeros((ysize, xsize))
    # wiMax = -1
    # wiMin = 1000
    # for i in range(0, ysize):
    #     for j in range(0, xsize):
    #         if (abs(dataAcc[i][j] - noDataValue) < UTIL_ZERO):
    #             wiGrid[i][j] = DEFAULT_NODATA
    #         else:
    #             if (abs(dataSlope[i][j]) < UTIL_ZERO):
    #                 dataSlope[i][j] = 0.1 / dx * 100.
    #
    #             wiGrid[i][j] = math.log((dataAcc[i][j] + 1) * cellArea / (dataSlope[i][j] / 100.))
    #             if (wiGrid[i][j] > wiMax):
    #                 wiMax = wiGrid[i][j]
    #             elif (wiGrid[i][j] < wiMin):
    #                 wiMin = wiGrid[i][j]

    sMin = 0.6  # minimum relative saturation
    sMax = 1.0

    wiUplimit = wiMax
    a = (sMax - sMin) / (wiUplimit - wiMin)
    b = sMin - a * wiMin

    def moistureCal(acc, wigrid):
        if (abs(acc - noDataValue) < UTIL_ZERO):
            return DEFAULT_NODATA
        else:
            return a * wigrid + b

    moistureCal_numpy = numpy.frompyfunc(moistureCal, 2, 1)
    moisture = moistureCal_numpy(dataAcc, wiGrid)
    # moisture = zeros((ysize, xsize))
    # for i in range(0, ysize):
    #     for j in range(0, xsize):
    #         if (abs(dataAcc[i][j] - noDataValue) < UTIL_ZERO):
    #             moisture[i][j] = DEFAULT_NODATA
    #         else:
    #             moisture[i][j] = a * wiGrid[i][j] + b

    filename = dstdir + os.sep + initSoilMoist
    WriteGTiffFile(filename, ysize, xsize, moisture, geotrans, srs, DEFAULT_NODATA, gdal.GDT_Float32)

    print 'The initial moisture is generated!'
    return filename


if __name__ == "__main__":
    s = time.clock()
    InitMoisture(WORKING_DIR)
    print "%f" % (time.clock() - s)
