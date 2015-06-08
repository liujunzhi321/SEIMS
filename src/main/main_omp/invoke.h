//! Main header file for the application
#pragma once

#include <string>

#include "util.h"
#include "tinyxml.h"
//#include <io.h>
#include "ParamInfo.h"
#include "PrintInfo.h"
#include "DBManager.h"
#include "SiteInfo.h"
#include "Settings.h"
#include "SettingsInput.h"
#include "SettingsOutput.h"
#include "StringList.h"
#include "text.h"
#include "ModelMain.h"
#include "ModelException.h"
//#include "vld.h"

using namespace std;

string _GetApplicationPath();
void testSettingInput();
void testSettingOutput();
void testMain(string,char*,int,int,int,LayeringMethod);
void testMainSQLite(string,int,int,LayeringMethod);
void testRaster();
void testModule();
void testBMP();

void checkTable(set<string>& tableNameSet, string dbName, const char* tableName);
void checkProject(string projectPath);
void checkDatabase(mongo* conn, string dbName);

bool isIPAddress(const char *ip);
bool isPathExists(const char *path);
