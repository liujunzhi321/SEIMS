## /usr/bin/env python
#coding=utf-8
### This program initializes soil chemical properties
### for nutrient related modules.
### Reference: soil_chem.f in SWAT 2012
### Author: Liangjun Zhu, Huiran Gao
### Date: 2016-3-28

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

### Revised by LiangJun Zhu, 2016-5-21
### @param[in] nlyrs Soil layers number
### @param[in] depth Depth from soil surface to bottom of layer (mm), e.g., 100, 400, 800.
### @param[in] om organic matter
### @param[in] clay
### @param[in] rock
### @param[in] bd bulk density
def SoilChemProperties(nlyrs, depth, om, clay, rock, bd):
    ## Calculate thick of each layer
    sol_thick = [depth[0]]
    for k in range(1, nlyrs):
        sol_thick.append(depth[k] - depth[k - 1])
    cbn = numpy.multiply(om, 0.58) ## van bemmelen coefficient
    ## Define some const values
    nactfr = .02
    ## Initialize some layer-based temporary and output variables
    sol_mass = numpy.zeros((nlyrs))
    sol_cmass = numpy.zeros((nlyrs))
	
    sol_fop = numpy.ones((nlyrs)) # phosphorus stored in the fresh organic (residue) pool
    sol_fon = numpy.ones((nlyrs)) # nitrogen stored in the fresh organic (residue) pool
    sol_cov = numpy.ones((nlyrs)) # residue on soil surface

    sol_orgn = numpy.ones((nlyrs))
    sol_no3 = numpy.zeros((nlyrs))
    sol_aorgn = numpy.zeros((nlyrs))
    sol_orgp = numpy.ones((nlyrs))
    sol_solp = numpy.zeros((nlyrs))
    sol_actp = numpy.zeros((nlyrs))
    sol_stap = numpy.zeros((nlyrs))
    sol_hum = numpy.zeros((nlyrs))
    sol_rsd = numpy.zeros((nlyrs)) # Organic matter in the soil layer classified as residue, currently assigned 0. TODO

    ## Initialize some single temporary and output variables
    cmup_kgh = 0.  ## kg/ha  current soil carbon for first soil layer
    cmtot_kgh = 0. ## kg/ha  current soil carbon integrated - aggregating all soil layers
    sumno3 = 0.
    sumorgn = 0.
    summinp = 0.
    sumorgp = 0.
    ## Calculate by layer
    ## calculate sol_cbn for lower layers if only have upper layer's data
    if nlyrs >= 3 and cbn[3] <= 0:
        for i in range(2,nlyrs):
            tmpDepth = depth[i] - depth[1]
            cbn[i] = cbn[i-1] * numpy.exp(-.001 * tmpDepth)

    for lyr in range(nlyrs):
        sol_mass[lyr] = (sol_thick[lyr] / 1000.) * 10000. * 1000. * bd[lyr] * (1 - rock[lyr]/100.)  ## soil mass of current layer (kg)
        sol_cmass[lyr] = sol_mass[lyr] * (cbn[lyr] / 100.) ## Carbon mass of current layer (kg)
        if lyr == 0:
            cmup_kgh = sol_cmass[lyr]
        cmtot_kgh = cmtot_kgh + sol_cmass[lyr]


    ##    calculate initial nutrient contents of layers, profile and
    ##    average in soil for the entire watershed
    ##    convert mg/kg (ppm) to kg/ha

    for j in range(nlyrs):
        wt1 = bd[j] * sol_thick[j] / 100.              ## mg/kg => kg/ha
        conv_wt = 1.e6 * wt1                    ## kg/kg => kg/ha

        sol_fop[j] = sol_rsd[j] * .0010
        sol_fon[j] = sol_rsd[j] * .0055
        sol_cov = sol_rsd[1]

        #if (sol_no3[j] <= 0.) :
        zdst = math.exp(-depth[j] / 1000.)
        sol_no3[j] = 10. * zdst * .7
        sol_no3[j] = sol_no3[j] * wt1          ## mg/kg => kg/ha
        sumno3 = sumno3 + sol_no3[j]

        sol_orgn[j] = sol_orgn[j] * wt1
        if (sol_orgn[j] > 0.0001):
            sol_orgn[j] = sol_orgn[j] * wt1      ## mg/kg => kg/ha
        else:
            ## assume C:N ratio of 10:1
            sol_orgn[j] = 10000. * (cbn[j] / 14.) * wt1  ## CN ratio changed back to 14
        sol_aorgn[j] = sol_orgn[j] * nactfr
        sol_orgn[j] = sol_orgn[j] * (1. - nactfr)

        sumorgn = sumorgn + sol_aorgn[j] + sol_orgn[j] + sol_fon[j]

        sol_orgp[j] = sol_orgp[j] * wt1
        sol_solp[j] = sol_solp[j] * wt1

        if (sol_orgp[j] > 0.0001):
            sol_orgp[j] = sol_orgp[j] * wt1      ## mg/kg => kg/ha
        else:
            ## assume N:P ratio of 8:1
            sol_orgp[j] = .125 * sol_orgn[j]
        if (sol_solp[j] > 0.0001) :
            sol_solp[j] = sol_solp[j] * wt1     ## mg/kg => kg/ha
        else:
            ## assume initial concentration of 5 mg/kg
            sol_solp[j] = 5. * wt1

        solp = 0.
        actp = 0.
        ## Set active pool based on dynamic PSP MJW
        if (conv_wt != 0):
            solp = (sol_solp[j] / conv_wt) * 1000000.
            if (clay[j] > 0.):
                psp = -0.045 * math.log(clay[j])+ (0.001 * solp)
                psp = psp - (0.035  * cbn[j]) + 0.43
            else:
                psp = 0.4
            ## Limit PSP range
            if (psp <.05) :
                 psp = 0.05
            elif (psp > 0.9):
                psp = 0.9

        sol_actp[j] = sol_solp[j] * (1. - psp) / psp

        ## Set Stable pool based on dynamic coefficant
        if (conv_wt != 0) :  ## From White et al 2009
            ## convert to concentration for ssp calculation
            actp = sol_actp[j] / conv_wt * 1000000.
            solp = sol_solp[j] / conv_wt * 1000000.
            ## estimate Total Mineral P in this soil based on data from sharpley 2004
            ssp = 25.044 * ((actp + solp) ** -0.3833)
            ##limit SSP Range
            if (ssp > 7.):
                ssp = 7.
            if (ssp < 1.):
                 ssp = 1.
            sol_stap[j] = ssp * (sol_actp[j] + sol_solp[j])  #define stableP
        else:
            ## The original code
            sol_stap[j] = 4. * sol_actp[j]

        sol_hum[j] = cbn[j] * wt1 * 17200.
        summinp = summinp + sol_solp[j] + sol_actp[j] + sol_stap[j]
        sumorgp = sumorgp + sol_orgp[j] + sol_fop[j]

    return (list(sol_fop), list(sol_fon), list(sol_no3), list(sol_orgn), list(sol_aorgn), list(sol_orgp),
            list(sol_solp), list(sol_actp), list(sol_stap), list(sol_hum), sol_cov, sumno3,
            sumorgn, summinp, sumorgp)

## Deprecated by LJ, 2016-5-21
# def writeTIFF(file, fpath, workingDir):
#     tiffile = workingDir + os.sep + "Density1.tif"
#     nRows = ReadRaster(tiffile).nRows
#     nCols = ReadRaster(tiffile).nCols
#     geotrans = ReadRaster(tiffile).geotrans
#     srs = ReadRaster(tiffile).srs
#     WriteGTiffFile(fpath, nRows, nCols, file, geotrans, srs, -9999, gdal.GDT_Float32)
#     print "%s" % fpath

## Test
if __name__ == "__main__":
    print WORKING_DIR
    nactfr = 0.5
    nlyrs = 2
    depth = [100,400]
    om = [1.1, 0.7]
    clay = [20,18]
    rock = [0,0]
    bd = [1.5,1.7]
    a = SoilChemProperties(nlyrs, depth, om, clay, rock, bd)
    print a
    print "Soil chemical properties initialized done!"
