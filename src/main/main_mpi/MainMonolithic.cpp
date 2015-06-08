#include <cstdio> 
#include <cstdlib>
#include <mpi.h>
#include <iostream>
#include <sstream> 
#include <fstream>
#include <ctime>
#include <set>

#include "ogrsf_frmts.h"
//gdal
#include "gdal.h"
#include "gdal_priv.h"
#include "cpl_string.h"

#include "ReadData.h"
#include "util.h"
#include "WetSpaException.h"

#include "mongo.h"
#include "bson.h"
#include "gridfs.h"

#include "parallel.h"

using namespace std;

void WriteFileInOut(string& projectPath, string& startTime, string& endTime)
{
	string fileInName = projectPath + "file.in";
	string fileOutName = projectPath + "file.out";
	
	ofstream fin(fileInName.c_str());
	fin << "MODE|Daily\nINTERVAL|1\n";
	fin << "STARTTIME|" << startTime << " 00:00:00\n";
	fin << "ENDTIME|" << endTime << " 00:00:00";
	fin.close();
	
	ofstream fout(fileOutName.c_str());
	
	//fout << "OUTPUTID|SOER\nCOUNT | 1\nTYPE | SUM\n";
	//fout << "STARTTIME|" << startTime << " 00:00:00\n";
	//fout << "ENDTIME|" << endTime << " 00:00:00\n";
	//fout << "FILENAME|SOER\n\n";
	
	fout << "OUTPUTID|SURU\nCOUNT | 1\nTYPE | SUM\n";
	fout << "STARTTIME|" << startTime << " 00:00:00\n";
	fout << "ENDTIME|" << endTime << " 00:00:00\n";
	fout << "FILENAME|SURU\n\n";

	//fout << "OUTPUTID|SOET\nCOUNT | 1\nTYPE | SUM\n";
	//fout << "STARTTIME|" << startTime << " 00:00:00\n";
	//fout << "ENDTIME|" << endTime << " 00:00:00\n";
	//fout << "FILENAME|SOET\n\n";

	//fout << "OUTPUTID|INLO\nCOUNT | 1\nTYPE | SUM\n";
	//fout << "STARTTIME|" << startTime << " 00:00:00\n";
	//fout << "ENDTIME|" << endTime << " 00:00:00\n";
	//fout << "FILENAME|INLO\n";
	
	fout.close();
}

int main(int argc, char *argv[]) {
	//if (argc < 6)
	//{
	//	cout << "Usage: parallel dbAddress dbPort dbName projectPath mudlePath [ThreadNumber] [LayeringMethod].\n";
	//	exit(-1);
	//}

	OGRRegisterAll();
	GDALAllRegister();

	string outputFile = "q.txt";

	//~ const char* dbAddress = argv[1];
	//~ int dbPort = atoi(argv[2]);
	//~ const char* dbName = argv[3];
	//~ string projectPath = argv[4];
	//~ string modulePath = argv[5];
	
	//~ int nThreads = 1;
	//~ if (argc >= 7)
		//~ nThreads = atoi(argv[6]);

	//~ LayeringMethod layeringMethod = DOWN_UP;
	//~ if (argc >= 8)
	//~ {
		//~ int lyering = atoi(argv[7]);// 0 for up-down layering; 1 for down-up layering
		//~ if (lyering == 0)
			//~ layeringMethod = UP_DOWN;
		//~ else
			//~ layeringMethod = DOWN_UP;
	//~ }
	
	const char* dbAddress = "10.1.1.14";
	dbAddress = "192.168.6.55";
	int dbPort = 27017;
	string modulePath = "modules";
	int nThreads = 1;
	LayeringMethod layeringMethod = UP_DOWN;
	
	const char* dbName = argv[1];
	string projectPath = argv[1];
	int nLen = projectPath.length();
	if (projectPath.substr(nLen-1, 1) != SEP)
	{
		projectPath = projectPath + SEP;
	}
	nLen = modulePath.length();
	if (modulePath.substr(nLen-1, 1) != SEP)
	{
		modulePath = modulePath + SEP;
	}
	
	string curPath = "/cluster/hpgc/app/hpgc_watershed/storm_model/build/bin";
	curPath = "/data/storm_model/build/bin";
	projectPath = curPath + SEP + projectPath;
	modulePath = curPath + SEP + modulePath;

	//string aim = argv[2];
	//string startTime = argv[3];
	//string endTime = argv[4];
	
	//WriteFileInOut(projectPath, startTime, endTime);
	
	int numprocs, rank, namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	int decompostionPlan = numprocs - 1;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelen);

	MPI_Group MPI_GROUP_WORLD, slaveGroup;
	MPI_Comm slaveComm;
	MPI_Comm_group(MPI_COMM_WORLD, &MPI_GROUP_WORLD);
	// create the master, transfer and calculation groups
	static int notSlaveRanks[] = {0};
	MPI_Group_excl(MPI_GROUP_WORLD, 1, notSlaveRanks, &slaveGroup);
	MPI_Comm_create(MPI_COMM_WORLD, slaveGroup, &slaveComm);

	int nSlaves = numprocs - 1;

	if(rank == MASTER_RANK)
	{
		vector<string> outputRasterList;
		for(int i = 2; i < argc; i++)
		{
			if(i > 9)
				break;
			outputRasterList.push_back(argv[i]);
		}
	
		// connect to mongodb
		mongo conn[1];
		int status = mongo_connect(conn, dbAddress, dbPort); 

		if( MONGO_OK != status ) 
		{ 
			cout << "can not connect to mongodb.\n";
			exit(-1);
		}

		// read river topology data 
		map<int, Subbasin*> subbasinMap;
		set<int> groupSet;
		//ReadRiverTopology(reachFile, subbasinMap, groupSet);
		ReadTopologyFromMongoDB(conn, dbName, subbasinMap, groupSet, decompostionPlan);

		if(nSlaves != groupSet.size())
		{
			cout << "The number of slave processes is not consist with the input group number.\n";
			exit(-1);
		}

		try
		{
			MasterProcess(subbasinMap, groupSet, projectPath, outputFile, outputRasterList, conn);
		}
		catch(WetSpaException e)
		{
			cout << e.toString() << endl;
		}
		
		mongo_destroy(conn);

		MPI_Finalize();
		return 0;
	}

	try
	{
		CalculateProcessMonolithic(rank, numprocs, nSlaves, slaveComm, projectPath, modulePath, dbAddress, dbPort, dbName, nThreads, layeringMethod);
	}
	catch(WetSpaException e)
	{   
	    cout << e.toString() << endl;
	}

	MPI_Finalize();

	return 0;
}
