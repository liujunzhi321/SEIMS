/*!
 * \file clsRasterData.cpp
 * \brief methods for clsRasterData class
 *
 * 
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date June 2010
 *
 * 
 */
#include "clsRasterData.h"
#include <fstream>
#include <vector>
#include "utils.h"
#include "util.h"
#include "ModelException.h"
#include "text.h"
#include "DBManager.h"

//gdal
#include "gdal.h"
#include "gdal_priv.h"
#include "cpl_string.h"
#include <iomanip>
#include "ogr_spatialref.h"
/*!
 * \brief Constructor of clsRasterData instance from ascFile
 *
 * \sa readASCFile()
 *
 * \param[in] ascFileName
 */
clsRasterData::clsRasterData(string ascFileName)
{
	m_rasterPositionData = NULL;
	m_rasterData = NULL;
	m_mask = NULL;
	m_fileName = ascFileName;
	readASCFile(ascFileName);
}
//! set \a m_rasterPositionData, \a m_rasterData, \a m_mask to NULL
clsRasterData::clsRasterData()
{
	m_rasterPositionData = NULL;
	m_rasterData = NULL;
	m_mask = NULL;
}
/*!
 * \brief Constructor of clsRasterData instance from mongoDB
 *
 * \sa ReadFromMongoDB()
 *
 * \param[in] gfs, romoteFilename, templateRaster
 */
clsRasterData::clsRasterData(gridfs* gfs, const char* remoteFilename, clsRasterData* templateRaster)
{
	m_rasterPositionData = NULL;
	m_rasterData = NULL;
	m_mask = NULL;
	m_fileName = "";
	ReadFromMongoDB(gfs, remoteFilename, templateRaster);
}
/*!
 * \brief Constructor of clsRasterData instance from ascFile and mask
 *
 * \sa readASCFile()
 *
 * \param[in] ascFileName, mask
 */
clsRasterData::clsRasterData(string ascFileName,clsRasterData* mask)
{
	m_rasterPositionData = NULL;
	m_rasterData = NULL;
	m_mask = mask;
	m_fileName = ascFileName;
	readASCFile(ascFileName,mask);
}
//! Destructor
clsRasterData::~clsRasterData(void)
{
	if(m_rasterData!=NULL) delete [] m_rasterData;

	if (m_rasterPositionData != NULL && m_mask == NULL)
	{
		for (int i = 0; i < m_nRows; ++i)
		{
			if (m_rasterPositionData[i] != NULL)
				delete [] m_rasterPositionData[i];
				
		}
		delete [] m_rasterPositionData;
	}
}
//! Read raster data from MongoDB
int clsRasterData::ReadFromMongoDB(gridfs *gfs, const char* remoteFilename)
{
	gridfile gfile[1];
	bson b[1];
	bson_init(b);
	bson_append_string(b, "filename",  remoteFilename);
	bson_finish(b);  
	int flag = gridfs_find_query(gfs, b, gfile); 
	if(0 != flag)
	{  
		throw ModelException("clsRasterData", "ReadFromMongoDB", "The file " + string(remoteFilename) + " does not exist.");
	}

	size_t length = (size_t)gridfile_get_contentlength(gfile);
	char* buf = (char*)malloc(length);
	gridfile_read (gfile, length, buf);
	float *data = (float*)buf;

	bson bmeta[1];
	gridfile_get_metadata(gfile, bmeta);
	bson_iterator iterator[1];
	if ( bson_find( iterator, bmeta, "NCOLS" )) 
		m_headers["NCOLS"] = (float)bson_iterator_int(iterator);
	if ( bson_find( iterator, bmeta, "NROWS" )) 
		m_headers["NROWS"] = (float)bson_iterator_int(iterator);
	if ( bson_find( iterator, bmeta, "NODATA_VALUE" )) 
		m_headers["NODATA_VALUE"] = (float)bson_iterator_double(iterator);
	if ( bson_find( iterator, bmeta, "XLLCENTER" )) 
		m_headers["XLLCENTER"] = (float)bson_iterator_double(iterator);
	if ( bson_find( iterator, bmeta, "YLLCENTER" )) 
		m_headers["YLLCENTER"] = (float)bson_iterator_double(iterator);
	if ( bson_find( iterator, bmeta, "CELLSIZE" )) 
	{
		m_headers["CELLSIZE"] = (float)bson_iterator_double(iterator);
		//m_headers["DY"] = m_headers["DX"];
	}
	
	int nRows = (int)m_headers["NROWS"];
	int nCols = (int)m_headers["NCOLS"];

	vector<float> values;
	vector<int> positionRows;
	vector<int> positionCols;
	//get all valid values
	float nodataFloat = m_headers["NODATA_VALUE"];
	for (int i = 0; i < nRows; ++i)
	{
		for (int j = 0; j < nCols; ++j)
		{
			int index = i*nCols + j;
			float value = data[index];
			if(FloatEqual(nodataFloat, value)) 
				continue;
			values.push_back(value);
			positionRows.push_back(i);
			positionCols.push_back(j);
		}
	}

	//create float array
	m_nRows = values.size();
	m_rasterData = new float[m_nRows];
	m_rasterPositionData = new float*[m_nRows];
	for (int i = 0; i < m_nRows; ++i)
	{
		m_rasterData[i] = values.at(i);
		m_rasterPositionData[i] = new float[2];
		m_rasterPositionData[i][0] = float(positionRows.at(i));
		m_rasterPositionData[i][1] = float(positionCols.at(i));
	}

	bson_destroy(b);
	gridfile_destroy(gfile);

	free(buf);

	return 0;
}

//! Read raster data from MongoDB and select by mask
int clsRasterData::ReadFromMongoDB(gridfs *gfs, const char* remoteFilename,clsRasterData* mask)
{
	if(mask == NULL) ReadFromMongoDB(gfs, remoteFilename);
	else
	{
		//clock_t start = clock();
		gridfile gfile[1];
		bson b[1];
		bson_init(b);
		bson_append_string(b, "filename",  remoteFilename);
		bson_finish(b);  
		int flag = gridfs_find_query(gfs, b, gfile); 
		if(0 != flag)
		{  
			//cout << "Failed in ReadFromMongoDB, Remote file: " << remoteFilename << endl;
			return -1;
		}

		size_t length = (size_t)gridfile_get_contentlength(gfile);
		char* buf = (char*)malloc(length);
		gridfile_read (gfile, length, buf);
		float *data = (float*)buf;

		//clock_t end = clock();
		//cout << "Read data: " << end - start << endl;

		//start = clock();

		//get the data posotion from mask
		int nRows;
		float** validPosition;
		mask->getRasterPositionData(&nRows,&validPosition);		
		m_nRows = nRows;

		m_mask = mask;
		//set header
		m_headers["NCOLS"] = mask->getASCCols();
		m_headers["NROWS"] = mask->getASCRows();
		m_headers["NODATA_VALUE"] = mask->getNoDataValue();
		m_headers["CELLSIZE"] = mask->getCellWidth();
		//m_headers["DY"] = mask->getCellWidth();
		m_headers["XLLCENTER"] = mask->getXllCenter();
		m_headers["YLLCENTER"] = mask->getYllCenter();

		//read data
		m_rasterData = new float[nRows];
		int ascCols = mask->getASCCols();
		int ascRows = mask->getASCRows();

		//int index = 0;
		//for (int i = 0; i < ascRows; ++i)
		//{
		//	for (int j = 0; j < ascCols; ++j)
		//	{
		//		float value = data[i*ascCols + j];
		//		if(index < nRows)
		//		{
		//			if(validPosition[index][0] == i && validPosition[index][1] == j) 
		//			{
		//				m_rasterData[index] = value;
		//				index++;
		//			}

		//		}
		//	}
		//}

		for(int index = 0; index < nRows; index++)
		{
			int i = validPosition[index][0];
			int j = validPosition[index][1];
			int rasterIndex = i*ascCols + j;
			m_rasterData[index] = data[rasterIndex];
		}

		//end = clock();
		//cout << "Rearrange data: " << end-start << endl;

		bson_destroy(b);
		gridfile_destroy(gfile);

		free(buf);
	}
	return 0;
}
//! Get Ascii file headers information
void clsRasterData::getASCHeaders(string databasePath,map<string,float>* headers)
{
	DBManager dbman;
	string sql;
	slTable* tbl;

	// open the database
	dbman.Open(databasePath + File_ParameterDB);
	// if there is not an error
	if(dbman.IsError()) throw ModelException("clsRasterData","getASCHeaders","Can't open paramter database!");

	// constrcut the SQL statement for the query
	sql = "SELECT Parameter,Value FROM Header";
	// run the query
	tbl = dbman.Load(sql);
	if (tbl->nRows == 0) throw ModelException("ModuleParamter","getParameterFromDatabase","Can't find ASC Headers in paramter database!");
	
	headers->clear();

    //headers = new map<string,float>();

	for(int i=1;i<=tbl->nRows;i++)
	{
		(*headers)[tbl->FieldValue(i, 1)] = float(atof(tbl->FieldValue(i, 2).c_str()));
	}	

	delete tbl;
	tbl = NULL;

	dbman.Close();
}

void clsRasterData::outputASCFile(clsRasterData* templateRasterData, float* value,string filename)
{
	int nRows;
	float** position; 
	templateRasterData->getRasterPositionData(&nRows,&position);
	clsRasterData::outputASCFile(*(templateRasterData->getRasterHeader()),nRows,position,value,filename);
}

void clsRasterData::outputGTiff(clsRasterData* templateRasterData, float* value,string& rasterName)
{
	int nRows;
	float** position; 
	templateRasterData->getRasterPositionData(&nRows,&position);
	clsRasterData::outputGTiff(*(templateRasterData->getRasterHeader()),nRows,position,value,rasterName);
}


void clsRasterData::outputToMongoDB(clsRasterData* templateRasterData, float* value, string filename, gridfs* gfs)
{
	int nRows;
	float** position; 
	templateRasterData->getRasterPositionData(&nRows,&position);
	clsRasterData::outputToMongoDB(*(templateRasterData->getRasterHeader()), nRows, position, value, filename, gfs);
}

void clsRasterData::outputAAIGrid(string& filename)
{
	clsRasterData::outputASCFile(m_headers, m_nRows, m_rasterPositionData, m_rasterData, filename);
}

void clsRasterData::outputWeightFile(clsRasterData* templateRasterData,int nCols, float weight,string filename)
{
	int nRows;
	float** position; 
	templateRasterData->getRasterPositionData(&nRows,&position);

	ofstream rasterFile(filename.c_str());
	//write header
	rasterFile << nRows << "\n";
	rasterFile << nCols << "\n";	

	//write file
	int rows = nRows;
	int cols = nCols;
	for (int i = 0; i < rows; ++i)
	{
		//rasterFile << position[i][0] << " ";	
		//rasterFile << position[i][1] << " ";	
		for (int j = 0; j < cols; ++j)
		{
			rasterFile << weight << " ";	
		}
		rasterFile << "\n";
	}

	rasterFile.close();
}

void clsRasterData::outputToMongoDB(map<string,float> header, int nValid, float** position, float* value,string remoteFilename, gridfs* gfs)
{
	float noData = -9999.0f;
	// prepare binary data
	int rows = int(header["NROWS"]);
	int cols = int(header["NCOLS"]);
	float *data = new float[rows*cols];
	
	int index = 0;
	int dataIndex = 0;
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			dataIndex = i*cols + j;
			if(index < nValid)
			{
				if(position[index][0] == i && position[index][1] == j) 
				{
					data[dataIndex] = value[index];	
					index++;
				}
				else 
					data[dataIndex] = noData;
			}
			else 
				data[dataIndex] = noData;				
		}
	}
	
	bson *p = (bson*)malloc(sizeof(bson));
	bson_init(p);
	bson_append_string(p, "ID", remoteFilename.c_str());
	bson_append_double(p, "CELLSIZE", header["CELLSIZE"]);
	bson_append_double(p, "NODATA_VALUE", noData);
	bson_append_double(p, "NCOLS", cols);
	bson_append_double(p, "NROWS", rows);
	bson_append_double(p, "XLLCENTER", header["XLLCENTER"]);
	bson_append_double(p, "YLLCENTER", header["YLLCENTER"]);
	bson_finish(p);
		
	gridfile gfile[1];
	gridfile_writer_init(gfile, gfs, remoteFilename.c_str(), "float");

	size_t iID = remoteFilename.find_first_of('_'); 
	int subbasinID = atoi(remoteFilename.substr(0, iID).c_str());
	gfile->id.ints[0] = subbasinID;
	
	for (int k = 0; k < rows; k++)
	{
		gridfile_write_buffer(gfile, (const char*)(data+cols*k), sizeof(float)*cols);
	}
	gridfile_set_metadata(gfile, p);
	int response = gridfile_writer_done(gfile);
	//cout << remoteFilename << "\t" << gfile->id.ints[0] <<  gfile->id.ints[1] << gfile->id.ints[2] << endl;

	gridfile_destroy(gfile);	
	bson_destroy(p);
	free(p);
	
	delete data;
}

void clsRasterData::outputGTiff(map<string,float> header,int nValidCells,float** position, float* value,string filename)
{
	float noDataValue = header["NODATA_VALUE"];

	int nCols = header["NCOLS"];
	int nRows = header["NROWS"];
	float xll = header["XLLCENTER"];
	float yll = header["YLLCENTER"];
	float dx = header["CELLSIZE"];

	int n = nRows * nCols;
	float *data = new float[n];

	int index = 0;
	for (int i = 0; i < nRows; ++i)
	{
		for (int j = 0; j < nCols; ++j)
		{
			if(index < nValidCells)
			{
				if(position[index][0] == i && position[index][1] == j) 
				{
					data[i*nCols+j] = value[index];	
					index++;
				}
				else 
					data[i*nCols+j] = noDataValue;
			}
			else 
				data[i*nCols+j] = noDataValue;				
		}
	}

	const char *pszFormat = "GTiff";
	GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);

	char **papszOptions = poDriver->GetMetadata();
	GDALDataset *poDstDS = poDriver->Create(filename.c_str(), nCols, nRows, 1, GDT_Float32, papszOptions );
	
	//write the data to new file
	GDALRasterBand  *poDstBand= poDstDS->GetRasterBand(1);
	poDstBand->RasterIO(GF_Write, 0, 0,  nCols, nRows, data,  nCols, nRows, GDT_Float32, 0, 0);
	poDstBand->SetNoDataValue(noDataValue);

	double geoTrans[6];
	geoTrans[0] = xll;
	geoTrans[1] = dx;
	geoTrans[2] = 0;
	geoTrans[3] = yll + nRows*dx;
	geoTrans[4] = 0;
	geoTrans[5] = -dx;
	poDstDS->SetGeoTransform(geoTrans);

	OGRSpatialReference srs;
	srs.SetACEA(25, 47, 0, 105, 0, 0);
	srs.SetWellKnownGeogCS("WGS84");
	
	char *pSrsWkt = NULL;
	srs.exportToWkt(&pSrsWkt);
	poDstDS->SetProjection(pSrsWkt);
	CPLFree(pSrsWkt);

	GDALClose(poDstDS);

	delete[] data;
}

void clsRasterData::outputASCFile(map<string,float> header,int nRows,float** position, float* value,string filename)
{
	float noData = -9999.0f;

	ofstream rasterFile(filename.c_str());
	//write header
	rasterFile << "NCOLS " << header["NCOLS"] << "\n";
	rasterFile << "NROWS " << header["NROWS"] << "\n";
	rasterFile << "XLLCENTER " << header["XLLCENTER"] << "\n";
	rasterFile << "YLLCENTER " << header["YLLCENTER"] << "\n";
	rasterFile << "CELLSIZE " << header["CELLSIZE"] << "\n";
	//rasterFile << "DY " << header["DY"] << "\n";
	rasterFile << "NODATA_VALUE "  << setprecision(6) << noData << "\n";

	//write file
	int rows = int(header["NROWS"]);
	int cols = int(header["NCOLS"]);
	
	int index = 0;
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			if(index < nRows)
			{
				if(position[index][0] == i && position[index][1] == j) 
				{
					rasterFile << setprecision(6) << value[index] << " ";	
					index++;
				}
				else rasterFile << setprecision(6) << noData << " ";
			}
			else rasterFile << setprecision(6) << noData << " ";				
		}
		rasterFile << "\n";
	}

	rasterFile.close();
}

void clsRasterData::getRasterData(int* nRows,float** data)
{
	*nRows = m_nRows;
	*data = m_rasterData;
}
void clsRasterData::getRasterPositionData(int* nRows, float ***data)
{
	if(m_mask != NULL) 
		m_mask->getRasterPositionData(nRows,data);
	else
	{
		*nRows = m_nRows;
		*data = m_rasterPositionData;
	}
}

map<string,float>* clsRasterData::getRasterHeader()
{
	if(m_mask != NULL) return m_mask->getRasterHeader();
	return &m_headers;
}

int clsRasterData::getASCCols()
{
	map<string,float>* header =  getRasterHeader();
	return int((*header)["NCOLS"]);
}

int clsRasterData::Size()
{
	return m_nRows;
}

int clsRasterData::getCellWidth()
{
	map<string,float>* header =  getRasterHeader();
	return int((*header)["CELLSIZE"]);
}

int clsRasterData::getASCRows()
{
	map<string,float>* header =  getRasterHeader();
	return int((*header)["NROWS"]);
}

float clsRasterData::getAverage()
{
	float temp = 0.0f;
	for(int i = 0;i < m_nRows ; i++)
	{
		temp += m_rasterData[i];
	}
	return temp / m_nRows;
}

void clsRasterData::readASCFile(string ascFileName,clsRasterData* mask)
{
	if(mask == NULL) readASCFile(ascFileName);
	else
	{
		utils util;
		if(!util.FileExists(ascFileName)) throw ModelException("clsRasterData","readASCFile","The file " + ascFileName + " does not exist or has not read permission.");

		StatusMessage(("read " + ascFileName + "...").c_str());

		//get the data posotion from mask
		int nRows;
		float** validPosition;
		mask->getRasterPositionData(&nRows,&validPosition);		
		m_nRows = nRows;

		ifstream rasterFile(ascFileName.c_str());
		string tmp;
		float tempFloat, noDataValue;

		//read header
		rasterFile >> tmp >> tmp;
		rasterFile >> tmp >> tmp;
		rasterFile >> tmp >> tmp;
		rasterFile >> tmp >> tmp;
		rasterFile >> tmp >> tmp;
		rasterFile >> tmp >> noDataValue;

		//read data
		m_rasterData = new float[nRows];
		int ascCols = mask->getASCCols();
		int ascRows = mask->getASCRows();

		int index = 0;
		float *pData = new float[ascRows*ascCols];
		for (int i = 0; i < ascRows; ++i)
		{
			for (int j = 0; j < ascCols; ++j)
			{
				rasterFile >> pData[i*ascCols+j];
				//if(index < nRows)
				//{
				//	if(validPosition[index][0] == i && validPosition[index][1] == j) 
				//	{
				//		m_rasterData[index] = tempFloat;
				//		index++;
				//	}
				//}
			}
		}

		for(int index = 0; index < nRows; index++)
		{
			int i = validPosition[index][0];
			int j = validPosition[index][1];
			int rasterIndex = i*ascCols + j;
			m_rasterData[index] = pData[rasterIndex];
		}
		delete pData;

		//if(ascFileName.find("SLOPE") != string::npos)
		//{
		//	cout << ascFileName << m_nRows << m_rasterData << endl;
		//}
		rasterFile.close();
	}
}

void clsRasterData::ReadFromGDAL(string fileName,clsRasterData* mask)
{
	if(mask == NULL) 
		ReadFromGDAL(fileName);
	else
	{
		m_mask = mask;
		utils util;
		if(!util.FileExists(fileName)) throw ModelException("clsRasterData","ReadFromGDAL","The file " + fileName + " does not exist or has not read permission.");

		StatusMessage(("read " + fileName + "...").c_str());

		//set header
		map<string, float> *maskHeader = mask->getRasterHeader();
		m_headers["NCOLS"] = (*maskHeader)["NCOLS"];
		m_headers["NROWS"] = (*maskHeader)["NROWS"];
		m_headers["NODATA_VALUE"] = (*maskHeader)["NODATA_VALUE"];
		m_headers["CELLSIZE"] = (*maskHeader)["CELLSIZE"];
		m_headers["XLLCENTER"] = (*maskHeader)["XLLCENTER"];
		m_headers["YLLCENTER"] = (*maskHeader)["YLLCENTER"];

		//get the data posotion from mask
		int nRows;
		float** validPosition;
		mask->getRasterPositionData(&nRows,&validPosition);		
		m_rasterPositionData = validPosition;
		m_nRows = nRows;

		string tmp;
		//float tempFloat;

		//read data
		m_rasterData = new float[nRows];
		int ascCols = mask->getASCCols();
		int ascRows = mask->getASCRows();

		GDALDataset  *poDataset = (GDALDataset *) GDALOpen( fileName.c_str(), GA_ReadOnly );
		if( poDataset == NULL )
		{
			cerr << "Open file " + fileName + " failed.\n";
			return;
		}

		GDALRasterBand  *poBand= poDataset->GetRasterBand(1);
		GDALDataType dataType = poBand->GetRasterDataType();

		if (dataType == GDT_Float32)
		{
			float *pData = (float *) CPLMalloc(sizeof(float)*ascCols*ascRows);
			poBand->RasterIO(GF_Read, 0, 0, ascCols, ascRows, pData, ascCols, ascRows, GDT_Float32, 0, 0);
			for(int index = 0; index < nRows; index++)
			{
				int i = validPosition[index][0];
				int j = validPosition[index][1];
				int rasterIndex = i*ascCols + j;
				m_rasterData[index] = pData[rasterIndex];
			}
			CPLFree(pData);
		}
		else if (dataType == GDT_Int32)
		{
			int *pData = (int *) CPLMalloc(sizeof(int)*ascCols*ascRows);
			poBand->RasterIO(GF_Read, 0, 0, ascCols, ascRows, pData, ascCols, ascRows, GDT_Int32, 0, 0);
			for(int index = 0; index < nRows; index++)
			{
				int i = validPosition[index][0];
				int j = validPosition[index][1];
				int rasterIndex = i*ascCols + j;
				m_rasterData[index] = pData[rasterIndex];
			}
			CPLFree(pData);
		}
		else
		{
			throw ModelException("clsRasterData","ReadFromGDAL","The dataype of " + fileName + " is neither GDT_Float32 nor GDT_Int32.");
		}

		GDALClose(poDataset);
	}
}

void clsRasterData::readASCFile(string ascFileName)
{
	utils util;
	if(!util.FileExists(ascFileName)) 
		throw ModelException("clsRasterData","readASCFile","The file " + ascFileName + " does not exist or has not read permission.");

	StatusMessage(("read " + ascFileName + "...").c_str());

	ifstream rasterFile(ascFileName.c_str());
	string tmp;
	float noData;
	int rows,cols;
	float tempFloat;
	vector<float> values;
	vector<int> positionRows;
	vector<int> positionCols;

	//read header
	rasterFile >> tmp >> cols;
	m_headers[GetUpper(tmp)] = float(cols);
	rasterFile >> tmp >> rows;
	m_headers[GetUpper(tmp)] = float(rows);
	rasterFile >> tmp >> tempFloat;
	m_headers[GetUpper(tmp)] = tempFloat;
	rasterFile >> tmp >> tempFloat;
	m_headers[GetUpper(tmp)] = tempFloat;
	rasterFile >> tmp >> tempFloat;
	m_headers[GetUpper(tmp)] = tempFloat;
	//rasterFile >> tmp >> tempFloat;
	//m_headers[tmp] = tempFloat;
	rasterFile >> tmp >> noData;
	m_headers[GetUpper(tmp)] = noData;

	//get all valid values
	tempFloat = noData;
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			rasterFile >> tempFloat;
			if(tempFloat == noData) continue;
			values.push_back(tempFloat);
			positionRows.push_back(i);
			positionCols.push_back(j);
		}
	}
	rasterFile.close();

	//create float array
	m_nRows = values.size();
	m_rasterData = new float[m_nRows];
	m_rasterPositionData = new float*[m_nRows];
	for (int i = 0; i < m_nRows; ++i)
	{
		m_rasterData[i] = values.at(i);
		m_rasterPositionData[i] = new float[2];
		m_rasterPositionData[i][0] = float(positionRows.at(i));
		m_rasterPositionData[i][1] = float(positionCols.at(i));
	}
}

void clsRasterData::ReadFromGDAL(string filename)
{
	GDALDataset  *poDataset = (GDALDataset *) GDALOpen( filename.c_str(), GA_ReadOnly );
	if( poDataset == NULL )
	{
		cerr << "Open file " + filename + " failed.\n";
		return;
	}

	GDALRasterBand  *poBand= poDataset->GetRasterBand(1);
	m_headers["NCOLS"] = poBand->GetXSize();
	m_headers["NROWS"] = poBand->GetYSize();
	m_headers["NODATA_VALUE"] = (float)poBand->GetNoDataValue();
	double adfGeoTransform[6];
	poDataset->GetGeoTransform(adfGeoTransform);
	m_headers["CELLSIZE"] = adfGeoTransform[1];
	//m_headers["DY"] = -adfGeoTransform[5];
	m_headers["XLLCENTER"] = adfGeoTransform[0] + 0.5f*m_headers["CELLSIZE"];
	m_headers["YLLCENTER"] = adfGeoTransform[3] + (m_headers["NROWS"] - 0.5f)*m_headers["CELLSIZE"];

	int nRows = (int)m_headers["NROWS"];
	int nCols = (int)m_headers["NCOLS"];

	vector<float> values;
	vector<int> positionRows;
	vector<int> positionCols;
	//get all valid values
	float tempFloat = m_headers["NODATA_VALUE"];

	GDALDataType dataType = poBand->GetRasterDataType();
	if (dataType == GDT_Float32)
	{
		float *pData = (float *) CPLMalloc(sizeof(float)*nCols*nRows);
		poBand->RasterIO(GF_Read, 0, 0, nCols, nRows, pData, nCols, nRows, GDT_Float32, 0, 0);
		for (int i = 0; i < nRows; ++i)
		{
			for (int j = 0; j < nCols; ++j)
			{
				int index = i*nCols + j;
				if(FloatEqual(pData[index], m_headers["NODATA_VALUE"]) )
					continue;
				values.push_back(pData[index]);
				positionRows.push_back(i);
				positionCols.push_back(j);
			}
		}
		CPLFree(pData);
	}
	else if (dataType == GDT_Int32)
	{
		int *pData = (int *) CPLMalloc(sizeof(int)*nCols*nRows);
		poBand->RasterIO(GF_Read, 0, 0, nCols, nRows, pData, nCols, nRows, GDT_Int32, 0, 0);
		for (int i = 0; i < nRows; ++i)
		{
			for (int j = 0; j < nCols; ++j)
			{
				int index = i*nCols + j;
				if(FloatEqual(pData[index], m_headers["NODATA_VALUE"]) )
					continue;
				values.push_back(pData[index]);
				positionRows.push_back(i);
				positionCols.push_back(j);
			}
		}
		CPLFree(pData);
	}
	else
	{
		throw ModelException("clsRasterData","ReadFromGDAL","The dataype of " + filename + " is neither GDT_Float32 nor GDT_Int32.");
	}

	GDALClose(poDataset);

	//create float array
	m_nRows = values.size();
	m_rasterData = new float[m_nRows];
	m_rasterPositionData = new float*[m_nRows];
	for (int i = 0; i < m_nRows; ++i)
	{
		m_rasterData[i] = values.at(i);
		m_rasterPositionData[i] = new float[2];
		m_rasterPositionData[i][0] = float(positionRows.at(i));
		m_rasterPositionData[i][1] = float(positionCols.at(i));
	}
}

float clsRasterData::getValue(clsRasterData* templateRasterData, float* rasterData, int row, int col)
{
	if(templateRasterData == NULL || rasterData == NULL) return -99.0f;
	int position = templateRasterData->getPosition(row,col);
	if(position == -1) return -99.0f;
	return rasterData[position];
}
	
int clsRasterData::getPosition(int row,int col)
{
	if(m_rasterPositionData == NULL) return -1;
	
	for(int i = 0;i< m_nRows; i++)
	{
		if(row == m_rasterPositionData[i][0] && col == m_rasterPositionData[i][1]) return i;
	}

	return -1;
}

int clsRasterData::getPosition(float x,float y)
{	
	float xllCenter = m_headers["XLLCENTER"];
	float yllCenter = m_headers["YLLCENTER"];
	float dx = m_headers["CELLSIZE"];
	float dy = m_headers["CELLSIZE"];
	float nRows = m_headers["NROWS"];
	float nCols = m_headers["NCOLS"];

	float xmin = xllCenter - dx/2;
	float xMax = xmin + dx * nCols;
	if(x>xMax || x < xllCenter) throw ModelException("Raster","At","The x coordinate is beyond the scale!");

	float ymin = yllCenter - dy/2;
	float yMax = ymin + dy * nRows;
	if(y>yMax || y < yllCenter ) throw ModelException("Raster","At","The y coordinate is beyond the scale!");

	int nRow = (int)((yMax - y)/dy); //calculate form ymax
	int nCol = (int)((x - xmin)/dx); //calculate from xmin
	return getPosition(nRow,nCol);
}

float clsRasterData::getValue(int validCellIndex)
{
	if(m_rasterData == NULL)
		throw ModelException("Raster","getValue","Pelase first initialize the raster object.");
	if(m_nRows < validCellIndex) 
		throw ModelException("Raster","getValue","The index is too big! There are not so many valid cell in the raster.");

	return m_rasterData[validCellIndex];
}

