/*!
 * \file clsRasterData.h
 * \ingroup data
 * \brief Define Raster class to handle raster data
 *
 * Using GDAL and MongoDB (currently, mongo-c-driver 1.3.5)
 *
 * \author Junzhi Liu, LiangJun Zhu
 * \version 2.0
 * \date Apr. 2011
 * \revised May. 2016
 * 
 */
#pragma once

#include <string>
#include <map>
#include "util.h"
#include "mongoc.h"
#include "MongoUtil.h"

using namespace std;
/*!
 * \ingroup data
 * \class clsRasterData
 *
 * \brief Raster data class to get access to raster data in MongoDB.
 */
class clsRasterData
{
public:
	/*!
	 * \brief Constructor of clsRasterData instance from ASCII file
	 *
	 * \sa ReadASCFile()
	 *
	 * \param[in] ascFileName ASCII file path
	 */
	clsRasterData(string);
	/*!
	 * \brief Constructor an empty clsRasterData instance
	 * Set \a m_rasterPositionData, \a m_rasterData, \a m_mask to \a NULL
	 */
	clsRasterData();
	/*!
	 * \brief Constructor of clsRasterData instance from  ASCII file and mask clsRasterData
	 *
	 * \sa ReadASCFile()
	 * \param[in] ascFileName \a string
	 * \param[in] mask \a clsRasterData
	 */
	clsRasterData(string,clsRasterData*);
	/*!
	 * \brief Constructor of clsRasterData instance from mongoDB
	 *
	 * \sa ReadFromMongoDB()
	 *
	 * \param[in] gfs \a mongoc_gridfs_t
	 * \param[in] romoteFilename \a char*
	 * \param[in] templateRaster \a clsRasterData, NULL as default
	 */
	clsRasterData(mongoc_gridfs_t* gfs, const char* remoteFilename, clsRasterData* templateRaster = NULL);
	//! Destructor, Set \a m_rasterPositionData, \a m_rasterData, \a m_mask to \a NULL
	~clsRasterData(void);
	//! Get the average of raster data
	float				getAverage();
	//! Get column number of raster data
	int					getCols(){return m_headers[HEADER_RS_NCOLS];}
	//! Get row number of raster data
	int					getRows(){return m_headers[HEADER_RS_NROWS];}
	//! Get cell size of raster data
	int					getCellWidth(){return m_headers[HEADER_RS_CELLSIZE];}
	//! Get cell numbers ignore NoData
	int					getCellNumber(){return m_nRows;}
	//! Get NoDATA value of raster data
	float				getNoDataValue(){return m_headers[HEADER_RS_NODATA];}
	//! Get position index in 1D raster data for specific row and column, return -1 is error occurs. 
	int					getPosition(int row,int col);
	//! Get position index in 1D raster data for given coordinate (x,y)
	int					getPosition(float x,float y);
	//! Get raster data, include valid cell number and data
	void				getRasterData(int*,float**);
	//! Get raster header information
	map<string,float>*	getRasterHeader(void);
	//! Get non-NODATA position index data, include cell number and (row, col)
	void				getRasterPositionData(int*, float ***);
	//! Get pointer of raster data 
	float*				getRasterDataPointer(){return m_rasterData;}
	//! Get raster data at the valid cell index, NoData if index cross the border
	float				getValue(int validCellIndex);
	//! Get raster data at the row and col, NoData if index cross the border
	float				getValue(int row, int col);
	//! Get raster data value at row and column of \a templateRasterData and \a rasterData
	static float		getValue(clsRasterData* templateRasterData, float* rasterData, int row, int col);
	//! Get X coordinate of left lower corner of raster data
	float				getXllCenter(){return m_headers[HEADER_RS_XLL];}
	//! Get Y coordinate of left lower corner of raster data
	float				getYllCenter(){return m_headers[HEADER_RS_YLL];}
	//! Write raster to ASC Grid file
	void				outputASCFile(string& filename);
	/*!
	 * \brief Write raster data into ASC file
	 *
	 * \param[in] header header information
	 * \param[in] nRows \a int, valid cell number
	 * \param[in] position \a float**, position index
	 * \param[in] value \a float*, Raster data
	 * \param[in] filename \a string, output ASC file path
	 */
	static void			outputASCFile(map<string,float>,int,float**, float*,string);
	/*!
	 * \brief Write raster data into ASC file
	 *
	 * \param[in] templateRasterData
	 * \param[in] value \a float*, Raster data
	 * \param[in] filename \a string, output ASC file path
	 */
	static void			outputASCFile(clsRasterData*, float*,string);
	/*!
	 * \brief Write raster data into GTIFF file
	 *
	 * \param[in] templateRasterData
	 * \param[in] value \a float*, Raster data
	 * \param[in] rasterName \a string, output GTIFF file path
	 */
	static void			outputGTiff(clsRasterData* templateRasterData, float* value,string& rasterName);
	/*!
	 * \brief Write raster data into GTIFF file
	 *
	 * \param[in] header header information
	 * \param[in] nValidCells \a int, valid cell number
	 * \param[in] position \a float**, position index
	 * \param[in] value \a float*, Raster data
	 * \param[in] filename \a string, output GTIFF file path
	 */
	static void			outputGTiff(map<string,float> header,int nValidCells,float** position, float* value,string filename);
	/*!
	 * \brief Write raster data into GTIFF file
	 *
	 * \param[in] header header information
	 * \param[in] nValidCells \a int, valid cell number
	 * \param[in] position \a float**, position index
	 * \param[in] filename \a string, output file name
	 * \param[in] gfs \a mongoc_gridfs_t
	 */
	static void			outputToMongoDB(map<string,float> header, int nValid, float** position, float* value,string remoteFilename, mongoc_gridfs_t* gfs);
	/*!
	 * \brief Write raster data into MongoDB
	 *
	 * \param[in] templateRasterData \a clsRasterData
	 * \param[in] value \a float*, Raster data
	 * \param[in] filename \a string, output file name
	 * \param[in] gfs \a mongoc_gridfs_t
	 */
	static void			outputToMongoDB(clsRasterData* templateRasterData, float* value, string filename, mongoc_gridfs_t* gfs);
	/*!
	 * \brief Write weight file according the weight value
	 * \param[in] templateRasterData \a clsRasterData
	 * \param[in] nCols \a int DO NOT KNOW HOW TO USE THIS FUNCTION? LJ
	 * \param[in] weight \a float
	 * \param[in] filename \a char*, weight file name
	 */
	static void			outputWeightFile(clsRasterData*, int, float ,string);
	/*!
	 * \brief Read raster data from ASC file
	 * \param[in] ascFileName \a string
	 */
	void				ReadASCFile(string);
	/*!
	 * \brief Read raster data from ASC file, using mask
	 * Be aware, this mask should have the same extent with the raster
	 * i.e., NROWS and NCOLS are the same!
	 * \param[in] ascFileName \a string
	 * \param[in] mask \a clsRasterData
	 */
	void				ReadASCFile(string,clsRasterData* mask);
	/*!
	 * \brief Read raster data using GDAL
	 * 
	 * \param[in] filename \a string
	 */
	void				ReadFromGDAL(string filename);
	/*!
	 * \brief Read raster data according the the given mask file using GDAL
	 * Be aware, this mask should have the same extent with the raster
	 * i.e., NROWS and NCOLS are the same!
	 * \param[in] filename \a string
	 * \param[in] mask \a clsRasterData
	 */
	void				ReadFromGDAL(string filename,clsRasterData* mask);
	/*!
	 * \brief Read raster data from MongoDB
	 * \param[in] gfs \a mongoc_gridfs_t
	 * \param[in] remoteFilename \a char*, raster file name
	 */
	int					ReadFromMongoDB(mongoc_gridfs_t *gfs, const char* remoteFilename);
	/*!
	 * \brief Read raster data according the the given mask file from MongoDB
	 * Be aware, this mask should have the same extent with the raster
	 * i.e., NROWS and NCOLS are the same!
	 * \param[in] gfs \a mongoc_gridfs_t
	 * \param[in] remoteFilename \a char*, raster file name
	 * \param[in] mask \a clsRasterData
	 */
	int					ReadFromMongoDB(mongoc_gridfs_t *gfs, const char* remoteFilename,clsRasterData* mask);
	/*!
	 * \brief Get cell number
	 * \sa getCellNumber()
	 */
	int					Size(){return m_nRows;}

private:
	///< raster file name
	string m_fileName;
	///< raster data (1D array)
	float* m_rasterData;
	///< cell index (row, col) in m_rasterData (2D array)
	float** m_rasterPositionData;
	///< cell number of raster data (exclude NODATA_VALUE)
	int m_nRows;
	///< Header information
	map<string,float> m_headers;
	///< mask clsRasterData instance
	clsRasterData* m_mask;	
};

