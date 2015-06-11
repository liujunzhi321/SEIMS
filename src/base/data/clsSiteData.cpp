/*!
 * \file clsSiteData.cpp
 * \brief Read hydrological or meteorological site data 
 *
 *
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date June 2010
 *
 * 
 */
#include "clsSiteData.h"
#include "util.h"
#include "utils.h"
#include "text.h"
#include "ModelException.h"
//#include <io.h>
#include "DBManager.h"
#include <sstream>
#include <stdio.h>
#include "mongo.h"

clsSiteData::clsSiteData(string databasePath,string tableName)
{
	m_databasePath = databasePath;
	readSiteBasicInfo(tableName);
}

clsSiteData::clsSiteData(int id, const bson* info)
{
	m_ID = id;
	bson_iterator it[1];
	bson_iterator_init(it, info);
	while(bson_iterator_next(it)){
		if(strcmp("Name", bson_iterator_key(it)) == 0)
			m_Name = bson_iterator_string(it);
		//else if(strcmp("AREA", bson_iterator_key(it)) == 0)
		//	m_Area = (float)bson_iterator_double(it);
		else if(strcmp("LocalX", bson_iterator_key(it)) == 0)
			m_XPR = (float)bson_iterator_double(it);
		else if(strcmp("LocalY", bson_iterator_key(it)) == 0)
			m_YPR = (float)bson_iterator_double(it);
		else if(strcmp("Lat", bson_iterator_key(it)) == 0)
			m_Latitude = (float)bson_iterator_double(it);
		else if(strcmp("Long", bson_iterator_key(it)) == 0)
			m_Longitude = (float)bson_iterator_double(it);
		else if(strcmp("Elevation", bson_iterator_key(it)) == 0)
			m_Elevation = (float)bson_iterator_double(it);
	}
}

clsSiteData::clsSiteData(string databasePath,int id,string type)
{
	string tableName = clsHydroClimateData::getTableNameFromIDAndType(databasePath,id,type);
	
	if(tableName.length() == 0)
	{
		ostringstream oss;
		oss << id;
		throw ModelException(	"clsSiteData","clsSiteData",
			"Can't find table name for station:" + oss.str() + ",type:"+ type);
	}

	m_databasePath = databasePath;
	readSiteBasicInfo(tableName);
}

void clsSiteData::addDataType(string type)
{
	if(!clsHydroClimateData::IsHydroClimateDataType(&type))
	{
		throw ModelException(	"clsSiteData","addDataType",
								"The data type " + type +
								"' is not correct.");					
	}	

	map<string,clsHydroClimateData*>::iterator it = m_timeSerieseData.find(type);
	if(it != m_timeSerieseData.end()) return;
	else
	{
		m_timeSerieseData[type] = new clsHydroClimateData(m_databasePath,m_ID,type);
	}

}

void clsSiteData::addDataTypeMongo(const string& type)
{
	if(!clsHydroClimateData::IsHydroClimateDataType(&type))
	{
		throw ModelException("clsSiteData", "addDataTypeMongo", "The data type '" + type + "' is not correct.");	
	}	

	map<string,clsHydroClimateData*>::iterator it = m_timeSerieseData.find(type);
	if(it != m_timeSerieseData.end()) return;
	else
	{
		m_timeSerieseData[type] = new clsHydroClimateData();
	}
}

bool clsSiteData::isHasDataOfType(string type)
{
	if(!clsHydroClimateData::IsHydroClimateDataType(&type))
	{
		return false;					
	}	

	if(m_timeSerieseData.find(type) == m_timeSerieseData.end()) return false;
	return true;
}

void clsSiteData::dump(ostream* fs)
{
	if(fs == NULL) return;

	*fs << "---------- Station:"<< m_ID <<" ----------" << endl;

	*fs << "*** Basic Information ***" << endl;
	*fs << "ID:" << m_ID  << endl;
	*fs << "Name:" << m_Name << endl;
	*fs << "Elevation:" << m_Elevation << endl;
	*fs << "XPR:" << m_XPR  << endl;
	*fs << "YPR:" << m_YPR  << endl;
	*fs << "Latitude:" << m_Latitude  << endl;
	*fs << "Longitude:" << m_Longitude  << endl;
	*fs << "Area:" << m_Area << endl;

	*fs << "*** Data ***" << endl;
	map<string,clsHydroClimateData*>::iterator it;
	for(it=m_timeSerieseData.begin();it!=m_timeSerieseData.end();it++)
	{
		it->second->dump(fs);
	}

}

float clsSiteData::getTimeSeriesData(time_t time,string type)
{
	if(clsHydroClimateData::IsHydroClimateDataType(&type))
	{
		map<string,clsHydroClimateData*>::iterator it = m_timeSerieseData.find(type);
		if(it != m_timeSerieseData.end() && it->second != NULL) 
			return it->second->getTimeSeriesData(time);
		else
			return -999.0f;//means do not contain data of this type 
	}
	else
	{
		throw ModelException(	"clsSiteData","getTimeSeriesData",
								"The data type '"+ type +
								"' is not correct.");						
	}
}

void clsSiteData::loadTimeSeriesData(time_t startTime,time_t endTime,int interval)
{
	map<string,clsHydroClimateData*>::iterator it;
	for(it=m_timeSerieseData.begin();it!=m_timeSerieseData.end();it++)
	{
		if(it->second != NULL)
			it->second->loadTimeSeriesData(startTime,endTime,interval);
	}
}

void clsSiteData::readSiteBasicInfo(string tableName)
{
	string path = m_databasePath + File_HydroClimateDB;
	if(!DBManager::IsTableExist(path,"stations"))
		throw ModelException(	"clsSiteData","readSiteBasicInfo",
								"The database " + path + 
								" dose not exist or the table stations does not exist in this database.");

	if(!DBManager::IsTableExist(path,tableName))
		throw ModelException(	"clsSiteData","readSiteBasicInfo",
								"The database " + path + 
								" dose not exist or the table "+tableName+" does not exist in this database.");

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
			string strSQL = "SELECT ID, NAME, XPR, YPR, LAT, LONG, ELEVATION, AREA, TYPE FROM stations WHERE TABLENAME='" + 
				tableName+"'";
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
					m_ID = atoi(tbl->FieldValue(1, 0).c_str());
					m_Name = tbl->FieldValue(1, 1).c_str();
					m_XPR = (float)atof(tbl->FieldValue(1, 2).c_str());
					m_YPR = (float)atof(tbl->FieldValue(1, 3).c_str());
					m_Latitude = (float)atof(tbl->FieldValue(1, 4).c_str());
					m_Longitude = (float)atof(tbl->FieldValue(1, 5).c_str());
					m_Elevation = (float)atof(tbl->FieldValue(1, 6).c_str());
					m_Area = (float)atof(tbl->FieldValue(1, 7).c_str());

					string type = tbl->FieldValue(1, 8);
					if(clsHydroClimateData::IsHydroClimateDataType(&type))
					{
						m_timeSerieseData[type] = new clsHydroClimateData(m_databasePath,tableName);						
					}
					else
					{
						throw ModelException(	"clsSiteData","readSiteBasicInfo",
												"The data type for 'station:" + m_Name + ",tableName:"+tableName+
												"' is not correct.");						
					}
				}
				else
					throw ModelException(	"clsSiteData","readSiteBasicInfo",
											"There is no row whose table name is " +tableName);

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

clsSiteData::~clsSiteData(void)
{
	StatusMessage("Start to release clsSiteData ...");
	map<string,clsHydroClimateData*>::iterator it;
	for(it= m_timeSerieseData.begin();it!=m_timeSerieseData.end();it++)
	{
		if(it->second!=NULL) delete it->second;
	}
	StatusMessage("End to release clsSiteData ...");
}

string clsSiteData::toString(time_t time, float value)
{
	utils util;
	char sValue[20];
	strprintf(sValue,20,"%f",value);
	return  util.ConvertToString(&time) + ":" + string(sValue);
}

void clsSiteData::checkData(string typeName,time_t time,float value)
{
	if(!StringMatch(typeName,DataType_MinimumTemperature) && !StringMatch(typeName,DataType_MaximumTemperature))
	{		
		if(value < 0 ) throw ModelException("clsSiteData","checkData",typeName + " data could not be less than 0." + toString(time,value));
		
		if(StringMatch(typeName,DataType_WindSpeed) && value > 50)  throw ModelException("clsSiteData","checkData","Wind speed could not be larger than 50." + toString(time,value));
		if(StringMatch(typeName,DataType_Precipitation) && value > 500)  throw ModelException("clsSiteData","checkData","Wind speed could not be larger than 500." + toString(time,value));
	}
	else
	{
		if(value<-90) throw ModelException("clsSiteData","checkData","Temperature could not be less than -90." + toString(time,value));
		if(value>60) throw ModelException("clsSiteData","checkData","Temperature could not be larger than 60." + toString(time,value));
	}
}

