/*!
 * \file SettingsOutput.cpp
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
#include "SettingsOutput.h"
#include "util.h"
#include "utils.h"
#include "text.h"
#include "ModelException.h"
//! Constructor
SettingsOutput::SettingsOutput(int subBasinID, string fileName, mongo* conn, gridfs* gfs):m_conn(conn), m_outputGfs(gfs)
{
	LoadSettingsFromFile(subBasinID, fileName);
}

//! Destructor
SettingsOutput::~SettingsOutput(void)
{
	StatusMessage("Start to release SettingsOutput ...");

	vector<PrintInfo*>::iterator it;
	for(it=m_printInfos.begin();it<m_printInfos.end();it++)
	{
		if((*it)!=NULL) delete (*it);
	}

	StatusMessage("End to release SettingsOutput ...");
}
//! Load settings from file
bool SettingsOutput::LoadSettingsFromFile(int subBasinID, string fileName)
{
	if(!Settings::LoadSettingsFromFile(fileName)) return false;
	if(!ParseOutputSettings(subBasinID)) return false;
	return true;
}
//! parse output settings for given subBasinID
bool SettingsOutput::ParseOutputSettings(int subBasinID)
{
	PrintInfo* pi = NULL;
	//PrintInfoItem* pii = NULL;
	ostringstream oss;
	oss << subBasinID << "_";
	string strSubbasinID = oss.str();

	for (size_t i=0; i<m_Settings.size(); i++)
	{
		// Sample output entries from the FILE.OUT file
		// placed here for reference
		//OUTPUTID  | PET_TS
		//INTERVAL  | 24 | HOURS
		//SITECOUNT | 2
		//SITENAME  | 720_pet
		//STARTTIME | 2000/01/01/00
		//ENDTIME   | 2000/12/31/00
		//FILENAME  | PET_720.txt
		//SITENAME  | 736_pet
		//STARTTIME | 2001/01/01/00
		//ENDTIME   | 2001/12/31/00
		//FILENAME  | 736_PET.txt

		//OUTPUTID  | D_PREC
		//TYPE      | SUM
		//COUNT     | 2
		//STARTTIME | 2000/01/01/00
		//ENDTIME   | 2000/01/31/00
		//FILENAME  | D_PREC_1.asc
		//STARTTIME | 2000/02/01/00
		//ENDTIME   | 2000/02/28/00
		//FILENAME  | D_PREC_2.asc

		// OUTPUTID starts a new PrintInfo 
		if (StringMatch(m_Settings[i][0], Tag_OutputID))
		{
			// is this the first PrintInfo or not
			if (pi != NULL)
			{
				// not the first PrintInfo so add this one to the list before starting a new one
				m_printInfos.push_back(pi);
			}
			// reset the pointer
			pi = NULL;
			// start a new PrintInfo
			pi = new PrintInfo();
			// set the OUTPUTID for the new PrintInfo
			pi->setOutputID(m_Settings[i][1]);

			// for QOUTLET or QTotal or SEDOUTLET or DissovePOutlet or AmmoniumOutlet or NitrateOutlet
			if (StringMatch(m_Settings[i][1], Tag_QOUTLET) || StringMatch(m_Settings[i][1], Tag_QTotal) || StringMatch(m_Settings[i][1], Tag_SEDOUTLET) 
				|| StringMatch(m_Settings[i][1], Tag_DisPOutlet) || StringMatch(m_Settings[i][1], Tag_AmmoOutlet) 
				|| StringMatch(m_Settings[i][1], Tag_NitrOutlet))
			{
				string starttm = "";
				string endtm = "";
				string fname = "";

				// check to see if we have all 4 values we need
				for (int flag=0; flag<4; flag++)
				{
					i++;
					if (StringMatch(m_Settings[i][0], Tag_Interval))
					{
						// set the interval length
						pi->setInterval(atoi(m_Settings[i][1].c_str()));
						// set the interval units
						pi->setIntervalUnits(m_Settings[i][2].c_str());
					}
					else if (StringMatch(m_Settings[i][0], Tag_StartTime))
					{
						// get the start time
						starttm = m_Settings[i][1];
					}
					else if (StringMatch(m_Settings[i][0], Tag_EndTime))
					{
						// get the end time
						endtm = m_Settings[i][1];
					}
					else if (StringMatch(m_Settings[i][0], Tag_FileName))
					{
						// get the filename, but not include the suffix.
						// modified by ZhuLJ, 2015/6/16
						fname = strSubbasinID + GetCoreFileName(m_Settings[i][1]);
					}
				}
				pi->AddPrintItem(starttm, endtm, fname);
			}
		}
		// INTERVAL is used only for the PET_TS output
		if (StringMatch(m_Settings[i][0], Tag_Interval))
		{
			// check that an object exists
			if (pi != NULL)
			{
				// set the interval length
				pi->setInterval(atoi(m_Settings[i][1].c_str()));
				// set the interval units
				pi->setIntervalUnits(m_Settings[i][2].c_str());
			}
		}
		// SITECOUNT is used only for the time series output for sites
		if (StringMatch(m_Settings[i][0], Tag_SiteCount))
		{
			string sitename = "";
			string starttm = "";
			string endtm = "";
			string fname = "";

			// get the number of sites in the list
			int cnt = atoi(m_Settings[i][1].c_str());
			// for each site int he list
			for (int idx=0; idx<cnt; idx++)
			{
				// reset values
				sitename = "";
				endtm = "";
				starttm = "";
				fname = "";

				// check to see if we have all 4 values we need
				for (int flag=0; flag<4; flag++)
				{
					i++;
					if (StringMatch(m_Settings[i][0], Tag_SiteID))
					{
						// get the sitename
						sitename = m_Settings[i][1];
					}
					else if (StringMatch(m_Settings[i][0], Tag_StartTime))
					{
						// get the start time
						starttm = m_Settings[i][1];
					}
					else if (StringMatch(m_Settings[i][0], Tag_EndTime))
					{
						// get the end time
						endtm = m_Settings[i][1];
					}
					else if (StringMatch(m_Settings[i][0], Tag_FileName))
					{
						// get the filename
						fname = strSubbasinID + GetCoreFileName(m_Settings[i][1]);
					}
				}

				// check to see if there is a site to save
				if (sitename.size() > 0)
				{
					// add the print item
					pi->AddPrintItem(starttm, endtm, fname, sitename,false);
				}

			}
		}
		// SUBBASINCOUNT is used only for the time series output for subbasins
		if (StringMatch(m_Settings[i][0], Tag_SubbasinCount) ||
			StringMatch(m_Settings[i][0], Tag_ReservoirCount))
		{
			string subbasinname = "";
			string starttm = "";
			string endtm = "";
			string fname = "";

			// get the number of sites in the list
			int cnt = atoi(m_Settings[i][1].c_str());
			// for each site int he list
			for (int idx=0; idx<cnt; idx++)
			{
				// reset values
				subbasinname = "";
				endtm = "";
				starttm = "";
				fname = "";

				// check to see if we have all 4 values we need
				for (int flag=0; flag<4; flag++)
				{
					i++;
					if (StringMatch(m_Settings[i][0], Tag_SubbasinId) ||
						StringMatch(m_Settings[i][0], Tag_ReservoirId))
					{
						// get the sitename
						subbasinname = m_Settings[i][1];
					}
					if (StringMatch(m_Settings[i][0], Tag_StartTime))
					{
						// get the start time
						starttm = m_Settings[i][1];
					}
					if (StringMatch(m_Settings[i][0], Tag_EndTime))
					{
						// get the end time
						endtm = m_Settings[i][1];
					}
					if (StringMatch(m_Settings[i][0], Tag_FileName))
					{
						// get the filename
						fname = strSubbasinID + GetCoreFileName(m_Settings[i][1]);
					}
				}

				// check to see if there is a site to save
				if (subbasinname.size() > 0)
				{
					// add the print item
					pi->AddPrintItem(starttm, endtm, fname, subbasinname,true);
				}
			}
		}
		if (StringMatch(m_Settings[i][0], Tag_Count))
		{
			string type = "";
			string starttm = "";
			string endtm = "";
			string fname = "";

			// get the number of sites in the list
			int cnt = atoi(m_Settings[i][1].c_str());
			// for each site int he list
			for (int idx=0; idx<cnt; idx++)
			{
				endtm = "";
				starttm = "";
				fname = "";

				// check to see if we have all 3 values we need
				for (int flag=0; flag<4; flag++)
				{
					i++;
					if (StringMatch(m_Settings[i][0], Tag_Type))
					{
						// get the type
						type = m_Settings[i][1];
					}
					if (StringMatch(m_Settings[i][0], Tag_StartTime))
					{
						// get the start time
						starttm = m_Settings[i][1];
					}
					if (StringMatch(m_Settings[i][0], Tag_EndTime))
					{
						// get the end time
						endtm = m_Settings[i][1];
					}
					if (StringMatch(m_Settings[i][0], Tag_FileName))
					{
						// get the filename
						fname = strSubbasinID + GetCoreFileName(m_Settings[i][1]);
					}
				}

				// check to see if there is a site to save
				if (starttm.size() > 0)
				{
					// add the print item					
					pi->AddPrintItem(type,starttm, endtm, fname, m_conn, m_outputGfs);
				}

			}
		}
	}

	if (pi != NULL)
	{
		// add the last one.
		m_printInfos.push_back(pi);
		pi = NULL;
	}

	return true;
}
//! check date
void SettingsOutput::checkDate(time_t startTime, time_t endTime)
{
	utils util;
	vector<PrintInfo*>::iterator it;
	for(it=m_printInfos.begin();it<m_printInfos.end();it++)
	{
		vector<PrintInfoItem*>::iterator itemIt;
		for(itemIt=(*it)->m_PrintItems.begin();itemIt<(*it)->m_PrintItems.end();itemIt++)
		{
			if((*itemIt)->getStartTime() < startTime)  
				throw ModelException("SettingsOutput","checkDate","The start time of output " + (*it)->getOutputID() + " to " + (*itemIt)->Filename + " is "+ (*itemIt)->StartTime +". It's earlier than start time of time series data "+util.ConvertToString(&startTime)+". Please check time setting of file.in and file.out.");
			if((*itemIt)->getEndTime() > endTime)  
				throw ModelException("SettingsOutput","checkDate","The end time of output " + (*it)->getOutputID() + " to " + (*itemIt)->Filename + " is "+ (*itemIt)->EndTime +". It's later than end time of time series data "+util.ConvertToString(&endTime)+". Please check time setting of file.in and file.out.");
		}
	}
}
//! output is ASCII file?
bool SettingsOutput::isOutputASCFile()
{
	vector<PrintInfo*>::iterator it;
	for(it=m_printInfos.begin();it<m_printInfos.end();it++)
	{
		if((*it)->m_OutputID.find_first_of("D") != string::npos) return true;
	}
	return false;
}
//! set specific cell raster output
void SettingsOutput::setSpecificCellRasterOutput(string projectPath,string databasePath,clsRasterData* templateRasterData)
{
	vector<PrintInfo*>::iterator itInfo;
	for(itInfo=m_printInfos.begin();itInfo<m_printInfos.end();itInfo++)
	{
		(*itInfo)->setSpecificCellRasterOutput(projectPath,databasePath,templateRasterData);
	}
}
//! dump
void SettingsOutput::Dump(string fileName)
{
	ofstream fs;
	utils util;
	fs.open(fileName.c_str(), ios::out);
	if (fs.is_open())
	{	
		for(size_t idx=0;idx<m_printInfos.size();idx++)
		{
			PrintInfo* info = m_printInfos.at(idx);

			fs << "Output ID: " << info->m_OutputID << endl;						

			fs << "---------- All the print info item----------" << endl;
			for(size_t idx2=0;idx2<info->m_PrintItems.size();idx2++)
			{
				PrintInfoItem* item = info ->m_PrintItems.at(idx2);
				fs << "Type: " << item->getAggregationType() << endl;
				fs << "Start Time:" <<item->StartTime << endl;
				fs << "End Time:" <<item->EndTime << endl;
				fs << "File Name:" <<item->Filename << endl;
			}	
			fs << "-------------------------------------------" << endl;
		}

		fs.close();
	}
}
