/*!
 * \defgroup Base
 * Base classes and functions of SEIMS 
 */
/** \defgroup Util
 * \ingroup Base
 * \brief include classes of MetadataInfo, ModelException and SimulationModule et al.
 */
/*!
 * \file utils.h
 * \brief definition of utils class
 *
 * utils class to handle string, date time and file
 *
 * \author Junzhi Liu
 * \version 1.0	
 * \date 29-July-2010
 *
 * 
 */

#pragma once

#include <vector>
#include <string>
#include <sstream> 
#include <algorithm> 
#include <iterator> 
#include <iostream>
#include <time.h>

using namespace std;
/*!
 * \class utils
 * \ingroup Util
 * \brief utils class to handle string, date time and file
 *
 *
 *
 */
class utils
{

public:
	utils(void);
	~utils(void);

	static void TrimSpaces( string& str);
	static vector<string> SplitString(string item, char delimiter);
	static vector<string> SplitString(string item);
	static time_t ConvertToTime(string strDate, string format, bool includeHour);
	static time_t ConvertToTime2(const string& strDate, const char* format, bool includeHour);
	static bool FileExists( string FileName );

	static void Log(string msg);

	static string ConvertToString(const time_t *);
	static string ConvertToString2(const time_t *date);

	static int GetDateInfoFromTimet(time_t* seconds, int* year, int* month, int* day);
};
