#include <cstdio> 
#include <cstdlib>
#include <mpi.h>
#include <iostream>
#include <sstream> 
#include <fstream>
#include <ctime>
#include <set>

#include "ogrsf_frmts.h"

#include "ReadData.h"
#include "util.h"
#include "WetSpaException.h"

#include "mongo.h"
#include "bson.h"
#include "gridfs.h"

#include "parallel.h"

using namespace std;

int main(int argc, char *argv[]) {
	if (argc < 6)
	{
		cout << "Usage: parallel dbAddress dbPort dbName projectPath mudlePath [ThreadNumber] [LayeringMethod].\n";
		exit(-1);
	}

	OGRRegisterAll();

	const char* outputFile = "q.txt";

	const char* dbAddress = argv[1];
	int dbPort = atoi(argv[2]);
	const char* dbName = argv[3];
	string projectPath = argv[4];
	string modulePath = argv[5];
	
	int nThreads = 1;
	if (argc >= 7)
		nThreads = atoi(argv[6]);

	LayeringMethod layeringMethod = UP_DOWN;
	if (argc >= 8)
	{
		int lyering = atoi(argv[7]);// 0 for up-down layering; 1 for down-up layering
		if (lyering == 0)
			layeringMethod = UP_DOWN;
		else
			layeringMethod = DOWN_UP;
	}


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

	// connect to mongodb
	mongo conn[1];
	int status = mongo_connect(conn, dbAddress, dbPort); 

	if( MONGO_OK != status ) 
	{ 
		cout << "can not connect to mongodb.\n";
		exit(-1);
	}
		

	if(rank == MASTER_RANK)
	{
		// read river topology data 
		map<int, Subbasin*> subbasinMap;
		set<int> groupSet;
		//ReadRiverTopology(reachFile, subbasinMap, groupSet);
		ReadTopologyFromMongoDB(conn, dbName, subbasinMap, groupSet, decompostionPlan);

		try
		{
			MasterProcess(subbasinMap, groupSet, outputFile);
		}
		catch(WetSpaException e)
		{
			cout << e.toString() << endl;
		}
		
		mongo_destroy(conn);
			
		MPI_Finalize();
		return 0;
	}
	
	// create the calculation groups
	int nSlaves = GetGroupCount(conn, dbName, decompostionPlan);
	mongo_destroy(conn);

	int nNotSlave = numprocs-nSlaves;
	int *notSlaveRanks = new int[nNotSlave];
	

	for(int i = 0; i < nNotSlave-1; i++)
	{
		//cout << numprocs - i - 1 << endl;
		notSlaveRanks[i] =  numprocs - i - 1;
	}

	notSlaveRanks[nNotSlave-1] = 0;
	int error = MPI_Group_excl(MPI_GROUP_WORLD, nNotSlave, notSlaveRanks, &slaveGroup);
	error = MPI_Comm_create(MPI_COMM_WORLD, slaveGroup, &slaveComm);
	cout << error  << endl;

	delete notSlaveRanks;
	
	for(int i = 1; i < nNotSlave; i++)
	{
		if(rank == numprocs - i)
		{
			MPI_Finalize();
			exit(0);
		}
	}

	cout << "rank:" << rank << endl;
	try
	{
		CalculateProcess(rank, nSlaves, slaveComm, projectPath, modulePath, dbAddress, dbPort, dbName, nThreads, layeringMethod);
	}
	catch(WetSpaException e)
	{   
	    cout << e.toString() << endl;
	}

	MPI_Finalize();
	return 0;
}
