/*!
 * \file clsRasterData.h
 * \brief Define clsRasterData class
 *
 * 
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date Apr. 2011
 *
 * 
 */
#pragma once

#include <string>
#include <map>
#include "mongo.h"
#include "gridfs.h"

using namespace std;
/*!
 * \ingroup data
 * \class clsRasterData
 *
 * \brief Raster data class to get access to raster data.
 *
 *
 *
 */
class clsRasterData
{
public:
	clsRasterData(string);
	clsRasterData();
	clsRasterData(string,clsRasterData*);
	~clsRasterData(void);

	float getValue(int validCellIndex);
	int Size();
	int getCellWidth();
	int getCellNumber(){return m_nRows;}
	void getRasterData(int*,float**);
	void getRasterPositionData(int*, float ***);
	map<string,float>* getRasterHeader(void);
	int getASCRows();
	int getASCCols();
	float getAverage();
	float getNoDataValue(){return m_headers["NODATA_VALUE"];}
	float getXllCenter(){return m_headers["XLLCENTER"];}
	float getYllCenter() {return m_headers["YLLCENTER"];}
	void outputAAIGrid(string& filename);
	static void outputASCFile(map<string,float>,int,float**, float*,string);
	static void outputASCFile(clsRasterData*, float*,string);
	static void outputGTiff(clsRasterData* templateRasterData, float* value,string& rasterName);
	static void outputGTiff(map<string,float> header,int nValidCells,float** position, float* value,string filename);
	static void outputToMongoDB(clsRasterData* templateRasterData, float* value, string filename, gridfs* gfs);
	static void outputToMongoDB(map<string,float> header, int nValid, float** position, float* value,string remoteFilename, gridfs* gfs);
	static void outputWeightFile(clsRasterData*, int, float ,string);
	static void getASCHeaders(string databasePath,map<string,float>* headers);

	//2011-4-11

	//! Get value corresponding to specific row and column
	static float getValue(clsRasterData* templateRasterData, float* rasterData, int row, int col);
	
	//! Get position in 1-d raster data array for specific row and column
	int getPosition(int row,int col);
	int getPosition(float x,float y);
	//! Get raster data pointer
	float* getRasterDataPointer()
	{
		return m_rasterData;
	}
	//! Constructor for using mongodb
	clsRasterData(gridfs* gfs, const char* remoteFilename, clsRasterData* templateRaster = NULL);
	
private:
	string m_fileName;
	float* m_rasterData;
	float** m_rasterPositionData;
	int m_nRows;
	map<string,float> m_headers;

	clsRasterData* m_mask;
public:
	void readASCFile(string);
	void readASCFile(string,clsRasterData* mask);
	void ReadFromGDAL(string filename);
	void ReadFromGDAL(string ascFileName,clsRasterData* mask);
	int ReadFromMongoDB(gridfs *gfs, const char* remoteFilename);
	int ReadFromMongoDB(gridfs *gfs, const char* remoteFilename,clsRasterData* mask);
};

