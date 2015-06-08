//! Classes to manage sqlite database access
#pragma once

#include <string>

#include "sqlite3.h"

using namespace std;

//! Table class for sqlite database tables
class slTable
{
public:
	int nRows;
	int nCols;
	char** pData;

	slTable ();
	virtual ~slTable();

	string FieldValue(int row, int col);
};


//! sqlite database management class
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

