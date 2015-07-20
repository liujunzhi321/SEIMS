//! Class to manage Print information from the file.out file
#include "PrintInfo.h"
#include "utils.h"
#include "util.h"
#include <time.h>
#include "ModelException.h"
#include <iomanip>
#include <fstream>
#ifndef linux
    #include <Windows.h>
#else
    #include <unistd.h>    
    #include <sys/types.h>
    #include <sys/stat.h>
#endif

//!  define string constants used in the code
#define Tag_Unknown "UNKNOWN" 
#define Tag_Sum "SUM"
#define Tag_Average "AVERAGE"
#define Tag_Average2 "AVE"
#define Tag_Minimum "MIN"
#define Tag_Maximum "MAX"
#define Tag_SpecificCells "SPECIFIC"

//! Constructor
PrintInfoItem::PrintInfoItem(void):m_2dData(NULL), m_nCols(0)
{
	m_Counter = 0;
	Data = NULL;
	Numrows = 0;
	SiteID = -1;
	SiteIndex = -1;
	SubbasinID = -1;
	SubbasinIndex = -1;
	RasterData = NULL;
	ValidCellCount = -1;
	TimeSeriesDataForSubbasinCount = -1;
	m_AggregationType = AT_Unknown;

}

void PrintInfoItem::setSpecificCellRasterOutput(string projectPath,string databasePath,
	clsRasterData* templateRasterData,string outputID)
{
	if(m_AggregationType == AT_SpecificCells)
		m_specificOutput = new clsSpecificOutput(projectPath,databasePath,templateRasterData,outputID);
}

//! Destructor
PrintInfoItem::~PrintInfoItem(void)
{
	string file = Filename;
	StatusMessage(("Start to release PrintInfoItem for " + file + " ...").c_str());

	if (Data != NULL)
	{
		for (int rw=0; rw<m_Counter; rw++)
		{
			delete[] Data[rw];
		}
		delete[] Data;
	}
	if(RasterData != NULL) 
	{
		delete[] RasterData;
		RasterData = NULL;
	}

	if(m_2dData != NULL)
	{
		for(int i = 0; i < ValidCellCount; i++)
			delete[] m_2dData[i];
		delete[] m_2dData;
		m_2dData = NULL;
	}


	map<time_t,float*>::iterator it;
	for(it=TimeSeriesDataForSubbasin.begin();it!=TimeSeriesDataForSubbasin.end();it++)
	{
		if(it->second != NULL) delete [] it->second;
	}

	Data = NULL;
	m_Counter = 0;
	Numrows = 0;
	m_AggregationType = AT_Unknown;

	StatusMessage(("End to release PrintInfoItem for " + file + " ...").c_str());
}

//! Determine if the given date is within the date range for this item
bool PrintInfoItem::IsDateInRange(time_t dt)
{
	bool bStatus = false;

	// TODO - this assumes the hour is included in the date string
	//        should put a check in to determine whether this is true
	//start = utl.ConvertToTime(StartTime, "%d/%d/%d/%d", true);
	//end = utl.ConvertToTime(EndTime, "%d/%d/%d/%d", true);
	if (dt >= m_startTime && dt <= m_endTime)
	{
		bStatus = true;
	}

	return bStatus;
}
//! get start time
time_t PrintInfoItem::getStartTime()
{
	//utils utl;
	return m_startTime;
	//return utl.ConvertToTime2(StartTime,"%d-%d-%d %d:%d:%d",true);
	//return utl.ConvertToTime(StartTime, "%d/%d/%d/%d", true);
}
//! get end time
time_t PrintInfoItem::getEndTime()
{
	return m_endTime;
	//utils utl;
	//return utl.ConvertToTime2(EndTime,"%d-%d-%d %d:%d:%d",true);
	//return utl.ConvertToTime(EndTime, "%d/%d/%d/%d", true);
}
//! add 1D time series data result
void PrintInfoItem::add1DTimeSeriesResult(time_t t,int n,float* data)
{
	float* temp = new float[n];
	for(int i=0;i<n;i++)
	{
		temp[i] = data[i];
	}
	TimeSeriesDataForSubbasin[t] = temp;
	TimeSeriesDataForSubbasinCount = n;
}
//! create "output" folder to store all results
void PrintInfoItem::Flush(string projectPath, clsRasterData* templateRaster, string header)
{
#ifndef linux
	projectPath = projectPath + "output\\";
	if(::GetFileAttributes(projectPath.c_str()) == INVALID_FILE_ATTRIBUTES)
	{
		LPSECURITY_ATTRIBUTES att = NULL;
		::CreateDirectory(projectPath.c_str(),att);
	}
#else
	projectPath = projectPath + "output/";
	if(access(projectPath.c_str(), F_OK) != 0)
	{
		mkdir(projectPath.c_str(), 0777);
    }
#endif
	
	StatusMessage(("Creating output file " + Filename + "...").c_str());
	// Don't forget add appropriate suffix to Filename... ZhuLJ, 2015/6/16 
	if(m_AggregationType == AT_SpecificCells)
	{
		if(m_specificOutput != NULL)
		{
			m_specificOutput->dump(projectPath + Filename + ".txt");
			StatusMessage(("Create " + projectPath + Filename + " successfully!").c_str());
		}
			
		return;
	}
	if(TimeSeriesData.size() > 0 && (SiteID != -1 || SubbasinID != -1))	//time series data
	{
		ofstream fs;
		utils util;
		string filename = projectPath + Filename + ".txt";
		fs.open(filename.c_str(), ios::out);
		if (fs.is_open())
		{
			utils util;
			map<time_t,float>::iterator it;
			for(it=TimeSeriesData.begin();it!=TimeSeriesData.end();it++)
			{
				fs << util.ConvertToString2(&(it->first)) << " " << right << fixed << setw(15) << setfill(' ') << setprecision(8) << it->second <<  endl;
			}		

			fs.close();
			StatusMessage(("Create " + filename + " successfully!").c_str());
		}
		return;
	}
	if(TimeSeriesDataForSubbasin.size() > 0 && SubbasinID != -1)	//time series data for subbasin
	{
		ofstream fs;
		utils util;
        string filename = projectPath + Filename+ ".txt";
		fs.open(filename.c_str(), ios::out);
		if (fs.is_open())
		{
			fs << header << endl;
			utils util;
			map<time_t,float*>::iterator it;
			for(it=TimeSeriesDataForSubbasin.begin();it!=TimeSeriesDataForSubbasin.end();it++)
			{
				fs << util.ConvertToString2(&(it->first));
				for(int i = 0; i<TimeSeriesDataForSubbasinCount ;i++)
				{
					fs << " " << right << fixed << setw(15) << setfill(' ') << setprecision(8) << (it->second)[i];
				}
				fs << endl;
			}		
			
			fs.close();
			StatusMessage(("Create " + filename + " successfully!").c_str());
		}
		return;
	}
	if(RasterData != NULL && ValidCellCount > -1)	//GeoTIFF file
	{
		if(templateRaster == NULL) 
			throw ModelException("PrintInfoItem", "Flush", "The templateRaster is NULL.");

        //cout << projectPath << Filename << endl;
		//clsRasterData::outputToMongoDB(templateRaster,RasterData,Filename, gfs);
		//clsRasterData::outputASCFile(templateRaster,RasterData,projectPath + Filename);
		clsRasterData::outputGTiff(templateRaster, RasterData, projectPath + Filename + ".tif");
		return;
	}

	if(m_2dData != NULL && ValidCellCount > 0)
	{
		if(templateRaster == NULL) 
			throw ModelException("PrintInfoItem", "Flush", "The templateRaster is NULL.");

		float *tmpData = new float[ValidCellCount];
		ostringstream oss;
		for(int j = 0; j < 2; j++)//For soil properties output, presuming the number of layers is 2
		{
			for(int i = 0; i < ValidCellCount; i++)
				tmpData[i] = m_2dData[i][j];
			oss.str("");
			oss << projectPath << Filename << "_L" << j << ".tif";  // Filename_L0.tif means layer 0
			//clsRasterData::outputToMongoDB(templateRaster,RasterData,Filename, gfs);
			clsRasterData::outputGTiff(templateRaster, tmpData, oss.str());
		}
		delete[] tmpData;

		return;
	}

	if(TimeSeriesData.size() > 0)	//time series data
	{
		ofstream fs;
		utils util;
        string filename = projectPath + Filename + ".txt";
		fs.open(filename.c_str(), ios::out);
		if (fs.is_open())
		{
			utils util;
			map<time_t,float>::iterator it;
			for(it=TimeSeriesData.begin();it!=TimeSeriesData.end();it++)
			{
				fs << util.ConvertToString2(&(it->first)) << " " << right << fixed << setw(15) << setfill(' ') << setprecision(8) << it->second <<  endl;
			}		

			fs.close();
			StatusMessage(("Create " + filename + " successfully!").c_str());
		}
		return;
	}

	throw ModelException("PrintInfoItem","flush","Creating " + Filename + " is failed. There is not result data for this file. Please check ouput varaibles of modules.");
}

//! Aggregate the data from the given data parameter using the given method type
void PrintInfoItem::AggregateData2D(time_t time, int nRows, int nCols, float** data)
{
	if(m_AggregationType == AT_SpecificCells)
	{
/*		if(m_specificOutput != NULL)
		{
			m_specificOutput->setData(time,data);
		}	*/		
	}
	else
	{
		// check to see if there is an aggregate array to add data to
		if (m_2dData == NULL)
		{
			// create the aggregate array
			ValidCellCount = nRows;
			m_nCols = nCols;
			m_2dData = new float*[ValidCellCount];
			for (int i = 0; i < ValidCellCount; i++)
			{
				m_2dData[i] = new float[m_nCols];
				for(int j = 0; j < m_nCols; j++)
					m_2dData[i][j] = 0.0f;
			}
			m_Counter = 0;
		}

		switch (m_AggregationType)
		{
			case AT_Average:
				for(int i = 0; i < ValidCellCount; i++)
					for(int j = 0; j < nCols; j++)
						m_2dData[i][j] = float((m_2dData[i][j] * m_Counter + data[i][j])/(m_Counter + 1));
				break;
			case AT_Sum:
				for(int i = 0; i < ValidCellCount; i++)
					for(int j = 0; j < nCols; j++)
						m_2dData[i][j] += data[i][j];
				break;
			case AT_Minimum:
				for(int i = 0; i < ValidCellCount; i++)
					for(int j = 0; j < nCols; j++){
						if(data[i][j] < m_2dData[i][j]) 
							m_2dData[i][j] = data[i][j];
					}
				break;
			case AT_Maximum:
				for(int i = 0; i < ValidCellCount; i++)
					for(int j = 0; j < nCols; j++){
						if(data[i][j] > m_2dData[i][j]) 
							m_2dData[i][j] = data[i][j];
					}
				break;
            default:
                break;
		}


		m_Counter++;
	}
}

//! Aggregate the data from the given data parameter using the given method type
void PrintInfoItem::AggregateData(time_t time,int numrows, float* data)
{
	if(m_AggregationType == AT_SpecificCells)
	{
		if(m_specificOutput != NULL)
		{
			m_specificOutput->setData(time,data);
		}			
	}
	else
	{
		// check to see if there is an aggregate array to add data to
		if (RasterData == NULL)
		{
			// create the aggregate array
			ValidCellCount = numrows;
			RasterData = new float[ValidCellCount];
			for (int i=0; i<ValidCellCount; i++)
			{
				RasterData[i] = 0.0f;
			}
			m_Counter = 0;
		}

		// depending on the type of aggregation
		for (int rw=0; rw<ValidCellCount; rw++)
		{
			switch (m_AggregationType)
			{
				case AT_Average:
					RasterData[rw] = float((RasterData[rw] * m_Counter + data[rw])/(m_Counter + 1));
					break;
				case AT_Sum:
					RasterData[rw] += data[rw];
					break;
				case AT_Minimum:
					if(RasterData[rw]>data[rw]) RasterData[rw]=data[rw];
					break;
				case AT_Maximum:
					if(RasterData[rw]<data[rw]) RasterData[rw]=data[rw];
					break;
                default:
                    break;
			}							

		}
		m_Counter++;
	}
}

//! Aggregate the data from the given data parameter using the given method type
void PrintInfoItem::AggregateData(int numrows, float** data, AggregationType type, float NoDataValue)
{
	// check to see if there is an aggregate array to add data to
	if (Data == NULL)
	{
		// create the aggregate array
		Numrows = numrows;
		Data = new float*[Numrows];
		for (int i=0; i<Numrows; i++)
		{
			Data[i] = new float[3];
			Data[i][0] = NoDataValue;
			Data[i][1] = NoDataValue;
			Data[i][2] = NoDataValue;
		}
		m_Counter = 0;
	}

	// depending on the type of aggregation
	switch (type)
	{
	case AT_Average:
		for (int rw=0; rw<Numrows; rw++)
		{
			if (!FloatEqual(data[rw][2], NoDataValue))
			{
				// initialize value to 0.0 if this is the first time
				if (FloatEqual(Data[rw][2], NoDataValue)) Data[rw][2] = 0.0f;
				Data[rw][0] = data[rw][0]; // store the row number
				Data[rw][1] = data[rw][1]; // store the column number
				if (m_Counter == 0)
				{
					// first value so average = value
					Data[rw][2] = data[rw][2];	// store the value
				}
				else
				{
					// calculate the incremental average
					Data[rw][2] = ((Data[rw][2] * m_Counter) + data[rw][2]) / (m_Counter + 1);
				}
			}
		}
		m_Counter++;
		break;
	case AT_Sum:
		for (int rw=0; rw<Numrows; rw++)
		{
			if (!FloatEqual(data[rw][2], NoDataValue))
			{
				// initialize value to 0.0 if this is the first time
				if (FloatEqual(Data[rw][2], NoDataValue)) Data[rw][2] = 0.0f;
				Data[rw][0] = data[rw][0];
				Data[rw][1] = data[rw][1];
				// add the next value to the current sum
				Data[rw][2] += data[rw][2];
			}
		}
		break;
	case AT_Minimum:
		for (int rw=0; rw<Numrows; rw++)
		{
			if (!FloatEqual(data[rw][2], NoDataValue))
			{
				// initialize value to 0.0 if this is the first time
				if (FloatEqual(Data[rw][2], NoDataValue)) Data[rw][2] = 0.0f;
				Data[rw][0] = data[rw][0];
				Data[rw][1] = data[rw][1];
				// if the next value is smaller than the current value
				if (data[rw][2] < Data[rw][2])
				{
					// set the current value to the next (smaller) value
					Data[rw][2] = data[rw][2];
				}
			}
		}
		break;
	case AT_Maximum:
		for (int rw=0; rw<Numrows; rw++)
		{
			if (!FloatEqual(data[rw][2], NoDataValue))
			{
				// initialize value to 0.0 if this is the first time
				if (FloatEqual(Data[rw][2], NoDataValue)) Data[rw][2] = 0.0f;
				Data[rw][0] = data[rw][0];
				Data[rw][1] = data[rw][1];
				// if the next value is larger than the current value
				if (data[rw][2] > Data[rw][2])
				{
					// set the current value to the next (larger) value
					Data[rw][2] = data[rw][2];
				}
			}
		}
		break;
    default:
        break;
	}
}

void PrintInfo::setSpecificCellRasterOutput(string projectPath, string databasePath,
clsRasterData* templateRasterData)
{
	vector<PrintInfoItem*>::iterator it;
	for(it= m_PrintItems.begin();it<m_PrintItems.end();it++)
	{
		(*it)->setSpecificCellRasterOutput(projectPath,databasePath,templateRasterData,m_OutputID);
	}
}


//! Constructor
PrintInfo::PrintInfo(void)
{
	m_Interval = 0;
	m_IntervalUnits = "";
	m_OutputID = "";	
	m_PrintItems.clear();
	m_param = NULL;
	m_moduleIndex = -1;
	m_subbasinSelectedArray = NULL;
}

//! Destructor
PrintInfo::~PrintInfo(void)
{
	m_Interval = 0;
	m_IntervalUnits = "";
	m_OutputID = "";
	m_PrintItems.clear();

	if(m_subbasinSelectedArray!=NULL) delete [] m_subbasinSelectedArray;
}

//! Set the OutputID for this object
void PrintInfo::setOutputID(string id)
{
	m_OutputID = id;
}

//! Get the OutputId for this object
string PrintInfo::getOutputID(void)
{
	return m_OutputID;
}

string PrintInfo::getOutputTimeSeriesHeader()
{
	vector<string> headers;
	if(StringMatch(m_OutputID,"T_SNWB"))
	{
		headers.push_back("Time");
		headers.push_back("P");
		headers.push_back("P_net");
		headers.push_back("P_blow");
		headers.push_back("T");
		headers.push_back("Wind");
		headers.push_back("SR");
		headers.push_back("SE");
		headers.push_back("SM");
		headers.push_back("SA");
	}
	else if(StringMatch(m_OutputID,"SOWB"))
	{
		headers.push_back("Time");
		headers.push_back("P");
		headers.push_back("T");
		headers.push_back("Soil_T");
		headers.push_back("pNet");
		headers.push_back("InterceptionET");
		headers.push_back("DepressionET");
		headers.push_back("Infiltration");
		headers.push_back("Total_ET");
		headers.push_back("Soil_ET");
		headers.push_back("Net_Percolation");
		headers.push_back("Revap");
		headers.push_back("RS");
		headers.push_back("RI");
		headers.push_back("RG");
		headers.push_back("R");
		headers.push_back("Moisture");
		headers.push_back("MoistureDepth");
	}
	else if(StringMatch(m_OutputID,"T_GWWB"))
	{
		headers.push_back("Time");
		headers.push_back("PERCO");
		headers.push_back("REVAP");
		headers.push_back("PERDE");
		headers.push_back("Baseflow(mm)");
		headers.push_back("GW");
		headers.push_back("Baseflow(m3/s)");
	}
	else if(StringMatch(m_OutputID,"T_RECH"))
	{
		headers.push_back("Time");
		headers.push_back("QS");
		headers.push_back("QI");
		headers.push_back("QG");
		headers.push_back("Q(m^3/s)");
		headers.push_back("Q(mm)");
	}
	else if(StringMatch(m_OutputID,"T_WABA"))
	{
		headers.push_back("Time");
		headers.push_back("Vin");
		headers.push_back("Vside");
		headers.push_back("Vdiv");
		headers.push_back("Vpoint");
		headers.push_back("Vseep");
		headers.push_back("Vnb");
		headers.push_back("Ech");
		headers.push_back("Lbank");
		headers.push_back("Vbank");
		headers.push_back("Vout");
		headers.push_back("Vch");
		headers.push_back("Depth");
		headers.push_back("Velocity");
	}
	else if(StringMatch(m_OutputID,"T_RSWB"))
	{
		headers.push_back("Time");
		headers.push_back("Qin(m^3/s)");
		headers.push_back("Area(ha)");
		headers.push_back("Storage(10^4*m^3)");
		headers.push_back("QSout(m^3/s)");
		headers.push_back("QIout(m^3/s)");
		headers.push_back("QGout(m^3/s)");
		headers.push_back("Qout(m^3/s)");
		headers.push_back("Qout(mm)");
	}
	else if(StringMatch(m_OutputID,"T_CHSB"))
	{
		headers.push_back("Time");
		headers.push_back("SedInUpStream");
		headers.push_back("SedInSubbasin");
		headers.push_back("SedDeposition");
		headers.push_back("SedDegradation");
		headers.push_back("SedStorage");
		headers.push_back("SedOut");
	}
	else if(StringMatch(m_OutputID,"T_RESB"))
	{
		headers.push_back("Time");
		headers.push_back("ResSedIn");
		headers.push_back("ResSedSettling");
		headers.push_back("ResSedStorage");
		headers.push_back("ResSedOut");
	}

	ostringstream oss;
	vector<string>::iterator it;
	for(it=headers.begin();it<headers.end();it++)
	{
		if(StringMatch(*it,"Time")) 
			oss << *it;
		else						
			oss << " " << setw(15) << right << setfill(' ') << *it;
	}

	return oss.str(); 
}

//! convert the given string into a matching Aggregation type
AggregationType PrintInfoItem::MatchAggregationType(string type)
{
	//AT_Sum = 1,
	//AT_Average = 2,
	//AT_Minimum = 3,
	//AT_Maximum = 4

	// TODO - should convert the given string to UPPERCASE for comparison

	AggregationType res = AT_Unknown;
	if (StringMatch(type, Tag_Unknown))
	{
		res = AT_Unknown;
	}
	if (StringMatch(type, Tag_Sum))
	{
		res = AT_Sum;
	}
	if (StringMatch(type, Tag_Average) || StringMatch(type, Tag_Average2))
	{
		res = AT_Average;
	}
	if (StringMatch(type, Tag_Minimum))
	{
		res = AT_Minimum;
	}
	if (StringMatch(type, Tag_Maximum))
	{
		res = AT_Maximum;
	}
	if (StringMatch(type,Tag_SpecificCells))
	{
		res = AT_SpecificCells;
	}

	return res;
}

//! Set the Aggregation type
void PrintInfoItem::setAggregationType(AggregationType type)
{
	m_AggregationType = type;
}

//! Get the Aggregation type
AggregationType PrintInfoItem::getAggregationType(void)
{
	return m_AggregationType;
}

//! Set the interval
void PrintInfo::setInterval(int interval)
{
	m_Interval = interval;
}

//! Set the interval units
void PrintInfo::setIntervalUnits(string units)
{
	m_IntervalUnits = units;
}

//! Get the interval
int PrintInfo::getInterval(void)
{
	return m_Interval;
}

//! Get the interval units
string PrintInfo::getIntervalUnits(void)
{
	return m_IntervalUnits;
}

//! Add an item with the given start time, end time and output file name
void PrintInfo::AddPrintItem(string start, string end, string file)
{
	// create a new object instance
	PrintInfoItem* itm = new PrintInfoItem();

	// set its properties
	itm->SiteID = -1;
	itm->StartTime = start;
	itm->EndTime = end;
	itm->Filename = file;

	itm->m_startTime = utils::ConvertToTime2(start, "%d-%d-%d %d:%d:%d",true);
	itm->m_endTime = utils::ConvertToTime2(end, "%d-%d-%d %d:%d:%d",true);
	// add it to the list
	m_PrintItems.push_back(itm);
}

//! Add an item with the given start time, end time and output file name
void PrintInfo::AddPrintItem(string type,string start, string end, string file, mongo* conn, gridfs* gfs)
{
	// create a new object instance
	PrintInfoItem* itm = new PrintInfoItem();

	// set its properties
	itm->SiteID = -1;
	itm->StartTime = start;
	itm->EndTime = end;
	itm->Filename = file;
	itm->conn = conn;
	itm->gfs = gfs;

	itm->m_startTime = utils::ConvertToTime2(start, "%d-%d-%d %d:%d:%d",true);
	itm->m_endTime = utils::ConvertToTime2(end, "%d-%d-%d %d:%d:%d",true);


    type = trim(type);
	AggregationType enumType = PrintInfoItem::MatchAggregationType(type);
	if(enumType == AT_Unknown) throw ModelException("PrintInfo","AddPrintItem","The type of output " + m_OutputID + " can't be unknown. Please check file.out. The type should be MIN, MAX, SUM or AVERAGE.");

	itm->setAggregationType(enumType);

	// add it to the list
	m_PrintItems.push_back(itm);
}

//! Add an item with the given start time , end time, output file name and sitename 
//! Overloaded method
void PrintInfo::AddPrintItem(string start, string end, string file, string sitename, bool isSubbasin)
{
	PrintInfoItem* itm = new PrintInfoItem();

	if(!isSubbasin) itm->SiteID		= atoi(sitename.c_str());
	else
	{
		itm->SubbasinID = atoi(sitename.c_str());
		itm->SubbasinIndex = m_subbasinSeleted.size();
		m_subbasinSeleted.push_back(itm->SubbasinID);
	}
	itm->StartTime = start;
	itm->EndTime = end;
	itm->Filename = file;

	itm->m_startTime = utils::ConvertToTime2(start, "%d-%d-%d %d:%d:%d",true);
	itm->m_endTime = utils::ConvertToTime2(end, "%d-%d-%d %d:%d:%d",true);

	m_PrintItems.push_back(itm);
}

void PrintInfo::getSubbasinSelected(int* count, float** subbasins)
{
	*count = m_subbasinSeleted.size();
	if(m_subbasinSelectedArray == NULL && m_subbasinSeleted.size() > 0)
	{
		m_subbasinSelectedArray = new float[m_subbasinSeleted.size()];
		int index = 0;
		vector<int>::iterator it;
		for(it=m_subbasinSeleted.begin();it<m_subbasinSeleted.end();it++) 
		{
			m_subbasinSelectedArray[index] = float(*it);
			index++;
		}
	}

	*subbasins = m_subbasinSelectedArray;
}

//! Return the number of items
int PrintInfo::ItemCount(void)
{
	return m_PrintItems.size();
}

//! Return a reference to the Item located at the given index position
PrintInfoItem* PrintInfo::getPrintInfoItem(int index)
{
	// default is NULL
	PrintInfoItem* res = NULL;

	// is the index in the valid range
	if (index >= 0 && index < (int)m_PrintItems.size())
	{
		// assign the reference to the given item
		res = m_PrintItems.at(index);
	}

	// return the reference
	return res;
}
