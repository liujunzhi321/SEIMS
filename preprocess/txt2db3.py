#! /usr/bin/env python
#coding=utf-8
## @ Import parameters and lookup tables to SQLite database
#
import os
import sqlite3
from itertools import islice
from text import *
from util import *

#
# def txt2Paras_Table(dataFile,dbFile):
#     f = open(dataFile)
#     for line in f:
#         if line !='' and line != '\n' and line.find('#')<0:
#             list = line.split("\t")
#             tbname = list[0]
#             dataRow = []
#             for i in range(1,len(list)):
#                 dataRow.append(list[i])
#             conn = sqlite3.connect(dbFile)
#             cur = conn.cursor()
#             create_table_sql = '''CREATE TABLE IF NOT EXISTS %s (
#                                Parameter TEXT,
#                                Description TEXT,
#                                Unit TEXT DEFAULT NULL,
#                                Module_ID TEXT DEFAULT NULL,
#                                Value FLOAT DEFAULT NULL,
#                                Impact FLOAT DEFAULT NULL,
#                                Change TEXT DEFAULT NULL,
#                                Max FLOAT DEFAULT NULL,
#                                Min FLOAT DEFAULT NULL,
#                                Use TEXT DEFAULT NULL
#                                )''' % tbname
#             cur.execute(create_table_sql)
#             load_sql = '''insert into %s values (?,?,?,?,?,?,?,?,?,?)''' % tbname
#             cur.execute(load_sql,dataRow)
#             conn.commit()
#     f.close()
#     cur.close()
#     conn.close()
# def txt2SoilLookup(dataFile,dbFile):
#     filename = dataFile.split('/')[-1]
#     tbname = os.path.splitext(filename)[0]
#     f = open(dataFile)
#     for line in islice(f,1,None):
#         if line !='' and line != '\n' and line.find('#')<0:
#             list = line.split("\t")
#             dataRow = []
#             for i in range(0,len(list)):
#                 dataRow.append(list[i])
#             conn = sqlite3.connect(dbFile)
#             cur = conn.cursor()
#             create_table_sql = '''CREATE TABLE IF NOT EXISTS %s (
#                                SOILCODE INT,
#                                SNAM TEXT,
#                                KS REAL DEFAULT NULL,
#                                POROSITY REAL DEFAULT NULL,
#                                FC REAL DEFAULT NULL,
#                                P_INDEX REAL DEFAULT NULL,
#                                RM REAL DEFAULT NULL,
#                                WiltingPoint REAL DEFAULT NULL,
#                                B_DENSITY REAL DEFAULT NULL,
#                                SAND REAL DEFAULT NULL,
#                                CLAY REAL DEFAULT NULL,
#                                SILT REAL DEFAULT NULL,
#                                USLE_K REAL DEFAULT NULL,
#                                TEXTURE INT DEFAULT NULL,
#                                HG INT DEFAULT NULL
#                                )''' % tbname
#             cur.execute(create_table_sql)
#             load_sql = '''insert into %(table)s values (%(arg)s)''' % {'table':tbname,'arg':','.join(['?' for i in range (0,len(list))]),}
#             cur.execute(load_sql,dataRow)
#             conn.commit()
#     f.close()
#     cur.close()
#     conn.close()
# def txt2LanduseLookup(dataFile,dbFile):
#     filename = dataFile.split('/')[-1]
#     tbname = os.path.splitext(filename)[0]
#     f = open(dataFile)
#     for line in islice(f,1,None):
#         if line !='' and line != '\n' and line.find('#')<0:
#             list = line.split("\t")
#             dataRow = []
#             for i in range(0,len(list)):
#                 dataRow.append(list[i])
#             conn = sqlite3.connect(dbFile)
#             cur = conn.cursor()
#             create_table_sql = '''CREATE TABLE IF NOT EXISTS %s (
#                                LANDUSE_ID INT,
#                                CODE TEXT,
#                                LANDUSE_NAME TEXT DEFAULT NULL,
#                                CN2A REAL DEFAULT NULL,
#                                CN2B REAL DEFAULT NULL,
#                                CN2C REAL DEFAULT NULL,
#                                CN2D REAL DEFAULT NULL,
#                                ROOT_DEPTH REAL DEFAULT NULL,
#                                MANNING REAL DEFAULT NULL,
#                                I_MAX REAL DEFAULT NULL,
#                                I_MIN REAL DEFAULT NULL,
#                                SHC REAL DEFAULT NULL,
#                                SOIL_T10 REAL DEFAULT NULL,
#                                USLE_C REAL DEFAULT NULL,
#                                PET_FR REAL DEFAULT NULL,
#                                PRC_ST1 REAL DEFAULT NULL,
#                                PRC_ST2 REAL DEFAULT NULL,
#                                PRC_ST3 REAL DEFAULT NULL,
#                                PRC_ST4 REAL DEFAULT NULL,
#                                PRC_ST5 REAL DEFAULT NULL,
#                                PRC_ST6 REAL DEFAULT NULL,
#                                PRC_ST7 REAL DEFAULT NULL,
#                                PRC_ST8 REAL DEFAULT NULL,
#                                PRC_ST9 REAL DEFAULT NULL,
#                                PRC_ST10 REAL DEFAULT NULL,
#                                PRC_ST11 REAL DEFAULT NULL,
#                                PRC_ST12 REAL DEFAULT NULL,
#                                SC_ST1 REAL DEFAULT NULL,
#                                SC_ST2 REAL DEFAULT NULL,
#                                SC_ST3 REAL DEFAULT NULL,
#                                SC_ST4 REAL DEFAULT NULL,
#                                SC_ST5 REAL DEFAULT NULL,
#                                SC_ST6 REAL DEFAULT NULL,
#                                SC_ST7 REAL DEFAULT NULL,
#                                SC_ST8 REAL DEFAULT NULL,
#                                SC_ST9 REAL DEFAULT NULL,
#                                SC_ST10 REAL DEFAULT NULL,
#                                SC_ST11 REAL DEFAULT NULL,
#                                SC_ST12 REAL DEFAULT NULL,
#                                DSC_ST1 REAL DEFAULT NULL,
#                                DSC_ST2 REAL DEFAULT NULL,
#                                DSC_ST3 REAL DEFAULT NULL,
#                                DSC_ST4 REAL DEFAULT NULL,
#                                DSC_ST5 REAL DEFAULT NULL,
#                                DSC_ST6 REAL DEFAULT NULL,
#                                DSC_ST7 REAL DEFAULT NULL,
#                                DSC_ST8 REAL DEFAULT NULL,
#                                DSC_ST9 REAL DEFAULT NULL,
#                                DSC_ST10 REAL DEFAULT NULL,
#                                DSC_ST11 REAL DEFAULT NULL,
#                                DSC_ST12 REAL DEFAULT NULL
#                                )''' % tbname
#             cur.execute(create_table_sql)
#             load_sql = '''insert into %(table)s values (%(arg)s)''' % {'table':tbname,'arg':','.join(['?' for i in range (0,len(list))]),}
#             cur.execute(load_sql,dataRow)
#             conn.commit()
#     f.close()
#     cur.close()
#     conn.close()
# def txt2CropLookup(dataFile,dbFile):
#     filename = dataFile.split('/')[-1]
#     tbname = os.path.splitext(filename)[0]
#     f = open(dataFile)
#     for line in islice(f,1,None):
#         if line !='' and line != '\n' and line.find('#')<0:
#             list = line.split("\t")
#             dataRow = []
#             for i in range(0,len(list)):
#                 dataRow.append(list[i])
#             conn = sqlite3.connect(dbFile)
#             cur = conn.cursor()
#             create_table_sql = '''CREATE TABLE IF NOT EXISTS %s (
#                                ICNUM INT Primary key,
#                                CPNM TEXT,
#                                DESCRIPTION TEXT DEFAULT NULL,
#                                IDC INT DEFAULT NULL,
#                                BIO_E REAL DEFAULT NULL,
#                                HVSTI REAL DEFAULT NULL,
#                                BLAI REAL DEFAULT NULL,
#                                FRGRW1 REAL DEFAULT NULL,
#                                LAIMX1 REAL DEFAULT NULL,
#                                FRGRW2 REAL DEFAULT NULL,
#                                LAIMX2 REAL DEFAULT NULL,
#                                DLAI REAL DEFAULT NULL,
#                                CHTMX REAL DEFAULT NULL,
#                                RDMX REAL DEFAULT NULL,
#                                T_OPT REAL DEFAULT NULL,
#                                T_BASE REAL DEFAULT NULL,
#                                CNYLD REAL DEFAULT NULL,
#                                CPYLD rREAL DEFAULT NULL,
#                                BN1 REAL DEFAULT NULL,
#                                BN2 REAL DEFAULT NULL,
#                                BN3 REAL DEFAULT NULL,
#                                BP1 REAL DEFAULT NULL,
#                                BP2 REAL DEFAULT NULL,
#                                BP3 REAL DEFAULT NULL,
#                                WSYF REAL DEFAULT NULL,
#                                USLE_C REAL DEFAULT NULL,
#                                GSI REAL DEFAULT NULL,
#                                VPDFR REAL DEFAULT NULL,
#                                FRGMAX REAL DEFAULT NULL,
#                                WAVP REAL DEFAULT NULL,
#                                CO2HI REAL DEFAULT NULL,
#                                BIOEHI REAL DEFAULT NULL,
#                                RSDCO_PL REAL DEFAULT NULL,
#                                OV_N REAL DEFAULT NULL,
#                                CN2A REAL DEFAULT NULL,
#                                CN2B REAL DEFAULT NULL,
#                                CN2C REAL DEFAULT NULL,
#                                CN2D REAL DEFAULT NULL,
#                                FERTFIELD REAL DEFAULT NULL,
#                                ALAI_MIN REAL DEFAULT NULL,
#                                BIO_LEAF REAL DEFAULT NULL,
#                                PHU REAL DEFAULT NULL,
#                                CNOP REAL DEFAULT NULL,
#                                LAI_INIT REAL DEFAULT NULL,
#                                BIO_INIT REAL DEFAULT NULL,
#                                CURYR_INIT INT DEFAULT NULL
#                                )''' % tbname
#             cur.execute(create_table_sql)
#             load_sql = '''insert into %(table)s values (%(arg)s)''' % {'table':tbname,'arg':','.join(['?' for i in range (0,len(list))]),}
#             cur.execute(load_sql,dataRow)
#             conn.commit()
#     f.close()
#     cur.close()
#     conn.close()

def txt2Sqlite(dataFiles, dbFile):
    dataImport = {} ### format: {tabName:[fieldName, Units, dataRows]}
    for dataFileItem in dataFiles:
        dataPath = TXT_DB_DIR + os.sep + dataFileItem[1] + ".txt"
        dataItems = ReadDataItemsFromTxt(dataPath)
        if dataFileItem[0] == Tag_Params:
            fieldNames = dataItems[0][1:]
            units = dataItems[1][1:]
            CHANGE_IDX  = fieldNames.index(PARAM_FLD_CHANGE) + 1
            IMPACT_IDX  = fieldNames.index(PARAM_FLD_IMPACT) + 1
            MAX_IDX     = fieldNames.index(PARAM_FLD_MAX) + 1
            MIN_IDX     = fieldNames.index(PARAM_FLD_MIN) + 1
            for i in range(2,len(dataItems)):
                curDataItem = dataItems[i]
                if curDataItem[CHANGE_IDX] == PARAM_CHANGE_NC or curDataItem[CHANGE_IDX] == '':
                    curDataItem[IMPACT_IDX] = 0
                if curDataItem[CHANGE_IDX] == PARAM_CHANGE_RC and curDataItem[IMPACT_IDX] == '':
                    curDataItem[IMPACT_IDX] = 1
                if curDataItem[CHANGE_IDX] == PARAM_CHANGE_AC and curDataItem[IMPACT_IDX] == '':
                    curDataItem[IMPACT_IDX] = 0
                if curDataItem[MAX_IDX] == '':
                    curDataItem[MAX_IDX] = DEFAULT_NODATA
                if curDataItem[MIN_IDX] == '':
                    curDataItem[MIN_IDX] = DEFAULT_NODATA
                if curDataItem[0] in dataImport.keys():
                    dataImport[curDataItem[0]][2].append(curDataItem[1:])
                else:
                    dataImport[curDataItem[0]] = [fieldNames, units, [curDataItem[1:]]]

        else:
            fieldNames = dataItems[0]
            units = dataItems[1]
            if dataFileItem[1] not in dataImport:
                dataImport[dataFileItem[1]] = [fieldNames, units, []]
            for i in range(2,len(dataItems)):
                dataImport[dataFileItem[1]][2].append(dataItems[i])
    #print dataImport
    importData2Sqlite(dataImport, dbFile)
def importData2Sqlite(dataImport, dbFile):
    conn = sqlite3.connect(dbFile)
    cur = conn.cursor()
    for tabName in dataImport.keys():
        flds = dataImport[tabName][0]
        unitTypes = dataImport[tabName][1]
        dataRow = dataImport[tabName][2]
        fieldNameStr = ''
        for i in range(len(flds)):
            fieldNameStr += flds[i] + ' ' + unitTypes[i] + ' DEFAULT NULL,'
        create_table_sql = '''CREATE TABLE IF NOT EXISTS %s (%s)''' % (tabName, fieldNameStr[:-1])
        #print create_table_sql
        cur.execute(create_table_sql)
        load_sql = '''insert into %(table)s values (%(arg)s)''' % {'table':tabName,'arg':','.join(['?' for i in range (0,len(flds))]),}
        #print load_sql
        for singledatarow in dataRow:
            cur.execute(load_sql,singledatarow)
    conn.commit()
    cur.close()
    conn.close()
def header2Sqlite(maskFile, sqliteFile):
    maskR = ReadRaster(maskFile)
    headerInfo = {}
    headerInfo[HEADER_RS_NODATA] = maskR.noDataValue
    headerInfo[HEADER_RS_CELLSIZE] = maskR.dx
    headerInfo[HEADER_RS_NCOLS] = maskR.nCols
    headerInfo[HEADER_RS_NROWS] = maskR.nRows
    headerInfo[HEADER_RS_XLL] = maskR.xMin
    headerInfo[HEADER_RS_YLL] = maskR.yMin
    #print headerInfo
    ## Construct dataImport
    dataImport = {}
    fldNames = [PARAM_FLD_NAME,PARAM_FLD_DESC,PARAM_FLD_UNIT,PARAM_FLD_MODS,PARAM_FLD_VALUE,\
                PARAM_FLD_IMPACT,PARAM_FLD_CHANGE,PARAM_FLD_MAX,PARAM_FLD_MIN,PARAM_FLD_USE]
    units = ["TEXT","TEXT","TEXT","TEXT","REAL","REAL","VARCHAR(2)","REAL","REAL","VARCHAR(1)"]
    dataItem = []
    for item in headerInfo.keys():
        dataItem.append([item, item, "", "All", headerInfo[item], "0", PARAM_CHANGE_NC,DEFAULT_NODATA,\
                         DEFAULT_NODATA,PARAM_USE_Y])
    dataImport[HEADER_RS_TAB] = [fldNames, units, dataItem]
    #print dataImport
    importData2Sqlite(dataImport, sqliteFile)
def reConstructSQLiteDB():
    # If the database file existed, DELETE it.
    if os.path.exists(sqliteFile):
        os.remove(sqliteFile)
    dataFiles = [[Tag_Params, init_params]]
    for df in lookup_tabs:
        dataFiles.append([Tag_Lookup, df])
    txt2Sqlite(dataFiles, sqliteFile)
    # Read Raster Header from MASK, and import to database, useless and deprecated! LJ
    #header2Sqlite(WORKING_DIR + os.sep + mask_to_ext, sqliteFile)
if __name__ == "__main__":
    reConstructSQLiteDB()

    #print dataFiles
    # dataFile1 = "G:/SEIMS/database/model_param_ini.txt"
    # dataFile2 = "G:/SEIMS/database/SoilLookup.txt"
    # dataFile3 = "G:/SEIMS/database/LanduseLookup.txt"
    # dataFile4 = "G:/SEIMS/database/CropLookup.txt"
    # DATA_BASE_DIR = r'G:\SEIMS\model_data\model_dianbu_30m_longterm'
    # dbFile = DATA_BASE_DIR + os.sep + "Parameter.db3"
    # txt2Paras_Table(dataFile1,dbFile)
    # txt2SoilLookup(dataFile2,dbFile)
    # txt2LanduseLookup(dataFile3,dbFile)
    # txt2CropLookup(dataFile4,dbFile)
    
