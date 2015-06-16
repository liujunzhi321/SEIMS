/*!
 * \file MongoUtil.h
 * \ingroup module_setting
 * \brief Read data from MongoDB.
 *
 *
 *
 * \author [your name]
 * \version 
 * \date June 2015
 *
 * 
 */
#pragma once
#include "mongo.h"
#include <string>
#include <set>
#include "gridfs.h"
#include "clsRasterData.h"
#include "util.h"

using namespace std;


extern int GetCollectionNames(mongo* conn, string& dbName, set<string>& tableNameSet);
extern void Read2DSoilAttr(gridfs* spatialData, string remoteFilename, clsRasterData* templateRaster, int& n, float**& data);
extern void Read2DArrayFromMongoDB(gridfs* spatialData, string& remoteFilename, clsRasterData* templateRaster, int& n, float**& data);
extern void Read1DArrayFromMongoDB(gridfs* spatialData, string& remoteFilename, clsRasterData* templateRaster, int& num, float*& data);
extern void ReadMutltiReachInfoFromMongoDB(LayeringMethod layeringMethod, string& dbName, gridfs* spatialData, string& remoteFilename, clsRasterData* templateRaster, int& nr, int& nc, float**& data);
extern void ReadReachInfoFromMongoDB(LayeringMethod layeringMethod, string& dbName, gridfs* spatialData, int nSubbasin, string remoteFilename, clsRasterData* templateRaster, int& num, int& nCols, float**& data);
extern void ReadIUHFromMongoDB(gridfs* spatialData, string& remoteFilename, clsRasterData* templateRaster, int& n, float**& data);
extern void ReadLongTermMutltiReachInfo(string& dbName, gridfs* spatialData, int& nr, int& nc, float**& data);
extern void ReadLongTermReachInfo(string& dbName, gridfs* spatialData, int subbasinID, int& nr, int& nc, float**& data);
