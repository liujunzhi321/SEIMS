## /usr/bin/env python
#coding=utf-8
### This program initializes soil chemical properties
### for nutrient related modules.
### Reference: soil_chem.f in SWAT 2009
### Author: Liangjun Zhu, Huiran Gao
### Date: 2016-3-28

import os
import math
from osgeo import gdal
import numpy
from util import *
from text import *

## INPUTS ##
#nactfr: nitrogen active pool fraction. The fraction of organic nitrogen in the active pool
#psp: Phosphorus availability index. The fraction of fertilizer P remaining in labile pool after initial rapid phase of P sorption.
#skoc(:)(mg/kg)/(mg/L): soil adsorption coefficient normalized for soil organic carbon content
#sol_bd(:,:)Mg/m**3: bulk density of the soil
#sol_cbn(:,:) %: percent organic carbon in soil layer
#sol_nly(:): number of soil layers
#sol_no3(:,:) mg N/kg soil : nitrate concentration in soil layer
#sol_orgn(:,:) |mg/kg : organic N concentration in soil layer
#sol_orgp(:,:) |mg/kg : organic P concentration in soil layer
#sol_rsd(:,:)  |kg/ha : amount of organic matter in the soil layer classified as residue
#sol_solp(:,:) |mg/kg : solution P concentration in soil layer
#sol_z(:,:)    |mm    : depth to bottom of soil layer

## OUTPUTS ##
#basminpi      |kg P/ha: average amount of phosphorus initially in the mineral P pool in watershed soil
#basno3i       |kg N/ha: average amount of nitrogen initially in the nitrate pool in watershed soil
#basorgni      |kg N/ha: average amount of nitrogen initially in the organic N pool in watershed soil
#basorgpi      |kg P/ha: average amount of phosphorus initially in the organic P pool in watershed soil
#conv_wt(:,:)  |none   : factor which converts kg/kg soil to kg/ha
#sol_actp(:,:) |kg P/ha: amount of phosphorus stored in the active mineral phosphorus pool
#sol_aorgn(:,:)|kg N/ha: amount of nitrogen stored in the active organic (humic) nitrogen pool
#sol_cov(:)    |kg/ha  : amount of residue on soil surface
#sol_fon(:,:)  |kg N/ha: amount of nitrogen stored in the fresh organic (residue) pool
#sol_fop(:,:)  |kg P/ha: amount of phosphorus stored in the fresh organic (residue) pool
#sol_hum(:,:)  |kg humus/ha : amount of organic matter in the soil layer classified as humic substances
#sol_no3(:,:)  |kg N/ha: amount of nitrogen stored in the nitrate pool. This variable is read in as
###                                 |a concentration and converted to kg/ha.
###                                 |(this value is read from the .sol file in
###                                 |units of mg/kg)
#sol_orgn(:,:) |kg N/ha: amount of nitrogen stored in the stable organic N pool NOTE UNIT CHANGE#
#sol_orgp(:,:) |kg P/ha: amount of phosphorus stored in the organic P pool NOTE UNIT CHANGE#
#sol_solp(:,:) |kg P/ha: amount of phosohorus stored in solution NOTE UNIT CHANGE#
#sol_stap(:,:) |kg P/ha: amount of phosphorus in the soil layer stored in the stable mineral phosphorus pool

## LOCAL VARIABLES ##
###    name        |units         |definition
###    ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
###    dg          |mm            |depth of layer
###    j           |none          |counter
###    jj          |none          |dummy variable to hold value
###    n           |none          |counter
###    nly         |none          |number of soil layers
###    soldepth    |mm            |depth from bottom of 1st soil layer to
###                               |the bottom of the layer of interest
###    solpst      |mg/kg         |concentration of pesticide in soil
###    summinp     |kg P/ha       |amount of phosphorus stored in the mineral P
###                               |pool in the profile
###    sumno3      |kg N/ha       |amount of nitrogen stored in the nitrate pool
###                               |in the soil profile
###    sumorgn     |kg N/ha       |amount of nitrogen stored in the organic N
###                               |pools in the profile
###    sumorgp     |kg P/ha       |amount of phosphorus stored in the organic P
###                               |pools in the profile
###    wt1         |none          |converts mg/kg (ppm) to kg/ha
###    xx          |none          |variable to hold value
###    zdst        |none          |variable to hold value
###    labfrac     |none          |fraction of total soil mineral P which is labile
###    soil_TP	   |kg/ha         |Total Soil Mineral P
###    sol_mass, FBM, FHP, RTNO, FHS, X1, RTO, sol_min_n
###    
def Soil_Chemical(workingDir, outputPath):
    bd1File = workingDir + os.sep + "Density.tif"   ##sol_bd layer1 and layer2
    bd2File = workingDir + os.sep + "Density2.tif"  ##sol_bd layer3
    om1File = workingDir + os.sep + "org.tif"       ##for sol_cbn layer1 and layers2
    om2File = workingDir + os.sep + "org2.tif"      ##for sol_cbn layer3
    rdFile = workingDir + os.sep + "RootDepth.tif"  ##for sol_z
    clay1File = workingDir + os.sep + "clay.tif"    ##clay in layer1 and layer2
    clay2File = workingDir + os.sep + "clay2.tif"   ##clay in layer1 and layer2

    bd1Raster = ReadRaster(bd1File)
    bd2Raster = ReadRaster(bd2File)
    om1Raster = ReadRaster(om1File)
    om2Raster = ReadRaster(om2File)
    rdRaster = ReadRaster(rdFile)
    clay1Raster = ReadRaster(clay1File)
    clay2Raster = ReadRaster(clay2File)

    nCols = bd1Raster.nCols
    nRows = bd1Raster.nRows
    noDataValue = bd1Raster.noDataValue

    sol_bd1 = bd1Raster.data
    sol_bd2 = bd2Raster.data
    sol_org1 = om1Raster.data
    sol_org2 = om2Raster.data
    root_depth = rdRaster.data
    sol_z = root_depth
    #numpy.savetxt("d:/file.txt", sol_z)
    clay1 = clay1Raster.data
    clay2 = clay2Raster.data

    sol_cbn1 = sol_org1 * 0.58 ## van bemmelen coefficient
    sol_cbn2 = sol_org2 * 0.58

    ## Calculate Cell by Cell
    sol_mass = 0.
    DG = 0.
    FBM = 0.
    FHP = 0.
    RTNO = 0.
    FHS = 0.
    X1 = 0.
    RTO = 0.
    nactfr = .02

    nly = 3   ## By default, soil layers equal to 3 for nutrient modules, 0~10mm, 10~100mm, and 100~rootDepth
    solpst = 0.
    sumno3 = 0.
    sumorgn = 0.
    summinp = 0.
    sumorgp = 0.

    lyr1 = 10   ## unit is mm
    lyr2 = 100
    lyr3 = root_depth
    sol_thick1 = lyr1
    sol_thick2 = lyr2 - lyr1
    sol_thick3 = lyr3 - lyr2
    sol_bd = [sol_bd1, sol_bd1, sol_bd2]
    sol_cbn = [sol_cbn1, sol_cbn1, sol_cbn2]
    sol_thick = [sol_thick1, sol_thick2, sol_thick3]
    sol_clay = [clay1, clay1, clay2]
    sol_rock = [0., 0., 0.]  ## fraction of rock in every soil layer, by default , sol_rock is 0
    cmtot_kgh = numpy.zeros((3, nRows, nCols)) ## ##kg/ha  current soil carbon integrated - aggregating all soil layers
    sol_mass = numpy.zeros((3, nRows, nCols))
    sol_cmass = numpy.zeros((3, nRows, nCols))
    for lyr in range(len(sol_thick)):
        #print lyr
        sol_mass[lyr] = (sol_thick[lyr] / 1000.) * 10000. * 1000. * (1 - sol_rock[lyr] / 100.) * sol_bd[lyr]  ##单层土壤质量(kg)
        sol_cmass[lyr] = sol_mass[lyr] * (sol_cbn[lyr] / 100.)  ##单层土壤的C含量(kg)
        if lyr == 0:
            cmtot_kgh[lyr] = sol_cmass[lyr]  ## ##kg/ha    current soil carbon for first soil layer
        cmtot_kgh[lyr] = cmtot_kgh[lyr] + sol_cmass[lyr]
##    calculate initial nutrient contents of layers, profile and
##    average in soil for the entire watershed
##    convert mg/kg (ppm) to kg/ha
    xx = 0.
    sol_fop = numpy.ones((3, nRows, nCols)) # phosphorus stored in the fresh organic (residue) pool
    sol_fon = numpy.ones((3, nRows, nCols)) # nitrogen stored in the fresh organic (residue) pool
    sol_cov = numpy.ones((nRows, nCols)) # residue on soil surface

    sol_orgn = numpy.ones((3, nRows, nCols))
    sol_no3 = numpy.zeros((3, nRows,  nCols))
    sol_aorgn = numpy.zeros((3, nRows,  nCols))
    sol_orgp = numpy.ones((3, nRows,  nCols))
    sol_solp = numpy.zeros((3, nRows,  nCols))
    sol_actp = numpy.zeros((3, nRows,  nCols))
    sol_stap = numpy.zeros((3, nRows,  nCols))
    sol_P_model = numpy.zeros((3, nRows,  nCols))
    sol_hum = numpy.zeros((3, nRows,  nCols))
    #sol_rsd is organic matter in the soil layer classified as residue
    sol_rsd = numpy.random.random_sample((3, nRows, nCols)) * .3  ##先随机生成sol_rsd，0~0.3的随机数
    solp = numpy.zeros((nRows, nCols))
    actp = numpy.zeros((nRows, nCols))

    sumno3 = sumorgn = summinp = sumorgp = numpy.zeros((nRows, nCols))

    for j in range(nly):
        dg = 0.
        wt1 = 0.
        dg = (sol_z - xx)
        wt1 = sol_bd[j] * dg / 100.              ## mg/kg => kg/ha
        conv_wt = 1.e6 * wt1                    ## kg/kg => kg/ha
        ##输出conv_wt
        conv_wt_file = outputPath + os.sep + "conv_wt%d.tif" % j
        #writeTIFF(conv_wt, conv_wt_file)

        sol_fop[j] = sol_rsd[j] * .0010
        sol_fon[j] = sol_rsd[j] * .0055
        sol_cov = sol_rsd[1]

        ##输出sol_fop,sol_fon
        sol_fop_file = outputPath + os.sep + "sol_fop%d.tif" % j
        sol_fon_file = outputPath + os.sep + "sol_fon%d.tif" % j
        #writeTIFF(sol_fop[j], sol_fop_file)
        #writeTIFF(sol_fon[j], sol_fon_file)

        #if (sol_no3[j] <= 0.) :
        zdst = numpy.zeros((nRows,  nCols))
        for m in range(nRows):
            for n in range(nCols):
                zdst[m][n] = math.exp(-sol_z[m][n] / 1000.)
        sol_no3[j] = 10. * zdst * .7
        sol_no3[j] = sol_no3[j] * wt1          ## mg/kg => kg/ha
        sumno3 = sumno3 + sol_no3[j]
        ##输出sol_no3
        sol_no3_file = outputPath + os.sep + "sol_no3%d.tif" % j
        #writeTIFF(sol_no3[j], sol_no3_file)

        sol_orgn[j] = sol_orgn[j] * wt1
        for m in range(nRows):
            for n in range(nCols):
                if (sol_orgn[j][m][n] > 0.0001):
                    sol_orgn[j][m][n] = sol_orgn[j][m][n] * wt1[m][n]      ## mg/kg => kg/ha
                else:
            ## assume C:N ratio of 10:1
                    sol_orgn[j][m][n] = 10000. * (sol_cbn[j][m][n] / 14.) * wt1[m][n]  ## CN ratio changed back to 14 cibin 03022012
        sol_aorgn[j] = sol_orgn[j] * nactfr
        sol_orgn[j] = sol_orgn[j] * (1. - nactfr)
        ##输出sol_orgn,sol_aorgn
        sol_orgn_file = outputPath + os.sep + "sol_orgn%d.tif" % j
        sol_aorgn_file = outputPath + os.sep + "sol_aorgn%d.tif" % j
        #writeTIFF(sol_orgn[j], sol_orgn_file)
        #writeTIFF(sol_aorgn[j], sol_aorgn_file)

        sumorgn = sumorgn + sol_aorgn[j] + sol_orgn[j] + sol_fon[j]

        sol_orgp[j] = sol_orgp[j] * wt1
        sol_solp[j] = sol_solp[j] * wt1
        for m in range(nRows):
            for n in range(nCols):
                if (sol_orgp[j][m][n] > 0.0001):
                    sol_orgp[j][m][n] = sol_orgp[j][m][n] * wt1[m][n]      ## mg/kg => kg/ha
                else:
	        ## assume N:P ratio of 8:1
                    sol_orgp[j][m][n] = .125 * sol_orgn[j][m][n]
                if (sol_solp[j][m][n] > 0.0001) :
                    sol_solp[j][m][n] = sol_solp[j][m][n] * wt1[m][n]     ## mg/kg => kg/ha
                else:
            ## assume initial concentration of 5 mg/kg
                    sol_solp[j][m][n] = 5. * wt1[m][n]
        ##输出sol_orgp,sol_solp
        sol_orgp_file = outputPath + os.sep + "sol_orgp%d.tif" % j
        sol_solp_file = outputPath + os.sep + "sol_solp%d.tif" % j
        #writeTIFF(sol_orgp[j], sol_orgp_file)
        #writeTIFF(sol_solp[j], sol_solp_file)

        ## Set active pool based on dynamic PSP MJW
        for m in range(nRows):
            for n in range(nCols):
                if (conv_wt[m][n] != 0):
                    solp[m][n] = (sol_solp[j][m][n] / conv_wt[m][n]) * 1000000.
                    if (sol_clay[j][m][n] > 0.):
                        psp = -0.045 * math.log(sol_clay[j][m][n])+ (0.001 * solp[m][n])
                        psp = psp - (0.035  * sol_cbn[j][m][n]) + 0.43
                    else:
                        psp = 0.4
                    ## Limit PSP range
                    if (psp <.05) :
                        psp = 0.05
                    elif (psp > 0.9):
                        psp = 0.9

                sol_actp[j][m][n] = sol_solp[j][m][n] * (1. - psp) / psp

                ## Set Stable pool based on dynamic coefficant
                if (conv_wt[m][n] != 0) :  ## From White et al 2009
                    ## convert to concentration for ssp calculation
                    actp[m][n] = sol_actp[j][m][n] / conv_wt[m][n] * 1000000.
                    solp[m][n] = sol_solp[j][m][n] / conv_wt[m][n] * 1000000.
                    ## estimate Total Mineral P in this soil based on data from sharpley 2004
                    ssp = 25.044 * ((actp[m][n] + solp[m][n]) ** -0.3833)
                    ##limit SSP Range
                    if (ssp > 7.):
                        ssp = 7.
                    if (ssp < 1.):
                        ssp = 1.
                    sol_stap[j][m][n] = ssp * (sol_actp[j][m][n] + sol_solp[j][m][n])  #define stableP
                else:
	        ## The original code
		            sol_stap[j] = 4. * sol_actp[j]

        sol_hum[j] = sol_cbn[j] * wt1 * 17200.
        xx = sol_z[j]
        summinp = summinp + sol_solp[j] + sol_actp[j] + sol_stap[j]
        sumorgp = sumorgp + sol_orgp[j] + sol_fop[j]

        ##输出sol_actp, sol_stap, sol_hum
        sol_actp_file = outputPath + os.sep + "sol_actp%d.tif" % j
        sol_stap_file = outputPath + os.sep + "sol_stap%d.tif" % j
        sol_hum_file = outputPath + os.sep + "sol_hum%d.tif" % j
        #writeTIFF(sol_actp[j], sol_actp_file)
        #writeTIFF(sol_stap[j], sol_stap_file)
        #writeTIFF(sol_hum[j], sol_hum_file)

    ##输出sol_cov
    sol_cov_file = outputPath + os.sep + "sol_cov.tif"
    #writeTIFF(sol_cov, sol_cov_file)

    #basno3i = basno3i + sumno3 * hru_km / da_km
    #basorgni = basorgni + sumorgn * hru_km / da_km
    #basminpi = basminpi + summinp * hru_km / da_km
    #basorgpi = basorgpi + sumorgp * hru_km / da_km
    basno3i_file = outputPath + os.sep + "sumno3.tif"
    basorgni_file = outputPath + os.sep + "sumorgn.tif"
    basminpi_file = outputPath + os.sep + "summinp.tif"
    basorgpi_file = outputPath + os.sep + "sumorgp.tif"
    #writeTIFF(sumno3, basno3i_file)
    #writeTIFF(sumorgn, basorgni_file)
    #writeTIFF(summinp, basminpi_file)
    #writeTIFF(sumorgp, basorgpi_file)

def writeTIFF(file, fpath):
    tiffile = "D:\SEIMS_model\Python\Preprocess\Model_data\model_dianbu_30m_test\data_prepare\output\Density.tif"
    nRows = ReadRaster(tiffile).nRows
    nCols = ReadRaster(tiffile).nCols
    geotrans = ReadRaster(tiffile).geotrans
    srs = ReadRaster(tiffile).srs
    for i in range(nRows):
        for j in range(nCols):
            if abs(ReadRaster(tiffile).data[i][j] - (-9999)) < DELTA:
                file[i][j] = -9999
    WriteGTiffFile(fpath, nRows, nCols, file, geotrans, srs, -9999, gdal.GDT_Float32)
    print "%s输出完成~" % fpath

#if __name__ == "__main__":
nactfr = 0.5;
outputPath = "D:\SEIMS_model\Python\Preprocess\Model_data\model_dianbu_30m_test\data_prepare\output\soil_chem"
WORKING_DIR = "D:\SEIMS_model\Python\Preprocess\Model_data\model_dianbu_30m_test\data_prepare\output"
Soil_Chemical(WORKING_DIR, outputPath);
print "Soil chemical properties initialized done!"