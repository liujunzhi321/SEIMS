#! /usr/bin/env python
#coding=utf-8
## @Meteorological daily data import
#
#
from pymongo import MongoClient
from pymongo.errors import ConnectionFailure
from util import *
import math
from text import *

def ImportData(db, measFileList, sitesLoc):
    for measDataFile in measFileList:
        dataPath = MEASUREMENT_DATA_DIR + os.sep + measDataFile
        measDataItems = ReadDataItemsFromTxt(dataPath)
        measFlds = measDataItems[0]
        requiredFlds = [Tag_ST_SiteID, Tag_DT_Year, Tag_DT_Month, Tag_DT_Day, \
                        Tag_DT_Type,Tag_DT_Value]
        for fld in requiredFlds:
            if not StringInList(fld, measFlds):
                exit(0)  ### data can not meet the request!
        for i in range(1,len(measDataItems)):
            dic = {}
            for j in range(len(measDataItems[i])):
                if StringMatch(measFlds[j], Tag_ST_SiteID):
                    dic[Tag_ST_SiteID] = int(measDataItems[i][j])
                elif StringMatch(measFlds[j], Tag_DT_Year):
                    curY = int(measDataItems[i][j])
                elif StringMatch(measFlds[j], Tag_DT_Month):
                    curM = int(measDataItems[i][j])
                elif StringMatch(measFlds[j], Tag_DT_Day):
                    curD = int(measDataItems[i][j])
                elif StringMatch(measFlds[j], Tag_DT_Type):
                    dic[Tag_DT_Type] = measDataItems[i][j]
                elif StringMatch(measFlds[j], Tag_DT_Value):
                    dic[Tag_DT_Value] = float(measDataItems[i][j])
            dt = datetime.datetime(curY, curM, curD, 0, 0)
            sec = time.mktime(dt.timetuple())
            utcTime = time.gmtime(sec)
            dic[Tag_DT_LocalT] = dt
            dic[Tag_DT_Zone] = time.timezone / 3600
            dic[Tag_DT_UTC] = datetime.datetime(utcTime[0], utcTime[1], utcTime[2], utcTime[3])
            db[Tag_ClimateDB_Measurement].insert_one(dic)
    for siteFile in sitesLoc:
        dataPath = MEASUREMENT_DATA_DIR + os.sep + siteFile
        siteDataItems = ReadDataItemsFromTxt(dataPath)
        siteFlds = siteDataItems[0]
        for i in range(1,len(siteDataItems)):
            dic = {}
            for j in range(len(siteDataItems[i])):
                if StringMatch(siteFlds[j], Tag_ST_SiteID):
                    dic[Tag_ST_SiteID] = int(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_Type):
                    type = siteDataItems[i][j].split(',')
                elif StringMatch(siteFlds[j], Tag_ST_Latitude):
                    dic[Tag_ST_Latitude] = float(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_Longitude):
                    dic[Tag_ST_Longitude] = float(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_LocalX):
                    dic[Tag_ST_LocalX] = float(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_LocalY):
                    dic[Tag_ST_LocalY] = float(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_UNIT):
                    unit = siteDataItems[i][j].split(',')
                elif StringMatch(siteFlds[j], Tag_ST_IsOutlet):
                    dic[Tag_ST_IsOutlet] = int(siteDataItems[i][j])

            for j in range(len(type)):
                if type[j] == 'FLOW':
                    dic[Tag_ST_UNIT] = unit[0]
                else:
                    dic[Tag_ST_UNIT] = unit[len(unit) - 1]
                curDic = {}
                curDic[Tag_ST_SiteID] = dic[Tag_ST_SiteID]
                curDic[Tag_ST_Type] = type[j]
                curDic[Tag_ST_Latitude] = dic[Tag_ST_Latitude]
                curDic[Tag_ST_Longitude] = dic[Tag_ST_Longitude]
                curDic[Tag_ST_LocalX] = dic[Tag_ST_LocalX]
                curDic[Tag_ST_LocalY] = dic[Tag_ST_LocalY]
                curDic[Tag_ST_UNIT] = dic[Tag_ST_UNIT]
                curDic[Tag_ST_IsOutlet] = dic[Tag_ST_IsOutlet]
                db[Tag_ClimateDB_MeasSites].insert_one(curDic)


def ImportMeasurementData(hostname,port,dbName,path):
    try:
        connMongo = MongoClient(hostname, port)
        print "Connected successfully"
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
    db = connMongo[dbName]
    cList = db.collection_names()

    if not Tag_ClimateDB_Data in cList:
        db.create_collection(Tag_ClimateDB_Data)

    fileList = os.listdir(path)
    measFileList = []
    siteLoc = []
    for fl in fileList:
        if(IsSubString('observed_',fl)):
            measFileList.append(fl)
        else:
            siteLoc.append(fl)
    ImportData(db, measFileList, siteLoc)
# if __name__ == "__main__":
#     ImportMeasurementData(HOSTNAME,PORT,ClimateDBName,MEASUREMENT_DATA_DIR)