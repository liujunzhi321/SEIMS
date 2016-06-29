from pymongo import MongoClient
from pymongo.errors import ConnectionFailure
import sqlite3
from gridfs import *
from struct import pack
from text import *
from util import *

def ImportParameters(sqlite_file, db):
    #read sqlite database
    conn = sqlite3.connect(sqlite_file)
    c = conn.cursor()
    #get all the tablename
    c.execute("select name from sqlite_master where type='table' order by name;")
    tablelist = c.fetchall()
    # Find parameter table list excluding "XXLookup"
    tablelist = [item[0].encode("ascii") for item in tablelist if (item[0].lower().find("lookup") < 0)]
    #print tablelist

    field_list = [PARAM_FLD_NAME,PARAM_FLD_DESC,PARAM_FLD_UNIT,PARAM_FLD_MODS,PARAM_FLD_VALUE,
                PARAM_FLD_IMPACT,PARAM_FLD_CHANGE,PARAM_FLD_MAX,PARAM_FLD_MIN,PARAM_FLD_USE]
    for tablename in tablelist:
        #print tablename
        str_sql = "select * from %s;" % (tablename,)
        cur = c.execute(str_sql)
        records = cur.fetchall()
        for items in records:
            dic = {}
            dic["TYPE"] = tablename
            for i in range(len(items)):
                if(type(items[i]) == type('a') or type(items[i]) == type(u'a')):
                    dic[field_list[i]] = items[i].encode('ascii')
                else:
                    dic[field_list[i]] = items[i]
            curfilter = {PARAM_FLD_NAME: dic[PARAM_FLD_NAME], "TYPE": tablename}
            db[DB_TAB_PARAMETERS].find_one_and_replace(curfilter, dic, upsert=True)
            # db[DB_TAB_PARAMETERS].insert_one(dic)
    db[DB_TAB_PARAMETERS].create_index(PARAM_FLD_NAME)
    c.close()
    conn.close()
    print 'Parameter tables are imported.'
def ImportLookupTables(sqlite_file, db):
    '''
    :param sqlite_file: SQLite database file contains lookup tables
    :param db: MongoDB Client
    :return: None
    '''
    #read sqlite database
    conn = sqlite3.connect(sqlite_file)
    c = conn.cursor()
    #get all the tablename
    c.execute("select name from sqlite_master where type='table' order by name;")
    tablelist = c.fetchall()
    # Find parameter table list excluding "XXLookup"
    tablelist = [item[0].encode("ascii") for item in tablelist if (item[0].lower().find("lookup") >= 0)]
    #print tablelist
    for tablename in tablelist:
        #print tablename
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
        #print itemValues
        if(nRow >= 1):
            nCol = len(itemValues[0])
            for i in range(nRow):
                if(nCol != len(itemValues[i])):
                    raise ValueError("Please check %s to make sure each item has the same numeric dimension."%tablename)
            ### import to mongoDB as GridFS
            spatial = GridFS(db, DB_TAB_SPATIAL)
            ### delete if the tablename file existed already.
            if(spatial.exists(filename=tablename)):
                x = spatial.get_version(filename=tablename)
                spatial.delete(x._id)
            metadic = {'ITEM_COUNT': nRow, 'FIELD_COUNT': nCol}
            curLookupGridFS = spatial.new_file(filename=tablename,metadic=metadic)
            fmt = '%df'%(nCol)
            for i in range(nRow):
                s = pack(fmt, *itemValues[i])
                curLookupGridFS.write(s)
            curLookupGridFS.close()
    c.close()
    conn.close()
    #print 'Lookup tables are imported.'
if __name__ == "__main__":
    try:
        conn = MongoClient(host=HOSTNAME, port=PORT)
        #conn = Connection(host=HOSTNAME, port=PORT)
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
    db = conn[SpatialDBName]
    # if "parameters" in db.collection_names():
    #     db.drop_collection("parameters")
    # ImportParameters(sqliteFile, db)
    ### IMPORT LOOKUP TABLES AS GRIDFS, DT_Array2D
    ImportLookupTables(sqliteFile, db)