#! /usr/bin/env python
#coding=utf-8
## @Import hydroClimate sites information and variables
#
#
from pymongo import MongoClient
from pymongo.errors import ConnectionFailure
from util import *
import math
from text import *

class SiteInfo:
    '''
    base class of HydroClimate site information
    :method: init(ID, Name, lat, lon, LocalX, LocalY, alti)
    '''
    def __init__(self, ID = 0, Name = '', lat=DEFAULT_NODATA, lon=DEFAULT_NODATA, LocalX=DEFAULT_NODATA, \
                 LocalY=DEFAULT_NODATA,alti=DEFAULT_NODATA):
        self.StationID = ID      ## integer
        self.Name      = Name    ## station name, string
        self.lat       = lat     ## latitude, float degree
        self.lon       = lon     ## longitude, float degree
        self.LocalX    = LocalX  ## X coordinate in projection, float
        self.LocalY    = LocalY  ## Y coordinate in projection, float
        self.alti      = alti    ## altitude, as ORIGIN: unit 0.1 meter
    def LonLat(self):
        return (self.lon, self.lat)
    def LocalXY(self):
        return (self.LocalX, self.LocalY)
### Import HydroClimate sites table
def ImportSitesTable(db, siteFile, siteType):
    sitesLoc = {}
    siteDataItems = ReadDataItemsFromTxt(siteFile)
    siteFlds = siteDataItems[0]
    for i in range(1,len(siteDataItems)):
        dic = {}
        for j in range(len(siteDataItems[i])):
            if StringMatch(siteFlds[j], Tag_ST_StationID):
                dic[Tag_ST_StationID] = int(siteDataItems[i][j])
            elif StringMatch(siteFlds[j], Tag_ST_StationName):
                dic[Tag_ST_StationName] = siteDataItems[i][j]  ## unicode(siteDataItems[i][j], 'gb2312')
            elif StringMatch(siteFlds[j], Tag_ST_LocalX):
                dic[Tag_ST_LocalX] = float(siteDataItems[i][j])
            elif StringMatch(siteFlds[j], Tag_ST_LocalY):
                dic[Tag_ST_LocalY] = float(siteDataItems[i][j])
            # elif StringMatch(siteFlds[j], Tag_ST_LocalPrjID):
            #     dic[Tag_ST_LocalPrjID] = siteDataItems[i][j]
            elif StringMatch(siteFlds[j], Tag_ST_Longitude):
                dic[Tag_ST_Longitude] = float(siteDataItems[i][j])
            elif StringMatch(siteFlds[j], Tag_ST_Latitude):
                dic[Tag_ST_Latitude] = float(siteDataItems[i][j])
            # elif StringMatch(siteFlds[j], Tag_ST_DatumID):
            #     dic[Tag_ST_DatumID] = siteDataItems[i][j]
            elif StringMatch(siteFlds[j], Tag_ST_Elevation):
                dic[Tag_ST_Elevation] = float(siteDataItems[i][j])
            elif StringMatch(siteFlds[j], Tag_ST_IsOutlet):
                dic[Tag_ST_IsOutlet] = float(siteDataItems[i][j])
        dic[Tag_ST_Type] = siteType
        insertResult = db[Tag_ClimateDB_Sites].insert_one(dic)
        if(not insertResult.acknowledged):
            print "Insert Site information to MongoDB faied!"
            exit(-1)
        if dic[Tag_ST_StationID] not in sitesLoc.keys():
            sitesLoc[dic[Tag_ST_StationID]] = SiteInfo(dic[Tag_ST_StationID], dic[Tag_ST_StationName],
                                                       dic[Tag_ST_Latitude], dic[Tag_ST_Longitude],
                                                       dic[Tag_ST_LocalX], dic[Tag_ST_LocalY],
                                                       dic[Tag_ST_Elevation])
    return sitesLoc
### Import variables table
def ImportVariableTable(db, varFile):
    varDataItems = ReadDataItemsFromTxt(varFile)
    varFlds = varDataItems[0]
    for i in range(1,len(varDataItems)):
        dic = {}
        for j in range(len(varDataItems[i])):
            if StringMatch(varFlds[j], Tag_VAR_Type):
                dic[Tag_VAR_Type] = varDataItems[i][j]  ## unicode(siteDataItems[i][j], 'gb2312')
            # elif StringMatch(varFlds[j], Tag_VAR_ID):
            #     dic[Tag_VAR_ID] = int(varDataItems[i][j])
            elif StringMatch(varFlds[j], Tag_VAR_UNIT):
                dic[Tag_VAR_UNIT] = varDataItems[i][j]
            # elif StringMatch(varFlds[j], Tag_VAR_IsReg):
            #     dic[Tag_VAR_IsReg] = varDataItems[i][j]
            # elif StringMatch(varFlds[j], Tag_VAR_Time):
            #     dic[Tag_VAR_Time] = float(varDataItems[i][j])
        db[Tag_ClimateDB_VARs].insert_one(dic)

def ImportHydroClimateSitesInfo(hostname,port,dbName,varfile, meteofile, precfile):
    try:
        connMongo = MongoClient(hostname, port)
        print "Import Climate Sites Information..."
        #print "Connected successfully"
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
    db = connMongo[dbName]
    cList = db.collection_names()
    tables = [Tag_ClimateDB_Sites, Tag_ClimateDB_VARs]
    for tb in tables:
        if not StringInList(tb, cList):
            db.create_collection(tb)
        else:
            db.drop_collection(tb)
    ImportVariableTable(db, varfile)
    SiteMLoc = ImportSitesTable(db,meteofile,DataType_Meteorology)
    SitePLoc = ImportSitesTable(db,precfile,DataType_Precipitation)
    connMongo.close()
    return (SiteMLoc, SitePLoc)