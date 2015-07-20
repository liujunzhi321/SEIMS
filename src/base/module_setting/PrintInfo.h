//! class to store and manage the PRINT information from the file.out file
#pragma once

#include <string>
#include <vector>
#include <time.h>
#include "ParamInfo.h"
#include <map>
#include "clsSpecificOutput.h"
#include "mongo.h"
#include "gridfs.h"

using namespace std;

//! Aggregation type enum 
enum AggregationType
{
	AT_Unknown = 0,  ///< unknown
	AT_Sum = 1,      ///< sum
	AT_Average = 2,  ///< ave
	AT_Minimum = 3,  ///< min
	AT_Maximum = 4,  ///< max
	AT_SpecificCells = 5 ///< output values of specific cells
};

/*!
 * \ingroup module_setting
 * \class PringInfoItem
 *
 * \brief Class stores the information 
 *
 *
 *
 */
class PrintInfoItem
{
private:
	int m_Counter;

private:
	clsSpecificOutput* m_specificOutput;		///< values of specific cells
public:
	void setSpecificCellRasterOutput(string projectPath,string databasePath,
		clsRasterData* templateRasterData,string outputID);

public:

	mongo* conn;
	gridfs* gfs;
	

	PrintInfoItem(void);
	~PrintInfoItem(void);

	// aggregated data
	float** Data;					
	int Numrows;

	float* RasterData;				///< for ascii file
	float **m_2dData;	///< for 2d array
	int ValidCellCount;
	int m_nCols;

	map<time_t,float> TimeSeriesData; ///< for time series data file

	map<time_t,float*> TimeSeriesDataForSubbasin; ///< for time series data for subbasin
	int TimeSeriesDataForSubbasinCount;
	void add1DTimeSeriesResult(time_t, int n, float* data);

	//! used only by PET_TS
	int SiteID;		///< The site id
	int SiteIndex;	///< The site index in output array1D variable

	int SubbasinID; ///< The subbasin id
	int SubbasinIndex;	///< The subbasin index
	
	//! used by all PrintItems
	string StartTime;
	time_t getStartTime();
	string EndTime;
	time_t getEndTime();
	string Filename;

	time_t m_startTime;
	time_t m_endTime;

	void Flush(string,clsRasterData*,string);
	bool IsDateInRange(time_t dt);
	void AggregateData(int numrows, float** data, AggregationType type, float NoDataValue);
	void AggregateData(time_t time,int numrows, float* data);

	void AggregateData2D(time_t time, int nRows, int nCols, float** data);
	
	void setAggregationType(AggregationType type);
	AggregationType getAggregationType(void);
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

	
	void setSpecificCellRasterOutput(string projectPath, string databasePath,
	clsRasterData* templateRasterData);
private:
	//! for subbasin time series data
	vector<int> m_subbasinSeleted;
	float* m_subbasinSelectedArray;
public:
	PrintInfo(void);
	~PrintInfo(void);

	/*
	** Get all the subbasins selected for this output id
	*/
	void getSubbasinSelected(int* count, float** subbasins);

	void setOutputID(string id);
	string getOutputID(void);
	string getOutputTimeSeriesHeader(void);

	void setInterval(int interval);
	void setIntervalUnits(string units);
	int getInterval(void);
	string getIntervalUnits(void);

	void AddPrintItem(string start, string end, string file); // used for all non-PET_TS
	void AddPrintItem(string type,string start, string end, string file, mongo* conn, gridfs* gfs); // used for all non-PET_TS
	void AddPrintItem(string start, string end, string file, string sitename,bool isSubbasin);	// used for PET_TS

	PrintInfoItem* getPrintInfoItem(int index);

	int ItemCount(void);
};
