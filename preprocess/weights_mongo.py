import glob
import os, sys
from pymongo import Connection
from pymongo.errors import ConnectionFailure
from gridfs import *  
import time
from datetime import datetime
from osgeo import gdal
import math
from struct import *
from config import *
import util

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

                       
def GenerateWeightInfo(conn, modelName, subbasinID, stormMode = False, useRsData = False):
    #print "\t Subbasin:", subbasinID
    dbModel = conn[modelName]
    spatial = GridFS(dbModel, 'spatial')
        
    #read mask file from mongodb
    maskName = str(subbasinID) + '_MASK'
    #print maskName
    mask = dbModel.spatial.files.find({"filename":maskName})[0]
    #data = band.ReadAsArray()
    ysize = int(mask["metadata"]["NROWS"])
    xsize = int(mask["metadata"]["NCOLS"])
    noDataValue =  mask["metadata"]["NODATA_VALUE"]
    dx = mask["metadata"]["CELLSIZE"]
    xll = mask["metadata"]["XLLCENTER"]
    yll = mask["metadata"]["YLLCENTER"]
    #print xsize, ysize, noDataValue
    #print dx, xll, yll
    
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

    siteLists = dbModel.SiteList.find({'SubbasinID':subbasinID})
    siteList = siteLists.next()
    dbName = siteList['DB']
    pList = siteList.get('SiteListP')
    mList = siteList.get('SiteListM')
    petList = siteList.get('SiteListPET')
    #print pList
    #print mList
    dbHydro = conn[dbName]
    
    typeList = ['M', 'P', 'PET']
    siteLists = [mList, pList, petList]
    if petList is None:
        del typeList[2]
        del siteLists[2]
    
    if(stormMode):
        typeList = ['P']
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
            qDic = {'ID': {'$in' : siteList}, 'Type':typeList[i]}
            cursor = dbHydro.Sites.find(qDic).sort('ID', 1)
            
            #meteorology station can also be used as precipitation station
            if cursor.count() == 0 and typeList[i] == 'P':
                qDic = {'ID': {'$in' : siteList}, 'Type':'M'}
                cursor = dbHydro.Sites.find(qDic).sort('ID', 1)
            
            #get site locations 
            idList = []
            locList = []
            for site in cursor:
                idList.append(site['ID'])
                locList.append([site['LocalX'], site['LocalY']])
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
                            line, nearIndex = thiessen(x, y, locList)
                        else:
                            #line = idw(x, y, locList)
                            line, nearIndex = thiessen(x, y, locList)
                        #weightList.append(line)
                        myfile.write(line)
                        #fTest.write("%f %f "%(x,y) + line+"\n")
                        
                        #print line
            #weightStr = '\n'.join(weightList)
            #spatial.put(weightStr, filename=fname, metadata=metadic)     
            myfile.close()
            #fTest.close()
            
if __name__ == "__main__":
    hostname = '192.168.6.55'
    port = 27017
    try:
        conn = Connection(host=hostname, port=27017)
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
        
    #for i in range(1, 900):
    GenerateWeightInfo(conn, 'cluster_model_ganjiang', 125, False)
    
    print 'done!'
    
    
    

