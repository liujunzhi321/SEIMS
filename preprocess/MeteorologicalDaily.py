#! /usr/bin/env python
#coding=utf-8
## @Meteorological daily data import
#
#
from pymongo import MongoClient
from pymongo.errors import ConnectionFailure
from pymongo.collection import Collection
import bson
import datetime, time
from util import *
import math, sys

def ImportDayData(db, siteFile,VariablesFile, fileName):
    cList = db.collection_names()
    sitesLoc = {}
    f = open(siteFile)
    
    for line in f.readlines():
        items = line.split()
        #print items
        dic = {}
        dic['ID'] = int(items[1])
        dic['Name'] = unicode(items[2], 'gb2312')
        dic['LocalX'] = float(items[3])
        dic['LocalY'] = float(items[4])
        dic['LocalProjectionID'] = '0'
        dic['Long'] = float(items[6])
        dic['Lat'] = float(items[7])
        print dic['Long'], dic['Lat']
        dic['LatLongDatumID'] = '0'#u''
        dic['Elevation'] = float(items[9])
        dic['Type'] = 'M'
    
        db.Sites.insert(dic)
        
        sitesLoc[dic['ID']] = [dic['Long'], dic['Lat']]
        
    f.close()
    
    f = open(VariablesFile)
    if u'Variables' in cList:
        db.drop_collection(u'Variables')
    
    varTypeDic = {}
    for line in f.readlines():
        items = line.split()
        dic = {}
        id = int(items[1])
        dic['ID'] = id
        dic['Type'] = unicode(items[2], 'gb2312')
        dic['Units'] = unicode(items[3], 'gb2312')
        dic['IsRegular'] = unicode(items[4], 'gb2312')
        dic['TimeSupport'] = float(items[5])
        varTypeDic[id] = dic['Type']
    
        db.Variables.insert(dic)
    
    f.close()
    
    #for yy in range(1951,2013):
    #    tname = 'DataValuesDay_%d' % (yy,)
    #    if tname in cList:
    #        db.drop_collection(tname)
        
    f = open(fileName)
    lines = f.readlines()
    nLines = len(lines)
    #print lines[1]
    #print lines[1].split()
    item1 = lines[1].split()
    presiteID = int(item1[0])
    preyear = int(item1[1])
    #print presiteID
    #print preyear
    tbname = ''
    
    #propertyList = ['T', 'TMAX', 'TMIN', 'RM', 'WS', 'SR', 'P']
    #indexList = [7, 8, 9, 10, 13, 18, 12]
    #defaultValue = {'T':10.0, 'TMAX':20, 'TMIN':0, 'RM':0.5, 'WS':3.0, 'SR':15.0, 'P':0.0}
    
    propertyList = ['T', 'TMAX', 'TMIN', 'RM', 'WS', 'SR', 'PRS', 'EVP']
    indexList = [4, 5, 6, 7, 8, 9, 10, 11]
    defaultValue = {'T':10.0, 'TMAX':20, 'TMIN':0, 'RM':0.5, 'WS':3.0, 'SR':15.0, 'PRS':0.0, 'EVP': 0.0}
    
    for i in range(1,nLines):
        items = lines[i].split()
        siteID = int(items[0])
        year = int(items[1])
        if year < 2000:
            continue
        #tableName = 'DataValuesDay_%d' % (year,)
        tableName = u'DataValues'
        #if siteID == presiteID:
            #if year != preyear:
    
        if not tableName in cList:
            tbname = db.create_collection(tableName)
            cList = db.collection_names()
        else:
            tbname = Collection(db,tableName)
            
        for j in range(len(propertyList)):
            property = propertyList[j]
            index = indexList[j]
        
            dic = {}
            dic['StationID'] = siteID
            dic['Type'] = property
            dt = datetime.datetime(int(items[1]),int(items[2]),int(items[3]),0,0)
            sec = time.mktime(dt.timetuple())
            utcTime = time.gmtime(sec)
            dic['LocalDateTime'] = dt
            dic['UTCOffset'] = 8
            dic['UTCDateTime'] = datetime.datetime(utcTime[0], utcTime[1], utcTime[2], utcTime[3])
            if property == 'SR':
                long, lat = sitesLoc[siteID]
                dic['Value'] = Rs(doy(dt), float(items[index]), lat*math.pi/180.0)
            elif property == 'RM':
                dic['Value'] = float(items[index])*0.01
            else:
                dic['Value'] = float(items[index])

            
#            if property == 'RM':
#                dic['Value'] = float(items[index])*0.01
#            elif property == 'SR':
#                long, lat = sitesLoc[siteID]
#                dic['Value'] = SolarRadiation.Rs(doy(dt), 0.1*float(items[index]), lat*math.pi/180.0)
#            else:
#                dic['Value'] = float(items[index])*0.1
                
            if items[index] == '32766' or items[index] == '-99999':
                print property, dt, defaultValue[property]
                dic['Value'] = defaultValue[property]
#            else:
#                defaultValue[property] = dic['Value']
            
#            if property == 'PRS' and (dic['Value'] > 200 or dic['Value'] < 0):
#                dic['Value'] = 0
                        
            tbname.insert(dic)
    f.close()

def ImportDailyMeteoData(hostname,port,dbName,VariablesFile,meteofile,sitefile):
    try:
        connMongo = MongoClient(hostname, port)
        print "Connected successfully"
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
    db = connMongo[dbName]
    cList = db.collection_names()
    if not 'Sites' in cList:
        db.create_collection('Sites')
    if not 'DataValues' in cList:
        db.create_collection('DataValues')
    ImportDayData(db, sitefile, VariablesFile,meteofile)
    connMongo.close()