# -*- coding: utf-8 -*-
"""
Created on Thu Jan 10 15:36:19 2013

@author: junzhi
@revised: LiangJun Zhu, Huiran Gao
@date: 2016-5-21
"""
from math import *
from osgeo import gdal
from pyproj import Proj, transform
#from numpy import *
import numpy
from util import *
from text import *
from soil_texture import *
from soil_chem import *
import os

class SoilProperty:
    '''
    base class of Soil properties
    :method: init(SEQN, SNAM)
    '''
    def __init__(self, SEQN, SNAM):
        self.SEQN       = SEQN
        self.SNAM       = SNAM
        self.SoilLAYERS       = DEFAULT_NODATA
        self.SoilDepth      = []
        self.OM     = []
        self.CLAY       = []
        self.SILT       = []
        self.SAND       = []
        self.ROCK       = []
        self.Sol_ZMX    = DEFAULT_NODATA
        self.ANION_EXCL = DEFAULT_NODATA
        self.Sol_CRK    = DEFAULT_NODATA
        self.Density     = []
        self.Conductivity      = []
        self.WiltingPoint     = []
        self.FieldCap     = []
        self.Sol_AWC    = []
        self.POROSITY = []
        self.Poreindex= []
        self.USLE_K = []
        self.Sol_ALB    = []
        self.Texture    = DEFAULT_NODATA
        self.Hydro_Group= DEFAULT_NODATA
        #self.Residue     = [] TODO: residue should be defined in Management module
        ### Here after are soil chemical properties
        self.Sol_FOP = []
        self.Sol_fon = []
        self.Sol_no3 = []
        self.Sol_orgn = []
        self.Sol_aorgn = []
        self.Sol_orgp = []
        self.Sol_solp = []
        self.Sol_actp = []
        self.Sol_stap = []
        self.Sol_hum = []
        self.conv_wt = DEFAULT_NODATA
        self.sol_cov = DEFAULT_NODATA
        self.sumno3 = DEFAULT_NODATA
        self.sumorgn = DEFAULT_NODATA
        self.summinp = DEFAULT_NODATA
        self.sumorgp = DEFAULT_NODATA

    def SoilDict(self):
        solDict = self.__dict__
        solDict.pop('SNAM')
        return solDict
    def CheckData(self): ## check the required input, and calculate all physical and chemical properties
        if self.SoilLAYERS == DEFAULT_NODATA:
            raise ValueError("Soil layers number must be provided, please check the input file!")
        if self.SoilDepth == [] or len(self.SoilDepth) != self.SoilLAYERS:
            raise IndexError("Soil depth must have a size equal to soil layers number!")
        if self.OM == [] or len(self.OM) != self.SoilLAYERS:
            raise IndexError("Soil organic matter must have a size equal to soil layers number!")
        if self.CLAY == [] or len(self.CLAY) != self.SoilLAYERS:
            raise IndexError("Soil Clay content must have a size equal to soil layers number!")
        if self.SILT == [] or len(self.SILT) != self.SoilLAYERS:
            raise IndexError("Soil Silt content must have a size equal to soil layers number!")
        if self.SAND == [] or len(self.SAND) != self.SoilLAYERS:
            raise IndexError("Soil Sand content must have a size equal to soil layers number!")
        if self.ROCK == [] or len(self.ROCK) != self.SoilLAYERS:
            raise IndexError("Soil Rock content must have a size equal to soil layers number!")
        ### temperory variables
        tmp_fc = []
        tmp_sat = []
        tmp_wp = []
        for i in range(self.SoilLAYERS):
            s = self.SAND[i] * 0.01
            c = self.CLAY[i] * 0.01
            om = self.OM[i]
            wpt = -0.024*s + 0.487*c + 0.006*om + 0.005*s*om - 0.013*c*om + 0.068*s*c + 0.031
            tmp_wp.append(1.14*wpt - 0.02)
            fct = -0.251*s + 0.195*c + 0.011*om + 0.006*s*om - 0.027*c*om + 0.452*s*c + 0.299
            fc = fct + 1.283*fct*fct - 0.374*fct - 0.015
            s33t = 0.278*s + 0.034*c + 0.022*om - 0.018*s*om - 0.027*c*om - 0.584*s*c + 0.078
            s33 = 1.636*s33t - 0.107
            sat = fc + s33 - 0.097*s + 0.043
            tmp_fc.append(fc)
            tmp_sat.append(sat)
        if self.Sol_ZMX == DEFAULT_NODATA or self.Sol_ZMX > self.SoilDepth[-1]:
            self.Sol_ZMX = self.SoilDepth[-1]
        if self.ANION_EXCL == DEFAULT_NODATA:
            self.ANION_EXCL = 0.5
        if self.WiltingPoint != [] and len(self.WiltingPoint) != self.SoilLAYERS:
            raise IndexError("Wilting point must have a size equal to soil layers number!")
        elif self.WiltingPoint == []:
            self.WiltingPoint = tmp_wp[:]
        if self.Density != [] and len(self.Density) != self.SoilLAYERS:
            raise IndexError("Bulk density must have a size equal to soil layers number!")
        elif self.Density == []:
            for i in range(self.SoilLAYERS):
                sat = tmp_sat[i]
                fc = tmp_fc[i]
                if self.FieldCap != [] and len(self.FieldCap) == self.SoilLAYERS:
                    sat = sat - fc + self.FieldCap[i]
                self.Density.append(2.65 * (1.0 - sat))
        if self.FieldCap != [] and len(self.FieldCap) != self.SoilLAYERS:
            raise IndexError("Field capacity must have a size equal to soil layers number!")
        elif self.FieldCap == []:
            for i in range(self.SoilLAYERS):
                fc = tmp_fc[i]
                sat = tmp_sat[i]
                if self.Density != [] and len(self.Density) == self.SoilLAYERS:
                    p_df = self.Density[i]
                else:
                    p_df = 2.65*(1.0 - sat)
                sat_df = 1 - p_df/2.65
                self.FieldCap.append(fc - 0.2 * (sat - sat_df))
        if self.Sol_AWC != [] and len(self.Sol_AWC) != self.SoilLAYERS:
            raise IndexError("Available water capacity must have the size equal to soil layers number!")
        elif self.Sol_AWC == []:
            for i in range(self.SoilLAYERS):
                self.Sol_AWC.append(self.FieldCap[i] - self.WiltingPoint[i])
        if self.Poreindex != [] and len(self.Poreindex) != self.SoilLAYERS:
            raise IndexError("Pore disconnectedness index must have a size equal to soil layers number!")
        elif self.Poreindex == []:
            for i in range(self.SoilLAYERS):
                fc = tmp_fc[i]
                wp = self.WiltingPoint[i]
                b = (log(1500.) - log(33.)) / (log(fc) - log(wp))
                self.Poreindex.append(1.0 / b)
        if self.POROSITY != [] and len(self.POROSITY) != self.SoilLAYERS:
            raise IndexError("Soil Porosity must have a size equal to soil layers number!")
        elif self.POROSITY == []:
            for i in range(self.SoilLAYERS):
                self.POROSITY.append(1 - self.Density[i] / 2.65)
        if self.Sol_CRK == DEFAULT_NODATA:
            self.Sol_CRK = numpy.mean(self.POROSITY)
        if self.Conductivity != [] and len(self.Conductivity) != self.SoilLAYERS:
            raise IndexError("Saturated conductivity must have a size equal to soil layers number!")
        elif self.Conductivity == []:
            for i in range(self.SoilLAYERS):
                lamda = self.Poreindex[i]
                fc = tmp_fc[i]
                sat = tmp_sat[i]
                self.Conductivity.append(1930 * pow(sat - fc, 3 - lamda))
        if self.Sol_ALB != [] and len(self.Sol_ALB) != self.SoilLAYERS:
            raise IndexError("Soil albedo must have a size equal to soil layers number!")
        elif self.Sol_ALB == []:
            for i in range(self.SoilLAYERS):
                cbn = self.OM[i] * 0.58
                self.Sol_ALB.append(0.2227 * exp(-1.8672 * cbn))
        if self.USLE_K != [] and len(self.USLE_K) != self.SoilLAYERS:
            raise IndexError("USLE K factor must have a size equal to soil layers number!")
        elif self.USLE_K == []:
            for i in range(self.SoilLAYERS): ## According to Liu BY et al., (1999)
                sand = self.SAND[i]
                silt = self.SILT[i]
                clay = self.CLAY[i]
                cbn = self.OM[i] * 0.58
                sn = 1 - sand * 0.01
                a = (0.2+0.3*exp(-0.0256*sand*(1-silt*0.01)))
                b = pow(silt/(clay+silt),0.3)
                c = (1-0.25*cbn/(cbn+exp(3.72-2.95*cbn)))
                d = (1-0.25*sn/(sn+exp(-5.51+22.9*sn)))
                k = a * b * c * d
                self.USLE_K.append(k)
        if self.Texture == DEFAULT_NODATA or self.Hydro_Group == DEFAULT_NODATA:
            st, hg, uslek = GetTexture(self.CLAY[0], self.SILT[0], self.SAND[0])
            self.Texture = st
            self.Hydro_Group = hg
        ### Here after is initialization of soil chemical properties. Algorithms from SWAT.
        ### Prepared by Huiran Gao
        ### Revised by LiangJun Zhu
        ### Date: 2016-5-21

        SoilChemProperties(self.SoilLAYERS, self.SoilDepth, self.OM, self.CLAY, self.ROCK, self.Density)


## Calculate soil properties from sand, clay and organic matter.
## TODO, add reference.
def GetProperties(s, c, om):
    ## wilting point (SOL_WP)
    wpt = -0.024*s + 0.487*c + 0.006*om + 0.005*s*om - 0.013*c*om + 0.068*s*c + 0.031
    wp = 1.14*wpt - 0.02
    
    ## bulk density according to field capacity
    fct = -0.251*s + 0.195*c + 0.011*om + 0.006*s*om - 0.027*c*om + 0.452*s*c + 0.299
    fc = fct + 1.283*fct*fct - 0.374*fct - 0.015

    s33t = 0.278*s + 0.034*c + 0.022*om - 0.018*s*om - 0.027*c*om - 0.584*s*c + 0.078
    s33 = 1.636*s33t - 0.107
    sat = fc + s33 - 0.097*s + 0.043
    pn = 2.65*(1.0 - sat)

    ## field capacity (SOL_FC) with density effects (df)
    p_df = pn
    sat_df = 1 - p_df/2.65  ## porosity
    fc_df = fc - 0.2*(sat - sat_df)

    ## available water capacity (SOL_AWC)
    awc = fc_df - wp

    # pore disconnectedness index
    b = (log(1500.) - log(33.)) / (log(fc) - log(wp))
    lamda = 1.0/b
    
    # saturated conductivity
    #print s, c, sat, fc, 3-lamda
    ks = 1930*pow(sat-fc, 3-lamda)
    
    #print wp, fc_df, awc,
    return wp, fc_df, sat_df, p_df, ks, lamda
    #"WiltingPoint", "FieldCap", "Porosity","Density","Conductivity", "Poreindex"

def GetValue(geoMask, geoMap, data, i, j):
    #pGeo = Proj("+proj=longlat +ellps=krass +no_defs")
    #pAlbers = Proj("+proj=aea +ellps=krass +lon_0=105 +lat_0=0 +lat_1=25 +lat_2=47")
    #xMask = geoMask[0] + (j+0.5)*geoMask[1]
    #yMask = geoMask[3] + (i+0.5)*geoMask[5]
    #xMap, yMap = transform(pAlbers, pGeo, xMask, yMask)

    xMap = geoMask[0] + (j+0.5)*geoMask[1]
    yMap = geoMask[3] + (i+0.5)*geoMask[5]

    jMap = (xMap - geoMap[0])/geoMap[1]
    iMap = (yMap - geoMap[3])/geoMap[5]

    return data[iMap][jMap]

## Deprecated by LJ, 2016-5-21
def GenerateSoilAttributes(outputFolder, layerNum, sandFile=None, clayFile=None, orgFile=None):
    maskFile = outputFolder + os.sep + mask_to_ext
#    if sandFile is None:
#        sandFile = '%s/sand%d_albers.img' % (SPATIAL_DATA_DIR, layerNum)
#    if clayFile is None:
#        clayFile = '%s/clay%d_albers.img' % (SPATIAL_DATA_DIR, layerNum)

    dataSom = None
    if orgFile is not None and os.path.exists(orgFile):
        dsSom = gdal.Open(orgFile)
        bandSom = dsSom.GetRasterBand(1)
        dataSom = bandSom.ReadAsArray()

    print sandFile
    print clayFile
    dsSand = gdal.Open(sandFile)
    if dsSand is None:
        print "Open file:", sandFile, " failed.\n"
        return
    bandSand = dsSand.GetRasterBand(1)
    dataSand = bandSand.ReadAsArray()
    geoSand = dsSand.GetGeoTransform()
    
    dsClay = gdal.Open(clayFile)
    if dsClay is None:
        print "Open file:", clayFile, " failed.\n"
        return
    bandClay = dsClay.GetRasterBand(1)
    dataClay = bandClay.ReadAsArray()
    
    dsMask = gdal.Open(maskFile)
    bandMask = dsMask.GetRasterBand(1)
    dataMask = bandMask.ReadAsArray()
    xSizeMask = bandMask.XSize
    ySizeMask = bandMask.YSize
    geoMask = dsMask.GetGeoTransform()
    noDataValue = bandMask.GetNoDataValue()
    print noDataValue

    srs = osr.SpatialReference()
    srs.ImportFromWkt(dsMask.GetProjection())

    attrMapList = []
    n = 6 
    for i in range(n):
        attrMapList.append(numpy.zeros((ySizeMask, xSizeMask)))
    
    attrPool = {}
    lastKey = None
    for i in range(ySizeMask):
        #print "soil", i
        for j in range(xSizeMask):
            if abs(dataMask[i][j] - noDataValue) < DELTA:
                for iAttr in range(n):
                    attrMapList[iAttr][i][j] = DEFAULT_NODATA
                continue
            s = GetValue(geoMask, geoSand, dataSand, i, j) 
            c = GetValue(geoMask, geoSand, dataClay, i, j) 

            k = "%d_%d_%d" % (int(s*1000), int(c*1000), layerNum)
            #print s, c

            if dataSom is not None:
                om = dataSom[i][j]
            else:
                om = defaultOrg  # om=2.5
                if layerNum >= 1:
                    om = 1.0

            if s < 0 or c < 0:
                attrs = attrPool[lastKey]
                k = lastKey
            elif k in attrPool.keys():
                attrs = attrPool[k]
            else:
                attrs = GetProperties(s * 0.01, c * 0.01, om)
                print dataMask[i][j], s, c
                #attrs = GetProperties(s, c, om)
                attrPool[k] = attrs
            lastKey = k

            for iAttr in range(n):
                attrMapList[iAttr][i][j] = attrs[iAttr]

    attrList = []
    ##layerStr = str(layerNum) if layerNum > 1 else ''
    for i in range(n):
        filename = outputFolder + os.sep + r"%s_%s.tif" % (SOIL_ATTR_LIST[i+2], str(layerNum))
        WriteGTiffFile(filename, ySizeMask, xSizeMask, attrMapList[i], \
                                   geoMask, srs, DEFAULT_NODATA, gdal.GDT_Float32)
        attrList.append(filename)

    return attrList



#if __name__ == "__main__":
    #testList = [[0.84627,0.04302,0.0208], [0.85,0.05,0.025], [0.65,0.1,0.025]]
    #for s in testList:
    #    print s[0], s[1], s[2]
    #    print GetProperties(s[0], s[1], s[2]*100)
#    layerNum = 1
#    inputFolder = r'F:\data\soilmap_china'
#    #outputFolder = r'F:\data\fenkeng_30m\data'
#    outputFolder = r'F:\data\poyanghu\fenkeng\data'
#    GenerateAttrMap(inputFolder, outputFolder, layerNum)
#    print 'done!'
