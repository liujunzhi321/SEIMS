#! /usr/bin/env python
#coding=utf-8
## @Extract parameters from landuse, soil properties etc.
#
from gen_lookup_table import CreateLookupTable
#from reclass_landuse import ReclassLanduse
from soil_param import *
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
from util import *
import numpy
import types
def soil_parameters2(dstdir, maskFile, soilSEQNTif, soilSEQNTxt):
    soilSEQNData = ReadDataItemsFromTxt(soilSEQNText)
    defaultSoilType = float(soilSEQNData[1][0])
    # ## Mask soil_SEQN tif
    # configFile = "%s%smaskSoilConfig.txt" % (dstdir, os.sep)
    # fMask = open(configFile, 'w')
    # fMask.write(maskFile+"\n")
    # fMask.write("%d\n" % (1,))
    soiltypeFile = dstdir + os.sep + soiltypeMFile
    # s = "%s\t%d\t%s\n" % (soilSEQNTif, defaultSoilType, soiltypeFile)
    # fMask.write(s)
    # fMask.close()
    # s = "%s/mask_raster %s" % (CPP_PROGRAM_DIR, configFile)
    # os.system(s)

    ## Read soil properties from txt file
    soilInstances = []
    soilPropFlds = soilSEQNData[0][:]
    for i in range(1, len(soilSEQNData)):
        curSoilDataItem = soilSEQNData[i][:]
        curSEQN = curSoilDataItem[0]
        curSNAM = curSoilDataItem[1]
        curSoilIns = SoilProperty(curSEQN, curSNAM)
        for j in range(2, len(soilPropFlds)):
            curFlds = SplitStr(curSoilDataItem[j], ',')                  ## Get field values
            for k in range(len(curFlds)): curFlds[k] = float(curFlds[k]) ## Convert to float
            if StringMatch(soilPropFlds[j], SOL_NLYRS) or StringMatch(soilPropFlds[j], 'NLAYERS'):
                curSoilIns.SoilLAYERS = int(curFlds[0])
            elif StringMatch(soilPropFlds[j], SOL_Z) or StringMatch(soilPropFlds[j], 'SOL_Z'):
                curSoilIns.SoilDepth = curFlds
            elif StringMatch(soilPropFlds[j], SOL_OM) or StringMatch(soilPropFlds[j], 'SOL_OM'):
                curSoilIns.OM = curFlds
            elif StringMatch(soilPropFlds[j], SOL_CLAY) or StringMatch(soilPropFlds[j], 'SOL_CLAY'):
                curSoilIns.CLAY = curFlds
            elif StringMatch(soilPropFlds[j], SOL_SILT) or StringMatch(soilPropFlds[j], 'SOL_SILT'):
                curSoilIns.SILT = curFlds
            elif StringMatch(soilPropFlds[j], SOL_SAND) or StringMatch(soilPropFlds[j], 'SOL_SAND'):
                curSoilIns.SAND = curFlds
            elif StringMatch(soilPropFlds[j], SOL_ROCK) or StringMatch(soilPropFlds[j], 'SOL_ROCK'):
                curSoilIns.ROCK = curFlds
            elif StringMatch(soilPropFlds[j], SOL_ZMX) or StringMatch(soilPropFlds[j], 'SOL_ZMX'):
                curSoilIns.Sol_ZMX = curFlds[0]
            elif StringMatch(soilPropFlds[j], SOL_ANIONEXCL) or StringMatch(soilPropFlds[j], 'ANIONEXCL'):
                curSoilIns.ANION_EXCL = curFlds[0]
            elif StringMatch(soilPropFlds[j], SOL_CRK) or StringMatch(soilPropFlds[j], 'SOL_CRK'):
                curSoilIns.Sol_CRK = curFlds[0]
            elif StringMatch(soilPropFlds[j], SOL_BD) or StringMatch(soilPropFlds[j], 'SOL_BD'):
                curSoilIns.Density = curFlds
            elif StringMatch(soilPropFlds[j], SOL_K) or StringMatch(soilPropFlds[j], 'SOL_K'):
                curSoilIns.Conductivity = curFlds
            elif StringMatch(soilPropFlds[j], SOL_WP) or StringMatch(soilPropFlds[j], 'SOL_WP'):
                curSoilIns.WiltingPoint = curFlds
            elif StringMatch(soilPropFlds[j], SOL_FC) or StringMatch(soilPropFlds[j], 'SOL_FC'):
                curSoilIns.FieldCap = curFlds
            elif StringMatch(soilPropFlds[j], SOL_AWC) or StringMatch(soilPropFlds[j], 'SOL_AWC'):
                curSoilIns.Sol_AWC = curFlds
            elif StringMatch(soilPropFlds[j], SOL_POROSITY) or StringMatch(soilPropFlds[j], 'SOL_POROSITY'):
                curSoilIns.POROSITY = curFlds
            elif StringMatch(soilPropFlds[j], SOL_USLE_K) or StringMatch(soilPropFlds[j], 'SOL_USLE_K'):
                curSoilIns.USLE_K = curFlds
            elif StringMatch(soilPropFlds[j], SOL_P_INDEX) or StringMatch(soilPropFlds[j], 'SOL_P_INDEX'):
                curSoilIns.Poreindex = curFlds
            elif StringMatch(soilPropFlds[j], SOL_ALB) or StringMatch(soilPropFlds[j], 'SOL_ALB'):
                curSoilIns.Sol_ALB = curFlds
            elif StringMatch(soilPropFlds[j], SOL_RM) or StringMatch(soilPropFlds[j], 'SOL_RM'):
                curSoilIns.Residual = curFlds
        curSoilIns.CheckData()
        soilInstances.append(curSoilIns)
    soilPropDict = {}
    for sol in soilInstances:
        curSolDict = sol.SoilDict()
        for fld in curSolDict.keys():
            if fld in soilPropDict.keys():
                soilPropDict[fld].append(curSolDict[fld])
            else:
                soilPropDict[fld] = [curSolDict[fld]]
    # print soilPropDict.keys()
    # print soilPropDict.values()

    replaceDicts = []
    dstSoilTifs  = []
    SEQNs = soilPropDict[SOL_SEQN]
    maxLyrNum = int(numpy.max(soilPropDict[SOL_NLYRS]))
    for key in soilPropDict.keys():
        if key != SOL_SEQN and key != SOL_NAME:
            keyL = 1
            for keyV in soilPropDict[key]:
                if type(keyV) is types.ListType:
                    if len(keyV) > keyL:
                        keyL = len(keyV)
            if keyL == 1:
                curDict = {}
                for i in range(len(SEQNs)):
                    curDict[float(SEQNs[i])] = soilPropDict[key][i]
                replaceDicts.append(curDict)
                dstSoilTifs.append(dstdir+os.sep+key+'.tif')
            else:
                for i in range(maxLyrNum):
                    curDict = {}
                    for j in range(len(SEQNs)):
                        if i < soilPropDict[SOL_NLYRS][j]:
                            curDict[float(SEQNs[j])] = soilPropDict[key][j][i]
                        else:
                            curDict[float(SEQNs[j])] = DEFAULT_NODATA
                    replaceDicts.append(curDict)
                    dstSoilTifs.append(dstdir+os.sep+key+'_'+str(i+1)+'.tif')
    print replaceDicts
    print(len(replaceDicts))
    print dstSoilTifs
    print(len(dstSoilTifs))

    ## Generate GTIFF
    for i in range(len(dstSoilTifs)):
        replaceByDict(soiltypeFile,replaceDicts[i],dstSoilTifs[i])


### Deprecated by LJ, 2016-5-21
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

    for i in range(n):
        #strLayer = str(i+1) if i > 0 else ''
        sandFile = "%s/sand_%s.tif" % (dstdir, str(i+1))
        clayFile = "%s/clay_%s.tif" % (dstdir, str(i+1))
        orgFile = "%s/org_%s.tif" % (dstdir, str(i+1))
        fMask.write("%s\t%d\t%s\n" % (sandList[i], defaultSand, sandFile))
        fMask.write("%s\t%d\t%s\n" % (clayList[i], defaultClay, clayFile))    
        if orgList is not None:
            fMask.write("%s\t%f\t%s\n" % (orgList[i], defaultOrg, orgFile)) 
    fMask.close()

    s = "%s/mask_raster %s" % (CPP_PROGRAM_DIR, configFile)
    os.system(s)

    for i in range(n):
        #strLayer = str(i+1) if i > 0 else ''
        sandFile = "%s/sand_%s.tif" % (dstdir, str(i+1))
        clayFile = "%s/clay_%s.tif" % (dstdir, str(i+1))
        orgFile = "%s/org_%s.tif" % (dstdir, str(i+1))
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
    
def ExtractParameters(inputLanduse, dstdir, genCN=False, genRunoffCoef=False, genCrop=False, genIUH=False, defaultLanduse=8):
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
    # Change soil properties to Raster2D data, 2016-5-20, LJ
    #str_sql = 'select soilcode,' + ','.join(SOIL_ATTR_DB) + ' from SoilLookup'
    #CreateLookupTable(sqliteFile, SOIL_ATTR_LIST, str_sql,dstdir)
    
    ## landuse parameters
    status = "Generating landuse attributes..."
    print "[Output] %s, %s" % (dstdir, status)
    f.write("%d,%s\n" % (20, status))
    f.flush()
    landuse_parameters(dstdir, maskFile, inputLanduse, landuseFile, sqliteFile, defaultLanduse)

    ## soil physical and chemical parameters
    status = "Calculating inital soil physical and chemical parameters..."
    print "[Output] %s, %s" % (dstdir, status)
    f.write("%d,%s\n" % (30, status))
    f.flush()
    soil_parameters2(WORKING_DIR, maskFile, soilSEQNFile, soilSEQNText)
    ##soil_parameters(dstdir, maskFile, sandList, clayList, orgList) ## replaced by LJ, 2016-5-21
    
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

if __name__ == "__main__":
    maskFile = r'E:\data\model_data\model_dianbu_30m_longterm\data_prepare\output\mask.tif'
    soil_parameters2(WORKING_DIR,maskFile,soilSEQNFile,soilSEQNText)
