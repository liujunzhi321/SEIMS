#! /usr/bin/env python
#coding=utf-8
## @Precipitation daily
#
#
import pymongo
from pymongo import MongoClient
from pymongo.errors import ConnectionFailure
from pymongo.collection import Collection
import bson
import datetime, time
import xlrd
from osgeo import ogr
import sys, os, math
from util import *
def ImportSites(db, shpFileName, siteType):
    # import sites
    shp = ogr.Open(shpFileName)
    lyr = shp.GetLayer()
    layerDef = lyr.GetLayerDefn()
    iName = layerDef.GetFieldIndex("Name_en")
    iLat = layerDef.GetFieldIndex("Lat")
    iLong = layerDef.GetFieldIndex("Lng")
    iLocalX = layerDef.GetFieldIndex("LocalX")
    iLocalY = layerDef.GetFieldIndex("LocalY")    
    iID = layerDef.GetFieldIndex("ID")
    iEle = layerDef.GetFieldIndex("Elevation")
    sitesDic = {}
    for i in range(0, lyr.GetFeatureCount()):
        feat = lyr.GetFeature(i)
        geom = feat.GetGeometryRef()
        dic = {}
        dic['ID'] = feat.GetFieldAsInteger(iID)
        dic['Name'] = feat.GetFieldAsString(iName)
        dic['LocalX'] = feat.GetFieldAsDouble(iLocalX)
        dic['LocalY'] = feat.GetFieldAsDouble(iLocalY)
        dic['LocalProjectionID'] = '0'#u''
        dic['Lat'] = feat.GetFieldAsDouble(iLat)
        dic['Long'] = feat.GetFieldAsDouble(iLong)
        dic['LatLongDatumID'] = '0'#u''
        dic['Elevation'] = feat.GetFieldAsDouble(iEle)
        dic['Type'] = siteType
        sitesDic[dic['Name']] = [dic['ID'], dic['Long'], dic['Lat']]
        db.Sites.insert(dic)    
    return sitesDic

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

def FindNearestStation(bk, name, siteDic, year, mon):
    x0 = siteDic[name][1]
    y0 = siteDic[name][2]
    minDis = 100000
    minName = ''
    for siteName in siteDic:
        if siteName == name:
            continue
        id = siteDic[siteName][0]
        x = siteDic[siteName][1]
        y = siteDic[siteName][2]
        
        dis = math.sqrt(pow(x-x0,2) + pow(y-y0,2))
        if dis < minDis:
            #check whether there is data in this site
            sh = bk.sheet_by_name(siteName)
            if not IsEmptyMonth(sh, year, mon):
                minDis = dis
                minName = siteName
    return minName
        

def ImportPrecipitation(db, siteDic, xlsFileName, year): 
    # import measurement
    #tableName = u'DataValuesDay_%d' % (year,)
    tableName = u'DataValues'
    preciTable = Collection(db, tableName)
    print xlsFileName
    bk = xlrd.open_workbook(xlsFileName)
    #print len(bk.sheet_names())
    for name in bk.sheet_names():
        curSh = bk.sheet_by_name(name)
        # the sum result is stored in row of index 32
        for j in range(1, 13):
            sh = curSh
            nDays = GetDayNumber(year, j)

            if IsEmptyMonth(sh, year, j):
                # find the nearest station
                nearestName = FindNearestStation(bk, name, siteDic, year, j)
                sh = bk.sheet_by_name(nearestName)
                print year, j, name, '->', nearestName 
                         
            for i in range(1, nDays+1):
                s = str(sh.cell_value(i, j))
                pValue = 0
                if(len(s) > 0):
                    if s.upper() == 'U':
                        pass
                    else:
                        if s[-1].upper() == 'A' or s[-1].upper() == 'U':
                            s = s[:-1]
                        if len(s) > 0:
                            item = s.split('*')[0].strip()
                            if len(item) > 0:
                                pValue = float(item)
                            
                dic = {}
                dic['StationID'] = siteDic.get(name, [0,])[0]
                dic['Type'] = 'P'
                dt = datetime.datetime(year, j, i, 0, 0)
                sec = time.mktime(dt.timetuple())
                utcTime = time.gmtime(sec)                
                dic['LocalDateTime'] = dt
                dic['UTCOffset'] = 8
                dic['UTCDateTime'] = datetime.datetime(utcTime[0], utcTime[1], utcTime[2], utcTime[3])
                dic['Value'] = pValue
                preciTable.insert(dic)

def ImportET(db, siteDic, xlsFileName, year): 
    # import measurement
    #tableName = u'DataValuesDay_%d' % (year,)
    tableName = u'DataValues'
    preciTable = Collection(db, tableName)
    print xlsFileName
    bk = xlrd.open_workbook(xlsFileName)
    #print len(bk.sheet_names())
    for name in bk.sheet_names():
        if not name in siteDic.keys():
            continue
        curSh = bk.sheet_by_name(name)
        # the sum result is stored in row of index 32
        for j in range(1, 13):
            sh = curSh
            nDays = GetDayNumber(year, j)
                       
            for i in range(1, nDays+1):
                s = str(sh.cell_value(i, j))
                pValue = 0
                if(len(s) > 0):
                    if s[-1].upper() == '+':
                        s = s[:-1]
                    pValue = float(s)                      
                            
                dic = {}
                dic['StationID'] = siteDic.get(name, [0,])[0]
                dic['Type'] = 'PET'
                dt = datetime.datetime(year, j, i, 0, 0)
                sec = time.mktime(dt.timetuple())
                utcTime = time.gmtime(sec)                
                dic['LocalDateTime'] = dt
                dic['UTCOffset'] = 8
                dic['UTCDateTime'] = datetime.datetime(utcTime[0], utcTime[1], utcTime[2], utcTime[3])
                dic['Value'] = pValue
                
                preciTable.insert(dic)
def ImportDailyPrecData(hostname,port,dbName,precSitesVor,precExcelPrefix,precYear):
    try:
        connMongo = MongoClient(hostname, port)
        print "Connected successfully"
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
    db = connMongo[dbName]
    cList = db.collection_names()
    sitesDic = ImportSites(db, precSitesVor, "P")

    for year in precYear:
        xlsFileName = r'%s%d.xls' % (precExcelPrefix, year)
        ImportPrecipitation(db, sitesDic, xlsFileName, year)
     
    db.DataValues.create_index('StationID')
    db.DataValues.create_index('Type')
    db.DataValues.create_index('UTCDateTime')
    db.DataValues.create_index([('StationID', pymongo.ASCENDING), ('Type', pymongo.ASCENDING), ('UTCDateTime', pymongo.ASCENDING)])
    
    db.Sites.create_index('Type')
    db.Sites.create_index('ID')
    db.Sites.create_index([('ID', pymongo.ASCENDING), ('Type', pymongo.ASCENDING)])    
    connMongo.close()