#from pymongo import Connection
from pymongo import MongoClient
from pymongo.errors import ConnectionFailure
import sqlite3
from text import *

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

    field_list = [PARAM_FLD_NAME,PARAM_FLD_DESC,PARAM_FLD_UNIT,PARAM_FLD_MODS,PARAM_FLD_VALUE,\
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
            #db.parameters.insert(dic, safe=True)
            db.parameters.insert(dic)

    db.parameters.create_index(PARAM_FLD_NAME)
    c.close()
    conn.close()
    print 'Parameter tables are imported.'
    
if __name__ == "__main__":
    try:
        conn = MongoClient(host=HOSTNAME, port=PORT)
        #conn = Connection(host=HOSTNAME, port=PORT)
    except ConnectionFailure, e:
        sys.stderr.write("Could not connect to MongoDB: %s" % e)
        sys.exit(1)
    db = conn[SpatialDBName]
    if "parameters" in db.collection_names():
        db.drop_collection("parameters")
    ImportParameters(sqliteFile, db)