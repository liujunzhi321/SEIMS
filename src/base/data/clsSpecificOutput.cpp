/*!
 * \file clsSpecificOutput.cpp
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
#include "clsSpecificOutput.h"
#include "DBManager.h"
#include "text.h"
#include "ModelException.h"
#include "utils.h"
#include <iomanip>
#include "util.h"
#include <fstream>
#include "clsRasterData.h"
#include <math.h>
//! if the outputID is SNAC, which means snowOutput
string clsSpecificOutput::TableNameFromOutputID(string outputID)
{
	utils util;
	vector<string> strs = util.SplitString(outputID,'_');
	string name = strs[strs.size()-1];
	if(StringMatch(name,"SNAC")) return "snowOutput";
	return "";
}
//! Constructor for Sqlite
//! \deprecated For now, this constructor is deprecated!
clsSpecificOutput::clsSpecificOutput(string projectPath,string databasePath,clsRasterData* templateRasterData,string outputID)
{
	m_outputID = outputID;
	string path = databasePath + File_HydroClimateDB;
	string tableName = TableNameFromOutputID(outputID);
	if(tableName.length() == 0)
		throw ModelException(	"clsSpecificOutput","clsSpecificOutput",
		"The output id "+ outputID + " can't output specific cells.");
	if(!DBManager::IsTableExist(path,tableName))
		throw ModelException(	"clsSpecificOutput","clsSpecificOutput",
								"The database " + path + 
								" dose not exist or the table "+tableName+" does not exist in this database.");

	if(templateRasterData == NULL)
		throw ModelException(	"clsSpecificOutput","clsSpecificOutput",
						"The templateRasterData is null.");
	
	m_templateRasterData = templateRasterData;

	utils util;
	DBManager db;
	DBManager dbman;
	try
	{
		// open the hydroclimate database
		dbman.Open(path);
		// if there is no error
		if (!dbman.IsError())
		{
			// create the query for the data table
			// Use the start date and end date to limit the time series data
			string strSQL = "SELECT TIME, Longitude, Latitude, ID, MEASURED FROM " + tableName + " order by TIME";
			// run the query
			slTable* tbl = dbman.Load(strSQL);
			// if the query is successful
			if (tbl != NULL)
			{			
				// read in the data
				for (int idx=1; idx<=tbl->nRows; idx++)
				{
					time_t time= util.ConvertToTime(tbl->FieldValue(idx,0), "%4d-%2d-%2d", false);
					float nrow = (float)(atof(tbl->FieldValue(idx,1).c_str()));
					float ncol = (float)(atof(tbl->FieldValue(idx,2).c_str()));

					int position = templateRasterData->getPosition(nrow,ncol);
					if(position > -1) 
					{
						m_times.push_back(time);
						m_positions.push_back(position);
						m_values.push_back(-99.0f);
						m_slope.push_back(-99.0f);
						m_curvature.push_back(-99.0f);
						m_landuse.push_back(-99.0f);
						m_ids.push_back(tbl->FieldValue(idx,3));

						//the measurement is added for convenient comparison
						m_measurement.push_back((float)(atof(tbl->FieldValue(idx,4).c_str())));
					}
				}

				setSlope(projectPath);
				setCurvature(projectPath);
				setLanduse(projectPath);

				delete tbl;
			}
			tbl = NULL;
			dbman.Close();
		}	
	}
	catch (...)
	{
		dbman.Close();
		throw;
	}
}

//! Destructor
clsSpecificOutput::~clsSpecificOutput(void)
{
}
//! write output to fileName
void clsSpecificOutput::dump(string fileName)
{
	
	ofstream fs;
	utils util;
	fs.open(fileName.c_str(), ios::out);
	if (fs.is_open())
	{
		fs << "Time"  
			<< "," << "ID" 
			<< "," << m_outputID
			<< ",Measurement"
			<< ",AbsoluteError" 
			<< ",RelativeError" 
			<< ",Slope" 
			<< ",Curvature" 
			<< ",Landuse" 
			<< endl;

		utils util;
		int size = (int)(m_positions.size());
		float ave1 = 0.0f;
		float ave2 = 0.0f;
		float total1 = 0.0f;
		float total2 = 0.0f;
		//float total3 = 0.0f;

		//to calculate the Coefficient of determination for Thijs
		//2011-5-2
		for(int i=0;i<size;i++)
		{
			ave1 += m_measurement.at(i);
			ave2 += m_values.at(i);
		}
		ave1 /= size;
		ave2 /= size;

		for(int i=0;i<size;i++)
		{
			fs << util.ConvertToString(&(m_times.at(i)))  
				<< "," << m_ids.at(i) 
				<< "," << setprecision(4) << m_values.at(i)
				<< "," << setprecision(4) << m_measurement.at(i)
				<< "," << setprecision(4) << m_values.at(i) - m_measurement.at(i)
				<< "," << setprecision(4) << (m_values.at(i) - m_measurement.at(i)) / m_measurement.at(i)
				<< "," << setprecision(4) << m_slope[i]
				<< "," << setprecision(4) << m_curvature[i]
				<< "," << setprecision(4) << m_landuse[i]
				<< endl;

			//to calculate the Coefficient of determination for Thijs
			//2011-5-2
			//total1 += (m_measurement.at(i) - ave1) * (m_values.at(i) - ave2);
			//total2 += pow(m_measurement.at(i) - ave1,2.0f);
			//total3 += pow(m_values.at(i) - ave2,2.0f);

			total1 += pow(m_values.at(i) - m_measurement.at(i),2.0f);
			total2 += pow(m_measurement.at(i) - ave1,2.0f);
		}
		fs << endl;

		float r = 1 - total1 / total2;
		fs << "NS," << r << ",RMSE," << sqrt(total1/size) << endl;
		fs.close();
	}
}
//! 
void clsSpecificOutput::dump()
{
	cout << "Time"  
		<< right << fixed << setw(15) << setfill(' ') << "ID" 
		<< right << fixed << setw(15) << setfill(' ') << m_outputID
		<< endl;

	utils util;
	int size = (int)(m_positions.size());
	for(int i=0;i<size;i++)
	{
		cout << util.ConvertToString(&(m_times.at(i)))  
			<< right << fixed << setw(15) << setfill(' ') << m_ids.at(i) 
			<< right << fixed << setw(15) << setfill(' ') << setprecision(4) << m_values.at(i)
			<< endl;
	}
}
//! set data
void clsSpecificOutput::setData(time_t time,float* data)
{
	int size = (int)(m_positions.size());
	for(int i=0;i<size;i++)
	{
		if(m_times.at(i) == time)
		{
			m_values[i] = data[m_positions[i]];
		}
	}
}
//! set slope data (slope_wind.asc)
void clsSpecificOutput::setSlope(string projectPath)
{
	clsRasterData* r = new clsRasterData(projectPath + "slope_wind.asc",m_templateRasterData);
	int rLength = -1;
	float* rData = NULL;
	r->getRasterData(&rLength,&rData);

	int size = (int)(m_positions.size());
	for(int i=0;i<size;i++)
	{
		m_slope[i] = rData[m_positions[i]];
	}

	delete r;
}
//! set curvature data (curva_wind.asc)
void clsSpecificOutput::setCurvature(string projectPath)
{
	clsRasterData* r = new clsRasterData(projectPath + "curva_wind.asc",m_templateRasterData);
	int rLength = -1;
	float* rData = NULL;
	r->getRasterData(&rLength,&rData);

	int size = (int)(m_positions.size());
	for(int i=0;i<size;i++)
	{
		m_curvature[i] = rData[m_positions[i]];
	}

	delete r;
}
//! set landuse data (landuse2.asc)
void clsSpecificOutput::setLanduse(string projectPath)
{
	clsRasterData* r = new clsRasterData(projectPath + "landuse2.asc",m_templateRasterData);
	int rLength = -1;
	float* rData = NULL;
	r->getRasterData(&rLength,&rData);

	int size = (int)(m_positions.size());
	for(int i=0;i<size;i++)
	{
		m_landuse[i] = rData[m_positions[i]];
	}

	delete r;
}
