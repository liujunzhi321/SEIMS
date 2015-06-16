/*!
 * \file clsLapseData.h
 * \brief Define clsLapseData class
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

using namespace std;
/*!
 * \ingroup data
 * \class clsLapseData
 *
 * \brief 
 *
 * \deprecated Currently, this is no longer used.
 *
 */
class clsLapseData
{
public:
	clsLapseData(string);
	~clsLapseData(void);

	void getLapseData(int* nRows,float*** lapseData);
	void show(void);
private:
	string m_lapseName;
	float** m_lapseData;

private:
	void readLapseData(string);
};

