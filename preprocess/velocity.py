#! /usr/bin/env python
# coding=utf-8
# @Calculate velocity
# Author: Junzhi Liu
# Revised: Liang-Jun Zhu
#

from numpy import *
from config import *
from util import *


def GenerateVelocity(filepath):
    slope = filepath + os.sep + slopeM
    slp_R = ReadRaster(slope)
    slo_data = slp_R.data
    xsize = slp_R.nCols
    ysize = slp_R.nRows
    noDataValue = slp_R.noDataValue

    # ds = gdal.Open(slope)
    # band = ds.GetRasterBand(1)
    # slo_data = band.ReadAsArray()

    radius = filepath + os.sep + radiusFile
    rad_data = ReadRaster(radius).data
    # ds = gdal.Open(radius)
    # band = ds.GetRasterBand(1)
    # rad_data = band.ReadAsArray()
    # xsize = band.XSize
    # ysize = band.YSize
    # noDataValue = band.GetNoDataValue()
    # geotransform = ds.GetGeoTransform()
    #
    # srs = osr.SpatialReference()
    # srs.ImportFromWkt(ds.GetProjection())
    #
    # cellsize = geotransform[1]

    Manning = filepath + os.sep + ManningFile
    Man_data = ReadRaster(Manning).data
    # ds = gdal.Open(Manning)
    # band = ds.GetRasterBand(1)
    # Man_data = band.ReadAsArray()

    vel_max = 3.0
    vel_min = 0.0005

    def velocityCal(rad, man, slp):
        if (abs(rad - noDataValue) < UTIL_ZERO):
            return DEFAULT_NODATA
        tmp = numpy.power(man, -1) * numpy.power(rad, 2 / 3) * \
              numpy.power(slp, 0.5)
        if tmp < vel_min:
            return vel_min
        if tmp > vel_max:
            return vel_max
        return tmp

    velocityCal_numpy = numpy.frompyfunc(velocityCal, 3, 1)
    velocity = velocityCal_numpy(rad_data, Man_data, slo_data)

    # velocity = zeros((ysize, xsize))
    # for i in range(0, ysize):
    #     for j in range(0, xsize):
    #         if (abs(rad_data[i][j] - noDataValue) < UTIL_ZERO):
    #             velocity[i][j] = -9999
    #             continue
    #         velocity[i][j] = math.pow(Man_data[i][j], -1) * math.pow(rad_data[i][j], 2 / 3) * math.pow(
    #             slo_data[i][j] / 100, 0.5)
    #         if (velocity[i][j] < vel_min):
    #             velocity[i][j] = vel_min
    #         if (velocity[i][j] > vel_max):
    #             velocity[i][j] = vel_max

    filename = filepath + os.sep + velocityFile
    WriteGTiffFile(filename, ysize, xsize, velocity,
                   slp_R.geotrans, slp_R.srs, DEFAULT_NODATA, gdal.GDT_Float32)
    print 'The velocity file is generated!'
    return filename


if __name__ == "__main__":
    GenerateVelocity(WORKING_DIR)
