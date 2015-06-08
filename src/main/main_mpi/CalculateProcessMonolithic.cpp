#include "parallel.h"
#include "mpi.h"
#include "util.h"
#include <sstream>
#include <iostream>

#include "WetSpaMain.h"
#include "main.h"
#include "ModuleFactory.h"
//#include "SimulationModule.h"
#include "mongo.h"

void CalculateProcessMonolithic(int rank, int numprocs, int nSlaves, MPI_Comm slaveComm, 
	string& projectPath, string& modulePath, const char* host, int port , const char* dbName, int nThreads, LayeringMethod layeringMethod)
{
	MPI_Request request;
	MPI_Status status;

	// receive task information from master process
	int *pTaskAll = NULL;
	int *pRankAll = NULL;
	int *pDisAll = NULL;
	int *pDownStreamAll = NULL;
	int *pUpNumsAll = NULL;
	int *pUpStreamAll = NULL;
	int *pGroupId = NULL;
	int maxTaskLen;
	if(rank == SLAVE0_RANK)
	{
		int nTaskAll;
		MPI_Status status;
		MPI_Recv(&nTaskAll, 1, MPI_INT, MASTER_RANK, WORK_TAG, MPI_COMM_WORLD, &status); 
		pTaskAll = new int[nTaskAll];
		pRankAll = new int[nTaskAll];
		pDisAll = new int[nTaskAll];
		pDownStreamAll = new int[nTaskAll];
		pUpNumsAll = new int[nTaskAll];
		pUpStreamAll = new int[nTaskAll * MAX_UPSTREAM];
		pGroupId = new int[nSlaves];

		MPI_Recv(pGroupId, nSlaves, MPI_INT, MASTER_RANK, WORK_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(pTaskAll, nTaskAll, MPI_INT, MASTER_RANK, WORK_TAG, MPI_COMM_WORLD, &status); 
		MPI_Recv(pRankAll, nTaskAll, MPI_INT, MASTER_RANK, WORK_TAG, MPI_COMM_WORLD, &status); 
		MPI_Recv(pDisAll, nTaskAll, MPI_INT, MASTER_RANK, WORK_TAG, MPI_COMM_WORLD, &status); 
		MPI_Recv(pDownStreamAll, nTaskAll, MPI_INT, MASTER_RANK, WORK_TAG, MPI_COMM_WORLD, &status); 
		MPI_Recv(pUpNumsAll, nTaskAll, MPI_INT, MASTER_RANK, WORK_TAG, MPI_COMM_WORLD, &status); 
		MPI_Recv(pUpStreamAll, MAX_UPSTREAM*nTaskAll, MPI_INT, MASTER_RANK, WORK_TAG, MPI_COMM_WORLD, &status); 

		maxTaskLen = nTaskAll / nSlaves;
	}
	MPI_Bcast(&maxTaskLen, 1, MPI_INT, 0, slaveComm);

	int groupId;
	int *pTasks = new int[maxTaskLen];
	int *pRanks = new int[maxTaskLen];
	int *pDis = new int[maxTaskLen];
	int *pDownStream = new int[maxTaskLen];
	int *pUpNums = new int[maxTaskLen];
	int *pUpStream = new int[maxTaskLen*MAX_UPSTREAM];

	MPI_Scatter(pGroupId, 1, MPI_INT, &groupId, 1, MPI_INT, 0, slaveComm);
	MPI_Scatter(pTaskAll, maxTaskLen, MPI_INT, pTasks, maxTaskLen, MPI_INT, 0, slaveComm);
	MPI_Scatter(pRankAll, maxTaskLen, MPI_INT, pRanks, maxTaskLen, MPI_INT, 0, slaveComm);
	MPI_Scatter(pDisAll, maxTaskLen, MPI_INT, pDis, maxTaskLen, MPI_INT, 0, slaveComm);
	MPI_Scatter(pDownStreamAll, maxTaskLen, MPI_INT, pDownStream, maxTaskLen, MPI_INT, 0, slaveComm);
	MPI_Scatter(pUpNumsAll, maxTaskLen, MPI_INT, pUpNums, maxTaskLen, MPI_INT, 0, slaveComm);
	MPI_Scatter(pUpStreamAll, maxTaskLen*MAX_UPSTREAM, MPI_INT, pUpStream, maxTaskLen*MAX_UPSTREAM, MPI_INT, 0, slaveComm);

	int slaveRank;
	MPI_Comm_rank(slaveComm, &slaveRank);

	//ostringstream oss;
	//oss << "Subbasins of slave process " << slaveRank << ":  \n";
	//for (int i = 0; i < maxTaskLen; i++)
	//{
		//if(pTasks[i] < 0) continue;
		//oss << pTasks[i] << " ";// <<  pRanks[i] << " " << pDownStream[i] << " ups:";
		//for(int j = 0; j < pUpNums[i]; j++)
		//{
		//    oss << pUpStream[MAX_UPSTREAM*i + j] << " ";
		//}
		//oss << "\n";
	//}
	//oss << "\n";
	//cout << oss.str();
	//cout << "rank " << rank << " received tasks.\n";
	delete[] pTaskAll;
	delete[] pRankAll;
	delete[] pDisAll;
	delete[] pDownStreamAll;
	delete[] pUpNumsAll;
	delete[] pUpStreamAll;
	// end of assign tasks
	//////////////////////////////////////////////////////////////////////////////////////

	int nSubbasins = 0;
	for(int i = 0; i < maxTaskLen; ++i)
	{
		if(pTasks[i] > 0)
			nSubbasins++;
	}
	double t1, t2;
	double t;
	t1 = MPI_Wtime();

	// setup models for subbasins
	vector<WetSpaMain*> modelList;
	modelList.reserve(nSubbasins);
	
	////////////////////////////////////////////////////////////////////
	//cout << "rank " << rank << " check project.\n";
	checkProject(projectPath);
	//string dbName = "model_1";
	mongo conn[1];
	//const char* host = "127.0.0.1";
	//int port = 27017;

	int mongoStatus = mongo_connect(conn, host, port); 
	if( MONGO_OK != mongoStatus ) 
	{ 
		cout << "can not connect to mongodb.\n";
		exit(-1);
	}
	//checkDatabase(conn, string(dbName));
	
	ModuleFactory *factory = new ModuleFactory(projectPath + File_Config, modulePath, conn, string(dbName));
	
	for (int i = 0; i < nSubbasins; i++)
	{
		SettingsInput *input = new SettingsInput(projectPath + File_Input, conn, string(dbName), pTasks[i]);	
		WetSpaMain *p = new WetSpaMain(conn, string(dbName), projectPath, input, factory, pTasks[i], 0, nThreads, layeringMethod);
		modelList.push_back(p);
	}

	t2 = MPI_Wtime();
	t = t2 - t1;
	double *tReceive = new double[nSlaves];
	MPI_Gather(&t, 1, MPI_DOUBLE, tReceive, 1, MPI_DOUBLE, 0, slaveComm);
	double ioTime = 0;
	if (slaveRank == 0)
	{
		ioTime = Max(tReceive, nSlaves);
		//cout << "[DEBUG]\tTime of reading data -- Max:" << ioTime << "   Total:" << Sum(tReceive, nSlaves) << "\n";
		//cout << "[DEBUG][TIMESPAN][IO]" << ioTime << "\n";
	}

	t1 = MPI_Wtime();

	// classification according to the rank of subbasin
	vector<int> sourceBasins;
	set<int> downStreamSet, downStreamIdSet; // used to find if the downstream subbasin of a finished subbsin is in the same process,
	// if so, the MPI send operation is not necessary.
	// the set container is more efficient for the finding operation
	bool includeChannel = false;
	if(modelList[0]->IncludeChannelProcesses())
	{
		includeChannel = true;
		for (int i = 0; i < nSubbasins; i++)
		{
			if(pRanks[i] == 1)
				sourceBasins.push_back(i);
			else
			{
				downStreamSet.insert(i);//index of the array
				downStreamIdSet.insert(pTasks[i]);//id of subbasin
			}
		}
	}
	// if no channel processes are simulated	
	else
	{
		for (int i = 0; i < nSubbasins; i++)
			sourceBasins.push_back(i);
	}

	double tTask1, tTask2;
	double tSlope = 0.0;
	double tChannel = 0.0;;	
	float buf[MSG_LEN];
	// time loop
	WetSpaMain *p = modelList[0];
	int dt = p->getInterval();
	//cout << p->getStartTime() << "\t" << p->getEndTime() << "\t" << dt << endl;
	utils util;
	//cout << "Whether include channel: " << includeChannel << endl;
	for(time_t t = p->getStartTime(); t <= p->getEndTime();t += dt)
	{
		//if(slaveRank == 0) 
		//	cout << util.ConvertToString2(&t) << endl;
		map<int, float> qMap; //used to contain the flowout of each subbasin

		tTask1 = MPI_Wtime();
		// 1. do the jobs that does not depend on other subbasins
		// 1.1 the slope and channel routing of source subbasins without upstreams
		for (int j = 0; j < sourceBasins.size(); ++j)
		{
			int index = sourceBasins[j];
			//cout << rank << ":" << pTasks[sourceBasins[j]] << endl;
			modelList[index]->Step(t);

			if(includeChannel)
			{
				float qOutlet = modelList[index]->GetQOutlet();
			
				// if the downstream subbasin is in the same process, 
				// there is no need to transfer outflow to the master process
				if(downStreamIdSet.find(pDownStream[index]) != downStreamIdSet.end())
				{
					qMap[pTasks[index]] = qOutlet;
					continue;
				}
				//cout << rank << ": pass" << pTasks[sourceBasins[j]] << "\n";

				// transfer the result to the master process
				buf[0] = 1.f;  // message type
				buf[1] = pTasks[index];   // subbasin id
				buf[2] = qOutlet;// flow out of subbasin
				buf[3] = t;
				MPI_Isend(buf, MSG_LEN, MPI_FLOAT, MASTER_RANK, WORK_TAG, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
			}
		}
		//cout << "step 1.2\n";

		if(!includeChannel)
			continue;

		// 1.2 the slope routing of downstream subbasins
		//for (set<int>::iterator it = downStreamSet.begin(); it != downStreamSet.end(); ++it)
		//{
		//	int index = *it;
		//	modelList[index]->StepOverland(t);
		//}
		//tTask2 = MPI_Wtime();
		//tSlope = tSlope + tTask2 - tTask1;

		tTask1 = MPI_Wtime();
		//cout << "rank: " << rank << "  step 2\n";
		// 2. the channel routing of  downStream subbasins are calculated 
		// if their upstream subbasins are already calculated
		set<int> toDoSet, canDoSet;
		//cout << "test rank: " << rank << " >> ";
		for (set<int>::iterator it = downStreamSet.begin(); it != downStreamSet.end(); it++)
		{
			toDoSet.insert(*it);
			//cout << pTasks[*it] << " ";
		}
		//cout << endl;
		while(!toDoSet.empty())
		{
			// find all subbasins that the channel routing can be done without asking the master process
			for (set<int>::iterator it = toDoSet.begin(); it != toDoSet.end(); )
			{
				int index = *it;

				bool upFinished = true;
				//ostringstream oss;
				//oss << "rank: " << rank << " id: " << pTasks[index] << "  nUps:" << pUpNums[index] << " ups: ";
				for(int j = 0; j < pUpNums[index]; ++j)
				{
					int upId = pUpStream[index*MAX_UPSTREAM + j];
					//oss << upId << ", ";
					// if can not find upstreams, this subbasin can not be done
					if(qMap.find(upId) == qMap.end())
					{
						upFinished = false;
						break;
					}
				}
				//oss << endl;
				//cout << oss.str();

				if(upFinished)
				{
					canDoSet.insert(index);
					toDoSet.erase(it++);
				}
				else
					it++;
			}

#ifdef DEBUG_OUTPUT
			cout << "rank" << rank << "  todo set: ";
			for(set<int>::iterator it = toDoSet.begin(); it != toDoSet.end(); ++it)
				cout << pTasks[*it] << " ";
			cout << endl;
			//cout << "rank: " << rank << "  cando set: ";
			//for(set<int>::iterator it = canDoSet.begin(); it != canDoSet.end(); ++it)
			//	cout << pTasks[*it] << " ";
			//cout << endl;
#endif
			// if can not find subbasins to calculate according to local information,
			// ask the master process if there are new upstream subbasins calculated
			if(canDoSet.empty())
			{
				buf[0] = 2.f;
				buf[1] = groupId;
				buf[2] = rank;
				
				MPI_Isend(buf, MSG_LEN, MPI_FLOAT, MASTER_RANK, WORK_TAG, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				int msgLen;
				MPI_Irecv(&msgLen, 1, MPI_INT, MASTER_RANK, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				

				float *pData = new float[msgLen];
				MPI_Irecv(pData, msgLen, MPI_FLOAT, MASTER_RANK, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				
				//cout << "recv rank" << rank << "  num:" << msgLen/2 << " data:";
				for(int j = 0; j < msgLen; j+=2)
				{
					//cout << pData[j] << " ";
					qMap[ (int)pData[j] ] = pData[j+1];
				}
				//cout << endl;
				//cout << "rank" << rank << " qMap: ";
				//for (map<int,float>::iterator it = qMap.begin(); it != qMap.end(); it++)
				//{
				//	cout << it->first << " ";
				//}
				//cout << endl;

				delete pData;
			}
			else
			{
				//cout << "rank:" << rank << endl;
				// sort according to the distance to outlet descendent 
				vector<int> vec; 
				set<int>::iterator it, itMax;
				while(!canDoSet.empty())
				{
					itMax = canDoSet.begin();
					for(it = canDoSet.begin(); it != canDoSet.end(); ++it)
					{
						if(pDis[*it] > pDis[*itMax])
							itMax = it;
					}
					vec.push_back(*itMax);
					canDoSet.erase(itMax);
				}

				for (vector<int>::iterator it = vec.begin(); it != vec.end(); ++it)
				{
					int index = *it;
					//cout << "index:" << index << endl;
					float overFlowIn = 0.f;
					for(int j = 0; j < pUpNums[index]; ++j)
					{
						int upId = pUpStream[index*MAX_UPSTREAM + j];
						overFlowIn += qMap[upId];
					}
					modelList[index]->SetChannelFlowIn(overFlowIn);
					//modelList[index]->StepChannel(t);
					modelList[index]->Step(t);
					
					float qOutlet = modelList[index]->GetQOutlet();

					//if(slaveRank == 7) cout << "rank: " << slaveRank << " id: " << pTasks[index] << "  downStream:" << pDownStream[index] << endl;  
					if(downStreamIdSet.find(pDownStream[index]) != downStreamIdSet.end())
					{
						qMap[pTasks[index]] = qOutlet;
						continue;
					}
					// transfer the result to the master process
					buf[0] = 1.f;  // message type
					buf[1] = pTasks[index];   // subbasin id
					buf[2] = qOutlet;// flow out of subbasin
					buf[3] = t;
					MPI_Isend(buf, MSG_LEN, MPI_FLOAT, MASTER_RANK, WORK_TAG, MPI_COMM_WORLD, &request);
					MPI_Wait(&request, &status);
				}

				//cout << "rank" << rank << " qMap after execute: ";
				//for(map<int, float>::iterator it = qMap.begin(); it != qMap.end(); ++it)
				//	cout << it->first << " ";
				//cout << endl;
			}
		}
		tTask2 = MPI_Wtime();
		tChannel = tChannel + tTask2 - tTask1;

		MPI_Barrier(slaveComm);
		if(slaveRank == 0)
		{
			buf[0] = 0.f;
			MPI_Isend(buf, MSG_LEN, MPI_FLOAT, MASTER_RANK, WORK_TAG, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);
		}
		MPI_Barrier(slaveComm);
	}


	//delete qs;
	//qFile.close();
	t2 = MPI_Wtime();
	t = t2 - t1;
	MPI_Gather(&t, 1, MPI_DOUBLE, tReceive, 1, MPI_DOUBLE, 0, slaveComm);
	if (slaveRank == 0)
	{
		double computingTime = Max(tReceive, nSlaves);
		//cout << "[DEBUG]\tnprocs: " << numprocs-1 << " Max: " << computingTime << "   Total: " << Sum(tReceive, nSlaves) << "\n";
		cout << "[DEBUG][TIMESPAN][COMPUTING] " << computingTime << "\n";
		//cout << "[DEBUG][TIMESPAN][TOTAL]" << computingTime + ioTime << "\n";
	}

	for (size_t i = 0; i < nSubbasins; i++)
	{
		modelList[i]->Output();
		modelList[i]->CloseGridFS();
	}

	//cout << "Rank: " << slaveRank << "\ttime:\t" << tSlope << "\t" << tChannel << endl;
	MPI_Barrier(slaveComm);
	// tell the master process to exit
	if(slaveRank == 0)
	{
		buf[0] = 9.f;
		MPI_Isend(buf, MSG_LEN, MPI_FLOAT, MASTER_RANK, WORK_TAG, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);

	}

	// clean up
	for (int i = 0; i < nSubbasins; i++)
	{
		if (modelList[i] != NULL)
			delete modelList[i];
	}

	delete[] pTasks;
	delete[] pRanks;
	delete[] pDis;
	delete[] pDownStream;
	delete[] pUpNums;
	delete[] pUpStream;

	delete[] tReceive;

	//delete input;
	delete factory;
}
