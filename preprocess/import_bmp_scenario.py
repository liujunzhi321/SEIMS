#! /usr/bin/env python
#coding=utf-8
## @Import BMP Scenario related parameters to MongoDB
## @Author Liang-Jun Zhu
## @Date   2016-6-16
#

from pymongo import MongoClient
from pymongo.errors import ConnectionFailure
from gridfs import *
from struct import pack
from text import *
from util import *
from import_parameters import ImportLookupTables

def ImportBMPTables():
    '''
    Import management data to MongoDB
    '''
    BMP_tabs_path = [BMP_DATA_DIR + os.sep + item + ".txt" for item in BMP_tabs]
    ## connect to MongoDB
    try:
        conn = MongoClient(host=HOSTNAME, port=PORT)
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
    db = conn[BMPScenarioDBName]
    ## delete if collection existed
    cList = db.collection_names()
    for item in BMP_tabs:
        if not item in cList:
            db.create_collection(item)
        else:
            db.drop_collection(item)
    for j in range(len(BMP_tabs_path)):
        bmpTxt = BMP_tabs_path[j]
        bmpTabName = BMP_tabs[j]
        dataArray = ReadDataItemsFromTxt(bmpTxt)
        fieldArray = dataArray[0]
        dataArray = dataArray[1:]
        for item in dataArray:
            dic = {}
            for i in range(len(fieldArray)):
                if(isNumericValue(item[i])):
                    dic[fieldArray[i]] = float(item[i])
                else:
                    dic[fieldArray[i]] = str(item[i])
            db[bmpTabName].insert_one(dic)

    print 'BMP tables are imported.'
    ImportLookupTables(sqliteFile, db)
    conn.close()
if __name__ == "__main__":
    ImportBMPTables()
