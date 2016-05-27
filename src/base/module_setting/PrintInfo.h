/*!
 * \file PrintInfo.h
 * \brief Class to store and manage the PRINT information from the file.out file
 *
 * \author Junzhi Liu, LiangJun Zhu
 * \version 1.1
 * \date June 2010
 */

#pragma once

#include <string>
#include <vector>
#include <time.h>
#include "ParamInfo.h"
#include <map>
//#include "clsSpecificOutput.h"
#include "mongoc.h"
#include "clsRasterData.h"

using namespace std;

/*!
 * \enum AggregationType
 * \ingroup module_setting
 * \brief Aggregation type for OUTPUT
 *
 */
enum AggregationType
{
	/// unknown
	AT_Unknown			= 0,
	/// sum
	AT_Sum				= 1,
	/// average
	AT_Average			= 2,
	/// minimum
	AT_Minimum			= 3,
	/// maximum
	AT_Maximum			= 4,
	/// output values of specific cells
	AT_SpecificCells	= 5
};

/*!
 * \ingroup module_setting
 * \class PrintInfoItem
 *
 * \brief Class stores output information 
 *
 */
class PrintInfoItem
{
private:
	//! 
	int					m_Counter;
private:
//	//! values of specific cells, e.g., landuse, curvature, and slope
//	clsSpecificOutput*	m_specificOutput;
//public:
//	void setSpecificCellRasterOutput(string projectPath,string databasePath,
//		clsRasterData* templateRasterData,string outputID);

public:
	//! MongoDB client
	mongoc_client_t*	conn;
	//! Database name
	string						dbName;
	//! GridFS
	mongoc_gridfs_t*	gfs;
	
	//! Constructor
	PrintInfoItem(void);
	//! Destructor
	~PrintInfoItem(void);

	//! Aggregated data
	float**				Data;	
	//! rows number
	int Numrows;
	//! For 1D raster data
	float*				RasterData;
	//! For 2D raster data
	float **			m_2dData;
	//! number of valid cells
	int					ValidCellCount;
	//! number of layer of 2D raster data
	int m_nCols;
	//! For time series data file
	map<time_t,float>	TimeSeriesData;
	//! For time series data for subbasin
	map<time_t,float*>	TimeSeriesDataForSubbasin;
	//! Count of \sa TimeSeriesDataForSubbasin
	int					TimeSeriesDataForSubbasinCount;
	//! Add 1D time series data result to \sa TimeSeriesDataForSubbasin
	void add1DTimeSeriesResult(time_t, int n, float* data);

	//! used only by PET_TS
	int SiteID;		///< The site id
	int SiteIndex;	///< The site index in output array1D variable

	int SubbasinID; ///< The subbasin id
	int SubbasinIndex;	///< The subbasin index
	
	//! used by all PrintItems
	//! Start time string
	string StartTime;
	//! Start time \a time_t
	time_t m_startTime;
	//! get start time \a time_t
	time_t getStartTime();
	//! End time string
	string EndTime;
	//! End time  \a time_t
	time_t m_endTime;
	//! Get end time  \a time_t
	time_t getEndTime();
	//! file suffix, e.g., txt, tif, asc, etc.
	string Suffix;
	//! output filename
	string Filename;
	//! create "output" folder to store all results
	void Flush(string,clsRasterData*,string);
	//! Determine if the given date is within the date range for this item
	bool IsDateInRange(time_t dt);
	//! Aggregate the data from the given data parameter using the given method type
	void AggregateData(int numrows, float** data, AggregationType type, float NoDataValue);
	//! Aggregate the data from the given data parameter using the given method type
	void AggregateData(time_t time,int numrows, float* data);
	//! Aggregate the 2D raster data from the given data parameter using the given method type
	void AggregateData2D(time_t time, int nRows, int nCols, float** data);
	//! Set the Aggregation type
	void setAggregationType(AggregationType type);
	//! Get the Aggregation type
	AggregationType getAggregationType(void);
	//! convert the given string into a matching Aggregation type
	static AggregationType MatchAggregationType(string type);
private:
	AggregationType m_AggregationType;
	
};
/*!
 * \ingroup module_setting
 * \class PrintInfo
 *
 * \brief 
 *
 *
 *
 */
class PrintInfo
{
public:
	//! used only for the PET_TS output
	int m_Interval;
	string m_IntervalUnits;
	int m_moduleIndex;

	//! used by all outputs
	string m_OutputID;
	ParamInfo* m_param; ///< The output variable corresponding to the output id
	vector<PrintInfoItem*> m_PrintItems;

	
	//void setSpecificCellRasterOutput(string projectPath, string databasePath,clsRasterData* templateRasterData);
private:
	//! for subbasin time series data
	vector<int> m_subbasinSeleted;
	float* m_subbasinSelectedArray;
public:
	PrintInfo(void);
	~PrintInfo(void);

	//! Get all the subbasins selected for this output id
	void getSubbasinSelected(int* count, float** subbasins);
	//! Set the OutputID for this object
	void setOutputID(string id);
	//! Get the OutputId for this object
	string getOutputID(void);
	string getOutputTimeSeriesHeader(void);
	//! Set the interval
	void setInterval(int interval);
	//! Set the interval units
	void setIntervalUnits(string units);
	//! Get the interval
	int getInterval(void);
	//! Get the interval units
	string getIntervalUnits(void);
	//! Add an output item with the given start time, end time and file name
	void AddPrintItem(string start, string end, string file, string sufi); 
	//! Add an output item with the given start time, end time and file name, for MongoDB
	void AddPrintItem(string type,string start, string end, string file,string sufi, mongoc_client_t* conn, mongoc_gridfs_t* gfs); 
	//! Add an output item with the given start time, end time and file name, Overloaded method
	void AddPrintItem(string start, string end, string file, string sitename, string sufi, bool isSubbasin);
	//! Get a reference to the output item located at the given index position
	PrintInfoItem* getPrintInfoItem(int index);
	//! Get the number of output items
	int ItemCount(void);
};
