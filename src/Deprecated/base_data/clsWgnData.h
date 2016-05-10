/*!
 * \file clsWgnData.h
 * \brief
 *
 *
 *
 * \author Junzhi Liu
 * \version 1.0
 * \date June 2010
 *
 * 
 */
#pragma once

#include <string>
#include <vector>

using namespace std;
///< enumerator 
enum WgnType
{
	WGN_PRECIPITATION, ///< rainfall
	WGN_SNOW           ///< snow
};
/*!
 * \ingroup data
 * \class clsWgnData
 *
 * \brief 
 *
 * \deprecated For now, this class is deprecated?
 *
 */
class clsWgnData
{
public:
	clsWgnData(string databasePath,WgnType type);
	~clsWgnData(void);
	void getWgnData(int* nRows, int* nCols,float*** wgnData);
	void show(void);
private:
	string m_wtgTableName; ///< table name
	vector<string> m_columns; ///< table column name
	float** m_wgnData; ///< wgn data

private:
	void readWgnData(string,WgnType type);
};

