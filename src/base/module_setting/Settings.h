/*!
 * \file Settings.h
 * \brief Settings class
 *
 * Class to store the settings information from configuration files
 *
 * \author [your name]
 * \version 
 * \date June 2015
 *
 * 
 */
#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream> 
#include <algorithm> 
#include <iterator> 
#include "utils.h"

using namespace std;
/** \defgroup module_setting
 * \ingroup Base
 * \brief module_setting group
 *
 *
 *
 */
//! \typdef 2D vector array to store string
typedef vector<vector<string> > string2DArray;
/*!
 * \ingroup module_setting
 * \class Settings
 *
 * \brief 
 *
 *
 *
 */
class Settings
{
public:
	string2DArray m_Settings; ///< m_Settings to store setting tag and values

public:
	Settings(void);
	~Settings(void);

	virtual bool LoadSettingsFromFile(string filename);
	virtual void Dump(string);
	string Value(string tag);

protected:
	string m_settingFileName; ///< input setting file path
};

