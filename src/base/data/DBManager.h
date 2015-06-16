/*!
 * \file DBManager.h
 * \brief Database manager for Sqlite
 *
 *
 *
 * \author [your name]
 * \version 
 * \date June 2015
 *
 * 
 */
//! Classes to manage sqlite database access
#pragma once

#include <string>

#include "sqlite3.h"

using namespace std;
/*!
 * \ingroup data
 * \class slTable
 *
 * \brief Table class for sqlite database tables
 *
 * \deprecated Currently, this is no longer used.
 *
 */
class slTable
{
public:
	int nRows; ///< rows
	int nCols; ///< cols
	char** pData; ///< data

	slTable ();
	virtual ~slTable();

	string FieldValue(int row, int col);
};

/*!
 * \ingroup data
 * \class DBManager
 *
 * \brief sqlite database management class
 *
 * \deprecated Currently, this is no longer used.
 *
 */
class DBManager
{
private:
	sqlite3* m_Database;
	char* errMsg;

public:
	DBManager(void);
	~DBManager(void);

	bool Open(string filename);
	void Close(void);

	bool IsError();
	string GetErrorMessage();

	slTable* Load(string strSQL);
	bool Execute(string strSQL);
	
public:
	static bool IsTableExist(string databasePath,string tableName);
};

