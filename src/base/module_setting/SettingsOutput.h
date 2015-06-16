/*!
 * \file SettingsOutput.h
 * \brief
 *
 *
 *
 * \author [your name]
 * \version 
 * \date June 2015
 *
 * 
 */
#pragma once
#include "Settings.h"
#include "PrintInfo.h"
#include <vector>
#include "clsRasterData.h"

#include "mongo.h"
#include "gridfs.h"
/*!
 * \ingroup module_setting
 * \class SettingsOutput
 *
 * \brief 
 *
 *
 *
 */
class SettingsOutput :
	public Settings
{
public:
	SettingsOutput(int, string, mongo* conn=NULL, gridfs* gfs=NULL);
	~SettingsOutput(void);

	bool LoadSettingsFromFile(int, string);	
	void Dump(string);
	void checkDate(time_t,time_t);
	bool isOutputASCFile(void);

	void setSpecificCellRasterOutput(string projectPath,string databasePath,clsRasterData* templateRasterData);
public:
	///All the print settings
	vector<PrintInfo*> m_printInfos;

private:
	mongo* m_conn;
	gridfs* m_outputGfs;
	///Get all the print settings
	///It comes from Alex's source code of function ParseOutputSettings in main.cpp
	bool ParseOutputSettings(int);
};

