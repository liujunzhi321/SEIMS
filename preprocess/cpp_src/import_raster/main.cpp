#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <cmath>
#include <sstream>
#include <fstream>
#include <cstdlib>

//gdal
#include "gdal.h"
#include "gdal_priv.h"
#include "cpl_string.h"

//mongodb
#include "mongo.h"
#include "bson.h"
#include "gridfs.h"

//this project
#include "util.h"
#include "SubBasin.h"
#include "Raster.cpp"

using namespace std;



int FindBoundingBox(Raster<int>& rsSubbasin, map<int, SubBasin>& bboxMap)
{
	int nXSize = rsSubbasin.GetNumberofColumns();
	int nYSize = rsSubbasin.GetNumberOfRows();

	int** pData = rsSubbasin.GetData();
	int noDataValue = rsSubbasin.GetNoDataValue();
	// find bounding box for each subbasin
	for (int i = 0; i < nYSize; i++)
	{
		for (int j = 0; j < nXSize; j++)
		{
			int id = pData[i][j];
			if (noDataValue != id)
			{
				if (bboxMap.find(id) == bboxMap.end())
				{
					bboxMap[id] = SubBasin(j, i, j, i);
				}
				else
				{
					if (j < bboxMap[id].xMin)
						bboxMap[id].xMin = j;
					else if (j > bboxMap[id].xMax)
						bboxMap[id].xMax = j;

					if (i > bboxMap[id].yMax)
						bboxMap[id].yMax = i;
				}
				bboxMap[id].cellCount += 1;
			}
		}
	}
	return 0;
}


int DecompositeRasterToMongoDB(map<int, SubBasin>& bboxMap, Raster<int>& rsSubbasin, const char* dstFile, mongo* conn, gridfs* gfs)
{
	Raster<float> rs;
	rs.ReadFromGDAL(dstFile);

	int nXSize = rs.GetNumberofColumns();
	int nYSize = rs.GetNumberOfRows();
	float noDataValue = rs.GetNoDataValue();

    //cout << nXSize << "\t" << nYSize << endl;
	float **rsData = rs.GetData();
	int **subbasinData = rsSubbasin.GetData();

	map<int, SubBasin>::iterator it;
	string coreName = GetCoreFileName(dstFile);

	for (it = bboxMap.begin(); it != bboxMap.end(); it++)
	{
		int id = it->first;
		SubBasin& subbasin = it->second;
		int subXSize = subbasin.xMax - subbasin.xMin + 1;
		int subYSize = subbasin.yMax - subbasin.yMin + 1;

		float *subData = new float[subXSize*subYSize];

		for (int i = subbasin.yMin; i <= subbasin.yMax; i++)
		{
			for (int j = subbasin.xMin; j <= subbasin.xMax; j++)
			{
				int index = i*nXSize + j;
				int subIndex = (i - subbasin.yMin) * subXSize + (j - subbasin.xMin);
				if (subbasinData[i][j] == id)
					subData[subIndex] = rsData[i][j];
				else
					subData[subIndex] = noDataValue;
			}
		}

		ostringstream remoteFilename;
		remoteFilename << id << "_" << GetUpper(coreName);
		float cellSize = rs.GetXCellSize();

		bson *p = (bson*)malloc(sizeof(bson));
		bson_init(p);
		bson_append_int(p, "SUBBASIN", id );
		bson_append_string(p, "TYPE", coreName.c_str());
		bson_append_string(p, "ID", remoteFilename.str().c_str());
		bson_append_string(p, "DESCRIPTION", coreName.c_str());
		bson_append_double(p, "CELLSIZE", cellSize);
		bson_append_double(p, "NODATA_VALUE", rs.GetNoDataValue());
		bson_append_double(p, "NCOLS", subXSize);
		bson_append_double(p, "NROWS", subYSize);
		bson_append_double(p, "XLLCENTER", rs.GetXllCenter() + subbasin.xMin * cellSize);
		bson_append_double(p, "YLLCENTER", rs.GetYllCenter() + (rs.GetNumberOfRows() - subbasin.yMax - 1) * cellSize);
		bson_finish(p);
		
		gridfile gfile[1];
		gridfile_writer_init(gfile, gfs, remoteFilename.str().c_str(), "float");
		for (int k = 0; k < subYSize; k++)
		{
			gridfile_write_buffer(gfile, (const char*)(subData+subXSize*k), sizeof(float)*subXSize);
		}
		gridfile_set_metadata(gfile, p);
		gridfile_writer_done(gfile);
		gridfile_destroy(gfile);
		
		//bson_destroy(con);
		//free(con);
		bson_destroy(p);
		free(p);

		
		delete subData;
	}
	return 0;
}



int DecompositeRaster(map<int, SubBasin>& bboxMap, Raster<int>& rsSubbasin, const char* dstFile, const char* tmpFolder)
{
	Raster<float> rs;
	rs.ReadFromGDAL(dstFile);

	int nXSize = rs.GetNumberofColumns();
	int nYSize = rs.GetNumberOfRows();
	float noDataValue = rs.GetNoDataValue();

	float **rsData = rs.GetData();
	int **subbasinData = rsSubbasin.GetData();

	const char *pszFormat = "GTiff";
	GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);

	map<int, SubBasin>::iterator it;
	string coreName = GetCoreFileName(dstFile);
	for (it = bboxMap.begin(); it != bboxMap.end(); it++)
	{
		int id = it->first;
		SubBasin& subbasin = it->second;
		int subXSize = subbasin.xMax - subbasin.xMin + 1;
		int subYSize = subbasin.yMax - subbasin.yMin + 1;

		ostringstream oss;
		oss << tmpFolder << "/" << id << "/" << GetUpper(coreName) << ".tif";
		string subbasinFile = oss.str();
        //cout << subbasinFile << endl;
		// create a new raster for the subbasin
		char **papszOptions = NULL;
		GDALDataset *poDstDS = poDriver->Create(subbasinFile.c_str(), subXSize, subYSize, 1, GDT_Float32, papszOptions );

		float *subData = (float *) CPLMalloc(sizeof(float)*subXSize*subYSize);

		for (int i = subbasin.yMin; i <= subbasin.yMax; i++)
		{
			for (int j = subbasin.xMin; j <= subbasin.xMax; j++)
			{
				int index = i*nXSize + j;
				int subIndex = (i - subbasin.yMin) * subXSize + (j - subbasin.xMin);
				if (subbasinData[i][j] == id)
					subData[subIndex] = rsData[i][j];
				else
					subData[subIndex] = noDataValue;
			}
		}

		//write the data to new file
		GDALRasterBand  *poDstBand= poDstDS->GetRasterBand(1);
		poDstBand->RasterIO(GF_Write, 0, 0, subXSize, subYSize, 
			subData, subXSize, subYSize, GDT_Float32, 0, 0);
		poDstBand->SetNoDataValue(noDataValue);

		double geoTrans[6];
		float cellSize = rs.GetXCellSize();
		geoTrans[0] = rs.GetXllCenter() + (subbasin.xMin - 0.5f) * cellSize;
		geoTrans[1] = cellSize;
		geoTrans[2] = 0;
		geoTrans[3] = rs.GetYllCenter() + (rs.GetNumberOfRows() - subbasin.yMin - 0.5f) * cellSize;
		geoTrans[4] = 0;
		geoTrans[5] = -cellSize;
		poDstDS->SetGeoTransform(geoTrans);

		CPLFree(subData);
		GDALClose(poDstDS);
		
		// import the subbasin file to BeyondDB
		string desc = "";
		//ImportToBeyondDB(subbasinFile, id, coreName, subbasin.cellCount, subXSize, subYSize);
	}
	return 0;
}

int main(int argc, char** argv)
{
	if (argc < 6)
	{
		cout << "Usage: " << "ImportRaster SubbasinFile DataFolder modelName hostname port [outputFolder]\n";
		exit(-1);
	}

	GDALAllRegister();

	//const char* subbasinFile = "F:\\modeldev\\integrated\\storm_model\\Debug\\model_lyg_10m\\mask.asc";
	//const char* folder = "F:\\modeldev\\integrated\\storm_model\\Debug\\model_lyg_10m";
	//const char* modelName = "model_lyg_10m";
	//const char* hostname = "192.168.5.195";
	const char* subbasinFile = argv[1];
	const char* folder = argv[2];
	const char* modelName = argv[3];
	const char* hostname = argv[4];
	int port = atoi(argv[5]);
    const char* outTifFolder = NULL;
    if(argc >= 7)
        outTifFolder = argv[6];

	vector<string> dstFiles;
	FindFiles(folder, "*.tif", dstFiles);

	//////////////////////////////////////////////////////////////////////////
	// read the subbasin file, and find the bounding box of each subbasin
	Raster<int> rsSubbasin;
	rsSubbasin.ReadFromGDAL(subbasinFile);
	map<int, SubBasin> bboxMap;
	FindBoundingBox(rsSubbasin, bboxMap);
	
	//////////////////////////////////////////////////////////////////////////
	// loop to precess the destination files
	
	// connect to mongodb
	mongo conn[1];
	gridfs gfs[1];
	int status = mongo_connect(conn, hostname, port); 
	
	if( MONGO_OK != status ) 
	{ 
		cout << "can not connect to mongodb.\n";
		exit(-1);
	}
	gridfs_init(conn, modelName, "spatial", gfs); 

    cout << "Importing spatial data to MongoDB...\n";
	for (size_t i = 0; i < dstFiles.size(); ++i)
	{
        cout << "\t" << dstFiles[i] << endl;
        if(outTifFolder != NULL)
            DecompositeRaster(bboxMap, rsSubbasin, dstFiles[i].c_str(), outTifFolder);
		DecompositeRasterToMongoDB(bboxMap, rsSubbasin, dstFiles[i].c_str(), conn, gfs);
	}

	gridfs_destroy(gfs);
	mongo_destroy(conn);
}

