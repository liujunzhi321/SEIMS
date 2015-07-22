from pymongo import Connection
from pymongo.errors import ConnectionFailure
import bson
import glob
import os
import sqlite3
from config import *

def ImportParameters(sqlite_file, db):
    #read sqlite database
    conn = sqlite3.connect(sqlite_file)
    c = conn.cursor()
    #get all the tablename
    c.execute("select name from sqlite_master where type='table' order by name;")
    tablelist = c.fetchall()
    tablelist = [item[0].encode("ascii") for item in tablelist if (item[0].find("sqlite") < 0 and item[0].lower().find("lookup") < 0 and item[0].lower().find("crop") < 0)]
    #print tablelist
    
    field_list = ['NAME', 'DESCRIPTION', 'UNIT', 'MODULE', 'VALUE', 'IMPACT', 'CHANGE', 'MAX', 'MIN', 'USE']
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
            db.parameters.insert(dic, safe=True)            

    db.parameters.create_index("NAME")
    c.close()
    conn.close()
    print 'parameter table is imported.'
    
if __name__ == "__main__":
    try:
        conn = Connection(host=HOSTNAME, port=PORT)
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
    db = conn[SpatialDBName]
    if "parameters" in db.collection_names():
        db.drop_collection("parameters")
    ImportParameters(sqliteFile, db)