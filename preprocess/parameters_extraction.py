#! /usr/bin/env python
#coding=utf-8
## @Extract parameters from landuse, soil properties etc.
#
#
import os
from gen_lookup_table import CreateLookupTable
#from reclass_landuse import ReclassLanduse
from soil_param import GenerateSoilAttributes 
from soil_texture import SoilTexture
from init_moisture import InitMoisture
from reclass_crop import ReclassCrop
from depression import DepressionCap
from cn2 import GenerateCN2
from radius import GenerateRadius
from velocity import GenerateVelocity
from t0_s import GenerateT0_s
from delta_s import GenerateDelta_s
from runoff_coef import RunoffCoefficent

from config import *
from util import *

def soil_parameters(dstdir, maskFile, sandList, clayList, orgList=None):
    # mask soil map using the mask_raster program
    configFile = "%s%smaskSoilConfig.txt" % (dstdir, os.sep)
    n = 0
    for item in sandList:
        if item is not None:
            n += 1
    if orgList is None:
        total = n * 2
    else:
        total = n * 3
    fMask = open(configFile, 'w')
    fMask.write(maskFile+"\n")
    fMask.write("%d\n"%(total))  # modified by Zhu LJ,2015-04-01
#    defaultSand = 40
#    defaultClay = 30
#    defaultOrg = 2.5
    for i in range(n):
        strLayer = str(i+1) if i > 0 else ''
        sandFile = "%s/sand%s.tif" % (dstdir, strLayer)
        clayFile = "%s/clay%s.tif" % (dstdir, strLayer)
        orgFile = "%s/org%s.tif" % (dstdir, strLayer)
        fMask.write("%s\t%d\t%s\n" % (sandList[i], defaultSand, sandFile))
        fMask.write("%s\t%d\t%s\n" % (clayList[i], defaultClay, clayFile))    
        if orgList is not None:
            fMask.write("%s\t%f\t%s\n" % (orgList[i], defaultOrg, orgFile)) 
    fMask.close()

    s = "%s/mask_raster %s" % (CPP_PROGRAM_DIR, configFile)
    os.system(s)

    for i in range(n):
        strLayer = str(i+1) if i > 0 else ''
        sandFile = "%s/sand%s.tif" % (dstdir, strLayer)
        clayFile = "%s/clay%s.tif" % (dstdir, strLayer)
        orgFile = "%s/som%s.tif" % (dstdir, strLayer)
        GenerateSoilAttributes(dstdir, i+1, sandFile, clayFile, orgFile)
    SoilTexture(dstdir)
    
def landuse_parameters(dstdir, maskFile, inputLanduse, landuseFile, sqliteFile, defaultLanduse):
    ## mask landuse map using the mask_raster program
    configFile = "%s%smaskLanduseConfig.txt" % (dstdir, os.sep)
    fMask = open(configFile, 'w')
    fMask.write(maskFile+"\n")
    fMask.write("%d\n" % (1,))
    s = "%s\t%d\t%s\n" % (inputLanduse, defaultLanduse, landuseFile)
    fMask.write(s)
    fMask.close()
    s = "%s/mask_raster %s" % (CPP_PROGRAM_DIR, configFile)
    os.system(s)
    
    ## reclassify
    reclassLuFile = "%s/reclassLanduseConfig.txt" % (dstdir)
    fReclassLu = open(reclassLuFile, 'w')
    fReclassLu.write("%s\t%d\n" % (landuseFile, 8))
    fReclassLu.write("%s/lookup\n" % (dstdir))
    fReclassLu.write(dstdir + "\n")
    landuseAttrList = LANDUSE_ATTR_LIST
    n = len(landuseAttrList)
    fReclassLu.write("%d\n" % (n))
    fReclassLu.write("\n".join(landuseAttrList))
    fReclassLu.close()

    s = "%s/reclassify %s %s/cpp_src/reclassify/neigh.nbr" % (CPP_PROGRAM_DIR, reclassLuFile, PREPROC_SCRIPT_DIR)
    os.system(s)
    #ReclassLanduse(landuseFile, sqliteFile, dstdir)
    
def ExtractParameters(inputLanduse, sandList, clayList, orgList, dstdir, genCN=False, genRunoffCoef=False, genCrop=False, genIUH=False, defaultLanduse=8):
    
    maskFile = dstdir + os.sep + mask_to_ext
    landuseFile = dstdir + os.sep + landuseMFile
    statusFile = dstdir + os.sep + "status_ExtractParameters.txt"
    f = open(statusFile, 'w')
    ## generate landuse and soil properties lookup tables
    status = "Generating landuse and soil properties lookup tables..."
    print "[Output] %s, %s" % (dstdir, status)
    f.write("%d,%s\n" % (10, status))
    f.flush()
    str_sql = 'select landuse_id, ' + ','.join(LANDUSE_ATTR_DB) + ' from LanduseLookup'
    CreateLookupTable(sqliteFile, LANDUSE_ATTR_LIST, str_sql,dstdir)
    str_sql = 'select soilcode,' + ','.join(SOIL_ATTR_DB) + ' from SoilLookup'
    CreateLookupTable(sqliteFile, SOIL_ATTR_LIST, str_sql,dstdir)
    
    ## landuse parameters
    status = "Generating landuse attributes..."
    print "[Output] %s, %s" % (dstdir, status)
    f.write("%d,%s\n" % (20, status))
    f.flush()
    landuse_parameters(dstdir, maskFile, inputLanduse, landuseFile, sqliteFile, defaultLanduse)

    ## soil parameters
    status = "Calculating inital soil parameters..."
    print "[Output] %s, %s" % (dstdir, status)
    f.write("%d,%s\n" % (30, status))
    f.flush()
    soil_parameters(dstdir, maskFile, sandList, clayList, orgList)
    
    ## parameters derived from DEM
    status = "Calculating inital soil moisture..."
    print "[Output] %s, %s" % (dstdir, status)
    f.write("%d,%s\n" % (40, status))
    f.flush()
    InitMoisture(dstdir)

    if genCrop:
        status = "Generating crop attributes..."
        print "[Output] %s, %s" % (dstdir, status) 
        f.write("%d,%s\n" % (50, status))
        f.flush()
        
        reclassFile = "%s/reclassCropConfig.txt" % (dstdir)
        fReclass = open(reclassFile, 'w')
        fReclass.write("%s\t%d\n" % (landuseFile, 8))
        fReclass.write("%s/lookup\n" % (dstdir))
        fReclass.write(dstdir + "\n")
        attrList = CROP_ATTR_LIST
        n = len(attrList)
        fReclass.write("%d\n" % (n))
        fReclass.write("\n".join(attrList))
        fReclass.close()

        s = "%s/reclassify %s %s/cpp_src/reclassify/neigh.nbr" % (CPP_PROGRAM_DIR, reclassFile, PREPROC_SCRIPT_DIR)
        os.system(s)
        ReclassCrop(landuseFile, dstdir)

    status = "Calculating depression storage..."
    print "[Output] %s, %s" % (dstdir, status) 
    f.write("%d,%s\n" % (70, status))
    f.flush()
    DepressionCap(dstdir, sqliteFile)
    
    if genCN:
        status = "Calculating CN numbers..."
        print "[Output] %s, %s" % (dstdir, status) 
        f.write("%d,%s\n" % (80, status))
        f.flush()
        GenerateCN2(dstdir, sqliteFile)
        
    if genIUH:
        status =  "Calculating IUH parameters..."
        print "[Output] %s, %s" % (dstdir, status) 
        f.write("%d,%s\n" % (85, status))
        f.flush()
        GenerateRadius(dstdir, "T2")
        GenerateVelocity(dstdir)
        GenerateT0_s(dstdir)
        GenerateDelta_s(dstdir)    
    
    if genRunoffCoef:
        status =  "Calculating runoff coefficient..."
        print "[Output] %s, %s" % (dstdir, status) 
        f.write("%d,%s\n" % (90, status)) 
        f.flush()
        RunoffCoefficent(dstdir, sqliteFile) 
    f.write("%d,%s\n" % (100, "Finished!"))
    f.close()
