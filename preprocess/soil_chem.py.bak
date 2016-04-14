#! /usr/bin/env python
#coding=utf-8
### This program initializes soil chemical properties
### for nutrient related modules.
### Reference: soil_chem.f in SWAT 2009
### Author: Liangjun Zhu
### Date: 2015-07-23

import os
from osgeo import gdal
import numpy
from util import *
from config import *

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
#!!                                 |a concentration and converted to kg/ha.
#!!                                 |(this value is read from the .sol file in
#!!                                 |units of mg/kg)
#sol_orgn(:,:) |kg N/ha: amount of nitrogen stored in the stable organic N pool NOTE UNIT CHANGE!
#sol_orgp(:,:) |kg P/ha: amount of phosphorus stored in the organic P pool NOTE UNIT CHANGE!
#sol_solp(:,:) |kg P/ha: amount of phosohorus stored in solution NOTE UNIT CHANGE!
#sol_stap(:,:) |kg P/ha: amount of phosphorus in the soil layer stored in the stable mineral phosphorus pool

## LOCAL VARIABLES ##
#!!    name        |units         |definition
#!!    ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
#!!    dg          |mm            |depth of layer
#!!    j           |none          |counter
#!!    jj          |none          |dummy variable to hold value
#!!    n           |none          |counter
#!!    nly         |none          |number of soil layers
#!!    soldepth    |mm            |depth from bottom of 1st soil layer to
#!!                               |the bottom of the layer of interest
#!!    solpst      |mg/kg         |concentration of pesticide in soil
#!!    summinp     |kg P/ha       |amount of phosphorus stored in the mineral P
#!!                               |pool in the profile
#!!    sumno3      |kg N/ha       |amount of nitrogen stored in the nitrate pool
#!!                               |in the soil profile
#!!    sumorgn     |kg N/ha       |amount of nitrogen stored in the organic N
#!!                               |pools in the profile
#!!    sumorgp     |kg P/ha       |amount of phosphorus stored in the organic P
#!!                               |pools in the profile
#!!    wt1         |none          |converts mg/kg (ppm) to kg/ha
#!!    xx          |none          |variable to hold value
#!!    zdst        |none          |variable to hold value
#!!    labfrac     |none          |fraction of total soil mineral P which is labile
#!!    soil_TP	   |kg/ha         |Total Soil Mineral P
#!!    sol_mass, FBM, FHP, RTNO, FHS, X1, RTO, sol_min_n
#!!    
def Soil_Chemical(workingDir):
    bd1File = workingDir + os.sep + "Density.tif"   ##sol_bd layer1 and layer2
    bd2File = workingDir + os.sep + "Density2.tif" ##sol_bd layer3
    om1File = workingDir + os.sep + "org.tif"       ##for sol_cbn layer1 and layers2
    om2File = workingDir + os.sep + "org2.tif"     ##for sol_cbn layer3
    rdFile = workingDir + os.sep + "RootDepth.tif"     ##for sol_z

    bd1Raster = ReadRaster(bd1File)
    bd2Raster = ReadRaster(bd2File)
    om1Raster = ReadRaster(om1File)
    om2Raster = ReadRaster(om2File)
    rdRaster = ReadRaster(rdFile)

    nCols = bd1Raster.nCols
    nRows = bd1Raster.nRows
    noDataValue = bd1Raster.noDataValue

    sol_bd1 = bd1Raster.data
    sol_bd2 = bd2Raster.data
    sol_org1 = om1Raster.data
    sol_org2 = om2Raster.data
    root_depth = rdRaster.data

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
    sol_rock = [0., 0., 0.]  ## fraction of rock in every soil layer, by default , sol_rock is 0
    cmtot_kgh = numpy.zeros((nRows,nCols)) ## !!kg/ha  current soil carbon integrated - aggregating all soil layers
    for lyr in range(len(sol_thick)):
        sol_mass = (sol_thick[lyr] / 1000.) * 10000. * sol_bd[lyr] * 1000. * (1 - sol_rock[lyr] / 100.)
        sol_cmass = sol_mass * (sol_cbn[lyr] / 100.)
        if lyr == 0:
            cmup_kgh = sol_cmass  ## !!kg/ha    current soil carbon for first soil layer
        cmtot_kgh = cmtot_kgh + sol_cmass
##    calculate initial nutrient contents of layers, profile and
##    average in soil for the entire watershed
##    convert mg/kg (ppm) to kg/ha
    xx = 0.
    sol_fop = numpy.ones((nRows,nCols)) # phosphorus stored in the fresh organic (residue) pool
    sol_fon = numpy.ones((nRows,nCols)) # nitrogen stored in the fresh organic (residue) pool
    sol_cov = numpy.ones((nRows,nCols)) # residue on soil surface
    #sol_rsd is organic matter in the soil layer classified as residue
      sol_fop(1) = sol_rsd(1) * .0010
      sol_fon(1) = sol_rsd(1) * .0055
      sol_cov(i) = sol_rsd(1)
      do j = 1, nly
        dg = 0.
        wt1 = 0.
        dg = (sol_z(j,i) - xx)
        wt1 = sol_bd(j,i) * dg / 100.              !! mg/kg => kg/ha
        conv_wt(j,i) = 1.e6 * wt1                  !! kg/kg => kg/ha

        if (sol_no3(j,i) <= 0.) then
          zdst = 0.
          zdst = Exp(-sol_z(j,i) / 1000.)
          sol_no3(j,i) = 10. * zdst * .7
        end if
        sol_no3(j,i) = sol_no3(j,i) * wt1          !! mg/kg => kg/ha
        sumno3 = sumno3 + sol_no3(j,i)

        if (sol_orgn(j,i) > 0.0001) then
          sol_orgn(j,i) = sol_orgn(j,i) * wt1      !! mg/kg => kg/ha
        else
          !! assume C:N ratio of 10:1
          sol_orgn(j,i) = 10000. * (sol_cbn(j,i) / 14.) * wt1  !! CN ratio changed back to 14 cibin 03022012
        end if
        sol_aorgn(j,i) = sol_orgn(j,i) * nactfr
        sol_orgn(j,i) = sol_orgn(j,i) * (1. - nactfr)
        sumorgn = sumorgn + sol_aorgn(j,i) + sol_orgn(j,i) +            
     &            sol_fon(j,i)

        if (sol_orgp(j,i) > 0.0001) then
          sol_orgp(j,i) = sol_orgp(j,i) * wt1      !! mg/kg => kg/ha
        else
	!! assume N:P ratio of 8:1 
          sol_orgp(j,i) = .125 * sol_orgn(j,i)   
        end if

        if (sol_solp(j,i) > 0.0001) then
          sol_solp(j,i) = sol_solp(j,i) * wt1      !! mg/kg => kg/ha
        else
          !! assume initial concentration of 5 mg/kg
          sol_solp(j,i) = 5. * wt1
        end if

    for i in range(nRows):
        for j in range(nCols):
            if abs(sol_bd1[i][j] - noDataValue) < DELTA:
                cmtot_kgh[i][j] = -9999
    testfile = workingDir+os.sep+"cmtot_kgh.tif"
    WriteGTiffFile(testfile, nRows, nCols, cmtot_kgh, bd1Raster.geotrans,\
                              bd1Raster.srs, -9999, gdal.GDT_Float32)
    
    print numpy.max(cmtot_kgh)

if __name__ == "__main__":
    nactfr = 0.5;
    workingDir = WORKING_DIR
    Soil_Chemical(WORKING_DIR);
    print "Soil chemical properties initialized done!"