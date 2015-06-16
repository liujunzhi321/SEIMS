/*!
 * \file clsInterpolationWeighData.h
 * \brief
 *
 *
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date June 2010
 *
 * 
 */
#pragma once
#include <string>
#include <ostream>
#include "gridfs.h"

using namespace std;
/*!
 * \ingroup data
 * \class clsInterpolationWeighData
 *
 * \brief 
 *
 *
 *
 */
class clsInterpolationWeighData
{
public:
	clsInterpolationWeighData(string weightFileName);
	clsInterpolationWeighData(gridfs *gfs, const char* remoteFilename);
	~clsInterpolationWeighData(void);

	void getWeightData(int*,float**);
	void dump(string);
	void dump(ostream *fs);
private:
	string m_fileName;
	float* m_weightData;
	int m_nRows, m_nCols;
	
private:
	void ReadFromMongoDB(gridfs *gfs, const char* remoteFilename);
};

