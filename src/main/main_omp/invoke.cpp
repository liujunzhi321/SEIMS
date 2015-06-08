//! Implementation of the main program functions
#include "invoke.h"
#include "clsRasterData.h"
#include "clsWgnData.h"
#include "text.h"
#include "clsSimpleTxtData.h"
#include "clsSiteData.h"
#include "clsInterpolationWeighData.h"
#include "clsSpecificOutput.h"
#include "Scenario.h"
#include "SettingsInput.h"
#include "SettingsOutput.h"
#include "MongoUtil.h"
#include <sys/types.h>
#include <sys/stat.h>

#define BUFSIZE 255



// gets the root path to the current executable.
string _GetApplicationPath()
{
	string RootPath;

#ifndef linux
	TCHAR buffer[BUFSIZE];
	GetModuleFileName(NULL, buffer, BUFSIZE);
	RootPath = string((char*)buffer);
#else
#define PATH_MAX 1024
    static char buf[PATH_MAX];
    int rslt = readlink("/proc/self/exe", buf, PATH_MAX);
    if(rslt < 0 || rslt >= PATH_MAX)
        buf[0] = '\0';
    else
        buf[rslt] = '\0';
    RootPath = buf;
#endif
	basic_string <char>::size_type idx = RootPath.find_last_of(SEP);
	RootPath = RootPath.substr(0, idx+1);

	return RootPath;
}


void checkTable(set<string>& tableNameSet, string dbName, const char* tableName)
{
	ostringstream oss;
	oss << dbName << "." << tableName;
	if (tableNameSet.find(oss.str()) == tableNameSet.end())
	{
		cerr << "The " << tableName << " table does not exist in database.\n";
		exit(-1);
	}
}

void checkProject(string projectPath)
{
	utils util;
	string checkFilePath = projectPath + File_Config;
	if(!util.FileExists(checkFilePath)) throw ModelException("ModelMain","checkProject",checkFilePath + " does not exist or has not the read permission!");

	checkFilePath = projectPath + File_Input;
	if(!util.FileExists(checkFilePath)) throw ModelException("ModelMain","checkProject",checkFilePath + " does not exist or has not the read permission!");

	checkFilePath = projectPath + File_Output;
	if(!util.FileExists(checkFilePath)) throw ModelException("ModelMain","checkProject",checkFilePath + " does not exist or has not the read permission!");
}

void checkDatabase(mongo* conn, string dbName)
{
	set<string> tableNames;
	GetCollectionNames(conn, dbName, tableNames);
	checkTable(tableNames, dbName, PARAMETERS_TABLE);
#ifndef MULTIPLY_REACHES
	checkTable(tableNames, dbName, TOPOLOGY_TABLE);
#endif
	checkTable(tableNames, dbName, SPATIAL_TABLE);
	checkTable(tableNames, dbName, LANDUSE_TABLE);
	checkTable(tableNames, dbName, SOIL_TABLE);
	checkTable(tableNames, dbName, STATIONS_TABLE);	
}

void testMainSQLite(string moduleName,int scenarioID, int numThread, LayeringMethod layingMethod)
{
	try
	{
		string exePath = _GetApplicationPath();
		string projectPath = exePath + moduleName + SEP;
		string modulePath = exePath  + SEP;
		//checkProject(projectPath);
		//string dbName = "model_xiajiang";
		string dbName = moduleName;
		mongo conn[1];
		//const char* host = "127.0.0.1";
		//const char* host = "192.168.5.195";
		//int port = 27017;
		//int status = mongo_connect(conn, host, port); 
		//if( MONGO_OK != status ) 
		//{ 
		//	cout << "can not connect to mongodb.\n";
		//	exit(-1);
		//}
		//checkDatabase(conn, dbName);
		dbName = exePath + "database" + SEP;
		SettingsInput *input = new SettingsInput(projectPath + File_Input, conn, dbName, -1);

		dbName = exePath + "database" + SEP + "Parameter.db3";
		ModuleFactory *factory = new ModuleFactory(projectPath + File_Config, modulePath, conn, dbName);

		int nSubbasin = 1;
		int scenarioID = 0;
		ModelMain main(conn, projectPath, projectPath, input, factory, nSubbasin, scenarioID, numThread, layingMethod);
		main.Execute();	
		main.Output();

		delete input;
		delete factory;
	}
	catch(ModelException e)
	{
		cout << e.toString() << endl;
		//if(main != NULL) delete main;
	}
	catch(exception e)
	{
		cout << e.what() << endl;
		//if(main != NULL) delete main;
	}

}

void testMain(string modelPath,char *host,int port,int scenarioID, int numThread, LayeringMethod layingMethod)
{
	try
	{
		string exePath = _GetApplicationPath();
		string projectPath = modelPath + SEP;
		string modulePath = exePath + SEP;
		checkProject(projectPath);
		//string dbName = "model_xiajiang";
		size_t nameIdx = modelPath.rfind(SEP);
		string dbName = modelPath.substr(nameIdx+1);
		//string dbName = modelPath;
		mongo conn[1];
		//const char* host = "127.0.0.1";
		//const char* host ="192.168.6.55"; //"202.197.18.11";
		//int port = 27017;
		int status = mongo_connect(conn, host, port); 
		if( MONGO_OK != status ) 
		{ 
			cout << "can not connect to mongodb.\n";
			exit(-1);
		}
		//checkDatabase(conn, dbName);

		int nSubbasin = 1;
		int scenarioID = 0;
		SettingsInput *input = new SettingsInput(projectPath + File_Input, conn, dbName, nSubbasin);
		ModuleFactory *factory = new ModuleFactory(projectPath + File_Config, modulePath, conn, dbName);

		ModelMain main(conn, dbName, projectPath, input, factory, nSubbasin, scenarioID, numThread, layingMethod);
		main.Execute();	
		main.Output();

		//delete input;
		delete factory;
	}
	catch(ModelException e)
	{
		cout << e.toString() << endl;
		//if(main != NULL) delete main;
	}
	catch(exception e)
	{
		cout << e.what() << endl;
		//if(main != NULL) delete main;
	}

}


bool isIPAddress(const char *ip)
{
	const char *pChar;  
	bool rv = true;  
	int tmp1, tmp2, tmp3, tmp4, i;  

	while(1)  
	{  
		i = sscanf(ip, "%d.%d.%d.%d", &tmp1, &tmp2, &tmp3, &tmp4);  

		if( i != 4 )  
		{  
			rv = false;
			cout<<"IP Address format is not correct!"<<endl;
			break;  
		}  

		if( (tmp1 > 255) || (tmp2 > 255) || (tmp3 > 255) || (tmp4 > 255) || (tmp1 < 0) || (tmp2 < 0) || (tmp3 < 0) || (tmp4 < 0))  
		{  
			rv = false;  
			cout<<"IP Address format is not correct!"<<endl;
			break;  
		}  

		for( pChar = ip; *pChar != 0; pChar++ )  
		{  
			if( (*pChar != '.') && ((*pChar < '0') || (*pChar > '9')) )  
			{  
				rv = false;  
				cout<<"IP Address format is not correct!"<<endl;
				break;  
			}  
		}  
		break;  
	}  

	return rv;
}

bool isPathExists(const char *path)
{
	bool isExists;
#ifndef linux
	struct _stat fileStat;
	isExists = (_stat(path, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR);
#else
	struct stat fileStat;
	isExists = (stat(dir.c_str(), &fileStat) == 0) && S_ISDIR(fileStat.st_mode);
#endif
	return isExists;
}