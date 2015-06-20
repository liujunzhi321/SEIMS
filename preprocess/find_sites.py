#! /usr/bin/env python
#coding=utf-8
import pymongo
from pymongo import Connection
from pymongo.errors import ConnectionFailure
import bson, sys
from shapely.geometry import Point
from shapely.geometry import Point, MultiPolygon
from shapely.wkt import dumps, loads
from osgeo import ogr
from config import *

def OGRWkt2Shapely(input_shape, idField):
    shapely_objects=[]
    idList = []
    print input_shape
    shp = ogr.Open(input_shape)
    lyr = shp.GetLayer()
    for n in range(0, lyr.GetFeatureCount()):
        feat = lyr.GetFeature(n)
        wkt_feat = loads(feat.geometry().ExportToWkt())
        shapely_objects.append(wkt_feat)
        idIndex = feat.GetFieldIndex(idField)
        idList.append(feat.GetField(idIndex))
    return shapely_objects, idList

def FindSites(dbModel, hydroDBName, subbasinFile, thiessenFileList, siteTypeList, mode):
    subbasinIdField = 'LINKNO'##'DN'
    #print subbasinFile
    try:
        subbasinList, subbasinIDList = OGRWkt2Shapely(subbasinFile, subbasinIdField)
    except:
        subbasinList, subbasinIDList = OGRWkt2Shapely(subbasinFile, 'Subbasin')
    nSubbasins = len(subbasinList)
    
    thiessenIdField = 'ID'
    
    n = len(thiessenFileList)
    siteDic = {}
    for i in range(len(subbasinList)):
        subbasin = subbasinList[i]
        id = subbasinIDList[i]
        dic = {}
        dic['SubbasinID'] = id
        dic['DB'] =  hydroDBName
        dic['Mode'] = mode
        for i in range(0, n):
            thiessenFile = thiessenFileList[i]
            #print thiessenFile
            siteType = siteTypeList[i]
            
            try:
                thiessenList, thiessenIDList = OGRWkt2Shapely(thiessenFile, thiessenIdField)
            except:
                thiessenList, thiessenIDList = OGRWkt2Shapely(thiessenFile, 'Subbasin')


            siteList = []
            for j in range(len(thiessenList)):
                thiessen = thiessenList[j]
                if subbasin.intersects(thiessen):
                    siteList.append(thiessenIDList[j])
            siteList.sort()
            slist = [str(item) for item in siteList]
            siteListStr = ','.join(slist)
            
            siteField = 'SiteList%s' % (siteType,)
            dic[siteField] = siteListStr
        
        dbModel.SiteList.insert(dic)      
    dbModel.SiteList.create_index([('SubbasinID', pymongo.ASCENDING), ('Mode', pymongo.ASCENDING)])        
    print 'meteorology sites table was generated.'
    return nSubbasins

if __name__ == "__main__":
    subbasinFile = r'F:\data\guanting\model_zjk_90m\basin.shp'
    thiessenFileMeteo = r'F:\data\guanting\meteorology_sites_thiessen.shp'
    thiessenFilePreci = r'F:\data\guanting\stations_need_thiessen.shp'
    
    hostname = '192.168.6.55'
    port = 27017
    try:
        conn = Connection(host=hostname, port=27017)
        print "Connected successfully"
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
    
    thiessenFileList = [thiessenFileMeteo, thiessenFilePreci]
    typeList = ['M', 'P']
    model_name = 'model_zjk_90m'
    FindSites(conn, model_name, 'qingshuihe', subbasinFile, thiessenFileList, typeList, 'DAILY')    
    
    print 'done!'
