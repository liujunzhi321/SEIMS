import math
import numpy
from TauDEM import *
from osgeo import gdal, osr
from util import WriteGTiffFile, FloatEqual, MaskRaster 

np = 8
e = 0
ne = math.pi * 0.25
n  = math.pi * 0.5
nw = math.pi * 0.75
w  = math.pi 
sw = math.pi * 1.25
s  = math.pi * 1.5
se = math.pi * 1.75

angleList = [e, ne, n, nw, w, sw, s, se]

def CheckOrtho(a):
    if FloatEqual(a, e):
        return 1
    elif FloatEqual(a, ne):
        return 128
    elif FloatEqual(a, n):
        return 64 
    elif FloatEqual(a, nw):
        return 32 
    elif FloatEqual(a, w):
        return 16 
    elif FloatEqual(a, sw):
        return 8
    elif FloatEqual(a, s):
        return 4
    elif FloatEqual(a, se):
        return 2 

def AssignDirCode(a, noDataValue):
    if FloatEqual(a, noDataValue):
        return (-9999,-9999) 

    d = CheckOrtho(a)
    if d is not None:
        return (d,1)

    if a < ne:
        a1 = a
        d  = 129 #1+128
    elif a < n:
        a1 = a - ne
        d  = 192 #128+64
    elif a < nw:
        a1 = a - n
        d  = 96  #64+32
    elif a < w:
        a1 = a - nw
        d  = 48  #32+16
    elif a < sw:
        a1 = a - w
        d  = 24  #16+8
    elif a < s:
        a1 = a - sw
        d  = 12  #8+4
    elif a < se:
        a1 = a - s
        d  = 6   #4+2
    else:
        a1 = a - se
        d  = 3   #2+1

    return (d, a1/math.pi*4.0)


def GenerateDinf(np, wdir, demFilled, flowDir, slopeFile, dirCodeFile, weightFile,mpiexeDir=None,exeDir=None):
    FlowDirDinf(np, wdir, demFilled, flowDir, slopeFile,mpiexeDir=mpiexeDir,exeDir=exeDir) 
    ds = gdal.Open(flowDir)
    band = ds.GetRasterBand(1)
    data = band.ReadAsArray()
    noDataValue = band.GetNoDataValue()

    calDirCode = numpy.frompyfunc(AssignDirCode, 2, 2)
    dirCode, weight = calDirCode(data, noDataValue)
    srs = osr.SpatialReference()
    srs.ImportFromWkt(ds.GetProjection())

    WriteGTiffFile(dirCodeFile, band.YSize, band.XSize, dirCode, ds.GetGeoTransform(), srs, -9999, gdal.GDT_Int16)
    WriteGTiffFile(weightFile, band.YSize, band.XSize, weight, ds.GetGeoTransform(), srs, -9999, gdal.GDT_Float32)
    

if __name__ == '__main__':
    import os
    resList = [270,]# 90, 30, 25, 20, 15, 10]
    wdir = '/data/liujz/data/qingshuihe_dinf'
    os.chdir(wdir)
    for res in resList:
        demFilled = '/data/liujz/data/qingshuihe_%dm/output/taudir/demFilledTau.tif' % (res,)
        flowDir = wdir + os.sep + "flow_dir_%dm.tif" % (res,)
        slope = wdir + os.sep + "slope_%dm_dinf_tmp.tif" % (res)
        weightFile = wdir + os.sep + "weight_%dm.tif" % (res,)
        dirCodeFileTmp = wdir + os.sep + "flow_dir_%dm_dinf_tmp.tif" % (res,)
        GenerateDinf(np, wdir, demFilled, flowDir, slope, dirCodeFileTmp, weightFile)

        maskFile = '/data/liujz/data/qingshuihe_%dm/output/mask.tif' % (res,)
        dirCodeFile = wdir + os.sep + "flow_dir_%dm_dinf.tif" % (res,)
        slopeFile = wdir + os.sep + "slope_%dm_dinf.tif" % (res,)
        MaskRaster(maskFile, dirCodeFileTmp, dirCodeFile)
        MaskRaster(maskFile, slope, slopeFile)


