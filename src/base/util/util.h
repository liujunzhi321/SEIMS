/*!
 * \file util.h
 * \brief Definition of Utility functions
 *
 *
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date 29-July-2010
 *
 * 
 */

#ifndef SEIM_UTIL_INCLUDE
#define SEIM_UTIL_INCLUDE

#define NODATA -99.0f
#define SEIM_FLOAT float

#include <string>
#include <vector>
#include <stdio.h>

using namespace std;
//! grid layering method for parallel computing
enum LayeringMethod
{
	UP_DOWN, ///< the set of grid layers using the layering-from-source method
	DOWN_UP  ///< the set of grid layers using the layering-from-outlet method
};


extern bool DoubleEqual(double d1, double d2);
extern bool FloatEqual(float d1, float d2);
extern string GetPathFromFullName(string& fullFileName);
extern bool StrEqualIgnoreCase(const char*, const char*);
extern bool StringMatch(string text1, string text2);
extern string GetUpper(string);
extern void StatusMessage(const char* msg);
extern void Read2DArray(const char* filename, int& nRows, float**& data);
extern void Read1DArray(const char* filename, int& nRows, float*& data);
extern void Output1DArray(int n, float* data, const char* filename);
extern void Output2DArray(int nRows, int nCols, float** data, const char* filename);
extern double Max(double *a, int n);
extern double Sum(double *a, int n);
extern string& trim(string& s);
extern int FindFiles(const char *lpPath, const char *expression, vector<string>& vecFiles);
extern string GetCoreFileName(const string& fullFileName);
extern void Read2DArrayFromString(const char* s, int& nRows, float**& data);
string GetAppPath();
void LocalTime(time_t tValue, struct tm *tmStruct);
#ifndef linux
    #define strprintf sprintf_s
	#define StringTok strtok_s
	#ifndef max
        #define max(a,b)  (((a) > (b)) ? (a) : (b))
    #endif

    #ifndef min
        #define min(a,b)  (((a) < (b)) ? (a) : (b))
    #endif
#else
    #define strprintf snprintf
	#define StringTok strtok_r
    
    extern int copyfile_linux(const char* srcfile, const char* dstfile);
#endif


#endif
