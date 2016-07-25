#! /usr/bin/env python
# coding=utf-8
# Find meteorology and precipitation sites in study area
# Author: Junzhi Liu
# Revised: Liang-Jun Zhu
#
import pymongo
from pymongo import MongoClient
from pymongo.errors import ConnectionFailure
from shapely.wkt import loads
from osgeo import ogr
from config import *


def OGRWkt2Shapely(input_shape, idField):
    shapely_objects = []
    idList = []
    # print input_shape
    shp = ogr.Open(input_shape)
    lyr = shp.GetLayer()
    for n in range(0, lyr.GetFeatureCount()):
        feat = lyr.GetFeature(n)
        wkt_feat = loads(feat.geometry().ExportToWkt())
        shapely_objects.append(wkt_feat)
        idIndex = feat.GetFieldIndex(idField)
        idList.append(feat.GetField(idIndex))
    return shapely_objects, idList


def FindSites(db, hydroDBName, subbasinFile, subbasinIdField, thiessenFileList, siteTypeList, mode):
    # subbasinIdField = FLD_LINKNO
    # print subbasinFile
    # try:
    #     subbasinList, subbasinIDList = OGRWkt2Shapely(subbasinFile, subbasinIdField)
    # except:
    #     subbasinList, subbasinIDList = OGRWkt2Shapely(subbasinFile, 'Subbasin')
    subbasinList, subbasinIDList = OGRWkt2Shapely(subbasinFile, subbasinIdField)
    nSubbasins = len(subbasinList)

    n = len(thiessenFileList)
    siteDic = {}
    for i in range(len(subbasinList)):
        subbasin = subbasinList[i]
        id = subbasinIDList[i]
        dic = {}
        dic[FLD_SUBBASINID.upper()] = id
        dic[FLD_DB.upper()] = hydroDBName
        dic[FLD_MODE.upper()] = mode
        curFileter = {FLD_SUBBASINID.upper(): id, FLD_DB.upper(): hydroDBName, FLD_MODE.upper(): mode}
        for i in range(0, n):
            thiessenFile = thiessenFileList[i]
            # print thiessenFile
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

            siteField = '%s%s' % (DB_TAB_SITELIST.upper(), siteType)
            dic[siteField] = siteListStr
        db[DB_TAB_SITELIST.upper()].find_one_and_replace(curFileter, dic, upsert = True)

    db[DB_TAB_SITELIST.upper()].create_index([(FLD_SUBBASINID.upper(), pymongo.ASCENDING),
                                              (FLD_MODE.upper(), pymongo.ASCENDING)])
    print 'Meteorology sites table was generated.'
    return nSubbasins


# TEST CODE
if __name__ == "__main__":
    hostname = '127.0.0.1'
    port = 27017
    try:
        conn = MongoClient(host = hostname, port = 27017)
        print "Connected successfully"
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)

    thiessenFileList = [MeteorSitesThiessen, PrecSitesThiessen]
    basinFile = WORKING_DIR + os.sep + 'basin.shp'
    typeList = ['M', 'P']
    db = conn[SpatialDBName]
    FindSites(db, ClimateDBName, basinFile, FLD_BASINID, thiessenFileList, typeList, 'DAILY')
