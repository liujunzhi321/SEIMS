#! /usr/bin/env python
# coding=utf-8
## @Import measurement data, such as discharge, sediment yield.
## @Author: Fang Shen
## @Revised: Liang-Jun Zhu
#
#
from pymongo import MongoClient
from pymongo.errors import ConnectionFailure
from util import *
from config import *


def ImportData(db, measFileList, sitesLoc):
    for measDataFile in measFileList:
        # print measDataFile
        measDataItems = ReadDataItemsFromTxt(measDataFile)
        ## If the dataitems is EMPTY or only have one header row, then goto next data file.
        if measDataItems == [] or len(measDataItems) == 1:
            continue
        measFlds = measDataItems[0]
        requiredFlds = [Tag_ST_StationID.upper(), Tag_DT_Year.upper(), Tag_DT_Month.upper(),
                        Tag_DT_Day.upper(), Tag_DT_Type.upper(), Tag_DT_Value.upper()]
        for fld in requiredFlds:
            if not StringInList(fld, measFlds):  ### data can not meet the request!
                raise ValueError("The %s cann't meet the required format!" % measDataFile)
        for i in range(1, len(measDataItems)):
            dic = {}
            for j in range(len(measDataItems[i])):
                if StringMatch(measFlds[j], Tag_ST_StationID):
                    dic[Tag_ST_StationID.upper()] = int(measDataItems[i][j])
                elif StringMatch(measFlds[j], Tag_DT_Year):
                    curY = int(measDataItems[i][j])
                elif StringMatch(measFlds[j], Tag_DT_Month):
                    curM = int(measDataItems[i][j])
                elif StringMatch(measFlds[j], Tag_DT_Day):
                    curD = int(measDataItems[i][j])
                elif StringMatch(measFlds[j], Tag_DT_Type):
                    dic[Tag_DT_Type.upper()] = measDataItems[i][j]
                elif StringMatch(measFlds[j], Tag_DT_Value):
                    dic[Tag_DT_Value.upper()] = float(measDataItems[i][j])
            dt = datetime.datetime(curY, curM, curD, 0, 0)
            sec = time.mktime(dt.timetuple())
            utcTime = time.gmtime(sec)
            dic[Tag_DT_LocalT.upper()] = dt
            dic[Tag_DT_Zone.upper()] = time.timezone / 3600
            dic[Tag_DT_UTC.upper()] = datetime.datetime(utcTime[0], utcTime[1], utcTime[2], utcTime[3])
            curfilter = {Tag_ST_StationID.upper(): dic[Tag_ST_StationID.upper()],
                         Tag_DT_Type.upper()     : dic[Tag_DT_Type.upper()],
                         Tag_DT_UTC.upper()      : dic[Tag_DT_UTC.upper()]}
            db[Tag_ClimateDB_Measurement.upper()].find_one_and_replace(curfilter, dic, upsert = True)

    for siteFile in sitesLoc:
        siteDataItems = ReadDataItemsFromTxt(siteFile)
        siteFlds = siteDataItems[0]
        for i in range(1, len(siteDataItems)):
            dic = {}
            for j in range(len(siteDataItems[i])):
                if StringMatch(siteFlds[j], Tag_ST_StationID):
                    dic[Tag_ST_StationID.upper()] = int(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_StationName):
                    dic[Tag_ST_StationName.upper()] = StripStr(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_Type):
                    type = SplitStr(StripStr(siteDataItems[i][j]), ',')
                elif StringMatch(siteFlds[j], Tag_ST_Latitude):
                    dic[Tag_ST_Latitude.upper()] = float(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_Longitude):
                    dic[Tag_ST_Longitude.upper()] = float(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_LocalX):
                    dic[Tag_ST_LocalX.upper()] = float(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_LocalY):
                    dic[Tag_ST_LocalY.upper()] = float(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_UNIT):
                    dic[Tag_ST_UNIT.upper()] = StripStr(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_Elevation):
                    dic[Tag_ST_Elevation.upper()] = float(siteDataItems[i][j])
                elif StringMatch(siteFlds[j], Tag_ST_IsOutlet):
                    dic[Tag_ST_IsOutlet.upper()] = float(siteDataItems[i][j])

            for j in range(len(type)):
                siteDic = {}
                siteDic[Tag_ST_StationID.upper()] = dic[Tag_ST_StationID.upper()]
                siteDic[Tag_ST_StationName.upper()] = dic[Tag_ST_StationName.upper()]
                siteDic[Tag_ST_Type.upper()] = type[j]
                siteDic[Tag_ST_Latitude.upper()] = dic[Tag_ST_Latitude.upper()]
                siteDic[Tag_ST_Longitude.upper()] = dic[Tag_ST_Longitude.upper()]
                siteDic[Tag_ST_LocalX.upper()] = dic[Tag_ST_LocalX.upper()]
                siteDic[Tag_ST_LocalY.upper()] = dic[Tag_ST_LocalY.upper()]
                siteDic[Tag_ST_Elevation.upper()] = dic[Tag_ST_Elevation.upper()]
                siteDic[Tag_ST_IsOutlet.upper()] = dic[Tag_ST_IsOutlet.upper()]
                curfilter = {Tag_ST_StationID.upper(): siteDic[Tag_ST_StationID.upper()],
                             Tag_ST_Type.upper()     : siteDic[Tag_ST_Type.upper()]}
                db[Tag_ClimateDB_Sites.upper()].find_one_and_replace(curfilter, siteDic, upsert = True)

                varDic = {}
                varDic[Tag_ST_Type.upper()] = type[j]
                varDic[Tag_ST_UNIT.upper()] = dic[Tag_ST_UNIT.upper()]
                curfilter = {Tag_ST_Type.upper(): varDic[Tag_ST_Type.upper()]}
                db[Tag_ClimateDB_VARs.upper()].find_one_and_replace(curfilter, varDic, upsert = True)


def ImportMeasurementData():
    # , , ,
    '''
    This function mainly to import measurement data to mongoDB
    data type may include Q (discharge, m3/s), totalN, totalP, etc.
    :param HOSTNAME: host IP address of MongoDB
    :param PORT: PORT number of MongoDB
    :param ClimateDBName: HydroClimate database name
    :param MEASUREMENT_DATA_DIR: MEASUREMENT_DATA_DIR of measurement data files
    '''
    try:
        connMongo = MongoClient(HOSTNAME, PORT)
        print "Import Site Measurement Data... "
        # print "Connected successfully"
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
    db = connMongo[ClimateDBName]
    cList = db.collection_names()
    if not StringInList(Tag_ClimateDB_Measurement.upper(), cList):
        db.create_collection(Tag_ClimateDB_Measurement.upper())

    if not StringInList(DB_TAB_DATAVALUES.upper(), cList):
        db.create_collection(DB_TAB_DATAVALUES.upper())

    fileList = GetFullPathWithSuffixes(MEASUREMENT_DATA_DIR, ['.txt'])
    measFileList = []
    siteLoc = []
    for fl in fileList:
        if (IsSubString('observed_', fl)):
            measFileList.append(fl)
        else:
            siteLoc.append(fl)
    ImportData(db, measFileList, siteLoc)


if __name__ == "__main__":
    LoadConfiguration(GetINIfile())
    ImportMeasurementData()
