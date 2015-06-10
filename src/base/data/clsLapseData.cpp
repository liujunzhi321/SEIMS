/*!
 * \file clsLapseData.cpp
 * \brief methods for clsLapseData class
 *
 * \deprecated This class is designed for Sqlite.
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date June 2010
 *
 * 
 */
#include "clsLapseData.h"
#include "DBManager.h"
#include "text.h"
#include "util.h"
#include <map>
#include "text.h"
#include "ModelException.h"
#include <string>
#include "stdio.h"
#include "stdlib.h"

clsLapseData::clsLapseData(string databasePath)
{
	m_lapseData = NULL;

	readLapseData(databasePath);
}


clsLapseData::~clsLapseData(void)
{
	if (m_lapseData != NULL)
	{
		for (int i = 0; i < 12; ++i)
		{
			if (m_lapseData[i] != NULL)
				delete [] m_lapseData[i];
				
		}
		delete [] m_lapseData;
	}
}

void clsLapseData::getLapseData(int* nRows,float*** lapseData)
{
	*nRows = 12;
	*lapseData = m_lapseData;
}

void clsLapseData::show()
{
	char s[100];
	StatusMessage("LapseRate");
	StatusMessage("month\tP\tT\tPET");
	for(int i=0;i<12;i++)
	{
#ifndef linux
		sprintf_s(s,100,"%d\t%f\t%f\t%f\t",i+1,m_lapseData[i][0],m_lapseData[i][1],m_lapseData[i][2]);
#else
        snprintf(s,100,"%d\t%f\t%f\t%f\t",i+1,m_lapseData[i][0],m_lapseData[i][1],m_lapseData[i][2]);
#endif
		StatusMessage(s);
	}
}

void clsLapseData::readLapseData(string databasePath)
{
	m_lapseName = Table_LapseRate;

	DBManager dbman;
	string strSQL;
	slTable* tbl;

	if(m_lapseData == NULL)
	{
		m_lapseData = new float*[12];
		for(int i=0;i<12;i++)
		{
			m_lapseData[i] = new float[3];
			for(int j=0;j<3;j++) m_lapseData[i][j] = 0.0f;
		}
	}

	StatusMessage("reading lapse rate ...");

	try
	{
		// open the database
		dbman.Open(databasePath + File_HydroClimateDB);
		// if there is no error
		if (!dbman.IsError())
		{
			// craete the SQL statement for the query
			strSQL = "SELECT MONTH, LAPSE, TYPE FROM " + string(Table_LapseRate);
			// run the query
			tbl = dbman.Load(strSQL);
			// if the query was successful
			if (tbl != NULL)
			{
				// read in the data
				//m_lapseData = new float*[tbl->nRows];
				//m_nRows = tbl->nRows;
				for (int idx=1; idx<=tbl->nRows; idx++)
				{
					// (row number * 3 columns) + 3 columns for header
					//int pos = (idx * 3) + 3;
					//m_lapse[(int)atoi(tbl->pData[pos + 1])] = (float)atof(tbl->pData[pos + 2]);
					
					
					//m_lapseData[idx] = new float[3];
					float month = float(atof(tbl->FieldValue(idx,0).c_str()));
					float value = float(atof(tbl->FieldValue(idx,1).c_str()));
					
					string sType = tbl->FieldValue(idx,2);
					if(StringMatch(sType,"P")) m_lapseData[int(month)-1][0] = value;
					else if(StringMatch(sType,"T")) m_lapseData[int(month)-1][1] = value;
					else if(StringMatch(sType,"PET")) m_lapseData[int(month)-1][2] = value;
					else throw ModelException("clsLapseData","readLapseData","Unknown lapse rate type:" + sType + ".");
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
