/*!
 * \file SettingsOutput.h
 * \brief Setting Outputs for SEIMS
 *
 * \author Junzhi Liu, LiangJun Zhu
 * \version 1.1
 * \date June 2010
 */
#pragma once
#include "Settings.h"
#include "PrintInfo.h"
#include <vector>
#include "clsRasterData.h"
#include "mongoc.h"
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
	//! Constructor
	SettingsOutput(int, string, mongoc_client_t* conn=NULL, mongoc_gridfs_t* gfs=NULL);
	//! Destructor
	~SettingsOutput(void);
	//! Load output setting from file
	bool LoadSettingsFromFile(int, string);	
	//! Write output information to log file
	void Dump(string);
	//! Check date of output settings
	void checkDate(time_t,time_t);
	//! is output be an ASC file? Deprecated by LJ
	//bool isOutputASCFile(void);
	
	///void setSpecificCellRasterOutput(string projectPath,string databasePath,clsRasterData* templateRasterData);
public:
	//! All the print settings
	vector<PrintInfo*> m_printInfos;

private:
	//! MongoDB client
	mongoc_client_t* m_conn;
	//! Output GridFS
	mongoc_gridfs_t* m_outputGfs;
	//! Parse output settings for given subBasinID
	bool ParseOutputSettings(int);
};

