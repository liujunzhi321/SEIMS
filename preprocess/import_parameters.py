#! /usr/bin/env python
# coding=utf-8
## @Import model calibrateion parameters
# Author: Junzhi Liu
# Revised: Liang-Jun Zhu
#
from pymongo import MongoClient
from pymongo.errors import ConnectionFailure
import sqlite3
from gridfs import *
from struct import pack
from config import *
from util import *


def ImportParameters(sqlite_file, db):
    # read sqlite database
    conn = sqlite3.connect(sqlite_file)
    c = conn.cursor()
    # get all the tablename
    c.execute("select name from sqlite_master where type='table' order by name;")
    tablelist = c.fetchall()
    # Find parameter table list excluding "XXLookup"
    tablelist = [item[0].encode("ascii") for item in tablelist if (item[0].lower().find("lookup") < 0)]
    # print tablelist

    field_list = [PARAM_FLD_NAME.upper(), PARAM_FLD_DESC.upper(), PARAM_FLD_UNIT.upper(),
                  PARAM_FLD_MODS.upper(), PARAM_FLD_VALUE.upper(), PARAM_FLD_IMPACT.upper(),
                  PARAM_FLD_CHANGE.upper(), PARAM_FLD_MAX.upper(), PARAM_FLD_MIN.upper(),
                  PARAM_FLD_USE.upper()]
    for tablename in tablelist:
        # print tablename
        str_sql = "select * from %s;" % (tablename,)
        cur = c.execute(str_sql)
        records = cur.fetchall()
        for items in records:
            dic = {}
            dic[Tag_DT_Type.upper()] = tablename
            for i in range(len(items)):
                if (type(items[i]) == type('a') or type(items[i]) == type(u'a')):
                    dic[field_list[i].upper()] = items[i].encode('ascii')
                else:
                    dic[field_list[i].upper()] = items[i]
            curfilter = {PARAM_FLD_NAME.upper(): dic[PARAM_FLD_NAME.upper()], Tag_DT_Type.upper(): tablename}
            db[DB_TAB_PARAMETERS.upper()].find_one_and_replace(curfilter, dic, upsert = True)

    db[DB_TAB_PARAMETERS.upper()].create_index(PARAM_FLD_NAME.upper())
    c.close()
    conn.close()
    print 'Parameter tables are imported.'


def ImportLookupTables(sqlite_file, db):
    '''
    :param sqlite_file: SQLite database file contains lookup tables
    :param db: MongoDB Client
    :return: None
    '''
    # read sqlite database
    conn = sqlite3.connect(sqlite_file)
    c = conn.cursor()
    # get all the tablename
    c.execute("select name from sqlite_master where type='table' order by name;")
    tablelist = c.fetchall()
    # Find parameter table list excluding "XXLookup"
    tablelist = [item[0].encode("ascii") for item in tablelist if (item[0].lower().find("lookup") >= 0)]
    # print tablelist
    for tablename in tablelist:
        # print tablename
        str_sql = "select * from %s;" % (tablename,)
        cur = c.execute(str_sql)
        records = cur.fetchall()
        itemValues = []
        for items in records:
            itemValue = []
            for i in range(len(items)):
                if isNumericValue(items[i]):
                    itemValue.append(float(items[i]))
            itemValues.append(itemValue)
        nRow = len(itemValues)
        # print itemValues
        if (nRow >= 1):
            nCol = len(itemValues[0])
            for i in range(nRow):
                if (nCol != len(itemValues[i])):
                    raise ValueError(
                            "Please check %s to make sure each item has the same numeric dimension." % tablename)
            ### import to mongoDB as GridFS
            spatial = GridFS(db, DB_TAB_SPATIAL.upper())
            ### delete if the tablename file existed already.
            if (spatial.exists(filename = tablename.upper())):
                x = spatial.get_version(filename = tablename.upper())
                spatial.delete(x._id)
            metadic = {META_LOOKUP_ITEM_COUNT.upper(): nRow, META_LOOKUP_FIELD_COUNT.upper(): nCol}
            curLookupGridFS = spatial.new_file(filename = tablename.upper(), metadata = metadic)
            fmt = '%df' % (nCol)
            for i in range(nRow):
                s = pack(fmt, *itemValues[i])
                curLookupGridFS.write(s)
            curLookupGridFS.close()
    c.close()
    conn.close()
    # print 'Lookup tables are imported.'


if __name__ == "__main__":
    try:
        conn = MongoClient(host = HOSTNAME, port = PORT)
        # conn = Connection(host=HOSTNAME, port=PORT)
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
    db = conn[SpatialDBName]
    ImportParameters(TXT_DB_DIR + os.sep + sqliteFile, db)
    ### IMPORT LOOKUP TABLES AS GRIDFS, DT_Array2D
    ImportLookupTables(TXT_DB_DIR + os.sep + sqliteFile, db)
