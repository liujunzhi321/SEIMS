#include "util.h"

#include <sstream>

//gdal
#include "gdal.h"
#include "gdal_priv.h"
#include "cpl_string.h"
#include "parallel.h"
#include "Raster.cpp"

void CombineRasterResults(string& folder, string& sVar, int nSubbasins, string& outputRaster) 
{ 


	float xMin = 1e10;
	float xMax = 0.f;
	float yMin = 1e10;
	float yMax = 0.f;

	float xll, yll, dx, dy, nRows, nCols, xur, yur;
	// loop to get global extent
	ostringstream oss;

	for (int i = 1; i <= nSubbasins; i++)
	{
		oss.str("");
		oss << folder << i << "_" << sVar << ".asc";

		Raster<float> rs;
		rs.ReadArcAscHeader(oss.str().c_str());

		nRows = rs.GetNumberOfRows();
		nCols = rs.GetNumberofColumns();
		dx = rs.GetXCellSize();
		dy = dx;
		xll = rs.GetXllCenter() - 0.5f*dx;
		yll = rs.GetYllCenter() - 0.5f*dy;
		xur = xll + nCols*dx;
		yur = yll + nRows*dy;
        //cout << "[DEBUG]\t" << nRows << "\t" << nCols << "\t" << xll << "\t" << yll << endl;

		if (i == 1)
		{
			xMin = xll;
			yMin = yll;
			xMax = xur;
			yMax = yur;
			continue;
		}

		xMin = (xll < xMin) ? xll : xMin;
		xMax = (xur > xMax) ? xur : xMax;
		yMin = (yll < yMin) ? yll : yMin;
		yMax = (yur > yMax) ? yur : yMax;

	}

	int nColsTotal = int((xMax - xMin)/dx + 0.5);
	int nRowsTotal = int((yMax - yMin)/dy + 0.5);

	//cout << xMin << "\t" << yMax << endl;
	//cout << nRowsTotal << "\t" << nColsTotal << endl;

	int nTotal = nRowsTotal * nColsTotal;
	float* data = new float[nTotal];
	float noDataValue = -9999;
	for(int i = 0; i < nTotal; i++)
	{
			data[i] = noDataValue;
	}

	// loop to put data in the array
	for (int i = 1; i <= nSubbasins; i++)
	{
		//cout << i << endl;
		oss.str("");
		oss << folder << i << "_" << sVar << ".asc";
		Raster<float> rs;
		rs.ReadFromArcAsc(oss.str().c_str());

		nRows = rs.GetNumberOfRows();
		nCols = rs.GetNumberofColumns();
		dx = rs.GetXCellSize();
		dy = dx;
		xll = rs.GetXllCenter() - 0.5f*dx;
		yll = rs.GetYllCenter() - 0.5f*dy;
		yur = yll + nRows*dy;
		float subNoDataValue = rs.GetNoDataValue();
		float** subData = rs.GetData();

		for (int k = 0; k < nRows; k++)
		{
			for (int m = 0; m < nCols; m++)
			{
				if(FloatEqual(subData[k][m], subNoDataValue))
					continue;

				int i =  int((yMax - yur + (k+0.5)*dy)/dy);
				int j = int((xll+(m+0.5)*dx - xMin)/dx);

				int index = i*nColsTotal + j;
				data[index] = subData[k][m];
			}
		}
	}	
	
	
	//Raster<float>::OutputRaster(outputFile.c_str(), nRowsTotal, nColsTotal, xMin, yMin, dx, noDataValue, data);
	Raster<float>::OutputGTiff(outputRaster.c_str(), nRowsTotal, nColsTotal, xMin, yMin, dx, noDataValue, data);
	cout << "[DEBUG]\toutput file: " << outputRaster << endl;
	
}


void CombineRasterResultsMongo(gridfs* gfs, string& sVar, int nSubbasins, string& outputRaster) 
{ 
	float xMin = 1e10;
	float xMax = 0.f;
	float yMin = 1e10;
	float yMax = 0.f;

	float xll, yll, dx, dy, nRows, nCols, xur, yur;
	// loop to get global extent
	ostringstream oss;

	for (int i = 1; i <= nSubbasins; i++)
	{
		oss.str("");
		oss << i << "_" << sVar;

		Raster<float> rs;
		rs.ReadMongoDBHeader(gfs, oss.str().c_str());

		nRows = rs.GetNumberOfRows();
		nCols = rs.GetNumberofColumns();
		dx = rs.GetXCellSize();
		dy = dx;
		xll = rs.GetXllCenter() - 0.5f*dx;
		yll = rs.GetYllCenter() - 0.5f*dy;
		xur = xll + nCols*dx;
		yur = yll + nRows*dy;
        //cout << "[DEBUG]\t" << nRows << "\t" << nCols << "\t" << xll << "\t" << yll << endl;

		if (i == 1)
		{
			xMin = xll;
			yMin = yll;
			xMax = xur;
			yMax = yur;
			continue;
		}

		xMin = (xll < xMin) ? xll : xMin;
		xMax = (xur > xMax) ? xur : xMax;
		yMin = (yll < yMin) ? yll : yMin;
		yMax = (yur > yMax) ? yur : yMax;

	}

	int nColsTotal = int((xMax - xMin)/dx + 0.5);
	int nRowsTotal = int((yMax - yMin)/dy + 0.5);

	//cout << xMin << "\t" << yMax << endl;
	//cout << nRowsTotal << "\t" << nColsTotal << endl;

	int nTotal = nRowsTotal * nColsTotal;
	float* data = new float[nTotal];
	float noDataValue = -9999;
	for(int i = 0; i < nTotal; i++)
	{
		data[i] = noDataValue;
	}

	// loop to put data in the array
	for (int i = 1; i <= nSubbasins; i++)
	{
		//cout << i << endl;
		oss.str("");
		oss << i << "_" << sVar;
		Raster<float> rs;
		rs.ReadFromMongoDB(gfs, oss.str().c_str());

		nRows = rs.GetNumberOfRows();
		nCols = rs.GetNumberofColumns();
		dx = rs.GetXCellSize();
		dy = dx;
		xll = rs.GetXllCenter() - 0.5f*dx;
		yll = rs.GetYllCenter() - 0.5f*dy;
		yur = yll + nRows*dy;
		float subNoDataValue = rs.GetNoDataValue();
		float** subData = rs.GetData();

		for (int k = 0; k < nRows; k++)
		{
			for (int m = 0; m < nCols; m++)
			{
				if(FloatEqual(subData[k][m], subNoDataValue))
					continue;

				int i =  int((yMax - yur + (k+0.5)*dy)/dy);
				int j = int((xll+(m+0.5)*dx - xMin)/dx);

				int index = i*nColsTotal + j;
				data[index] = subData[k][m];
			}
		}
	}	
	
	
	//Raster<float>::OutputRaster(outputFile.c_str(), nRowsTotal, nColsTotal, xMin, yMin, dx, noDataValue, data);
	Raster<float>::OutputGTiff(outputRaster.c_str(), nRowsTotal, nColsTotal, xMin, yMin, dx, noDataValue, data);
	cout << "[DEBUG]\toutput file: " << outputRaster << endl;
	
}

//int main()
//{
//	int nSubbasins = 899;
//	string folder = "D:\\watershed_simulation\\output\\";
//	string sVar = "SURU";
//	GDALAllRegister();
//	CombineRaster(folder, sVar, nSubbasins);
//	return 0;
//}
