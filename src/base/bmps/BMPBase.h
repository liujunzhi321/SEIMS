#pragma once

#include <string>
#include <map>
#include <vector>
#include "BMPText.h"
#include "util.h"
#include "utils.h"
#include "ModelException.h"
#include <sstream>
#include <ostream>
#include <iomanip>

using namespace std;

namespace MainBMP
{
	/*
	** The the base class of all BMPs in BMP database.
	** One scenario may have different types of BMPs. And each type of BMP
	** maybe a single one or a collection of same kind of BMP.
	*/
	class BMPBase
	{
	public:
		~BMPBase(void);

	public:
		/*
		** @brief Static class factory for new bmp database
		**
		** @param string bmpDatabasePath	The path of the BMP database
		** @param int id					The BMP id, used to decide how to load distribution and parameter
		** @param string name				The BMP name, just used for exception information
		** @param int type					The BMP type id, used to decide how to load distribution and parameter
		** @param string distribution		The distribution of BMP, asc file or table name
		** @param string parameter			The parameter table name of BMP, table name
		*/
		BMPBase(string bmpDatabasePath,int id,string name, int type,string parameter);

		virtual void Dump(ostream* fs);

		static string ReachBMPColumnNameFromBMPID(int reachbmpId);
	protected:
		int m_bmpType;
		int m_bmpId;
		string m_bmpName;
		string m_bmpDatabasePath;
		string m_parameterTableName;
	};
}



