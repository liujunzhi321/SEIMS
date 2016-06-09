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
### Import climate data table
def ImportDayData(db, ClimateDateFile, sitesLoc):
    climDataItems = ReadDataItemsFromTxt(ClimateDateFile)
    climFlds = climDataItems[0]
    T_mean = []
    Year = []
    requiredFlds = [Tag_DT_Year, Tag_DT_Month, Tag_DT_Day, \
                    DataType_MaximumTemperature, DataType_MinimumTemperature, \
                    DataType_RelativeAirMoisture, DataType_WindSpeed]
    for fld in requiredFlds:
        if not StringInList(fld, climFlds):
            exit(0)  ### data can not meet the request!
    for i in range(1,len(climDataItems)):
        dic = {}
        curSSD = DEFAULT_NODATA
        for j in range(len(climDataItems[i])):
            if StringMatch(climFlds[j], Tag_DT_StationID):
                dic[Tag_DT_StationID] = int(climDataItems[i][j])
            elif StringMatch(climFlds[j], Tag_DT_Year):
                curY = int(climDataItems[i][j])
            elif StringMatch(climFlds[j], Tag_DT_Month):
                curM = int(climDataItems[i][j])
            elif StringMatch(climFlds[j], Tag_DT_Day):
                curD = int(climDataItems[i][j])
            elif StringMatch(climFlds[j], DataType_MeanTemperature):
                dic[DataType_MeanTemperature] = float(climDataItems[i][j])
            elif StringMatch(climFlds[j], DataType_MinimumTemperature):
                dic[DataType_MinimumTemperature] = float(climDataItems[i][j])
            elif StringMatch(climFlds[j], DataType_MaximumTemperature):
                dic[DataType_MaximumTemperature] = float(climDataItems[i][j])
            elif StringMatch(climFlds[j], DataType_PotentialEvapotranspiration):
                dic[DataType_PotentialEvapotranspiration] = float(climDataItems[i][j])
            elif StringMatch(climFlds[j], DataType_SolarRadiation):
                dic[DataType_SolarRadiation] = float(climDataItems[i][j])
            elif StringMatch(climFlds[j], DataType_WindSpeed):
                dic[DataType_WindSpeed] = float(climDataItems[i][j])
            elif StringMatch(climFlds[j], DataType_RelativeAirMoisture):
                dic[DataType_RelativeAirMoisture] = float(climDataItems[i][j]) * 0.01
            elif StringMatch(climFlds[j], DataType_SunDurationHour):
                curSSD = float(climDataItems[i][j])
        ### Date transformation
        dt = datetime.datetime(curY, curM, curD, 0, 0)
        sec = time.mktime(dt.timetuple())
        utcTime = time.gmtime(sec)
        dic[Tag_DT_LocalT] = dt
        dic[Tag_DT_Zone] = time.timezone / 3600
        dic[Tag_DT_UTC] = datetime.datetime(utcTime[0], utcTime[1], utcTime[2], utcTime[3])

        ### Do if some of these data are not provided
        if DataType_MeanTemperature not in dic.keys():
            dic[DataType_MeanTemperature] = (dic[DataType_MaximumTemperature] + dic[DataType_MinimumTemperature]) / 2.
        if DataType_SolarRadiation not in dic.keys():
            if curSSD == DEFAULT_NODATA:
                print DataType_SolarRadiation + " or " + DataType_SunDurationHour + " must be provided!"
                exit(0)
            else:
                if dic[Tag_DT_StationID] in sitesLoc.keys():
                    curLon, curLat = sitesLoc[dic[Tag_DT_StationID]].LonLat()
                    dic[DataType_SolarRadiation] = Rs(doy(dt), float(curSSD), curLat * math.pi / 180.0)
        outputFlds = [DataType_MeanTemperature, DataType_MaximumTemperature, DataType_MinimumTemperature,\
                      DataType_RelativeAirMoisture, DataType_PotentialEvapotranspiration, DataType_WindSpeed,\
                      DataType_SolarRadiation]
        for fld in outputFlds:
            curDic = {}
            if fld in dic.keys():
                curDic[Tag_DT_Value] = dic[fld]
                curDic[Tag_DT_StationID] = dic[Tag_DT_StationID]
                curDic[Tag_DT_UTC] = dic[Tag_DT_UTC]
                curDic[Tag_DT_Zone] = dic[Tag_DT_Zone]
                curDic[Tag_DT_LocalT] = dic[Tag_DT_LocalT]
                curDic[Tag_DT_Type] = fld
                db[Tag_ClimateDB_Data].insert_one(curDic)

        T_mean.append(dic[DataType_MeanTemperature])
        Year.append(curY)
    for i in range(0,len(list(set(Year)))):
        curDic = {}
        HU = 0
        T_base = 0
        for j in range(0,len(Year)):
            if(Year[j]==list(set(Year))[i]):
                HU += T_mean[j] - T_base*1.0
        curDic[Tag_DT_Value] = HU
        curDic[Tag_DT_Year] = list(set(Year))[i]
        curDic[Tag_VAR_UNIT] = "deg C"
        curDic[Tag_VAR_Type] = DataType_YearlyHeatUnit
        db[Tag_ClimateDB_YearlyHeatUnit].insert_one(curDic)

def ImportDailyMeteoData(hostname,port,dbName,meteofile,siteMLoc):
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
    # else:
    #     db.drop_collection(tb)
    ImportDayData(db, meteofile, siteMLoc)
    connMongo.close()
