/*!
 * \file clsSimpleTxtData.h
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
#include <ostream>

using namespace std;
/*!
 * \ingroup data
 * \class clsSimpleTxtData
 *
 * \brief read string line from text file
 *
 *
 *
 */
class clsSimpleTxtData
{
public:
	clsSimpleTxtData(string fileName);
	~clsSimpleTxtData();

	void getData(int *nRow, float **data);
	void dump(ostream *fs);
private:
	int		m_row;
	float*	m_data;
};

