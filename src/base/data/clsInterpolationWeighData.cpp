/*!
 * \file clsInterpolationWeighData.cpp
 * \brief methods for clsInterpolationWeighData class
 *
 *
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date June 2010
 *
 * 
 */
#include "clsInterpolationWeighData.h"
#include <fstream>
#include "utils.h"
#include "util.h"
#include "ModelException.h"
#include <string>
#include <iostream>

using namespace std;

clsInterpolationWeighData::clsInterpolationWeighData(string weightFileName)
{
	m_fileName = weightFileName;
	m_weightData = NULL;

	//readWeightData(weightFileName);
}

clsInterpolationWeighData::clsInterpolationWeighData(gridfs *gfs, const char* remoteFilename)
{
	m_fileName = remoteFilename;
	m_weightData = NULL;
	ReadFromMongoDB(gfs, remoteFilename);
}

clsInterpolationWeighData::~clsInterpolationWeighData(void)
{
	if (m_weightData != NULL)
	{
		delete [] m_weightData;
	}
}

void clsInterpolationWeighData::getWeightData(int* n, float** data)
{
	*n = m_nRows;
	*data = m_weightData;
}

void clsInterpolationWeighData::dump(ostream *fs)
{
	if(fs == NULL) return;

	int index = 0;
	for (int i = 0; i < m_nRows; ++i)
	{
		//rasterFile >> tmp >> tmp;	
		for (int j = 0; j < m_nCols; ++j)
		{
			index = i*m_nCols + j;
			*fs << m_weightData[index] << "\t";
		}
		*fs << endl;
	}
}

void clsInterpolationWeighData::dump(string fileName)
{
	ofstream fs;
	fs.open(fileName.c_str(), ios::out);
	if (fs.is_open())
	{
		//read file
		//for (int i = 0; i < m_nRows; ++i)
		//{
		//	//rasterFile >> tmp >> tmp;	
		//	for (int j = 0; j < m_nCols; ++j)
		//	{
		//		fs << m_weightData[i][j] << " ";
		//	}
		//	fs << endl;
		//}
		dump(&fs);

		fs.close();
	}
}

//void clsInterpolationWeighData::readWeightData(string weightFileName)
//{
//	utils util;
//	if(!util.FileExists(weightFileName)) throw ModelException("clsInterpolationWeighData","readWeightData","The file " + weightFileName + " does not exist or has not read permission.");
//
//	StatusMessage(("reading "+ weightFileName + "...").c_str());
//	//read
//	ifstream rasterFile(weightFileName.c_str());
//
//	//string tmp;
//	//read header
//	rasterFile >> m_nRows;
//	rasterFile >> m_nCols;	
//
//	if(m_nRows <= 0 || m_nCols <=0)
//		throw ModelException("clsInterpolationWeighData","readWeightData",
//		"The number of rows and columns of "+weightFileName+
//		" is not correct. They should be larger than 0. Please make sure the format is correct.");
//
//	m_weightData = new float*[m_nRows];
//	for (int i = 0; i < m_nRows; ++i)
//	{
//		m_weightData[i] = new float[m_nCols];
//	}
//
//	//read file
//	for (int i = 0; i < m_nRows; ++i)
//	{
//		//rasterFile >> tmp >> tmp;	
//		for (int j = 0; j < m_nCols; ++j)
//		{
//			rasterFile >> m_weightData[i][j];
//		}
//	}
//	rasterFile.close();
//}


void clsInterpolationWeighData::ReadFromMongoDB(gridfs *gfs, const char* remoteFilename)
{
	//clock_t start = clock();

	gridfile gfile[1];
	bson b[1];
	bson_init(b);
	bson_append_string(b, "filename",  remoteFilename);
	bson_finish(b);  
	int flag = gridfs_find_query(gfs, b, gfile); 
	bson_destroy(b);

	if (flag != 0)
	{
		string filename = remoteFilename;
		int index = filename.find_last_of('_');
		string type = filename.substr(index+1);
		if (StringMatch(type, "PET") || StringMatch(type, "SR") || StringMatch(type, "SM") || StringMatch(type, "T")
			|| StringMatch(type, "TMax") || StringMatch(type, "TMin"))
		{
			string meteoFileName = filename.substr(0, index+1) +  "M";
			bson bm[1];
			bson_init(bm);
			bson_append_string(bm, "filename",  meteoFileName.c_str());
			bson_finish(bm);  
			flag = gridfs_find_query(gfs, bm, gfile); 
		}
	}
	

	if(0 != flag)
		throw ModelException("clsInterpolationWeighData", "ReadFromMongoDB", "Failed in gridfs_find_query file: " + string(remoteFilename) + ".\n");

	size_t length = (size_t)gridfile_get_contentlength(gfile);
	//char* buf = (char*)malloc(length);
	m_weightData = new float[length/4];
	char* buf = (char*)m_weightData;
	gridfile_read (gfile, length, buf);
	
	bson bmeta[1];
	gridfile_get_metadata(gfile, bmeta);
	bson_iterator iterator[1];
	if ( bson_find( iterator, bmeta, "NUM_CELLS" )) 
		m_nRows = bson_iterator_int(iterator);
	if ( bson_find( iterator, bmeta, "NUM_SITES" )) 
		m_nCols = bson_iterator_int(iterator);

	//m_weightData = new float*[m_nRows];
	//for (int i = 0; i < m_nRows; ++i)
	//{
	//	m_weightData[i] = new float[m_nCols];
	//}

	//char* context = NULL;
	//char *tokenPtr = StringTok(buf, "\n", &context);

	//for (int i = 0; i < m_nRows; ++i)
	//{
	//	if(tokenPtr == NULL)
	//		throw ModelException("clsInterpolationWeighData", "ReadFromMongoDB", "The number of cells in metadata is not the same as that in the gridfs file.");
	//	istringstream iss(tokenPtr);
	//	for (int j = 0; j < m_nCols; ++j)
	//	{
	//		iss >> m_weightData[i][j];
	//	}
	//	if(i < m_nRows - 1)
	//		tokenPtr = StringTok(NULL, "\n", &context);
	//}
	//
	//delete buf;

	//clock_t end = clock();
	//cout << "Read data: " << end-start << endl;


	gridfile_destroy(gfile);

}
