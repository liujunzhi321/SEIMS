/*!
 * \file DBManager.cpp
 * \brief Database manager for Sqlite
 *
 * Implementation of the methods for the DBManager class
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date June 2010
 * 
 * 
 */

#include "DBManager.h"
#include "ModelException.h"
#include "utils.h"

//! Constructor
DBManager::DBManager(void)
{
	m_Database = NULL;
	errMsg = NULL;
}

//! Destructor
DBManager::~DBManager(void)
{
	if (errMsg != NULL)
	{
		sqlite3_free(errMsg);
	}
	errMsg = NULL;
	Close();
}

//! Open the given sqlite database
bool DBManager::Open(string filename)
{
	bool bStatus = false;

	bStatus = (sqlite3_open(filename.c_str(), &m_Database) == 0);

	return bStatus;
}

//! Close the currently open database
void DBManager::Close()
{
	if (m_Database != NULL)
	{
		sqlite3_close(m_Database);
	}
	m_Database = NULL;
}

//! Returns a flag indicating if an error occured
bool DBManager::IsError(void)
{
	if (errMsg != NULL)
		return true;
	else
		return false;
}

//! Returns the error message for the last error if any
string DBManager::GetErrorMessage()
{
	string res = "";

	if (errMsg != NULL)
	{
		res = errMsg;
	}

	return res;
}

//! Run the given SQL query and return an instance of a slTable class containing the results
slTable* DBManager::Load(string strSQL)
{
	slTable* tbl;

	try
	{
		tbl = new slTable();
		if (tbl != NULL)
		{
			sqlite3_get_table(m_Database, strSQL.c_str(), &tbl->pData, &tbl->nRows, &tbl->nCols, &errMsg);
		}
	}
	catch (...)
	{		
		delete tbl;
		tbl = NULL;
		throw;
	}

	return tbl;
}

//! Execute a non-query SQL string
bool DBManager::Execute(string strSQL)
{
	bool bStatus = false;

	try
	{
		sqlite3_exec(m_Database, strSQL.c_str(), NULL, NULL, &errMsg);
		bStatus = true;
	}
	catch(...)
	{
		bStatus = false;
		throw;
	}

	return bStatus;
}

//! Constructor
slTable::slTable()
{
	nCols = 0;
	nRows = 0;
	pData = NULL;
}
//! Destructor
slTable::~slTable()
{
	sqlite3_free_table(pData);
	nCols = 0;
	nRows = 0;
	pData = NULL;
}

//! Return the value in the field defined by the given row and column
string slTable::FieldValue(int row, int col)
{
	string res = "";
	int index;

	if (pData != NULL)
	{
		if (row <= nRows && col < nCols)
		{
			index = (row * nCols) + col;
			if (pData[index] != NULL)
			{
				res = pData[index];
			}
		}
	}

	return res;
}

//! if the given table exists
bool DBManager::IsTableExist(string databasePath,string tableName)
{
	utils util;
	if(!(util.FileExists(databasePath))) return false;

	DBManager dbman;
	bool exist = false;

	dbman.Open(databasePath);
	if (!dbman.IsError())
	{
		string strSQL = "pragma table_info("+tableName+")";
		//string strSQL = "SELECT * FROM sqlite_master WHERE type = 'table' and name = '"+tableName+"'";
		slTable* tbl = dbman.Load(strSQL);
		if(tbl->nRows > 0) exist = true;
		delete tbl;
		tbl = NULL;
	}		
	dbman.Close();

	return exist;
}
