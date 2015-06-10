/*!
 * \file utils.cpp
 * \brief methods of utils class
 *
 * Implementation of the methods for the utils class
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date 29-July-2010
 *
 * 
 */

#include "utils.h"
#include <fstream>
#include <cstdio>
#include <ctime>
#ifndef linux
#include <io.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif

using namespace std;

#define UTIL_ZERO 0.0000000001 

//! Return a flag indicating if the given file exists
bool utils::FileExists( string FileName )
{
#ifndef linux
	struct _finddata_t fdt;
	intptr_t ptr = _findfirst(FileName.c_str(), &fdt);
	bool found = (ptr != -1);
	_findclose(ptr);
	return found;
#else
    if(access(FileName.c_str(), F_OK) == 0)
        return true;
    else
        return false;
#endif
	//fstream fs;
	//fs.open(FileName.c_str(),ios::in);
	//if(!fs)
	//{
	//	return false;
	//}
	//else
	//{
	//	fs.close();
	//	return true;
	//}
 //   FILE* fp = NULL;
	//errno_t err;

 //   //will not work if you do not have read permissions
 //   //to the file, but if you don't have read, it
 //   //may as well not exist to begin with.

	//err = fopen_s(&fp, FileName.c_str(), "rb");
 //   if( fp != NULL )
 //   {
 //       fclose( fp );
 //       return true;
 //   }

//    return false;
}

//! Constructor
utils::utils(void)
{
}

//! Destructor
utils::~utils(void)
{
}

//! Writes an entry to the log file. Normally only used for debugging
void utils::Log(string msg)
{
	//struct tm timeptr;
	//time_t now;
	//char buffer[32];

	//time(&now);
	//localtime_s(&timeptr, &now);
	//asctime_s(buffer, 32, &timeptr);
	//string timestamp = buffer;
	//timestamp = timestamp.substr(0, timestamp.length() -1);

	//ofstream fs = ofstream("SEIMSCore.log", ios::app);
	//if (fs != NULL)
	//{
	//	if (fs.is_open())
	//	{
	//		fs << timestamp;
	//		fs << ": ";
	//		fs << msg;
	//		fs << endl;
	//		fs.close();
	//	}
	//}
}

//! Trim Both leading and trailing spaces
void utils::TrimSpaces( string& str)
{
    size_t startpos = str.find_first_not_of(" \t"); // Find the first character position after excluding leading blank spaces
    size_t endpos = str.find_last_not_of(" \t"); // Find the first character position from reverse af
 
    // if all spaces or empty return an empty string
    if(( string::npos == startpos ) || ( string::npos == endpos))
    {
        str = "";
    }
    else
	{
        str = str.substr( startpos, endpos-startpos+1 );
	}
}

//! Splits the given string based on the given delimiter
vector<string> utils::SplitString(string item, char delimiter)
{
	istringstream iss(item); 
	vector<string> tokens; 
	
	std::string field;
	while (std::getline(iss, field, delimiter))
	{
		tokens.push_back(field);
	}

	return tokens;
}
//! Splits the given string by spaces
vector<string> utils::SplitString(string item)
{
	istringstream iss(item); 
	vector<string> tokens; 

	std::string field;
	iss >> field;
	while(!iss.eof())
	{
		tokens.push_back(field);
		iss >> field;
	}
	tokens.push_back(field);

	return tokens;
}
//! Convert date time to string as the format of "YYYY-mm-dd"
string utils::ConvertToString(const time_t *date)
{
	struct tm dateInfo;
#ifndef linux
	localtime_s(&dateInfo,date);
#else
    localtime_r(date, &dateInfo);
#endif
	if (dateInfo.tm_isdst > 0)
		dateInfo.tm_hour -= 1;

	char dateString[11];	
	strftime(dateString,11,"%Y-%m-%d",&dateInfo);
	
	return string(dateString);
}

static int daysOfMonth[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
#define LEAPYEAR(y) ((y % 4) == 0 && ((y % 100) != 0 || (y % 400) == 0))
//! Convert date time to string as the format of "YYYY-mm-dd hh"
string utils::ConvertToString2(const time_t *date)
{
    struct tm dateInfo;
#ifndef linux
    localtime_s(&dateInfo, date);
#else
    localtime_r(date, &dateInfo);
#endif
    if (dateInfo.tm_isdst > 0)
    {
        if (dateInfo.tm_hour != 0)
            dateInfo.tm_hour -= 1;
        else
        {
            dateInfo.tm_hour = 23;
            dateInfo.tm_mday -= 1;
            if (dateInfo.tm_mday == 0)
            {
                dateInfo.tm_mon -= 1;
                
                if (dateInfo.tm_mon == 0)
                {
                    dateInfo.tm_year -= 1;
                    dateInfo.tm_mon = 12;
                    dateInfo.tm_mday = 31;
                }
                else
                {
                    if (LEAPYEAR(dateInfo.tm_year))
                        dateInfo.tm_mday = daysOfMonth[dateInfo.tm_mon] + 1;
                    else
                        dateInfo.tm_mday = daysOfMonth[dateInfo.tm_mon];
                }
                    
            }
        }
    }
    char dateString[30];
    strftime(dateString, 30, "%Y-%m-%d %X", &dateInfo);

    string s(dateString);
    return s;
}


// assumes the strDate contains only the year month and day
// format string should indicate positions of year, month and day
// Ex: strDate => 20000323, format=> %4d%2d%2d
// Ex: strDate => 2000-03-23, format => %d-%d-%d
//! Convert string to date time, string format could be %4d%2d%2d or %d-%d-%d
time_t utils::ConvertToTime(string strDate, string format, bool includeHour)
{
	struct tm* timeinfo;
	time_t t;
	int yr;
	int mn;
	int dy;
	int hr = 0;

	try
	{
		if (includeHour)
		{
			sscanf(strDate.c_str(), format.c_str(), &yr, &mn, &dy, &hr);
		}
		else
		{
			sscanf(strDate.c_str(), format.c_str(), &yr, &mn, &dy);
		}

		timeinfo = new struct tm;
		timeinfo->tm_year = yr - 1900;
		timeinfo->tm_mon = mn - 1;
		timeinfo->tm_mday = dy;
		timeinfo->tm_hour = hr;
		timeinfo->tm_min = 0;
		timeinfo->tm_sec = 0;
		timeinfo->tm_isdst = false;
		t = mktime(timeinfo);
	}
	catch (...)
	{
		throw;
	}

	return t;
}
//! Convert string to date time, string format could be %4d%2d%2d or %d-%d-%d
time_t utils::ConvertToTime2(const string& strDate, const char* format, bool includeHour)
{
	time_t t;
	int yr;
	int mn;
	int dy;
	int hr = 0;
	int m = 0;
	int s = 0;

	try
	{
		if (includeHour)
		{
			sscanf(strDate.c_str(), format, &yr, &mn, &dy, &hr, &m, &s);
		}
		else
		{
			sscanf(strDate.c_str(), format, &yr, &mn, &dy);
		}

		struct tm timeinfo;
		timeinfo.tm_year = yr - 1900;
		timeinfo.tm_mon = mn - 1;
		timeinfo.tm_mday = dy;
		timeinfo.tm_hour = hr;
		timeinfo.tm_min = m;
		timeinfo.tm_sec = s;
		timeinfo.tm_isdst = false;
		t = mktime(&timeinfo);
		
	}
	catch (...)
	{
		cout << "Error in ConvertToTime2.\n";
		throw;
	}

	return t;
}
/*!
 * \brief Get date information from time_t variable
 *
 * 
 *
 * \param[in] t
 * \param[out] year, month, day
 */
int utils::GetDateInfoFromTimet(time_t* t, int* year, int* month, int* day)
{
	struct tm dateInfo;
#ifndef linux
	localtime_s(&dateInfo, t);
#else
	localtime_r(t, &dateInfo);
#endif
	if (dateInfo.tm_isdst > 0)
		dateInfo.tm_hour -= 1;

	char dateString[30];	
	strftime(dateString,30,"%Y-%m-%d %X",&dateInfo);
	int hour, min, sec;
	sscanf(dateString, "%4d-%2d-%2d %2d:%2d:%2d", year, month, day, &hour, &min, &sec);

	return 0;
}
