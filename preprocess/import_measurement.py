#! /usr/bin/env python
#coding=utf-8
## @Import measurement data, such as discharge, sediment yield.
## @Author: Fang Shen
## @Revised: Liang-Jun Zhu
#
#
from pymongo import MongoClient
from pymongo.errors import ConnectionFailure
from util import *
import math
from text import *

def ImportData(db, measFileList, sitesLoc):
    for measDataFile in measFileList:
        #print measDataFile
        measDataItems = ReadDataItemsFromTxt(measDataFile)
        ## If the dataitems is EMPTY or only have one header row, then goto next data file.
        if measDataItems == [] or len(measDataItems) == 1:
            continue
        measFlds = measDataItems[0]
        requiredFlds = [Tag_ST_StationID, Tag_DT_Year, Tag_DT_Month, Tag_DT_Day,
                        Tag_DT_Type, Tag_DT_Value]
        for fld in requiredFlds:
            if not StringInList(fld, measFlds):### data can not meet the request!
                raise ValueError("The %s cann't meet the required format!" % measDataFile)
        for i in range(1,len(measDataItems)):
            dic = {}
            for j in range(len(measDataItems[i])):
                if StringMatch(measFlds[j], Tag_ST_StationID):
                    dic[Tag_ST_StationID] = int(measDataItems[i][j])
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
            curfilter = {Tag_ST_StationID: dic[Tag_ST_StationID],
                         Tag_DT_Type: dic[Tag_DT_Type], Tag_DT_UTC: dic[Tag_DT_UTC]}
            db[Tag_ClimateDB_Measurement].find_one_and_replace(curfilter, dic, upsert=True)
            # db[Tag_ClimateDB_Measurement].insert_one(dic)
    for siteFile in sitesLoc:
        siteDataItems = ReadDataItemsFromTxt(siteFile)
        siteFlds = siteDataItems[0]
        for i in range(1, len(siteDataItems)):
            dic = {}
            for j in range(len(siteDataItems[i])):
                if StringMatch(siteFlds[j], Tag_ST_StationID):
                    dic[Tag_ST_StationID] = int(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_StationName):
                    dic[Tag_ST_StationName] = StripStr(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_Type):
                    type = SplitStr(StripStr(siteDataItems[i][j]),',')
                elif StringMatch(siteFlds[j], Tag_ST_Latitude):
                    dic[Tag_ST_Latitude] = float(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_Longitude):
                    dic[Tag_ST_Longitude] = float(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_LocalX):
                    dic[Tag_ST_LocalX] = float(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_LocalY):
                    dic[Tag_ST_LocalY] = float(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_UNIT):
                    dic[Tag_ST_UNIT] = StripStr(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_Elevation):
                    dic[Tag_ST_Elevation] = float(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_IsOutlet):
                    dic[Tag_ST_IsOutlet] = float(siteDataItems[i][j])

            for j in range(len(type)):
                siteDic = {}
                siteDic[Tag_ST_StationID] = dic[Tag_ST_StationID]
                siteDic[Tag_ST_StationName] = dic[Tag_ST_StationName]
                siteDic[Tag_ST_Type] = type[j]
                siteDic[Tag_ST_Latitude] = dic[Tag_ST_Latitude]
                siteDic[Tag_ST_Longitude] = dic[Tag_ST_Longitude]
                siteDic[Tag_ST_LocalX] = dic[Tag_ST_LocalX]
                siteDic[Tag_ST_LocalY] = dic[Tag_ST_LocalY]
                siteDic[Tag_ST_Elevation] = dic[Tag_ST_Elevation]
                siteDic[Tag_ST_IsOutlet] = dic[Tag_ST_IsOutlet]
                curfilter = {Tag_ST_StationID:siteDic[Tag_ST_StationID],
                             Tag_ST_Type:siteDic[Tag_ST_Type]}
                db[Tag_ClimateDB_Sites].find_one_and_replace(curfilter, siteDic, upsert=True)
                # results = db[Tag_ClimateDB_Sites].find({Tag_ST_StationID:siteDic[Tag_ST_StationID],
                #                                         Tag_ST_Type:siteDic[Tag_ST_Type]})
                # records = [data for data in results]
                # if records == []:
                #     db[Tag_ClimateDB_Sites].insert_one(siteDic)
                varDic = {}
                varDic[Tag_ST_Type] = type[j]
                varDic[Tag_ST_UNIT] = dic[Tag_ST_UNIT]
                curfilter = {Tag_ST_Type:varDic[Tag_ST_Type]}
                db[Tag_ClimateDB_VARs].find_one_and_replace(curfilter, varDic, upsert=True)
                # results = db[Tag_ClimateDB_VARs].find({Tag_ST_Type:varDic[Tag_ST_Type]},{Tag_ST_Type:1})
                # records = [data for data in results]
                # if records == []:
                #     db[Tag_ClimateDB_VARs].insert_one(varDic)

def ImportMeasurementData(hostname,port,dbName,path):
    '''
    This function mainly to import measurement data to mongoDB
    data type may include Q (discharge, m3/s), totalN, totalP, etc.
    :param hostname: host IP address of MongoDB
    :param port: port number of MongoDB
    :param dbName: HydroClimate database name
    :param path: path of measurement data files
    '''
    try:
        connMongo = MongoClient(hostname, port)
        print "Import Site Measurement Data... "
        #print "Connected successfully"
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
    db = connMongo[dbName]
    cList = db.collection_names()
    if not StringInList(Tag_ClimateDB_Measurement, cList):
        db.create_collection(Tag_ClimateDB_Measurement)
    # else:
    #     db.drop_collection(Tag_ClimateDB_Measurement)

    if not StringInList(Tag_ClimateDB_Data, cList):
        db.create_collection(Tag_ClimateDB_Data)

    fileList = GetFullPathWithSuffixes(path,['.txt'])
    measFileList = []
    siteLoc = []
    for fl in fileList:
        if(IsSubString('observed_',fl)):
            measFileList.append(fl)
        else:
            siteLoc.append(fl)
    ImportData(db, measFileList, siteLoc)
if __name__ == "__main__":
    ImportMeasurementData(HOSTNAME, PORT, ClimateDBName, MEASUREMENT_DATA_DIR)
