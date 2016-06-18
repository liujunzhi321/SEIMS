#! /usr/bin/env python
#coding=utf-8
## @Precipitation daily
#
#
import pymongo
from pymongo import MongoClient
from pymongo.errors import ConnectionFailure
from pymongo.collection import Collection
import xlrd
from osgeo import ogr
from util import *
from text import *
# Comment: Currently, there is no need to load from Shapefile
# def ImportSites(db, shpFileName, siteType):
#     # import sites
#     shp = ogr.Open(shpFileName)
#     lyr = shp.GetLayer()
#     layerDef = lyr.GetLayerDefn()
#     ### TODO: NEED to ENHANCE! LJ
#     iName = layerDef.GetFieldIndex(Tag_ST_StationName)
#     iLat = layerDef.GetFieldIndex(Tag_ST_Latitude)
#     iLong = layerDef.GetFieldIndex(Tag_ST_Longitude)
#     iLocalX = layerDef.GetFieldIndex(Tag_ST_LocalX)
#     iLocalY = layerDef.GetFieldIndex(Tag_ST_LocalY)
#     iID = layerDef.GetFieldIndex(Tag_ST_StationID)
#     iEle = layerDef.GetFieldIndex(Tag_ST_Elevation)
#     sitesDic = {}
#     for i in range(0, lyr.GetFeatureCount()):
#         feat = lyr.GetFeature(i)
#         geom = feat.GetGeometryRef()
#         dic = {}
#         dic[Tag_ST_StationID] = int(feat.GetFieldAsInteger(iID))
#         dic[Tag_ST_StationName] = feat.GetFieldAsString(iName)
#         dic[Tag_ST_LocalX] = feat.GetFieldAsDouble(iLocalX)
#         dic[Tag_ST_LocalY] = feat.GetFieldAsDouble(iLocalY)
#         dic[Tag_ST_LocalPrjID] = '0'  ### how to get this variable? LJ
#         dic[Tag_ST_Latitude] = feat.GetFieldAsDouble(iLat)
#         dic[Tag_ST_Longitude] = feat.GetFieldAsDouble(iLong)
#         dic[Tag_ST_DatumID] = '0'  ### how to get this variable? LJ
#         dic[Tag_ST_Elevation] = feat.GetFieldAsDouble(iEle)
#         dic[Tag_ST_Type] = siteType
#         sitesDic[dic[Tag_ST_StationName]] = [dic[Tag_ST_StationID], dic[Tag_ST_Longitude], dic[Tag_ST_Latitude]]
#         db[Tag_ClimateDB_Sites].insert_one(dic)
#     print sitesDic
#     return sitesDic

def IsEmptyMonth(sh, year, mon):
    nDays = GetDayNumber(year, mon)
    isEmptyMonth = True
    for i in range(1, nDays+1):
        s = str(sh.cell_value(i, mon))
        if(len(s) > 0):
            if s.upper() == 'U':
                pass
            else:
                if s[-1].upper() == 'A' or s[-1].upper() == 'U':
                    s = s[:-1]
                if len(s.strip()) > 0:
                    isEmptyMonth = False
                    break
    return isEmptyMonth

def FindNearestStation(bk, siteID, siteDic, year, mon):
    x0,y0 = siteDic[siteID].LocalXY()
    name = siteDic[siteID].Name
    #x0 = siteDic[name][1]
    #y0 = siteDic[name][2]
    minDis = 100000
    minName = ''
    for curID in siteDic.keys():
        if curID == siteID:
            continue
        x,y = siteDic[curID].LocalXY()
        siteName = siteDic[curID].Name
        dis = math.sqrt(pow(x-x0,2) + pow(y-y0,2))
        if dis < minDis:
            #check whether there is data in this site
            sh = bk.sheet_by_name(siteName)
            if not IsEmptyMonth(sh, year, mon):
                minDis = dis
                minName = siteName
    return minName

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
        dic[Tag_DT_Zone] = time.timezone / 3600
        dic[Tag_DT_UTC] = datetime.datetime(utcTime[0], utcTime[1], utcTime[2], utcTime[3])

        for j in range(len(StationID)):
            curDic = {}
            curDic[Tag_DT_Value] = precipitation[j]
            curDic[Tag_DT_StationID] = StationID[j]
            curDic[Tag_DT_Type] = DataType_Precipitation
            curDic[Tag_DT_Zone] = dic[Tag_DT_Zone]
            curDic[Tag_DT_LocalT] = dic[Tag_DT_LocalT]
            curDic[Tag_DT_UTC] = dic[Tag_DT_UTC]
            db[Tag_ClimateDB_Data].insert_one(curDic)


# def ImportPrecipitation(db, siteDic, xlsFileName, year):
#     # import measurement
#     #tableName = u'DataValuesDay_%d' % (year,)
#     tableName = unicode(Tag_ClimateDB_Data)
#     preciTable = Collection(db, tableName)
#     print xlsFileName
#     bk = xlrd.open_workbook(xlsFileName)
#     #print len(bk.sheet_names())
#     for name in bk.sheet_names():
#         curSh = bk.sheet_by_name(name)
#         ## Find stationID by Name
#         siteFound = False
#         for ss in siteDic.keys():
#             if StringMatch(siteDic[ss].Name, name):
#                 curSiteID = ss
#                 siteFound = True
#                 break
#         if(not siteFound):
#             print("Site Name: %s is not found in SiteList"%name)
#             exit(-1)
#         # the sum result is stored in row of index 32
#         for j in range(1, 13):
#             sh = curSh
#             nDays = GetDayNumber(year, j)
#
#             if IsEmptyMonth(sh, year, j):
#                 # find the nearest station
#                 # bug fixed: Distance should be calculated by LocalX and LocalY. By LJ
#                 nearestName = FindNearestStation(bk, curSiteID, siteDic, year, j)
#                 sh = bk.sheet_by_name(nearestName)
#                 print year, j, name, '->', nearestName
#
#             for i in range(1, nDays+1):
#                 s = str(sh.cell_value(i, j))
#                 pValue = 0
#                 if(len(s) > 0):
#                     if s.upper() == 'U':
#                         pass
#                     else:
#                         if s[-1].upper() == 'A' or s[-1].upper() == 'U':
#                             s = s[:-1]
#                         if len(s) > 0:
#                             item = s.split('*')[0].strip()
#                             if len(item) > 0:
#                                 pValue = float(item)
#
#                 dic = {}
#                 dic[Tag_DT_StationID] = curSiteID
#                 #dic[Tag_DT_StationID] = siteDic.get(name, [0, ])[0]
#                 dic[Tag_DT_Type] = DataType_Precipitation
#                 dt = datetime.datetime(year, j, i, 0, 0)
#                 sec = time.mktime(dt.timetuple())
#                 utcTime = time.gmtime(sec)
#                 dic[Tag_DT_LocalT] = dt
#                 dic[Tag_DT_Zone] = time.timezone / 3600
#                 dic[Tag_DT_UTC] = datetime.datetime(utcTime[0], utcTime[1], utcTime[2], utcTime[3])
#                 dic[Tag_DT_Value] = pValue
#                 preciTable.insert_one(dic)
### TODO ImportET Function should be moved to MeteorologicalDaily.py
# def ImportET(db, siteDic, xlsFileName, year):
#     # import measurement
#     #tableName = u'DataValuesDay_%d' % (year,)
#     tableName = unicode(Tag_ClimateDB_Data)
#     preciTable = Collection(db, tableName)
#     print xlsFileName
#     bk = xlrd.open_workbook(xlsFileName)
#     #print len(bk.sheet_names())
#     for name in bk.sheet_names():
#         if not name in siteDic.keys():
#             continue
#         curSh = bk.sheet_by_name(name)
#         # the sum result is stored in row of index 32
#         for j in range(1, 13):
#             sh = curSh
#             nDays = GetDayNumber(year, j)
#             for i in range(1, nDays+1):
#                 s = str(sh.cell_value(i, j))
#                 pValue = 0
#                 if(len(s) > 0):
#                     if s[-1].upper() == '+':
#                         s = s[:-1]
#                     pValue = float(s)
#
#                 dic = {}
#                 dic[Tag_DT_StationID] = int(siteDic.get(name, [0,])[0])
#                 dic[Tag_DT_Type] = DataType_PotentialEvapotranspiration
#                 dt = datetime.datetime(year, j, i, 0, 0)
#                 sec = time.mktime(dt.timetuple())
#                 utcTime = time.gmtime(sec)
#                 dic[Tag_DT_LocalT] = dt
#                 dic[Tag_DT_Zone] = time.timezone / 3600
#                 dic[Tag_DT_UTC] = datetime.datetime(utcTime[0], utcTime[1], utcTime[2], utcTime[3])
#                 dic[Tag_DT_Value] = pValue
#                 preciTable.insert_one(dic)
def ImportDailyPrecData(hostname,port,dbName,precdata,sitePLoc):
    try:
        connMongo = MongoClient(hostname, port)
        #print "Connected successfully"
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
    db = connMongo[dbName]
    cList = db.collection_names()
    if not Tag_ClimateDB_Data in cList:
        db.create_collection(Tag_ClimateDB_Data)

    ImportPrecipitation(db,precdata,sitePLoc)
    connMongo.close()
    #sitesDic = ImportSites(db, precSitesVor, DataType_Precipitation)

    # for year in precYear:
    #     xlsFileName = r'%s%d.xls' % (precExcelPrefix, year)
    #     ImportPrecipitation(db, sitesDic, xlsFileName, year)
    #
    # db[Tag_ClimateDB_Data].create_index(Tag_DT_StationID)
    # db[Tag_ClimateDB_Data].create_index(Tag_DT_Type)
    # db[Tag_ClimateDB_Data].create_index(Tag_DT_UTC)
    # db[Tag_ClimateDB_Data].create_index([(Tag_DT_StationID, pymongo.ASCENDING), (Tag_DT_Type, pymongo.ASCENDING), (Tag_DT_UTC, pymongo.ASCENDING)])
    #
    # db[Tag_ClimateDB_Sites].create_index(Tag_ST_Type)
    # db[Tag_ClimateDB_Sites].create_index(Tag_DT_StationID)
    # db[Tag_ClimateDB_Sites].create_index([(Tag_DT_StationID, pymongo.ASCENDING), (Tag_ST_Type, pymongo.ASCENDING)])
    # connMongo.close()