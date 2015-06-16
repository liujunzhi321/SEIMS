/*!
 * \file clsSpecificOutput.h
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
#include "clsRasterData.h"
#include <vector>

using namespace std;
/*!
 * \ingroup data
 * \class clsSpecificOutput
 *
 * \brief 
 *
 * 
 *
 */
class clsSpecificOutput
{
public:
	clsSpecificOutput(string projectPath,string databasePath,clsRasterData* templateRasterData,string outputID);
	~clsSpecificOutput(void);

	void setData(time_t time,float* data);

	void dump();
	void dump(string fileName);

	static string TableNameFromOutputID(string outputID);
private:
	vector<time_t> m_times;
	vector<string> m_ids;
	vector<int>    m_positions;
	vector<float>  m_values;
	vector<float>  m_measurement;

	vector<float>  m_slope;
	vector<float>  m_curvature;
	vector<float>  m_landuse;

	string m_outputID;
	clsRasterData* m_templateRasterData;

	void setSlope(string projectPath);
	void setCurvature(string projectPath);
	void setLanduse(string projectPath);
};

