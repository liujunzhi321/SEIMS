//! Class to store the settings information from configuration files
#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream> 
#include <algorithm> 
#include <iterator> 
#include "utils.h"

using namespace std;

typedef vector<vector<string> > string2DArray;

class Settings
{
public:
	string2DArray m_Settings;

public:
	Settings(void);
	~Settings(void);

	virtual bool LoadSettingsFromFile(string filename);
	virtual void Dump(string);
	string Value(string tag);

protected:
	string m_settingFileName;
};

