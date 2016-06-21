from pymongo import MongoClient
from pymongo.errors import ConnectionFailure
from gridfs import *  
import math
from struct import *
from text import *
import util
import numpy as np

def cal_dis(x1, y1, x2, y2):
    dx = x2 - x1
    dy = y2 - y1
    return math.sqrt(dx*dx + dy*dy)

# there exist some problem in this funciton
def idw(x, y, locList):
    ex = 2
    coef_list = []
    sum = 0
    for pt in locList:
        dis = cal_dis(x, y, pt[0], pt[1])
        coef = math.pow(dis, -ex)
        coef_list.append(coef)
        sum = sum + coef
    weightList = []
    for coef in coef_list:
        weightList.append(coef/sum)
    #print weightList
    fmt = '%df'%(len(weightList))
    s = pack(fmt, *weightList)
    return s

def thiessen(x, y, locList):
    iMin = 0
    coef_list = []
    #print locList
    if len(locList) <= 1:
        coef_list.append(1)
        fmt = '%df'%(1)
        return pack(fmt, *coef_list), iMin
    
    disMin = cal_dis(x, y, locList[0][0], locList[0][1])
    
    coef_list.append(0)
    
    for i in range(1, len(locList)):
        coef_list.append(0)
        dis = cal_dis(x, y, locList[i][0], locList[i][1])
        #print x, y, locList[i][0], locList[i][1], dis
        if dis < disMin:
            iMin = i
            disMin = dis
    coef_list[iMin] = 1
    fmt = '%df'%(len(coef_list))

    s = pack(fmt, *coef_list)
    return s, iMin

def GenerateWeightDependentParameters(conn, subbasinID):
    '''
    Generate some parameters dependent on weight data and only should be calculated once.
    Such as PHUTOT (annual average total potential heat units)
    :return:
    '''
    dbModel = conn[SpatialDBName]
    spatial = GridFS(dbModel, DB_TAB_SPATIAL)
    #read mask file from mongodb
    maskName = str(subbasinID) + '_MASK'
    #read WEIGHT_M file from mongodb
    weightMName = str(subbasinID) + '_WEIGHT_M'
    mask = dbModel.spatial.files.find({"filename":maskName})[0]
    weightM = dbModel.spatial.files.find({"filename":weightMName})[0]
    numCells = int(weightM["metadata"]["NUM_CELLS"])
    numSites = int(weightM["metadata"]["NUM_SITES"])
    #read meteorlogy sites
    siteLists = dbModel[DB_TAB_SITELIST].find({'SubbasinID':subbasinID})
    siteList = siteLists.next()
    dbName = siteList['DB']
    mList = siteList.get('SiteListM')
    dbHydro = conn[dbName]

    siteList = mList.split(',')
    siteList = [int(item) for item in siteList]

    qDic = {Tag_ST_StationID: {'$in' : siteList}, Tag_DT_Type:Datatype_PHU0}
    cursor = dbHydro[Tag_ClimateDB_ANNUAL_STATS].find(qDic).sort(Tag_ST_StationID, 1)

    idList = []
    phuList = []
    for site in cursor:
        idList.append(site[Tag_ST_StationID])
        phuList.append(site[Tag_DT_Value])

    weightMData = spatial.get(weightM["_id"])
    totalLen = numCells*numSites
    fmt = '%df' % (totalLen,)
    weightMData = unpack(fmt, weightMData.read())

    #calculate PHU0
    phu0Data = np.zeros((numCells))
    for i in range(numCells):
        for j in range(numSites):
            phu0Data[i] += phuList[j] * weightMData[i * numSites + j]

    ysize = int(mask["metadata"]["NROWS"])
    xsize = int(mask["metadata"]["NCOLS"])
    noDataValue = mask["metadata"]["NODATA_VALUE"]
    maskData = spatial.get(mask["_id"])
    totalLen = xsize*ysize
    fmt = '%df' % (totalLen,)
    maskData = unpack(fmt, maskData.read())
    fname = "%s_%s" % (str(subbasinID), Datatype_PHU0)
    if(spatial.exists(filename=fname)):
        x = spatial.get_version(filename=fname)
        spatial.delete(x._id)
    metaDic = mask["metadata"]
    metaDic["TYPE"] = Datatype_PHU0
    metaDic["ID"] = fname
    metaDic["DESCRIPTION"] = Datatype_PHU0
    myfile = spatial.new_file(filename=fname, metadata=metaDic)
    vaildCount = 0
    for i in range(0, ysize):
        curRow = []
        for j in range(0, xsize):
            index = i*xsize + j
            #print index
            if(abs(maskData[index] - noDataValue) > util.DELTA):
                curRow.append(phu0Data[vaildCount])
                vaildCount += 1
            else:
                curRow.append(noDataValue)
        fmt = '%df'%(xsize)
        myfile.write(pack(fmt, *curRow))
    myfile.close()
    print vaildCount
def GenerateWeightInfo(conn, modelName, subbasinID, stormMode = False, useRsData = False):
    '''
    Generate and import weight information using Thiessen polygon method.
    :param conn:
    :param modelName:
    :param subbasinID:
    :param stormMode:
    :param useRsData:
    :return:
    '''
    #print "\t Subbasin:", subbasinID
    dbModel = conn[modelName]
    spatial = GridFS(dbModel, DB_TAB_SPATIAL)

    #read mask file from mongodb
    maskName = str(subbasinID) + '_MASK'
    mask = dbModel.spatial.files.find({"filename":maskName})[0]

    ysize = int(mask["metadata"]["NROWS"])
    xsize = int(mask["metadata"]["NCOLS"])
    noDataValue = mask["metadata"]["NODATA_VALUE"]
    dx = mask["metadata"]["CELLSIZE"]
    xll = mask["metadata"]["XLLCENTER"]
    yll = mask["metadata"]["YLLCENTER"]

    data = spatial.get(mask["_id"])
    
    totalLen = xsize*ysize
    fmt = '%df' % (totalLen,)
    data = unpack(fmt, data.read())
    #print data[0], len(data), type(data)
    
    #count number of valid cells
    num = 0
    for i in range(0, totalLen):
        if(abs(data[i] - noDataValue) > util.DELTA):
            num = num + 1
    
    #read stations information from database    
    metadic = {
    'SUBBASIN': subbasinID, 
    'NUM_CELLS': num}    

    siteLists = dbModel[DB_TAB_SITELIST].find({'SubbasinID':subbasinID})
    siteList = siteLists.next()
    dbName = siteList['DB']
    pList = siteList.get('SiteListP')
    mList = siteList.get('SiteListM')
    petList = siteList.get('SiteListPET')
    #print pList
    #print mList
    dbHydro = conn[dbName]
    
    typeList = [DataType_Meteorology, DataType_Precipitation, DataType_PotentialEvapotranspiration]
    siteLists = [mList, pList, petList]
    if petList is None:
        del typeList[2]
        del siteLists[2]
    
    if(stormMode):
        typeList = [DataType_Precipitation]
        siteLists = [pList]   
    #print typeList
    #print siteLists
    
    for i in range(len(typeList)):
        fname = '%d_WEIGHT_%s'%(subbasinID,typeList[i])
        print fname
        siteList = siteLists[i]
        if siteList != None:
            siteList = siteList.split(',')
            #print siteList
            siteList = [int(item) for item in siteList]            
            metadic['NUM_SITES'] = len(siteList)
            #print siteList
            qDic = {Tag_ST_StationID: {'$in' : siteList}, Tag_ST_Type:typeList[i]}
            cursor = dbHydro[Tag_ClimateDB_Sites].find(qDic).sort(Tag_ST_StationID, 1)
            
            #meteorology station can also be used as precipitation station
            if cursor.count() == 0 and typeList[i] == DataType_Precipitation:
                qDic = {Tag_ST_StationID: {'$in' : siteList}, Tag_ST_Type:DataType_Meteorology}
                cursor = dbHydro[Tag_ClimateDB_Sites].find(qDic).sort(Tag_ST_StationID, 1)
            
            #get site locations 
            idList = []
            LocList = []
            for site in cursor:
                idList.append(site[Tag_ST_StationID])
                LocList.append([site[Tag_ST_LocalX], site[Tag_ST_LocalY]])
                #print site['ID'], [site['LocalX'], site['LocalY']]
            #print 'loclist', locList
            #interpolate using the locations
            #weightList = []
            myfile = spatial.new_file(filename=fname, metadata=metadic)
            #fTest = open(r"/data/tmp/weight_%d_%s.txt"%(subbasinID, typeList[i]), 'w')
            for i in range(0, ysize):
                for j in range(0, xsize):
                    index = i*xsize + j
                    #print index
                    if(abs(data[index] - noDataValue) > util.DELTA):
                        #x = geo[0] + (j+0.5)*geo[1] + i*geo[2]
                        #y = geo[3] + j*geo[4] + (i+0.5)*geo[5]
                        x = xll + j*dx
                        y = yll + (ysize-i-1)*dx
                        nearIndex = 0
                        #print locList
                        if useRsData:
                            line, nearIndex = thiessen(x, y, LocList)
                        else:
                            #line = idw(x, y, locList)
                            line, nearIndex = thiessen(x, y, LocList)
                        #weightList.append(line)
                        myfile.write(line)
                        #fTest.write("%f %f "%(x,y) + line+"\n")
                        
                        #print line
            #weightStr = '\n'.join(weightList)
            #spatial.put(weightStr, filename=fname, metadata=metadic)     
            myfile.close()
            #fTest.close()
            
if __name__ == "__main__":
    #hostname = '192.168.6.55'
    hostname = '127.0.0.1'
    port = 27017
    try:
        conn = MongoClient(host=hostname, port=27017)
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
        
    #for i in range(1, 900):
    #GenerateWeightInfo(conn, 'model_dianbu_10m_longterm', 1, False)
    GenerateWeightDependentParameters(conn,1)
    print 'done!'
    
    
    

