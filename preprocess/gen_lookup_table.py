#! /usr/bin/env python
#coding=utf-8
## @Generate landuse lookup table
#
#
from osgeo import gdal,osr
import math, os, sys
import sqlite3
from numpy import *

def CreateLookupTable(dbname, property_namelist, str_sql,dstdir):
    property_map = {}
    num_propeties = len(property_namelist)
    conn = sqlite3.connect(dbname)
    cursor = conn.cursor()
    
    cursor.execute(str_sql)
    for row in cursor:
        #print row
        id = int(row[0])
        value_map = {}
        for i in range(num_propeties):
            pName = property_namelist[i]
            if pName == "USLE_P":
                value_map[pName] = 1
            else:
                if pName == "Manning":
                    value_map[pName] = row[i+1]*10
                else:
                    value_map[pName] = row[i+1]
        property_map[id] = value_map
   
    n = len(property_map)
    os.chdir(dstdir)
    for propertyName in property_namelist:
        if not os.path.exists('lookup'):
            os.mkdir("lookup")
        f = open("lookup/%s.txt" % (propertyName,), 'w')
        f.write("%d\n"%(n))
        for id in property_map:
            s = "%d %f\n" % (id, property_map[id][propertyName])
            f.write(s)
        f.close()

if __name__ == "__main__":
    dbname = "../../spatial_data/Parameter.db3" 
    property_namelist = ["Manning", "Interc_max", "Interc_min", "RootDepth", "USLE_C", "SOIL_T10", "USLE_P"]
    str_sql = 'select landuse_id,manning,i_max,i_min, root_depth, usle_c, SOIL_T10 from LanduseLookup'
    CreateLookupTable(dbname, property_namelist, str_sql)

    property_namelist = ["Sand", "Clay", "FieldCap", "Conductivity", "Porosity", "USLE_K", "WiltingPoint", "Poreindex", "Residual", "Density"]
    str_sql = 'select soilcode,sand,clay, fc, ks,porosity, usle_k, wp, P_INDEX, rm, B_DENSITY  from SoilLookup'
    CreateLookupTable(dbname, property_namelist, str_sql)

