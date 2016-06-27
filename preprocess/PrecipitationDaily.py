#! /usr/bin/env python
#coding=utf-8
## @Precipitation daily
#
#
import pymongo
from pymongo import MongoClient
from pymongo.errors import ConnectionFailure
from util import *
from text import *

def ImportPrecipitation(db,ClimateDateFile,sitesLoc):
    climDataItems = ReadDataItemsFromTxt(ClimateDateFile)
    climFlds = climDataItems[0]
    StationID = []
    for i in range(3,len(climFlds)):
         StationID.append(climFlds[i])
    for i in range(1,len(climDataItems)):
        dic = {}
        precipitation = []
        for j in range(len(climDataItems[i])):
            if StringMatch(climFlds[j], Tag_DT_Year):
                curY = int(climDataItems[i][j])
            elif StringMatch(climFlds[j], Tag_DT_Month):
                curM = int(climDataItems[i][j])
            elif StringMatch(climFlds[j], Tag_DT_Day):
                curD = int(climDataItems[i][j])
            else:
                for k in range(len(StationID)):
                    if StringMatch(climFlds[j],StationID[k]):
                        precipitation.append(float(climDataItems[i][j]))

        dt = datetime.datetime(curY, curM, curD, 0, 0)
        sec = time.mktime(dt.timetuple())
        utcTime = time.gmtime(sec)
        dic[Tag_DT_LocalT] = dt
        dic[Tag_DT_Zone] = time.timezone / 3600.
        dic[Tag_DT_UTC] = datetime.datetime(utcTime[0], utcTime[1], utcTime[2], utcTime[3])

        for j in range(len(StationID)):
            curDic = {}
            curDic[Tag_DT_Value] = precipitation[j]
            curDic[Tag_DT_StationID] = int(StationID[j])
            curDic[Tag_DT_Type] = DataType_Precipitation
            curDic[Tag_DT_Zone] = dic[Tag_DT_Zone]
            curDic[Tag_DT_LocalT] = dic[Tag_DT_LocalT]
            curDic[Tag_DT_UTC] = dic[Tag_DT_UTC]
            curfilter = {Tag_DT_StationID: curDic[Tag_DT_StationID],
                         Tag_DT_Type: curDic[Tag_DT_Type],
                         Tag_DT_UTC: curDic[Tag_DT_UTC]}
            db[Tag_ClimateDB_Data].find_one_and_replace(curfilter, curDic, upsert=True)
            # db[Tag_ClimateDB_Data].insert_one(curDic)
    ## Create index
    db[Tag_ClimateDB_Data].create_index([(Tag_DT_StationID, pymongo.ASCENDING),
                                         (Tag_DT_Type, pymongo.ASCENDING),
                                         (Tag_DT_UTC, pymongo.ASCENDING)])

def ImportDailyPrecData(hostname,port,dbName,precdata,sitePLoc):
    try:
        connMongo = MongoClient(hostname, port)
        print "Import Daily Precipitation Data... "
        #print "Connected successfully"
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
    db = connMongo[dbName]
    cList = db.collection_names()
    if not StringInList(Tag_ClimateDB_Data, cList):
        db.create_collection(Tag_ClimateDB_Data)

    ImportPrecipitation(db, precdata, sitePLoc)
    connMongo.close()

if __name__ == "__main__":
    from hydroclimate_sites import ImportHydroClimateSitesInfo
    SitesMList, SitesPList = ImportHydroClimateSitesInfo(HOSTNAME, PORT, ClimateDBName,HydroClimateVarFile, MetroSiteFile, PrecSiteFile)
    ImportDailyPrecData(HOSTNAME, PORT, ClimateDBName, PrecDailyFile, SitesPList)
