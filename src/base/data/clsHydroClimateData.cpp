/*!
 * \file clsHydroClimateData.cpp
 * \brief Methods for clsHydroClimateData class
 *
 * Methods for Hydrological climate data class
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date June 2010
 *
 * 
 */
#include "clsHydroClimateData.h"
#include "text.h"
#include "DBManager.h"
#include "ModelException.h"
#include "utils.h"
#include "util.h"
#include <sstream>
#include <iomanip>
#include "mongo.h"

clsHydroClimateData::clsHydroClimateData(string databasePath,string tableName)
{
	m_databasePath = databasePath;
	m_TableName = tableName;

	readData();
}

clsHydroClimateData::clsHydroClimateData()
{
	//utils util;
	//bson_iterator it[1];
	//bson_iterator_init(it, stationInfo);
	////bson_print((bson*)stationInfo);
	//while(bson_iterator_next(it))
	//{
	//	const char* key = bson_iterator_key(it);
	//	if(strcmp("TYPE", key) == 0)
	//		m_Type = bson_iterator_string(it);
	//	else if(strcmp("UNITS", key) == 0)
	//		m_Units = bson_iterator_string(it);
	//	else if(strcmp("STARTDATE", key) == 0)
	//		m_StartDate = bson_iterator_date(it)/1000;
	//	else if(strcmp("ENDDATE", key) == 0)
	//		m_EndDate = bson_iterator_date(it)/1000;
	//	else if(strcmp("INTERVAL", key) == 0)
	//		m_Interval = bson_iterator_int(it);
	//}

	//loadTimeSeriesDataMongo(type.c_str(), stationInfo);
}

clsHydroClimateData::clsHydroClimateData(string databasePath,int id,string type)
{
	string tableName = getTableNameFromIDAndType(databasePath,id,type);
	if(tableName.length() == 0)
	{
		ostringstream oss;
		oss << id;
		throw ModelException(	"clsHydroClimateData","clsHydroClimateData",
			"Can't find table name for station:" + oss.str() + ",type:"+ type);
	}

	m_databasePath = databasePath;
	m_TableName = tableName;

	readData();
}

clsHydroClimateData::~clsHydroClimateData(void)
{
}

string clsHydroClimateData::getTableNameFromIDAndType(string databasePath,int id,string type)
{
	string path = databasePath + File_HydroClimateDB;
	
	if(!DBManager::IsTableExist(path,"stations"))
		return "";	

	//read data
	DBManager dbman;
	try
	{
		// open the hydrclimate database
		dbman.Open(path);
		// if there is no error
		if (!dbman.IsError())
		{
			// create the SQL query for the stations table
			//ID, NAME, XPR, YPR, LAT, LONG, ELEVATION, TYPE, UNITS, AREA, STARTDATE, ENDDATE, INTERVAL, TABLENAME
			ostringstream oss;
			oss << id;
			string strSQL = "SELECT TABLENAME FROM stations WHERE ID=" + 
				oss.str()+" and type like '" +type+ "'";
			// run the query
			slTable* tbl = dbman.Load(strSQL);
			// if the query is successful
			if (tbl != NULL)
			{
				// if there is at least one record
				if (tbl->nRows > 0)
				{
					return tbl->FieldValue(1,0);
				}
				else
					return "";

				delete tbl;
			}
			tbl = NULL;
		}
		dbman.Close();
	}
	catch (...)
	{
		dbman.Close();		
	}
	return "";
}

bool clsHydroClimateData::IsHydroClimateDataType(const string* type)
{
	if(StringMatch(*type,DataType_Meteorology)) 
		return true;
	else if(StringMatch(*type,DataType_Precipitation)) 
		return true;
	else if(StringMatch(*type,DataType_MinimumTemperature)) 
		return true;
	else if(StringMatch(*type,DataType_MaximumTemperature)) 
		return true;
	else if(StringMatch(*type,DataType_PotentialEvapotranspiration)) 
		return true;
	else if(StringMatch(*type,DataType_SolarRadiation)) 
		return true;
	else if(StringMatch(*type,DataType_WindSpeed)) 
		return true;
	else if(StringMatch(*type,DataType_RelativeAirMoisture)) 
		return true;
	
	return false;
}

float clsHydroClimateData::getTimeSeriesData(time_t time)
{
	if(m_data.size() == 0) 
		throw ModelException(	"clsHydroClimateData","getTimeSeriesData",
								"Please first load data using loadTimeSeriesData function.");

	//map<time_t,float>::iterator it = m_data.find(time);
	map<time_t,float>::iterator it;
	for (it = m_data.begin(); it != m_data.end(); ++it)
	{
		if (time < it->first)
			break;
	}
	if(it == m_data.end())
		return 0.f;
	//{
	//	utils util;
	//	if(time>m_EndDate || time < m_StartDate)
	//		throw ModelException(	"clsHydroClimateData","getTimeSeriesData",
	//								"The time " + util.ConvertToString(&time) + 
	//								" is out of the time range.");
	//	else
	//		throw ModelException(	"clsHydroClimateData","getTimeSeriesData",
	//								"There is data in " + util.ConvertToString(&time) +
	//								". But it has not been loaded. Please expand the time range in file.in.");
	//}

	return it->second;
}

void clsHydroClimateData::loadTimeSeriesData(time_t startTime,time_t endTime,int interval)
{
	//if(m_Interval != interval)
	//{
	//	ostringstream oss;
	//	oss << "The time interval of " << m_TableName << " is " << m_Interval << " not " << interval << ".";
	//	throw ModelException(	"clsHydroClimateData","loadTimeSeriesData",oss.str());
	//}

	utils util;
	string sStart = util.ConvertToString2(&startTime);
	string sEnd = util.ConvertToString2(&endTime);

	if(m_StartDate > startTime)
		throw ModelException(	"clsHydroClimateData","loadTimeSeriesData",
								"The climate data in " + m_TableName + " start from " + 
								util.ConvertToString(&m_StartDate) + " which is late than " + 
								sStart + ". Please check file.in and hydroclimate database");
	if(m_EndDate < endTime)
		throw ModelException(	"clsHydroClimateData","loadTimeSeriesData",
								"The climate data in " + m_TableName + " end in " + 
								util.ConvertToString(&m_EndDate) + " which is early than " + 
								sEnd + ". Please check file.in and hydroclimate database");

	string path = m_databasePath + File_HydroClimateDB;

	if(!DBManager::IsTableExist(path,m_TableName))
		throw ModelException(	"clsHydroClimateData","loadTimeSeriesData",
								"The database " + path + 
								" dose not exist or the table "+m_TableName+" does not exist in this database.");
	
	m_data.clear();

	DBManager dbman;
	try
	{
		// open the hydrclimate database
		dbman.Open(path);
		// if there is no error
		if (!dbman.IsError())
		{
			// create the query for the data table
			// Use the start date and end date to limit the time series data
			string strSQL = "SELECT DATE, VALUE FROM " + m_TableName + 
				" WHERE [DATE] >= DATETIME('"+sStart+"') AND [DATE] <= DATETIME('"+sEnd+"')";
            //cout << strSQL << endl;
			// run the query
			slTable* tbl = dbman.Load(strSQL);
			// if the query is successful
			if (tbl != NULL)
			{
				if(tbl->nRows == 0) throw ModelException("clsHydroClimateData","loadTimeSeriesData",
					"The time series data in " + m_TableName + " between "+ sStart + " and " + sEnd + " is empty!");
				
				// read in the data
				for (int idx=1; idx<=tbl->nRows; idx++)
				{
					m_data[util.ConvertToTime2(tbl->FieldValue(idx,0), "%d-%d-%d %d:%d:%d", true)] = 
						(float)atof(tbl->FieldValue(idx,1).c_str());
                    //cout << m_TableName << "\t" << util.ConvertToTime2(tbl->FieldValue(idx,0), "%d-%d-%d %d:%d:%d", true) << " " << tbl->FieldValue(idx,1).c_str() << endl;
				}

				delete tbl;
			}
			tbl = NULL;
			dbman.Close();
		}	
	}
	catch (...)
	{
		dbman.Close();
		throw;
	}
}

void clsHydroClimateData::readData()
{
	string path = m_databasePath + File_HydroClimateDB;
	if(!DBManager::IsTableExist(path,"stations"))
		throw ModelException(	"clsHydroClimateData","readData",
								"The database " + path + 
								" dose not exist or the table stations does not exist in this database.");

	if(!DBManager::IsTableExist(path,m_TableName))
		throw ModelException(	"clsHydroClimateData","readData",
								"The database " + path + 
								" dose not exist or the table "+m_TableName+" does not exist in this database.");

	//read data
	DBManager dbman;
	try
	{
		// open the hydrclimate database
		dbman.Open(path);
		// if there is no error
		if (!dbman.IsError())
		{
			// create the SQL query for the stations table
			//ID, NAME, XPR, YPR, LAT, LONG, ELEVATION, TYPE, UNITS, AREA, STARTDATE, ENDDATE, INTERVAL, TABLENAME
			string strSQL = "SELECT TYPE,UNITS,STARTDATE,ENDDATE,INTERVAL FROM stations WHERE TABLENAME='" + 
				m_TableName+"'";
			// run the query
			slTable* tbl = dbman.Load(strSQL);
			// if the query is successful
			if (tbl != NULL)
			{
				// if there is at least one record
				if (tbl->nRows > 0)
				{
					utils util;

					// remember row 0 contains the field names not values
					m_Type = tbl->FieldValue(1, 0).c_str();
					m_Units = tbl->FieldValue(1, 1).c_str();
					m_StartDate = util.ConvertToTime2(tbl->FieldValue(1, 2),"%d-%d-%d %d:%d:%d",true);
					m_EndDate = util.ConvertToTime2(tbl->FieldValue(1, 3).c_str(),"%d-%d-%d %d:%d:%d",true);
					m_Interval = atoi(tbl->FieldValue(1, 4).c_str());
				}
				else
					throw ModelException(	"clsHydroClimateData","readData",
											"There is no row whose table name is " +m_TableName);

				delete tbl;
			}
			tbl = NULL;
		}
		dbman.Close();
	}
	catch (...)
	{
		dbman.Close();		
		throw;
	}
}

void clsHydroClimateData::dump(ostream* fs)
{
	if(fs == NULL) return;

	utils util;
	*fs << "----------" << m_Type << " data ----------" << endl;

	*fs << "*** Basic Information ***" << endl;
	*fs << "StartDate:" << util.ConvertToString2(&m_StartDate)  << endl;
	*fs << "EndDate:" << util.ConvertToString2(&m_EndDate)  << endl;
	*fs << "TableName:" << m_TableName  << endl;
	*fs << "Units:" << m_Units   << endl;
	*fs << "Interval:" << m_Interval   << endl;

	*fs << "*** Data ***" << endl;
	map<time_t,float>::iterator it;
	for(it=m_data.begin();it!=m_data.end();it++)
	{
		*fs << util.ConvertToString2(&(it->first)) << "  " << right << fixed << setw(15) << setfill(' ') << setprecision(4) << it->second << endl;
	}
}
