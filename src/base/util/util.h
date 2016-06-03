/*!
 * \file util.h
 * \ingroup util
 * \brief Utility functions to handle numeric, string and file
 * \author Junzhi Liu, LiangJun Zhu
 * \version 1.1
 * \date Jul. 2010
 *
 * 
 */
#ifndef SEIMS_UTIL_INCLUDE
#define SEIMS_UTIL_INCLUDE
#pragma once
#include <string>
#include <vector>
#include <stdio.h>
#include "text.h"
#include <cmath>

using namespace std;
/**
 * \def NODATA_VALUE
 * \brief NODATA value
 */
#define NODATA_VALUE	-9999.0f
#define NODATA					-99.0f

/**
 * \def ZERO
 * \brief zero value used in numeric calculation
 */
#define UTIL_ZERO				1.0e-6
/**
 * \def PI
 * \brief PI value used in numeric calculation
 */
#define PI								3.14159265358979323846
/**
 * \def MINI_SLOPE
 * \brief Minimum slope gradient
 */
#define MINI_SLOPE			0.0001

/*!
 * \ingroup util
 * \enum LayeringMethod
 * \brief Grid layering method for parallel computing
 */
enum LayeringMethod
{
	/// layering-from-source method
	UP_DOWN, 
	/// layering-from-outlet method
	DOWN_UP
};
/*!
 * \brief Whether d1 is equal to d2 or not
 *
 * \param[in] d1, d2 \a double
 * \return true or false
 * \sa ZERO
 */
extern bool DoubleEqual(double d1, double d2);
/// Check the argument against upper and lower boundary values prior to taking the Exponential
extern float Expo(float xx);
/*!
 * \brief Find files in given paths (Windows version)
 * \param[in] lpPath, expression
 * \param[out] vecFiles
 * \return 0 means success
 */
extern int FindFiles(const char *lpPath, const char *expression, vector<string>& vecFiles);
/*!
 * \brief Whether f1 is equal to f2
 *
 * \param[in]  f1, f2 \a float
 * \return true or false
 */
extern bool FloatEqual(float d1, float d2);
/*!
 * \brief Get the root path of the current executable file
 * \return \a string root path
 */
string GetAppPath();
/*!
 * \brief Return the file name from a given file's path
 *
 * \param[in] fullFileName 
 * \return CoreFileName
 * \sa GetPathFromFullName
 */
extern string GetCoreFileName(const string& fullFileName);
/*!
 * \brief Return the suffix of a given file's path
 *
 * \param[in] fullFileName 
 * \return Suffix
 * \sa GetPathFromFullName
 */
extern string GetSuffix(const string& fullFileName);
/*!
 * \brief Get Path From full file path string
 *
 * \param[in] fullFileName \a string
 * \return filePath string
 * \sa GetCoreFileName
 */
extern string GetPathFromFullName(string& fullFileName);
/*!
 * \brief Get Uppercase of given string
 *
 * \param[in] string
 * \return Uppercase string
 */
extern string GetUpper(string);
/*!
 * \brief Get local time
 *
 * \param[in] tValue \a time_t date
 * \param[out] tmStruct \a tm struct date
 */
void LocalTime(time_t tValue, struct tm *tmStruct);
/*!
 * \brief Max value of a double array
 *
 * Get maximum value in a double array with size n.
 *
 * \param[in] a, n
 * \return max value
 */
extern double Max(double *a, int n);
/*!
 * \brief Write 1D array to a file
 *
 * \sa Read1DArrayFromTxtFile(), Read2DArrayFromTxtFile(), Output2DArrayToTxtFile()
 *
 * \param[in] n, data, filename
 */
extern void Output1DArrayToTxtFile(int n, float* data, const char* filename);
/*!
 * \brief Write 2D array to a file
 *
 * \sa Read1DArrayFromTxtFile(), Read2DArrayFromTxtFile(), Output1DArrayToTxtFile()
 *
 * \param[in] nRows, nCols, data, filename
 */
extern void Output2DArrayToTxtFile(int nRows, int nCols, float** data, const char* filename);
/// deal with positive and negative float numbers
float Power(float a, float n);
/*!
 * \brief Read 1D array from file
 *
 * The input file should follow the format:
 *     a 1D array sized nRows * 1
 * The size of data is nRows
 * \sa Read2DArrayFromTxtFile(), Output1DArrayToTxtFile(), Output2DArrayToTxtFile()
 * \param[in] filename, nRows
 * \param[out] data
 */
extern void Read1DArrayFromTxtFile(const char* filename, int& nRows, float*& data);
/*!
 * \brief Read 2D array from file
 *
 * The input file should follow the format:
 *     a 2D array sized nRows * nRows
 * The size of data is nRows * (nRows + 1), the first element of each row is the nRows
 * \sa Read1DArrayFromTxtFile(), Output1DArrayToTxtFile(), Output2DArrayToTxtFile()
 * \param[in] filename, nRows
 * \param[out] data
 */
extern void Read2DArrayFromTxtFile(const char* filename, int& nRows, float**& data);
/*!
 * \brief Read 2D array from string
 *
 * The input string should follow the format:
 *     float value, total number is nRows * nRows
 * The size of data is nRows * (nRows + 1), the first element of each row is the nRows.
 *
 * \param[in] s, nRows
 * \param[out] data
 */
extern void Read2DArrayFromString(const char* s, int& nRows, float**& data);

/*!
 * \brief Print status messages for Debug
 */
extern void StatusMessage(const char* msg);
/*!
 * \brief Match \a char ignore cases
 *
 * \param[in] a, b \a char* 
 * \return true or false
 * \sa StringMatch()
 */
extern bool StrEqualIgnoreCase(const char*, const char*);
/*!
 * \brief Match Strings in UPPERCASE manner
 *
 * \param[in] text1, text2
 * \return true or false
 */
extern bool StringMatch(string text1, string text2);
/*!
 * \brief Sum of a double array
 *
 * Get sum value of a double array with size n.
 *
 * \param[in] a,n
 * \return sum
 * \sa StrEqualIgnoreCase()
 */
extern double Sum(double *a, int n);
/*!
 * \brief Trim given string's heading and tailing by "<space>,\n,\t,\r"
 *
 * \param[in] s \a string information
 * \return Trimmed string
 */
extern string& trim(string& s);
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
	/*!
	 * \brief Copy file in linux
	 *
	 * \param[in] srcfile \a char source file path
	 * \param[in] dstfile \a char destination file path
	 */
    extern int copyfile_linux(const char* srcfile, const char* dstfile);
#endif
#endif
/*!
* \brief Convert value to string
*
* \param[in] val value, e.g., a int, or float
* \return converted string
*/
template<typename T>
string ValueToString(T &val)
{
	ostringstream oss;
	oss << val;
	return oss.str();
}