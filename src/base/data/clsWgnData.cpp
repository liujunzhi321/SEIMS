#include "clsWgnData.h"
#include "DBManager.h"
#include "text.h"
#include "util.h"
#include <map>
#include "text.h"
#include "ModelException.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
//! Constructor
clsWgnData::clsWgnData(string databasePath,WgnType type)
{
	m_wgnData = NULL;

	readWgnData(databasePath,type);
}

//! Destructor
clsWgnData::~clsWgnData(void)
{
	if (m_wgnData != NULL)
	{
		for (int i = 0; i < 12; ++i)
		{
			if (m_wgnData[i] != NULL)
				delete [] m_wgnData[i];
				
		}
		delete [] m_wgnData;
	}
}
//! get wgn data, nRows is 12 (months), 
void clsWgnData::getWgnData(int* nRows,int* nCols,float*** wgnData)
{
	*nRows = 12;
	*nCols = m_columns.size();
	*wgnData = m_wgnData;
}
//! show data
void clsWgnData::show()
{
	int size = m_columns.size();
	
	StatusMessage("wgnData");
	string s = "month";
	for(int j=0;j<size;j++)
	{
		s+="\t" + m_columns[j];
	}
	StatusMessage(s.c_str());

	char temp[20];
	for(int i=0;i<12;i++)
	{	
		s = "";
#ifndef linux
		sprintf_s(temp,20,"%d",i+1);
#else
        snprintf(temp, 20, "%d", i+1);
#endif
		s.append(temp);
		for(int j=0;j<size;j++)
		{
#ifndef linux
			sprintf_s(temp,20,"\t%f",m_wgnData[i][j]);
#else
            snprintf(temp,20,"\t%f",m_wgnData[i][j]);
#endif
			s.append(temp);
		}		
		StatusMessage(s.c_str());
	}
}
//! read wgn data from Sqlite database. 
void clsWgnData::readWgnData(string databasePath,WgnType type)
{
	m_wtgTableName = Table_WGN;
	
	if(type == WGN_PRECIPITATION)
	{
		m_columns.push_back("RAINHHMX");
		m_columns.push_back("PCPMM");
		m_columns.push_back("PCPD");
	}

	//get fields
	string fields= "";
	int size = m_columns.size();
	for(int j=0;j<size;j++)
	{
		if(fields.length() > 0) fields += ",";
		fields += m_columns.at(j);
	}

	//start to read data
	DBManager dbman;
	string strSQL;
	slTable* tbl;
		
	if(m_wgnData == NULL)
	{		
		m_wgnData = new float*[12];
		for(int i=0;i<12;i++)
		{
			m_wgnData[i] = new float[size];
			for(int j=0;j<size;j++) m_wgnData[i][j] = 0.0f;
		}
	}

	StatusMessage("reading wgn data ...");

	try
	{
		// open the database
		dbman.Open(databasePath + File_HydroClimateDB);
		// if there is no error
		if (!dbman.IsError())
		{
			// craete the SQL statement for the query
			strSQL = "SELECT " + fields + " FROM " + string(m_wtgTableName) + " order by MONTH";
			// run the query
			tbl = dbman.Load(strSQL);
			// if the query was successful
			if (tbl != NULL)
			{
				//if(tbl->nRows != 13)
				//	throw ModelException("clsWgnData","readWgnData","The wgn data must have 12 columns.");
				for (int idx=1; idx<=tbl->nRows; idx++)
				{
					for(int j=0;j<size;j++)
					{
						float value = float(atof(tbl->FieldValue(idx,j).c_str()));
						m_wgnData[idx-1][j] = value;
					}
				}

				delete tbl;
			}
			tbl = NULL;

			// close the database
			dbman.Close();
		}
	}
	catch (...)
	{
		if (tbl != NULL)
		{
			delete tbl;
		}
		tbl = NULL;
		dbman.Close();
		throw;
	}
}
