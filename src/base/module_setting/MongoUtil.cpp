/*!
 * \file MongoUtil.cpp
 * \brief
 *
 *
 *
 * \author [your name]
 * \version 
 * \date June 2015
 *
 * 
 */
#include "MongoUtil.h"
#include <sstream>
#include "ModelException.h"
#include "utils.h"
#include "text.h"
#include <fstream>
using namespace std;
//! Get collection names in mongoDB
int GetCollectionNames(mongo* conn, string& dbName, set<string>& tableNameSet)
{
	//bson bsonTable[1];
	//bson_init(bsonTable);
	//bson_append_regex(bsonTable, "name", "^[^\$]*$", "");
	//bson_finish(bsonTable);

	//mongo_cursor cursor[1];
	//ostringstream oss;
	//oss << dbName << ".system.namespaces";
	//mongo_cursor_init(cursor, conn, oss.str().c_str());
	//mongo_cursor_set_query(cursor, bsonTable);

	//
	//while( mongo_cursor_next(cursor) == MONGO_OK ) 
	//{
	//	bson_iterator iterator[1];
	//	if (bson_find(iterator, mongo_cursor_bson(cursor), "name")) 
	//	{
	//		tableNameSet.insert(string(bson_iterator_string(iterator)));
	//	}
	//}

	//bson_destroy(bsonTable);
	//mongo_cursor_destroy(cursor);

	return 0;
}

void ReadIUHFromMongoDB(gridfs* spatialData, string& remoteFilename, clsRasterData* templateRaster, int& n, float**& data)
{
	gridfile gfile[1];
	bson b[1];
	bson_init(b);
	bson_append_string(b, "filename",  remoteFilename.c_str());
	bson_finish(b);  
	int flag = gridfs_find_query(spatialData, b, gfile); 
	if(0 != flag)
		throw ModelException("MongoUtil.cpp","Read2DArrayFromMongoDB", "Failed in  gridfs_find_query filename: " + remoteFilename + "\n");

	size_t length = (size_t)gridfile_get_contentlength(gfile);
	char* buf = (char*)malloc(length);
	gridfile_read (gfile, length, buf);

	float *floatValues = (float*)buf;
	n = (int)floatValues[0];
	data = new float*[n];

	int index = 1;
	for(int i = 0; i < n; i++)
	{
		int nSub = floatValues[index+1] - floatValues[index] + 3;
		data[i] = new float[nSub];

		data[i][0] = floatValues[index];
		data[i][1] = floatValues[index+1];
		for (int j = 2; j < nSub; j++)
			data[i][j] = floatValues[index+j];

		index = index + nSub;
	}

	bson_destroy(b);
	gridfile_destroy(gfile);

	free(buf);
}

void Read2DSoilAttr(gridfs* spatialData, string remoteFileName, clsRasterData* templateRaster, int& n, float**& data)
{
	string remoteFile2 = remoteFileName + "2";
	clsRasterData raster1(spatialData, remoteFileName.c_str(), templateRaster);
	clsRasterData raster2(spatialData, remoteFile2.c_str(), templateRaster);

	float *data1, *data2;
	raster1.getRasterData(&n, &data1);
	raster2.getRasterData(&n, &data2);

	//data = new float*[2];
	//data[0] = new float[n];
	//data[1] = new float[n];

	//memcpy(data[0], data1, n*sizeof(float));
	//memcpy(data[1], data2, n*sizeof(float));

	data = new float*[n];
	for(int i = 0; i < n; i++){
		data[i] = new float[2];
		data[i][0] = data1[i];
		data[i][1] = data2[i];
	}
}

void Read2DArrayFromMongoDB(gridfs* spatialData, string& remoteFilename, clsRasterData* templateRaster, int& n, float**& data)
{
	gridfile gfile[1];
	bson b[1];
	bson_init(b);
	bson_append_string(b, "filename",  remoteFilename.c_str());
	bson_finish(b);  
	int flag = gridfs_find_query(spatialData, b, gfile); 
	if(0 != flag)
		throw ModelException("ModuleParamter", "Read2DArrayFromMongoDB", "Failed in  gridfs_find_query filename: " + remoteFilename + "\n");

	size_t length = (size_t)gridfile_get_contentlength(gfile);
	char* buf = (char*)malloc(length);
	gridfile_read (gfile, length, buf);

	//Read2DArrayFromString(buf, n, data);
	float *floatValues = (float*)buf;
	int nRows = (int)floatValues[0];
	n = (int)floatValues[0];
	data = new float*[n];

	int index = 1;
	for(int i = 0; i < n; i++)
	{
		int nSub = int(floatValues[index]) + 1;
		data[i] = new float[nSub];

		data[i][0] = floatValues[index];
		for (int j = 1; j < nSub; j++)
			data[i][j] = floatValues[index+j];

		index = index + nSub;
	}


	bson_destroy(b);
	gridfile_destroy(gfile);

	free(buf);
}

void Read1DArrayFromMongoDB(gridfs* spatialData, string& remoteFilename, clsRasterData* templateRaster, int& num, float*& data)
{
	gridfile gfile[1];
	bson b[1];
	bson_init(b);
	bson_append_string(b, "filename",  remoteFilename.c_str());
	bson_finish(b);  
	int flag = gridfs_find_query(spatialData, b, gfile); 
	if(0 != flag)
		throw ModelException("ModuleParamter","Read1DArrayFromMongoDB", "Failed in  gridfs_find_query filename: " + remoteFilename + "\n");

	size_t length = (size_t)gridfile_get_contentlength(gfile);
	num = length/4;
	data = new float[num];
	char* buf = (char*)data;
	gridfile_read (gfile, length, buf);
	
	if(templateRaster->getCellNumber() != num)
		throw ModelException("MongoUtil","Read1DArrayFromMongoDB","The data length in " + remoteFilename
		+ " is not the same as the template.");

	bson_destroy(b);
	gridfile_destroy(gfile);
}

//storage as the imwebs reachparameter file in mongodb
//void ReadMutltiReachInfoFromMongoDB(gridfs* spatialData, string& remoteFilename, clsRasterData* templateRaster, int& nr, int& nc, float**& data)
//{
//	gridfile gfile[1];
//	bson b[1];
//	bson_init(b);
//	bson_append_string(b, "filename",  remoteFilename.c_str());
//	bson_finish(b);  
//	int flag = gridfs_find_query(spatialData, b, gfile); 
//	if(0 != flag)
//		throw ModelException("MongoUtil","ReadMutltiReachInfoFromMongoDB", "Failed in  gridfs_find_query filename: " + remoteFilename + "\n");
//
//	size_t length = (size_t)gridfile_get_contentlength(gfile);
//	char* buf = (char*)malloc(length);
//	gridfile_read (gfile, length, buf);
//
//	string tmp;
//	int n;
//	istringstream iss(buf);
//
//	utils utl;
//	int nRows, nCols;
//	iss >> nRows >> nCols;
//	int nAttr = 5;
//	data = new float*[nAttr];
//	for (int i = 0; i < nAttr; i++)
//	{
//		data[i] = new float[nRows];
//	}
//	string test = iss.str();
//	string line;
//	getline(iss, line);
//	for (int i = 0; i < nRows; ++i)
//	{
//		getline(iss, line);
//		vector<string> vec = utl.SplitString(line);
//		data[0][i] = atof(vec[0].c_str());
//		data[1][i] = atof(vec[10].c_str());//order
//		data[2][i] = atof(vec[10].c_str());//order
//		data[3][i] = atof(vec[18].c_str());//downstream id
//		data[4][i] = atof(vec[34].c_str());//manning's n
//	}
//
//	nr = nAttr;
//	nc = nRows;
//
//	bson_destroy(b);
//	gridfile_destroy(gfile);
//}

// assume the reaches table contains all the reaches information
void ReadMutltiReachInfoFromMongoDB(LayeringMethod layeringMethod, string& dbName, gridfs* spatialData, string& remoteFilename, clsRasterData* templateRaster, int& nr, int& nc, float**& data)
{
	/////////////////////////////////////////////////////
	// build query
	bson b[1];
	bson_init(b);

	bson_append_start_object(b, "$query");
	bson_append_finish_object(b);

	bson_append_start_object(b, "$orderby");
	bson_append_int(b, "SUBBASIN", 1);
	bson_append_finish_object(b);

	bson_finish(b);  
	//////////////////////////////////////////////////////

	mongo_cursor cursor[1];
	bson bsonTable[1];

	ostringstream oss;
	oss << dbName << ".reaches";
	mongo_cursor_init(cursor, spatialData->client, oss.str().c_str());
	mongo_cursor_set_query(cursor, b);

	vector< vector<float> > vecReaches;
	bson_iterator iterator[1];
	float id, upDownOrder, downUpOrder, downStreamID, manning, v0;
	while( mongo_cursor_next(cursor) == MONGO_OK ) 
	{
		const bson *bsonReach = mongo_cursor_bson(cursor);
		if (bson_find(iterator, bsonReach, "SUBBASIN")) 
			id = bson_iterator_int(iterator);
		if (bson_find(iterator, bsonReach, "DOWN_UP_ORDER")) 
			downUpOrder = bson_iterator_int(iterator);
		if (bson_find(iterator, bsonReach, "UP_DOWN_ORDER")) 
			upDownOrder = bson_iterator_int(iterator);
		if (bson_find(iterator, bsonReach, "DOWNSTREAM")) 
			downStreamID = bson_iterator_int(iterator);
		if (bson_find(iterator, bsonReach, "MANNING")) 
			manning = (float)bson_iterator_double(iterator);
		if (bson_find(iterator, bsonReach, "V0")) 
			v0 = (float)bson_iterator_double(iterator);
		vector<float> vec;
		vec.resize(5);
		vec[0] = id;
		if (layeringMethod == UP_DOWN)
			vec[1] = upDownOrder;//order
		else
			vec[1] = downUpOrder;//downstream id
		vec[2] = downStreamID;
		vec[3] = manning;//manning's n
		vec[4] = v0;

		vecReaches.push_back(vec);
	}

	int numRec = vecReaches.size();
	int nAttr = 5;
	data = new float*[nAttr];
	for (int i = 0; i < nAttr; i++)
	{
		data[i] = new float[numRec];
	}
	for (int i = 0; i < numRec; ++i)
	{
		data[0][i] = vecReaches[i][0];
		data[1][i] = vecReaches[i][1];//order
		data[2][i] = vecReaches[i][2];
		data[3][i] = vecReaches[i][3];//manning's n
		data[4][i] = vecReaches[i][4];
	}

	nr = nAttr;
	nc = numRec;

	mongo_cursor_destroy(cursor);
}

void ReadLongTermReachInfo(string& dbName, gridfs* spatialData, int subbasinID, int& nr, int& nc, float**& data)
{
	bson b[1];
	bson_init(b);

	bson_append_start_object(b, "$query");
		bson_append_int(b, "SUBBASIN",  subbasinID);
		bson_append_int(b, "GROUP_DIVIDE",  1);
	bson_append_finish_object(b);

	bson_finish(b);  

	mongo_cursor cursor[1];
	bson bsonTable[1];

	ostringstream oss;
	oss << dbName << ".reaches";
	mongo_cursor_init(cursor, spatialData->client, oss.str().c_str());
	mongo_cursor_set_query(cursor, b);
	mongo_cursor_next(cursor);

	int nReaches = 1;
	int nAttr = 10;
	float **tmpData = new float*[nReaches];
	for (int i = 0; i < nReaches; i++)
	{
		tmpData[i] = new float[nAttr];
	}

	bson_iterator iterator[1];

	int i = 0;
	//while( mongo_cursor_next(cursor) == MONGO_OK ) 
	//{
		const bson *bsonReach = mongo_cursor_bson(cursor);
		if (bson_find(iterator, bsonReach, "SUBBASIN")) 
			tmpData[i][0] = bson_iterator_int(iterator);
		if (bson_find(iterator, bsonReach, "DOWNSTREAM")) 
			tmpData[i][1] = bson_iterator_int(iterator);
		if (bson_find(iterator, bsonReach, "UP_DOWN_ORDER")) 
			tmpData[i][2] = bson_iterator_int(iterator);
		if (bson_find(iterator, bsonReach, "WIDTH")) 
			tmpData[i][3] = bson_iterator_double(iterator);
		if (bson_find(iterator, bsonReach, "LENGTH")) 
			tmpData[i][4] = bson_iterator_double(iterator);
		if (bson_find(iterator, bsonReach, "DEPTH")) 
			tmpData[i][5] = bson_iterator_double(iterator);
		if (bson_find(iterator, bsonReach, "V0")) 
			tmpData[i][6] = bson_iterator_double(iterator);
		if (bson_find(iterator, bsonReach, "AREA")) 
			tmpData[i][7] = bson_iterator_double(iterator);
		if (bson_find(iterator, bsonReach, "MANNING")) 
			tmpData[i][8] = bson_iterator_double(iterator);
		if (bson_find(iterator, bsonReach, "SLOPE")) 
			tmpData[i][9] = bson_iterator_double(iterator);

		i++;
	//}

	data = new float*[nAttr];
	for (int i = 0; i < nAttr; i++)
		data[i] = new float[nReaches+1];

	for (int i = 0; i < nAttr; i++)
	{
		for (int j = 0; j < nReaches; j++)
		{
			data[i][j+1] = tmpData[j][i];
		}
	}

	nr = nAttr;
	nc = nReaches+1;

	mongo_cursor_destroy(cursor);

	for (int i = 0; i < nReaches; i++)
		delete[] tmpData[i];
	delete[] tmpData;
}

// assume the reaches table contains all the reaches information
void ReadLongTermMutltiReachInfo(string& dbName, gridfs* spatialData, int& nr, int& nc, float**& data)
{
	bson b[1];
	bson_init(b);

	bson_append_start_object(b, "$query");
	bson_append_finish_object(b);

	bson_append_start_object(b, "$orderby");
	bson_append_int(b, "SUBBASIN", 1);
	bson_append_finish_object(b);

	bson_finish(b);  

	mongo_cursor cursor[1];
	bson bsonTable[1];

	ostringstream oss;
	oss << dbName << ".reaches";
	mongo_cursor_init(cursor, spatialData->client, oss.str().c_str());

	mongo_cursor_set_query(cursor, b);
	int nReaches = mongo_count(spatialData->client, dbName.c_str(), "reaches", NULL);
	int nAttr = 10;
	float **tmpData = new float*[nReaches];
	for (int i = 0; i < nReaches; i++)
	{
		tmpData[i] = new float[nAttr];
	}

	bson_iterator iterator[1];
	
	int i = 0;
	while( mongo_cursor_next(cursor) == MONGO_OK ) 
	{
		const bson *bsonReach = mongo_cursor_bson(cursor);
		if (bson_find(iterator, bsonReach, "SUBBASIN")) 
			tmpData[i][0] = bson_iterator_int(iterator);
		if (bson_find(iterator, bsonReach, "DOWNSTREAM")) 
			tmpData[i][1] = bson_iterator_int(iterator);
		if (bson_find(iterator, bsonReach, "UP_DOWN_ORDER")) 
			tmpData[i][2] = bson_iterator_int(iterator);
		if (bson_find(iterator, bsonReach, "WIDTH")) 
			tmpData[i][3] = bson_iterator_double(iterator);
		if (bson_find(iterator, bsonReach, "LENGTH")) 
			tmpData[i][4] = bson_iterator_double(iterator);
		if (bson_find(iterator, bsonReach, "DEPTH")) 
			tmpData[i][5] = bson_iterator_double(iterator);
		if (bson_find(iterator, bsonReach, "V0")) 
			tmpData[i][6] = bson_iterator_double(iterator);
		if (bson_find(iterator, bsonReach, "AREA")) 
			tmpData[i][7] = bson_iterator_double(iterator);
		if (bson_find(iterator, bsonReach, "MANNING")) 
			tmpData[i][8] = bson_iterator_double(iterator);
		if (bson_find(iterator, bsonReach, "SLOPE")) 
			tmpData[i][9] = bson_iterator_double(iterator);

		
		i++;
	}

	data = new float*[nAttr];
	for (int i = 0; i < nAttr; i++)
		data[i] = new float[nReaches+1];
	
	for (int i = 0; i < nAttr; i++)
	{
		for (int j = 0; j < nReaches; j++)
		{
			data[i][j+1] = tmpData[j][i];// index of the reach is the ID in the reach table (1 to nReaches)
		}
	}

	nr = nAttr;
	nc = nReaches+1;

	mongo_cursor_destroy(cursor);

	for (int i = 0; i < nReaches; i++)
		delete[] tmpData[i];
	delete[] tmpData;
}

void ReadReachInfoFromMongoDB(LayeringMethod layeringMethod, string& dbName, gridfs* spatialData, int nSubbasin, string remoteFilename, clsRasterData* templateRaster, int& nr, int& nc, float**& data)
{
	bson b[1];
	bson_init(b);
		bson_append_start_object(b, "$query");
		bson_append_int(b, "SUBBASIN",  nSubbasin);
		bson_append_int(b, "GROUP_DIVIDE",  1);
	bson_append_finish_object(b);
	bson_finish(b);  
    //bson_print(b);

	mongo_cursor cursor[1];
	
	ostringstream oss;
	oss << dbName << ".reaches";
	mongo_cursor_init(cursor, spatialData->client, oss.str().c_str());
	mongo_cursor_set_query(cursor, b);
    if (!mongo_cursor_next(cursor) == MONGO_OK)
       throw ModelException("MongoUtil.cpp", "ReadReachInfoFromMongoDB", "Query reach info failed."); 

	int nAttr = 5;
	int nReaches = 1;
	data = new float*[nAttr];
	for (int i = 0; i < nAttr; i++)
	{
		data[i] = new float[nReaches];
	}

	float id, upDownOrder, downUpOrder, downStreamID, manning, v0;
	bson_iterator iterator[1];
	const bson *bsonReach = mongo_cursor_bson(cursor);
    

	if (bson_find(iterator, bsonReach, "SUBBASIN")) 
		id = bson_iterator_int(iterator);
	if (bson_find(iterator, bsonReach, "DOWN_UP_ORDER")) 
		downUpOrder = bson_iterator_int(iterator);
	if (bson_find(iterator, bsonReach, "UP_DOWN_ORDER")) 
		upDownOrder = bson_iterator_int(iterator);
	if (bson_find(iterator, bsonReach, "DOWNSTREAM")) 
		downStreamID = bson_iterator_int(iterator);
	if (bson_find(iterator, bsonReach, "MANNING")) 
		manning = (float)bson_iterator_double(iterator);
	if (bson_find(iterator, bsonReach, "V0")) 
		v0 = (float)bson_iterator_double(iterator);

	for (int i = 0; i < nReaches; ++i)
	{
		data[0][i] = id;
		if (layeringMethod == UP_DOWN)
			data[1][i] = upDownOrder;//order
		else
			data[1][i] = downUpOrder;//downstream id
		data[2][i] = downStreamID;
		data[3][i] = manning;//manning's n
		data[4][i] = v0;
	}

	nr = nAttr;
	nc = nReaches;

	bson_destroy(b);
}
